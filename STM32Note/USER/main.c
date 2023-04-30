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

//��������
uint8_t Card_Type1[2]; 
uint16_t system_time=0;

//Environmet data
u8 tempH=19;				//�¶���������
u8 tempL=10;				//�¶�С������
u8 humiH=21;				//ʪ����������
u8 humiL=11;				//ʪ��С������
float light=34.6;
u8 i;

int main(void)
{	
	unsigned int timeCount1 = 0;	//���ͼ������
	unsigned int timeCount2 = 0;	//���ͼ������
	unsigned int pingCount = 0;	//���ͼ������
	unsigned char *dataPtr = NULL;
	
	//RFID Card ID
	Card_Type1[0]=0x04;
	Card_Type1[1]=0x00;
	
	//System Init
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	Usart1_init(115200);	 //���ڳ�ʼ��Ϊ115200
	Usart2_Init(115200);	//����2������SIM800C��
	Usart3_Init(9600);		//����3��������ݮ��ͨ��	
	TIM4_Int_Init(1000-1,72-1);			//�ж�1ms
	
	//ģ��Init
 	Command_GPIO_Init();	//ͨ��GPIO��ʼ��
	beep=0;
	DHT11_Init();			//��ʼ��dht11��ʪ�ȴ�����
	BH1750_Init();			//��ʼ��bh1750���մ�����
	RC522_IO_Init();		//RC522��ʼ��
	PcdReset(); 			//��λRC522
	printf("Hardware init OK\r\n");
	delay_ms(100);
	beep=0;
	//��ʼ��M5310�����������
	M5310_Init();
	while(OneNet_DevLink())			//����OneNET
		delay_ms(500);
	
	//�����������ʾ
	beep=1;
	delay_ms(600);
	beep=0;
	printf("�ɹ����������\r\n");
	
 	while(1)			//һ��while���50ms
	{	
/************************************���ݲɼ�****************************************/
		if(++timeCount1 >= 40)			//5s  
		{
			DHT11_Read_Data(&humiH,&humiL,&tempH,&tempL);
			if(!i2c_CheckDevice(BH1750_Addr))
				light=LIght_Intensity();
			printf("temp:%d.%d   humi:%d.%d  light:%.1f lx\r\n",tempH,tempL,humiH,humiL,light);
			timeCount1=0;
		}	
/*******************************���������ͼ�� 50s************************************/
		if(++pingCount >= 1200)			//60s		
		{
			if(OneNet_Ping()==0)
				printf("�ѳɹ�����һ����������\r\n");
			pingCount=0;
		}
/*********************************�����ϴ� ��� 5s***********************/
		if(++timeCount2 >= 120)			//3s
		{
			OneNet_SendData();									//��������
			UsartPrintf(USART_DEBUG, "OneNet_SendData\r\n");
			timeCount2 = 0;
			M5310_Clear();
		}		
		
/******************************�����·�ָ��**************************************/
		dataPtr = M5310_GetIPD(0);
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		delay_ms(10);
	
/******************************RFID�Ž�ϵͳ****************************************/ 
		if(MI_OK==PcdRequest(0x52, Card_Type1))  //Ѱ������������ɹ�����MI_OK  ��ӡ��ο���
		{
			uint16_t cardType = (Card_Type1[0]<<8)|Card_Type1[1];//����ͬ��������
			printf("�����ͣ�(0x%04X)\r\n",cardType);  //"Card Type(0x%04X):"
			printf("carType=%d\r\n",cardType);
			if(cardType==1024)
			{	
				//��������ʾ
				for(i=0;i<6;i++)
				{
					beep=~beep;
					delay_ms(250);
				}
				//����
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
