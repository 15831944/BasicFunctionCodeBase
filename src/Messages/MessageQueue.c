/*实现子进程向管道中写入数据，父进程读出数据*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>


#include "MessageQueue.h"
#include "logMessage.h"


static int MessageQueueBuildTime()
{
    messageLogInfo("Message Module Build time :"__DATE__" "__TIME__" \n");
    return 0;
}


/**
 *  name::  MessageQueueCreate
 *  para::  messageSize
 *          在消息队列创建时，需指定每条消息的大小，用于入队时压入消息队列和出队时从消息队列中取出的消息的大小；
 *          注意，必须是消息的大小，若出队、入队的消息大小不一致，则可能会出现错误；
 **/
MessageQueue_Type MessageQueueCreate(int messageSize)
{
    MessageQueue_Type messageQueue;

    MessageQueueBuildTime();

    messageQueue = (MessageQueue_Type)malloc(sizeof(struct MessageQueue));
    if (NULL == messageQueue) {
        messageLogError("malloc error.\n");
        return NULL;
    }

    if (pipe(messageQueue->fd)) {
        free(messageQueue);
        messageLogError("pipe error.\n");
        return NULL;
    }
    fcntl(messageQueue->fd[MessageQueue_Read_FD], F_SETFL, fcntl(messageQueue->fd[MessageQueue_Read_FD], F_GETFL) | O_NONBLOCK);
    fcntl(messageQueue->fd[MessageQueue_Write_FD], F_SETFL, fcntl(messageQueue->fd[MessageQueue_Write_FD], F_GETFL) | O_NONBLOCK);

    messageQueue->size = messageSize;

    messageLogDebug("Message Queue Address: [%p]\n", messageQueue);
    return messageQueue;
}


int MessageQueueDelete(MessageQueue_Type messageQueue)
{
    if (messageQueue == NULL)
        return -1;

    close(messageQueue->fd[MessageQueue_Read_FD]);
    close(messageQueue->fd[MessageQueue_Write_FD]);
    free(messageQueue);

    return 0;
}



int MessageQueueEnqueue(MessageQueue_Type messageQueue, char *message, unsigned long usec)
{
    fd_set writeSet;
    struct timeval time;
    int ret;

    if (messageQueue == NULL) {
        messageLogError("messageQueue=[%p].\n", messageQueue);
        return -1;
    }
    messageLogDebug("Enqueue Message Queue Address: [%p]\n", messageQueue);

    if (usec != 0) {
        time.tv_sec = usec / (1000 * 1000);
        time.tv_usec = usec % (1000 * 1000);

        FD_ZERO(&writeSet);
        FD_SET(messageQueue->fd[MessageQueue_Write_FD], &writeSet);
        ret = select(messageQueue->fd[MessageQueue_Write_FD] + 1, NULL, &writeSet, NULL, &time);
        if ( ret < 0) {
            messageLogError("select return value = [%d].\n", ret);
            return -1;
        }
    }

    ret = write(messageQueue->fd[MessageQueue_Write_FD], message, messageQueue->size);
    if (ret != messageQueue->size) {
        messageLogError("write return value = [%d], messageQueue size = [%d].\n", ret, messageQueue->size);
        return -1;
    }

    return 0;
}


int MessageQueueDequeue(MessageQueue_Type messageQueue, char *message, unsigned long usec)
{
    fd_set readSet;
    struct timeval time;
    int ret;

    if (messageQueue == NULL) {
        messageLogError("messageQueue = [%p].\n", messageQueue);
        return -1;
    }
    messageLogDebug("Dequeue Message Queue Address: [%p]\n", messageQueue);

    if (usec != 0) {
        time.tv_sec = usec / (1000 * 1000);
        time.tv_usec = usec % (1000 * 1000);

        FD_ZERO(&readSet);
        FD_SET(messageQueue->fd[MessageQueue_Read_FD], &readSet);
        ret = select(messageQueue->fd[MessageQueue_Read_FD] + 1, &readSet, NULL, NULL, &time);
        messageLogDebug("select ret[%d].\n", ret);
        if ( ret < 0) {
            messageLogError("select return value = [%d].\n", ret);
            return -1;
        }
    }

    ret = read(messageQueue->fd[MessageQueue_Read_FD], message, messageQueue->size);
    if (ret != messageQueue->size) {
        messageLogError("strerror: %s\n", strerror(errno));
        messageLogError("read return value = [%d], messageQueue->size = [%d]\n", ret, messageQueue->size);
        return -1;
    }

    return messageQueue->size;
}


int GetMessageQueueFd(MessageQueue_Type messageQueue)
{
    if (messageQueue == NULL) {
        messageLogError("messageQueue = [%p].\n", messageQueue);
        return -1;
    }

    return messageQueue->fd[MessageQueue_Read_FD];
}









