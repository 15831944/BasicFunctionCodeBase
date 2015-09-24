
/*  Copyright (C) 1996 N.M. Maclaren
    Copyright (C) 1996 The University of Cambridge
This includes all of the 'safe' headers and definitions used across modules. No changes should be needed for any system 
that is even remotely like Unix. */


#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#define VERSION         "1.3"          /* Just the version string */


/* Defined in main.c */
#define op_client           1          /* Behave as a challenge client */
#define op_server           2          /* Behave as a response server */
#define op_listen           3          /* Behave as a listening client */
#define op_broadcast        4          /* Behave as a broadcast server */

extern const char *argv0;
extern int verbose, operation;
extern void fatal (int syserr, const char *message, const char *insert);


/* Defined in unix.c */
extern void do_nothing (int seconds);
extern int ftty (FILE *file);
extern void set_lock (int lock);


/* Defined in internet.c */
/* extern void find_address (struct in_addr *address, int *port, char *hostname, int timespan); */


/* Defined in socket.c */
extern void open_socket (char *hostname, int timespan);
extern void write_socket (void *packet, int length);
extern int read_socket (void *packet, int length, int waiting);
extern int flush_socket (void);
extern void close_socket (void);


/* Defined in timing.c */
extern double current_time (double offset);
extern time_t convert_time (double value, int *millisecs);
extern void adjust_time (double difference, int immediate, double ignore);
#include <syslog.h>
void slogff(char *file,char *func,int line,const char *fmt,...);
void dslogff(char *file,char *func,int line,int dlevel,const char *fmt,...);
#define slogf(fmt...) slogff(__FILE__,__FUNCTION__,__LINE__,fmt)
#define dslogf(fmt...) dslogff(__FILE__,__FUNCTION__,__LINE__,fmt)
#define printf(args...) slogf(args)
#define fprintf(dst,args...) slogf(args)
//#define printf(args...) syslog(LOG_DAEMON + LOG_WARNING,args)
//#define fprintf(dst, args...) syslog(LOG_DAEMON + LOG_WARNING,args)
#define fatal(args...) return 


