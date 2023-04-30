#include "equipment.h"

int secondfloorLightInit(int pinNum);				//一些函数声明
int secondfloorLightOpen(int pinNum);
int secondfloorLightClose(int pinNum);
struct Equipment *addSecondfloorLightToLink(struct Equipment *phead);


struct Equipment secondfloorLight = {		//“二楼灯”设备链表节点
	.equipName = "secondfloorLight",
	.pinNum = 22,							//树莓派gpio引脚22
	.Init = secondfloorLightInit,
	.open = secondfloorLightOpen,
	.close = secondfloorLightClose,
	.changeStatus = secondfloorLightChangeStatus,
};


int secondfloorLightInit(int pinNum)			//初始化函数
{
	pinMode(pinNum,OUTPUT);					//配置引脚为输出引脚
	digitalWrite(pinNum,HIGH);				//引脚输出高电平，即默认为关闭状态
}

int secondfloorLightOpen(int pinNum)			//打开函数
{
	digitalWrite(pinNum,LOW);
}

int secondfloorLightClose(int pinNum)			//关闭函数
{
	digitalWrite(pinNum,HIGH);
}


struct Equipment *addSecondfloorLightToLink(struct Equipment *phead)		//头插法将设备节点加入设备工厂链表函数
{
	if(phead == NULL){
		return &secondfloorLight;
	}else{
		secondfloorLight.next = phead;
		phead = &secondfloorLight;
		return phead;
	}
}
