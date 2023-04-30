/**
	************************************************************
	************************************************************
	************************************************************
	*	文件名： 	onenet.c
	*
	*	作者： 		张继瑞
	*
	*	日期： 		2017-05-08
	*
	*	版本： 		V1.1
	*
	*	说明： 		与onenet平台的数据交互接口层
	*
	*	修改记录：	V1.0：协议封装、返回判断都在同一个文件，并且不同协议接口不同。
	*				V1.1：提供统一接口供应用层使用，根据不同协议文件来封装协议相关的内容。
	************************************************************
	************************************************************
	************************************************************
**/

//单片机头文件
#include "stm32f10x.h"

//网络设备
#include "M5310.h"

//协议文件
#include "onenet.h"
#include "mqttkit.h"

//硬件驱动
#include "usart.h"
#include "delay.h"
//#include "adxl362.h"
#include "led.h"

//C库
#include <string.h>
#include <stdio.h>


#define PROID		"454469"

#define AUTH_INFO	"NBhome666"

#define DEVID		"770099829"

//外部数据集
extern unsigned char buf[128];
extern u8 tempH;				//温度整数部分
extern u8 tempL;				//温度小数部分
extern u8 humiH;				//湿度整数部分
extern u8 humiL;				//湿度小数部分
extern float light;

//==========================================================
//	函数名称：	OneNet_DevLink
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_DevLink(void)
{
	//协议包
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					
	unsigned char *dataPtr;
	_Bool status = 1;
	UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
							"PROID: %s,	AUIF: %s,	DEVID:%s\r\n"
                        , PROID, AUTH_INFO, DEVID);
	if(MQTT_PacketConnect(PROID, AUTH_INFO, DEVID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0){
		M5310_SendData(mqttPacket._data, mqttPacket._len);	 //上传平台
		dataPtr = M5310_GetIPD(300);					//等待平台响应250
		if(dataPtr == NULL)    								//如果dataPtr为空
		printf("dataPtr是空的\r\n");
		//如果dataPtr不为空
		if(dataPtr != NULL){
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK){
				switch(MQTT_UnPacketConnectAck(dataPtr)){
					case 0:UsartPrintf(USART_DEBUG, "Tips:	连接成功\r\n");status = 0;break;
					case 1:UsartPrintf(USART_DEBUG, "WARN:	连接失败：协议错误\r\n");break;
					case 2:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法的clientid\r\n");break;
					case 3:UsartPrintf(USART_DEBUG, "WARN:	连接失败：服务器失败\r\n");break;
					case 4:UsartPrintf(USART_DEBUG, "WARN:	连接失败：用户名或密码错误\r\n");break;
					case 5:UsartPrintf(USART_DEBUG, "WARN:	连接失败：非法链接(比如token非法)\r\n");break;
					default:UsartPrintf(USART_DEBUG, "ERR:	连接失败：未知错误\r\n");break;
				}
			}
		}
		MQTT_DeleteBuffer(&mqttPacket);								//删包
	}
	else
		UsartPrintf(USART_DEBUG, "WARN:	MQTT_PacketConnect Failed\r\n");
	return status;
}


//==========================================================
//	函数名称：	OneNet_Ping
//
//	函数功能：	与onenet创建连接
//
//	入口参数：	无
//
//	返回参数：	1-成功	0-失败
//
//	说明：		与onenet平台建立连接
//==========================================================
_Bool OneNet_Ping(void)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//协议包

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
//	MQTT_DeleteBuffer(&mqttPacket);								//删包
	if(MQTT_PacketPing(&mqttPacket)== 0)
	{
		M5310_SendData(mqttPacket._data, mqttPacket._len);				//上传平台
		dataPtr = M5310_GetIPD(250);									//等待平台响应250
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_PINGRESP)
			{
				status=0;
			}
		}
		else
		{
			return status;
		}
		MQTT_DeleteBuffer(&mqttPacket);								//删包		
	}
	return status;
}

unsigned char OneNet_FillBuf(char *buf)
{
	char text[64];
	
	memset(text, 0, sizeof(text));
	
	strcpy(buf, "{\"datastreams\":[");
	
	//温湿度
	memset(text, 0, sizeof(text));
	sprintf(text, "{\"id\":\"temperature\",\"datapoints\":[{\"value\":%0.1f}]},", tempH+tempL*0.1);
	strcat(buf, text);
	
	memset(text, 0, sizeof(text));
	sprintf(text, "{\"id\":\"humidity\",\"datapoints\":[{\"value\":%0.1f}]},", humiH+humiL*0.1);
	strcat(buf, text);
	
	//光照
	memset(text, 0, sizeof(text));
	sprintf(text, "{\"id\":\"light\",\"datapoints\":[{\"value\":%0.2f}]}", light);
	strcat(buf, text);
	
	strcat(buf, "]}");
	return strlen(buf);
	
}

//==========================================================
//	函数名称：	OneNet_SendData
//	函数功能：	上传数据到平台
//	入口参数：	type：发送数据的格式
//	返回参数：	无	
//==========================================================
void OneNet_SendData(void)
{
	//协议包
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};												
	char buf[256];
	short body_len = 0, i = 0;
	UsartPrintf(USART_DEBUG, "Tips:	OneNet_SendData-MQTT\r\n");
	memset(buf, 0, sizeof(buf));
	
  //获取当前需要发送的数据流的总长度
	body_len = OneNet_FillBuf(buf);
	printf("body_len:%d \r\n",body_len);
	if(body_len)
	{
		//封包
		if(MQTT_PacketSaveData(DEVID, body_len, NULL, 1, &mqttPacket) == 0)						
		{
			for(; i < body_len; i++)
				mqttPacket._data[mqttPacket._len++] = buf[i];
			//上传数据到平台
			M5310_SendData(mqttPacket._data, mqttPacket._len);										
			UsartPrintf(USART_DEBUG, "Send %d Bytes\r\n", mqttPacket._len);
			
			MQTT_DeleteBuffer(&mqttPacket);															//删包
		}
		else
			UsartPrintf(USART_DEBUG, "WARN:	MQTT_NewBuffer Failed\r\n");
	}
}

//==========================================================
//	函数名称：	OneNet_RevPro
//
//	函数功能：	平台返回数据检测
//
//	入口参数：	dataPtr：平台返回的数据
//
//	返回参数：	无
//
//	说明：		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//协议包
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	unsigned short req_len = 0;
	unsigned char type = 0;
	short result = 0;
	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
			case MQTT_PKT_CMD:															//命令下发
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//解出topic和消息体
			if(result == 0)
			{
					UsartPrintf(USART_DEBUG, "cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);
					if(MQTT_PacketCmdResp(cmdid_topic, req_payload, &mqttPacket) == 0)	//命令回复组包
					{
						UsartPrintf(USART_DEBUG, "Tips:	Send CmdResp\r\n");
						M5310_SendData(mqttPacket._data, mqttPacket._len);		//回复命令
						MQTT_DeleteBuffer(&mqttPacket);				//删包
					}
			}     break;
			
		case MQTT_PKT_PUBACK:														//发送Publish消息，平台回复的Ack
			if(MQTT_UnPacketPublishAck(cmd) == 0)
						UsartPrintf(USART_DEBUG, "Tips:	MQTT Publish Send OK\r\n");		break;
		default:	result = -1;	break;
	}
	M5310_Clear();										//清空缓存
	if(result == -1)
			return;
	dataPtr = strchr(req_payload, 'D');					//搜索'D'
	if(dataPtr != NULL && result != -1)					//如果找到了
	{
			dataPtr++;
			while(*dataPtr >= '0' && *dataPtr <= '9')		//判断是否是下发的命令控制数据
			{
				numBuf[num++] = *dataPtr++;
			}
			numBuf[num] = 0;
			num = atoi((const char *)numBuf);				//转为数值形式
	}
	printf("number is:%d\r\n",num);
	switch(num)
	{
		case 11:	Usart_SendString(USART3, (unsigned char *)"openLED1",8);	break;
		case 10:	Usart_SendString(USART3, (unsigned char *)"closeLED1",9);	break;
		case 21:	Usart_SendString(USART3, (unsigned char *)"openLED2",8);	break;
		case 20:	Usart_SendString(USART3, (unsigned char *)"closeLED2",9);	break;
		case 31:	Usart_SendString(USART3, (unsigned char *)"openLED3",8);	break;
		case 30:	Usart_SendString(USART3, (unsigned char *)"closeLED3",9);	break;
		case 41:	Usart_SendString(USART3, (unsigned char *)"openLED4",8);	break;
		case 40:	Usart_SendString(USART3, (unsigned char *)"closeLED4",9);	break;
		case 51:	led1=0; 	break;
		case 50:	led1=1;		break;
		case 61:	led2=0;		break;
		case 60:	led2=1;		break;
		case 71:	led3=0; 	break;
		case 70:	led3=1;		break;		
	}
	
	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
