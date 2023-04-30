#include "equipment.h"

int restaurantLightInit(int pinNum);				//一些函数声明
int restaurantLightOpen(int pinNum);
int restaurantLightClose(int pinNum);
struct Equipment *addRestaurantLightToLink(struct Equipment *phead);


struct Equipment restaurantLight = {		//“餐厅灯”设备链表节点
	.equipName = "restaurantLight",
	.pinNum = 24,							//树莓派gpio引脚24
	.Init = restaurantLightInit,
	.open = restaurantLightOpen,
	.close = restaurantLightClose,
};


int restaurantLightInit(int pinNum)				//初始化函数
{
	pinMode(pinNum,OUTPUT);					//配置引脚为输出引脚
	digitalWrite(pinNum,HIGH);				//引脚输出高电平，即默认为关闭状态
}

int restaurantLightOpen(int pinNum)				//打开函数
{
	digitalWrite(pinNum,LOW);
}

int restaurantLightClose(int pinNum)			//关闭函数
{
	digitalWrite(pinNum,HIGH);
}


struct Equipment *addRestaurantLightToLink(struct Equipment *phead)			//头插法将设备节点加入设备工厂链表函数
{
	if(phead == NULL){
		return &restaurantLight;
	}else{
		restaurantLight.next = phead;
		phead = &restaurantLight;
		return phead;
	}
}
