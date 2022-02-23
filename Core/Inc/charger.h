#ifndef __CHARGER_H
#define __CHARGER_H

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"

#define  CHANNEL_MAX		3U    		        //通道数
#define  LOWEST_CELL_VOLTAGE            3.0F    //锂电池最低电压
#define  OUT_LOWEST_VOLTAGE		        3.0F    //充电器最小输出电压
#define  OUT_HIGHEST_VOLTAGE	        16.0F   //充电器最大输出电压
// #define  CHECK_CURRENT		            0.20F    //启动充电电流
//#define  VOLTAGE_CALIBRATION_ERROR     4.0F	//硬件电路实际输出电压误差
#define  INSTANTANEOUS_MAXIMUM_CURRENT  4.0F     //瞬时最大电流
#define  SWEEP_VOLTAGE 					0.25F    //通道扫描电压
#define  SWITCH_TIMERS(C)		        (C * 5)       //MOS管切断等待时间，单位ms

/*输入电源总功率:120W*/
#define TOTAL_POWER         120.0F
/*硬件允许输出最高电压*/
#define MAX_HARDWARE_VOLTAGE 18.0F
/*硬件允许输出最小电压*/
#define MIN_HARDWARE_VOLTAGE 3.0F
/*允许最大充电时间:3小时*/
#define MAX_CHARGING_TIMES   180U
/*允许充的最小电池容量*/
#define MIN_CAPACITY        500U
/*允许充的最大电池容量*/
#define MAX_CAPACITY        5000U
/*最低检测电流 > 90mA*/
#define CHECK_CURRENT       0.09F
/*IBAT端最大输出电流*/
#define IBAT_MAX_CURRENT    3.0F
/*单个单元最大充电电流选用电池容量的0.60C*/
#define ONCE_MAXCURRENT	    0.60F	
/*单个单元标准充电电流选用电池容量的0.40C */   
#define ONCE_STDCURRENT		0.40F
/*单个单元最小充电电流选用电池容量的0.10C*/    
#define ONCE_MINCURRENT		0.10F	   
/*充电截止电流:电池容量1/10*/
#define END_CURRNRT(c)      (ONCE_MINCURRENT * c)
/*涓流阶段电流时IBAT的1/10*/
#define TRICKLE_CURRNET     (ONCE_MINCURRENT * IBAT_MAX_CURRENT)
/*通道电压下限3.0V*/
#define LOWEST_VOLTAGE		2.0F      
/*锂电池单节允许最高电压*/
#define ONCE_MAXVOLTAGE     4.20F
/*锂电池单节允许最低电压为SC8913BAT电压的70%*/
#define ONCE_MINVOLTAGE     (ONCE_MAXVOLTAGE * 0.7F)
/*SC8913认为充满时电压*/
#define ONCE_FULLVOLTAGE    (ONCE_MAXVOLTAGE * 0.98F)
/*DCDC设备内部二极管压降0.7V*/
#define VOLTAGE_DROP        0.7F
/*断路次数*/
#define OPENTIME            50U
/*充电参数设置辅助次数*/
#define AUX_COUNT           3U
/*区间错误码*/
#define INTERVAL_ERROR_CODE 0x10

/*充电器充电参数*/
#define CHARGER_MAXCURRENT		1.50F	//充电器单通道最大电流
#define CHARGER_ENDCURRENT		0.20F	//充电结束电流在200MA左右
#define ONECELL_BASEVOLTAGE		2.75F	//单节锂电池最小放电电压
#define ONECELL_ENDVOLTAGE		4.25F	//单节锂电池充电截止电压
#define TRRICKLE_CURRENT		(CHARGER_MAXCURRENT * 0.20F)	//涓流充电目标电流(最大充电电流0.2c)
#define TRRICKLE_VOLTAGE		3.00F	//涓流充电目标电压
#define CURRENT_RATIO           20.0F   //恒流阶段电流调整倍率
#define MIN_OFFSET_CURRENT      0.01F   //电流变化差值


/*使用卡尔曼滤波*/
#define KALMAN  1

#if(KALMAN == 1)
/*以下为卡尔曼滤波参数*/
#define LASTP           0.500F   //上次估算协方差
#define COVAR_Q 	    0.005F   //过程噪声协方差
#define COVAR_R 		0.067F   //测噪声协方差

typedef struct
{
    float Last_Covariance;			//上次估算协方差 初始化值为0.02
    float Now_Covariance;			//当前估算协方差 初始化值为0
    float Output;					//卡尔曼滤波器输出 初始化值为0
    float Kg;						//卡尔曼增益 初始化值为0
    float Q;						//过程噪声协方差 初始化值为0.001
    float R;						//观测噪声协方差 初始化值为0.543
}KFP;

extern float kalmanFilter(KFP *kfp, float input);
#else
/*左移次数*/
#define FILTER_SHIFT 4U

typedef struct
{
	bool  First_Flag;
	float SideBuff[1 << FILTER_SHIFT];
    float *Head;
    float Sum;
}SideParm;
extern float sidefilter(SideParm *side, float input);
#endif


/*定义STATUS寄存器位*/
#define EOC_BIT 0x01
#define OTP_BIT 0x02
#define VBUS_SHORT_BIT 0x03
#define INDET1_BIT 0x04
#define INDET2_BIT 0x05
#define AC_OK_BIT 0x06
#define DM_L_BIT 0x07

/*采样类型*/
typedef enum
{
    VBUS = 0,
    VBAT,
    IBAT
}S_Type;

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

/*后台上报数据：保存到内部flash*/
typedef struct
{  
    float Trickle_Target_Current;	 		//涓流目标电流
	float Trickle_Target_Voltage;  			//涓流目标电压

	float ConstantCurrent_Target_Current;  	//恒流目标电流
	float ConstantCurrent_Target_Voltage;  	//恒流目标电压

	float ConstantVoltage_Target_Current;   //恒压目标电流
	float ConstantVoltage_Target_Voltage;	//恒压目标电压
    uint32_t Battery_Capacity;              //电池容量  
}Background_Parameters __attribute__((aligned(4)));


/*通道层封装*/
typedef enum 
{
    Channel0 = 0x00,
    Channel1 = 0x01,
    Channel2 = 0x02
}SPI_Channel;

typedef enum
{
    Input_A = 0x00,
    Input_B
}MCP48xx_Channel;

typedef struct 
{
    MCP48xx_Channel Mcpxx_Id;
    SPI_Channel Spi_Id;
}Charger_Channel;

#if(KALMAN == 1)
typedef struct
{
	KFP	     Vbus_Kfp_Filter;
    uint32_t Vbus_Channel;
    KFP	     Vbat_Kfp_Filter;
    uint32_t Vbat_Channel;
    KFP	     Ibat_Kfp_Filter;
    uint32_t Ibat_Channel;
	// volatile bool voltage_Filte_Flag;
}ADC_Filter;
#else
typedef struct
{			  
	sideparm Vbus_Slide_Filter;
    sideparm Vbat_Slide_Filter;
    sideparm Ibat_Slide_Filter;
	// volatile bool voltage_Filte_Flag;
}ADC_Filter;
#endif

typedef struct 
{
    /*当前通道ID*/
    Charger_Channel Channel_Id;
    /*通道滤波系数*/
    ADC_Filter Factor;
    /*设备类型*/	
    Device_Type    Dev_Type;
    /*锂电池节数*/
    uint32_t Batterry_Numbers;
    /*充电状态*/
	Charging_State  CState;
    /*充电电压*/
    __IO float Charging_Voltage;
    /*当前通道需求功率*/
    float Need_Power;
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
        volatile bool Is_ChargingFlag;
        /*设备插入标志*/
        volatile bool Insert_Flag;
        /*充电动画状态 */  	           
	    volatile bool Cartoon_Flag;
        /*过流标志*/        
	    volatile bool OverCurrent_Flag;   
    }Charger_Flag; 
    /*不同电池节数下的VBA倍率*/
    // float Vbat_Ratio; 	       
}Charging_Typedef __attribute__((aligned(4)));

extern Charging_Typedef g_Charger[CHANNEL_MAX];
extern float Get_Vbus_Vbat(Charging_Typedef *const p_ch, S_Type type);
extern float Get_Ibat(Charging_Typedef *const p_ch);
extern void Open_StartSignal(Charging_Typedef *const p_ch);
extern void Close_StartSignal(Charging_Typedef *const p_ch);
extern void Set_Voltage(Charging_Typedef *const p_ch);
extern void Get_Voltage_MicroCompensate(Charging_Typedef *const p_ch, const float UserCurrent);
extern void Charging_Sampling(Charging_Typedef *const p_ch);
extern void Charging_Poll(Charging_Typedef *const p_ch);
extern void Report_ChargerInfo(Charging_Typedef *const p_ch);
#ifdef __cplusplus
}
#endif

#endif /* __SC8913_H */
