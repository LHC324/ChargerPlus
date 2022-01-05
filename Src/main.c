#include "GPIO.h"
#include "timer.h"
#include "usart.h"
#include "charger.h"
#include "wifi.h"

#if (DEBUGGING == 1)
void HardDefault(uint8_t channel, uint8_t error);
#endif
void Event_Init(void);
static void Event_Polling(void);
static void Debug_Task(void);
static void ReadAdc_Task(void);
static void Charging_Handle_Task(void);
static void Report_Task(void);

/*禁止编译器优化该模块*/
#pragma OPTIMIZE(0)

/*软定时器任务事件组*/
TIMERS timers_handle[] =
    {
#if (DEBUGGING == 1)
        {0, 100, true, false, Debug_Task          }, /*调试接口（3s）*/
#else
        {0, 50,  true, false, ReadAdc_Task        }, /*轮询读取通道中充电信息（500ms）*/
        {0, 10,  true, false, Charging_Handle_Task}, /*充电事件处理任务（100ms）*/
        {0, 100, true, false, Report_Task         }, /*上报充电器数据到迪文屏幕（1.0s）*/
#endif
};
/*获得当前软件定时器个数*/
const uint8_t g_TimerNumbers = sizeof(timers_handle) / sizeof(TIMERS);

void main(void)
{	
	/*设置WiFi芯片复引脚：不复位会导致连接不上云平台*/
	WIFI_RESET = 0;
    Delay_Ms(200);
	WIFI_RESET = 1;
	WIFI_RELOAD = 1;
    /*初始化引脚*/
    Gpio_Init();
    /*定时器0初始化*/
    Timer0_Init();
    /*串口1初始化*/
    Uart1_Init();
    /*串口2初始化*/
    Uart2_Init();
	/*WiFi初始化*/
//	Wifi_Init();
    /*定时事件初始化*/
    Event_Init();
    /*初始化SC8913*/
    Sc8913_Init();
    /*开总中断*/
    OPEN_GLOBAL_OUTAGE();
    while (1)
    {
        ET0 = 0;
        Event_Polling();
        ET0 = 1;
    }
}

/**
 * @brief	把当前定义任务加载到任务组
 * @details	任务注册
 * @param	None
 * @retval	None
 */
void Event_Init(void)
{
    data uint8_t a = 0;
    for (a = 0; a < g_TimerNumbers; a++)
    {
        Timer_Group[a].timercnt = timers_handle[a].timercnt;
        Timer_Group[a].targetcnt = timers_handle[a].targetcnt;
        Timer_Group[a].enable = timers_handle[a].enable;
        Timer_Group[a].execute_flag = timers_handle[a].execute_flag;
        Timer_Group[a].ehandle = timers_handle[a].ehandle;
    }
}

/**
 * @brief	任务组时间片调度
 * @details	按照指定时间片和执行标志调度任务
 * @param	None
 * @retval	None
 */
/*变量出现问题，加static*/
// #pragma disable
void Event_Polling(void)
{
    data uint8_t b = 0;
    /*执行标志*/
    for (b = 0; b < g_TimerNumbers; b++)
    {
        if (Timer_Group[b].execute_flag == true)
        {
            Timer_Group[b].ehandle();
            Timer_Group[b].execute_flag = false;
        }
    }
}

#if (DEBUGGING == 1)
/**
 * @brief	调试任务
 * @details	调试阶段输出调试信息
 * @param	None
 * @retval	None
 */
void Debug_Task(void)
{
    data uint8_t c = 0;

    for (c = 0; c < CHANNEL_MAX; c++)
    {
        Uartx_Printf(&Uart1, "channel %bd :STATU is 0x%x\r\n", c, (uint8_t)g_Sc8913_Registers[c][STATUS_ADDR]);
        Uartx_Printf(&Uart1, "channel %bd :VBUS is %f\r\n", c, READ_VBUS_VALUE(c, VBUS_FB_VALUE_ADDR));
        Uartx_Printf(&Uart1, "channel %bd :VBAT is %f\r\n", c, READ_VBAT_VALUE(c, VBAT_FB_VALUE_ADDR));
        Uartx_Printf(&Uart1, "channel %bd :IBUS is %f\r\n", c, READ_IBUS_VALUE(c, IBUS_VALUE_ADDR));
        Uartx_Printf(&Uart1, "channel %bd :IBAT is %f\r\n", c, READ_IBAT_VALUE(c, IBAT_VALUE_ADDR));
    }
}
#endif

/**
 * @brief	通道数据采集任务
 * @details	轮询模式，寄存器采用连续读模式
 * @param	None
 * @retval	None
 */
void ReadAdc_Task(void)
{
    data uint8_t h = 0;
    /*轮询获取对应通道下采样寄存器信息*/
    for (h = 0; h < CHANNEL_MAX; h++)
    {
        if (Read_Register_Value(h) == REG_FAILE)
        { /*如果检测到硬件错误，进行记录，并继续下一个*/
#if (DEBUGGING == 1)
            HardDefault(h, REG_FAILE);
#endif
            continue;
        }
    }
}

/**
 * @brief	充电事件处理任务
 * @details	轮询模式
 * @param	None
 * @retval	None
 */
void Charging_Handle_Task(void)
{
    uint8_t m = 0;

    for (m = 0; m < CHANNEL_MAX; m++)
    {
        Charging_Poll(&g_Charger[m]);
    }
}

/**
 * @brief	迪文屏幕数据上报任务
 * @details	轮询模式
 * @param	None
 * @retval	None
 */
void Report_Task(void)
{
    uint8_t n = 0;

    for (n = 0; n < CHANNEL_MAX; n++)
    {
        Report_ChargerInfo(&g_Charger[n]);
    }
}

#if (DEBUGGING == 1)
/**
 * @brief	硬件错误检测
 * @details	轮询模式
 * @param	None
 * @retval	None
 */
void HardDefault(uint8_t channel, uint8_t error)
{
    Uartx_Printf(&Uart1,"\r\nChannel %bd product a error:%bd\r\n", channel , error);
}
#endif

/************************************外设初始化************************************/
/**
 * @brief	GPIO初始化
 * @details	初始化对应的外设引脚
 * @param	None
 * @retval	None
 */
void Gpio_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    //    #ifdef EXTERNAL_CRYSTAL //只要有宏名，就成立
#if EXTERNAL_CRYSTAL
    P_SW2 = 0x80;
    XOSCCR = 0xC0;
    /*启动外部晶振11.0592MHz*/
    while (!(XOSCCR & 1))
        ;
    /*时钟不分频*/
    CLKDIV = 0x01;
    /*选择外部晶振*/
    CKSEL = 0x01;
    P_SW2 = 0x00;
#endif
    //  P_SW1 = 0xC0; //串口1切换到P4.3、4.4(P0.2、0.3)
    //	P_SW2 |= 0x01; //串口2切换到P4.0、4.2(P1.0、1.1)（新板子引脚问题）

    /*手册提示，串口1、2、3、4的发送引脚必须设置为强挽输出*/
    /*设置P0.0、 P0.1、P0.5为开漏输出*/
    GPIO_InitStruct.Mode = GPIO_OUT_OD;
    GPIO_InitStruct.Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5;
    GPIO_Inilize(GPIO_P0, &GPIO_InitStruct);
    /*设置P1.0为准双向口*/
    GPIO_InitStruct.Mode = GPIO_PullUp;
    GPIO_InitStruct.Pin = GPIO_Pin_0;
    GPIO_Inilize(GPIO_P1, &GPIO_InitStruct);
    /*设置P1.1为推免输出*/
    GPIO_InitStruct.Mode = GPIO_OUT_PP;
    GPIO_InitStruct.Pin = GPIO_Pin_1;
    GPIO_Inilize(GPIO_P1, &GPIO_InitStruct);
    /*设置P1.2、P1.3、 P1.4、P1.5为开漏输出*/
    GPIO_InitStruct.Mode = GPIO_OUT_OD;
    GPIO_InitStruct.Pin = GPIO_Pin_2 | GPIO_Pin_3 |GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_Inilize(GPIO_P1, &GPIO_InitStruct);
    /*设置P2.7为开漏输出*/
    GPIO_InitStruct.Mode = GPIO_OUT_OD;
    GPIO_InitStruct.Pin = GPIO_Pin_7;
    GPIO_Inilize(GPIO_P2, &GPIO_InitStruct);
    /*设置P3.0为准双向口*/
    GPIO_InitStruct.Mode = GPIO_PullUp;
    GPIO_InitStruct.Pin = GPIO_Pin_0;
    GPIO_Inilize(GPIO_P3, &GPIO_InitStruct);
    /*设置P3.1为推免输出*/
    GPIO_InitStruct.Mode = GPIO_OUT_PP;
    GPIO_InitStruct.Pin = GPIO_Pin_1;
    GPIO_Inilize(GPIO_P3, &GPIO_InitStruct);
    /*设置P4.4为开漏输出*/
    GPIO_InitStruct.Mode = GPIO_OUT_OD;
    GPIO_InitStruct.Pin = GPIO_Pin_4;
    GPIO_Inilize(GPIO_P4, &GPIO_InitStruct);
}
/************************************外设初始化************************************/
