#include "equipment.h"

int bathroomLightInit(int pinNum);				//一些函数声明
int bathroomLightOpen(int pinNum);
int bathroomLightClose(int pinNum);
struct Equipment *addBathroomLightToLink(struct Equipment *phead);


struct Equipment bathroomLight = {		//“浴室灯”设备链表节点
	.equipName = "bathroomLight",
	.pinNum = 21,						//树莓派gpio引脚21
	.Init = bathroomLightInit,
	.open = bathroomLightOpen,
	.close = bathroomLightClose,
};


int bathroomLightInit(int pinNum)			//初始化函数
{
	pinMode(pinNum,OUTPUT);					//配置引脚为输出引脚
	digitalWrite(pinNum,HIGH);				//引脚输出高电平，即默认为关闭状态
}

int bathroomLightOpen(int pinNum)			//打开函数
{
	digitalWrite(pinNum,LOW);
}

int bathroomLightClose(int pinNum)			//关闭函数
{
	digitalWrite(pinNum,HIGH);
}


struct Equipment *addBathroomLightToLink(struct Equipment *phead)		//头插法将设备节点加入设备工厂链表函数
{
	if(phead == NULL){
		return &bathroomLight;
	}else{
		bathroomLight.next = phead;
		phead = &bathroomLight;
		return phead;
	}
}
