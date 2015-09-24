#ifndef __PLAY_LIST_UTILITY_H__
#define __PLAY_LIST_UTILITY_H__

#include <string>

int LoadList(int playerIndex);


//播放方式控制
int GetPlayListFirst(std::string& fileUrl);
int GetPlayListLast(std::string& fileUrl);
int GetPlayListCurrent(std::string& name);
int GetPlayListPrevious(std::string& name, std::string& fileUrl);
int GetPlayListNext(std::string& name, std::string& fileUrl);

/////// 
int GetPlayRepeatList(int playerIndex);    //循环播放 all repeat
int GetPlayRepeatItem(std::string& fileUrl);    //单曲循环 repeat once
int GetPlayShuffleByList(int playerIndex);   //随机播放 shuffle
int GetPlayOnceByList(int playerIndex);      //顺序播放 order      
int GetPlayOnceByItem(int playerIndex);      //单曲播放 once

///// 
int SetPlayListByIndex(int playerIndex);    //须记录上次的播放文件
int SetPlayFileByOffset(int playerIndex);   //按照上次退出时的播放位置开始播放



/////用于UI控制
int AddItemToPlayList(std::string& url);
int DeleteItemFromPlayList(std::string& name);

int SortByFileName(int orderMode);
int SortByFilePath(int orderMode);
int SortByFileType(int orderMode);
int SortByFileSize(int orderMode);
int SortByPlayDuration(int orderMode);
int SortBySinger(int orderMode);














#endif   //__PLAY_LIST_UTILITY_H__

