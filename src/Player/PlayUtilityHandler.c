/**
 *  PlayUtility4LowerInterfaces.c文件
 *  主要有以下功能：
 *          提供接口供codec的调用接口
 *
 **/

#include <stdio.h>
#include <sys/select.h>
#include <fcntl.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "log/LogC.h"
#include "LogPlayer.h"
#include "ThreadTask.h"
#include "MessageQueue.h"

#include "PlayUtilityMessageType.h"
#include "AudioCtrl/AudioUtilityInterfaces.h"
#include "Codecs/CodecUtilityInterfaces.h"
#include "PlayUtilityHandler.h"


#define PLAYER_CMD_RECEIVING_DELAY_TIME       0//units: usec    // (10*1000+1500) //


// Two Thread functions
static int PlayerBasicCtrlThread(void *args);
int StreamCtrlThreadCreate(StreamCtrl_Type *streamCtrl)
{
    playerLogDebug("StreamCtrlThreadCreate.\n");

    //TODO::    创建并初始化线程函数所需要的参数的结构；
    
    
    //TODO::    创建播放线程，等待播放命令
    ThreadTaskCreateAdvanced("PlayerBasicCtrlThread", PlayerBasicCtrlThread, (void *)streamCtrl);

    sleep(2);
    return 0;
}



//===========================================================================================================================
//===========================================================================================================================


/**
 *  name::  PlayerHighPriorityPlayModeMessageHandle
 *          Handle some of High Priority player messages;
 *  para::  playMode
 *
 **/
static int PlayerHighPriorityPlayModeMessageHandle(int playMode)
{
    playerLogDebug("High Priority Message[%d].\n", playMode);
    int isHandled = 0;
    int tempCmd = (int)playMode;


    if (playMode < PLAY_MODE_RESERVED || playMode > PLAY_MODE_MAX) {
        playerLogWarning("Message[%d] is not High Priority && Untreated.\n", playMode);
        return isHandled;
    }

    //高优先级命令处理；
    switch (tempCmd) {
    case PLAY_MODE_PLAYFIRST: {
            playerLogVerbose("PLAY_MODE_PLAYFIRST \n");
            
            isHandled = 1;
            break;
        }
    case PLAY_MODE_PLAYLAST: {
            playerLogVerbose("PLAY_MODE_PLAYLAST \n");
            isHandled = 1;
            break;
        }
    case PLAY_MODE_PLAYNEXT: {
            playerLogVerbose("PLAY_MODE_PLAYNEXT \n");
            isHandled = 1;
            break;
        }
    case PLAY_MODE_PLAYPREVIOUS: {
            playerLogVerbose("PLAY_MODE_PLAYPREVIOUS \n");
            isHandled = 1;
            break;
        }
        
    case PLAY_MODE_PLAYREPEAT_LIST: {
            playerLogVerbose("PLAY_MODE_PLAYREPEAT_LIST \n");
            isHandled = 1;
            break;
        }
    case PLAY_MODE_PLAYREPEAT_ITEM: {
            playerLogVerbose("PLAY_MODE_PLAYREPEAT_ITEM \n");
            isHandled = 1;
            break;
        }
    case PLAY_MODE_PLAYONCE_BYLIST: {
            playerLogVerbose("PLAY_MODE_PLAYONCE_BYLIST \n");
            isHandled = 1;
            break;
        }
    case PLAY_MODE_PLAYONCE_BYITEM: {
            playerLogVerbose("PLAY_MODE_PLAYONCE_BYITEM \n");
            isHandled = 1;
            break;
        }
    case PLAY_MODE_PLAYSHUFFLE_BYLIST: {
            playerLogVerbose("PLAY_MODE_PLAYSHUFFLE_BYLIST \n");
            isHandled = 1;
            break;
        }
                
    case PLAY_MODE_PLAYBYLISTINDEX: {
            playerLogVerbose("PLAY_MODE_PLAYBYLISTINDEX \n");
            isHandled = 1;
            break;
        }
    case PLAY_MODE_PLAYBYFILEOFFSET: {
            playerLogVerbose("PLAY_MODE_PLAYBYFILEOFFSET \n");
            isHandled = 1;
            break;
        }
    case PLAY_MODE_PLAYBYENTRYID: {
            playerLogVerbose("PLAY_MODE_PLAYBYENTRYID \n");
            isHandled = 1;
            break;
        }
    default: {
            playerLogVerbose("default \n");
            isHandled = -1;
            playerLogWarning("Message[%d] unable to recognize && untreated.\n", playMode);
            break;
        }
    } //end switch

    return isHandled;
}



/**
 *  name::  PlayerHighPriorityPlayListCommandHandle
 *          Handle some of High Priority player messages;
 *  para::  listCmd
 *
 **/
static int PlayerHighPriorityPlayListCommandHandle(int listCmd)
{
    playerLogDebug("High Priority PlayList Command[%d].\n", listCmd);
    int isHandled = 0;
    int tempCmd = (int)listCmd;


    if (listCmd < PLAYLIST_CMD_RESERVED || listCmd > PLAYLIST_CMD_MAX) {
        playerLogWarning("Message[%d] is not High Priority && Untreated.\n", listCmd);
        return isHandled;
    }

    //高优先级命令处理；
    switch (tempCmd) {
    case PLAYLIST_CMD_ADD_ITEMS: {
            playerLogVerbose("PLAYLIST_CMD_ADD_ITEMS \n");
            isHandled = 1;
            break;
        }
    case PLAYLIST_CMD_ADD_PLAYLIST: {
            playerLogVerbose("PLAYLIST_CMD_ADD_PLAYLIST \n");
            isHandled = 1;
            break;
        }
    case PLAYLIST_CMD_DELETE_ITEMS: {
            playerLogVerbose("PLAYLIST_CMD_DELETE_ITEMS \n");
            isHandled = 1;
            break;
        }
    case PLAYLIST_CMD_DELETE_PLAYLIST: {
            playerLogVerbose("PLAYLIST_CMD_DELETE_PLAYLIST \n");
            isHandled = 1;
            break;
        }
    case PLAYLIST_CMD_SORTBY_FILENAME: {
            playerLogVerbose("PLAYLIST_CMD_SORTBY_FILENAME \n");
            isHandled = 1;
            break;
        }
    case PLAYLIST_CMD_SORTBY_TIMELENGTH: {
            playerLogVerbose("PLAYLIST_CMD_SORTBY_TIMELENGTH \n");
            isHandled = 1;
            break;
        }
    case PLAYLIST_CMD_SORTBY_FILESIZE: {
            playerLogVerbose("PLAYLIST_CMD_SORTBY_FILESIZE \n");
            isHandled = 1;
            break;
        }
    case PLAYLIST_CMD_SORTBY_FILETYPE: {
            playerLogVerbose("PLAYLIST_CMD_SORTBY_FILETYPE \n");
            isHandled = 1;
            break;
        }
    default: {
            playerLogVerbose("default \n");
            isHandled = -1;
            playerLogWarning("Message[%d] unable to recognize && untreated.\n", listCmd);
            break;
        }
    } //end switch


    return isHandled;
}




/**
 *  name::  PlayerBasicCtrlThread
 *          Player thread of handling play ctrl commands ;
 *  para::  args
 *
 **/
static int PlayerBasicCtrlThread(void *args)
{
    PlayerCommand_Type  playCmd;
    StreamCtrl_Type    *streamCtrl;
    MessageQueue_Type   messageQueue ;
    volatile int    cont = 0;   //强制每次都要访问内存
    int             ret;

    //TODO:: 得到消息队列
    streamCtrl = (StreamCtrl_Type *)args;
    messageQueue = streamCtrl->msgQueue;
    playerLogDebug("Get args(streamCtrl)[%p], messageQueue[%p]\n", args, messageQueue);

    while (1) {
        //TODO:: 使用select延时取消息
        printf("PlayerBasicCtrlThread   trap loops....[%d]\n", cont);
        fd_set readSet;
        struct timeval time;
        int sec = 365 * 24 * 3600;

        time.tv_sec = sec ;
        time.tv_usec = 0;
        FD_ZERO(&readSet);
        FD_SET(messageQueue->fd[MessageQueue_Read_FD], &readSet);
        ret = select(messageQueue->fd[MessageQueue_Read_FD] + 1, &readSet, NULL, NULL, &time);
        if ( ret != 1) {
            playerLogWarning("select delay time return value = [%d].\n", ret);
            continue;
        }

        memset(&playCmd, 0, sizeof(playCmd));
        ret = MessageQueueDequeue(messageQueue, (char *)&playCmd, PLAYER_CMD_RECEIVING_DELAY_TIME);
        playerLogDebug("Get cmd from messageQueue, cmd[%d], subCMD[%d], arg1[%d], arg2[%d]\n", playCmd.cmd, playCmd.subCMD, playCmd.arg1, playCmd.arg2);
        if (PLAY_CMD_RESERVED > playCmd.cmd || playCmd.cmd > PLAY_CMD_MAX) { //PLAY_MESSAGE_VALUE_MAX
            playerLogError("Received cmd [%d] is error, unable to recognize.\n", playCmd.cmd);
            return -1;
        }

        //TODO:: Handle Play Mode Message
        if (PlayerHighPriorityPlayModeMessageHandle(playCmd.cmd) == 1) {
            playerLogVerbose("High Priority Message[%d] Has been processed.\n", playCmd.cmd);
            continue;
        }
        
        //TODO:: Handle PlayList command 
        if (PlayerHighPriorityPlayListCommandHandle(playCmd.cmd) == 1) {
            playerLogVerbose("High Priority Message[%d] Has been processed.\n", playCmd.cmd);
            continue;
        }


        //TODO:: 处理播放命令，并设置播放状态
        switch (playCmd.cmd) {
        case PLAY_CMD_OPEN: {
                playerLogVerbose("PLAY_CMD_OPEN\n");
                PlayerOptionsOpen();
                break;
            }
        case PLAY_CMD_PLAY: {
                playerLogVerbose("PLAY_CMD_PLAY\n");
                PlayerOptionsPlay();
                break;
            }
        case PLAY_CMD_CLOSE: {
                playerLogVerbose("PLAY_CMD_CLOSE\n");
                PlayerOptionsClose();
                break;
            }
        case PLAY_CMD_PAUSE: {
                playerLogVerbose("PLAY_CMD_PAUSE\n");
                PlayerOptionsPause();
                break;
            }
        case PLAY_CMD_RESUME: {
                playerLogVerbose("PLAY_CMD_RESUME\n");
                PlayerOptionsResume();
                break;
            }
        case PLAY_CMD_STOP: {
                playerLogVerbose("PLAY_CMD_STOP\n");
                PlayerOptionsStop();
                break;
            }
        case PLAY_CMD_SEEKTO: {
                playerLogVerbose("PLAY_CMD_SEEKTO\n");
                PlayerOptionsSeekto();
                break;
            }
        case PLAY_CMD_FASTFORWARD: {
                playerLogVerbose("PLAY_CMD_FASTFORWARD\n");
                PlayerOptionsFastForward();
                break;
            }
        case PLAY_CMD_FASTREWIND: {
                playerLogVerbose("PLAY_CMD_FASTREWIND\n");
                PlayerOptionsFastRewind();
                break;
            }
        case PLAY_CMD_SEEKTO_START: {
                playerLogVerbose("PLAY_CMD_SEEKTO_START\n");
                PlayerOptionsSeektoStart();
                break;
            }
        case PLAY_CMD_SEEKTO_END: {
                playerLogVerbose("PLAY_CMD_SEEKTO_END\n");
                PlayerOptionsSeektoEnd();
                break;
            }
        default:
            playerLogVerbose("default\n");
            playerLogWarning("Command[%d] unable to recognize && untreated.\n", playCmd.cmd);
            break;
        } //end switch

        cont++;
        printf("While 1 loop account times:[%d]\n", cont);
    } //end wihle 1

    playerLogDebug("deal end...\n");
    return 0;
}







//=================================================================================================
//=================================================================================================

















