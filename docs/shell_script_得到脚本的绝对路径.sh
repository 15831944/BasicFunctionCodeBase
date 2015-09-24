#!/bin/sh

#下面两种方法是 直接得到绝对路径，缺陷是对于软链接显示的是软链接所在的目录：
baseDirForScriptSelf=$(cd "$(dirname "$0")"; pwd)
echo "full path to currently executed script is : ${baseDirForScriptSelf}"

#或者
FULLPATH=$(cd "$(dirname "$0")"; pwd)
echo "full path to currently executed script is : ${FULLPATH}"
BASEDIR=`dirname $FULLPATH`
echo "parent dir is $BASEDIR"


#或者  下面的解决了使用ln -s target linkName创造软链接无法正确取到真实脚本的问题
SOURCE="$0"
while [ -h "$SOURCE"  ]; do # resolve $SOURCE until the file is no longer a symlink
    DIR="$( cd -P "$( dirname "$SOURCE"  )" && pwd  )"
    SOURCE="$(readlink "$SOURCE")"
    [[ $SOURCE != /*  ]] && SOURCE="$DIR/$SOURCE" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
done
DIR="$( cd -P "$( dirname "$SOURCE"  )" && pwd  )"



#或者
name=$0
echo "name : $name"
dir=$(dirname ${name})
echo "dir  : $dir"
path=$(pwd)
echo "path : $path"
path=${dir/./${path}}
echo "path : $path"


#或者
path=$(dirname $0)
if [ ${path:0:1} == . ]
then
path=${path/./$(pwd)}
fi











