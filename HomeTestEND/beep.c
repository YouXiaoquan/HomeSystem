#include "equipment.h"

int buzzerInit(int pinNum);					//一些函数声明
int buzzerOpen(int pinNum);
int buzzerClose(int pinNum);
struct Equipment *addBuzzerToLink(struct Equipment *phead);


struct Equipment buzzer = {			//“蜂鸣器”设备链表节点
	.equipName = "buzzer",
	.pinNum = 29,					//树莓派gpio引脚29
	.Init = buzzerInit,
	.open = buzzerOpen,
	.close = buzzerClose,
};


int buzzerInit(int pinNum)					//初始化函数
{
	pinMode(pinNum,OUTPUT);					//配置引脚为输出引脚
	digitalWrite(pinNum,HIGH);				//引脚输出高电平，即默认为关闭状态
}

int buzzerOpen(int pinNum)					//打开函数
{
	digitalWrite(pinNum,LOW);
}

int buzzerClose(int pinNum)					//关闭函数
{
	digitalWrite(pinNum,HIGH);
}


struct Equipment *addBuzzerToLink(struct Equipment *phead)			//头插法将设备节点加入设备工厂链表函数
{
	if(phead == NULL){
		return &buzzer;
	}else{
		buzzer.next = phead;
		phead = &buzzer;
		return phead;
	}
}
