#ifndef __MESSAGE_QUEUE_H__
#define __MESSAGE_QUEUE_H__


enum {
    MessageQueue_Read_FD = 0,
    MessageQueue_Write_FD = 1
};

struct MessageQueue {
	int size; 	// length of  message
	int fd[2];
};
typedef struct MessageQueue*   MessageQueue_Type;


#ifdef __cplusplus
extern "C" {
#endif



MessageQueue_Type MessageQueueCreate(int messageSize);
int MessageQueueDelete(MessageQueue_Type messageQueue);


int MessageQueueEnqueue(MessageQueue_Type messageQueue, char* message, unsigned long usec);
int MessageQueueDequeue(MessageQueue_Type messageQueue, char* message, unsigned long usec);


int GetMessageQueueFd(MessageQueue_Type messageQueue);








#ifdef __cplusplus
}
#endif

#endif //__MESSAGE_QUEUE_H__





/**
 *  
 *  在linux进程间通信(IPC)可以通过信号量、文件系统、消息队列、共享内存还有管道来实现的。其中消息队列、内存管理是在System V中提出的。
进程通信间涉及到了管道，而且管道在shell命令中也大有用处。那就简要说说管道：
管道顾名思义，你可以将其理解为日常生活中的管子，一边流入，一边流出。它可以有半双工和全双工。半双工就是只能一边流入，另一边流出；全双工则是一边可以流入，也可以流出。
pipe就是一种半双工的管道。其中，fd[1] 用来向管道中写入数据，而fd[0]在另一端用来读出数据。如果现有两个进程要利用pipe进行通信。此时，就要保证只能有一个写入端和一个读出端，即：fd[1]和fd[0]只能有一个。
 *  
 *  fifo是一种全双工，即：它的一端既可以进行读取fd[0]，也可以进行写入fd[1]。
正因为它的这种通信方式，使其可以用来涉及基于C/S模式的网络通信。具体做法：
首先让服务器产生一个服务器端的FIFO，然后让各个客户端产生以其PID为名称的客户端的FIFO，在客户于服务器进行通信时，客户端向服务器端发送自己的PID，以使服务器对客户的请求进行响应时，向其客户端的FIFO写入响应信息。代码实现客户端和服务器进行各自的名称和PID交换。
 *  
 **/