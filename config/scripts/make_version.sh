#!/bin/sh

echo "-----------------------------------------------"
echo "            generate build_info.h              "
echo "-----------------------------------------------"
export LANG="en_US.utf8"

# 获取XML版本的svn信息，这样可以避免不同语言的问题
#xml=`svn info --xml --incremental`
# 我们可以获取到2个版本号，一个是最新版本库版本号，一个是自己的提交版本号。删除自己提交的版本号。
#revision=`echo "$xml"|sed '/revision/!d'|sed '$d'`
# 提取出版本号的数字部分
#BUILD_SVN_VERSION=`$revision|sed 's/revision="\([0-9]\+\)">\?/\1/'`

RootDIR=$(cd "$(dirname "$0")"; pwd)

#BUILD_SVN_VERSION=`svn info | grep "Revision" | grep -ior "[0-9]*"`
BUILD_SVN_VERSION="5325"
BUILD_MAJOR_VERSION="1"
BUILD_MINOR_VERSION="0"

BUILD_TIME=`date`
BUILD_USER=`whoami`
BUILD_HOST=`uname -n`
BUILD_KERNEL=`uname -srm`
BUILD_HEADER="$RootDIR/../../src/includes/build_info.h"

if [ -f $BUILD_HEADER ]; then
    echo "delete $BUILD_HEADER"
    rm $BUILD_HEADER
fi
touch $BUILD_HEADER

echo "#ifndef __BUILD_INFO_H__"   | tee "$BUILD_HEADER"
echo "#define __BUILD_INFO_H__"   | tee -a "$BUILD_HEADER"
echo " "   | tee -a "$BUILD_HEADER"
echo " "   | tee -a "$BUILD_HEADER"


echo "#define g_make_build_date             \"$BUILD_TIME\""   | tee -a "$BUILD_HEADER"
echo "#define g_make_build_user_name        \"$BUILD_USER\""   | tee -a "$BUILD_HEADER"
echo "#define g_make_build_host_name        \"$BUILD_HOST\""   | tee -a "$BUILD_HEADER"
echo "#define g_make_build_kernel_revision  \"$BUILD_KERNEL\"" | tee -a "$BUILD_HEADER"

echo "#define g_make_svn_version    \"$BUILD_SVN_VERSION\""   | tee -a "$BUILD_HEADER"
echo "#define g_make_major_version  \"$BUILD_MAJOR_VERSION\"" | tee -a "$BUILD_HEADER"
echo "#define g_make_minor_version  \"$BUILD_MINOR_VERSION\"" | tee -a "$BUILD_HEADER"
echo "#define g_make_build_version  \"$BUILD_MAJOR_VERSION.$BUILD_MINOR_VERSION.$BUILD_SVN_VERSION\"" | tee -a "$BUILD_HEADER"


echo " "   | tee -a "$BUILD_HEADER"
echo "#endif //__BUILD_INFO_H__"   | tee -a "$BUILD_HEADER"

echo "-- written to $BUILD_HEADER"

