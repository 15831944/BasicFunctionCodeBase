/* TCP socket operations for control and data sockets.*/
#include "conct.h"
#include "func.h"
extern char tmp_buffer[];
extern int ReadCommand;
int get_listen_socket()
{
    int sockfd, flag=1,len;
    struct sockaddr_in  serv_addr, TempAddr;
    char *port,*ipaddr;
    char tmp_buffer[64]={0};
    /* Open an Internet stream socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
perror("socket");
return INVALID_SOCKET;
    }
    /* Fill in structure fields for binding */
   if( bSendPort ) {
       bzero((char *) &serv_addr, sizeof(serv_addr));
       serv_addr.sin_family      = AF_INET;
       serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
       serv_addr.sin_port        = htons(0);
   }
   else {
     /* reuse the control socket then */
      if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,
         (char *)&flag,sizeof(flag)) < 0) {
 perror("setsockopt");
 close(sockfd);
 return INVALID_SOCKET;
      }
   }
    /* bind the address to the socket */
    if (bind(sockfd,(struct sockaddr *)&serv_addr, 
     sizeof(serv_addr)) < 0) {
perror("bind");
close(sockfd);
return INVALID_SOCKET;
    }
    
    len = sizeof(serv_addr);
    if(getsockname(sockfd,
   (struct sockaddr *)&serv_addr,
   &len)<0) {
       perror("getsockname");
close(sockfd);
       return INVALID_SOCKET;
    }
    len = sizeof(TempAddr);
    if(getsockname(hControlSocket,
   (struct sockaddr *)&TempAddr,
   &len)<0) {
       perror("getsockname");
close(sockfd);
       return INVALID_SOCKET;
    }
    ipaddr = (char *)&TempAddr.sin_addr;
    port  = (char *)&serv_addr.sin_port;
#define  UC(b)  (((int)b)&0xff)
    sprintf(tmp_buffer,"PORT %d,%d,%d,%d,%d,%drn",
          UC(ipaddr[0]), UC(ipaddr[1]), UC(ipaddr[2]), UC(ipaddr[3]),
          UC(port[0]), UC(port[1]));
    /* allow only one ftp server to connect */
    if( listen(sockfd, 1) < 0) {
perror("listen");
close(sockfd);
return INVALID_SOCKET;
    }
    send_ctrl_msg(tmp_buffer,strlen(tmp_buffer));
    get_host_reply();
    hListenSocket = sockfd;
    return sockfd;
}
int accept_connection()
{
    struct sockaddr_in cli_addr;
    int clilen = sizeof(cli_addr);
    int sockfd;
    sockfd = accept(hListenSocket, (struct sockaddr *) &cli_addr,
   &clilen);
    if (sockfd < 0) {
        perror("accept");
return INVALID_SOCKET;
    }
   
    hDataSocket = sockfd;
    close(hListenSocket);
    return sockfd;
}
int flag_connect_to_server(char *name, char *port)
{
  int s;
  unsigned int portnum;
  struct sockaddr_in server;
  struct hostent *hp;
  while( name && *name == ' ') name++;
  if( !name || ! (*name) )
      return INVALID_SOCKET;
  portnum = atoi(port);
  bzero((char *) &server, sizeof(server));
  if( isdigit(name[0])) {
   server.sin_family      = AF_INET;
   server.sin_addr.s_addr = inet_addr(name);
   server.sin_port        = htons(portnum);
  }
  else{ 
   if ( (hp = gethostbyname(name)) == NULL)
    {
         perror("gethostbyname");
  return INVALID_SOCKET;
    }
    bcopy(hp->h_addr,(char *) &server.sin_addr,hp->h_length);
    server.sin_family = hp->h_addrtype;
    server.sin_port = htons(portnum);  
   }/* else */
  /* create socket */
  if( (s = socket(AF_INET, SOCK_STREAM, 0)) < 1) {
  perror("socket");
  
    return INVALID_SOCKET;
  }
  
  if (connect(s,(struct sockaddr *)&server, sizeof(server))< 0) {
  perror("connect");
    return INVALID_SOCKET;
  }
  
  setsockopt(s,SOL_SOCKET,SO_LINGER,0,0);
  setsockopt(s,SOL_SOCKET,SO_REUSEADDR,0,0);
  setsockopt(s,SOL_SOCKET,SO_KEEPALIVE,0,0);
  hDataSocket = s;
  
  return s;
}
int ReadControlMsg( char *tmp_buffer, int len)
{
  int ret;
   if( (ret=recv(hControlSocket,tmp_buffer,len,0)) <= 0)
       return 0;
   return ret;
}
int send_ctrl_msg( char *tmp_buffer, int len)
{
   if( send(hControlSocket,tmp_buffer,len,0) <= 0)
       return 0;
   
   return 1;
}
int data_msg( char *tmp_buffer, int len)
{
   int ret;
   if( (ret=recv(hDataSocket,tmp_buffer,len,0)) <= 0)
       return 0;
   
   return ret;
}
int send_data_msg( char *tmp_buffer, int len)
{
 
   if( send(hDataSocket,tmp_buffer,len,0) <= 0)
       return 0;
   
   return 1;
}
void close_data_connection( void )
{
      close(hDataSocket);
      hDataSocket = INVALID_SOCKET;
}
void close_control_connect( void )
{
      close(hControlSocket);
      hControlSocket = INVALID_SOCKET;
}
void close_listen_socket( void )
{
      close(hListenSocket);
      hListenSocket = INVALID_SOCKET;
}
int CheckControlMsg( char *szPtr, int len)
{
    
    return recv(hControlSocket,szPtr,len,MSG_PEEK);
}
int get_line()
{
    int done=0, iRetCode =0, iLen, iBuffLen=0;
    char *szPtr = tmp_buffer, nCode[3]={0},ch=0;
    while( (iBuffLen < 1024) && 
    (CheckControlMsg(&ch,1)  > 0) ){
        iLen = ReadControlMsg(&ch,1);
        iBuffLen += iLen;
        *szPtr = ch;
        szPtr += iLen;
        if( ch == 'n' )
            break;
    }
   *(szPtr+1) = (char)0;
   strncpy(nCode, tmp_buffer, 3);
   return (atoi(nCode));
}
int get_host_reply()
{
    int done = 0, iRetCode = 0;
     
    memset(tmp_buffer,0,1024);
    while(!done ) {
        iRetCode = get_line();
        (void)strtok(tmp_buffer,"rn");
        puts(tmp_buffer);
        if( tmp_buffer[3] != '-' && iRetCode > 0 )
            done = 1;
        memset(tmp_buffer,0,1024);
   }
   return iRetCode;
}
int check_input()
{
    int rval, i;
    fd_set readfds, writefds, exceptfds;
    struct timeval timeout;
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    FD_CLR (fileno(stdin),&readfds);
    timeout.tv_sec = 0 ;                /* 0-second timeout. */
    timeout.tv_usec = 0 ;               /* 0 microseconds.  */
    FD_SET(fileno(stdin),&readfds);
    i=select ( fileno(stdin)+1,
         &readfds,
        &writefds,
        &exceptfds,
        &timeout);
    /* SELECT interrupted by signal - try again. 
    if (errno == EINTR && i ==-1)  {
      return 0;
    }*/
    return ( FD_ISSET(fileno(stdin),&readfds) );
}
int  CheckFds( char *command)
{
    int rval, i;
    fd_set readfds, writefds, exceptfds;
    struct timeval timeout;
    char *szInput=command;
    /*  memset(command,0,1024); */
    memset(tmp_buffer,0,1024);
    FD_ZERO(&readfds);
    FD_ZERO(&writefds);
    FD_ZERO(&exceptfds);
    FD_CLR (fileno(stdin),&readfds);
    if( hControlSocket > 0) 
        FD_CLR (hControlSocket,&readfds);
    timeout.tv_sec = 0 ;                /* 1-second timeout. */
    timeout.tv_usec = 0 ;               /* 0 microseconds.  */
    FD_SET(fileno(stdin),&readfds);
    if( hControlSocket > 0) 
          FD_SET(hControlSocket,&readfds);
    i=select ((hControlSocket > 0) ? (hControlSocket+1) : 1,
         &readfds,
        &writefds,
        &exceptfds,
        &timeout);
    /* SELECT interrupted by signal - try again.  
    if (errno == EINTR && i ==-1)  {
    //memset(command,0,1024);
    return 0;
    }
    */
    if( (hControlSocket > 0) && FD_ISSET(hControlSocket, &readfds) ) 
    {
        if ( ( rval = ReadControlMsg(tmp_buffer,1024))  > 0)
        {
            printf(tmp_buffer);
            printf("ftp>");
            fflush(stdout);
            return 0;
        }
        else {
            printf("rnflag_connection closed by serverrn");
            #if (defined(WIN32) || defined(_WIN32) )
            SetConsoleTitle("dftp: flag_connection closed");
            #endif

            close_control_connect();
            hControlSocket = -1;
            return 0;
        }
    }
    if( FD_ISSET(fileno(stdin),&readfds) )  
        return (ReadCommand = get_unix_input(command));
    return (ReadCommand = 0);
}
