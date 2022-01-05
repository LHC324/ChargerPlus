#ifndef __CHARGER_H
#define __CHARGER_H

#ifdef __cplusplus
extern "C" {
#endif
#include "SC8913.h"

/*SC9813最低检测电流>0.3A*/
#define CHECK_CURRENT       0.3F
/*涓流阶段电流时IBAT的1/10*/
#define TRICKLE_CURRNET     (0.1F * IBAT_MAX_CURRENT)
/*通道电压下限2.0V*/
#define LOWEST_VOLTAGE		2.0F      
/*锂电池单节允许最高电压*/
#define ONCE_MAXVOLTAGE     4.20F
/*锂电池单节允许最低电压为SC8913BAT电压的70%*/
#define ONCE_MINVOLTAGE     (ONCE_MAXVOLTAGE * 0.7F)
/*SC8913认为充满时电压*/
#define ONCE_FULLVOLTAGE    (ONCE_MAXVOLTAGE * 0.98F)
/*断路次数*/
#define OPENTIME            50U

/*定义STATUS寄存器位*/
#define EOC_BIT 0x01
#define OTP_BIT 0x02
#define VBUS_SHORT_BIT 0x03
#define INDET1_BIT 0x04
#define INDET2_BIT 0x05
#define AC_OK_BIT 0x06
#define DM_L_BIT 0x07

#define REVISE_REGISTER(ch, value, ratio, result) \
do \
{ \
    if(Sc8913_Write_Register(ch->Channel_Id, VBAT_SET_ADDR, &value ,1U)) \
    { \
        result = true; \
    } \
    if(ratio == true) \
    { \
        p_ch->Vbat_Ratio = VBAT_RATIO; \
    }\
    else \
    { \
        p_ch->Vbat_Ratio = VBAT_RATIO * 2.5F; \
    } \
	value = SET_RATIO(ratio); \
	if(Sc8913_Write_Register(ch->Channel_Id, RATIO_ADDR, &value ,1U)) \
    { \
        result = true; \
    } \
}while(0);


/*设备类型*/
typedef enum
{
	Cell = 0x00,
	Dcdc,
	No_Device,
}Device_Type;

/*充电状态*/
typedef enum
{   
    /*涓流充电*/
	Trickle_Mode = 0x00,
    /*恒流充电*/ 	
	Constant_Current_Mode,
    /*恒压充电*/	
	Constant_Voltage_Mode,
    /*充电结束*/	
	End_Charging,	
    /*待机模式*/
    stdantby_Mode	
}Charging_State;    

/*前台实时上报数据*/
typedef struct
{   /*上报的电压*/
	float Voltage;
    /*上报的电流*/	
	float Current;
    /*充电时长*/	
	uint32_t Time;	
    /*充电电量*/
	float Electricity; 
}Foreground_Parameters;     

/*后台上报数据*/
typedef struct
{
	float Trickle_Target_Current;	 		//涓流目标电流
	float Trickle_Target_Voltage;  			//涓流目标电压

	float ConstantCurrent_Target_Current;  	//恒流目标电流
	float ConstantCurrent_Target_Voltage;  	//恒流目标电压

	float ConstantVoltage_Target_Current;   //恒压目标电流
	float ConstantVoltage_Target_Voltage;	//恒压目标电压
}Background_Parameters;


typedef struct 
{
    /*当前通道ID*/
    IIC_Channel Channel_Id;
    /*设备类型*/	
    Device_Type    Dev_Type;
    /*锂电池节数*/
    uint8_t Batterry_Numbers;
    /*充电状态*/
	Charging_State  CState;
    /*前台参数*/
    Foreground_Parameters For_ChargingPara;
    Background_Parameters Bac_ChargingPara;
    struct 
    {   /*充电器后台参数地址*/
        uint32_t Back_Para_Addr;				   
        /*充电动画地址*/
	    uint32_t Animation_Addr;      
        /*充电状态地址*/
	    uint32_t State_Addr;
    }Charger_Show_Addr;    
    struct 
    {   /*正在充电标志*/
        volatile uint8_t Is_ChargingFlag;
        /*充电动画状态 */  	           
	    volatile uint8_t Cartoon_Flag;
        /*过流标志*/        
	    volatile uint8_t OverCurrent_Flag;   
    }Charger_Flag; 
    /*不同电池节数下的VBA倍率*/
    float Vbat_Ratio; 	       
}Charging_Typedef;

extern const Charging_Typedef g_Charger[CHANNEL_MAX];
extern void Charging_Poll(Charging_Typedef *const p_ch);
extern void Report_ChargerInfo(Charging_Typedef *const p_ch);
#ifdef __cplusplus
}
#endif

#endif /* __SC8913_H */