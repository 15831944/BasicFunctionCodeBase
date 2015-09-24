#include <stdio.h>

#include "Log/LogC.h"
#include "logUtils.h"
#include "Utils.h"

extern int log_utils_init();

int main()
{
    log_utils_init();

    utilsLogInfo("=====utilsLogInfo==main=ini parser=\n");
    printf("log_utils_init=====ini parser main=========\n");

    return 0;
}

