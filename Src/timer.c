#include "timer.h"

static void TIM_Base_MspInit(TIM_HandleTypeDef *const tim_baseHandle);
TIMERS Timer_Group[TIMER_EVENTS];
TIM_HandleTypeDef Timer0;

/*********************************************************
* 函数名： Timer0_Init()
* 功能：   定时器0 的初始化设置
* 参数：   无
* 作者：   LHC
* note：
*		   定时器2、3、4均工作在16bit自动重载模式
**********************************************************/
/*禁止编译器优化该模块*/
#pragma OPTIMIZE(0)

void Timer0_Init(void)
{
	Timer0.Instance = TIMER0;
	/*12T模式*/
	Timer0.AUXR_Reg.AUXR.AUXR_U8 = 0x7F;//T0X12
	Timer0.TMOD_Reg.TMOD.TMOD_U8 = 0xF0;//T0_AutoReload_16bit 
	Timer0.Timer_Count   =(COUNTMAX - (T12_MODE));
	/*定时器0优先级为11，最高优先级*/ 	
	Timer0.Timer_NVIC.IP_Reg.IP.IP_U8	 = 0x20;
	Timer0.Timer_NVIC.IPH_Reg.IPH.IPH_U8 = 0x20;
	/*开启定时器0中断*/ 
	Timer0.IE_Reg.IE.IE_U8 = true;
	/*开启定时器0*/
	Timer0.TCON_Reg.TCON.TCON_U8 = true;
	TIM_Base_MspInit(&Timer0);		
}

void TIM_Base_MspInit(TIM_HandleTypeDef *const tim_baseHandle)
{	/*首次初始化，把TMOD和AUXR全部置零*/
	switch(tim_baseHandle->Instance)
	{
		case TIMER0:
		{
			AUXR &= tim_baseHandle->AUXR_Reg.AUXR.AUXR_U8;
			/*模式0*/
			TMOD &= tim_baseHandle->TMOD_Reg.TMOD.TMOD_U8;
			/*65536-11.0592M/12/1000 */                                
			TL0 =  (uint8_t)(tim_baseHandle->Timer_Count);                
			TH0 =  (uint8_t)((tim_baseHandle->Timer_Count) >> 8);
			IP  |= tim_baseHandle->Timer_NVIC.IP_Reg.IP.IP_U8;
			IPH |= tim_baseHandle->Timer_NVIC.IPH_Reg.IPH.IPH_U8;		
			/*使能定时器中断*/                        
			ET0 =  tim_baseHandle->IE_Reg.IE.IE_U8;                      
			/*启动定时器0*/ 			 
			TR0 =  tim_baseHandle->TCON_Reg.TCON.TCON_U8;
			/*T0溢出中断清零*/
			TF0 = 0;
		}break;
		default : break;
	}
}

/*定时器0的中断服务函数*/
void Timer0_ISR() interrupt 1	
{	
	data uint8_t e = 0;

	for(e = 0; e < g_TimerNumbers; e++)
	{
		if(Timer_Group[e].enable == true)    //使能
		{
			Timer_Group[e].timercnt++;
			if(Timer_Group[e].timercnt == Timer_Group[e].targetcnt)
			{
				Timer_Group[e].execute_flag = true;
				Timer_Group[e].timercnt = 0U;
			}
		}
	}	
}



