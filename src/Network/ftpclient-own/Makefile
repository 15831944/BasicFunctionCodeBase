# Makefile for ftp    by wzt

CC         = gcc
CFLAGS         = -O2

.c.o:        $(CC) $(CFLAGS) \
        -c -o $*.o $<
OBJS = socket.o ftp.o
        
client: $(OBJS)
        $(CC) -o ftp $(OBJS)
clean:
        rm -f ftp ftp.exe *.o