/**
 *  PlayCodec4UpperInterfaces.c文件
 *  主要有以下功能：
 *          codec提供接口供上层调用
 *
 **/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#include "log/LogC.h"
#include "LogPlayer.h"
#include "InitPlayer.h"

#include "ThreadMutex.h"
#include "ThreadTask.h"
#include "MessageQueue.h"
#include "PlayUtilityMessageType.h"

#include "../AudioCtrl/AudioUtilityInterfaces.h"
#include "../Codecs/CodecUtilityInterfaces.h"
#include "UtilityTime.h"


#ifdef __cplusplus
}
#endif

#include "BasicPlayer.h"
#include "PlayUtility4UpperInterfaces.h"


using namespace Player;
using namespace std;


int testCommonPlayer(void *args);
extern "C" {
    int BasicPlayerInit()
    {
        playerLogInfo(" BasicPlayerInit\n");

        TimeDelay( TIME_DELAY_MODE_SELECT, 3, 100 * 1000);

        ThreadTaskCreateAdvanced("testCommonPlayer", testCommonPlayer, (void *)"testCommonPlayer");

        printf("threadCreateTest  TestPlayerMain   222\n");
        sleep(5);
        return 0;
    }

}




//================================================================================================
//================================================================================================

int testCommonPlayer(void *args)
{
    int i = 0;
    int usec = 5;

    playerLogInfo(" testCommonPlayer\n");

    Player::BasicPlayer *player = new Player::BasicPlayer();


    for (i = 0; i <= 20000; ) {
        usleep(usec);
        player->Open();
        printf("===========================11111=======iii[%d]======================\n", i++);

        //TimeDelay( TIME_DELAY_MODE_SELECT, 3, 100 * 1000);
        //player->Close();
        //printf("===========================22222=====iii[%d]======================\n", i++);

        TimeDelay( TIME_DELAY_MODE_SELECT, 2, 100 * 1000);
        player->Play();
        printf("===========================33333=====iii[%d]======================\n", i++);

        //TimeDelay( TIME_DELAY_MODE_SELECT, 4, 100 * 1000);
        //player->Stop();
        //printf("===========================44444======iii[%d]======================\n", i++);

        TimeDelay( TIME_DELAY_MODE_SELECT, 1, 100 * 1000);
        player->Pause();
        printf("===========================55555======iii[%d]======================\n", i++);

        TimeDelay( TIME_DELAY_MODE_SELECT, 5, 100 * 1000);
        player->Resume();
        printf("===========================66666======iii[%d]======================\n", i++);

        TimeDelay( TIME_DELAY_MODE_SELECT, 4, 100 * 1000);
        player->FastForward(0);
        printf("===========================77777======iii[%d]======================\n", i++);

        TimeDelay( TIME_DELAY_MODE_SELECT, 2, 100 * 1000);
        player->FastRewind(0);
        printf("===========================88888=======iii[%d]======================\n", i++);

        TimeDelay( TIME_DELAY_MODE_SELECT, 3, 100 * 1000);
        player->SeekTo();
        printf("===========================99999======iii[%d]======================\n", i++);

        TimeDelay( TIME_DELAY_MODE_SELECT, 1, 100 * 1000);
        player->SeekToStart();
        printf("===========================12345=======iii[%d]======================\n", i++);

        TimeDelay( TIME_DELAY_MODE_SELECT, 5, 100 * 1000);
        player->SeekEnd();
        printf("===========================56789=========iii[%d]======================\n", i++);

        TimeDelay( TIME_DELAY_MODE_SELECT, 5, 100 * 1000);
        std::string strValue;
        player->GetAllSubtitleInfo(strValue);
        printf("===========================98765=========iii[%d]======================\n", i++);
    }

    return 0;
}










