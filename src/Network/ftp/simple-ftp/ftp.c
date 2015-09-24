/*
* Copyright (c) 2005
* All rights reserved.
*
* software£ºzftp
* FTP client
*
* ver£º1.1
* Xiaoliang Zhang  
* date:2005.3.10
*
* replace ver£º1.09
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "ftp.h"
#include "func.h"


extern void TranslateReply(int k);
int ReadCommand, flag_connected;
char tmp_buffer[1025]={0};  /* buffer used to read/write */
char ftp_user[20];          /* stores username */
char ftp_pass[256];         /* stores user password */
int  k;

int check_cmd(char *command)
{
    if( !strncmp(command,"ls ",3) || !strcmp(command,"ls") || !strcmp(command,"dir"))
        return LS;
    if( !strcmp(command,"cd") || !strncmp(command,"cd ",3) )
        return CD;
    if( !strcmp(command,"delete") || !strncmp(command,"delete ",7) )
        return DELETE;
    if( !strcmp(command,"close"))
        return CLOSE;
    if( !strcmp(command,"quit") || !strcmp(command,"bye") || !strcmp(command,"exit") )
        return EXIT;
    if( !strcmp(command,"connect") ||!strncmp(command,"connect ",8))
        return CONNECT;
    if( !strcmp(command,"lcd") || !strncmp(command, "lcd ",4) || !strcmp(command,"pwd"))
        return PWD;
    if( !strcmp(command,"user") || !strncmp(command,"user ",5))
        return USER;
    if( !strcmp(command,"bin") || !strcmp(command,"binary") )
        return BINARY;
    if( !strcmp(command,"as") || !strcmp(command,"ascii") )
        return ASCII;
    if( !strncmp(command,"!",1) )
        return SHELL;
    if( !strncmp(command,"read ", 4) )
        return READ;
    if( !strncmp(command,"write ", 4) )
        return WRITE;
    if( !strcmp(command,"help") || !strcmp(command,"h")|| !strcmp(command,"?"))
        return HELP;
    
    return -1;   /* unsupported command */
}
/*
 * func_connect
 * this function is called when the o,open command is issued.
 * it connects to the requested host, and logs the user in
 *
 */
void func_connect( char *command)
{
  char *host_add=NULL;  /* remote host */
   /* 
    * do not do anything if we are already connect.
    */
   if( flag_connected ) {
       printf("Already connect.  Close connection first.n");
       fflush(stdout);
       return;
   }
   
   /*
    * extract the remote host from command line (if given ).
    * make a copy of the host name with strdup.
    */
   if(!strcmp(command,"connect")) 
   {
       printf("Host:"); 
       fgets(tmp_buffer,1024,stdin);
       (void)strtok(tmp_buffer,"n");
       host_add = (char *)strdup(tmp_buffer);
   }
   else if( !strncmp(command,"connect ",8))
host_add = strdup(&command[8]);
   else
host_add = strdup(command);
   printf("connecting to %sn",host_add);
   hControlSocket = flag_connect_to_server(host_add,"21");
   sleep(1);
   if( hControlSocket > 0)  {
     printf("connected to %sn",host_add);
     flag_connected = 1;         /* we ar now connect */
     k=get_host_reply();            /* get reply (welcome message) from server */
     TranslateReply(k);	/* parse the meaning of the remote's reply number*/
     func_login((char *)NULL); /* prompt for username and password */
     func_binary_mode();            /* default binary mode */
   }
   free(host_add); /* free the strdupped string */
}
/*
 * func_login logs the user into the remote host.
 */
void func_login( char *command)
{
   char *User=NULL;
 
   if( command && *command)
     User=&command[5];
   if( flag_connected )  {
     /* ignore leading whitespace */
     
     while(User && (*User == ' ' || *User == 't') && *User)
 User++;
     /* if user name was not provided via command line, read it in. */
     
     if(!User || !(*User) ) {
printf("Login:"); fgets(ftp_user,20,stdin);
User = ftp_user;
(void)strtok(ftp_user,"n");   /* remove 'n' */
     }
     /*
      * send user name & password to server  & get reply message
      */
     sprintf(tmp_buffer,"USER %srn",User);
     send_ctrl_msg(tmp_buffer,strlen(tmp_buffer));
     k=get_host_reply();
     TranslateReply(k);
     getpassword( ftp_pass );
     sprintf(tmp_buffer,"PASS %srn",ftp_pass);
     send_ctrl_msg(tmp_buffer,strlen(tmp_buffer));
     k=get_host_reply();
     TranslateReply(k);
   }
   else
      printf("Not connected.n");
}
/*
 * func_close
 * closes connection to the ftp server
 */
void func_close( void )
{
   if( !flag_connected  ) {
     printf("Not connected.n");
    
   }
   else {
   send_ctrl_msg("quitrn",6);
   get_host_reply();
   close_control_connect();
   hControlSocket = -1;
   flag_connected = 0;
   }
}
/*
func_delete("DELE %srn", argv[1]);
*/
void func_delete(char *command)
{
   char *file=&command[6];
   if( !flag_connected ) {
       printf("Not flag_connected.n");
       return;
   }
   /*
    * ignore leading whitespace
    */
   while( *file && (*file == ' ' || *file == 't') ) 
       file++;
   /*
    * if file is not specified, read it in
    */
   if( ! (*file) ) {
      printf("Remote file name:");
      fgets(tmp_buffer,1024,stdin);
      (void)strtok(tmp_buffer,"n");
      file = (char *)strdup(tmp_buffer);
      while( *file && (*file) == ' ') 
       file++;
      if( !(*file) ) {
printf("Usage: delete remote-filen");
return;
      }
   }
   
   /*
    * send command to server and read response
    */
   sprintf(tmp_buffer, "DELE %srn",file);
   send_ctrl_msg(tmp_buffer,strlen(tmp_buffer));
   int k=get_host_reply();
   TranslateReply(k);
   printf("%s deleted.n", file);
   
}
/*
 * func_list
 * perform directory listing i.e: ls
 */
void func_list( char *command)
{
   if( !flag_connected ) {
      printf("Not flag_connected.n");
      return;
   }
   /*
    * obtain a listening socket
    */
   if( get_listen_socket() < 0) {
       printf("Cannot obtain a listen socket.n");
       return;
   }
   
   /*
    * parse command
    */
   if( !strcmp(command,"ls") )  {
       sprintf(tmp_buffer,"NLSTrn");
   }
   else if( !strcmp(command,"dir") ) 
       sprintf(tmp_buffer,"LISTrn");
   else if( !strncmp(command, "ls ",3)) {
       while( *command == ' ') command++;
       sprintf(tmp_buffer,"LIST %srn",&command[3]);
   }
   /*
    * send command to server and get response
    */
   send_ctrl_msg(tmp_buffer,strlen(tmp_buffer));
   memset(tmp_buffer,0,1024);
   k=get_host_reply();
   TranslateReply(k);
   /*
    * accept server's connection
    */
   if(accept_connection() < 0) {
      printf("Cannot accept connection.n");
      return;
   }
   close_listen_socket();       /* close listening socket */
   /*
    * display directory listing.
    */
   while( data_msg(tmp_buffer,1024) > 0) {
       fflush(stdout);
       printf(tmp_buffer);
       memset(tmp_buffer,0,1024);
   }
   /*
    * read response
    */
   k=get_host_reply();
   TranslateReply(k);
}
/*
 * func_cdir
 * chang to another directory on the remote system
 */
void func_cdir( char *command)
{
   char *dir=&command[2];
   if( !flag_connected ) {
       printf("Not flag_connected.n");
       return;
   }
   /*
    * ignore leading whitespace
    */
   while( *dir && (*dir == ' ' || *dir == 't') ) 
       dir++;
   /*
    * if dir is not specified, read it in
    */
   if( ! (*dir) ) {
      printf("Remote directory:");
      fgets(tmp_buffer,1024,stdin);
      (void)strtok(tmp_buffer,"n");
      dir = (char *)strdup(tmp_buffer);
      while( *dir && (*dir) == ' ') 
       dir++;
      if( !(*dir) ) {
printf("Usage: cd remote-directoryn");
return;
      }
   }
   
   /*
    * send command to server and read response
    */
   sprintf(tmp_buffer, "CWD %srn",dir);
   send_ctrl_msg(tmp_buffer,strlen(tmp_buffer));
   int k=get_host_reply();
   TranslateReply(k);
}
/*
 * func_pwd
 * change directory on the local system
 */
void func_pwd( char *command)
{
   char *dir = &command[3];
   while(*dir && (*dir == ' ' || *dir == 't') ) dir++;
   /*
    * if dir is not specified, then print the current dir
    */
   if( ! *dir ) {
      dir = getcwd((char *)NULL,256);
      if( !dir)
perror("getcwd");
      else
printf("Current directory is: %sn",dir);
   }
   else {
      if( chdir(dir) < 0) 
perror("chdir");
      else {
dir = getcwd((char *)NULL,256);
if( !dir)
    perror("getcwd");
else
printf("Current directory is: %sn",dir);
      }
   }
}
/*
 * function to pass commands to the system, ie: dir.
 * this function gets called when '!' is encountered
 */
void func_Shell_cmd( char *command )
{
  command++;  /* ignore '!' */
    system(command);
}
/*
 * func_binary_mode
 * set file transfer mode to binary
 */
void func_binary_mode()
{
  if( !flag_connected ) {
      printf("Not flag_connected.n");
      return;
  }
   sprintf(tmp_buffer, "TYPE Irn");
   send_ctrl_msg(tmp_buffer,strlen(tmp_buffer));
   int k=get_host_reply();
   TranslateReply(k);
   printf("File transfer modes set to binary.n");
   bMode = BINARY;
}
/*
 * func_ascii_mode
 * set file transfer mode to ascii text
 */
void func_ascii_mode()
{
  if( !flag_connected ) {
      printf("Not flag_connected.n");
      return;
  }
   sprintf(tmp_buffer, "TYPE Arn");
   send_ctrl_msg(tmp_buffer,strlen(tmp_buffer));
   int k=get_host_reply();
   TranslateReply(k);
   printf("File transfer modes set to ascii.n");
   bMode = ASCII;
}
/*
 * func_read
 * retrieve a file from the remote host.  calls getfile(..)
 */
void func_read( char *command)
{
  if( !flag_connected ) {
      printf("Not flag_connected.n");
      return;
  }
  (void)strtok(command," ");
  getfile(strtok((char *)NULL, " "));//get filename by limiter ' '.
}
/*
 * func_write
 * send a file to the remote host.  calls put_file(..)
 */
void func_write( char *command )
{
  if( !flag_connected ) {
      printf("Not flag_connected.n");
      return;
  }
  (void)strtok(command," ");
  put_file(strtok((char *)NULL, " "));
}
void func_help(char *command)
{
char  tmp_buffer[1025]={0};
printf("n");
if( !strncmp(command+5,"ls",2)|| !strncmp(command+5,"dir",3))
{
printf("this command is used to list the files of the server.n");
}
    if( !strncmp(command+5,"cd",2) )
{
printf("this command is used to change to directory of the servern");
}
    if( !strncmp(command+5,"delete",6))
{
printf("this command is used to delete the file on the server if we have the permission.n");
printf("usage:n");
printf("delete:filename.n");
}
    if( !strncmp(command+5,"close",5))
{
printf("this command is used to close the server we have connect.n");
}
    if( !strncmp(command+5,"quit",4) || !strncmp(command+5,"bye",3) ||
!strncmp(command+5,"exit",4) )
{
printf("the command 'quit','bye','exit' is to stop the connection with the server,n");
printf("if we have already connect to a server,these commands do the sama as ''close,n");
printf("if we have not connect to the server,then we do nothing");
}
    if( !strncmp(command+5,"connect",7) ||!strncmp(command+5,"connect ",8))
{
printf("this conmmand is used to connect to a server:n");
printf("if followed by an address of a server,then we will connect to the server,n");
printf("if there is nothing followed, then there will appear a prompt 'ftp>'to let you inter a address to connect.n");
}
    if( !strncmp(command+5,"lcd",3) || !strncmp(command+5, "lcd ",4) )
    {
    	printf("this command is used to get the current directory.n");
    	}
    if( !strncmp(command+5,"bin",3) || !strncmp(command+5,"binary",6) )
{
printf("this command is used to set the transfer mode to binary.n");
}
    if( !strncmp(command+5,"as",2) || !strncmp(command+5,"ascii",5) )
{
printf("this command is used to set the transfer mode to ascii.n");
}
    if( !strncmp(command+5,"!",1) )
{
printf("this command is used to pass commands to the system.n");
}
    if( !strncmp(command+5,"read ", 4))
{
printf("this command to get a file from the remote server.n");
printf("usage:  read filenamen");
}
    if( !strncmp(command+5,"write ", 4) )
{
printf("this command is used to send a file to a remote server if we have the permission.n");
}
else 
printf("this is help function.n");
}
int main(int argc, char *argv[])
{
   void (*OldSig)(int);
   OldSig=signal(SIGINT,SIG_IGN);
   printf("nn************************************************************rn");
   printf("*                           zFTP                           *rn");
   printf("************************************************************rn");
   printf("                                 Copyright (c) 2005rn");
   printf("                                 All rights reserved.rn");
   printf("                                 By Ji Surn");
   if(argc > 0)
   {
     printf("nPlease use 'connect <system name> <user>'  command to connect to a FTPserverrn");
     printf("For anonymous user, please leave <user> blank.rn");
   }	 
   
   //Main loop starts; 
   //Checks on keyboard i/o for interaction and socket communication.
 
   int exit=0;
char command[1024];
printf("nzftp>");
while(!exit) {
 if( ReadCommand ) {
     printf("nzftp>");
     fflush(stdout);
 }
 
 if( !CheckFds(command) )
      continue;
 
 switch(check_cmd(command) ) {
 case READ:
      func_read(command);
      break;
 case WRITE:
      func_write(command);
      break;
case BINARY:
      func_binary_mode();
      break;
case ASCII:
      func_ascii_mode();
      break;
case CLOSE:
 func_close();
 break;
case CONNECT:
 func_connect(command);
 break;
case CD:
func_cdir(command);
  	break;
case DELETE:
func_delete(command);
  	break;
case PWD:
func_pwd(command);
break;
case LS:
func_list(command);
break;
case HELP:
func_help(command);
  	break;
case SHELL:
func_Shell_cmd(command);
break;
 case EXIT:
 exit = 1;
 if( flag_connected )
     func_close();
 break;
 default:
if(command[0] == 0 ) ;
else
    printf("Invalid command: %sn",command);
break;
  }
 }
   
   (void)signal(SIGINT,OldSig);
   return 0;
}
