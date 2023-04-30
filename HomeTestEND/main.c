#include <stdio.h>
#include <string.h>
#include "equipment.h"
#include "command.h"
#include <pthread.h>
#include <unistd.h>

#include <curl/curl.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char buf[1024] = {'\0'};		//用于存放翔云返回的数据

struct Equipment *findEquipByName(char *name,struct Equipment *phead);		//一些函数声明
struct Command *findCommandByName(char *name,struct Command *phead);
char *getPicBase64FromFile(char *filePath);
size_t readData(void *ptr, size_t size, size_t nmemb, void *stream);
unsigned int postUrl();

void *voiceControlThread(void *data);
void *socketControlThread(void *data);
void *socketReadThread(void *data);
void *fireAlarmThread(void *data);

struct Equipment *equiphead = NULL;			//设备工厂链表头节点
struct Command *cmdhead = NULL;				//指令控制工厂链表节点头
struct Command *socketHandler = NULL;		//“网络控制线程”执行的函数使用到的全局变量


int main()
{
	if(wiringPiSetup() == -1){					//使用wiringPi库需要初始化
		printf("wiringPiSetup failed!\n");
		return -1; 
	}

	equiphead = addBathroomLightToLink(equiphead);			//各设备加入设备工厂
	equiphead = addSecondfloorLightToLink(equiphead);	
	equiphead = addLivingroomLightToLink(equiphead);
	equiphead = addRestaurantLightToLink(equiphead);
	equiphead = addFireDetectionToLink(equiphead);
	equiphead = addBuzzerToLink(equiphead);
	equiphead = addEleLockToLink(equiphead);

	cmdhead = addVoiceControlToLink(cmdhead);				//各指令控制加入指令控制工厂
	cmdhead = addSocketControlToLink(cmdhead);

	struct Equipment *tmpequiphead = equiphead;
	while(tmpequiphead != NULL){						//设备工厂所有设备初始化
		tmpequiphead->Init(tmpequiphead->pinNum);
		tmpequiphead = tmpequiphead->next;
	}

	pthread_t voiceControl_thread;
	pthread_t socketControl_thread;
	pthread_t fireAlarm_thread;
	pthread_create(&voiceControl_thread,NULL,voiceControlThread,NULL);		//创建线程：语音控制
	pthread_create(&socketControl_thread,NULL,socketControlThread,NULL);	//创建线程：网络控制
	pthread_create(&fireAlarm_thread,NULL,fireAlarmThread,NULL);			//创建线程：火灾报警系统


	pthread_join(voiceControl_thread, NULL);		//主函数等待线程退出
	pthread_join(socketControl_thread, NULL);		//主函数等待线程退出
	pthread_join(fireAlarm_thread, NULL);			//主函数等待线程退出
	
	return 0;
}


void *voiceControlThread(void *data)			//“语音控制线程”执行的函数
{
	int nread;
	char *temName = NULL;
	struct Command *voiceHandler = NULL;
	struct Equipment *linkHandler;


	voiceHandler = findCommandByName("voiceControl",cmdhead);		//寻找“语音控制”所在节点，返回给voiceHandler
	if(voiceHandler == NULL){
		printf("find voiceHandler error\n");
		pthread_exit(NULL);
	}
	if(voiceHandler->Init(voiceHandler) < 0){				//“语音控制”功能初始化
		printf("voiceControl init error\n");
		pthread_exit(NULL);
	}


	while(1){
		nread = voiceHandler->getCommand(voiceHandler);			//获取指令
		if(nread == 0){											//没有获取到指令
			printf("No command received\n");
		}else{													//获取到指令
			printf("Get voice command:%s\n",voiceHandler->command);

												//以下为根据不同指令执行相应操作

			if(strstr("OpBaLi\r\n",voiceHandler->command) != NULL){
				linkHandler = findEquipByName("bathroomLight",equiphead);
				linkHandler->open(linkHandler->pinNum);
			}

			if(strstr("ClBaLi\r\n",voiceHandler->command) != NULL){
				linkHandler = findEquipByName("bathroomLight",equiphead);
				linkHandler->close(linkHandler->pinNum);
			}

			if(strstr("OpSeLi\r\n",voiceHandler->command) != NULL){
				linkHandler = findEquipByName("secondfloorLight",equiphead);
				linkHandler->open(linkHandler->pinNum);
			}

			if(strstr("ClSeLi\r\n",voiceHandler->command) != NULL){
				linkHandler = findEquipByName("secondfloorLight",equiphead);
				linkHandler->close(linkHandler->pinNum);
			}

			if(strstr("OpLiLi\r\n",voiceHandler->command) != NULL){
				linkHandler = findEquipByName("livingroomLight",equiphead);
				linkHandler->open(linkHandler->pinNum);
			}

			if(strstr("ClLiLi\r\n",voiceHandler->command) != NULL){
				linkHandler = findEquipByName("livingroomLight",equiphead);
				linkHandler->close(linkHandler->pinNum);
			}

			if(strstr("OpReLi\r\n",voiceHandler->command) != NULL){
				linkHandler = findEquipByName("restaurantLight",equiphead);
				linkHandler->open(linkHandler->pinNum);
			}

			if(strstr("ClReLi\r\n",voiceHandler->command) != NULL){
				linkHandler = findEquipByName("restaurantLight",equiphead);
				linkHandler->close(linkHandler->pinNum);
			}

			if(strstr("OpAlLi\r\n",voiceHandler->command) != NULL){
				linkHandler = findEquipByName("bathroomLight",equiphead);
				linkHandler->open(linkHandler->pinNum);
			
				linkHandler = findEquipByName("secondfloorLight",equiphead);
				linkHandler->open(linkHandler->pinNum);
				
				linkHandler = findEquipByName("livingroomLight",equiphead);
				linkHandler->open(linkHandler->pinNum);
				
				linkHandler = findEquipByName("restaurantLight",equiphead);
				linkHandler->open(linkHandler->pinNum);
			}

			if(strstr("ClAlLi\r\n",voiceHandler->command) != NULL){
				linkHandler = findEquipByName("bathroomLight",equiphead);
				linkHandler->close(linkHandler->pinNum);
			
				linkHandler = findEquipByName("secondfloorLight",equiphead);
				linkHandler->close(linkHandler->pinNum);
				
				linkHandler = findEquipByName("livingroomLight",equiphead);
				linkHandler->close(linkHandler->pinNum);
				
				linkHandler = findEquipByName("restaurantLight",equiphead);
				linkHandler->close(linkHandler->pinNum);
			}
			
			if(strstr("OpLock\r\n",voiceHandler->command) != NULL){
				linkHandler = findEquipByName("eleLock",equiphead);
				
				system("raspistill -o image.jpg");						//拍摄照片命名为 image.jpg
				system("convert -resize 400 image.jpg face2.jpg");		//降低拍摄的照片的大小，命名为 face2.jpg
				system("rm image.jpg");									//删除 image.jpg 临时照片
				// face1.jpg 为对比人脸照片，face2.jpg 为即拍人脸照片
				
				postUrl();
				system("rm face2.jpg");
				if(strstr(buf,"是") != NULL){							//如果返回对比数据是同一个人
					linkHandler->open(linkHandler->pinNum);				//开锁
					delay(10000);										//延时10000毫秒=10秒
					linkHandler->close(linkHandler->pinNum);			//关锁
				}
			}
		}
	}
}


void *socketControlThread(void *data)				//“网络控制线程”执行的函数
{
	int c_fd;
	struct sockaddr_in c_addr;
	memset(&c_addr,0,sizeof(struct sockaddr_in));
	socklen_t clen = sizeof(struct sockaddr_in);
	pthread_t socketRead_thread;


	socketHandler = findCommandByName("socketControl",cmdhead);		//寻找“网络控制”所在节点，返回给socketHandler
	if(socketHandler == NULL){
		printf("find socketHandler error\n");
		pthread_exit(NULL);
	}
	if(socketHandler->Init(socketHandler) < 0){				//“网络控制”功能初始化
		printf("socketControl init error\n");
		pthread_exit(NULL);
	}


	while(1){
		c_fd = accept(socketHandler->s_fd,(struct sockaddr*)&c_addr,&clen);		//接收连接请求，阻塞至有客户端完成三次握手
		socketHandler->fd = c_fd;					//将套接字描述符返回给“网络控制”链表节点

		pthread_create(&socketRead_thread,NULL,socketReadThread,NULL);			//创建新线程：用于读取TCP端口指令
	}
}


void *socketReadThread(void *data)				//“读取tcp端口指令线程”执行的函数
{
	int nread;
	struct Equipment *linkHandler;
	memset(socketHandler->command,'\0',sizeof(socketHandler->command));		//将指令存放的空间置空
	nread = read(socketHandler->fd,socketHandler->command,sizeof(socketHandler->command));		//读取指令
	if(nread == 0){
		printf("No command received\n");			//没有读取到指令
	}else{
		printf("Get net command:%s\n",socketHandler->command);		//读取到指令

											//以下为根据不用指令执行相应操作

		if(strstr("OpBaLi\r\n",socketHandler->command) != NULL){
			linkHandler = findEquipByName("bathroomLight",equiphead);
			linkHandler->open(linkHandler->pinNum);
		}

		if(strstr("ClBaLi\r\n",socketHandler->command) != NULL){
			linkHandler = findEquipByName("bathroomLight",equiphead);
			linkHandler->close(linkHandler->pinNum);
		}

		if(strstr("OpSeLi\r\n",socketHandler->command) != NULL){
			linkHandler = findEquipByName("secondfloorLight",equiphead);
			linkHandler->open(linkHandler->pinNum);
		}

		if(strstr("ClSeLi\r\n",socketHandler->command) != NULL){
			linkHandler = findEquipByName("secondfloorLight",equiphead);
			linkHandler->close(linkHandler->pinNum);
		}

		if(strstr("OpLiLi\r\n",socketHandler->command) != NULL){
			linkHandler = findEquipByName("livingroomLight",equiphead);
			linkHandler->open(linkHandler->pinNum);
		}

		if(strstr("ClLiLi\r\n",socketHandler->command) != NULL){
			linkHandler = findEquipByName("livingroomLight",equiphead);
			linkHandler->close(linkHandler->pinNum);
		}

		if(strstr("OpReLi\r\n",socketHandler->command) != NULL){
			linkHandler = findEquipByName("restaurantLight",equiphead);
			linkHandler->open(linkHandler->pinNum);
		}

		if(strstr("ClReLi\r\n",socketHandler->command) != NULL){
			linkHandler = findEquipByName("restaurantLight",equiphead);
			linkHandler->close(linkHandler->pinNum);
		}

		if(strstr("OpAlLi\r\n",socketHandler->command) != NULL){
			linkHandler = findEquipByName("bathroomLight",equiphead);
			linkHandler->open(linkHandler->pinNum);
			
			linkHandler = findEquipByName("secondfloorLight",equiphead);
			linkHandler->open(linkHandler->pinNum);
				
			linkHandler = findEquipByName("livingroomLight",equiphead);
			linkHandler->open(linkHandler->pinNum);
				
			linkHandler = findEquipByName("restaurantLight",equiphead);
			linkHandler->open(linkHandler->pinNum);
		}

		if(strstr("ClAlLi\r\n",socketHandler->command) != NULL){
			linkHandler = findEquipByName("bathroomLight",equiphead);
			linkHandler->close(linkHandler->pinNum);
		
			linkHandler = findEquipByName("secondfloorLight",equiphead);
			linkHandler->close(linkHandler->pinNum);
				
			linkHandler = findEquipByName("livingroomLight",equiphead);
			linkHandler->close(linkHandler->pinNum);
				
			linkHandler = findEquipByName("restaurantLight",equiphead);
			linkHandler->close(linkHandler->pinNum);
		}

		if(strstr("OpLock\r\n",socketHandler->command) != NULL){
			linkHandler = findEquipByName("eleLock",equiphead);
				
			system("raspistill -o image.jpg");						//拍摄照片命名为 image.jpg
			system("convert -resize 400 image.jpg face2.jpg");		//降低拍摄的照片的大小，命名为 face2.jpg
			system("rm image.jpg");									//删除 image.jpg 临时照片
			// face1.jpg 为对比人脸照片，face2.jpg 为即拍人脸照片
				
			postUrl();
			system("rm face2.jpg");
			if(strstr(buf,"是") != NULL){							//如果返回对比数据是同一个人
					linkHandler->open(linkHandler->pinNum);			//开锁
					delay(10000);									//延时10000毫秒=10秒
					linkHandler->close(linkHandler->pinNum);		//关锁
			}
		}		
	}
}

void *fireAlarmThread(void *data)				//“火灾报警器线程”执行的函数
{
	int status;
	struct Equipment *firetmp = NULL;
	struct Equipment *buztmp = NULL;

	firetmp = findEquipByName("fireDetection",equiphead);		//寻找“火焰传感器”链表节点，返回给firetmp
	buztmp = findEquipByName("buzzer",equiphead);				//寻找“蜂鸣器”链表节点，返回给buztmp

	while(1){
		status = firetmp->readStatus(firetmp->pinNum);			//读取“火焰传感器”状态
	
		if(status == 0){						//检测到火焰或强光源
			buztmp->open(buztmp->pinNum);		//打开蜂鸣器
			delay(1000);						//延时1000毫秒=1秒
		}

		if(status == 1){						//未检测到火焰、强光源或解除警报
			buztmp->close(buztmp->pinNum);		//关闭蜂鸣器
		}
	}
}

unsigned int postUrl()
{
		CURL *curl;
        CURLcode res;
        char *key = "xxxxxxxxxxxxxxxxxxxxxxxx";						//用户 OCR Key 值
        char *secret = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";			//用户 OCR Secret 值
        int typeId = 21;											//识别类型 21
        char *format = "xml";										//设置返回格式 "xml"

        char *postString;


        char *bufPic1 = getPicBase64FromFile("./face1.jpg");		//获取图片 1 的 base64流
        char *bufPic2 = getPicBase64FromFile("./face2.jpg");		//获取图片 2 的 base64流

        int len = strlen(key)+strlen(secret)+strlen(bufPic1)+strlen(bufPic2)+124;			//计算所需传参字符串大小
        postString = (char *)malloc(len);													//为传参字符串创建空间
        memset(postString,'\0',len);														//初始化传参字符串空间
        sprintf(postString,"&img1=%s&img2=%s&key=%s&secret=%s&typeId=%d&format=%s",
                        bufPic1,bufPic2,key,secret,typeId,format);							//拼接平台要求的传参字符串

        curl = curl_easy_init();
        if (curl)
        {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postString);    					//指定 post 内容
                curl_easy_setopt(curl, CURLOPT_URL, "https://netocr.com/api/faceliu.do");	//指定 url
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, readData);					//接收到数据，调用回调函数
                res = curl_easy_perform(curl);
				curl_easy_cleanup(curl);													//清除 curl
        }
        return 1;
}

char *getPicBase64FromFile(char *filePath)
{
		int fd;
        int filelen;
        char cmd[128];
        char *bufPic;

        sprintf(cmd,"base64 %s > tmpFile",filePath);		//拼接系统调用字符串
        system(cmd);										//系统调用获取图片 base64流

        fd = open("./tmpFile",O_RDWR);						//存放图片 base64 流的临时文件
        filelen = lseek(fd,0,SEEK_END);						//计算文件字符数
        lseek(fd,0,SEEK_SET);								//指针回到文件头部

        bufPic = (char *)malloc(filelen+2);					//创建空间存放图片 base64流
        memset(bufPic,'\0',filelen+2);						//初始化空间
        read(fd,bufPic,filelen);							//文件内容读取到空间
        close(fd);											//关闭文件描述符

        system("rm -f tmpFile");							//忽略提示关闭临时文件

        return bufPic;										//返回图片 base64流
}

size_t readData(void *ptr, size_t size, size_t nmemb, void *stream)		//回调函数
{
        strncpy(buf,ptr,1024);
}

struct Equipment *findEquipByName(char *name,struct Equipment *phead)		//根据名字寻找设备工厂链表链节函数，并返回链节
{
	struct Equipment *tmp = phead;

	if(phead == NULL){
		return NULL;
	}

	while(tmp != NULL){
		if(strcmp(name,tmp->equipName) == 0){
			return tmp;
		}
		tmp = tmp->next;
	}
	return NULL;
}


struct Command *findCommandByName(char *name,struct Command *phead)			//根据名字寻找指令控制工厂链表链节函数，并返回链节
{
	struct Command *tmp = phead;

	if(phead == NULL){
		return NULL;
	}

	while(tmp != NULL){
		if(strcmp(name,tmp->commandName) == 0){
			return tmp;
		}
		tmp = tmp->next;
	}
	return NULL;
}
