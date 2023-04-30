#include "equipment.h"

int fireDetectionInit(int pinNum);					//一些函数声明
int readFireDetectionStatus(int pinNum);
struct Equipment *addFireDetectionToLink(struct Equipment *phead);


struct Equipment fireDetection = {			//“火焰传感器”设备链表节点
	.equipName = "fireDetection",
	.pinNum = 25,							//树莓派gpio引脚25
	.Init = fireDetectionInit,
	.readStatus = readFireDetectionStatus,
};

int fireDetectionInit(int pinNum)			//初始化函数
{
	pinMode(pinNum,INPUT);					//配置引脚为输入引脚
	digitalWrite(pinNum,HIGH);				//引脚输出高电平，即默认为关闭状态
}

int readFireDetectionStatus(int pinNum)		//读取“火焰传感器”状态函数
{
	return digitalRead(pinNum);
}


struct Equipment *addFireDetectionToLink(struct Equipment *phead)
{
	if(phead == NULL){
		return &fireDetection;
	}else{
		fireDetection.next = phead;
		phead = &fireDetection;
		return phead;
	}
}
