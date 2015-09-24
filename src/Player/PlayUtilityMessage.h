#ifndef __PLAY_UTILITY_MESSAGE_H__
#define __PLAY_UTILITY_MESSAGE_H__

#define PLAYER_CMD_SENDING_DELAY_TIME      (200*1000+1000)   //21ms, units: usec


int PlayCommandToSending(MessageQueue_Type messageQueue, int cmd, int subCMD, int arg1, int arg2, unsigned int microSecond );

int SendPlayMessageToHandler(int playMsg, int playerIndex);











#endif  //__PLAY_UTILITY_MESSAGE_H__

