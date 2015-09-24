#!/bin/sh

basename example.tar.gz .tar.gz
# => example

FILE="example.tar.gz"

echo "${FILE%%.*}"
# => example

echo "${FILE%.*}"
# => example.tar

echo "${FILE#*.}"
# => tar.gz

echo "${FILE##*.}"
# => gz

# 在bash中可以这么写
filename=$(basename "$fullfile")
extension="${filename##*.}"
filename="${filename%.*}"


