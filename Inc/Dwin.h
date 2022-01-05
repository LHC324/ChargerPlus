/*
 * Dwin.h
 *
 *  Created on: Nov 19, 2020
 *      Author: play
 */

#ifndef INC_DWIN_H_
#define INC_DWIN_H_

#include "config.h"

#define RX_BUF_SIZE	 128
#define TX_BUF_SIZE  128

#define WRITE_CMD	 0x82		//写
#define READ_CMD	 0x83		//读

#define PAGE_CHANGE_CMD  0x84	//页面切换
#define TOUCH_CMD	     0xD4   //触摸动作


#define SHOW_YEAR_ADDR							0x1000     //年显示地址
#define SHOW_MONTH_ADDR							0x1001     //月显示地址
#define SHOW_DATE_ADDR							0x1002     //日显示地址
#define SHOW_WEEK_ADDR							0x1003     //周显示地址
#define SHOW_HOUR_ADDR							0x1004     //时显示地址
#define SHOW_MIN_ADDR							0x1005     //分显示地址

#define SETTING_YEAR_ADDR						0x1200     //年显示地址
#define SETTING_MONTH_ADDR						0x1210     //月显示地址
#define SETTING_DATE_ADDR						0x1220     //日显示地址
#define SETTING_HOUR_ADDR						0x1230     //时显示地址
#define SETTING_MIN_ADDR						0x1240     //分显示地址

#define TRICKLE_CHARGE_CURRENT1_ADDR			0x1010     //涓流充电充电电流
#define TRICKLE_CHARGE_TARGET_VOLTAGE1_ADDR		0x1012     //涓流充电目标电压
#define CONSTANT_CURRENT_CURRENT1_ADDR			0x1014     //恒流充电充电电流
#define CONSTANT_CURRENT_TARGET_VOLTAGE1_ADDR	0x1016     //恒流充电目标电压
#define CONSTANT_VOLTAGE_VOLTAGE1_ADDR			0x1018     //恒压充电充电电压
#define CONSTANT_VOLTAGE_TARGET_CURRENT1_ADDR   0x101A     //恒压充电阈值电流

#define CHARGE_VOLTAGE_NOW1_ADDR				0x1020	   //当前充电电压
#define CHARGE_CURRENT_NOW1_ADDR				0x1022     //当前充电电流
#define CHARGE_TIME_NOW1_ADDR				    0x1024     //当前充电时长
#define CHARGE_QUANTITY_NOW1_ADDR				0x1026     //当前充电电量

#define TRICKLE_CHARGE_CURRENT2_ADDR			0x1030     //涓流充电充电电流
#define TRICKLE_CHARGE_TARGET_VOLTAGE2_ADDR		0x1032     //涓流充电目标电压
#define CONSTANT_CURRENT_CURRENT2_ADDR			0x1034     //恒流充电充电电流
#define CONSTANT_CURRENT_TARGET_VOLTAGE2_ADDR	0x1036     //恒流充电目标电压
#define CONSTANT_VOLTAGE_VOLTAGE2_ADDR			0x1038     //恒压充电充电电压
#define CONSTANT_VOLTAGE_TARGET_CURRENT2_ADDR   0x103A     //恒压充电阈值电流

#define CHARGE_VOLTAGE_NOW2_ADDR				0x1040	   //当前充电电压
#define CHARGE_CURRENT_NOW2_ADDR				0x1042     //当前充电电流
#define CHARGE_TIME_NOW2_ADDR				    0x1044	   //当前充电时长
#define CHARGE_QUANTITY_NOW2_ADDR				0x1046     //当前充电电量

#define TRICKLE_CHARGE_CURRENT3_ADDR			0x1050     //涓流充电充电电流
#define TRICKLE_CHARGE_TARGET_VOLTAGE3_ADDR		0x1052     //涓流充电目标电压
#define CONSTANT_CURRENT_CURRENT3_ADDR			0x1054     //恒流充电充电电流
#define CONSTANT_CURRENT_TARGET_VOLTAGE3_ADDR	0x1056     //恒流充电目标电压
#define CONSTANT_VOLTAGE_VOLTAGE3_ADDR			0x1058     //恒压充电充电电压
#define CONSTANT_VOLTAGE_TARGET_CURRENT3_ADDR   0x105A     //恒压充电阈值电流

#define CHARGE_VOLTAGE_NOW3_ADDR				0x1060	   //当前充电电压
#define CHARGE_CURRENT_NOW3_ADDR				0x1062     //当前充电电流
#define CHARGE_TIME_NOW3_ADDR				    0x1064     //当前充电时长
#define CHARGE_QUANTITY_NOW3_ADDR				0x1066     //当前充电电量

#define SHOW_ANIMATION1_ADDR					0x2000	   //电池充电动画1
#define SHOW_ANIMATION2_ADDR					0x2010     //电池充电动画2
#define SHOW_ANIMATION3_ADDR					0x2020	   //电池充电动画3

#define CHARGE_STATE1_ADDR						0x2030     //电池充电状态1
#define CHARGE_STATE2_ADDR						0x2040     //电池充电状态2
#define CHARGE_STATE3_ADDR						0x2050     //电池充电状态3

#define CHARGE_STATE_NOW1_ADDR					0x1028     //当前充电状态
#define CHARGE_STATE_NOW2_ADDR					0x1048     //当前充电状态
#define CHARGE_STATE_NOW3_ADDR					0x1068     //当前充电状态

#define PARAM_START_ADDR                        0x3000     //电源管理芯片内部参数显示地址
#define DWIN_ADDR_OFFSET                        0x0020     //迪文屏幕变量便宜地址

typedef struct
{
	uint8_t RxBuf[RX_BUF_SIZE];
	uint16_t RxCount;

	uint8_t TxBuf[TX_BUF_SIZE];
	uint16_t TxCount;
}Dwin_T;

typedef void (*pfunc)(uint8_t*,uint8_t);
typedef struct
{
	uint32_t addr;
	pfunc event;
}DwinMap;


extern Dwin_T g_Dwin;
extern void Dwin_SendWithCRC(uint8_t *_pBuf, uint16_t _ucLen);
extern void Dwin_Send(uint8_t *_pBuf, uint16_t _ucLen);
extern void Dwin_Write(uint16_t start_addr, uint8_t *dat, uint16_t length);
extern void Dwin_Read(uint16_t start_addr, uint16_t words);
extern uint16_t Get_Crc16(uint8_t *ptr, uint16_t length, uint16_t init_dat);

#endif /* INC_DWIN_H_ */
