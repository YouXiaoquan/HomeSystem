#ifndef _M5310_H_
#define _M5310_H_





#define REV_OK		0	//接收完成标志
#define REV_WAIT	1	//接收未完成标志


void M5310_Init(void);

void M5310_Clear(void);

void M5310_SendData(unsigned char *data, unsigned short len);

unsigned char *M5310_GetIPD(unsigned short timeOut);


#endif
