#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct Command									//指令控制工厂链表节点定义
{
	char commandName[128];						//“控制方式”名字
	char deviceFilesName[128];					//存放初始化功能需要打开的文件的路径
	char command[32];							//存放指令
	int fd;										//存放文件描述符
	int (*Init)(struct Command *file);			//“初始化”函数指针
	int s_fd;									//存放套接字描述符
	char ipAdress[32];							//存放IP地址
	char port[12];								//存放端口号
	int (*getCommand)(struct Command *cmd);		//“获取指令”函数指针
	char log[1024];								//日志（暂未使用）

	struct Command *next;
};

struct Command *addVoiceControlToLink(struct Command *phead);		//“语音控制”加入指令控制工厂链表函数声明
struct Command *addSocketControlToLink(struct Command *phead);		//“网络控制”加入指令控制工厂链表函数声明
