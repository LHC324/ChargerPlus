#include "charger.h"
#include "Dwin.h"
#include "ModbusMaster.h"

uint8_t g_Cloud_Buffer[CHANNEL_MAX * sizeof(float) * 4U] = {0};

/*初始化各充电通道*/
const Charging_Typedef g_Charger[CHANNEL_MAX] =
{	 /*通道1*/
	 {Channel0, No_Device, 0U, stdantby_Mode, {0.0F, 0.0F, 0U, 0.0F}, {0.0F, 0.0F, 0.0F, 0.0F}, {TRICKLE_CHARGE_CURRENT1_ADDR, SHOW_ANIMATION1_ADDR, CHARGE_STATE_NOW1_ADDR}, {false, false, false}, VBAT_RATIO}, /*参数默认VBAT倍率为5x，可以充1s-2s*/
	 /*通道2*/
	 {Channel1, No_Device, 0U, stdantby_Mode, {0.0F, 0.0F, 0U, 0.0F}, {0.0F, 0.0F, 0.0F, 0.0F}, {TRICKLE_CHARGE_CURRENT2_ADDR, SHOW_ANIMATION2_ADDR, CHARGE_STATE_NOW2_ADDR}, {false, false, false}, VBAT_RATIO},
	 /*通道3*/
	 {Channel2, No_Device, 0U, stdantby_Mode, {0.0F, 0.0F, 0U, 0.0F}, {0.0F, 0.0F, 0.0F, 0.0F}, {TRICKLE_CHARGE_CURRENT3_ADDR, SHOW_ANIMATION3_ADDR, CHARGE_STATE_NOW3_ADDR}, {false, false, false}, VBAT_RATIO}
};

/*按单个锂电池计算*/
static const float Battery_Interval[][2] =
{
		{1U * ONCE_MINVOLTAGE, 1U * ONCE_MAXVOLTAGE},
		{2U * ONCE_MINVOLTAGE, 2U * ONCE_MAXVOLTAGE},
		{3U * ONCE_MINVOLTAGE, 3U * ONCE_MAXVOLTAGE},
		{4U * ONCE_MINVOLTAGE, 4U * ONCE_MAXVOLTAGE}
};

/*检查电池接入*/
static uint8_t Get_Battery_Range(float persent_voltage);
static uint8_t Set_Charging_Mode(Charging_Typedef *const p_ch);
static uint8_t Get_StatusBit(Charging_Typedef *const p_ch, uint8_t abit);
static uint8_t Set_AcDef(Charging_Typedef *const p_ch);
static uint8_t Check_Connect(Charging_Typedef *const p_ch);
/*获取充电设备后台充电参数*/
static void Get_BackCharging_Parameters(Charging_Typedef *const p_ch);
static void Get_NowChargingMode(Charging_Typedef *const p_ch);
/*检查设备是否断开连接*/
static uint8_t Check_DisConnect(Charging_Typedef *const p_ch);
static void ChargingStage_Monitor(Charging_Typedef *const p_ch);
static void Set_Charging_Amimation(uint8_t state, uint32_t addr);
static void Set_Charging_State(Charging_State state, uint32_t addr);

/*不优化*/
// #pragma OPTIMIZE(0)
/**
 * @brief	充电器上报信息处理
 * @details	
 * @param	p_ch:当前通道
 * @retval	None
 */
void Report_ChargerInfo(Charging_Typedef *const p_ch)
{
	Foreground_Parameters Report = {0};
	uint16_t back_addr = CHARGE_VOLTAGE_NOW1_ADDR + DWIN_ADDR_OFFSET * p_ch->Channel_Id;
	uint16_t param_addr = PARAM_START_ADDR + DWIN_ADDR_OFFSET * p_ch->Channel_Id;
	uint8_t temp_buf[REGISTER_MAX * 2U] = {0};
	uint8_t i = 0;
	static uint8_t counter = 0;

	for(i = 0; i < (REGISTER_MAX * 2U); i += 2U)
	{
		temp_buf[i] = 0x00;
		temp_buf[i + 1U] = g_Sc8913_Registers[p_ch->Channel_Id][i / 2U];
	}

	if (p_ch->Charger_Flag.Is_ChargingFlag == true)
	{	/*充电还没有结束*/
		if (p_ch->CState != End_Charging)
		{
			/*累加充电时间*/
			p_ch->For_ChargingPara.Time++;
			/*统计充电电量*/
			p_ch->For_ChargingPara.Electricity += p_ch->For_ChargingPara.Current;
		}
	}
	else
	{ /*清空充电时间和电量*/
		p_ch->For_ChargingPara.Time = 0;
		p_ch->For_ChargingPara.Electricity = 0;
	}
	/*导入数据到上报区*/
	Report.Voltage = p_ch->For_ChargingPara.Voltage;
	Report.Current = p_ch->For_ChargingPara.Current;
	Report.Time = p_ch->For_ChargingPara.Time / 60U;
	Report.Electricity = p_ch->For_ChargingPara.Electricity * 10.0F / 36.0F; //(*1000/3600)
	/*上报前台数据*/
	Dwin_Write(back_addr, (uint8_t *)&Report, sizeof(Report));
	Set_Charging_State(p_ch->CState, p_ch->Charger_Show_Addr.State_Addr);
	Set_Charging_Amimation(p_ch->Charger_Flag.Cartoon_Flag, p_ch->Charger_Show_Addr.Animation_Addr);
	/*上报后台数据*/
	Dwin_Write(p_ch->Charger_Show_Addr.Back_Para_Addr, (uint8_t *)&p_ch->Bac_ChargingPara, sizeof(p_ch->Bac_ChargingPara));
	/*上报SC8913内部寄存器数据*/
	Dwin_Write(param_addr, temp_buf, sizeof(temp_buf));
	memcpy(&g_Cloud_Buffer[counter* sizeof(Report)] , (uint8_t *)&Report, sizeof(Report));
	if (++counter == CHANNEL_MAX)
	{
		/*把数据上报到4G模块*/
	    MOD_46H(SLAVEADDRESS, 0x0000, sizeof(g_Cloud_Buffer) / 2U, sizeof(g_Cloud_Buffer), g_Cloud_Buffer);
		counter = 0U;
	}
}

/*9级优化，速度优先*/
//#pragma OPTIMIZE(9, speed)
/**
 * @brief	获取当前通道BAT电流
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	BATCurrent
 */
float Get_BAT_Current(Charging_Typedef *const p_ch)
{
	return READ_IBAT_VALUE(p_ch->Channel_Id, IBAT_VALUE_ADDR);
}

/**
 * @brief	获取当前通道BAT电压
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	BATvoltage
 */
float Get_BAT_Voltage(Charging_Typedef *const p_ch)
{
	return ((4U * g_Sc8913_Registers[p_ch->Channel_Id][VBAT_FB_VALUE_ADDR] +
		   (((g_Sc8913_Registers[p_ch->Channel_Id][VBAT_FB_VALUE_ADDR + 1U]) & 0xc0) >> 6U) + 1.0F) *
			2.0F * p_ch->Vbat_Ratio / 1000.0F);
}

/**
 * @brief	根据检测到的电压，获取电池区间
 * @details	电池区间以单节锂离子电池计算
 * @param	p_ch:当前通道
 * @retval	false/true
 */
uint8_t Get_Battery_Range(float persent_voltage)
{
	uint16_t length = (sizeof(Battery_Interval) / sizeof(float)) / 2U;
	uint8_t i = 0;

	for (i = 0; i < length; i++)
	{
		if ((persent_voltage > Battery_Interval[i][0]) && (persent_voltage < Battery_Interval[i][1]))
		{
			return i;
		}
	}
	/*不在合理电压区间*/
	return 4U;
}

/**
 * @brief	通过获取当前电池区间，设置SC8913合适的充电参数
 * @details	设置好充电参数后，再检测设备(同时获取了电池节数)
 * @param	p_ch:当前通道
 * @retval	false/true
 */
uint8_t Set_Charging_Mode(Charging_Typedef *const p_ch)
{
	uint8_t ret = false;
	uint8_t dat = 0;

	switch (Get_Battery_Range(p_ch->For_ChargingPara.Voltage))
	{
	case 0:
	{
		p_ch->Batterry_Numbers = 0x01;
		dat = 0x01;
		REVISE_REGISTER(p_ch, dat, true, ret);
	}
	break;
	case 1:
	{
		p_ch->Batterry_Numbers = 0x02;
		dat = 0x09;
		REVISE_REGISTER(p_ch, dat, true, ret);
	}
	break;
	case 2:
	{
		p_ch->Batterry_Numbers = 0x03;
		dat = 0x11;
		REVISE_REGISTER(p_ch, dat, false, ret);
	}
	break;
	case 3:
	{
		p_ch->Batterry_Numbers = 0x04;
		dat = 0x19;
		REVISE_REGISTER(p_ch, dat, false, ret);
	}
	break;
	default:
		break;
	}
	return ret;
}

/**
 * @brief	获取STATUS寄存器中断位
 * @details	产生中断的位会被置1
 * @param	p_ch:当前通道;abit:位置
 * @retval	true/false
 */
/*STATUS: 7:DM_L 6:AC_OK 5:IDENT2 4:IDENT1 3:VBUSSHORT 2:OTP 1:EOC 0:None*/	
uint8_t Get_StatusBit(Charging_Typedef *const p_ch, uint8_t abit)
{	/*获取中断位在STATUS中位置*/
	uint8_t site = (0x01 << abit);
	
	if(site & g_Sc8913_Registers[p_ch->Channel_Id][STATUS_ADDR])
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief	检测AC_Ok中断位
 * @details	当产生AC_OK中断时，动态跟随AC_DEF位
 * @param	p_ch:当前通道
 * @retval	true:交流电源接入;false:交流断开,或者VBAT > VBUS
 */
uint8_t Set_AcDef(Charging_Typedef *const p_ch)
{
	uint8_t temp = 0;

	/*AC_OK位被置为1*/
	if(Get_StatusBit(p_ch, AC_OK_BIT))
	{	/*设置AC_DEF位为1:当VCIN < 3 || VBUS < VBAT时，AC_0k = 0*/
        temp = SET_CTRL0_SET(1U);
        if(!Sc8913_Write_Register(p_ch->Channel_Id, CTRL0_SET_ADDR, &temp, sizeof(temp)))
        {	/*错误码*/
            return 2U;
        }
		return true;
	}
	/*AC_OK位被置为0*/
	else
	{	/*清除AC_DEF位为0：当VCIN > 3V, AC_OK = 1; VCIN < 3,AC_ok = 0*/
        temp = SET_CTRL0_SET(0U);
        if(!Sc8913_Write_Register(p_ch->Channel_Id, CTRL0_SET_ADDR, &temp, sizeof(temp)))
        {	/*错误码*/
            return 2U;
        }
		return false;
	}
}

/**
 * @brief	充电投票
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	None
 */
void Charging_Poll(Charging_Typedef *const p_ch)
{
	/*可以根据返回值确定SC8913是否接入交流电*/
	Set_AcDef(p_ch);
	/*读取当前通道BAT电压*/
	p_ch->For_ChargingPara.Voltage = Get_BAT_Voltage(p_ch);
	/*读取当前通道BAT电流*/
	p_ch->For_ChargingPara.Current = Get_BAT_Current(p_ch);
	/*待机阶段*/
	if (p_ch->Charger_Flag.Is_ChargingFlag == false)
	{ 	/*是否有设备*/
		if (Check_Connect(p_ch) == true)
		{
			p_ch->Charger_Flag.Is_ChargingFlag = true;
			p_ch->Charger_Flag.Cartoon_Flag = true;
			Get_BackCharging_Parameters(p_ch);
		}
		else
		{
			p_ch->CState = stdantby_Mode;
		}
	}
	else
	/*充电阶段*/
	{
		/*获得当前电池充电状态*/
		Get_NowChargingMode(p_ch);
		ChargingStage_Monitor(p_ch);
	}
}

/**
 * @brief	检测是否有设备接入
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	false/true
 */
uint8_t Check_Connect(Charging_Typedef *const p_ch)
{
	static uint8_t mutex[CHANNEL_MAX] = {false, false, false};
	static uint8_t counts[CHANNEL_MAX] = {0, 0, 0};

	/*如果已经检测到电压，则设置芯片合适参数*/
	if (p_ch->For_ChargingPara.Voltage > LOWEST_VOLTAGE)
	{ /*在芯片输出关闭情况下能够检测到电压，说明是电池设备*/
		p_ch->Dev_Type = Cell;
		/*避免重复设置充电参数*/
		if (!mutex[p_ch->Channel_Id])
		{
			if (!Set_Charging_Mode(p_ch))
			{
				return false;
			}
			/*打开芯片电源输出*/
			Set_Sc8913_Stop(p_ch->Channel_Id, Low);
			mutex[p_ch->Channel_Id] = true;
		}
	}
	else /*无电池接入*/
	{
		return false;
	}

	/*是否检测到电流*/
	if (p_ch->For_ChargingPara.Current > CHECK_CURRENT)
	{ 
		mutex[p_ch->Channel_Id] = false;
		return true;
	}
	/*接入的设备无需充电*/
	else
	{	/*连续检测5s*/
		if (++counts[p_ch->Channel_Id] >= OPENTIME)
		{
			counts[p_ch->Channel_Id] = 0;
			/*关闭芯片电源输出*/
			Set_Sc8913_Stop(p_ch->Channel_Id, High);
			p_ch->CState = End_Charging;
			return true;
		}
	}

	return false;
}

/**
 * @brief	计算出当前通道下电池的后台信息
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	None
 */
void Get_BackCharging_Parameters(Charging_Typedef *const p_ch)
{ 
	/*涓流目标电压*/
	p_ch->Bac_ChargingPara.Trickle_Target_Voltage = ONCE_MINVOLTAGE * p_ch->Batterry_Numbers;
	/*涓流目标电流*/
	p_ch->Bac_ChargingPara.Trickle_Target_Current = TRICKLE_CURRNET;
	/*恒流目标电压*/
	p_ch->Bac_ChargingPara.ConstantCurrent_Target_Voltage = ONCE_FULLVOLTAGE * p_ch->Batterry_Numbers;
	/*恒流目标电流:利用动态总功率算出恒流电流*/
	p_ch->Bac_ChargingPara.ConstantCurrent_Target_Current = (IBAT_MAX_CURRENT * ONCE_MAXVOLTAGE) / p_ch->For_ChargingPara.Voltage;
	/*恒压目标电压*/
	p_ch->Bac_ChargingPara.ConstantVoltage_Target_Voltage = ONCE_MAXVOLTAGE * p_ch->Batterry_Numbers;
	/*恒压目标电流*/
	p_ch->Bac_ChargingPara.ConstantVoltage_Target_Current = CHECK_CURRENT;
}

/**
 * @brief	计算出当前通道下电池的后台信息
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	None
 */
void Get_NowChargingMode(Charging_Typedef *const p_ch)
{
	if (p_ch->CState == End_Charging) 
		return;

	if (p_ch->For_ChargingPara.Voltage < LOWEST_VOLTAGE)
	{
		p_ch->CState = stdantby_Mode;
	}/*当前充电对象电压<=涓流电压*/
	else if (p_ch->For_ChargingPara.Voltage <= p_ch->Bac_ChargingPara.Trickle_Target_Voltage)
	{
		p_ch->CState = Trickle_Mode;
	}
	else if (p_ch->For_ChargingPara.Voltage <= p_ch->Bac_ChargingPara.ConstantVoltage_Target_Voltage)
	{
		p_ch->CState = Constant_Current_Mode;
	}
	else
	{
		p_ch->CState = Constant_Voltage_Mode;
	}
}

/**
 * @brief	充电阶段监测
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	None
 */
void ChargingStage_Monitor(Charging_Typedef *const p_ch)
{ 
	/*是否充电结束*/
	if(Get_StatusBit(p_ch, EOC_BIT) == true)
	{ /*电池被拔掉也会造成EOC位置1，在确定当前通道是否还有电压*/
		if (p_ch->For_ChargingPara.Voltage > LOWEST_VOLTAGE)
		{ /*认为充电结束*/
			p_ch->CState = End_Charging;
			/*关闭芯片输出引脚*/
			Set_Sc8913_Stop(p_ch->Channel_Id, High);
		}
	}
	/*由于电流消失而造成的断路*/
	if (Check_DisConnect(p_ch) == true)
	{
		p_ch->Charger_Flag.Is_ChargingFlag = false;
		/*待机*/
		p_ch->CState = stdantby_Mode;
		p_ch->Dev_Type = No_Device;
		/*关闭充电动画*/
		p_ch->Charger_Flag.Cartoon_Flag = false;
	} 
}

/**
 * @brief	检测充电设备是否断开连接
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	None
 */
uint8_t Check_DisConnect(Charging_Typedef *const p_ch)
{
	if ((p_ch->For_ChargingPara.Current < CHECK_CURRENT) && (p_ch->For_ChargingPara.Voltage < LOWEST_VOLTAGE))
	{
		return true;
	}

	return false;
}

/**
 * @brief	设置充电动画
 * @details	轮询处理每个通道
 * @param	state:状态,addr:地址
 * @retval	None
 */
void Set_Charging_Amimation(uint8_t state, uint32_t addr)
{
	uint16_t temp_value = 0x0000;

	if (state == true)
	{
		temp_value = 0x01;
	}

	Dwin_Write(addr, (uint8_t *)&temp_value, sizeof(temp_value));
}

/**
 * @brief	设置充电状态
 * @details	轮询处理每个通道
 * @param	state:状态,addr:地址
 * @retval	None
 */
void Set_Charging_State(Charging_State state, uint32_t addr)
{
	uint16_t temp_value = (uint8_t)state;
	/*把充电图标地址和状态对应起来*/
	Dwin_Write(addr, (uint8_t *)&temp_value, sizeof(temp_value));
}
