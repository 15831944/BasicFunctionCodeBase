#include <stdio.h>
#include <stdlib.h>

#include "build_info.h"
#include "LogC.h"
#include "InitVersions.h"

extern struct _Version  g_version;

/**
 *  NOTE:
 *      在使用之前先调用该函数，对该模块的log组件进行初始化
 **/
int VersionInit()
{
#if 1
    //init g_version
    g_version.Major = atoi(g_make_major_version);
    g_version.Minor = atoi(g_make_minor_version);
    g_version.BuildVersion = atoi(g_make_svn_version);
#else
    g_version.Major = 9;
    g_version.Minor = 88;
    g_version.BuildVersion = 777;
#endif

    printf("==================version_init==[%s:%s:%s]================\n", g_make_major_version, g_make_minor_version, g_make_svn_version);
    return 0;
}

