#!/bin/sh

echo "----------------------------------------------------------------------------"
echo "                             prepare arch libs                              "
echo "----------------------------------------------------------------------------"
export LANG="en_US.utf8"



########################################################################################
###########        prepare ffmpeg && SDL2 libs && includes path           ##############
########################################################################################
LIBRARY_OUTPUT_PATH=$1
SRC_LIBS_PATH=$2
SRC_INCLUDE_PATH=$3
FFMPEG_LIBS_VERSION=$4
SDL2_LIBS_VERSION=$5
TARGET_SYSTEM=$6

### ffmpeg's PATHs of libs && include
FFMPEG_LIBS_SRC_PATH=${SRC_LIBS_PATH}/ffmpeg-${FFMPEG_LIBS_VERSION}-${TARGET_SYSTEM}-libs.tar.gz
FFMPEG_LIBS_DEST_PATH=${LIBRARY_OUTPUT_PATH}/ffmpeg 

FFMPEG_INCLUDE_SRC_PATH=${SRC_INCLUDE_PATH}/ffmpeg-${FFMPEG_LIBS_VERSION}-${TARGET_SYSTEM}-includes.tar.gz
FFMPEG_INCLUDE_DEST_PATH=${SRC_INCLUDE_PATH}/ffmpeg

### SDL2's PATHs of libs && include
SDL2_LIBS_SRC_PATH=${SRC_LIBS_PATH}/SDL2-${SDL2_LIBS_VERSION}-${TARGET_SYSTEM}-libs.tar.gz
SDL2_LIBS_DEST_PATH=${LIBRARY_OUTPUT_PATH}/SDL2

SDL2_INCLUDE_SRC_PATH=${SRC_INCLUDE_PATH}/SDL2-${SDL2_LIBS_VERSION}-${TARGET_SYSTEM}-includes.tar.gz
SDL2_INCLUDE_DEST_PATH=${SRC_INCLUDE_PATH}/SDL2


echo ${SDL2_LIBS_PATH}
echo ${SDL2_INCLUDE_PATH}



########################################################################################
###########        unzip ffmpeg && SDL2 libs && includes files           ##############
########################################################################################

###################      unzip ffmpeg libs && includes files     ######################
if [ -f ${FFMPEG_LIBS_SRC_PATH} ]; then 
    if [ -d ${FFMPEG_LIBS_DEST_PATH} ]; then 
        rm -rf ${FFMPEG_LIBS_DEST_PATH}
    fi
    mkdir -p ${FFMPEG_LIBS_DEST_PATH}
    tar -zxvf ${FFMPEG_LIBS_SRC_PATH}  -C ${FFMPEG_LIBS_DEST_PATH} 
fi
echo "Decompress ${FFMPEG_LIBS_SRC_PATH} to ${FFMPEG_LIBS_DEST_PATH} done.............. "

if [ -f ${FFMPEG_INCLUDE_SRC_PATH} ]; then 
    if [ -d ${FFMPEG_INCLUDE_DEST_PATH} ]; then
        rm -rf ${FFMPEG_INCLUDE_DEST_PATH}
    fi
    mkdir -p ${FFMPEG_INCLUDE_DEST_PATH}
    tar -zxvf ${FFMPEG_INCLUDE_SRC_PATH}  -C ${FFMPEG_INCLUDE_DEST_PATH}
fi
echo "Decompress ${FFMPEG_INCLUDE_SRC_PATH} to ${FFMPEG_INCLUDE_DEST_PATH} done.............. "


###################       unzip SDL2 libs && includes files      ######################
if [ -f ${SDL2_LIBS_SRC_PATH} ]; then 
    if [ -d ${SDL2_LIBS_DEST_PATH} ]; then 
        rm -rf ${SDL2_LIBS_DEST_PATH}
    fi
    mkdir -p ${SDL2_LIBS_DEST_PATH}
    tar -zxvf ${SDL2_LIBS_SRC_PATH}  -C ${SDL2_LIBS_DEST_PATH} 
fi
echo "Decompress ${SDL2_LIBS_SRC_PATH} to ${SDL2_LIBS_DEST_PATH} done.............. "

if [ -f ${SDL2_INCLUDE_SRC_PATH} ]; then 
    if [ -d ${SDL2_INCLUDE_DEST_PATH} ]; then
        rm -rf ${SDL2_INCLUDE_DEST_PATH}
    fi
    mkdir -p ${SDL2_INCLUDE_DEST_PATH}
    tar -zxvf ${SDL2_INCLUDE_SRC_PATH}  -C ${SDL2_INCLUDE_DEST_PATH}
fi
echo "Decompress ${SDL2_INCLUDE_SRC_PATH} to ${SDL2_INCLUDE_DEST_PATH} done.............. "



echo "-- written to "

