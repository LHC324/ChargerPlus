#include "usart.h"


/*********************************************************
* 函数名：
* 功能：
* 参数：
* 作者：
* note：
		同时使用多个串口的时候会出现数据传输错误的情况 建议在使用该板子与其他
		通讯模块建立通讯的时候使用1对1的建立连接的模式

		解决了多串口同时数据传输错误问题 //2021/5/31

		在切换串口的引脚输入时，建议将RX端初始化的时候给个0值 TX引脚手动给个1值
		（基于STC单片机的特性）

**********************************************************/
Uart_HandleTypeDef Uart1; //串口1句柄
Uart_HandleTypeDef Uart2; //串口2句柄
Uart_HandleTypeDef Uart3; //串口3句柄
Uart_HandleTypeDef Uart4; //串口4句柄

/*********************************************************
* 函数名：void Uart_1Init(void)
* 功能：  串口1的初始化
* 参数：
* 作者：  LHC
* note：
*		使用的是定时器1作为波特率发生器,LAN口用
**********************************************************/
void Uart1_Init(void) //串口1选择定时器1作为波特率发生器
{
    Uart1.Instance = UART1;
    Uart1.Register_SCON = 0x50; //模式1，8位数据，可变波特率
    Uart1.Uart_Mode = 0x00; //定时器模式0，16bit自动重载
    Uart1.Uart_Count = BRT_1T(BAUD_115200);
    Uart1.RunUart_Enable = true;
    Uart1.Interrupt_Enable = true;
    Uart1.Register_AUXR = 0x40; //定时器1，1T模式
    Uart1.Register_AUXR &= 0xFE; //波特率发生器选用定时器1，最好按照要求来

    Uart1.Uart_NVIC.Register_IP = 0xEF; //PS=0,PSH=0,串口1中断优先级为第0级，最低级
    Uart1.Uart_NVIC.Register_IPH = 0xEF;

    Uart_Base_MspInit(&Uart1);
}


/*********************************************************
* 函数名：void Uart1_ISR() interrupt 4 using 2
* 功能：  串口1的定时中断服务函数
* 参数：
* 作者：  LHC
* note：
*		使用的是定时器2作为波特率发生器,485口用
**********************************************************/
void Uart1_ISR() interrupt 4 using 2	//串口1的定时中断服务函数
{ 	
    if(TI) //发送中断标志
	{		
		TI = 0;
		Uart1.Uartx_busy =false; //发送完成，清除占用
    }
	
	if(RI) //接收中断标志
	{		
        RI = 0;
    }
}

/*********************************************************
* 函数名：void Uart_2Init(void)
* 功能：  串口2的初始化
* 参数：
* 作者：  LHC
* note：
*		使用的是定时器2作为波特率发生器,485口用
**********************************************************/
void Uart2_Init(void) //串口2选择定时器2作为波特率发生器
{
    Uart2.Instance = UART2;
    Uart2.Register_SCON = 0x10; //模式1，8位数据，可变波特率，开启串口2接收
    Uart2.Uart_Mode = 0x00; //定时器模式0，16bit自动重载
    Uart2.Uart_Count = BRT_1T(BAUD_115200);
    Uart2.RunUart_Enable = true;
    Uart2.Interrupt_Enable = 0x01;
    Uart2.Register_AUXR = 0x14; //开启定时器2，1T模式
    Uart2.Uart_NVIC.Register_IP = 0x01; //PS2=1,PS2H=0,串口2中断优先级为第1级
    Uart2.Uart_NVIC.Register_IPH = 0xFE;

    Uart_Base_MspInit(&Uart2);
}


/*********************************************************
* 函数名：void Uart2_ISR() interrupt 8 using 2
* 功能：  串口2中断函数
* 参数：
* 作者：  LHC
* note：
*		使用的是定时器2作为波特率发生器,4G口用
**********************************************************/
void Uart2_ISR() interrupt 8 using 2
{
    if (S2CON & S2TI) //发送中断
	{  
        S2CON &= ~S2TI;
		Uart2.Uartx_busy = false; //发送完成，清除占用
		
    }
	
    if (S2CON & S2RI) //接收中断
	{  
        S2CON &= ~S2RI;
		
	}
}

/**********************************公用函数************************/

/*********************************************************
* 函数名：Uart_Base_MspInit(Uart_HandleTypeDef *uart_baseHandle)
* 功能：  所有串口底层初始化函数
* 参数：  Uart_HandleTypeDef *uart_baseHandle串口句柄
* 作者：  LHC
* note：
*		注意正确给出串口句柄
**********************************************************/
void Uart_Base_MspInit(Uart_HandleTypeDef *const uart_baseHandle)
{
    if(uart_baseHandle->Instance == UART1)
    {
        SCON = uart_baseHandle->Register_SCON;
        TMOD |= uart_baseHandle->Uart_Mode;
        TL1 =  uart_baseHandle->Uart_Count;
        TH1 =  uart_baseHandle->Uart_Count >> 8;
        TR1 =  uart_baseHandle->RunUart_Enable;
        AUXR |= uart_baseHandle->Register_AUXR;
        IP &= uart_baseHandle->Uart_NVIC.Register_IP;
        IPH &= uart_baseHandle->Uart_NVIC.Register_IPH;
        #if USEING_PRINTF //如果使用printf
        TI = 1; //放到printf重定向
        #else
        ES = uart_baseHandle->Interrupt_Enable;	//串口1中断允许位
        #endif
    }
    else if(uart_baseHandle->Instance == UART2)
    {
        S2CON = uart_baseHandle->Register_SCON;
        TL2 =  uart_baseHandle->Uart_Count;
        TH2 =  uart_baseHandle->Uart_Count >> 8;
        AUXR |= uart_baseHandle->Register_AUXR;
        IE2 |= uart_baseHandle->Interrupt_Enable;	//串口2中断允许位
        IP2 &= uart_baseHandle->Uart_NVIC.Register_IP;
        IP2H &= uart_baseHandle->Uart_NVIC.Register_IPH;
    }
    else if(uart_baseHandle->Instance == UART3)
    {
        S3CON = uart_baseHandle->Register_SCON;
        T4T3M = uart_baseHandle->Uart_Mode;
        T3L =  uart_baseHandle->Uart_Count;
        T3H =  uart_baseHandle->Uart_Count >> 8;
        IE2 |= uart_baseHandle->Interrupt_Enable;	//串口3中断允许位
    }
    else if(uart_baseHandle->Instance == UART4)
    {
        S4CON = uart_baseHandle->Register_SCON;
        T4T3M |= uart_baseHandle->Uart_Mode; //此处串口3和串口4共用
        T4L =  uart_baseHandle->Uart_Count;
        T4H =  uart_baseHandle->Uart_Count >> 8;
        IE2 |= uart_baseHandle->Interrupt_Enable;	//串口4中断允许位
    }
}

/*********************************************************
* 函数名：static void Busy_Await(Uart_HandleTypeDef * const Uart, uint16_t overtime)
* 功能：  字节发送超时等待机制
* 参数：  Uart_HandleTypeDef * const Uart;uint16_t overtime
* 作者：  LHC
* note：
*		
**********************************************************/
 void Busy_Await(Uart_HandleTypeDef * const Uart, uint16_t overtime)
 {

     while(Uart->Uartx_busy)  //等待发送完成：Uart->Uartx_busy清零
     {
         if(!(overtime--))
             break;
     }
	
 	Uart->Uartx_busy = true; //发送数据，把相应串口置忙
 }

/*********************************************************
* 函数名：Uartx_SendStr(Uart_HandleTypeDef *const Uart,uint8_t *p,uint8_t length)
* 功能：  所有串口字符串发送函数
* 参数：  Uart_HandleTypeDef *const Uart,uint8_t *p;uint8_t length
* 作者：  LHC
* note：
*		
**********************************************************/
 void Uartx_SendStr(Uart_HandleTypeDef *const Uart, uint8_t *p, uint8_t length, uint16_t time_out){ 
	
 	while(length--){
 		Busy_Await(&(*Uart), time_out); //等待当前字节发送完成
 		switch(Uart->Instance){
 			case UART1: SBUF  = *p++; break;
 			case UART2:	S2BUF = *p++; break;
 			case UART3: S3BUF = *p++; break; 
 			case UART4: S4BUF = *p++; break;
 			default   : break;
 		}
 	}
 }

/*********************************************************
* 函数名：char putchar(char str)
* 功能：  putchar重定向,被printf调用
* 参数：  char str，发送的字符串
* 作者：  LHC
* note：
*		  使用printf函数将会占用1K 左右FLASH
**********************************************************/
void Uartx_Printf(Uart_HandleTypeDef *const uart, const char *format, ...)
{
	  uint16_t length = 0;
      char UARTx_Buffer[256] = { 0 };
	  va_list ap;

	  va_start(ap, format);
      /*使用可变参数的字符串打印,类似sprintf*/
	  length = vsprintf(UARTx_Buffer, format, ap); 
	  va_end(ap);

      Uartx_SendStr(uart, (uint8_t *)&UARTx_Buffer[0], length, UART_BYTE_SENDOVERTIME);
}

/**********************************公用函数************************/
