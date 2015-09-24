#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#include "Log/LogC.h"
#include "logMessage.h"
#include "Message.h"

#include "MessageQueue.h"



typedef enum {
    Message_Type_PUT,
    Message_Type_GET,
    Message_Type_IN,
    Message_Type_OUT
} Message_Type;


extern int log_message_init();
int main ()
{
    MessageQueue_Type messageQueue;
    pid_t childpid;
    char *data[] = { "put message from child process\n", "get message from child process\n",
                     "in message from child process\n", "out message from child process\n"
                   };
    char recv[64] = {0};
    int ret = 0, i = 0;
    struct timeval tv;

    log_message_init();

    messageQueue = MessageQueueCreate(64);


    if ((childpid = fork()) == -1) {
        perror("fork");
        exit(1);
    }

    if (childpid == 0) {
        ret = MessageQueueEnqueue(messageQueue, data[Message_Type_PUT], Message_Type_PUT * 100000 + 100000);
        messageLogDebug("Message_Type_PUT, ret = [%d]\n", ret);
        ret = MessageQueueEnqueue(messageQueue, data[Message_Type_GET], Message_Type_GET * 100000 + 100000);
        messageLogVerbose("Message_Type_GET, ret = [%d]\n", ret);
        ret = MessageQueueEnqueue(messageQueue, data[Message_Type_IN], Message_Type_IN * 100000 + 100000);
        messageLogInfo("Message_Type_IN, ret = [%d]\n", ret);
        ret = MessageQueueEnqueue(messageQueue, data[Message_Type_OUT], Message_Type_OUT * 100000 + 100000);
        messageLogWarning("Message_Type_OUT, ret = [%d]\n", ret);
    } else {
        for (i = 0; i < 5; i++) {
            ret = MessageQueueDequeue(messageQueue, recv, Message_Type_IN * 10000 + 2000);
            messageLogDebug("recv = [%s], ret = [%d].\n", recv, ret);
        }
        messageLogInfo("ret = [%d].\n", ret);
    }

    tv.tv_sec = 5;
    tv.tv_usec = 0;
    ret = select(0, NULL, NULL, NULL, &tv);
    messageLogInfo("end recv.\n");
    MessageQueueDelete(messageQueue);

    return 0;
}


