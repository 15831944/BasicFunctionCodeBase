/* FTP.H
 */
#ifndef __FTP_H__
#define __FTP_H__
#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "func.h"
extern int hControlSocket,
   hListenSocket,
   hDataSocket;
int ReadCommand = 0;
int bMode=BINARY;
#if (defined(ultrix) || defined(__ULTRIX) || defined (__ultrix)) 
char *strdup( const char *str)
{
  char *szTemp;
  szTemp = (char *)malloc(strlen(str));
  if( !szTemp )
     return (char *)NULL;
  
  strcpy(szTemp,str);
  return szTemp;
}
#endif
void func_connect(char *);
void func_list(char *);
void func_cdir(char *);
void func_delete(char *);
void func_Shell_cmd(char *);
void func_login(char *);
void func_close(void);
void func_pwd( char *);
void func_write( char *);
void func_read( char *);
void func_binary_mode();
void func_ascii_mode();
void func_help();
int  get_host_reply();
int  CheckFds(char *);
#endif
