#include "equipment.h"

int eleLockInit(int pinNum);				//一些函数声明
int eleLockOpen(int pinNum);
int eleLockClose(int pinNum);
struct Equipment *addEleLockToLink(struct Equipment *phead);


struct Equipment eleLock = {			//“电磁锁”设备链表节点
	.equipName = "eleLock",
	.pinNum = 1,						//树莓派gpio引脚 1
	.Init = eleLockInit,
	.open = eleLockOpen,
	.close = eleLockClose,
};


int eleLockInit(int pinNum)			//初始化函数
{
	pinMode(pinNum,OUTPUT);					//配置引脚为输出引脚
	digitalWrite(pinNum,HIGH);				//引脚输出高电平，即默认为关闭状态
}

int eleLockOpen(int pinNum)			//打开函数
{
	digitalWrite(pinNum,LOW);
}

int eleLockClose(int pinNum)			//关闭函数
{
	digitalWrite(pinNum,HIGH);
}


struct Equipment *addEleLockToLink(struct Equipment *phead)		//头插法将设备节点加入设备工厂链表函数
{
	if(phead == NULL){
		return &eleLock;
	}else{
		eleLock.next = phead;
		phead = &eleLock;
		return phead;
	}
}
