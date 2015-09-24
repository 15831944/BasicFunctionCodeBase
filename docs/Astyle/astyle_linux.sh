#!/bin/sh


echo "-----------------------------------------------"
echo "        astyle format *.c/*.cpp, etc...        "
echo "-----------------------------------------------"
###########################################################################################
#
#                               astyle 使用常见参数
###########################################################################################
# -C 类中public,pretected,private关键字，一个tab的缩进
# -K switch中case关键字，无缩进
# -H 在c/c++ 关键字的后面增加一个空格
# -U 移除括号两边不必要的空格
# -w 对宏进行对齐处理
# -c 把TAB字符替换成空格
# -p 在运算符号左右加上空格
# --style=linux Linux风格格式和缩进
# --indent=spaces=4 缩进用4个空格
# -suffix=none 不保存原始文件
# -S switch中case关键字，一个tab的缩进
# -N 被namespace包含的block，一个tab的缩进
# --mode=c 格式化的是C/C++的源文件或者头文件（缺省值）
# --mode=java 格式化的是JAVA的源文件
# --exclude=#### 优化时不包含“####”文件或目录
# -Z 修改后保持文件的修改时间不变
# -X 将错误信息输出到标准输出设备（stdout），而不是标准错误设备（stderr）
# -Q 只显示格式化前后发生变化的文件
# -q 不输出任何信息
# -z1 使用windows版本的回车符(CRLF)
# -z2 使用linux版本的回车符(LF)
# --help 显示帮助信息
# -v 显示版本信息
###########################################################################################

echo "    astyle  HELP !!!   "
echo " astyle 常用配置参数说明："
echo "     -C  类中public,pretected,private关键字，一个tab的缩进;"
echo "     -K  switch中case关键字，无缩进"
echo "     -H  在c/c++ 关键字的后面增加一个空格"
echo "     -U  移除括号两边不必要的空格"
echo "     -w  对宏进行对齐处理"
echo "     -c  把TAB字符替换成空格"
echo "     -p  在运算符号左右加上空格"
echo "     -v  显示版本信息"
echo "     -S  switch中case关键字，一个tab的缩进"
echo "     -N  被namespace包含的block，一个tab的缩进"
echo "     -z2  使用linux版本的回车符(LF)"
echo "     --help  显示帮助信息"
echo "     --mode=c  格式化的是C/C++的源文件或者头文件（缺省值）"
echo "     --suffix=none  不保存原始文件"
echo "     --style=linux  Linux风格格式和缩进"
echo "     --indent=spaces=4  缩进用4个空格"


path=$(cd "$(dirname "$0")"; pwd)
echo "full path to currently executed script is : ${path}"
dir=`dirname $path`
basedir=`dirname $dir`
echo "parent dir is $basedir"


##递归遍历子目录， 指定需遍历的子目录路径
SPATH=$basedir/src
FILELIST() {
    filelist=`ls $SPATH`
    echo "$filelist"
    for file_name in $filelist; do
        if [ -f $file_name ]; then 
            #echo File： $file_name
            if [ "${file_name##*.}" = "c" ]; then   ## 判断扩展名，格式化*.c文件
                echo "file[$file_name] extension:[.c]"
                $path/astyle.elf -CKHwcp --style=linux --indent=spaces=4 --align-pointer=name --suffix=none ./*.c
            fi
            if [ "${file_name##*.}" = "cpp" ]; then ## 判断扩展名，格式化*.cpp文件
                echo "file[$file_name] extension:[.cpp]"
                $path/astyle.elf -CKHwcp --style=linux --indent=spaces=4 --align-pointer=name --suffix=none ./*.cpp
            fi
        elif [ -d $file_name ]; then
            echo Directory： $file_name
            cd $file_name
            SPATH=`pwd`
            #echo $SPATH
            FILELIST
            cd ..
        else
            echo "$SPATH/$file_name is not a common file."
        fi
    done
}
cd $SPATH
FILELIST
echo "Done."



echo "astyle working end..."

