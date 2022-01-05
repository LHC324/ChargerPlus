#ifndef _SYSTEMTIMER_H
#define	_SYSTEMTIMER_H

#include "config.h"

/***********************************�����ʱ������***********************************/
#define T_10MS 			1
#define T_20MS 			2
#define T_30MS 			3
#define T_40MS 			4
#define T_50MS 			5
#define T_60MS 			6
#define T_70MS 			7
#define T_80MS 			8
#define T_90MS 			9
#define T_100MS 		10
#define T_200MS 		20
#define T_300MS 		30
#define T_500MS			50

#define T_1S  			100
#define T_2S  			200
#define T_3S  			300
#define T_4S  		    400
#define T_5S  			500
#define T_6S  			600
#define T_7S  			700
#define T_8S  			800
#define T_9S  			900
#define T_10S 			1000
#define T_20S 			2000
#define T_60S		    6000
/***********************************�����ʱ������***********************************/	

/******************************************************************
* 
*
*
*******************************************************************/

typedef struct 
{
	unsigned char Timer8Flag:1;	//��ʱ���������־
	unsigned char Timer8Count;	//ʱ�������
} TIMER8;
 
typedef struct
{
	unsigned short Timer16Flag:1;//
	unsigned short Timer16Count;

}TIMER16;


typedef struct
{
	TIMER8 Excample8;//һ����λ��ϵͳ��ʱ��
	TIMER8 refrMainDly;
	TIMER8 Usart4Dly;			 //��ʱ�����������
	TIMER8 Uasrt2_3Dly;		 //��ʱ�����������
	TIMER8 Lte_reportDly;  //����ģ���ϱ���ʱ��

}PUBLIC_TIMER8;


typedef struct
{
	TIMER16 Excample16;	 //һ��16λ��ϵͳ��ʱ��
	TIMER16 MenuDly16;	 //һ��16λ�Ĳ˵���ʱ��

}PUBLIC_TIMER16;

extern PUBLIC_TIMER8 PublicTimer8;
extern PUBLIC_TIMER16 PublicTimer16;

void Delay1us();
void Delay_us(unsigned char ii);

void Delay1ms();
void Delay_ms(unsigned short time);
void systemTimer(void);



#endif	/* _SYSTEMTIMER_H */
