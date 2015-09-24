#include <stdio.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>



// 帮助
void cmd_help() {
    printf(" get \t get a file from server.\n");
    printf(" put \t send a file to server.\n");
    printf(" pwd \t get the present directory on server.\n");
    printf(" dir \t list the directory on server.\n");
    printf(" cd \t change the directory on server.\n");
    printf(" ?/help\t help you know how to use the command.\n");
    printf(" quit \t quit client.\n");
}



int ClientMain(int argc, char* argv[]) 
{
    if (argc != 2 && argc != 3) {
        printf("Usage: %s <host> [<port>]\n", argv[0]);
        exit(-1);
    } else if (argc == 2) {
        cmd_help();
    } else {
        cmd_help();
    }
    
    return 0;
}


















