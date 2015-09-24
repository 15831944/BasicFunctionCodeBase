/**
 *  BasicPlayer.c文件
 *  主要有以下功能：
 *          播放器基本功能的抽象
 *          基本功能接口
 *          需要把各个接口获取到的数据封装（封装成json格式 或ini或xml文件）
 *
 **/

#ifdef __cplusplus
extern "C" {
#endif

#include "LogC.h"
#include "LogPlayer.h"

#include "PlayUtilityMessageType.h"


#ifdef __cplusplus
}
#endif

#include "BasicPlayer.h"
#include "PlayUtility4UpperInterfaces.h"



namespace Player
{

using namespace std;

BasicPlayer :: BasicPlayer()
{


}

BasicPlayer :: ~BasicPlayer()
{


}


/** 基本的播放控制 **/
//============================================================================================================

// 播放方式
int BasicPlayer :: PlayFirst(void)
{
    playerLogDebug("BasicPlayer :: PlayFirst\n");
    StreamPlayerUtilityModePlayFirst(0);

    return 0;
}


int BasicPlayer :: PlayLast(void)
{
    playerLogDebug("BasicPlayer :: PlayLast\n");
    StreamPlayerUtilityModePlayLast(0);



    return 0;
}


int BasicPlayer :: PlayNext(void)
{
    playerLogDebug("BasicPlayer :: PlayNext\n");
    StreamPlayerUtilityModePlayNext(0);


    return 0;
}


int BasicPlayer :: PlayPrevious(void)
{
    playerLogDebug("BasicPlayer :: PlayPrevious\n");
    StreamPlayerUtilityModePlayPrevious(0);


    return 0;
}


int BasicPlayer :: PlayByListIndex(int)  //从播放列表的第几个位置开始
{
    playerLogDebug("BasicPlayer :: PlayByListIndex\n");
    StreamPlayerUtilityModePlayByListIndex(0);


    return 0;
}


int BasicPlayer :: PlayByFileOffset(int) //从上次离开时记录的播放位置开始  //remember the play location of last times
{
    playerLogDebug("BasicPlayer :: PlayByFileOffset\n");
    StreamPlayerUtilityModePlayByFileOffset(0);


    return 0;
}


int BasicPlayer :: PlayByEntryId(int)  //从指定的列表中的文件ID开始
{
    playerLogDebug("BasicPlayer :: PlayByEntryId\n");
    StreamPlayerUtilityModePlayByEntryId(0);


    return 0;
}





}


//end namespace Player



