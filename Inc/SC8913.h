#ifndef __SC8913_H
#define __SC8913_H

#ifdef __cplusplus
extern "C" {
#endif
#include "IIC.h"

/*是否启用外部中断信号检测引脚*/
#define USING_INT_PIN 0
/*是否启用快速充电引脚*/
#define USING_DPORDM_PIN 0
/*硬件是否启用充电使能引脚*/
#define USING_CHARGEING_PIN 0

/*定义SC8913拓展IO*/
#if (USING_INT_PIN)
sbit INT_CHANNEL0 = P3^6;
sbit INT_CHANNEL1 = P3^2;
sbit INT_CHANNEL2 = P3^7;
#endif

#if (USING_DPORDM_PIN)
sbit DP_CHANNEL0  = P2^1;
sbit DP_CHANNEL1  = P4^3;
sbit DP_CHANNEL2  = P0^4;

sbit DM_CHANNEL0  = P2^2;
sbit DM_CHANNEL1  = P0^2;
sbit DM_CHANNEL2  = P0^3;
#endif

sbit STOP_CHANNEL0  = P0^1;
sbit STOP_CHANNEL1  = P1^3;
sbit STOP_CHANNEL2  = P1^5;

#if (USING_CHARGEING_PIN)
sbit CHARGER_CHANNEL0  = P2^4;
sbit CHARGER_CHANNEL1  = P2^5;
sbit CHARGER_CHANNEL2  = P2^6;
#endif

/*通道数*/
#define CHANNEL_MAX         0x03
/*SC8913寄存器个数*/
#define REGISTER_MAX        0x1C
/*定时读取只读寄存器个数*/
#define READONLY_REGS       0x0B
/*定义支持写寄存器个数*/
#define WRITE_OPTION_REGS   0x0D

/*从机地址*/
#define SAVLE_ADDRESS	    0x74
/*从机命令写地址*/
#define SAVLE_WRITE_ADDRESS 0xE8
/*从机命令读地址*/
#define SAVLE_READ_ADDRESS 	0xE9

/*SC8913寄存器地址定义*/
#define VBAT_SET_ADDR		    0x00    /**/
#define VBUSREF_I_SET_ADDR		0x01
#define VBUSREF_I_SET2_ADDR		0x02
#define VBUSREF_E_SET_ADDR		0x03
#define VBUSREF_E_SET2_ADDR		0x04
#define IBUS_LIM_SET_ADDR		0x05
#define IBAT_LIM_SET_ADDR		0x06
#define VINREG_SET_ADDR		    0x07
#define RATIO_ADDR		        0x08
#define CTRL0_SET_ADDR		    0x09
#define CTRL1_SET_ADDR		    0x0A
#define CTRL2_SET_ADDR		    0x0B
#define CTRL3_SET_ADDR		    0x0C
#define VBUS_FB_VALUE_ADDR		0x0D
#define VBUS_FB_VALUE2_ADDR		0x0E
#define VBAT_FB_VALUE_ADDR		0x0F
#define VBAT_FB_VALUE2_ADDR		0x10
#define IBUS_VALUE_ADDR		    0x11
#define IBUS_VALUE2_ADDR		0x12
#define IBAT_VALUE_ADDR	        0x13
#define IBAT_VALUE2_ADDR	    0x14
#define ADIN_VALUE_ADDR	        0x15
#define ADIN_VALUE_2_ADDR	    0x16
#define STATUS_ADDR	    		0x17
#define RESERVED_ADDR	    	0x18
#define MASK_ADDR	    	    0x19
#define DP_DM_CTRL_ADDR	    	0x1A
#define DP_DM_READ_ADDR	    	0x1B

#define REG_OK      2U
#define REG_FAILE   3U


#define NAME(scr)       #scr
#define REG_R(r)        r##_Reg /*直接拼接字符串，不需要替换*/
#define REG_TYPE(t)     t##_Type
#define REG_Addr(a)     a##_Addr
#define REG_VALUE(v)    v##_U8
#define REG_BIT(b)      b##_Bit
#define BIT_HIGH(h)     b##h##_HIGHT8
#define BIT_LOW(l)      b##l##_LOW2

/*IBUS端输入阈值电压：低于此值后，芯片将对BAT端限流*/
#define IBUS_THRESHOLD_VOLTAGE 20.0F
/*充电器输入端最大电流*/
#define IBUS_MAX_CURRENT    0.8333F
/*充电器充电通道最大输出电流*/
#define IBAT_MAX_CURRENT    2.00F
/*电路中电流、电压检测电阻：单位mΩ*/
#define CHECK_RESISTANCE     10.0F
/*BUS端输入电压倍率*/
#define VBUS_RAITO           12.5F
/*BAT端输入电压倍率:<9v时，采用5x,否则12.5x*/
#define VBAT_RATIO           5.0F
/*BUS端输入电流倍率*/
#define IBUS_RAITO           3.0F
/*BAT端输入电流倍率*/
#define IBAT_RATIO           12.0F
/*电源输入电压比率*/
#define VINREG_RATIO         100.0F
/*设置BUS端电流上限*/
#define SET_IBUS_LIMT(x)     ((256U * (x * CHECK_RESISTANCE) - IBUS_RAITO * 10.0F) / (IBUS_RAITO * 10.0F))
/*设置BAT端电流上限*/
#define SET_IBAT_LIMT(x)     ((256U * (x * CHECK_RESISTANCE) - IBAT_RATIO * 10.0F) / (IBAT_RATIO * 10.0F))
/*设置电源输入电源电压*/
#define SET_VINREG_SET(x)    ((x * 1000.0F - VINREG_RATIO) / VINREG_RATIO)
/*设置VBUS内部参考编程电压*/
#define SET_VBUSREF_I_SET(x) (((x * 1000.0F)/ (2.0F * VBUS_RAITO) - 4.0F) / 4.0F)
/*1S-2S电池设置VBAT_MON_RATIO位为1;3S-4S电池设置VBAT_MON_RATIO位为0*/
#define SET_RATIO(x)         (x ? (0x02 | 0x38) : (0xFD & 0x38))
/*当AC_ok为0时，设置AC_DEF=0;当AC_ok为1时，设置AC_DEF=1*/
#define SET_CTRL0_SET(x)     (x ? (0x20 | 0x04) : (0xDF & 0x04)) 
/*x=1时，选择BAT端电流作为IBAT上限;x=0时，选择BUS端电流作为IBAT上限*/
#define SET_CTRL1_SET(x)     (x ? (0x80 | 0x01) : (0x7F & 0x01))
/*x=1时，退出工厂模式;x=0时，进入工厂模式*/
#define SET_CTRL2_SET(x)     (x ? (0x08 | 0x01) : (0xF7 & 0x01))
/*x=1时，打开ADC;x=0时，关闭ADC*/
#define SET_CTRL3_SET(x)     (x ? (0x20 | 0x02) : (0xDF & 0x02))
/*得到只读寄存器在g_Sc8913_Registers数组中下标*/
#define GET_SITE(x)           (x - VBUS_FB_VALUE_ADDR)

#define READ_VBUS_VALUE(ch, addr)    ((4U * g_Sc8913_Registers[ch][addr] \
                                  +  (((g_Sc8913_Registers[ch][addr + 1]) & 0xc0) >> 6U) + 1.0F) * 2.0F * VBUS_RAITO / 1000.0F)
#define READ_VBAT_VALUE(ch, addr)    ((4U * g_Sc8913_Registers[ch][addr] \
                                  +  (((g_Sc8913_Registers[ch][addr + 1]) & 0xc0) >> 6U) + 1.0F) * 2.0F * VBAT_RATIO / 1000.0F)
#define READ_IBUS_VALUE(ch, addr)    ((4U * g_Sc8913_Registers[ch][addr] \
                                  +  (((g_Sc8913_Registers[ch][addr + 1]) & 0xc0) >> 6U) + 1.0F) / 600.0F * IBUS_RAITO)
#define READ_IBAT_VALUE(ch, addr)    ((4U * g_Sc8913_Registers[ch][addr] \
                                  +  (((g_Sc8913_Registers[ch][addr + 1]) & 0xc0) >> 6U) + 1.0F) / 600.0F * IBAT_RATIO)

/*SC8913寄存器定义*/
typedef enum
{
    Write = 0x00,
    Read,
    Both
}Reg_Type;

/****************************************寄存器VBAT_SET************************************/
typedef enum
{
    R_0m = 0x00, /*0mΩ*/
    R_20m,
    R_40m,
    R_80m
}Vbat_IRCompensation;

typedef enum
{
    Inside_Mode = 0x00, /*内部模式*/
    External_Mode
}Vbat_Mode;

typedef enum
{
    One_Cell = 0x00, /*单节*/
    Two_Cell,
    Three_Cell,
    Four_Cell
}Vbat_Units;

typedef enum
{
    V4_10 = 0x00, /*4.1V*/
    V4_20,
    V4_25,
    V4_30,
    V4_35,
    V4_40,
    V4_45,
    V4_50
}Vbat_Units_Voltage;

/*充电设置寄存器*/
typedef struct 
{
	Reg_Type VBAT_SET_Type;
    uint8_t  VBAT_SET_Addr;
    union 
	{
		struct 
		{
			uint8_t bIRCOMP  : 2; /*电池充电内部电阻补偿*/
			uint8_t bVBAT_SEL : 1; /*充电电压选择设置*/
			uint8_t bCSEL    : 2; /*内部电压设置模式下，充电电池节数选择*/
			uint8_t bVCELL_SET : 3; /*内部电压设置模式下，每节电池充电截止电压设置*/
		}VBAT_SET_Bit;
		uint8_t VBAT_SET_U8;	
	}VBAT_SET;	
}VBAT_SET_Register;/*default value: 0x01*/ 
/****************************************寄存器VBAT_SET************************************/

/****************************************寄存器VBUSREF_I_SET************************************/
/*电源总线内部模式参考电压设置寄存器*/
typedef struct 
{
	Reg_Type VBUSREF_I_SET_Type;
    uint8_t  VBUSREF_I_SET_Addr;
    union 
	{
		uint8_t VBUSREF_I_SET_U8; /*为内部VBUS编程设置的参考电压*/	
	}VBUSREF_I_SET;	
}VBUSREF_I_SET_Register;/*default value: 0x31*/ 

/*电源总线内部模式参考电压设置寄存器*/
typedef struct 
{
	Reg_Type VBUSREF_I_SET2_Type;
    uint8_t  VBUSREF_I_SET2_Addr;
    union 
	{
		struct 
		{
			uint8_t bVBUSREF_I_SET_2  : 2; /*为内部VBUS编程设置的参考电压*/
			uint8_t bRESERVED : 6; 
		}VBUSREF_I_SET2_Bit;
		uint8_t VBUSREF_I_SET2_U8;	
	}VBUSREF_I_SET2;	
}VBUSREF_I_SET2_Register;/*default value: 0x11xx xxxx*/ 

/****************************************寄存器VBUSREF_I_SET************************************/

/****************************************寄存器IBUS_LIM_SET************************************/
/*输入源电流上限设置寄存器*/
typedef struct 
{
	Reg_Type IBUS_LIM_SET_Type;
    uint8_t  IBUS_LIM_SET_Addr;
    union 
	{
		uint8_t IBUS_LIM_SET_U8; /*为内部VBUS编程设置的参考电压*/	
	}IBUS_LIM_SET;	
}IBUS_LIM_SET_Register;/*default value: 0xFF*/ 

/*输入源电压上限设置寄存器*/
typedef struct 
{
	Reg_Type IBAT_LIM_SET_Type;
    uint8_t  IBAT_LIM_SET_Addr;
    union 
	{
		uint8_t IBAT_LIM_SET_U8; /*为内部VBUS编程设置的参考电压*/	
	}IBAT_LIM_SET;	
}IBAT_LIM_SET_Register;/*default value: 0xFF*/ 

/*充电模式下，输入源电压范围设置寄存器*/
typedef struct 
{
	Reg_Type VINRANG_SET_Type;
    uint8_t  VINRANG_SET_Addr;
    union 
	{
        uint8_t VINRANG_SET_U8; /*设置输入源电压的范围*/	
	}VINRANG_SET;	
}VINRANG_SET_Register;/*default value: 0x2C*/ 
/****************************************寄存器寄存器IBUS_LIM_SET************************************/

/*BUS、BAT总线上电压、电流倍率设置寄存器*/
typedef struct 
{
	Reg_Type RATIO_Type;
    uint8_t  RATIO_Addr;
    union 
	{
        struct 
        {
            uint8_t bNULL : 3;
            uint8_t bIBAT_RATIO :1; /*设置BAT总线电流倍率*/
            uint8_t bIBUS_RATIO : 2; /*设置BUS总线电流倍率*/
            uint8_t bVBAT_MON_RATIO : 1;  /*设置BAT总线电压倍率*/
            uint8_t bVBUS_RATIO : 1;  /*设置BAT总线电压倍率*/
        }RATIO_Bit;
        uint8_t RATIO_U8; /*设置BUS、BAT总线的电压、电流倍率*/	
	}RATIO;
}RATIO_Register;/*default value: 0x38*/ 

/*CTRLX_SET设置寄存器*/
typedef struct 
{
	Reg_Type CTRL0_SET_Type;
    uint8_t  CTRL0_SET_Addr;
    union 
	{
        struct 
        {
            uint8_t bEN_OTG : 1; /*充放电模式选择*/
            uint8_t bNULL : 1; 
            uint8_t bAC_DET : 1; /*交流插拔检测控制位*/
            uint8_t bVINREG_RATIO : 1;  /*设置输入电源电压倍率*/
            uint8_t bFREQ_SET : 1;  /*设置开关频率*/
            uint8_t bDT_SET : 2; /*设置开关死区时间*/
        }CTRL0_SET_Bit;
        uint8_t CTRL0_SET_U8; 	
	}CTRL0_SET;	
}CTRL0_SET_Register;/*default value: 0x04*/ 

typedef struct 
{
	Reg_Type CTRL1_SET_Type;
    uint8_t  CTRL1_SET_Addr;
    union 
	{
        struct 
        {
            uint8_t bICHAE_SEL : 1; /*充电模式下，最终能够输出的电流极限参考选择（IBUS/IBAT）*/
            uint8_t bDIS_TRICKLE : 1; /*是否启用涓流模式*/
            uint8_t bDIS_TERM : 1; /*充电终止控制*/
            uint8_t bFB_SEL : 1;  /*VBUS控制电压设置，仅适用于放电模式*/
            uint8_t bTRICKLE_SET : 1;  /*涓流模式的开启设置为VBAT的%x，默认为VBAT*%60*/
            uint8_t bDIS_OVP : 1; /*放电模式的OVP保护设置*/
            uint8_t bNULL : 2;
        }CTRL1_SET_Bit;
        uint8_t CTRL1_SET_U8; 	
	}CTRL1_SET;	
}CTRL1_SET_Register;/*default value: 0x01*/

typedef struct 
{
	Reg_Type CTRL2_SET_Type;
    uint8_t  CTRL2_SET_Addr;
    union 
	{
        struct 
        {
            uint8_t bNULL : 4; 
            uint8_t bFACTORY : 1; /*工厂设置位。通电后，MCU应将这一位写成1*/
            uint8_t bEN_DITHEM : 1; /*是否启用开关频率抖动功能*/
            uint8_t bSLEW_SET  : 2;  /*放电模式下的参考电压旋转率/变化速率设置*/
        }CTRL2_SET_Bit;
        uint8_t CTRL2_SET_U8; 	
	}CTRL2_SET;	
}CTRL2_SET_Register;/*default value: 0x01*/

typedef struct 
{
	Reg_Type CTRL3_SET_Type;
    uint8_t  CTRL3_SET_Addr;
    union 
	{
        struct 
        {
            uint8_t bEN_PGATE : 1; /*PMOS控制*/ 
            uint8_t bGPO_CTRL : 1; /*GPO输出控制*/
            uint8_t bAD_START : 1; /*是否启用ADC*/
            uint8_t bILIM_BW_SEL  : 1;  /*ILIM循环带宽设置*/
            uint8_t bLOOP_SET : 1; /*循环响应控制*/
            uint8_t bDIS_SHORTFOLDBACK : 1; /*IBUS电流折叠控制为VBUS短路情况，仅在放电模式下有效*/
            uint8_t bEOC_SET : 1; /*充电结束阶段目标电流与充电电流倍率*/
            uint8_t bEN_PFM  : 1; /*光照条件下的PFM控制，仅针对放电模式*/
        }CTRL3_SET_Bit;
        uint8_t CTRL3_SET_U8; 	
	}CTRL3_SET;	
}CTRL3_SET_Register;/*default value: 0x02*/

/****************************************只读寄存器************************************/
typedef struct 
{
	Reg_Type VBUS_FB_VALUE_Type;
    uint8_t  VBUS_FB_VALUE_Addr;
    union 
	{
        struct 
        {
            
            uint8_t bVBUS_FB_VALUE_HULL : 6;
            uint8_t bVBUS_FB_VALUE_HIGHT8 : 8;  
            uint8_t bVBUS_FB_VALUE_LOW2   : 2;
        }VBUS_FB_VALUE_Bit;
        uint16_t VBUS_FB_VALUE_U8; 	
	}VBUS_FB_VALUE;	
}VBUS_FB_VALUE_Register;

typedef struct 
{
	Reg_Type VBAT_FB_VALUE_Type;
    uint8_t  VBAT_FB_VALUE_Addr;
    union 
	{
        struct 
        {
            
            uint8_t bVBAT_FB_VALUE_HULL : 6;
            uint8_t bVBAT_FB_VALUE_HIGHT8 : 8;  
            uint8_t bVBAT_FB_VALUE_LOW2   : 2;
        }VBAT_FB_VALUE_Bit;
        uint16_t VBAT_FB_VALUE_U8; 	
	}VBAT_FB_VALUE;	
}VBAT_FB_VALUE_Register;

typedef struct 
{
	Reg_Type IBUS_FB_VALUE_Type;
    uint8_t  IBUS_FB_VALUE_Addr;
    union 
	{
        struct 
        {
            
            uint8_t bIBUS_FB_VALUE_HULL : 6;
            uint8_t bIBUS_FB_VALUE_HIGHT8 : 8;  
            uint8_t bIBUS_FB_VALUE_LOW2   : 2;
        }IBUS_FB_VALUE_Bit;
        uint16_t IBUS_FB_VALUE_U8; 	
	}IBUS_FB_VALUE;	
}IBUS_FB_VALUE_Register;

typedef struct 
{
	Reg_Type IBAT_FB_VALUE_Type;
    uint8_t  IBAT_FB_VALUE_Addr;
    union 
	{
        struct 
        {
            
            uint8_t bIBAT_FB_VALUE_HULL : 6;
            uint8_t bIBAT_FB_VALUE_HIGHT8 : 8;  
            uint8_t bIBAT_FB_VALUE_LOW2   : 2;
        }IBAT_FB_VALUE_Bit;
        uint16_t IBAT_FB_VALUE_U8; 	
	}IBAT_FB_VALUE;	
}IBAT_FB_VALUE_Register;


typedef struct 
{
	Reg_Type MASK_Type;
    uint8_t  MASK_Addr;
    union 
	{
        struct 
        {
            
            uint8_t bDM_L_MASK : 1; /*检测到DM中断设置位*/
            uint8_t bAC_OK_MASK : 1; /*插入交流适配器中断设置位*/  
            uint8_t bINDET2_MASK : 1; /*USB-A在INDET2针脚处检测到一个负载插入中断设置位*/
            uint8_t bINDET1_MASK : 1; /*USB-A在INDET1针脚处检测到一个负载插入中断设置位*/
            uint8_t bVBUS_SHORT_MASK : 1; /*在放电模式下发生VBUS短路故障中断设置位*/
            uint8_t bVBUS_OTP_MASK : 1; /*OTP故障发生中断设置位*/
            uint8_t bVBUS_EOC_MASK : 1; /*满足EOC条件中断设置位*/
            uint8_t bNULL : 1;
        }MASK_Bit;
        uint8_t MASK_U8; 	
	}MASK;	
}MASK_Register;

/****************************************只读寄存器************************************/


typedef struct 
{
	Reg_Type STATUS_Type;
    uint8_t  STATUS_Addr;
    union 
	{
        struct 
        {
            
            uint8_t bDM_L : 1; /*检测到DM电压低于0.325V*/
            uint8_t bAC_OK : 1; /*插入交流适配器*/  
            uint8_t bINDET2 : 1; /*USB-A在INDET2针脚处检测到一个负载插入*/
            uint8_t bINDET1 : 1; /*USB-A在INDET1针脚处检测到一个负载插入*/
            uint8_t bVBUS_SHORT : 1; /*在放电模式下发生VBUS短路故障*/
            uint8_t bVBUS_OTP : 1; /*OTP故障发生*/
            uint8_t bVBUS_EOC : 1; /*满足EOC条件*/
            uint8_t bNULL : 1;
        }STATUS_Bit;
        uint8_t STATUS_U8; 	
	}STATUS;	
}STATUS_Register;

extern data uint8_t g_Sc8913_Registers[CHANNEL_MAX][REGISTER_MAX]; 
extern uint8_t Sc8913_Init(void);
extern void Set_Sc8913_Stop(IIC_Channel channel, IIC_Pin_State state);
extern void Set_Sc8913_charger(IIC_Channel channel, IIC_Pin_State state);
extern uint8_t Sc8913_Write_Register(IIC_Channel channel, uint8_t start_addr, uint8_t *dat, uint8_t length);
extern uint8_t Sc8913_Read_Register(IIC_Channel channel, uint8_t start_addr, uint8_t *dat, uint8_t length);
extern uint8_t Read_Register_Value(IIC_Channel channel);
#ifdef __cplusplus
}
#endif

#endif /* __SC8913_H */