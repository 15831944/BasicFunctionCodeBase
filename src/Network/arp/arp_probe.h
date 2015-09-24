#ifndef __ARP_PROBE_H__
#define __ARP_PROBE_H__

#define    E_RESERVED          -1     /* reserved. */
#define    IP_VALID             0     /* Legitimate ip address */
#define    EIPNU                1     /* Ip is null */
#define    ENETNU               2     /* Net name is null */
#define    EGMACF               3     /* Get mac address fail */
#define    ECTHDF               4     /* Create thread fail */
#define    EDTHDF               5     /* Detach thread fail */
#define    ECSOCKF              6     /* Create socket fail */
#define    ESOCKBF              7     /* Bind socket fail */
#define    ESELEF               8     /* Select fail */
#define    ESNDPF               9     /* Send arp packet fail */
#define    ERCVPF               10    /* Receive arp packet fail */

#define    IP_CONFLICT_PROBE    12    /* Ip address conflict in probe phase */
#define    IP_CONFLICT_ONGOING  13    /* Ip address conflict in ongoing phase */
#define    IP_CONFLICT_IN10SEC  14    /* Ip address conflict in ongoing phase. receive 2 conflict arp packets in 10 seconds */

#define    DATALEN              128   /* the length of describe information */

struct reportdata {
	char info[DATALEN];               /* reported detail information */
	unsigned char  mac[6];            /* received peer mac address */
	struct timeval tv;                /* time recorded of received packet */
};

typedef int (*probe_handle)(int, struct reportdata *);     /* define the callback function pointer */

int arp_probe_init(int mode_flag, probe_handle report);

int arp_probe_start(const unsigned char *ip, const char *net_name, struct reportdata *msg);

int arp_probe_stop(void);
#endif
