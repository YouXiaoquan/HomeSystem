#include "led.h"
void Command_GPIO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOB, ENABLE);	 //使能PA,PC端口时钟
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	//System LED  PC13
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				 
	GPIO_Init(GPIOC, &GPIO_InitStructure);					
	GPIO_SetBits(GPIOC,GPIO_Pin_13);						

	//PB12  led1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;	    		 	
	GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 	
	GPIO_SetBits(GPIOB,GPIO_Pin_12); 
	
	//PB13	led2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;	    		 	
	GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 	
	GPIO_SetBits(GPIOB,GPIO_Pin_13);
	
	//PB14	led3
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;	    		 	
	GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 	
	GPIO_SetBits(GPIOB,GPIO_Pin_14); 
	
	//PB15	led4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;	    		 	
	GPIO_Init(GPIOB, &GPIO_InitStructure);	
	GPIO_SetBits(GPIOB,GPIO_Pin_15); 
	
	//lock   PB8
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	    		 	
	GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 	
	GPIO_ResetBits(GPIOB,GPIO_Pin_8); 
	
	//beep   PB9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	    		 	
	GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 	
	GPIO_SetBits(GPIOB,GPIO_Pin_9); 
}
 
