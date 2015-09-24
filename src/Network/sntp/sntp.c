
/*  Copyright (C) 1996, 1997 N.M. Maclaren
    Copyright (C) 1996, 1997 The University of Cambridge
This is a complete SNTP implementation, which was easier to write thad to port XNTP to a new version of Unix 
with any hope of maintaining it thereafter.  It supports the full SNTP (RFC 2030) client- and server-side 
challenge-response and broadcast protocols.  It should achieve nearly optimal accuracy with very few transactions, 
provided only that a client has access to a trusted server and that communications are not INVARIABLY slow.  As 
this is the environment in which 90-99% of all NTP systems are run ....
The specification of this program is:
    msntp [ -v | -V ]
        [ -B [ period ] | -S |
            [ -r | -a ] [ -e minerr ] [ -E maxerr ] [ -c count ]
            [ -d delay | -x [ separation ] [ -f savefile ] ] [ address ] ]
    -v indicates that diagnostic messages should be written to standard error.
-V requests much more verbose debugging output, and will interfere with the timing when writing to the terminal 
(because of line buffered output from C). Note that the times produced by -V are the corrections needed, and not 
the error in the local clock.
    -B indicates that it should behave as a server, broadcasting time packets at intervals of 'period' minutes. 
Acceptable values of 'period' are from 1 to 1440 (a day) and the default is 60.  Naturally, this will work only 
if the user has enough privilege.
    -S indicates that it should behave as a server, responding to time requests from clients.  Naturally, this 
will work only if the user has enough privilege.
    The default is that it should behave as a client, and the following options are then relevant:
    -r indicates that the system clock should be reset by 'settimeofday'. Naturally, this will work only if the 
user has enough privilege.
    -a indicates that the system clock should be reset by 'adjtime'. Naturally, this will work only if the user
has enough privilege.
    -x indicates that the program should run as a daemon (i.e. forever), and allow for clock drift.
    The default is to write the current date and time to the standard output in a format like '1996 Oct 15 
20:17:25.123 + 4.567 +/- 0.089 secs', indicating the estimated true (local) time and the error in the local clock. 
In daemon mode, it will add drift information in a format like ' + 1.3 +/- 0.1 ppm', and display this at roughly 
'separation' intervals.
    'minerr' is the maximum ignorable variation between the clocks.  Acceptable values are from 0.001 to 1, and 
the default is 0.1 if 'address' is specified and 0.5 otherwise.
    'maxerr' is the maximum clock change that will be made automatically. Acceptable values are from 0.1 to 60, 
and the default is 5.  If the program is being run interactively, larger values will cause a prompt.
    'count' is the maximum number of NTP packets to require.  Acceptable values are from 1 to 25 if 'address' is 
specified and '-x' is not, and from 5 to 25 otherwise; the default is 5.  If the maximum isn't enough, you need 
a better consistency algorithm than this program uses.  Don't increase it.
    'delay' is a rough limit on the total running time in seconds.  Acceptable values are from 1 to 3600, and the 
default is 15 if 'address' is specified and 300 otherwise.
    'separation' is the minimum time between calls to the server in minutes if 'address' is specified, and between 
broadcast packets if not.  Acceptable values are from 1 to 1440 (a day), and the default is 300.
    'savefile' may be used in daemon mode to store a record of previous packets, which may speed up recalculating 
the drift after msntp has to be restarted (e.g. because of network or server outages).
    'address' is the DNS name or IP number of a host to poll; if no name is given, the program waits for broadcasts.  
Note that a single component numeric address is not allowed.

	For sanity, it is also required that 'minerr' < 'maxerr' < 'delay' (if listening for broadcasts, 'delay/count' 
abd, in daemon mode, 'separation') and, for sordid Unixish reasons, that 2*'count' < 'delay'.  The latter could be 
fixed, but isn't worth it.  Note that none of the above values are closely linked to the limits described in the 
NTP protocol (RFC 1305).  Do not increase the compiled-in bounds excessively, or the code will fail.
	The algorithm used to decide whether to accept a correction is whether it would seem to improve matters. Unlike 
the 'xntp' suite, no attempt is made to handle really knotted scenarios, and diagnostics are written to standard 
error. In non-daemon client mode, it is intended to be run as a command or in a 'cron' job.  Unlike 'ntpdate', its 
default mode is simply to display the clock error. It assumes that floating-point arithmetic is tolerably efficient,
which is true for even the cheapest personal computer nowadays.  If, however, you want to port this to a toaster, 
you may have problems!
	In non-daemon client mode, its return code is EXIT_SUCCESS if the operation was completed successfully and 
EXIT_FAILURE otherwise.
	In server or daemon mode, it runs for ever and stops with a return code EXIT_FAILURE only after a severe error.  
Normally, two server processes will be run, one with each of the -B and -S options.  In daemon mode, it will fail if
the server is inaccessible for a long time or seriously sick, and will need manual restarting. 
*/


#include "header.h"
#include <limits.h>
#include <float.h>
#include <math.h>
#define MAIN
#include "kludges.h"
#undef MAIN



/* NTP definitions.  Note that these assume 8-bit bytes - sigh.  There is little point in parameterising everything, 
	as it is neither feasible nor useful.  It would be very useful if more fields could be defined as 'unspecified.  
	The NTP packet-handling routines contain a lot of extra assumptions. */
#define JAN_1970   2208988800.0        /* 1970 - 1900 in seconds */
#define NTP_SCALE  4294967296.0        /* 2^32, of course! */
#define NTP_PACKET_MIN       48        /* Without authentication */
#define NTP_PACKET_MAX       68        /* With authentication (ignored) */
#define NTP_DISP_FIELD        8        /* Offset of dispersion field */
#define NTP_REFERENCE        16        /* Offset of reference timestamp */
#define NTP_ORIGINATE        24        /* Offset of originate timestamp */
#define NTP_RECEIVE          32        /* Offset of receive timestamp */
#define NTP_TRANSMIT         40        /* Offset of transmit timestamp */
#define NTP_LI_FUDGE          0        /* The current 'status' */
#define NTP_VERSION           3        /* The current version */
#define NTP_VERSION_MAX       4        /* The maximum valid version */
#define NTP_STRATUM           0        /* The current stratum */
#define NTP_STRATUM_MAX      15        /* The maximum valid stratum */
#define NTP_POLLING           8        /* The current 'polling interval' */
#define NTP_PRECISION         0        /* The current 'precision' - 1 sec. */
#define NTP_ACTIVE            1        /* NTP symmetric active request */
#define NTP_PASSIVE           2        /* NTP symmetric passive response */
#define NTP_CLIENT            3        /* NTP client request */
#define NTP_SERVER            4        /* NTP server response */
#define NTP_BROADCAST         5        /* NTP server broadcast */
#define NTP_INSANITY     3600.0        /* Errors beyond this are hopeless */
#define RESET_MIN            60        /* Minimum period between resets */
#define ABSCISSA            3.0        /* Scale factor for standard errors */

/* Local definitions and global variables (mostly options).  These are all of the quantities that control 
	the main actions of the program.  The first three are the only ones that are exported to other modules. */
const char *argv0 = NULL;              /* For diagnostics only - not NULL */
int verbose = 0,                       /* Default = 0, -v = 1, -V = 2 */
    operation = 0;                     /* Defined in header.h - see action */
#define COUNT_MAX          25          /* Do NOT increase this! */
#define WEEBLE_FACTOR     1.2          /* See run_server() and run_daemon() */
#define ETHERNET_MAX        5          /* See run_daemon() and run_client() */
#define action_display      1          /* Just display the result */
#define action_reset        2          /* Reset using 'settimeofday' */
#define action_adjust       3          /* Reset using 'adjtime' */
#define action_broadcast    4          /* Behave as a server, broadcasting */
#define action_server       5          /* Behave as a server for clients */
static const char version[] = VERSION; /* For reverse engineering :-) */
static int action = 2,                 /* Defined above - see operation */
    period = 0,                        /* -B value in seconds (broadcast) */
    count = 5,                         /* -c value in seconds */
    delay = 15,                         /* -d or -x value in seconds */
    attempts = 0,                      /* Packets transmitted up to 2*count */
    waiting = 3,                       /* -d/-c except for in daemon mode */
    locked = 0;                        /* set_lock(1) has been called */
static double outgoing[2*COUNT_MAX],   /* Transmission timestamps */
    minerr = 0.0,                      /* -e value in seconds */
    maxerr = 0.0,                      /* -E value in seconds */
    dispersion = 0.0;                  /* The source dispersion in seconds */
static FILE *savefile = NULL;          /* Holds the data to restart from */

/* The unpacked NTP data structure, with all the fields even remotely relevant to SNTP. */
typedef struct NTP_DATA {
    unsigned char status, version, mode, stratum, polling, precision;
    double dispersion, reference, originate, receive, transmit, current;
} ntp_data;

/* The following structure is used to keep a record of packets in daemon mode;
	it contains only the information that is actually used for the drift and error calculations. */
typedef struct {
    double dispersion, weight, when, offset, error;
} data_record;

/*
void fatal (int syserr, const char *message, const char *insert) 
{
    int k = errno;
    static int called = 0;
    if (message != NULL) {
        fprintf(stderr,"%s: ",argv0);
        fprintf(stderr,message,insert);
        fprintf(stderr,"n");
    }
    errno = k;
    if (syserr) 
		perror(argv0);
    if (! called) {
        called = 1;
        if (savefile != NULL && fclose(savefile))
            fatal(1,"unable to close the daemon save file",NULL);
        if (locked) 
			set_lock(0);
    }
    exit(EXIT_FAILURE);
}
*/

void syntax (void) 
{
	/* The standard, unfriendly Unix error message.  Some errors are diagnosed more helpfully. */
    fprintf(stderr,"Syntax: %s [ -v | -V ] [ -B period | -S |n",argv0);
    fprintf(stderr,"    [ -r | -a ] [ -e minerr ] [ -E maxerr ] ");
    fprintf(stderr,"[ -c count ]n");
    fprintf(stderr,"    [ -d delay | -x [ separation ] [ -f savefile ] ] ");
    fprintf(stderr,"[ address ] ]n");
    fatal(0,NULL,NULL);
}

void display_data (ntp_data *data) 
{
	/* This formats the essential NTP data, as a debugging aid. */
    fprintf(stderr,"sta=%d ver=%d mod=%d str=%d pol=%d dis=%.6f ref=%.6fn", data->status, data->version, data->mode, 
				data->stratum, data->polling, data->dispersion,data->reference);
    fprintf(stderr,"ori=%.6f rec=%.6fn",data->originate,data->receive);
    fprintf(stderr,"tra=%.6f cur=%.6fn",data->transmit,data->current);
}

void display_packet (unsigned char *packet, int length) 
{
	/* This formats a possible packet very roughly, as a debugging aid. */
    int i;
    if (length < NTP_PACKET_MIN || length > NTP_PACKET_MAX) 
		return;
    for (i = 0; i < length; ++i) {
        if (i != 0 && i%32 == 0) {
            fprintf(stderr,"n");
        } else if (i != 0 && i%4 == 0) {
             fprintf(stderr," ");
		}
        fprintf(stderr,"%.2x",packet[i]);
    }
    fprintf(stderr,"n");
}

void pack_ntp (unsigned char *packet, int length, ntp_data *data)
{
	/* Pack the essential data into an NTP packet, bypassing struct layout and endian problems.  
		Note that it ignores fields irrelevant to SNTP. */
    int i, k;
    double d;
    memset(packet,0,length);
    packet[0] = (data->status<<6)|(data->version<<3)|data->mode;
    packet[1] = data->stratum;
    packet[2] = data->polling;
    packet[3] = data->precision;
    d = data->originate/NTP_SCALE;
    for (i = 0; i < 8; ++i) {
        if ((k = (int)(d *= 256.0)) >= 256) 
			k = 255;
        packet[NTP_ORIGINATE+i] = k;
        d -= k;
    }
    d = data->receive/NTP_SCALE;
    for (i = 0; i < 8; ++i) {
        if ((k = (int)(d *= 256.0)) >= 256) 
			k = 255;
        packet[NTP_RECEIVE+i] = k;
        d -= k;
    }
    d = data->transmit/NTP_SCALE;
    for (i = 0; i < 8; ++i) {
        if ((k = (int)(d *= 256.0)) >= 256) 
			k = 255;
        packet[NTP_TRANSMIT+i] = k;
        d -= k;
    }
}

void unpack_ntp (ntp_data *data, unsigned char *packet, int length) 
{
	/* Unpack the essential data from an NTP packet, bypassing struct layout and endian problems.  
		Note that it ignores fields irrelevant to SNTP. */
    int i;
    double d;
    data->current = current_time(JAN_1970);    /* Best to come first */
    data->status = (packet[0] >> 6);
    data->version = (packet[0] >> 3)&0x07;
    data->mode = packet[0]&0x07;
    data->stratum = packet[1];
    data->polling = packet[2];
    data->precision = packet[3];
    d = 0.0;
    for (i = 0; i < 4; ++i) 
		d = 256.0*d+packet[NTP_DISP_FIELD+i];
    data->dispersion = d/65536.0;
    d = 0.0;
    for (i = 0; i < 8; ++i) 
		d = 256.0*d+packet[NTP_REFERENCE+i];
    data->reference = d/NTP_SCALE;
    d = 0.0;
    for (i = 0; i < 8; ++i) 
		d = 256.0*d+packet[NTP_ORIGINATE+i];
    data->originate = d/NTP_SCALE;
    d = 0.0;
    for (i = 0; i < 8; ++i) 
		d = 256.0*d+packet[NTP_RECEIVE+i];
    data->receive = d/NTP_SCALE;
    d = 0.0;
    for (i = 0; i < 8; ++i) 
		d = 256.0*d+packet[NTP_TRANSMIT+i];
    data->transmit = d/NTP_SCALE;
}

void make_packet (ntp_data *data, int mode) 
{
	/* Create an outgoing NTP packet, either from scratch or starting from a request from a client.  Note that it
		implements the NTP specification, even when this is clearly insane, except possibly for the setting of LI. 
		It would be easy enough to add a sanity flag, but I am not in the business of designing an alternative 
		protocol (however much better it might be). */
    data->status = NTP_LI_FUDGE<<6;
    data->stratum = NTP_STRATUM;
    data->reference = data->dispersion = 0.0;
    if (mode == NTP_SERVER) {
        data->mode = (data->mode == NTP_CLIENT ? NTP_SERVER : NTP_PASSIVE);
        data->originate = data->transmit;
        data->receive = data->current;
    } else {
        data->version = NTP_VERSION;
        data->mode = mode;
        data->polling = NTP_POLLING;
        data->precision = NTP_PRECISION;
        data->receive = data->originate = 0.0;
    }
    data->current = data->transmit = current_time(JAN_1970);
}

int read_packet (ntp_data *data, double *off, double *err) 
{
	/* Check the packet and work out the offset and optionally the error.  Note that this contains more checking 
		than xntp does. This returns 0 for success, 1 for failure and 2 for an ignored broadcast packet (a kludge 
		for servers).  Note that it must not change its arguments if it fails. */
    unsigned char receive[NTP_PACKET_MAX+1];
    double delay1, delay2, x, y;
    int response = 0, failed, length, i, k;
/* Read the packet and deal with diagnostics. */
    if ((length = read_socket(receive,NTP_PACKET_MAX+1,waiting)) <= 0) 
		return 1;
    if (length < NTP_PACKET_MIN || length > NTP_PACKET_MAX) {
        if (verbose)
            fprintf(stderr,"%s: bad length %d for NTP packetn",argv0,length);
        return 1;
    }
    if (verbose > 1) {
        fprintf(stderr,"Incoming packet:n");
        display_packet(receive,length);
    }
    unpack_ntp(data,receive,length);
    if (verbose > 1) 
		display_data(data);
	/* Start by checking that the packet looks reasonable.  Be a little paranoid, but allow for version 1 semantics 
		and sick clients. */
    if (operation == op_server) {
        if (data->mode == NTP_BROADCAST) 
			return 2;
        failed = (data->mode != NTP_CLIENT && data->mode != NTP_ACTIVE);
    } else if (operation == op_listen)
        failed = (data->mode != NTP_BROADCAST);
    else {
        failed = (data->mode != NTP_SERVER && data->mode != NTP_PASSIVE);
        response = 1;
    }
    if (failed || data->status != 0 || data->version < 1 || data->version > NTP_VERSION_MAX || data->stratum > NTP_STRATUM_MAX) {
        if (verbose)
            fprintf(stderr,"%s: totally spurious NTP packet rejectedn",argv0);
        return 1;
    }
	/* Note that the conventions are very poorly defined in the NTP protocol, so we have to guess.  Any full NTP 
		server perpetrating  completely unsynchronised packets is an abomination, anyway, so reject it. */
    delay1 = data->transmit-data->receive;
    delay2 = data->current-data->originate;
    failed = ((data->stratum != 0 && data->reference == 0.0) || data->transmit == 0.0);
    if (response && (data->originate == 0.0 || data->receive == 0.0 || 
					(data->reference != 0.0 && data->receive < data->reference) ||
					delay1 < 0.0 || delay1 > NTP_INSANITY || delay2 <= 0.0 || data->dispersion > NTP_INSANITY))
        failed = 1;
    if (failed) {
        if (verbose)
            fprintf(stderr,"%s: incomprehensible NTP packet rejectedn",argv0);
        return 1;
    }
	/* If it is a response, check that it corresponds to one of our requests and has got here in a reasonable 
		length of time. */
    if (response) {
        k = 0;
        for (i = 0; i < attempts; ++i)
            if (data->originate == outgoing[i]) {
                outgoing[i] = 0.0;
                ++k;
            }
        if (k != 1 || delay2 > NTP_INSANITY) {
            if (verbose)
                fprintf(stderr,"%s: bad response from NTP server rejectedn", argv0);
            return 1;
        }
    }
	/* Now return the time information.  If it is a server response, it contains enough information that we can be
		almost certain that we have not been fooled too badly.  Heaven help us with broadcasts - make a wild kludge 
		here, and see elsewhere for other kludges. */
    if (dispersion < data->dispersion) 
		dispersion = data->dispersion;
    if (operation == op_listen) {
        *off = data->transmit-data->current;
        *err = NTP_INSANITY;
    } else {
        x = data->receive-data->originate;
        y = data->transmit-data->current;
        *off = 0.5*(x+y);
        *err = x-y;
        x = data->current-data->originate;
        if (0.5*x > *err) 
			*err = 0.5*x;
    }
    return 0;
}

void format_time (char *text, int length, double offset, double error, double drift, double drifterr) 
{
	/* Format the current time into a string, with the extra information as requested.  Note that the rest of the
		program uses the correction needed, which is what is printed for diagnostics, but this formats the error 
		in the local system for display to users.  So the results from this are the negation of those printed by 
		the verbose options. */
    int milli, len;
    time_t now;
    struct tm *gmt;
    static const char *months[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
	/* Work out and format the current local time.  Note that some semi-ANSI systems do not set the return value 
		from (s)printf. */
    now = convert_time(current_time(offset),&milli);
    errno = 0;
    if ((gmt = localtime(&now)) == NULL)
        fatal(1,"unable to work out local time",NULL);
    len = 24;
    if (length <= len) 
		fatal(0,"internal error calling format_time",NULL);
    errno = 0;
    sprintf(text,"%.4d %s %.2d %.2d:%.2d:%.2d.%.3d", gmt->tm_year+1900, months[gmt->tm_mon], gmt->tm_mday, 
				gmt->tm_hour, gmt->tm_min, gmt->tm_sec,milli);
    if (strlen(text) != len)
        fatal(1,"unable to format current local time",NULL);
	/* Append the information about the offset, if requested. */
    if (error >= 0.0) {
        if (length < len+30)
            fatal(0,"internal error calling format_time",NULL);
        errno = 0;
        sprintf(&text[len]," %c %.3f +/- %.3f secs",(offset > 0.0 ? '-' : '+'), (offset > 0.0 ? offset : -offset),dispersion+error);
        if (strlen(&text[len]) < 22)
            fatal(1,"unable to format clock correction",NULL);
    }
	/* Append the information about the drift, if requested. */
    if (drifterr >= 0.0) {
        len = strlen(text);
        if (length < len+25)
            fatal(0,"internal error calling format_time",NULL);
        errno = 0;
        sprintf(&text[len]," %c %.1f +/- %.1f ppm",
                (drift > 0.0 ? '-' : '+'),1.0e6*fabs(drift),
                1.0e6*drifterr);
        if (strlen(&text[len]) < 17)
            fatal(1,"unable to format clock correction",NULL);
    }
	/* It would be better to check for field overflow, but it is a lot of code to trap extremely implausible scenarios.  
		This will usually stop chaos from spreading. */
    if (strlen(text) >= length)
        fatal(0,"internal error calling format_time",NULL);
}

double reset_clock (double offset, double error, int daemon) 
{
	/* Reset the clock, if appropriate, and return the correction actually used. This contains most of the checking 
		for whether changes are worthwhile, except in daemon mode. */
    double absoff = (offset < 0 ? -offset : offset);
    char text[50];
	/* If the correction is large, ask for confirmation before proceeding. */
/*	if (absoff > maxerr) {
        if (! daemon && ftty(stdin) && ftty(stdout)) {
            printf("The time correction is %.3f +/- %.3f+%.3f secondsn",
                offset,dispersion,error);
            printf("Do you want to correct the time anyway? ");
            fflush(NULL);
            if (toupper(getchar()) != 'Y') {
                printf("OK - quittingn");
                fatal(0,NULL,NULL);
            }
        } else {
            sprintf(text,"%.3f +/- %.3f+%.3f",offset,dispersion,error);
            fatal(0,"time correction too large: %s seconds",text);
        }
    }
*/
	/* See if the correction is reasonably reliable and worth making. */
    if (absoff < (daemon ? 0.5 : 1.0)*minerr) {
        if (daemon ? verbose > 1 : verbose)
            fprintf(stderr,"%s: correction %.3f +/- %.3f+%.3f secs - ignoredn",
                argv0,offset,dispersion,error);
        return 0.0;
    } else if (absoff < 2.0*error) {
        if (daemon ? verbose > 1 : verbose)
            fprintf(stderr,
                "%s: correction %.3f +/- %.3f+%.3f secs - suppressedn",
                argv0,offset,dispersion,error);
        return 0.0;
    }
	/* Make the correction.  Provide some protection against the previous correction not having completed, but it 
		will rarely help much. */
    adjust_time(offset,(action == action_reset ? 1 : 0), (daemon ? 2.0*minerr : 0.0));
    if (daemon ? verbose > 1 : verbose) {
        format_time(text,50,0.0,-1.0,0.0,-1.0);
        fprintf(stderr,
            "%s: time changed by %.3f secs to %s +/- %.3f+%.3fn",
            argv0,offset,text,dispersion,error);
    }
    return offset;
}

void run_server (void) 
{
	/* Set up a socket, and either broadcast at intervals or wait for requests. It is quite tricky to get this to 
		fail, and it will usually indicate that the local system is sick. */
    unsigned char transmit[NTP_PACKET_MIN];
    ntp_data data;
    double started = current_time(JAN_1970), successes = 0.0, failures = 0.0,
        broadcasts = 0.0, weeble = 1.0, x, y;
    int i, j;
    open_socket(NULL,0);
    while (1) {
		/* In server mode, provide some tracing of normal running (but not too much, except when debugging!) */
        if (operation == op_server) {
            x = current_time(JAN_1970)-started;
            if (verbose && x/60.0+successes+failures >= weeble) {
                weeble *= WEEBLE_FACTOR;
                x -= 3600.0*(i = (int)(x/3600.0));
                x -= 60.0*(j = (int)(x/60.0));
                if (i > 0)
                    fprintf(stderr,"%s: after %d hours %d mins ",argv0,i,j);
                else if (j > 0)
                    fprintf(stderr,"%s: after %d mins %.0f secs ",argv0,j,x);
                else
                    fprintf(stderr,"%s: after %.1f secs ",argv0,x);
                fprintf(stderr,"%.0f acc. %.0f rej. %.0f b'castn",
                    successes,failures,broadcasts);
            }
			/* Respond to incoming requests or plaster broadcasts over the net.  Note that we could skip almost 
				all of the decoding, but it provides a healthy amount of error detection. */
            i = read_packet(&data,&x,&y);
            if (i == 2) {
                ++broadcasts;
                continue;
            } else if (i != 0) {
                ++failures;
                continue;
            } else {
                ++successes;
                make_packet(&data,NTP_SERVER);
            }
        } else {
            do_nothing(period);
            make_packet(&data,NTP_BROADCAST);
        }
        if (verbose > 1) {
            fprintf(stderr,"Outgoing packet:n");
            display_data(&data);
        }
        pack_ntp(transmit,NTP_PACKET_MIN,&data);
        if (verbose > 1) 
			display_packet(transmit,NTP_PACKET_MIN);
        write_socket(transmit,NTP_PACKET_MIN);
    }
}

double estimate_stats (int *a_total, int *a_index, data_record *record, double correction, double disp, 
					double *a_when, double *a_offset, double *a_error, double *a_drift, double *a_drifterr) 
{
	/* This updates the running statistics and returns the best estimate of what to do now.  It returns the time 
		stamp relevant to the correction.  If broadcasts are rare and the drift is large, it will fail - you should 
		then use a better synchronisation method.  It will also fail if something goes severely wrong (e.g. if the
		local clock is reset by another process or the transmission errors are beyond reason). */
    double weight, when, offset, error, drift, drifterr, now, w, x, y, z;
    int total = *a_total, index = *a_index, i;
    char text[50];
 
	/* Correct the previous data and store the new entry in the circular buffer. */
    for (i = 0; i < total; ++i) {
        record[i].when += correction;
        record[i].offset -= correction;
    }
    record[index].dispersion = disp;
    record[index].when = *a_when;
    record[index].offset = *a_offset;
    if (operation == op_listen) {
        record[index].error = minerr;
        record[index].weight = 1.0;
    } else {
        record[index].error = x = *a_error;
        record[index].weight = 1.0/(x > minerr ? x*x : minerr*minerr);
    }
    if (++index >= count) 
		index = 0;
    *a_index = index;
    if (++total > count) 
		total = count;
    *a_total = total;
    if (verbose > 1)
        fprintf(stderr,"corr=%.6f tot=%d ind=%dn",correction,total,index);
	/* If there is insufficient data yet, use the latest estimates and return forthwith.  Note that this will not 
		work for broadcasts, but they will be disabled in run_daemon(). */
    if ((operation == op_listen && total < count) || total < 3) {
        *a_drift = 0.0;
        *a_drifterr = -1.0;
        waiting = delay;
        return *a_when;
    }
	/* Work out the average time, offset, error etc.  Note that 'dispersion' is global and not subject to the 
		central limit theorem. Unfortunately, the variation in the source's dispersion is our only indication 
		of how consistent its clock is. */
    dispersion = weight = when = offset = y = 0.0;
    for (i = 0; i < total; ++i) {
        weight += w = record[i].weight;
        when += w*record[i].when;
        offset += w*record[i].offset;
        y += w*record[i].dispersion;
        if (dispersion < record[i].dispersion)
            dispersion = record[i].dispersion;
    }
    when /= weight;
    offset /= weight;
    y /= weight;
    if (verbose > 1)
        fprintf(stderr,"disp=%.6f wgt=%.3f when=%.6f off=%.6fn", dispersion,weight,when,offset);
	/* If there is enough data, estimate the drift and errors by regression.  Note that it is essential to calculate
		the mean square error, not the mean error. */
    error = drift = x = z = 0.0;
    for (i = 0; i < total; ++i) {
        w = record[i].weight/weight;
        x += w*(record[i].when-when)*(record[i].when-when);
        drift += w*(record[i].when-when)*(record[i].offset-offset);
        z += w*(record[i].offset-offset)*(record[i].offset-offset);
        error += w*record[i].error*record[i].error + 2.0*w*(record[i].dispersion-y)*(record[i].dispersion-y);
    }
    if (verbose > 1)
        fprintf(stderr,"X2=%.3f XY=%.6f Y2=%.9f E2=%.6f ",x,drift,z,error);
	/* When calculating the errors, add some paranoia mainly to check for coding errors and complete lunacy. There
		will be more thorough checks later. Note that we cannot usefully check the error for broadcasts. */
    z -= drift*drift/x;
    if (verbose > 1) 
		fprintf(stderr,"S2=%.9fn",z);
    if (operation == op_client) {
        if (z > error+dispersion*dispersion)
            fatal(0,"incompatible (i.e. erroneous) timestamps",NULL);
        else if (z > error && verbose)
            fprintf(stderr,"%s: anomalously high error %.3f > %.3fn",
                argv0,sqrt(z),sqrt(error));
    } else {
        if (z > maxerr*maxerr)
            fatal(0,"broadcasts too unreliable for time estimation",NULL);
    }
    drift /= x;
    drifterr = ABSCISSA*sqrt(z/(x*total));
    error = (operation == op_listen ? minerr : 0.0)+ABSCISSA*sqrt(z/total);
    if (verbose > 1)
        fprintf(stderr,"err=%.6f drift=%.6f+/-%.6fn",error,drift,drifterr);
    if (error+drifterr*delay > NTP_INSANITY)
        fatal(0,"unable to get a reasonable drift estimate",NULL);
	/* Estimate the optimal short-loop period, checking it carefully.  Remember to check that this whole process 
		is likely to be accurate enough and that the delay function may be inaccurate. */
    waiting = delay;
    x = (drift < 0.0 ? -drift : drift);
    if (x*delay < 0.5*minerr) {
        if (verbose > 1) fprintf(stderr,"Drift too small to correctn");
    } else if (x < 2.0*drifterr) {
        if (verbose > 1)
            fprintf(stderr,"Drift correction suppressedn");
    } else {
        if ((z = drifterr*delay) < 0.5*minerr) z = 0.5*minerr;
        waiting = (x < z/delay ? delay : (int)(z/x+0.5));
        waiting = (int)(delay/(int)(delay/(double)waiting+0.999)+0.999);
        if (waiting > delay)
            fatal(0,"internal error in drift calculation",NULL);
        if (drift*waiting > maxerr || waiting < RESET_MIN) {
            sprintf(text,"%.6f+/-%.6f",drift,drifterr);
            fatal(0,"drift correction too large: %s",text);
        }
    }
	/* Now work out what the correction should be, as distinct from what it should have been, remembering that 
		older times are less certain. */
    now = current_time(JAN_1970);
    x = now-when;
    offset += x*drift;
    error += x*drifterr;
    for (i = 0; i < total; ++i) {
        x = now-record[i].when;
        z = record[i].error+x*drifterr;
        if (z < error) {
            when = record[i].when;
            offset = record[i].offset+x*drift;
            error = z;
        }
    }
    if (verbose > 1)
        fprintf(stderr,"now=%.6f when=%.6f off=%.6f err=%.6f wait=%dn", now,when,offset,error,waiting);
	/* Finally, return the result. */
    *a_when = when;
    *a_offset = offset;
    *a_error = error;
    *a_drift = drift;
    *a_drifterr = drifterr;
    return now;
}

double correct_drift (double *a_when, double *a_offset, double drift) 
{
	/* Correct for the drift since the last time it was done, provided that a long enough time has elapsed.  And do
		remember to kludge up the time and discrepancy, when appropriate. */
    double d, x;
    d = current_time(JAN_1970) - *a_when;
    *a_when += d;
    x = *a_offset+d*drift;
    if (verbose > 1)
        fprintf(stderr, "Correction %.6f @ %.6f off=%.6f ", x, *a_when, *a_offset);
    if (d >= waiting && (x < 0.0 ? -x : x) >= 0.5*minerr) {
        if (verbose > 1) 
			fprintf(stderr,"performedn");
        adjust_time(x,(action == action_reset ? 1 : 0),0.5*minerr);
        *a_offset = 0.0;
        return x;
    } else {
        if (verbose > 1) 
			fprintf(stderr,"ignoredn");
        *a_offset = x;
        return 0.0;
    }
}

void handle_saving (int output, int *total, int *index, data_record *record, double *previous, double *when, double *correction) 
{
	/* This handles the saving and restoring of the state to a file.  While it is subject to spoofing, this is not 
		a major security problem. But, out of general paranoia, check everything in sight when restoring. Note that 
		this function has no external effect if something goes wrong. */
    struct {
        data_record record[COUNT_MAX];
        double previous, when, correction;
        int operation, delay, count, total, index, waiting;
    } buffer;
    double x, y;
    int i;
    if (savefile == NULL) 
		return;
	/* Read the restart file and print its data in diagnostic mode.  Note that some care is necessary to avoid 
		introducing a security exposure - but we trust the C library not to trash the stack on bad numbers! */
    if (! output) {
        if (fread(&buffer,sizeof(buffer),1,savefile) != 1 || ferror(savefile)) {
            if (ferror(savefile))
                fatal(1,"unable to read record from daemon save file",NULL);
            else if (verbose && ! feof(savefile))
                fprintf(stderr,"%s: bad daemon restart informationn",argv0);
            return;
        }
        if (verbose > 1) {
            fprintf(stderr,"prev=%.6f when=%.6f corr=%.6fn", buffer.previous,buffer.when,buffer.correction);
            fprintf(stderr,"op=%d dly=%d cnt=%d tot=%d ind=%d wait=%dn", buffer.operation, buffer.delay,
					buffer.count, buffer.total, buffer.index, buffer.waiting);
            if (buffer.total < COUNT_MAX)
                for (i = 0; i < buffer.total; ++i)
                    fprintf(stderr, "disp=%.6f wgt=%.3f when=%.6f off=%.6f err=%.6fn", buffer.record[i].dispersion, 
							buffer.record[i].weight, buffer.record[i].when, buffer.record[i].offset, buffer.record[i].error);
        }
		/* Start checking the data for sanity. */
        if (buffer.operation == 0 && buffer.delay == 0 &&buffer.count == 0) {
            if (verbose)
                fprintf(stderr,"%s: restarting from a cleared filen",argv0);
            return;
        }
        if (buffer.operation != operation || buffer.delay != delay ||
                buffer.count != count) {
            if (verbose)
                fprintf(stderr,"%s: different parameters for restartn",argv0);
            return;
        }
        if (buffer.total < 1 || buffer.total > count || buffer.index < 0 || buffer.index >= count ||
				buffer.correction < -maxerr || buffer.correction > maxerr || buffer.waiting < RESET_MIN ||
				buffer.waiting > delay || buffer.previous > buffer.when || 
				buffer.previous < buffer.when-count*delay || buffer.when >= *when) {
            if (verbose)
                fprintf(stderr,"%s: corrupted restart informationn",argv0);
            return;
        }
		/* Checking the record is even more tedious. */
        x = *when;
        y = 0.0;
        for (i = 0; i < buffer.total; ++i) {
            if (buffer.record[i].dispersion < 0.0 || buffer.record[i].dispersion > maxerr ||
                    buffer.record[i].weight <= 0.0 || buffer.record[i].weight > 1.001/(minerr*minerr) ||
                    buffer.record[i].offset < -maxerr || buffer.record[i].offset > maxerr ||
                    buffer.record[i].error < 0.0 || buffer.record[i].error > maxerr) {
                if (verbose)
                    fprintf(stderr,"%s: corrupted restart recordn",argv0);
                return;
            }
            if (buffer.record[i].when < x) 
				x = buffer.record[i].when;
            if (buffer.record[i].when > y) 
				y = buffer.record[i].when;
        }
		/* Check for consistency and, finally, whether this is too old. */
        if (y > buffer.when || y-x < (buffer.total-1)*delay || y-x > (buffer.total-1)*buffer.total*delay) {
            if (verbose)
                fprintf(stderr,"%s: corrupted restart timesn",argv0);
            return;
        }
        if (buffer.when < *when-count*delay) {
            if (verbose)
                fprintf(stderr,"%s: restart information too oldn",argv0);
            return;
        }
		/* If we get here, just copy the data back.  It is a good idea to trash the restart file, because there 
			are further checks in estimate_stats() and a failure there could cause the restarts to loop until 
			the data time out. */
        memcpy(record,buffer.record,sizeof(buffer.record));
        *previous = buffer.previous;
        *when = buffer.when;
        *correction = buffer.correction;
        *total = buffer.total;
        *index = buffer.index;
        waiting = buffer.waiting;
        memset(&buffer,0,sizeof(buffer));
        if (fseek(savefile,0l,SEEK_SET) != 0 || fwrite(&buffer,sizeof(buffer),1,savefile) != 1 || 
				fflush(savefile) != 0 || ferror(savefile))
            fatal(1,"unable to clear daemon save file",NULL);
	/* All errors on output are fatal. */
    } else {
        memcpy(buffer.record,record,sizeof(buffer.record));
        buffer.previous = *previous;
        buffer.when = *when;
        buffer.correction = *correction;
        buffer.operation = operation;
        buffer.delay = delay;
        buffer.count = count;
        buffer.total = *total;
        buffer.index = *index;
        buffer.waiting = waiting;
        if (fseek(savefile,0l,SEEK_SET) != 0 || fwrite(&buffer,sizeof(buffer),1,savefile) != 1 || 
				fflush(savefile) != 0 || ferror(savefile))
            fatal(1,"unable to write record to daemon save file",NULL);
    }
}

void run_daemon (char *hostname) 
{
	/* This does not adjust the time between calls to the server, but it does adjust the time between clock resets. 
		This function will survive short periods of server inaccessibility or network glitches, but not long ones, 
		and will then need restarting manually. It is far too complex for a single function, but could only be 
		simplified by making most of its variables global.  Oh, for nested scopes as in Algol 68! */
    double history[COUNT_MAX], started, previous, when, correction = 0.0, weeble = 1.0, accepts = 0.0, 
			rejects = 0.0, flushes = 0.0, replicates = 0.0, skips = 0.0, offset = 0.0, error = -1.0, 
			drift = 0.0, drifterr = -1.0, maxoff = 0.0, x;
    data_record record[COUNT_MAX];
    int total = 0, index = 0, item = 0, rej_level = 0, rep_level = 0, retry = 1, i, j, k;
    unsigned char transmit[NTP_PACKET_MIN];
    ntp_data data;
    char text[100];
	/* After initialising, restore from a previous run if possible.  Note that only a few of the variables are 
		actually needed to control the operation and the rest are mainly for diagnostics. */
    if (action != action_display) {
        set_lock(1);
        locked = 1;
    }
    for (i = 0; i < count; ++i) 
		history[i] = 0.0;
    attempts = 0;
    open_socket(hostname,delay);
    started = previous = when = current_time(JAN_1970);
    handle_saving(0,&total,&index,record,&previous,&when,&correction);
    if (total > 0 && started-previous < delay) 
		retry = 0;
    if (verbose > 1)
        fprintf(stderr,"start=%.6f prev=%.6f when=%.6f retry=%dn",
            started,previous,when,retry);
    while (1) {
		/* Print out a reasonable amount of diagnostics, rather like a server.  Note that it may take a little time, 
			but shouldn't affect the estimates much. */
        if (verbose > 1) fprintf(stderr,"item=%d rej=%dn",item,rej_level);
        x = current_time(JAN_1970)-started;
        if (verbose && x/60.0 + accepts + rejects + flushes + replicates + skips >= weeble) {
            weeble *= WEEBLE_FACTOR;
            x -= 3600.0*(i = (int)(x/3600.0));
            x -= 60.0*(j = (int)(x/60.0));
            if (i > 0)
                fprintf(stderr,"%s: after %d hours %d mins ",argv0,i,j);
            else if (j > 0)
                fprintf(stderr,"%s: after %d mins %.0f secs ",argv0,j,x);
            else
                fprintf(stderr,"%s: after %.1f secs ",argv0,x);
            fprintf(stderr,"acc. %.0f rej. %.0f flush %.0f", accepts,rejects,flushes);
            if (operation == op_listen)
                fprintf(stderr," rep. %.0f skip %.0f",replicates,skips);
            fprintf(stderr," max.off. %.3f corr. %.3fn",maxoff,correction);
            format_time(text,100,offset,error,drift,drifterr);
            fprintf(stderr,"%s: %sn",argv0,text);
            maxoff = 0.0;
        }
		/* Listen for the next broadcast packet.  This allows up to ETHERNET_MAX replications per packet, for systems 
			with multiple addresses for receiving broadcasts; the only reason for a limit is to protect against broken
			NTPservers always returning the same time. */
        if (operation == op_listen) {
            flushes += flush_socket();
            if (read_packet(&data,&offset,&error)) {
                ++rejects;
                if (++rej_level > count)
                    fatal(0,"too many bad or lost packets",NULL);
                if (action != action_display && drifterr >= 0.0) {
                    correction += correct_drift(&when,&offset,drift);
                    handle_saving(1,&total,&index,record,&previous,&when, &correction);
                }
                continue;
            }
            if ((rej_level -= (count < 5 ? count : 5)) < 0) rej_level = 0;
            x = data.transmit;
            for (i = 0; i < count; ++i)
                if (x == history[i]) {
                    ++replicates;
                    if (++rep_level > ETHERNET_MAX)
                        fatal(0,"too many replicated packets",NULL);
                    goto continue2;
                }
            rep_level = 0;
            history[item] = x;
            if (++item >= count) 
				item = 0;
			/* Accept a packet only after a long enough period has elapsed. */
            when = data.current;
            if (! retry && when < previous+delay) {
                ++skips;
                continue;
            }
            retry = 0;
            if (verbose > 1)
                fprintf(stderr,"Offset=%.6f @ %.6f disp=%.6fn", offset,when,dispersion);
			/* Handle the client/server model.  It keeps a record of transmitted times, mainly out of paranoia.  
				The waiting time is kludged up to attempt to provide reasonable resilience against both lost 
				packets and dead servers.  But it won't handle much of either, and will stop after a while, needing
				manual restarting. */
        } else {
            if (! retry) 
				do_nothing(waiting);
            flushes += flush_socket();
            make_packet(&data,NTP_CLIENT);
            outgoing[item] = data.transmit;
            if (++item >= 2*count) 
				item = 0;
            if (attempts < 2*count) 
				++attempts;
            if (verbose > 1) {
                fprintf(stderr,"Outgoing packet:n");
                display_data(&data);
            }
            pack_ntp(transmit,NTP_PACKET_MIN,&data);
            if (verbose > 1) 
				display_packet(transmit,NTP_PACKET_MIN);
            write_socket(transmit,NTP_PACKET_MIN);
			/* Read the packet and check that it is an appropriate response.  Because this is rather more numerically 
				sensitive than simple resynchronisation, reject all very inaccurate packets.  Be careful if you modify
				this, because the error handling is rather nasty to avoid replicating code. */
            k = read_packet(&data,&offset,&error);
            if (! k)
                when = (data.originate+data.current)/2.0;
            else if (action != action_display && drifterr >= 0.0) {
                correction += correct_drift(&when,&offset,drift);
                handle_saving(1,&total,&index,record,&previous,&when, &correction);
            }
            if (! k && ! retry && when < previous+delay-2) {
                if (verbose)
                    fprintf(stderr,"%s: packets out of ordern",argv0);
                k = 1;
            }
            if (! k && data.current-data.originate > 5.0*maxerr) {
                if (verbose)
                    fprintf(stderr,"%s: very slow response rejectedn",argv0);
                k = 1;
            }
			/* Count the number of rejected packets. */
            if (k) {
                ++rejects;
                if (++rej_level > count)
                    fatal(0,"too many bad or lost packets",NULL);
                else {
                    retry = 1;
                    continue;
                }
            } else
                retry = 0;
            if ((rej_level -= (count < 5 ? count : 5)) < 0) 
				rej_level = 0;
            if (verbose > 1)
                fprintf(stderr,"Offset=%.6f+/-%.6f @ %.6f disp=%.6fn", offset,error,when,dispersion);
        }
		/* Calculate the statistics, and display the results or make the initial correction.  Note that 
			estimate_stats() also updates waiting. */
        ++accepts;
        previous = when = estimate_stats(&total, &index, record, correction, data.dispersion, &when, 
										 &offset, &error, &drift, &drifterr);
        x = (maxoff < 0.0 ? -maxoff : maxoff);
        if ((offset < 0.0 ? -offset : offset) > x) 
			maxoff = offset;
        correction = 0.0;
        if (operation == op_client || accepts >= count) {
            if (action == action_display) {
                format_time(text,100,offset,error,drift,drifterr);
                printf("%sn",text);
            } else {
                x = reset_clock(offset,error,1);
                correction += x;
                offset -= x;
            }
        } else
            waiting = delay;
        handle_saving(1,&total,&index,record,&previous,&when,&correction);
		/* Now correct the clock for a while, before getting another packet and updating the statistics. */
        while (when < previous+delay-waiting) {
            do_nothing(waiting);
            if (action == action_display)
                when += waiting;
            else {
                correction += correct_drift(&when,&offset,drift);
                handle_saving(1,&total,&index,record,&previous,&when, &correction);
            }
        }
continue2: ;
    }
}

void run_client (char *hostname) 
{
	/* Get enough responses to do something with; or not, as the case may be.  Note that it allows for half of 
		the packets to be bad, so may make up to twice as many attempts as specified by the -c value.  The 
		deadline checking is merely paranoia, to protect against broken signal handling - it cannot easily be 
		triggered if the signal handling works. */
    double history[COUNT_MAX], guesses[COUNT_MAX], offset, error, deadline, a, b, x, y;
    int accepts = 0, rejects = 0, flushes = 0, replicates = 0, k;
    unsigned char transmit[NTP_PACKET_MIN];
    ntp_data data;
    char text[75];
    if (action != action_display) {
        set_lock(1);
        locked = 1;
    }
    attempts = 0;
    deadline = current_time(JAN_1970) + delay;
    open_socket(hostname,delay);
    flushes = flush_socket();
	/* Listen to broadcast packets and select the best (i.e. earliest).  This will be sensitive to a bad NTP 
		broadcaster, but I believe such things are very rare in practice.  In any case, if you have one, it 
		is probably the only one on your subnet, so you are knackered!  This allows up to ETHERNET_MAX 
		replications per packet, for systems with multiple addresses for receiving broadcasts; the only reason 
		for a limit is to protect against broken NTP servers always returning the same time. */
    if (operation == op_listen) {
        while (accepts < count) {
            if (current_time(JAN_1970) > deadline)
                fatal(0,"not enough valid broadcasts received in time",NULL);
            if (read_packet(&data,&x,&y)) {
                if (++rejects > count)
                    fatal(0,"too many bad or lost packets",NULL);
                else
                    continue;
            } else {
                a = data.transmit;
                for (k = 0; k < accepts; ++k)
                    if (a == history[k]) {
                        if (++replicates > ETHERNET_MAX*count)
                            fatal(0,"too many replicated packets",NULL);
                        goto continue2;
                    }
                history[accepts] = a;
                guesses[accepts++] = x;
            }
            if (verbose > 1)
                fprintf(stderr,"Offset=%.6f+/-%.6f disp=%.6fn",x,y,dispersion);
			/* Note that bubblesort IS a good method for this amount of data.  */
            for (k = accepts-2; k >= 0; --k)
                if (guesses[k] < guesses[k+1])
                    break;
                else {
                    x = guesses[k];
                    guesses[k] = guesses[k+1];
                    guesses[k+1] = x;
                }
continue2:  ;
        }
        offset = guesses[0];
        error = minerr+guesses[count <= 5 ? count-1 : 5]-offset;
        if (verbose > 1)
            fprintf(stderr,"accepts=%d rejects=%d flushes=%d replicates=%dn", accepts,rejects,flushes,replicates);
		/* Handle the client/server model.  It keeps a record of transmitted times, mainly out of paranoia. */
    } else {
        offset = 0.0;
        error = NTP_INSANITY;
        while (accepts < count && attempts < 2*count) {
            if (current_time(JAN_1970) > deadline)
                fatal(0,"not enough valid responses received in time",NULL);
            make_packet(&data,NTP_CLIENT);
            outgoing[attempts++] = data.transmit;
            if (verbose > 1) {
                fprintf(stderr,"Outgoing packet:n");
                display_data(&data);
            }
            pack_ntp(transmit,NTP_PACKET_MIN,&data);
            if (verbose > 1) 
				display_packet(transmit,NTP_PACKET_MIN);
            write_socket(transmit,NTP_PACKET_MIN);
            if (read_packet(&data,&x,&y)) {
                if (++rejects > count)
                    fatal(0,"too many bad or lost packets",NULL);
                else
                    continue;
            } else
                ++accepts;
			/* Work out the most accurate time, and check that it isn't more accurate than the results warrant. */
            if (verbose > 1)
                fprintf(stderr,"Offset=%.6f+/-%.6f disp=%.6fn",x,y,dispersion);
            if ((a = x-offset) < 0.0) 
				a = -a;
            if (accepts <= 1) 
				a = 0.0;
            b = error+y;
            if (y < error) {
                offset = x;
                error = y;
            }
            if (verbose > 1)
                fprintf(stderr,"best=%.6f+/-%.6fn",offset,error);
            if (a > b) 
				fatal(0,"inconsistent times got from NTP server",NULL);
            if (error <= minerr) break;
        }
        if (verbose > 1)
            fprintf(stderr,"accepts=%d rejects=%d flushes=%dn",
                accepts,rejects,flushes);
    }
	/* Tidy up the socket, issues diagnostics and perform the action. */
    close_socket();
    if (accepts == 0) fatal(0,"no acceptable packets received",NULL);
    if (error > NTP_INSANITY)
        fatal(0,"unable to get a reasonable time estimate",NULL);
    if (verbose > 1)
        fprintf(stderr,"Correction: %.6f +/- %.6f disp=%.6fn", offset,error,dispersion);
    if (action == action_display) {
        format_time(text,75,offset,error,0.0,-1.0);
        printf("%sn",text);
    } else
        (void)reset_clock(offset,error,0);
    if (locked) 
		set_lock(0);
    return;
}

int msntp_main (int argc, char *argv[]) 
{
	/* This is the entry point and all that.  It decodes the arguments and calls one of the specialised routines 
		to do the work. */
    char *hostname = NULL, *savename = NULL;
    int daemon = 0, k;
    char c;
    argv0 = argv[0];
    if (argv0 == NULL || argv0[0] == '') 
		argv0 = "msntp";
    setvbuf(stdout,NULL,_IOLBF,BUFSIZ);
    setvbuf(stderr,NULL,_IOLBF,BUFSIZ);
    if (INT_MAX < 2147483647) 
		fatal(0,"msntp assumes 32-bit ints",NULL);
    if (DBL_EPSILON > 1.0e-13)
        fatal(0,"msntp assumes doubles with eps < 1.0e-13",NULL);
	/* Decode the arguments. */
    while (argc > 1) {
        k = 1;
        if (strcmp(argv[1],"-B") == 0 && action == 0) {
            action = action_broadcast;
            if (argc > 2) {
                if (sscanf(argv[2],"%d%c",&period,&c) != 1) 
					syntax();
                if (period < 1 || period > 1440)
                    fatal(0,"%s option value out of range","-B");
                period *= 60;
                k = 2;
            } else
                period = 60*60;
        } else if (strcmp(argv[1],"-S") == 0 && action == 0) {
            action = action_server;
        } else if (strcmp(argv[1],"-r") == 0 && action == 0){
            action = action_reset;
        } else if (strcmp(argv[1],"-a") == 0 && action == 0){
            action = action_adjust;
        } else if (strcmp(argv[1],"-x") == 0 && daemon == 0) {
            if (argc > 2 && sscanf(argv[2],"%d%c",&daemon,&c) == 1) {
                if (daemon < 1 || daemon > 1440)
                    fatal(0,"%s option value out of range","-x");
                k = 2;
            } else
                daemon = 300;
        } else if (strcmp(argv[1],"-f") == 0 && savename == NULL && argc > 2) {
            savename = argv[2];
            k = 2;
        } else if (strcmp(argv[1],"-v") == 0 && verbose == 0)
            verbose = 1;
        else if (strcmp(argv[1],"-V") == 0 && verbose == 0)
            verbose = 2;
        else if (strcmp(argv[1],"-e") == 0 && minerr == 0.0 && argc > 2) {
            if (sscanf(argv[2],"%lf%c",&minerr,&c) != 1) 
				syntax();
            if (minerr <= 0.000999999 || minerr > 1.0)
                fatal(0,"%s option value out of range","-e");
            k = 2;
        } else if (strcmp(argv[1],"-E") == 0 && maxerr == 0.0 && argc > 2) {
            if (sscanf(argv[2],"%lf%c",&maxerr,&c) != 1) 
				syntax();
            if (maxerr <= 0.0999999 || maxerr > 60.0)
                fatal(0,"%s option value out of range","-E");
            k = 2;
        } else if (strcmp(argv[1],"-d") == 0 && delay == 0 && argc > 2) {
            if (sscanf(argv[2],"%d%c",&delay,&c) != 1) 
				syntax();
            if (delay < 1 || delay > 3600)
                fatal(0,"%s option value out of range","-d");
            k = 2;
        } else if (strcmp(argv[1],"-c") == 0 && count == 0 && argc > 2) {
            if (sscanf(argv[2],"%d%c",&count,&c) != 1) 
				syntax();
            if (count < 1 || count > COUNT_MAX)
                fatal(0,"%s option value out of range","-c");
            k = 2;
        } else
            break;
        argc -= k;
        argv += k;
    }
	/* Check the arguments for consistency and set the defaults. */
    if (action == action_broadcast || action == action_server) {
        operation = (action == action_server ? op_server : op_broadcast);
        if (argc != 1 || minerr != 0.0 || maxerr != 0.0 || count != 0 || delay != 0 || daemon != 0)
            syntax();
    } else {
        if (argc < 1 || argc > 2 || (daemon != 0 && delay != 0))
            syntax();
        if (argc == 2) {
            if (argv[1][0] == '-')
                fatal(0,"invalid Internet address '%s'",argv[1]);
            operation = op_client;
            hostname = argv[1];
        } else {
            operation = op_listen;
            hostname = NULL;
        }
        if (action == 0) 
			action = action_display;
        if (minerr == 0) 
			minerr = (operation == op_listen ? 0.5 : 0.1);
        if (maxerr == 0) 
			maxerr = 5.0;
        if (count == 0) 
			count = 5;
        if ((argc == 1 || daemon != 0) && count < 5)
            fatal(0,"at least 5 packets needed in this mode",NULL);
		/* The '-x' option changes the implications of many other settings, though this is not usually apparent
			to the caller.  Most of the time delays are to ensure that stuck states terminate, and do not affect
			the result. */
        if (daemon != 0) {
            if (minerr >= maxerr || maxerr >= daemon)
                fatal(0,"values not in order -e < -E < -x",NULL);
            waiting = delay = daemon *= 60;
        } else {
            if (savename != NULL)
                fatal(0,"-f can be specified only as an option to -x",NULL);
            if (delay == 0) 
				delay = (operation == op_listen ? 300 : 15);
            if (operation == op_listen) {
                if (minerr >= maxerr || maxerr > delay/count)
                    fatal(0,"values not in order -e < -E < -d/-c",NULL);
            } else {
                if (minerr >= maxerr || maxerr > delay)
                    fatal(0,"values not in order -e < -E < -d",NULL);
            }
            if (2*count >= delay) 
				fatal(0,"-c must be less than half -d",NULL);
            waiting = delay/count;
        }
    }
	/* Diagnose where we are, if requested, and separate out the classes of operation. The calls do not return. */
    if (verbose > 1) {
        fprintf(stderr,"Options: a=%d p=%d v=%d e=%.3f E=%.3f d=%d c=%d ",
            action,period,verbose,minerr,maxerr,delay,count);
        fprintf(stderr,"x=%d op=%d %sn",
            daemon,operation,(operation == op_client ? hostname : ""));
    }
    if (operation == op_server || operation == op_broadcast) {
        run_server();
    } else if (daemon != 0) {
        if (savename != NULL && (savefile = fopen(savename,"rb+")) == NULL && (savefile = fopen(savename,"wb+")) == NULL)
            fatal(0,"unable to open the daemon save file",NULL);
        run_daemon(hostname);
    } else {
        run_client(hostname);
	}
    return EXIT_FAILURE;
}


