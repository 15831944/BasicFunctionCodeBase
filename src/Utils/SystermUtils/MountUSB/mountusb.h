#ifndef __MOUNT_USB_H__
#define __MOUNT_USB_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <limits.h>
#include <linux/input.h>
#include <linux/types.h>
#include <mntent.h>
#include <sys/ioctl.h>
#include <sys/ipc.h>
#include <sys/mount.h>
#include <sys/msg.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <linux/netlink.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <libgen.h>
#include <sys/wait.h>
#include <ctype.h>
#include <sys/klog.h>
#include <syslog.h>

#define printe printf
#define MAX_PART 16
#define PID_FILE                "/var/run/usbmounter.pid"
#ifdef PRINT_ON
#define PRINTF(x...) if (deamon_mode) syslog(LOG_USER|LOG_INFO, x); else printf(x); 
#else
#define PRINTF(x...)
#endif

int mountusb_socket_init(void);
void mountusb_socket_uninit(int sockfd);
int mountmsb_process_data(int sockfd);
static int get_digit_suffix(char* devName);
static int check_mount (char *device_name, char *mnt_dir);
static int get_mounted_dir(char *device_name, char* dir_name);
static int get_udisk_dev_list();
static int mount_all_usb();
static int umount_usb();
static int log_pid();

#endif
