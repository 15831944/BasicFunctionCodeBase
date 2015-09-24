/**
 *  
 *  用于对MappingPlayList类的封装
 *  
 **/

using namespace std;
#include <fstream>
#include <iostream>
#include <algorithm>

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>  
#include <stdlib.h>
#include <time.h> 
#include <string.h>
#include <unistd.h>

#include "log/LogC.h"
#include "../LogPlayer.h"

#include "ThreadMutex.h"
#include "MessageQueue.h"

#ifdef __cplusplus
};
#endif


#include "PlayListUtility.h"
#include "MappingPlayList.h"
#include "../PlayUtilityMessage.h"


#define PLAYLIST_FILE_SYSTEM     "playlist_system.list"
#define PLAYLIST_FILE_CUSTOM01   "playlist_custom01.list"
#define PLAYLIST_FILE_CUSTOM02   "playlist_custom02.list"
#define PLAYLIST_FILE_CUSTOM03   "playlist_custom03.list"



static MappingPlayList* g_PlayList;

int LoadList(int playerIndex)
{
    playerLogInfo("PlayFirst, playerIndex[%d]\n", playerIndex);
    g_PlayList = new MappingPlayList();
    (*g_PlayList).addPlayListFile(PLAYLIST_FILE_SYSTEM);
    (*g_PlayList).addPlayListFile(PLAYLIST_FILE_CUSTOM01);
    (*g_PlayList).addPlayListFile(PLAYLIST_FILE_CUSTOM02);
    (*g_PlayList).addPlayListFile(PLAYLIST_FILE_CUSTOM03);
     
    (*g_PlayList).loadPlayListMap();
    
    return 0;
}



int GetPlayListFirst(std::string& fileUrl)
{
    PlayListItemS playListItem;
    
    (*g_PlayList).getFirstItem(playListItem);
    fileUrl.append(playListItem.Path);
    fileUrl.append("/");
    fileUrl.append(playListItem.Name);
    
    playerLogInfo("GetPlayListFirst, fileUrl[%d]\n", fileUrl.c_str());
    return 0;
}


int GetPlayListLast(std::string& fileUrl)
{
    PlayListItemS playListItem;
    (*g_PlayList).getLastItem(playListItem);
    
    fileUrl.append(playListItem.Path);
    fileUrl.append("/");
    fileUrl.append(playListItem.Name);
    
    playerLogInfo("GetPlayListLast, fileUrl[%d]\n", fileUrl.c_str());

    return 0;
}


int GetPlayListCurrent(std::string& name)
{
    PlayListItemS playListItem;
    (*g_PlayList).getCurrentPlayingItem(playListItem);
    name.append(playListItem.Name);
    
    return 0;
}


int GetPlayListPrevious(std::string& name, std::string& fileUrl)
{
    PlayListItemS playListItem;
    (*g_PlayList).getPreviousByItem(name, playListItem);
    
    fileUrl.append(playListItem.Path);
    fileUrl.append("/");
    fileUrl.append(playListItem.Name);
    
    playerLogInfo("GetPlayListPrevious, fileUrl[%d]\n", fileUrl.c_str());

    return 0;
}


int GetPlayListNext(std::string& name, std::string& fileUrl)
{
    PlayListItemS playListItem;
    (*g_PlayList).getNextByItem(name, playListItem);
    
    fileUrl.append(playListItem.Path);
    fileUrl.append("/");
    fileUrl.append(playListItem.Name);
    
    playerLogInfo("GetPlayListNext, fileUrl[%d]\n", fileUrl.c_str());
    return 0;
}




/////// 
int GetPlayRepeatList(int playerIndex)    //循环播放 all repeat
{
    playerLogInfo("SetPlayRepeatList, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}


int GetPlayRepeatItem(std::string& fileUrl)   //单曲循环 repeat once
{
    playerLogInfo("SetPlayRepeatItem\n");
    PlayListItemS playListItem;
    (*g_PlayList).getCurrentPlayingItem(playListItem);
    
    fileUrl.append(playListItem.Path);
    fileUrl.append("/");
    fileUrl.append(playListItem.Name);
    
    playerLogInfo("GetPlayListNext, fileUrl[%d]\n", fileUrl.c_str());
    return 0;
}


int GetPlayShuffleByList(int playerIndex)   //随机播放 shuffle
{
    playerLogInfo("SetPlayShuffleByList, playerIndex[%d]\n", playerIndex);
    
    (*g_PlayList).sortPlayListByField(SortField_FileName, SortOrder_Random);
    
    
    return 0;
}


int GetPlayOnceByList(int playerIndex)    //顺序播放 order     
{
    playerLogInfo("SetPlayOnceByList, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}

 
int GetPlayOnceByItem(int playerIndex)     //单曲播放 once
{
    playerLogInfo("SetPlayOnceByItem, playerIndex[%d]\n", playerIndex);
    
    
    
    return 0;
}




///// 
int SetPlayListByIndex(int playerIndex)    //须记录上次的播放文件
{
    playerLogInfo("SetPlayListByIndex, playerIndex[%d]\n", playerIndex);
    
    
    return 0;
}


int SetPlayFileByOffset(int playerIndex)   //按照上次退出时的播放位置开始播放
{
    playerLogInfo("SetPlayFileByOffset, playerIndex[%d]\n", playerIndex);
   
    
    return 0;
}





/////用于UI控制
int AddItemToPlayList(std::string& url)
{
    playerLogInfo("AddItemToPlayList\n");
    
    PlayListItemS* playListItem;
    //get Name
    playListItem->Name = "ERROR";
    
    //get Path
    playListItem->Path = "ERROR";
    
    //get Type
    playListItem->Type = "ERROR";
    
    //get Size
    std::ifstream fin(url.c_str());
    fin.seekg(0, ios::end);         //设置文件指针到文件流的尾部
    long size = fin.tellg();        //读取文件指针的位置
    fin.close();                    //关闭文件流
    playListItem->Size = size;  
    
    //get AddTime
    playListItem->AddTime = "ERROR";
    
    //get LastPlayPosition
    playListItem->LastPlayPosition = "ERROR";
     
    //get Duration
    playListItem->Duration = "error";
    
    //get Singer
    playListItem->Singer = "error";
    
    //get AVinfo
    playListItem->AVinfo = "error";
    
    //get IsCurrentPlaying
    playListItem->IsCurrentPlaying = 0;
    
    //get random value
    srand((int)time(NULL));     //每次执行种子不同，生成不同的随机数
    playListItem->RandomValue = Random(10000);
    
    (*g_PlayList).addItemToPlayListMap(playListItem); 
    
    return 0;
}


int DeleteItemFromPlayList(std::string& name)
{
    playerLogInfo("DeleteItemFromPlayList\n");
    
    PlayListItemS playListItem;
    (*g_PlayList).deleteItemToPlayListMap(name, playListItem);
    
    return 0;
}



int SortByFileName(int orderMode)
{
    playerLogInfo("SortByFileName, orderMode[%d]\n", orderMode);
    if(orderMode == 0){
        (*g_PlayList).sortPlayListByField(SortField_FileName, SortOrder_Ascending);
    } else {
        (*g_PlayList).sortPlayListByField(SortField_FileName, SortOrder_Descending);
    }
    return 0;
}


int SortByFilePath(int orderMode)
{
    playerLogInfo("SortByFilePath, orderMode[%d]\n", orderMode);
    if(orderMode == 0){
        (*g_PlayList).sortPlayListByField(SortField_FilePath, SortOrder_Ascending);
    } else {
        (*g_PlayList).sortPlayListByField(SortField_FilePath, SortOrder_Descending);
    }
    return 0;
}


int SortByFileType(int orderMode)
{
    playerLogInfo("SortByFileType, orderMode[%d]\n", orderMode);
    if(orderMode == 0){
        (*g_PlayList).sortPlayListByField(SortField_FileType, SortOrder_Ascending);
    } else {
        (*g_PlayList).sortPlayListByField(SortField_FileType, SortOrder_Descending);
    }
    return 0;
}


int SortByFileSize(int orderMode)
{
    playerLogInfo("SortByFileSize, orderMode[%d]\n", orderMode);
    if(orderMode == 0){
        (*g_PlayList).sortPlayListByField(SortField_FileSize, SortOrder_Ascending);
    } else {
        (*g_PlayList).sortPlayListByField(SortField_FileSize, SortOrder_Descending);
    }
    return 0;
}


int SortByPlayDuration(int orderMode)
{
    playerLogInfo("SortByPlayDuration, orderMode[%d]\n", orderMode);
    if(orderMode == 0){
        (*g_PlayList).sortPlayListByField(SortField_PlayDuration, SortOrder_Ascending);
    } else {
        (*g_PlayList).sortPlayListByField(SortField_PlayDuration, SortOrder_Descending);
    }
    return 0;
}


int SortBySinger(int orderMode)
{
    playerLogInfo("SortBySinger, orderMode[%d]\n", orderMode);
    if(orderMode == 0){
        (*g_PlayList).sortPlayListByField(SortField_Singer, SortOrder_Ascending);
    } else {
        (*g_PlayList).sortPlayListByField(SortField_Singer, SortOrder_Descending);
    }
    return 0;
}

























