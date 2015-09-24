#ifndef _SNTP_H_
#define _SNTP_H_

#include <time.h>
#include <ngsocket.h>

#define SNTP_HANDLE_INVALID	100
#define SNTP_NETWORK_ERROR	101

typedef struct  _SYSTEM_TIME 
{
    struct tm Seconds;
    unsigned short Milliseconds;
}  SYSTEM_TIME;
typedef struct _SystemTimeStoreStru
{
SYSTEM_TIME SystemTime;
unsigned long	 TickCount;
} SystemTimeStoreStru;
typedef struct _SNTP_Manage_Info
{
int      iNetSocket;              /* Socket handle */
int	   iNetPort;	/*  UDP port*/
int      iAddr_len;              /* Parameters for Socket intialization */
struct sockaddr_in ServerAddr;      /* Parameters for Socket intialization */
SYSTEM_TIME SystemTime; 
} SNTP_Manage_Info, *SNTP_Handle;

#ifdef _cplusplus
extern "c" {
#endif

void Init_LocalTime(void);
void SetTimeZone(int TimeZone);
void GetLocalTime(SYSTEM_TIME *dst);
void SetUTCTime(SYSTEM_TIME *src);
void SetTimeZone(int TimeZone);
/*******************************************************************
   Name:         SNTP_Init
   
   Purpose:     Initialize struct SNTP_Handle
   
   Arguments:   <SNTP_Handle *handle>: The struct to initialized
   
   ReturnValue: < int >:  return SNTP_SUCCESS if succeeded, otherwise return SNTP_FAILURE
   
   Public:      Yes
   
   ************************************************************************ */
SNTP_Handle SNTP_Init(SNTP_Manage_Info *info);
/*******************************************************************
   Name:         SNTP_Open
   
   Purpose:     Establish the socket
   
   Arguments:   <SNTP_Handle *handle>: Contain the socket to be establish;
   	  <char *pIpAddr>: Contain the ip address of the SNTP server;
   
   ReturnValue: < int >:  return SNTP_SUCCESS if succeeded, otherwise return SNTP_FAILURE
   
   Public:      Yes
   
   ************************************************************************ */
int SNTP_Open(SNTP_Handle handle);
/*******************************************************************
   Name:         SNTP_Get
   
   Purpose:     Get the time from server
   
   Arguments:   <SNTP_Handle *handle>: Contain the time struct to be filled;
   
   ReturnValue: < int >:  return SNTP_SUCCESS if succeeded, otherwise return SNTP_FAILURE
   
   Public:      Yes
   
   ************************************************************************ */
int SNTP_Get(SNTP_Handle handle,  char *pServerIP);
/*******************************************************************
   Name:         SNTP_Close
   
   Purpose:     Close the socket
   
   Arguments:   <SNTP_Handle *handle>: Contain the socket to be closed;
   
   ReturnValue: < int >:  return SNTP_SUCCESS if succeeded, otherwise return SNTP_FAILURE
   
   Public:      Yes
   
   ************************************************************************ */
int SNTP_Close(SNTP_Handle handle);
/*******************************************************************
   Name:         SNTP_Term
   
   Purpose:     Deallocate the memory that SNTP_Init allocated
   
   Arguments:   <SNTP_Handle *handle>:  The struct that SNTP_Init initialized;
   
   ReturnValue: < int >:  return SNTP_SUCCESS if succeeded, otherwise return SNTP_FAILURE
   
   Public:      Yes
   
   ************************************************************************ */
int SNTP_Term(SNTP_Handle handle);
#ifdef _cplusplus
}
#endif
#endif /*_SNTP_H_*/