#include "equipment.h"

int livingroomLightInit(int pinNum);				//一些函数声明
int livingroomLightOpen(int pinNum);
int livingroomLightClose(int pinNum);
struct Equipment *addLivingroomLightToLink(struct Equipment *phead);

struct Equipment livingroomLight = {		//“客厅灯”设备链表节点
	.equipName = "livingroomLight",
	.pinNum = 23,							//树莓派gpio引脚23
	.Init = livingroomLightInit,
	.open = livingroomLightOpen,
	.close = livingroomLightClose,
};

int livingroomLightInit(int pinNum)				//初始化函数
{
	pinMode(pinNum,OUTPUT);					//配置引脚为输出引脚
	digitalWrite(pinNum,HIGH);				//引脚输出高电平，即默认为关闭状态
}

int livingroomLightOpen(int pinNum)				//打开函数
{
	digitalWrite(pinNum,LOW);
}

int livingroomLightClose(int pinNum)			//关闭函数
{
	digitalWrite(pinNum,HIGH);
}


struct Equipment *addLivingroomLightToLink(struct Equipment *phead)			//头插法将设备节点加入设备工厂链表函数
{
	if(phead == NULL){
		return &livingroomLight;
	}else{
		livingroomLight.next = phead;
		phead = &livingroomLight;
		return phead;
	}
}
