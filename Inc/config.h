#ifndef __CONFIG_H_
#define __CONFIG_H_

#include <STC8.H>
#include <stdio.h>
#include <intrins.h>
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <absacc.h> //可直接操作内存地址
#include <stdarg.h>


/**********************布尔变量定义**********************/
#define true  1
#define false 0
/**********************布尔变量定义**********************/

/***********************************API配置接口***********************************/
/*使用外部晶振*/
#define EXTERNAL_CRYSTAL 0 
/*调试是否启用串口*/
#define USE_PRINTF_DEBUG 0 
/*调试选项*/
#define DEBUGGING    0
/*迪文屏幕使用CRC校验*/
#define USING_CRC 1
/*定义WIFI模块相关引脚*/
#define WIFI_RESET		P23
#define WIFI_RELOAD		P20

#define COUNTMAX     65536U

/*消除编译器未使用变量警告*/
#define UNUSED_VARIABLE(x)	((void)(x))
#define UNUSED_PARAMETER(x) UNUSED_VARIABLE(x)

//(1/FOSC)*count =times(us)->count = time*FOSC/1000(ms)
#define FOSC         27000000UL//11059200UL
/*1ms(时钟频率越高，所能产生的时间越小)*/  
#define TIMES        10U  
/*定时器模式选择*/
#define TIMER_MODE   12U
/*定时器分频系数，默认为一分频*/  
#define TIME_DIV     1U  
#define T12_MODE (TIMES * FOSC / 1000 / 12 / TIME_DIV) 
#define T1_MODE  (TIMES * FOSC/ 1000 / TIME_DIV)
#define TIMERS_OVERFLOW (COUNTMAX * 1000 * TIMER_MODE * TIME_DIV) / FOSC

#define OPEN_GLOBAL_OUTAGE()  (EA = 1 << 0)
#define CLOSE_GLOBAL_OUTAGE() (EA = 0 << 0)
/*判断延时数是否超出硬件允许范围*/
#if(TIMES > TIMERS_OVERFLOW)
#error The timer cannot generate the current duration!
#endif
/***********************************API配置接口***********************************/

/***********************************常用的数据类型***********************************/
typedef unsigned char  uint8_t;
typedef signed char    int8_t;
typedef unsigned short int  uint16_t;
typedef unsigned long  uint32_t;
typedef signed long    int32_t;
typedef volatile       __IO;   

/***********************************常用的数据类型***********************************/

/***********************************系统上电参数***********************************/
#define DEFAULT_SYSTEM_PARAMETER  "\xFF\xFF\xFF\xFF\x02\x01\x00\x84\x50"

/***********************************系统上电参数***********************************/

/***********************************结构体的妙用 ***********************************/
/*获得结构体(TYPE)的变量成员(MEMBER)在此结构体中的偏移量*/ 
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
/*根据"结构体(type)变量"中的"域成员变量(member)的指针(ptr)"来获取指向整个结构体变量的指针*/ 
#define container_of(ptr, type, member) ({ \
        const typeof( ((type *)0)->member ) *__mptr = (ptr); \
        (type *)( (char *)__mptr - offsetof(type,member) );}) 
/***********************************结构体的妙用 ***********************************/


extern const uint8_t g_TimerNumbers;
/***********************************函数声明***********************************/
void  Gpio_Init(void);
/***********************************函数声明***********************************/
#endif