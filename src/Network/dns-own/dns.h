#ifndef _DNS_H_
#define _DNS_H_
#include <netdb.h>
#include <netinet/in.h>
typedef struct {
char *name;
struct in_addr in;
  struct in_addr *addr_list[2];
struct hostent h;
} DNS_CACHE;
typedef struct {
const char *name;
unsigned char ns[4];
int s;
unsigned char addr[4];
} DNS_CONTEXT;
typedef enum {
DNS_QUERY=0,
DNS_IQUERY=1,
DNS_STATUS=2
} DNS_OPCODE;
/** Request / response type codes */
typedef enum {
DNS_T_A=1,	/** host address */
DNS_T_NS=2,	/** authoritive nameserver */
DNS_T_MD=3,	/** mail destination (obsolete, use MX) */
DNS_T_MF=4,	/** mail forwarder (obsolete, use MX) */
DNS_T_CNAME=5,	/** cannonical name (alias) */
DNS_T_SOA=6,	/** start zone of authority */
DNS_T_MB=7,	/** mailbox domain name (experimental) */
DNS_T_MG=8,	/** mail group member destination (experimental) */
DNS_T_MR=9,	/** mail rename domain name (experimental) */
DNS_T_NULL=10,	/** (experimental) */
DNS_T_WKS=11,	/** well known service description */
DNS_T_PTR=12,	/** domain name pointer */
DNS_T_HINFO=13,	/** host information */
DNS_T_MINFO=14,	/** mailbox info*/
DNS_T_MX=15,	/** mail exchange */
DNS_T_TXT=16,	/** text strings */
} DNS_T_TYPE;
typedef enum {
DNSF_QUERY=(1<<15),
DNSF_AUTHORITIVE_ANSWER=(1<<11),
DNSF_RECURSIVE=(1<<10),
DNSF_RECURSIVE_AVAILABLE=(1<<9),
} DNS_FLAGS;
typedef struct{
unsigned short id;
unsigned short flags;
unsigned short questionCount;
unsigned short answerCount;
unsigned short authorityCount;
unsigned short additionalCount;
} DNS_HEADER;
#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
extern "C" {
#endif
extern void setNameServer(const char *ns1, const char *ns2);
extern struct hostent *gethostbyaddr(const char *addr, int len, int type);
extern struct hostent *gethostbyname(const char *name);
extern void dns_init(void);
#if defined(_LANGUAGE_C_PLUS_PLUS)||defined(__cplusplus)||defined(c_plusplus)
}
#endif
#define resolvInit dns_init
#endif