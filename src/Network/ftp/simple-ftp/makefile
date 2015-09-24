progs=zftp
                                                                  
SOLARISLIBS= -lnsl
                                                                  
CC=gcc
                                                                  
all: zftp
                                                                  
zftp: ftp.c ftp.h io.c conct.c func.h conct.h t_reply.c                                  
$(CC) -g -o zftp ftp.c io.c conct.c t_reply.c $(SOLARISLIBS)            
                                                                  
clean:                                                            
rm $(progs)                                                   
                                                                  
                                                                  
