#!/bin/sh


path=$(cd "$(dirname "$0")"; pwd)
echo "full path to currently executed script is : ${path}"
dir=`dirname $path`


#read -p "Input Path：" SPATH
SPATH=$dir
FILELIST() {
    filelist=`ls $SPATH`
    echo "$filelist"
    for filename in $filelist; do
        if [ -f $filename ]; then
            echo File： $filename
        elif [ -d $filename ]; then
            echo Directory： $filename
            cd $filename
            SPATH=`pwd`
            echo $SPATH
            FILELIST
            cd ..
        else
            echo "$SPATH/$filename is not a common file."
        fi
    done
}
cd $SPATH
FILELIST
echo "Done."






##方法二：
echo "please input the directory："
#read dir
for dir1 in $(find $dir -type d); do
    echo change to $dir1
    cd $dir1
    ls
    echo “----------”
done
cd $dir

