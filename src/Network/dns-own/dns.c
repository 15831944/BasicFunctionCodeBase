
/**
 * DNS Resolution Code
 * @author Craig Graham
 * @date 07/11/2001
 * @file dns.c
 */
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>        /* for AF_INET */
#include <netinet/in.h>
#include <sys/time.h>
#include <string.h>
#include "dns.h"
#define DNS_MAX_RETRY 5
#define DNS_CACHE_MAX 10
#define DNS_DEBUG 1
/** DNS lookup cache (used to avoid doing lookups for every connection) */
static DNS_CACHE cache[DNS_CACHE_MAX];
/** Index of next DNS cache slot to use */
static int cachePtr=0;
/** Query ID */
static unsigned short queryId=0x1234;
/**
 * Build up a DNS query packet header
 */
void dns_makeHeader(unsigned char packet[],DNS_OPCODE opcode)
{
unsigned short flags;
flags=(opcode<<12)|DNSF_RECURSIVE;
packet[0]=(queryId>>8)&0xff;
packet[1]=(queryId)&0xff;
packet[2]=(flags>>8)&0xff;
packet[3]=flags&0xff;
packet[4]=0; // questions count MSB
packet[5]=1; // questions count LSB
packet[6]=0; // answers count MSB
packet[7]=0; // answers count LSB
packet[8]=0; // authority count MSB
packet[9]=0; // authority count LSB
packet[10]=0; // additional count MSB
packet[11]=0; // additional count LSB
queryId++;
}
/**
 * Decode a DNS response packet header
 */
static void dns_decodeHeader(unsigned char packet[], DNS_HEADER *h)
{
h->id=(packet[0]<<8)|packet[1];
h->flags=(packet[2]<<8)|packet[3];
h->questionCount=(packet[4]<<8)|packet[5];
h->answerCount=(packet[6]<<8)|packet[7];
h->authorityCount=(packet[8]<<8)|packet[9];
h->additionalCount=(packet[10]<<8)|packet[11];
}
static char *encodeDotted(const char *name,char *output)
{
int l;
const char *startSegment;
while(*name)
{
l=0;
for(startSegment=name; (*name)&&(*name!='.'); name++)
{
l++;
output[l]=*name;
}
*output++=l;
output+=l;
if(*name)
name++;
}
*output++='';
return output;
}
static char *decodeDotted(char *packet, char *input, char *output)
{
int segLen;
unsigned short offset;
char *rtn=NULL;
while(1)
{
while((*input&(0xC0))==0xC0)
{
offset=((input[0]&~0xc0)<<8)|input[1];
if(rtn==NULL)
rtn=&input[2];
input=&packet[offset];
}
segLen=*input++;
while(segLen--)
*output++=*input++;
if(!*input)
break;
*output++='.';
}
*output='';
return (rtn)?rtn:++input;
}
static int dns_sendQuery(DNS_CONTEXT *c)
{
unsigned char packet[1024],*p;
unsigned long l,pl;
struct sockaddr_in ns;
int ret;
pl=l=strlen(c->name);
dns_makeHeader(packet,DNS_QUERY);
p=encodeDotted(c->name,&packet[12]);
*p++=(DNS_T_A>>8); *p++=DNS_T_A&0xff; //type = host address query
*p++=0; *p++=1;	//class 1 - internet address
pl=((unsigned long)p)-(unsigned long)packet;
c->s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
if(c->s<0)
{
#if DNS_DEBUG
printf("dns_sendQuery: cann't create socketn");
#endif
return -1;
}
memset(&ns, 0 , sizeof(ns)) ;
ns.sin_family = AF_INET ;	/* Using TCP/IP protocol */
ns.sin_port = htons(53) ;	/* converts to network short */
memcpy(&ns.sin_addr, c->ns, 4) ;
if (connect (c->s, (struct sockaddr *)&ns, sizeof(ns)) != 0)
{
#if DNS_DEBUG
printf("dns_sendQuery: cann't connect to socketn");
#endif
return -2;
}
ret=write(c->s,packet,pl);
if(ret!=pl)
{
#if DNS_DEBUG
printf("dns_sendQuery: write failledn");
#endif
close(c->s);
return -3;
}
return 0;
}
#if DNS_DEBUG
char recursionIndicator[100];
int recursionIndicatorI=0;
void incR(void)
{
recursionIndicator[recursionIndicatorI++]='|';
recursionIndicator[recursionIndicatorI++]=' ';
recursionIndicator[recursionIndicatorI]='';
}
void decR(void)
{
recursionIndicatorI-=2;
recursionIndicator[recursionIndicatorI]='';
}
#else
#define incR()
#define decR()
#endif
static int dns_getResponse(DNS_CONTEXT *c)
{
unsigned char packet[1024];
unsigned char name[1024],*p,*aS;
DNS_HEADER h;
int ret,cnt,gotAddress=0;
unsigned short qClass,aClass,aLength;
DNS_T_TYPE qType,aType;
unsigned long aTTL;
unsigned char aAddress[4];
struct timeval timeout;
fd_set readFds;
int retryCount=0;
timeout.tv_sec=2;
timeout.tv_usec=0;
while(!gotAddress)
{
__retry:
FD_ZERO(&readFds);
FD_SET(c->s,&readFds);
ret=select(c->s+1,&readFds,NULL,NULL, &timeout);
if(ret<=0)
{
// timeout or error, retry...
#if DNS_DEBUG
if(ret)
printf("%serror, retryn",recursionIndicator);
else
printf("%stimeout, retryn",recursionIndicator);
#endif
close(c->s);
retryCount++;
if(retryCount==DNS_MAX_RETRY)
return -4;
ret=dns_sendQuery(c);
if(ret)
return ret;
continue;
}
ret=read(c->s,packet,1024);
close(c->s);
if(ret>=12)
{
dns_decodeHeader(packet,&h);
#if DNS_DEBUG
printf("%sgot response from ns %d.%d.%d.%d : n",recursionIndicator,c->ns[0],c->ns[1],c->ns[2],c->ns[3]);
printf("%s questionCount=%dn",recursionIndicator,h.questionCount);
printf("%s answerCount=%dn",recursionIndicator,h.answerCount);
printf("%s authorityCount=%dn",recursionIndicator,h.authorityCount);
#endif
p=&packet[sizeof(DNS_HEADER)];
// process query section of packet
for(cnt=h.questionCount; cnt; cnt--)
{
p=decodeDotted(packet,p, name);
qType=(p[0]<<8)|p[1];
qClass=(p[2]<<8)|p[3];
p+=4;
#if DNS_DEBUG
printf("%squestion = "%s" type=%x class=%xn",recursionIndicator,name,qType,qClass);
#endif
}
// process answer section of packet
aS=p;
for(cnt=h.answerCount; cnt; cnt--)
{
p=decodeDotted(packet, p, name);
aType=(p[0]<<8)|p[1];
aClass=(p[2]<<8)|p[3];
aTTL=(p[4]<<24)|(p[5]<<16)|(p[6]<<8)|p[7];
aLength=(p[8]<<8)|p[9];
if(aType==DNS_T_A)
{
// we've got an address, so we can return here....
c->addr[0]=p[10];
c->addr[1]=p[11];
c->addr[2]=p[12];
c->addr[3]=p[13];
#if DNS_DEBUG
printf("%sanswer: "%s" type=%x class=%x TTL=%lx address=%d.%d.%d.%dn",recursionIndicator,
name,aType,aClass,aTTL,c->addr[0],c->addr[1],c->addr[2],c->addr[3]);
#endif
return 0;
}
#if DNS_DEBUG
printf("%sanswer: "%s" type=%x class=%x TTL=%lxn",recursionIndicator,name,aType,aClass,aTTL);
#endif
p+=10+aLength;
}
// process authority section of packet
for(cnt=h.authorityCount; cnt; cnt--)
{
p=decodeDotted(packet, p, name);
aType=(p[0]<<8)|p[1];
aClass=(p[2]<<8)|p[3];
aTTL=(p[4]<<24)|(p[5]<<16)|(p[6]<<8)|p[7];
aLength=(p[8]<<8)|p[9];
p+=10;
#if DNS_DEBUG
printf("%sauthority: "%s" type=%x class=%x TTL=%lxn",recursionIndicator,name,aType,aClass,aTTL);
#endif
if(aType==DNS_T_NS)
{
// Try to resolve authoritive nameserver for this address....
struct hostent *authoritiveServer;
p=decodeDotted(packet, p, name);
#if DNS_DEBUG
printf("%s           nameServer="%s"n",recursionIndicator,name);
#endif
if(strcmp(name,c->name))
{
authoritiveServer=gethostbyname(name);
if(authoritiveServer)
{
// we found the authoritive server, re-target our original request at the authoritive nameserver..
memcpy(c->ns,authoritiveServer->h_addr_list[0],4);
#if DNS_DEBUG
printf("%s!!!! trying authoritive server %sn",recursionIndicator,name);
#endif
incR();
ret=dns_sendQuery(c);
if(!ret)
{
ret=dns_getResponse(c);
decR();
if(!ret)
return 0;
#if DNS_DEBUG
printf("%s!!!! server %s didn't respond :(n",recursionIndicator,name);
#endif
}else{
decR();
#if DNS_DEBUG
printf("%s!!!! couldn't contact server %s :(n",recursionIndicator,name);
#endif
}
}
}
}
}
}
ret=dns_sendQuery(c);
if(ret)
return ret;
}
return 0;
}
char nameserver[4]={195,184,228,6};
void setNameServer(const char *ns1, const char *ns2)
{
UI32 a=ntohl(inet_addr(ns1));
memcpy(nameserver,&a,4);
}
struct hostent *addCacheEntry(DNS_CONTEXT *c)
{
struct hostent *rtn;
// got a response, cache it and return the cache entry...
printf("dns lookup, got address for %s, caching in slot %dn",c->name,cachePtr);
if(cache[cachePtr].name)
free(cache[cachePtr].name);
memcpy(&cache[cachePtr].in, c->addr, sizeof(cache[cachePtr].in));
cache[cachePtr].name=strdup(c->name);
cache[cachePtr].h.h_name = cache[cachePtr].name;
cache[cachePtr].h.h_addrtype = AF_INET;
cache[cachePtr].h.h_length = sizeof(cache[cachePtr].in);
cache[cachePtr].h.h_addr_list = (char **) cache[cachePtr].addr_list;
cache[cachePtr].addr_list[0]=&cache[cachePtr].in;
cache[cachePtr].addr_list[1]=NULL;
rtn=&cache[cachePtr].h;
if(++cachePtr==DNS_CACHE_MAX)
cachePtr=0;
return rtn;
}
struct hostent *getHostFromCache(const char *name)
{
int i;
// do dns cache lookup...
for(i=0; i<DNS_CACHE_MAX; i++)
{
if((cache[i].name)&&(!strcmp(name,cache[i].name)))
{
printf("getHostFromCache(%s): got cached address in slot %dn",name,i);
return &cache[i].h;
}
}
return NULL;
}
struct hostent *gethostbyaddr(const char *addr, int len, int type)
{
DNS_CONTEXT c;
if(inet_aton(addr,(struct in_addr*)&c.addr))
{
c.name=addr;
return addCacheEntry(&c);
}
return NULL;
}
struct hostent *gethostbyname(const char *name)
{
DNS_CONTEXT c;
int ret;
struct hostent *h;
#if DNS_DEBUG
printf("gethostbyname(%s)n",name);
#endif
// try for cached address from a previous lookup...
h=getHostFromCache(name);
if(h)
return h;
// not a totally valid thing to do here, but we check for numeric address's
// here anyway...(makes higher level code simpler)
h=gethostbyaddr(name,4,1);
if(h)
return h;
// ok, it's not a numeric address and it's not in the DNS cache,
// so we'll have to do a proper DNS lookup...
c.name=name;
c.ns[0]=nameserver[0];
c.ns[1]=nameserver[1];
c.ns[2]=nameserver[2];
c.ns[3]=nameserver[3];
ret=dns_sendQuery(&c);
if(ret<0)
return NULL;
ret=dns_getResponse(&c);
if(ret==0)
{
return addCacheEntry(&c);
}
return NULL;
}
void dns_init(void)
{
int i;
#if DNS_DEBUG
recursionIndicator[0]='';
recursionIndicatorI=0;
#endif
printf("dns_init()n");
for(i=0; i<DNS_CACHE_MAX; i++)
{
cache[i].name=NULL;
}
}


