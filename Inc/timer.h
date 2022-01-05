#ifndef __TIMER_H_
#define __TIMER_H_
/*
	data：固定指前面0x00-0x7F的128个RAM，可以用acc直接读写，速度最快，生成的代码也最小。
	idata：固定指前面0x00-0xFF的256个RAM，其中前128和data的128完全相同，只是访问的方式不同。
	xdata：外部扩展RAM。
	code：ROM。
*/
#include "STC_NVIC.h"
/*软件定时器个数*/
#define TIMER_EVENTS 50U	

#define T0X1  0x80
#define T0X12 0x7F

typedef enum
{
	TIMER0 = 0x00,
	TIMER1,
	TIMER2,
	TIMER3,
	TIMER4
}Timer_TypsDef;

/*定时器0模式*/
typedef enum 
{
	T0_AutoReload_16bit    = 0xF0,
	T0_No_AutoReload_16bit = 0x01,
	T0_AutoReload_8bit     = 0x02,
	/*Same as mode 0, but interrupts are not maskable*/
	T0_SAutoReload_16bit   = 0x03
}Timer0_Mode;

/*定时器1模式*/
typedef enum
{
	T1_AutoReload_16bit    = 0x00,
	T1_No_AutoReload_16bit = 0x10,
	T1_AutoReload_8bit     = 0x20,
	T1_Timer1_Stop         = 0x30
}Timer1_Mode;

/*定时器辅助寄存器*/
typedef struct 
{
	union 
	{
		struct 
		{
			uint8_t bT0x12 : 1;
			uint8_t bT1x12 : 1;
			uint8_t bUART_M0x6 : 1;
			uint8_t bT2R : 1;
			uint8_t bT2_C_T : 1;
			uint8_t bT2x12  : 1;
			uint8_t bEXTRAM : 1;
			uint8_t bS1ST2  : 1;
		}AUXR_Bit;
		uint8_t AUXR_U8;	
	}AUXR;	
}AUXR_Register; //idata AUXR_Register _at_ 0x8e

/*定时器1/0模式选择寄存器*/
typedef struct
{
	union 
	{
		struct 
		{
			uint8_t bT1_GATE : 1;
			uint8_t bT1_C_T  : 1;
			uint8_t bT1_M1   : 1;
			uint8_t bT1_M0   : 1;
			uint8_t bT0_GATE : 1;
			uint8_t bT0_C_T  : 1;
			uint8_t bT0_M1   : 1;
			uint8_t bT0_M0   : 1;	
		}TMOD_Bit;
		uint8_t TMOD_U8;	
	}TMOD;	
}TMOD_Register;


/*定时器3/4控制寄存器*/
typedef struct
{
	union 
	{
		struct 
		{
			uint8_t bT4R : 1;
			uint8_t bT4_C_T : 1;
			uint8_t bT4x12  : 1;
			uint8_t bT4CLKO : 1;
			uint8_t bT3R : 1;
			uint8_t bT3_C_T : 1;
			uint8_t bT3x12  : 1;
			uint8_t bT3CLKO : 1;
		}T4T3M_Bit;
		uint8_t T4T3M_U8;
	}T4T3M;
}T4T3M_Register;

/*掉电唤醒定时器计数寄存器*/
typedef struct
{
	union 
	{
		struct
		{
			uint8_t bWKTEN : 1;
			uint8_t bNULL  : 7;
		}WKTCH_Bit;
		uint8_t WKTCH_U8;	
	}WKTCH;
}WKTCH_Register;

// typedef struct
// {
// 	Timer_TypsDef Instance;
// 	uint8_t Register_AUXR;
// 	uint8_t Timer_Mode;
// 	uint16_t Timer_Count;
// 	uint8_t RunTimer_Enable : 1;
// 	uint8_t Interrupt_Enable : 1;
// 	TIMER_NVIC_TypeDef Timer_NVIC;
// }TIM_HandleTypeDef;

typedef struct
{
	Timer_TypsDef Instance;
	AUXR_Register AUXR_Reg;
	TMOD_Register TMOD_Reg;
	uint16_t   Timer_Count;
	TIMER_NVIC_TypeDef Timer_NVIC;
	IE_Register     IE_Reg;
	TCON_Register TCON_Reg;
}TIM_HandleTypeDef;

typedef void (*event_handle)(void); //large reentrant;
typedef struct
{
	uint16_t timercnt;	    /*硬件定时中断次数 10ms/次）*/
	uint16_t targetcnt;		/*事件定时器中断次数*/
	uint8_t  enable;	    /*对应事件使能标志*/
	uint8_t  execute_flag;	/*对应事件执行标志*/
	event_handle ehandle;   /*对应事件接口函数*/
}TIMERS;

extern TIMERS Timer_Group[TIMER_EVENTS];
extern void Timer0_Init();
extern void Timer1_Init();

#endif