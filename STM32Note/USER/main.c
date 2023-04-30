#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "onenet.h"
#include "M5310.h"
#include <string.h>
#include "RC522.h"
#include "tim.h"
#include "dht11.h"
#include "bh1750.h"

//变量定义
uint8_t Card_Type1[2]; 
uint16_t system_time=0;

//Environmet data
u8 tempH=19;				//温度整数部分
u8 tempL=10;				//温度小数部分
u8 humiH=21;				//湿度整数部分
u8 humiL=11;				//湿度小数部分
float light=34.6;
u8 i;

int main(void)
{	
	unsigned int timeCount1 = 0;	//发送间隔变量
	unsigned int timeCount2 = 0;	//发送间隔变量
	unsigned int pingCount = 0;	//发送间隔变量
	unsigned char *dataPtr = NULL;
	
	//RFID Card ID
	Card_Type1[0]=0x04;
	Card_Type1[1]=0x00;
	
	//System Init
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	Usart1_init(115200);	 //串口初始化为115200
	Usart2_Init(115200);	//串口2，驱动SIM800C用
	Usart3_Init(9600);		//串口3，用于树莓派通信	
	TIM4_Int_Init(1000-1,72-1);			//中断1ms
	
	//模块Init
 	Command_GPIO_Init();	//通用GPIO初始化
	beep=0;
	DHT11_Init();			//初始化dht11温湿度传感器
	BH1750_Init();			//初始化bh1750光照传感器
	RC522_IO_Init();		//RC522初始化
	PcdReset(); 			//复位RC522
	printf("Hardware init OK\r\n");
	delay_ms(100);
	beep=0;
	//初始化M5310并接入服务器
	M5310_Init();
	while(OneNet_DevLink())			//接入OneNET
		delay_ms(500);
	
	//连入服务器提示
	beep=1;
	delay_ms(600);
	beep=0;
	printf("成功接入服务器\r\n");
	
 	while(1)			//一个while间隔50ms
	{	
/************************************数据采集****************************************/
		if(++timeCount1 >= 40)			//5s  
		{
			DHT11_Read_Data(&humiH,&humiL,&tempH,&tempL);
			if(!i2c_CheckDevice(BH1750_Addr))
				light=LIght_Intensity();
			printf("temp:%d.%d   humi:%d.%d  light:%.1f lx\r\n",tempH,tempL,humiH,humiL,light);
			timeCount1=0;
		}	
/*******************************心跳包发送间隔 50s************************************/
		if(++pingCount >= 1200)			//60s		
		{
			if(OneNet_Ping()==0)
				printf("已成功发送一个心跳包！\r\n");
			pingCount=0;
		}
/*********************************数据上传 间隔 5s***********************/
		if(++timeCount2 >= 120)			//3s
		{
			OneNet_SendData();									//发送数据
			UsartPrintf(USART_DEBUG, "OneNet_SendData\r\n");
			timeCount2 = 0;
			M5310_Clear();
		}		
		
/******************************接收下发指令**************************************/
		dataPtr = M5310_GetIPD(0);
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		delay_ms(10);
	
/******************************RFID门禁系统****************************************/ 
		if(MI_OK==PcdRequest(0x52, Card_Type1))  //寻卡函数，如果成功返回MI_OK  打印多次卡号
		{
			uint16_t cardType = (Card_Type1[0]<<8)|Card_Type1[1];//读不同卡的类型
			printf("卡类型：(0x%04X)\r\n",cardType);  //"Card Type(0x%04X):"
			printf("carType=%d\r\n",cardType);
			if(cardType==1024)
			{	
				//蜂鸣器提示
				for(i=0;i<6;i++)
				{
					beep=~beep;
					delay_ms(250);
				}
				//开锁
				for(i=0;i<3;i++)
				{
					lock=1;
					delay_ms(500);
				}
				lock=0;
			}
			cardType=0;
		}
	}	
/***************************************************************************************/	
}
