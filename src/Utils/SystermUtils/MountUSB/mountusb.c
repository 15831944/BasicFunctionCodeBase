#include "mountusb.h"

static pthread_mutex_t mount_mutex;
static char devlist[8][MAX_PART];
static int deamon_mode = 0;

static int load_procfile(char* filename, char* buf);

//得到所有的U分（分区）设备列表，以分号分隔
static int get_udisk_dev_list()
{
	int major, minor;
	long long blocks;
	char name[256], line[256];
	int len, fd, partnum=0;
	FILE *f;
	char *frc, *iobuf = NULL;
	
	fd = open("/proc/partitions", O_RDONLY, 0);
	if (fd < 0)
	{
		//fprintf(stderr, "Failed to open /proc/partitions: %s\n", strerror(errno));
		return -1;
	}
	iobuf = (char *)malloc(16384);
	if (!iobuf)
	{
		//fprintf(stderr, "out of memory.\n");
		return -1;
	}
	len = read(fd, iobuf, 16384);
	close(fd);
	if (len < 0)
	{
		fprintf(stderr, "failed to read /proc/partitions: %s\n", strerror(errno));
		return -1;
	}
	memset(devlist, 0, 256);
	f = (FILE*)fmemopen((void*)iobuf, len, "r");
	
	frc = fgets(line, sizeof(line), f);
	if (frc)
		frc = fgets(line, sizeof(line), f);
	if (frc)
		frc = fgets(line, sizeof(line), f);
	while(frc)
	{
		if (sscanf(line, "%d %d %lld %s", &major, &minor, &blocks, name) != 4)
		{
			fprintf(stderr, "invalid line in /proc/partitions: '%s'\n", line);
			if (iobuf)
			free(iobuf);
			fclose(f);
			return -1;
		}
		if ((blocks > 10) && (major==8))
		{
			char buf[16];
			sprintf(buf, "/dev/%s", name);
			if (access(buf, R_OK)==0)
			strncpy(devlist[partnum++], name, 8);
				//sprintf(devlist, "%s;%s", devlist, name);
			PRINTF("Part %d: %s found\n", partnum, name);
		}
		frc = fgets(line, sizeof(line), f);
	}
	if (iobuf)
	free(iobuf);
	fclose(f);
	return partnum;
}

//检查指定的U盘（分区）或目标目录是否已经mount上了。
static int check_mount (char *device_name, char *mnt_dir)
{
	FILE *f;
	struct mntent *mnt;
	
	if((f = setmntent("/proc/mounts", "r")) == NULL)
		return 0;

	while((mnt = getmntent(f)) != NULL) {
		//PRINTF("device: %s mount on: %s\n", mnt->mnt_fsname, mnt->mnt_dir);
		if (device_name && (strcmp(device_name, mnt->mnt_fsname) == 0))
			goto quit;

		if (mnt_dir && (strcmp(mnt_dir, mnt->mnt_dir) == 0))
			goto quit;
	}
	endmntent( f);
	return 0;

quit:
	PRINTF("Checking mount: %s or %s have been mounted\n", device_name, mnt_dir);
	return 1;
}

static int get_mounted_dir(char *device_name, char* dir_name)
{
	FILE *f;
	struct mntent *mnt;

	if ((f = setmntent("/proc/mounts", "r")) == NULL)
		return -1;

	while ((mnt = getmntent(f)) != NULL) {
		if (strcmp(device_name, mnt->mnt_fsname) == 0) {
			strcpy(dir_name, mnt->mnt_dir);
			endmntent(f);
			return 0;
		}
	}
	endmntent(f);

	return -1;	
}

//把指定的U盘（分区）设备mount到目标目录上。
static int mountusb2dir(char *dev, char *target)
{
#ifdef USE_CMD
	char *argv[] = { "/bin/mount", "-t", "vfat", NULL, NULL, NULL};
	pid_t pid;
	int status;
	int errors = 0;
	
	if ((!dev) || (!target))
		return 1;
	argv[3] = dev;
	argv[4] = target;
	if (!(pid = vfork()))
	{
		execv(argv[0], argv);
		_exit(1);
	}
	waitpid(pid, &status, 0);
	if (!((((__extension__ ({ union { __typeof(status) __in; int __i; } __u; __u.__in = (status); __u.__i; }))) & 0x7f) == 0) || ((((__extension__ ({ union { __typeof(status) __in; int __i; } __u; __u.__in = (status); __u.__i; }))) & 0xff00) >> 8))
	{
		fprintf(stderr, "/bin/mount failed\n");
		errors = -1;
	}
	
	return errors;
#else
	char buf[512];
	int ret = 1;
	if (!load_procfile("/proc/filesystems", buf))
		return 1;
	if (strstr(buf, "vfat")) {
		PRINTF("Try to mount %s to %s as VFAT\n", dev, target);
		ret = mount(dev, target, "vfat", MS_NOEXEC, "iocharset=utf8,shortname=mixed");
	}
	if ((ret) && (strstr(buf, "ext3"))) {
		PRINTF("Try to mount %s to %s as EXT3\n", dev, target);
		ret = mount(dev, target, "ext3", MS_NOEXEC, 0);
	}
	if ((ret) && (!access("/home/yx5532/bin/ntfs-3g.elf", R_OK))) {
		int pid;
		PRINTF("Try to mount %s as NTFS\n", dev);
		if (!(pid = vfork())) {
			PRINTF("Run ntfs-3g.elf %s %s\n", dev, target);
			ret = execl("/home/yx5532/bin/ntfs-3g.elf", "/home/yx5532/bin/ntfs-3g.elf", dev, target, NULL);
			PRINTF("ntfs-3g.elf return %d\n", ret);
			_exit(ret);
		}
		waitpid(pid, &ret, 0);
		//ret = mount(dev, target, "ntfs", 0, "iocharset=gb2312");
	}
	PRINTF("%s end\n", __FUNCTION__);
	return ret;
#endif	
}

/* Mount所有的U盘（分区）。*/
static int mount_all_usb()
{
	char buf[12], tmp[12];
	int i, rc;
//	int cur_dir_no = 0;
	int partnum = get_udisk_dev_list();

#ifdef MS_DIRSYNC
	int flags = MS_SYNCHRONOUS | MS_NOATIME | MS_NODIRATIME | MS_NOSUID; /* sync mode is better for removeable */
	flags |= MS_DIRSYNC; 
#endif

	i = 0;
	while (i < partnum)
	{
		/* Skip duplicat partition and disk device. For example, skip sda if sda1 found */
		int j, k;
		k = 1;
		PRINTF("Checking %s: ", devlist[i]);
		for (j = i + 1; j < partnum; j++) {
			k = strncmp(devlist[i], devlist[j], strlen(devlist[i]));
			PRINTF("%s=%d ", devlist[j], k);
			if (k == 0)
				break;
		}
		PRINTF("\n");
		if (k) {
			sprintf(tmp, "/dev/%s", devlist[i]);
			if (check_mount(tmp, NULL)) {
				fprintf(stderr,"Device %s has been mounted\n", tmp);
				i++;
				continue;
			}
			
			//找一个没有mount设备的目录。
			for (j = 1; j < MAX_PART; j++) {
				printf(">>  j = %d\n", j);
				sprintf(buf, "/mnt/usb%d", j);
				if (access(buf, R_OK) == 0)
					if (!check_mount(NULL, buf))
						break;
			}
			if (j < MAX_PART) {
				fprintf(stderr, "Mounting %s to %s ", tmp, buf);
					
				rc = mountusb2dir(tmp, buf);
				if (rc != 0) {
					fprintf(stderr, "failed: %s\n", strerror(errno));
				} else {
					fprintf(stderr, "ok\n");
					usleep(100000);
				}
			}
		}
		i++;
	}
	return 0;
}

//卸载mount到指定目录的U盘（分区）。
static int umount_usb(char *mdir)
{
	if(mdir != NULL)
	{
		printf("umount %s", mdir);
		if(umount2(mdir, MNT_FORCE) != 0)
		{
			printf(" fail\n");
			return -1;
		}
		else
			printf(" ok\n");
	}
	else
		return -1;
	return 0;
}

//创建用于接收hotplug消息的socket。
int mountusb_socket_init(void)
{
	int hotplug_sock = 0;
    struct sockaddr_nl snl;
//    const int buffersize = 16 * 1024 * 1024;
    int retval;
	setlinebuf(stdout);
    memset(&snl, 0x00, sizeof(struct sockaddr_nl));
    snl.nl_family = 16;
    snl.nl_pid = getpid();
    snl.nl_groups = 1;

    hotplug_sock = socket(16, SOCK_DGRAM, 15);
    if (hotplug_sock == -1)
    {
        PRINTF("error getting socket: %s", strerror(errno));
        return -1;
    }

    retval = bind(hotplug_sock, (struct sockaddr *) &snl, sizeof(struct sockaddr_nl));
    if (retval < 0) {
        PRINTF("bind failed: %s", strerror(errno));
        close(hotplug_sock);
        hotplug_sock = -1;
        return -1;
    }
    pthread_mutex_init (&mount_mutex, NULL);
    return hotplug_sock;

}

//关闭用于接收hotplug消息的socket。
void mountusb_socket_uninit(int sockfd)
{
	close(sockfd);
}

//取得设备号尾部的数字。
static int get_digit_suffix(char* devName)
{
	char c;
	int i;

	for(i=0; i<strlen(devName); i++)
	{
		c = *(devName+i);
		if( isdigit(c) )
		{
			int no;
			no = atoi(devName+i);
			//sprintf(dev, "/dev/%s", devName);
			return no;
		}
	}
	return -1;
}

//从hotplug消息行中提最设备号。
static int extract_dev_no(char* info, char* dev)
{
	char *p1 = basename(info);
	if (p1 && dev) {
		char* devName = p1;
		sprintf(dev, "/dev/%s", devName);
		return get_digit_suffix(devName);
	}
	return -1;
}

static int load_procfile(char* filename, char* buf)
{
	FILE* fp = fopen(filename, "rb");
	if (fp) {
		int ret = fread(buf, 1, 512, fp);
		fclose(fp);
		return ret;
	}
	return 0;
}

static int extract_dev_product(char* dir, char* product, char* vendor)
{
	char *p, filename[256];

	if (strncmp(dir, "add@/devices", 11))
		return 1;

	p = (char*)(dir + 4);
	sprintf(filename, "/sys%s/product", p);
	if (!load_procfile(filename, product))
		return 1;

	sprintf(filename, "/sys%s/manufacturer", p);
	if (!load_procfile(filename, vendor))
		return 1;

	return 0;
}

int mountmsb_process_data(int sockfd)
{
	char buf[4096] = {0};
	char devName[20];
	char dirName[20];

	recv(sockfd, &buf, sizeof(buf), 0);

	if (strstr(buf, "add@/block/")) {
		PRINTF("Received: %s\n", buf);
		mount_all_usb();	
	} else 
	if (strstr(buf, "remove@/block/")) {
		extract_dev_no(buf, devName);
		PRINTF("umount %s\n", devName);
		
		if (get_mounted_dir(devName, dirName))
			return -1;
		
		if (umount_usb(dirName)) {
			int i = 10;
			PRINTF("umount %s: %s!\n", dirName, strerror(errno));
			while (i--) {
				sleep(1);
				if (umount_usb(dirName)) {
					PRINTF("umount %s retry%d: %s!\n", dirName, i, strerror(errno));
					PRINTF("================================\n\n");
				} else {
					PRINTF("Umount %s ok\n", dirName);
					break;
				}
			}
		}
	} else
	if (0 == extract_dev_product(buf, devName, dirName)) {
		if (strstr(devName, "802.11") && strstr(devName, "WLAN")) {
			PRINTF("WLAN device found!\n");
			sleep(2);
			system("ifconfig rausb0 up");
		}
	}
	return 0;
}

static int log_pid(void) 
{
	FILE *f;

	if((f = fopen(PID_FILE, "r")) != NULL) {
		fclose(f);
		return 1;
	}
	
	if((f = fopen(PID_FILE, "w")) == NULL) {
		printe("fopen %s mode=w Err.\n", PID_FILE);
		return -1;
	}
	fprintf(f, "%d\n", getpid());
	fclose(f);
	
	return 0;
}

int main(int argc,char *argv[])
{
	int rc, socket_usb;	
	int fd,fdtablesize;
	int fp;   
	
	rc = log_pid();
	switch (rc) {
		case 1: 
			fprintf(stderr, "%s is runing, kill it and remove %s ...\n", argv[0], PID_FILE);
			exit(1);
		break;
		case -1: 
			fprintf(stderr, "Cannot save %s\n", PID_FILE);
			exit(-1);
		break;
	}
	
	if ((argc < 2) || strncmp(argv[1], "-d", 2)) {
		deamon_mode = 1;
	// Run in deamon mode
	//屏蔽一些有关控制终端操作的信号。防止在守护进程没有正常运转起来时，控制终端受到
	//干扰 退出或挂起，此处忽略了终端I/O信号、STOP信号
		 signal(SIGTTOU,SIG_IGN);
		 signal(SIGTTIN,SIG_IGN);
		 signal(SIGTSTP,SIG_IGN);
		 signal(SIGHUP,SIG_IGN);
	//由于子进程会继承父进程的某些特性，如控制终端、登录会话、进程组等，而守护进程
	//最终要脱离控制终端到后台去运行，所以必须把父进程杀掉，以确保子进程不是进程组长，
	//这也是成功调用setsid()所要求的。
		 if (fork() != 0)
		 {
/*			syslog(LOG_USER|LOG_INFO,"Run  %d\n", __LINE__); */
			exit(1);
		 }
	//脱离了控制终端还要脱离登录会话和进程组，这里可以调用setsid()函数，调用成功后
	//进程成为新的会话组长和新的进程组长，并与原来的登录会话和进程组脱离，由于会话过程
	//对控制终端的独占性，进程同时与控制终端脱离。
	
		 //if 
		 setsid();
		 {
/*			syslog(LOG_USER|LOG_INFO,"Run to Line %d\n", __LINE__); */
//			exit(1);
		 }

		 //要达到setsid()函数的功能也可以用如下处理方法。"/dev/tty"是一个流设备，也是终端映射，调用close()函数将终端关闭。
		 if ((fp = open("/dev/tty", O_RDWR)) >= 0)
		 {
			 ioctl(fp, TIOCNOTTY, NULL);
			 close(fp);
		 }

		/*从父进程继承过来的当前工作目录可能在一个装配的文件系统中。因为守护进程通常在系统重启之前是一直存在的，
		所以如果守护进程的当前工作目录在一个装配文件系统中，那么该文件系统就不能被卸载。
		比如说从父进程继承的当前目录是/mnt下面的一个被挂载的目录。
		*/
		 if (chdir("/tmp") == -1) {
			syslog(LOG_USER|LOG_INFO, "Run to Line %d\n", __LINE__);
			exit(1);
		 }
		 /*关闭打开的文件描述符，或重定向标准输入、标准输出和标准错误输出的文件描述符。
		 进程从创建它的父进程那里继承了打开的文件描述符。如果不关闭，将会浪费系统资源，
		 引起无法预料的错误。getdtablesize()返回某个进程所能打开的最大的文件数。*/
		for (fd=0, fdtablesize = getdtablesize(); fd < fdtablesize; fd++)
			close(fd);

		 //有的程序有些特殊的需求，还需要将这三者重新定向。
	/*
		 error=open("/tmp/error",O_WRONLY|O_CREAT,0600);
		 dup2(error,2);
		 close(error);
		 in=open("/tmp/in",O_RDONLY|O_CREAT,0600);
		 if(dup2(in,0)==-1) perror("in");
		 close(in);
		 out=open("/tmp/out",O_WRONLY|O_CREAT,0600);
		 if(dup2(out,1)==-1) perror("out");
		 close(out);
	*/
		 /* 由继承得来的文件方式创建的屏蔽字可能会拒绝设置某些权限，所以要重新赋于所有权限。 */
		 umask(0);

		/*如果父进程不等待子进程结束，子进程将成为僵尸进程（zombie）从而占用系统资源，
		如果父进程等待子进程结束，将增加父进程的负担，影响服务器进程的并发性能。因此需要对SIGCHLD信号做出处理，
		回收僵尸进程的资源，避免造成不必要的资源浪费。 */
		signal(SIGCHLD,SIG_IGN);
	}

	mount_all_usb();
	socket_usb = mountusb_socket_init();

	while (1) {
		mountmsb_process_data(socket_usb);
	}
	mountusb_socket_uninit(socket_usb);
	unlink(PID_FILE);
	return 0;
}
