#ifndef __PLAYER_LIST_UTILITY_INTERFACES_H__
#define __PLAYER_LIST_UTILITY_INTERFACES_H__



/**
 *  PlayListUtility提供的接口
 **/


int PlayListUtilityPlayFirst(int playerIndex);
int PlayListUtilityPlayLast(int playerIndex);
int PlayListUtilityPlayPrevious(int playerIndex);
int PlayListUtilityPlayNext(int playerIndex);


/////// 
int PlayListUtilityPlayRepeatList(int playerIndex);    //循环播放 all repeat
int PlayListUtilityPlayRepeatItem(int playerIndex);    //单曲循环 repeat once
int PlayListUtilityPlayShuffleByList(int playerIndex);   //随机播放 shuffle
int PlayListUtilityPlayOnceByList(int playerIndex);      //顺序播放 order      
int PlayListUtilityPlayOnceByItem(int playerIndex);      //单曲播放 once


///// 
int PlayListUtilityPlayListByIndex(int playerIndex);    //须记录上次的播放文件
int PlayListUtilityPlayFileByOffset(int playerIndex);   //按照上次退出时的播放位置开始播放


///sort
int PlayListUtilitySortByFileName(int playerIndex);
int PlayListUtilitySortByFilePath(int playerIndex);
int PlayListUtilitySortByFileType(int playerIndex);
int PlayListUtilitySortByFileSize(int playerIndex);
int PlayListUtilitySortByPlayDuration(int playerIndex);
int PlayListUtilitySortBySinger(int playerIndex);













#endif  //__PLAYER_LIST_UTILITY_INTERFACES_H__



