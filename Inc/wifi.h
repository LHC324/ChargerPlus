/********************************************************************
 **--------------文件信息---------------------------------------------
 **文   件   名：wifi.h
 **创 建  日 期：2021年4月9日
 **最后修改日期：
 **版 权 信  息: 云南兆富科技有限公司
 **程   序   员：LHC
 **版   本   号：V3.0
 **描        述：WIFI驱动程序（USR-C215）
 **修 改 日  志:
 *********************************************************************/
#ifndef _WIFI_H_
#define	_WIFI_H_

#ifdef	__cplusplus
extern "C" {
#endif

#include "config.h"

/*定义WIFI模块相关引脚*/
#define WIFI_RESET		P23
#define WIFI_RELOAD		P20

/*定义设备ID*/
#define DRIVERS_ID "32"
/*定义热点ID*/
#define AP_ID "ChargerPlus"
/*转化为字符串*/
#define SWITCH_STR(S) #S
/*拼接一个宏定义和字符串*/
#define STR_MCRO() ("AT+WAP=" AP_ID ",NONE\r\n")
/*连接两个字符串S1、S2*/
#define STR_CONNECT(S1,S2) (S1##""##S2)
/*当前设备热点名称*/
#define AP_NAME	 STR_MCRO()//"AT+WAP=PLC7_AP,NONE\r\n"
/*当前设备云平台设备号*/
#define CLOUD_ID "AT+REGCLOUD=000196390000000000" DRIVERS_ID ",SkdGAzyl\r\n"
/*当前WIFI模块工作方式*/
#define AP_STA_MODE "AT+WMODE=APSTA\r\n"
#define STA_MODE "AT+WMODE=APSTA\r\n"
/*进入透传模式命令*/
#define ENTM_CMD "AT+ENTM\r\n"
/*模块重启命令*/
#define RESTART_CMD "AT+Z\r\n"

typedef unsigned char (*event)(void);
typedef struct
{
	char* pSend;
	char* pRecv;
	uint16_t WaitTimes;
	event pFunc;
}AT_Command;

void AT_SetWap(char* ssid,char* key);
void AT_SetWsta(char* ssid,char* key);


/**
 * @brief	初始化WIFI模块
 * @details	
 * @param	None
 * @retval	None
 */
extern void Wifi_Init(void);

/**
 * @brief	WIFI模块启停
 * @details	
 * @param	status:状态
 * @retval	None
 */
extern void Wifi_Enable(uint8_t status);

/**
 * @brief	WIFI模块执行指令表中特定指令
 * @details	
 * @param	None
 * @retval	None
 */
static void Exe_Appoint_Cmd(uint8_t *str, uint16_t times);

/**
 * @brief	打开WIFI模块热点
 * @details	
 * @param	None
 * @retval	None
 */
extern void Wifi_Open_Ap(void);

/**
 * @brief	关闭WIFI模块热点
 * @details	
 * @param	None
 * @retval	None
 */
extern void Wifi_Close_Ap(void);

#ifdef	__cplusplus
}
#endif

#endif	/* _WIFI_H_ */

