title 文件夹 树形目录生成器
echo off
cls

echo 请输入需要生成目录的文件夹，格式举例：目录=F:\Musics1，生成的txt文件请另存为。
set /p 目录=目录=
tree /f "%目录%">%temp%\文件夹树形目录.txt
%temp%\文件夹树形目录.txt