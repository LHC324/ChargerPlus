#ifndef __LTE_H
#define __LTE_H
#include <STC8.h>

#define LTE_POWER_KEY     P25   //4G模块开关脚
#define LTE_RELOAD		  P41	//4G模块重载
#define LTE_RESET         P20   //4G模块复位
#define LTE_LINKA		  P23	//4G模块Link脚
#define LTE_NET_STATE	  P37	//4G模块状态指示

typedef enum
{
	FALSE = 0,
	TRUE = !FALSE,
}bool;

typedef struct
{
	unsigned char* Cmd;
	unsigned char* Ack;
	unsigned char  Timer;	
}AT_COMMAND;

void LTEinit(void);					//初始化
void LTEreset(void);				//复位
void LTEreload(void);				//重载
void LTEconnect(void);			//连接服务器
bool LTEisLinkA(void);			//是否连接上服务器A
bool LTEisLinkB(void);			//是否连接上服务器B
void LTEenable(bool Enable);  //模块启停
void LTEreport(unsigned char* Payload,unsigned Length);   //模块上报数据
//void LTEreciveHandle(unsigned char* Payload,unsigned Length);  //模块数据接收处理
 #endif