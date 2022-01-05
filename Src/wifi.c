/********************************************************************
 **--------------文件信息---------------------------------------------
 **文   件   名：wifi.h
 **创 建  日 期：2022年1月5日
 **最后修改日期：
 **版 权 信  息: 云南兆富科技有限公司
 **程   序   员：LHC
 **版   本   号：V2.0
 **描        述：WIFI驱动程序（RAK425）
 **修 改 日  志:
 *********************************************************************/
#include "wifi.h"
#include "usart.h"

extern void Delay_Ms(uint16_t ms);
/*USR-C210模块AT指令列表*/
const AT_Command code Wifi_Cmd[] =
{
	/*WIFI模块推出透传模式进入AT指令模式*/
	{"+++", "a", 100U,  NULL},
	/*WIFI模块响应后，主动发送”a“*/
	{"a",   "+Ok", 20U, NULL},
	/*关闭回显*/ 
	//{"AT+E=OFF\r\n", "+OK", 500},           
	/*显示SSID*/ 
	//{"AT+HSSID = OFF\r\n", "+OK", 500}, 
	/*WIFI工作模式：AP + STA*/ 
	{AP_STA_MODE, "+OK", 20U, NULL},        
	/*设置路由器名称*/ 
	{AP_NAME, "+OK", 20U, NULL},
	/*设置心跳数据:www.ynpax.com*/ 
	{"AT+HEARTDT=7777772E796E7061782E636F6D\r\n", "+OK", 20U, NULL},   
	/*SSID和密码不能程序输入，需要在现场根据用户方的WIFI设置通过WEB方式修改*/
	/*设置WIFI登录SSID，密码*/ 
	{"AT+WSTA=union,*!ynzfkj20091215!*\r\n", "+OK", 20U, NULL}, 
	/*透传云设置*/       
	{"AT+REGENA=CLOUD,FIRST\r\n", "+OK", 20U, NULL},
	/*设置STOCKA参数*/
	{"AT+SOCKA=TCPC,cloud.usr.cn,15000\r\n", "+OK", 20U, NULL},
	/*设置搜索服务器和端口*/
	{"AT+SEARCH=15000,cloud.usr.cn\r\n", "+OK", 20U, NULL},
	/*透传云ID，透传云密码*/  
	{CLOUD_ID, "+OK", 20U, NULL},
	/*设置DHCP*/ 
	{"AT+WANN=DHCP\r\n", "+OK", 20U, NULL},
	/*软件重启USR-C210*/
	{"AT+Z\r\n", "+OK", 20U, NULL},  
	/*设置透传模式*/     
	// {"AT+ENTM\r\n", "+OK", 50U, NULL}    
};

#define WIFI_CMDSIZE sizeof(Wifi_Cmd) / sizeof(AT_Command)


/**
 * @brief	WIFI模块启停
 * @details	
 * @param	status:状态
 * @retval	None
 */
void Wifi_Enable(uint8_t status)
{
	switch(status)
	{
		case true:
		{
			WIFI_RESET  = 1;
		}break;
		case false:
		{
			WIFI_RESET  = 0;
		}break;
		default : break;
	}
}

/**
 * @brief	初始化WIFI模块
 * @details	WIFI模块复位需要一定延时；在退出透传模式时延时必须>500ms,每条指令间间隔必须>100ms
 * @param	None
 * @retval	None
 */
void Wifi_Init(void)
{
	uint8_t i = 0;

	/*先复位WIFI*/
	WIFI_RESET  = 0;
	WIFI_RELOAD = 1;
	/*等待模块复位*/
	Delay_Ms(100);
	WIFI_RESET  = 1;

	for(i = 0; i < WIFI_CMDSIZE; i++)
	{
		// Uartx_SendStr(&Uart2, (uint8_t *)Wifi_Cmd[i].pSend, strlen(Wifi_Cmd[i].pSend));
		// /*执行对应的等待时间*/
		// Delay_ms(Wifi_Cmd[i].WaitTimes);
		Exe_Appoint_Cmd((uint8_t *)Wifi_Cmd[i].pSend, Wifi_Cmd[i].WaitTimes);
	}
}

/**
 * @brief	WIFI模块执行指令表中特定指令
 * @details	
 * @param	None
 * @retval	None
 */
static void Exe_Appoint_Cmd(uint8_t *str, uint16_t times)
{
	if((str != NULL) && (times))
	{
		Uartx_SendStr(&Uart2, str, strlen(str), UART_BYTE_SENDOVERTIME);
		Delay_Ms(times);
	}
}


/**
 * @brief	WIFI模块退出透传模式
 * @details	
 * @param	None
 * @retval	None
 */
static void Wifi_Exit_Trt(void)
{
	
	uint8_t i = 0;

	for(i = 0; i < 2U; i++)
	{
		if((Wifi_Cmd[i].pSend != '\0') && (Wifi_Cmd[i].WaitTimes))
		{
			// Uartx_SendStr(&Uart2, (uint8_t *)Wifi_Cmd[i].pSend, strlen(Wifi_Cmd[i].pSend));
			// /*执行对应的等待时间*/
			// Delay_ms(Wifi_Cmd[i].WaitTimes);
			Exe_Appoint_Cmd((uint8_t *)Wifi_Cmd[i].pSend, Wifi_Cmd[i].WaitTimes);
		}
	}
}

/**
 * @brief	打开WIFI模块热点
 * @details	
 * @param	None
 * @retval	None
 */
void Wifi_Open_Ap(void)
{
	/*退出透传模式*/
	Wifi_Exit_Trt();
	/*发送配置指令*/
	Exe_Appoint_Cmd(AP_STA_MODE, 200);
	/*进入透传模式*/
	Exe_Appoint_Cmd(ENTM_CMD, 200);
	/*重启模块*/
	Exe_Appoint_Cmd(RESTART_CMD, 200);
}

/**
 * @brief	关闭WIFI模块热点
 * @details	
 * @param	None
 * @retval	None
 */
void Wifi_Close_Ap(void)
{	
	/*退出透传模式*/
	Wifi_Exit_Trt();
	/*发送配置指令*/
	Exe_Appoint_Cmd(STA_MODE, 200);
	/*进入透传模式*/
	Exe_Appoint_Cmd(ENTM_CMD, 200);
	/*重启模块*/
	Exe_Appoint_Cmd(RESTART_CMD, 200);
}