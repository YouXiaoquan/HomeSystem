#include "command.h"
#include <unistd.h>

int voiceControlInit(struct Command *file);							//“语音控制”功能初始化函数声明
int voiceControlGetCommand(struct Command *cmd);					//“获取指令”函数声明
struct Command *addVoiceControlToLink(struct Command *phead);		//“语音控制”加入指令控制工厂链表函数声明


struct Command voiceControl = {				//“语音控制”链表节点
	.commandName = "voiceControl",
	.deviceFilesName = "/dev/ttyAMA0",
	.command = {'\0'},
	.Init = voiceControlInit,
	.getCommand = voiceControlGetCommand,
	.log = {'\0'},
};


int voiceControlInit(struct Command *file)
{
	int fd;
	if((fd = serialOpen(file->deviceFilesName,9600)) == -1){		//打开树莓派串口，波特率为9600
		exit(-1);
	}
	file->fd = fd;				//打开串口文件成功，返回“文件描述符”到“语音控制”链表节点中
}


int voiceControlGetCommand(struct Command *cmd)					//“获取指令”函数
{
	int nread = 0;
	memset(cmd->command,'\0',sizeof(cmd->command));					//读取串口
	nread = read(cmd->fd,cmd->command,sizeof(cmd->command));		//返回读取到数据的字节数
	return nread;
}


struct Command *addVoiceControlToLink(struct Command *phead)		//头插法将“语音控制”链表节点加入指令控制工厂链表函数
{
	if(phead == NULL){
		return &voiceControl;
	}else{
		voiceControl.next = phead;
		phead = &voiceControl;
		return phead;
	}
}
