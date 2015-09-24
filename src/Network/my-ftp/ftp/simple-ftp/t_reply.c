/*This t_reply.c was according as RFC 765 - File Transfer Protocol specification
* parse the meaning of the remote's reply number
*/
#include <stdio.h>
void TranslateReply(int k)
{
    switch(k){

    case 110:
    printf("Restart marker reply.n");
    break;

    case 119:
    printf("Terminal not available, will try mailbox.n");
    break;
    case 120:
    printf("Service ready in nnn minutes.n");
    break;

    case 125:
    printf("Data connection already open; transfer starting.n");
    break;
    case 150:
    printf("File status okay; about to open data connection.n");
    break;

    case 151:
    printf("User not local; Will forward to <user>@<host>.n");
    break;

    case 152:
    printf("User Unknown; Mail will be forwarded by the operator.n");
    break;

    case 200:
    printf("Command okay.n");
    break;

    case 202:
    printf("Command not implemented, superfluous at this site.n");
    break;

    case 211:
    printf("System status, or system help reply.n");
    break;

    case 212:
    printf("Directory status.n");
    break;

    case 213:
    printf("File status.n");
    break;

    case 214:
    printf("Help message.n");
    break;

    case 215:
    printf("<scheme> is the preferred scheme.n");
    break;

    case 220:
    printf("Service ready for new user.n");
    break;
    case 221:
    printf("Service closing TELNET connection.n");
    break;

    case 225:
    printf("Data connection open; no transfer in progress.n");
    break;
    case 226:
    printf("Closing data connection;requested file action successful.n");
    break;
    case 227:
    printf("Entering Passive Mode.n");
    break;
    case 230:
    printf("User logged in, proceed.n");
    break;
    case 250:
    printf("Requested file action okay, completed.n");
    break;
    case 331:
    printf("User name okay, need passwordn");
    break;
    case 332:
    printf("Need account for loginn");
    break;
    case 350:
    printf("Requested file action pending further information.n");
    break;
    case 354:
    printf("Start mail input; end with <CR><LF>.<CR><LF>.n");
    break;
    case 421:
    printf("Service not available, closing TELNET connection.n");
    break;
    case 425:
    printf("Can't open data connection.n");
    break;
    case 426:
    printf("flag_connection closed; transfer aborted.n");
    break;
    case 450:
    printf("Requested file action not taken: file unavailable.n");
    break;
    case 451:
    printf("Requested action aborted: local error in processing.n");
    break;
    case 452:
    printf("Requested action not taken: insufficient storage space in system.n");
    break;
    case 500:
    printf("Syntax error, command unrecognized.n");
    break;
    case 501:
    printf("Syntax error in parameters or arguments.n");
    break;
    case 502:
    printf("Command not implemented.n");
    break;
    case 503:
    printf("Bad sequence of commands.n");
    break;
    case 504:
    printf("Command not implemented for that parameter.n");
    break;
    case 530:
    printf("Not logged in.n");
    break;
    case 532:
    printf("Need account for storing files.n");
    break;
    case 550:
    printf("Requested action not taken: file unavailable.n");
    break;
    case 551:
    printf("Requested action aborted: page type unknown.n");
    break;
    case 552:
    printf("Requested file action aborted: exceeded storage allocationn");
    break;
    case 553:
    printf("Requested action not taken: file name not allowedn");
    break;
    default:
    printf("There is no this reply number.n");
}
return;
}
