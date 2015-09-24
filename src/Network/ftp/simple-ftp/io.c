/* zftp: File I/O */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <setjmp.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "func.h"
#ifdef TERMIOS
#include <termios.h>
#else
#ifdef SGTTYB
#include <sgtty.h>
#else
#include <termio.h>
#endif
#endif
#define PASSWORD_LENGTH 256
#ifdef TERMIOS
struct termios oldtty, noecho;
#else
#ifdef SGTTYB
struct sgttyb oldtty, noecho;
#else
struct termio oldtty, noecho;
#endif
#endif
extern int hControlSocket,
   hDataSocket,
   bMode;
extern char tmp_buffer[1024];
static ControlCHit=0;
jmp_buf abortfile;
void signal_handler(int unused)
{
   ControlCHit = 1;
   longjmp(abortfile,1); 
}
/* called to retrive a file from remote host */
void getfile( char *fname)
{
   FILE *fp=NULL;
   int fd, nTotal=0, nBytesRead=0, retval, aborted=0;
   char *abortstr = "ABORrn", ch;
   if( !fname || ! (*fname)) {
      printf("No file specified.n");
      return;
   }
   /*
    * open the file with current mode
    */
   if(! (fp=fopen(fname,(bMode==ASCII) ? "wt" : "wb"))) {
      perror("file open");
      return;
   }
   /*
    * obtain a listen socket
    */
   if( get_listen_socket() < 0) {
       fclose(fp);
       return;
   }
   
   /*
    * send command to server and read response
    */
   sprintf(tmp_buffer,"RETR %srn",fname);
   if(!send_ctrl_msg(tmp_buffer,strlen(tmp_buffer))) {
      fclose(fp);
      return;
   }
   int l= get_host_reply();
if(l==550)return;
   
   /*
    * accept server connection
    */
   if( accept_connection() <= 0) {
       fclose(fp);
       return;
   }
   /* 
    * now get file and store
    */
   
   fd = fileno(fp);
   printf("Type q and hit return to abortrn");
   while( (nBytesRead=data_msg(tmp_buffer,1024)) > 0) {
       
   
   write(fd,tmp_buffer,nBytesRead);
   nTotal+=nBytesRead;
   printf("%s : %d receivedr",fname,nTotal);
   if( check_input() ) {
        ch = getchar();
        if( ch != 'n') {
        while( getchar() != 'n') ;      /* read 'til new line */
        }
        if( ch == 'q') 
        aborted = 1;
   }
   
   /*
    * did we abort?
    */
   if( aborted ) {
  
   printf("rnAbort: Waiting for server to finish.");
   send_ctrl_msg(abortstr,strlen(abortstr));
   break;
   }
   }
   if( aborted ) {         // ignore everything if aborted.
   while( (nBytesRead=data_msg(tmp_buffer,1024)) > 0);
   get_host_reply();
   }
 /*  (void)signal(SIGINT,OldHandler); */
   printf("rn");
   close(fd);
   close_data_connection(hDataSocket);
 /*/  ControlCHit = 0; */
   get_host_reply();
}
/*
 * put_file
 */
void put_file( char *fname)
{
   FILE *fp=NULL;
   int fd, nTotal=0, nBytesRead=0, retval, aborted=0;
   char *abortstr = "ABORrn", ch;
  /* void (*OldHandler)(int); */
   if( !fname || ! (*fname)) {
      printf("No file specified.n");
      return;
   }
   if(! (fp=fopen(fname,(bMode==ASCII) ? "rt" : "rb"))) {
      perror("file open");
      return;
   }
   if( get_listen_socket() < 0) {
       fclose(fp);
       return;
   }
   
   /*
    * send command to server & read reply
    */
   sprintf(tmp_buffer,"STOR %srn",fname);
   if(!send_ctrl_msg(tmp_buffer,strlen(tmp_buffer))) {
      fclose(fp);
      return;
   }
   int m=get_host_reply();
   if(m==550)
return;
   /*
    * accept server connection
    */
   if( accept_connection() <= 0) {
       fclose(fp);
       return;
   }
   /* 
    * now send file
    */
   
   fd = fileno(fp);
   printf("Type q and hit return to abortrn");
   while( (nBytesRead=read(fd,tmp_buffer,1024)) > 0) {
      send_data_msg(tmp_buffer,nBytesRead);
      nTotal+=nBytesRead;
      printf("%s : %d sentr",fname,nTotal);
   if( check_input() ) {
        ch = getchar();
        if( ch != 'n') {
        while( getchar() != 'n') ;      /* read 'til new line */
        }
        if( ch == 'q') 
        aborted = 1;
   }
   /*
    * send an abort command to server if we aborted.
    */
   if( aborted ) {
  
   printf("rnAbort: Waiting for server to finish.");
   send_ctrl_msg(abortstr,strlen(abortstr));
   break;
   }
   }
   /*(void)signal(SIGINT,OldHandler); */
   printf("rn");
   /*
    * close data connection
    */
   close_data_connection(hDataSocket);
   close(fd);
   get_host_reply();
}
/*
 * turn_off_echo
 */
void turn_off_echo()
{
    int fd = fileno(stdin);
           /*
    * first, get the current settings.
    * then turn off the ECHO flag.
    */
    #ifdef TERMIOS
    if (tcgetattr(fd, &oldtty) < 0)
        perror("tcgetattr");
    noecho = oldtty;
    noecho.c_lflag &= ~ECHO;
    #else
    #ifdef SGTTYB
    if (ioctl(fd, TIOCGETP, &oldtty) < 0)
        perror("ioctl");
    noecho = oldtty;
    noecho.sg_flags &= ~ECHO;
    #else
    if (ioctl(fd, TCGETA, &oldtty) < 0)
        perror("ioctl");
    noecho = oldtty;
    noecho.c_lflag &= ~ECHO;
    #endif
    #endif
    /*
     * now set the current tty to this setting
     */
    #ifdef TERMIOS
    if (tcsetattr(fd, TCSAFLUSH, &noecho) < 0)
            perror("tcsetattr");
    #else
    #ifdef SGTTYB
    if (ioctl(fd, TIOCSETP, &noecho) < 0)
        perror("ioctl");
    #else
    if (ioctl(fd, TCSETA, &noecho) < 0)
        perror("ioctl");
    #endif
    #endif  /* !TERMIOS */
}
/*
 * start_echo
 * turn echoing back on. (UNIX only )
 */
void start_echo()
{
    int fd = fileno(stdin);
    /*
     * set tty back to it's original state
     */
    #ifdef TERMIOS
    if (tcsetattr(fd, TCSAFLUSH, &oldtty) < 0)
        perror("tcsetattr");
    #else
    #ifdef SGTTYB
    if (ioctl(fd, TIOCSETP, oldtty) < 0)
        perror("ioctl");
    #else
    if (ioctl(fd, TCSETA, &oldtty) < 0)
        perror("ioctl");
    #endif
    #endif  /* !TERMIOS */
}
/*
 * getpassword
 * read in the user's password.  turn off echoing before reading.
 * then turn echoing back on.
 */
void getpassword( char *ftp_pass )
{
     
     turn_off_echo();
     (void)printf("Password:");
     (void)fgets(ftp_pass,PASSWORD_LENGTH,stdin);
     (void)strtok(ftp_pass,"n");
     start_echo(stdin,1);
     (void)printf("rn");
}
/*
 * get_unix_input
 * function called to get user input on the UNIX side.  
 */
int get_unix_input(char *command)
{
    char ch;
    int i;
    while( (ch=getchar()) == ' ' || ch == 't') ;
    if( ch != 'n') {
        command[0] = ch;
        fgets(&command[1],1024,stdin);
        strtok(command,"n");
        i = strlen(command) - 1;
        while( i>0 && isspace(command[i]))
           i--;
        if( i>= 0)
           command[i+1] = 0;
    }
    return 1;
}
