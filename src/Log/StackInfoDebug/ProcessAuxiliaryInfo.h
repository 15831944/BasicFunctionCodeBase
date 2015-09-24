/**
 *  ProcessAuxiliaryInfo.h
 *  进程初始化的时候，堆栈里面保存了关于进程执行环境和命令行参数等信息。
 *  事实上，堆栈里面还保存了动态链接器所需要的一些辅助信息数组（Auxiliary Vector）。
 *  那么进程执行环境、命令行参数信息以及辅助信息数组在进程堆栈中是怎么分布的呢？
 *   
 **/

/**
 *  编译： gcc -g auxiliary_vector.c -o auxiliary_vector
 *  运行： ./auxiliary_vector heshuang write
 *  结果：
            Argument count:3  
            Argument 0 : ./auxiliary_vector  
            Argument 1 : heshuang  
            Argument 2 : write  
            Environment:  
            SSH_AGENT_PID=1524  
            GPG_AGENT_INFO=/tmp/keyring-twYZ4a/gpg:0:1  
            TERM=xterm  
            SHELL=/bin/bash  
            XDG_SESSION_COOKIE=41039a83fa6262cfc72afc1e00000006-1385343710.39082-378826950  
            WINDOWID=62914565  
            GNOME_KEYRING_CONTROL=/tmp/keyring-twYZ4a  
            GTK_MODULES=canberra-gtk-module:canberra-gtk-module  
            USER=hs  
            LS_COLORS=rs=0:di=01;34:ln=01;36:mh=00:pi=40;33:so=01;35:do=01;35:bd=40;33;01:cd=40;33;01:or=40;31;01:su=37;41:sg=30;43:ca=30;41:tw=30;42:ow=34;42:st=37;44:ex=01;32:*.tar=01;31:*.tgz=01;31:*.arj=01;31:*.taz=01;31:*.lzh=01;31:*.lzma=01;31:*.tlz=01;31:*.txz=01;31:*.zip=01;31:*.z=01;31:*.Z=01;31:*.dz=01;31:*.gz=01;31:*.lz=01;31:*.xz=01;31:*.bz2=01;31:*.bz=01;31:*.tbz=01;31:*.tbz2=01;31:*.tz=01;31:*.deb=01;31:*.rpm=01;31:*.jar=01;31:*.rar=01;31:*.ace=01;31:*.zoo=01;31:*.cpio=01;31:*.7z=01;31:*.rz=01;31:*.jpg=01;35:*.jpeg=01;35:*.gif=01;35:*.bmp=01;35:*.pbm=01;35:*.pgm=01;35:*.ppm=01;35:*.tga=01;35:*.xbm=01;35:*.xpm=01;35:*.tif=01;35:*.tiff=01;35:*.png=01;35:*.svg=01;35:*.svgz=01;35:*.mng=01;35:*.pcx=01;35:*.mov=01;35:*.mpg=01;35:*.mpeg=01;35:*.m2v=01;35:*.mkv=01;35:*.ogm=01;35:*.mp4=01;35:*.m4v=01;35:*.mp4v=01;35:*.vob=01;35:*.qt=01;35:*.nuv=01;35:*.wmv=01;35:*.asf=01;35:*.rm=01;35:*.rmvb=01;35:*.flc=01;35:*.avi=01;35:*.fli=01;35:*.flv=01;35:*.gl=01;35:*.dl=01;35:*.xcf=01;35:*.xwd=01;35:*.yuv=01;35:*.cgm=01;35:*.emf=01;35:*.axv=01;35:*.anx=01;35:*.ogv=01;35:*.ogx=01;35:*.aac=00;36:*.au=00;36:*.flac=00;36:*.mid=00;36:*.midi=00;36:*.mka=00;36:*.mp3=00;36:*.mpc=00;36:*.ogg=00;36:*.ra=00;36:*.wav=00;36:*.axa=00;36:*.oga=00;36:*.spx=00;36:*.xspf=00;36:  
            XDG_SESSION_PATH=/org/freedesktop/DisplayManager/Session0  
            XDG_SEAT_PATH=/org/freedesktop/DisplayManager/Seat0  
            SSH_AUTH_SOCK=/tmp/keyring-twYZ4a/ssh  
            USERNAME=hs  
            DEFAULTS_PATH=/usr/share/gconf/ubuntu-2d.default.path  
            SESSION_MANAGER=local/hs-virtual-machine:@/tmp/.ICE-unix/1492,unix/hs-virtual-machine:/tmp/.ICE-unix/1492  
            XDG_CONFIG_DIRS=/etc/xdg/xdg-ubuntu-2d:/etc/xdg  
            PATH=/usr/lib/lightdm/lightdm:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games  
            DESKTOP_SESSION=ubuntu-2d  
            QT_IM_MODULE=xim  
            PWD=/home/hs/program/C-program/程序员的自我修养  
            XMODIFIERS=@im=ibus  
            GNOME_KEYRING_PID=1483  
            LANG=zh_CN.UTF-8  
            MANDATORY_PATH=/usr/share/gconf/ubuntu-2d.mandatory.path  
            UBUNTU_MENUPROXY=libappmenu.so  
            GDMSESSION=ubuntu-2d  
            SHLVL=1  
            HOME=/home/hs  
            LANGUAGE=zh_CN:zh  
            GNOME_DESKTOP_SESSION_ID=this-is-deprecated  
            LOGNAME=hs  
            XDG_DATA_DIRS=/usr/share/ubuntu-2d:/usr/share/gnome:/usr/local/share/:/usr/share/  
            DBUS_SESSION_BUS_ADDRESS=unix:abstract=/tmp/dbus-tYpEHv9TD3,guid=e54092127d5ebd0d453565fa00000017  
            LESSOPEN=| /usr/bin/lesspipe %s  
            DISPLAY=:0  
            XDG_CURRENT_DESKTOP=Unity  
            GTK_IM_MODULE=ibus  
            LESSCLOSE=/usr/bin/lesspipe %s %s  
            COLORTERM=gnome-terminal  
            XAUTHORITY=/home/hs/.Xauthority  
            OLDPWD=/home/hs  
            _=./auxiliary_vector  
            Auxiliary Vectors:  
            Type: 32 Value: 480414  
            Type: 33 Value: 480000  
            Type: 16 Value: 1febf3ff  
            Type: 06 Value: 1000  
            Type: 17 Value: 64  
            Type: 03 Value: 8048034  
            Type: 04 Value: 20  
            Type: 05 Value: 9  
            Type: 07 Value: c13000  
            Type: 08 Value: 0  
            Type: 09 Value: 8048360  
            Type: 11 Value: 3e8  
            Type: 12 Value: 3e8  
            Type: 13 Value: 3e8  
            Type: 14 Value: 3e8  
            Type: 23 Value: 0  
            Type: 25 Value: bfee1b7b  
            Type: 31 Value: bfee3fe9  
            Type: 15 Value: bfee1b8b
 *  
 *  
 *  
 **/
#ifndef __PROCESS_AUXILIARY_INFO_H__
#define __PROCESS_AUXILIARY_INFO_H__




int ProcessAuxiliaryInfoOutPut(const char *file, int line, const char *function, int argc, char * argv[])  ;


#define ProcessAuxiliaryInfoDebug( argc, args...) \
        do {                                      \
            ProcessAuxiliaryInfoOutPut(__FILE__, __LINE__, __FUNCTION__, argc, args);  \
        } while(0)











#endif  //__PROCESS_AUXILIARY_INFO_H__
  
