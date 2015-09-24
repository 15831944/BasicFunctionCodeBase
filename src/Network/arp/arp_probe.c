#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/types.h>
#include <asm/types.h>
#include <sys/time.h>
#include <netpacket/packet.h>
#include <net/ethernet.h>     
#include <netinet/if_ether.h>
#include "arp_probe.h"
#if 1//def ERRORCODE
#include "sys_msg.h"
#include "config.h"
#endif

#define    IP_ALEN              4     /* the length of ip */
#define    PROBE_WAIT           1     /* initial random delay */
#define    PROBE_NUM            3     /* number of probe packets */
#define    PROBE_MIN            1     /* minimum delay until repeated probe */
#define    PROBE_MAX            2     /* maximum delay until repeated probe */
#define    ANNOUNCE_WAIT        2     /* delay before announcing */
#define    ANNOUNCE_NUM         2     /* number of announcement packets */
#define    ANNOUNCE_INTERVAL    2     /* time between announcement packets */
#define    MAX_CONFLICTS        10    /* max conflicts before rate-limiting */
#define    RATE_LIMIT_INTERVAL  60    /* delay between successive attempts */
#define    DEFEND_INTERVAL      10    /* minimum interval between defensive ARPs */
#define    ARP_PROBE_MSG        "/var/arp_probe_msg"
#define    MAC_BCAST_ADDR       (uint8_t *)"\xff\xff\xff\xff\xff\xff"

static int conflict_num  = 0;
static int exit_flag  = E_RESERVED;
static struct reportdata repinfo;

struct arpmsg {
	struct  ether_header eh;
    struct  ether_arp ea;
	uint8_t padding[18];
}__attribute((aligned(1)));

static struct localsocket {
	int lsockfd;
	socklen_t len;
	struct sockaddr_un addr;
}unixsock;

typedef struct threaddata {
	int mode_flag;
	probe_handle  repfunc;
}thdparam;

typedef struct ctldata {
	int standby_flag;
	uint8_t ip[IP_ALEN];
	uint8_t mac[ETH_ALEN];
	char netname[16];
}ctlmsg;

static long get_random(int min, int max)
{
	return (((max - min)*(float)rand()/RAND_MAX + min)*1000000);
}

static int get_mac(const char *net_name, uint8_t *mac, int mac_len)
{
	if(NULL == net_name || NULL == mac) {
		fprintf(stderr, "Error: receive parameter is invalid!\n");
		return -1;
	}

	int reqfd = -1;
    struct ifreq macreq;

    if((reqfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("Socket error");
		return -1;
	}

	strcpy(macreq.ifr_name, net_name);

    /* get MAC address*/
	if(ioctl(reqfd, SIOCGIFHWADDR, &macreq) != 0) {
		perror("Ioctl error");
		close(reqfd);
		return -1;
	}

    memcpy(mac, macreq.ifr_hwaddr.sa_data, mac_len);
	close(reqfd);
	return 0;
}

static int get_timeout(long interval, struct timeval *timestart, struct timeval *tm)
{
	if(NULL == timestart || NULL == tm) { 
		fprintf(stderr, "Error: receive parameter is invalid!\n");
		return -1;
	}

	struct timeval timeend;

	gettimeofday(&timeend, NULL);

	tm->tv_sec = timestart->tv_sec + interval/1000000 - timeend.tv_sec;
	tm->tv_usec = timestart->tv_usec + interval%1000000 - timeend.tv_usec;

	if(tm->tv_usec < 0) {
		tm->tv_usec = tm->tv_usec + 1000000;
		tm->tv_sec = tm->tv_sec - 1;
	}

	if(tm->tv_sec < 0) {
		tm->tv_sec = 0;
		tm->tv_usec = 0;
	}
	
	return 0;
}

static int report_info(int msgcode, const char *info, const uint8_t *mac, struct timeval *tv)
{
	bzero(&repinfo, sizeof(repinfo));

	if(NULL != info) 
	memcpy(repinfo.info, info, strlen(info)+1);

	if((NULL != mac) && (NULL != tv)) {
		memcpy(repinfo.mac, mac, ETH_ALEN);
		memcpy(&repinfo.tv, tv, sizeof(struct timeval));
	}

	exit_flag = msgcode;
	
	return 0; 
}

static int send_arp_packet(int sockfd, const uint8_t *src_mac, const uint8_t *src_ip, const uint8_t *dst_ip, struct sockaddr_ll *reqsa)
{
	if(NULL == src_mac || NULL == src_ip || NULL == dst_ip) {
		fprintf(stderr, "Error: receive parameter is invalid!\n");
		return -1;
	}
	
	struct arpmsg packarp;
    bzero(&packarp, sizeof(struct arpmsg));
	
	/* write ethernet header */
    memcpy(packarp.eh.ether_dhost, MAC_BCAST_ADDR, ETH_ALEN);
    memcpy(packarp.eh.ether_shost, src_mac, ETH_ALEN);
    packarp.eh.ether_type = htons(ETHERTYPE_ARP);

    /* write arp data */
    packarp.ea.arp_hrd = htons(ARPHRD_ETHER);
    packarp.ea.arp_pro = htons(ETHERTYPE_IP);
    packarp.ea.arp_hln = ETH_ALEN;
    packarp.ea.arp_pln = IP_ALEN;
    packarp.ea.arp_op  = htons(ARPOP_REQUEST);
    memcpy(packarp.ea.arp_sha, src_mac, ETH_ALEN);
    memcpy(packarp.ea.arp_spa, src_ip,  IP_ALEN);
    memcpy(packarp.ea.arp_tpa, dst_ip,  IP_ALEN);

	/* send arp packet */
    if(sendto(sockfd, &packarp, sizeof(packarp), 0, (struct sockaddr *)reqsa, sizeof(struct sockaddr_ll)) <= 0) {
		perror("Sendto error");
        return -1;
    }

	return 0;
}

static void *arp_packet_handle(void *param)
{
	int n = 0, num = 1, ret = -1, sockfd = -1, maxfd = 0, ongoing_phase = 0;
	long interval = 0;
	uint8_t ip_probe[IP_ALEN] = {0}, ip_announce[IP_ALEN] = {0};
    char recv_buf[128] = {0};
    struct sockaddr_ll reqsa;
	struct timeval tv, timestart, start, end;
	struct timeval *tm = NULL;
	int		is_started = 0;
	fd_set rdfds, current_rdfds;
	ctlmsg recv_msg;
	thdparam thd;

    bzero(&reqsa, sizeof(reqsa));
	bzero(&tv, sizeof(tv));
	bzero(&timestart, sizeof(timestart));
	bzero(&start, sizeof(start));
	bzero(&end, sizeof(end));
	bzero(&recv_msg, sizeof(recv_msg));
    gettimeofday(&start, NULL);
    start.tv_sec -= 10;
	memcpy(&thd, (thdparam *)param, sizeof(thdparam));
	printf("flagdd:%d\n", ((thdparam *)param)->mode_flag);
	printf("flag:%d\n", thd.mode_flag);

	srand((unsigned)time(NULL));

	FD_ZERO(&rdfds);
    FD_SET(unixsock.lsockfd, &rdfds);
    maxfd = unixsock.lsockfd;

	while(1) {
//   	printf("num:%d\n", num);
		if(NULL != tm) {
			get_timeout(interval, &timestart, tm);
		}
        /*
		if(NULL != tm)
		    printf("%ld,%ld\n", tm->tv_sec, tm->tv_usec);
        */

        current_rdfds = rdfds;
		ret = select(maxfd+1, &current_rdfds, NULL, NULL, tm);
		// printf("ret: %d\n", ret);

		if(ret < 0) {
			perror("Select error");
			report_info(ESELEF, "Select fail", NULL, NULL);
			return (void *)-1;
		}

		else if(ret == 0 && sockfd >= 0) {
			switch(num) {
			case 1:                                                                               /* send first arp probe packet */
			case 2:                                                                               /* send second arp probe packet */
				if(send_arp_packet(sockfd, recv_msg.mac, ip_probe, recv_msg.ip, &reqsa)) {
					report_info(ESNDPF, "Send arp packet fail", NULL, NULL);
					return (void *)-1;
				}
				interval = get_random(PROBE_MIN, PROBE_MAX);
				gettimeofday(&timestart, NULL);
				num++;
				break;
			case PROBE_NUM:                                                                       /* send third arp probe packet */
				if(send_arp_packet(sockfd, recv_msg.mac, ip_probe, recv_msg.ip, &reqsa)) {
					report_info(ESNDPF, "Send arp packet fail", NULL, NULL);
					return (void *)-1;
				}
				interval = ANNOUNCE_WAIT * 1000000;
				gettimeofday(&timestart, NULL);
				num++;
				break;
			case PROBE_NUM + 1:                                                                   /* send first arp announcement packet */ 
				if(send_arp_packet(sockfd, recv_msg.mac, ip_announce, recv_msg.ip, &reqsa)) {
					report_info(ESNDPF, "Send arp packet fail", NULL, NULL);
					return (void *)-1;
				}
				report_info(IP_VALID, "Legitimate ip address", NULL, NULL);
				ongoing_phase = 1;
				interval = ANNOUNCE_INTERVAL * 1000000;
				gettimeofday(&timestart, NULL);
				num++;
				break;
			case PROBE_NUM + ANNOUNCE_NUM:                                                        /* send second arp announcement packet */
				if(send_arp_packet(sockfd, recv_msg.mac, ip_announce, recv_msg.ip, &reqsa)) {
					memset(&repinfo, 0, sizeof(repinfo));
					strcpy(repinfo.info, "Send arp packet fail");
					thd.repfunc(ESNDPF, &repinfo);
					return (void *)-1;
				}
				tm = NULL;
				break;
			default:
				break;
			}
		}
					
	    else if(FD_ISSET(unixsock.lsockfd, &current_rdfds)) {
			if((n = recvfrom(unixsock.lsockfd, &recv_msg, sizeof(ctlmsg), 0, NULL, NULL)) <= 0) {
				perror("Recvfrom error");
				report_info(ERCVPF, "Receive arp packet fail", NULL, NULL);
				return (void *)-1;
			}

			if(recv_msg.standby_flag == 0 && !is_started) {
				reqsa.sll_family = PF_PACKET;
				reqsa.sll_protocol = htons(ETH_P_ARP);
				reqsa.sll_ifindex = if_nametoindex(recv_msg.netname);
             	memcpy(ip_announce, recv_msg.ip, IP_ALEN);

                if((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) < 0) {
					perror("Socket error");
			    	report_info(ECSOCKF, "Create socket fail", NULL, NULL);
				    return (void *)-1;
			    }

			    FD_SET(sockfd, &rdfds);
			    maxfd = (maxfd > sockfd) ? maxfd : sockfd;
				printf("mode:%d\n", thd.mode_flag);
				if(thd.mode_flag == 1) {
					num = PROBE_NUM;
				}

             	if(conflict_num < MAX_CONFLICTS) {
					interval = get_random(0, PROBE_WAIT);
	            }
            	else {
					interval = RATE_LIMIT_INTERVAL * 1000000;
	            }  

				tm = &tv;
             	gettimeofday(&timestart, NULL);
				is_started = 1;
			}

			if(recv_msg.standby_flag == 1 && is_started) {
				tm = NULL;
				close(sockfd);
				FD_CLR(sockfd, &rdfds);
                maxfd = unixsock.lsockfd;
				num = 1;
				is_started = 0;
				exit_flag = E_RESERVED;
                ongoing_phase = 0;
			}
		}

	   else if(FD_ISSET(sockfd, &current_rdfds)) { 
			if((n = recvfrom(sockfd, recv_buf, sizeof(struct arpmsg), 0, NULL, NULL)) <= 0) {
				perror("Recvfrom error");
				report_info(ERCVPF, "Receive arp packet fail", NULL, NULL);
				return (void *)-1;
			}
		//	printf("peer mac is:%02x:%02x:%02x:%02x:%02x:%02x\n",(uint8_t)*(recv_buf+22), (uint8_t)*(recv_buf+23), (uint8_t)*(recv_buf+24),
		//			(uint8_t)*(recv_buf+25), (uint8_t)*(recv_buf+26), (uint8_t)*(recv_buf+27));
		//  printf("peer ip is:%d.%d.%d.%d\n", (uint8_t)*(recv_buf +28), (uint8_t)*(recv_buf + 29), (uint8_t)*(recv_buf + 30), (uint8_t)*(recv_buf + 31));
			if(ongoing_phase == 0) {
				if((!memcmp(recv_msg.ip, recv_buf + 28, IP_ALEN)) || ((!memcmp(ip_probe, recv_buf + 28, IP_ALEN)) 
							&& (!memcmp(recv_msg.ip, recv_buf + 38, IP_ALEN)) && memcmp(recv_msg.mac, recv_buf + 22, ETH_ALEN))) {
					conflict_num++;
					gettimeofday(&tv, NULL);
					report_info(IP_CONFLICT_PROBE, "Ip address conflict in probe phase", (uint8_t *)(recv_buf + 22), &tv);
					tm = NULL;
					close(sockfd);
					FD_CLR(sockfd, &rdfds);
					maxfd = unixsock.lsockfd;
				}
			}
			if(ongoing_phase == 1) {
				if((!memcmp(ip_announce, recv_buf + 28, IP_ALEN)) && memcmp(recv_msg.mac, recv_buf + 22, ETH_ALEN)) {
					gettimeofday(&end, NULL);
										
					if(1000000*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec > 1000000*DEFEND_INTERVAL) {
						if(send_arp_packet(sockfd, recv_msg.mac, ip_announce, recv_msg.ip, &reqsa)) {
							memset(&repinfo, 0, sizeof(repinfo));
							strcpy(repinfo.info, "Send arp packet fail");
							thd.repfunc(ESNDPF, &repinfo);
							return (void *)-1;
						}
					} else {
                        strcpy(repinfo.info, "Ip address conflict. receive 2 conflict arp packets in 10 seconds");
                        memcpy(repinfo.mac, recv_buf + 22, ETH_ALEN);
                        memcpy(&repinfo.tv, &end, sizeof(struct timeval));
                        thd.repfunc(IP_CONFLICT_IN10SEC, &repinfo);
                    }
                    strcpy(repinfo.info, "Ip address conflict in ongoing phase");
                    memcpy(repinfo.mac, recv_buf + 22, ETH_ALEN);
                    memcpy(&repinfo.tv, &end, sizeof(struct timeval));
                    thd.repfunc(IP_CONFLICT_ONGOING, &repinfo);
                    exit_flag = E_RESERVED;
					memcpy(&start, &end, sizeof(struct timeval));
				}
			}
		}
	}
		return NULL;
}

int arp_probe_init(int mode_flag, probe_handle report)
{
	static int inited = 0;
	pthread_t pid;
	static thdparam param;

	if(inited)
		return 0;
	inited = 1;

	bzero(&unixsock, sizeof(unixsock));
    param.mode_flag = mode_flag;
	param.repfunc = report;
	unixsock.addr.sun_family = AF_UNIX;
	strcpy(unixsock.addr.sun_path, ARP_PROBE_MSG);
    unixsock.len = sizeof(struct sockaddr_un);
	unlink(ARP_PROBE_MSG);
	
	if((unixsock.lsockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
		perror("Socket error");
		inited = 0;
		return ECSOCKF;
	}

	if(bind(unixsock.lsockfd, (struct sockaddr *)&unixsock.addr, unixsock.len) < 0)
	{
		perror("Bind error");
		close(unixsock.lsockfd);
		inited = 0;
		return ESOCKBF;
	}

	if(pthread_create(&pid, NULL, arp_packet_handle, &param) != 0) {
		perror("Thread create error");
		close(unixsock.lsockfd);
		inited = 0;
		return ECTHDF;
	}


	if(pthread_detach(pid)) {
		perror("Thread detach error");
		return EDTHDF;
	}
	
	return 0;
}


int arp_probe_start(const uint8_t *ip, const char *net_name, struct reportdata *msg)
{
    if(NULL == ip) {
		if(NULL != msg) {
			strcpy(msg->info, "Ip is null");
		}
		return EIPNU;
	}

	if(NULL == net_name) {
		if(NULL != msg) {
			strcpy(msg->info, "Net name is null");
		}
		return ENETNU;
	}

	int n;
	uint8_t mac[ETH_ALEN] = {0};
	ctlmsg start_msg;
	
	if(exit_flag != E_RESERVED)
		return exit_flag;

	bzero(&repinfo, sizeof(repinfo));
	bzero(&start_msg, sizeof(start_msg));

	if(get_mac(net_name, mac, ETH_ALEN)) {
		if(NULL != msg) {
			strcpy(msg->info, "Get mac address fail");
		}
		return EGMACF;
	}

	start_msg.standby_flag = 0;
	memcpy(start_msg.ip, ip, IP_ALEN);
	memcpy(start_msg.mac, mac, ETH_ALEN);
	strcpy(start_msg.netname, net_name);

    if((n = sendto(unixsock.lsockfd, &start_msg, sizeof(start_msg), 0, (struct sockaddr *)&unixsock.addr, sizeof(unixsock.addr))) <= 0) {
		perror("Sendto error");
		if(NULL != msg) {
			strcpy(msg->info, "Send message fail");
		}

		return ESNDPF;
    }

	while(exit_flag == E_RESERVED) {
		usleep(1000);
	}
    
    n = exit_flag;
    exit_flag = E_RESERVED;

	if(NULL != msg) {
		memcpy(msg, &repinfo, sizeof(struct reportdata));
	}

	return n;
}

int arp_probe_stop(void)
{
	ctlmsg stop_msg;

	bzero(&stop_msg, sizeof(stop_msg));
	stop_msg.standby_flag = 1;

    if(sendto(unixsock.lsockfd, &stop_msg, sizeof(stop_msg), 0, (struct sockaddr *)&unixsock.addr, sizeof(unixsock.addr)) <= 0) {
		perror("Sendto error");
		return ESNDPF;
	}

	return 0;
}
