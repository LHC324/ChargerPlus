#ifndef __LTE_H
#define __LTE_H
#include <STC8.h>

#define LTE_POWER_KEY     P25   //4Gģ�鿪�ؽ�
#define LTE_RELOAD		  P41	//4Gģ������
#define LTE_RESET         P20   //4Gģ�鸴λ
#define LTE_LINKA		  P23	//4Gģ��Link��
#define LTE_NET_STATE	  P37	//4Gģ��״ָ̬ʾ

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

void LTEinit(void);					//��ʼ��
void LTEreset(void);				//��λ
void LTEreload(void);				//����
void LTEconnect(void);			//���ӷ�����
bool LTEisLinkA(void);			//�Ƿ������Ϸ�����A
bool LTEisLinkB(void);			//�Ƿ������Ϸ�����B
void LTEenable(bool Enable);  //ģ����ͣ
void LTEreport(unsigned char* Payload,unsigned Length);   //ģ���ϱ�����
//void LTEreciveHandle(unsigned char* Payload,unsigned Length);  //ģ�����ݽ��մ���
 #endif