#include "command.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int socketControlInit(struct Command *file);					//“网络控制”功能初始化函数声明
struct Command *addSocketControlToLink(struct Command *phead);	//“网络控制”加入指令控制工厂链表函数声明

struct Command socketControl = {		//“网络控制”链表节点
	.commandName = "socketControl",
	.command = {'\0'},
	.Init = socketControlInit,
	.ipAdress = "192.168.43.97",		//树莓派连接网络时的IP地址
	.port = "8888",						//树莓派打开待外界连接的端口号
	.log = {'\0'},
};

int socketControlInit(struct Command *file)
{
	int s_fd;											//套接字描述符
	struct sockaddr_in s_addr;
	memset(&s_addr,0,sizeof(struct sockaddr_in));

	s_fd = socket(AF_INET,SOCK_STREAM,0);				//创建套接字
    if(s_fd == -1){										//创建套接字失败时
            perror("socketControl error");
            exit(-1);
    }

	s_addr.sin_family = AF_INET;
    s_addr.sin_port = htons(atoi(file->port));
    inet_aton(file->ipAdress,&s_addr.sin_addr);
	if(bind(s_fd,(struct sockaddr*)&s_addr,sizeof(struct sockaddr_in)) == -1){		//套接字与端口号绑定
    	perror("bind error");
    	exit(-1);
    }

	if(listen(s_fd,10) == -1){				//打开监听
    	perror("listen error");
    	exit(-1);
    }

	file->s_fd = s_fd;						//套接字描述符返回到“网络控制”链表节点
}

struct Command *addSocketControlToLink(struct Command *phead)			//头插法将设备节点加入设备工厂链表函数
{
	if(phead == NULL){
		return &socketControl;
	}else{
		socketControl.next = phead;
		phead = &socketControl;
		return phead;
	}
}
