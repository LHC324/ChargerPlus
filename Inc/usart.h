#ifndef __USART_H
#define __USART_H
	 
#include "config.h"	 
	 

//以下宏定义用于改变串口的输出引脚
//串口1默认是p30 p31  以下定义的是串口1的切换
// P_SW1 = 0xa2   P_SW2 = 0xba


#define USART1_USE_P30_P31 	   P_SW1 = 0x00
#define USART1_USE_P36_P37 	   P_SW1 = 0x40
#define USART1_USE_P16_P17 	   P_SW1 = 0x80
#define USART1_USE_P43_P44 	   P_SW1 = 0xC0

//串口2的默认输出引脚为	P10 P11

#define USART2_USE_P10_P11 	   P_SW2 = 0x00
#define USART2_USE_P40_P42 	   P_SW2 = 0x01

//串口3的默认输出引脚是 P00 P01

#define USART3_USE_P00_P01	   P_SW2 = 0x00
#define USART3_USE_P50_P51 	   P_SW2 = 0x02

//串口4的默认输出引脚为 P02 P03
#define USART3_USE_P02_P03 	   P_SW1 = 0x00
#define USART3_USE_P52_P53 	   P_SW1 = 0x04


//*******************外加485芯片的定义*******************//
#define USART3_EN			P36
//******************************************************//



#define USEING_PRINTF 0

/***********************************常用通讯波特率***********************************/
#define BAUD_921600  921600UL
#define BAUD_115200  115200UL
#define BAUD_57600	 57600UL
#define BAUD_56000	 56000UL
#define BAUD_38400	 38400UL
#define BAUD_19200   19200UL
#define BAUD_14400   14400UL
#define BAUD_9600    9600UL
#define BAUD_4800    4800UL
#define BAUD_2400    2400UL
#define BAUD_1200    1200UL
/***********************************常用通讯波特率***********************************/

#define UART_DIV 4
#define UART_BYTE_SENDOVERTIME (uint16_t)5000U

#define BRT_1T(BAUD)  (COUNTMAX-FOSC/BAUD/UART_DIV) //1T
#define BRT_12T(BAUD) (COUNTMAX-FOSC/BAUD/UART_DIV/12) //12T


typedef enum
{
	UART1 = 0x01,
	UART2,
	UART3,
	UART4
}Uart_TypsDef;


typedef struct //串口中断优先级
{
	uint8_t Register_IP;
	uint8_t Register_IPH;
}UART_NVIC_TypeDef;

typedef struct
{
	Uart_TypsDef Instance;
	uint8_t Register_SCON;
	uint8_t Uart_Mode;
	uint16_t Uart_Count;
	uint8_t RunUart_Enable:1;
	uint8_t Interrupt_Enable;
	uint8_t Register_AUXR;
	uint8_t Uartx_busy:1; //串口接收占用标志
	UART_NVIC_TypeDef Uart_NVIC;
}Uart_HandleTypeDef;


void Uart_Base_MspInit(Uart_HandleTypeDef *uart_baseHandle);
extern void Uart1_Init(void);
extern void Uart2_Init(void);
extern void Uart3_Init(void);
extern void Uart4_Init(void);


void Uartx_SendStr(Uart_HandleTypeDef *const Uart, uint8_t *p, uint8_t length, uint16_t time_out);
void Busy_Await(Uart_HandleTypeDef *const Uart, uint16_t overtime);
void Uartx_Printf(Uart_HandleTypeDef *const uart, const char *format, ...);

extern Uart_HandleTypeDef Uart1; //串口1句柄
extern Uart_HandleTypeDef Uart2; //串口2句柄
extern Uart_HandleTypeDef Uart3; //串口3句柄
extern Uart_HandleTypeDef Uart4; //串口4句柄

#endif