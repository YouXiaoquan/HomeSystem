/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	M5310.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-10-20
	*
	*	版本： 		V1.0
	*
	*	说明： 		M5310驱动
	*
	*	修改记录：	
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备驱动
#include "M5310.h"

//硬件驱动
#include "delay.h"
#include "usart.h"

//C库
#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#define M5310_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"183.230.40.39\",6002\r\n"

unsigned char M5310_buf[128];
unsigned short M5310_cnt = 0, M5310_cntPre = 0;


//==========================================================
//	函数名称：	M5310_Clear
//
//	函数功能：	清空缓存
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void M5310_Clear(void)
{

	memset(M5310_buf, 0, sizeof(M5310_buf));
	M5310_cnt = 0;

}

//==========================================================
//	函数名称：	M5310_WaitRecive
//
//	函数功能：	等待接收完成
//
//	入口参数：	无
//
//	返回参数：	REV_OK-接收完成		REV_WAIT-接收超时未完成
//
//	说明：		循环调用检测是否接收完成
//==========================================================
_Bool M5310_WaitRecive(void)
{

	if(M5310_cnt == 0) 							//如果接收计数为0 则说明没有处于接收数据中，所以直接跳出，结束函数
		return REV_WAIT;
		
	if(M5310_cnt == M5310_cntPre)				//如果上一次的值和这次相同，则说明接收完毕
	{
		M5310_cnt = 0;							//清0接收计数
			
		return REV_OK;							//返回接收完成标志
	}
		
	M5310_cntPre = M5310_cnt;					//置为相同
	
	return REV_WAIT;							//返回接收未完成标志

}

//==========================================================
//	函数名称：	M5310_SendCmd
//
//	函数功能：	发送命令
//
//	入口参数：	cmd：命令
//				res：需要检查的返回指令
//
//	返回参数：	0-成功	1-失败
//
//	说明：		
//==========================================================
_Bool M5310_SendCmd(char *cmd, char *res)
{
	
	unsigned char timeOut = 200;

	Usart_SendString(USART2, (unsigned char *)cmd, strlen((const char *)cmd));
	
	while(timeOut--)
	{
		if(M5310_WaitRecive() == REV_OK)							//如果收到数据
		{
			if(strstr((const char *)M5310_buf, res) != NULL)		//如果检索到关键词
			{
				M5310_Clear();										//清空缓存
				
				return 0;
			}
		}
		
		delay_ms(10);
	}
	
	return 1;

}

//==========================================================
//	函数名称：	M5310_SendData
//
//	函数功能：	发送数据
//
//	入口参数：	data：数据
//				len：长度
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void M5310_SendData(unsigned char *data, unsigned short len)
{

	char cmdBuf[32];
	
	M5310_Clear();								//清空接收缓存
	sprintf(cmdBuf, "AT+CIPSEND=%d\r\n", len);	//发送命令
	if(!M5310_SendCmd(cmdBuf, ">"))				//收到‘>’时可以发送数据
	{
		Usart_SendString(USART2, data, len);	//发送设备连接请求数据
	}

}

//==========================================================
//	函数名称：	M5310_GetIPD
//
//	函数功能：	获取平台返回的数据
//
//	入口参数：	timeOut等待的时间(乘以10ms)
//
//	返回参数：	平台返回的原始数据
//
//	说明：		不同网络设备返回的格式不同，需要去调试
//==========================================================
unsigned char *M5310_GetIPD(unsigned short timeOut)
{

	char *ptrIPD;
	
	do
	{
		if(M5310_WaitRecive() == REV_OK)								//如果接收完成
		{
				//ptrIPD = strstr((char *)M5310_buf, "+IPD,");				//搜索“IPDATA”头
			ptrIPD = strstr((char *)M5310_buf, "IPD,");				//搜索“IPDATA”头
			if(ptrIPD == NULL)											//如果没找到，可能是IPDATA头的延迟，还是需要等待一会，但不会超过设定的时间
			{
				//UsartPrintf(USART_DEBUG, "\"IPD\" not found\r\n");
			}
			else
			{
				//ptrIPD = strchr(ptrIPD, '\n');							//找到'\n'
				ptrIPD = strchr(ptrIPD, ':');							//找到':'
				if(ptrIPD != NULL)
				{
					ptrIPD++;
					return (unsigned char *)(ptrIPD);
				}
				else
					return NULL;
			}
		}
		delay_ms(10);													//延时等待
	} while(timeOut--);
	return NULL;														//超时还未找到，返回空指针
}

//==========================================================
//	函数名称：	M5310_Init
//	函数功能：	初始化M5310
//	入口参数：	无
//	返回参数：	无
//==========================================================
void M5310_Init(void)
{
	M5310_Clear();
	UsartPrintf(USART_DEBUG, "1. AT\r\n");
	while(M5310_SendCmd("AT\r\n", "OK"))                 				//确保模块数据收发正常
		delay_ms(500);	
	
	UsartPrintf(USART_DEBUG, "2. AT+CPIN?\r\n");
	while(M5310_SendCmd("AT+CPIN?\r\n", "+CPIN: READY"))			//确保SIM卡PIN码解锁，返回READY，表示解锁成功	 
		delay_ms(500);

	UsartPrintf(USART_DEBUG, "3. AT+CREG?\r\n");
	while(M5310_SendCmd("AT+CREG?\r\n", "0,1"))  
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "4. AT+CGREG?\r\n");       
//	while(M5310_SendCmd("AT+CGREG?\r\n","OK"))					
	while(M5310_SendCmd("AT+CGREG?\r\n","0,1"))							//检查网络注册状态
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "5. AT+CIPSHUT\r\n");
	while(M5310_SendCmd("AT+CIPSHUT\r\n","SHUT OK"))				//关闭移动场景 		
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "6. AT+CGCLASS=\"B\"\r\n");
	while(M5310_SendCmd("AT+CGCLASS=\"B\"\r\n","OK"))				//设置GPRS移动台类别为B,支持包交换和数据交换	
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "7. AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n");
	while(M5310_SendCmd("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n","OK"))		//设置PDP上下文,互联网接协议,接入点等信息		
		delay_ms(500);	
	
	UsartPrintf(USART_DEBUG, "8. AT+CGATT=1\r\n");
	while(M5310_SendCmd("AT+CGATT=1\r\n","OK"))					       				//附着GPRS业务
		delay_ms(500);

	UsartPrintf(USART_DEBUG, "9. AT+CIPMUX=0\r\n");
	while(M5310_SendCmd("AT+CIPMUX=0\r\n","OK"))					     				//必须为单连接，不然平台IP都连不上
		delay_ms(500);
	
	UsartPrintf(USART_DEBUG, "10. AT+CIPHEAD=1\r\n");
	while(M5310_SendCmd("AT+CIPHEAD=1\r\n","OK"))				              //显示IP头
		delay_ms(500); 
	
	UsartPrintf(USART_DEBUG, "11. AT+CIPSTART\r\n");
	while(M5310_SendCmd(M5310_ONENET_INFO,"CONNECT"))				
		delay_ms(500);
	UsartPrintf(USART_DEBUG, "12. M5310 Init OK\r\n");
}

//==========================================================
//	函数名称：	USART2_IRQHandler
//
//	函数功能：	串口2收发中断
//
//	入口参数：	无
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void USART2_IRQHandler(void)
{
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) //接收中断
	{
		if(M5310_cnt >= sizeof(M5310_buf))	M5310_cnt = 0; //防止串口被刷爆
		M5310_buf[M5310_cnt++] = USART2->DR;
		
		USART_ClearFlag(USART2, USART_FLAG_RXNE);
	}
}
