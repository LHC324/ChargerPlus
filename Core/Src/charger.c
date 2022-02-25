#include "charger.h"
#include "adc.h"
#include "Dwin.h"
#include "ModbusMaster.h"
#include "Mcp4822.h"
#include "cmsis_os.h"
#include "tim.h"
#include "clibration.h"
#include "shell.h"
#include "shell_port.h"

/*电源剩余功率*/
float g_Residual_Power = TOTAL_POWER;
uint8_t g_Cloud_Buffer[CHANNEL_MAX * sizeof(float) * 4U] = {0};
float g_Voltage_Array[CHANNEL_MAX + 1U] = {5.0F, 9.0F, 13.3F, 17.5F};

/*初始化各充电通道*/
Charging_Typedef g_Charger[CHANNEL_MAX] =
	{/*通道0*/
	 {{Input_A, Channel0}, {{LASTP, 0, 0, 0, COVAR_Q, COVAR_R}, ADC_CHANNEL_0, {LASTP, 0, 0, 0, COVAR_Q, COVAR_R}, ADC_CHANNEL_1, {LASTP, 0, 0, 0, COVAR_Q, COVAR_R}, ADC_CHANNEL_2}, No_Device, .Batterry_Numbers = 0U, stdantby_Mode, 0, 0, {0.0F, 0.0F, 0U, 0.0F}, {0U, 0.0F, 0.0F, 0.0F, 0.0F}, {TRICKLE_CHARGE_CURRENT1_ADDR, SHOW_ANIMATION1_ADDR, CHARGE_STATE_NOW1_ADDR}, {false, false, false, false}},
	 /*通道1*/
	 {{Input_B, Channel1}, {{LASTP, 0, 0, 0, COVAR_Q, COVAR_R}, ADC_CHANNEL_3, {LASTP, 0, 0, 0, COVAR_Q, COVAR_R}, ADC_CHANNEL_4, {LASTP, 0, 0, 0, COVAR_Q, COVAR_R}, ADC_CHANNEL_5}, No_Device, 0U, stdantby_Mode, 0, 0, {0.0F, 0.0F, 0U, 0.0F}, {0U, 0.0F, 0.0F, 0.0F, 0.0F}, {TRICKLE_CHARGE_CURRENT2_ADDR, SHOW_ANIMATION2_ADDR, CHARGE_STATE_NOW2_ADDR}, {false, false, false, false}},
	 /*通道2*/
	 {{Input_A, Channel2}, {{LASTP, 0, 0, 0, COVAR_Q, COVAR_R}, ADC_CHANNEL_6, {LASTP, 0, 0, 0, COVAR_Q, COVAR_R}, ADC_CHANNEL_7, {LASTP, 0, 0, 0, COVAR_Q, COVAR_R}, ADC_CHANNEL_8}, No_Device, 0U, stdantby_Mode, 0, 0, {0.0F, 0.0F, 0U, 0.0F}, {0U, 0.0F, 0.0F, 0.0F, 0.0F}, {TRICKLE_CHARGE_CURRENT3_ADDR, SHOW_ANIMATION3_ADDR, CHARGE_STATE_NOW3_ADDR}, {false, false, false, false}}};

/*按单个锂电池计算*/
static const float Battery_Interval[][2] =
	{
		{0U * ONCE_MINVOLTAGE, 1U * ONCE_MAXVOLTAGE},
		{1U * ONCE_MAXVOLTAGE, 2U * ONCE_MAXVOLTAGE},
		{2U * ONCE_MAXVOLTAGE, 3U * ONCE_MAXVOLTAGE},
		{3U * ONCE_MAXVOLTAGE, 4U * ONCE_MAXVOLTAGE}};

/*静态函数声明*/
static uint32_t Get_Battery_Range(float persent_voltage);
static bool Check_Connect(Charging_Typedef *const p_ch);
/*获取充电设备后台充电参数*/
static uint32_t Get_BackCharging_Parameters(Charging_Typedef *const p_ch);
/*检查设备是否断开连接*/
static bool Check_DisConnect(Charging_Typedef *const p_ch);
static void ChargingStage_Monitor(Charging_Typedef *const p_ch);
static void Set_Charging_Amimation(uint8_t state, uint32_t addr);
static void Set_Charging_State(Charging_State state, uint32_t addr);

#define Get_Start_Pin(ch) \
	((ch == Channel0) ? START0_Pin : ((ch == Channel1) ? START1_Pin : START2_Pin))
/**
 * @brief   使能充电输出开关
 * @details
 * @param	p_ch:当前通道
 * @retval	None
 */
__inline void Open_StartSignal(Charging_Typedef *const p_ch)
{
	uint16_t GPIO_Pinx = Get_Start_Pin(p_ch->Channel_Id.Spi_Id);

	HAL_GPIO_WritePin(GPIOB, GPIO_Pinx, GPIO_PIN_RESET);
}

/**
 * @brief   关闭充电输出开关
 * @details
 * @param	p_ch:当前通道
 * @retval	None
 */
void Close_StartSignal(Charging_Typedef *const p_ch)
{
	uint16_t GPIO_Pinx = Get_Start_Pin(p_ch->Channel_Id.Spi_Id);

	HAL_GPIO_WritePin(GPIOB, GPIO_Pinx, GPIO_PIN_SET);
	/*关闭输出开关的同时设置充电电压最低*/
	p_ch->Charging_Voltage = 0;
	Set_Voltage(p_ch);
}

/**
 * @brief	充电器上报信息处理
 * @details
 * @param	p_ch:当前通道
 * @retval	None
 */
void Report_ChargerInfo(Charging_Typedef *const p_ch)
{
	Foreground_Parameters Report = {0};
	uint16_t back_addr = CHARGE_VOLTAGE_NOW1_ADDR + DWIN_ADDR_OFFSET * p_ch->Channel_Id.Spi_Id;
	static uint8_t counter = 0;
	static uint32_t temp_times[CHANNEL_MAX] = {0};
	/*避免大小端交换导致数据不一致，采用副本*/
	Background_Parameters Back_Para = {0};

	if (p_ch->Charger_Flag.Is_ChargingFlag == true)
	{ /*充电还没有结束*/
		if (p_ch->CState != End_Charging)
		{
			/*累加充电时间*/
			temp_times[p_ch->Channel_Id.Spi_Id]++;
			p_ch->For_ChargingPara.Time = temp_times[p_ch->Channel_Id.Spi_Id] / 60U;
			/*统计充电电量*/
			p_ch->For_ChargingPara.Electricity += (p_ch->For_ChargingPara.Current * 10.0F / 36.0F);//(*1000/3600)
		}
	}
	else
	{ /*清空充电时间和电量*/
		temp_times[p_ch->Channel_Id.Spi_Id] = 0;
		p_ch->For_ChargingPara.Time = 0;
		p_ch->For_ChargingPara.Electricity = 0;
		
	}
	/*导入数据到上报区*/
	memcpy((uint8_t *)&Report, (uint8_t *)&p_ch->For_ChargingPara, sizeof(Report));
	/*小端数据转换成大端*/
	for (uint16_t i = 0; i < sizeof(Report) / sizeof(float); i++)
	{
		Endian_Swap((uint8_t *)&Report, i * 4U, sizeof(float));
	}
	/*上报前台数据*/
	Dwin_Write(back_addr, (uint8_t *)&Report, sizeof(Report));
	Set_Charging_State(p_ch->CState, p_ch->Charger_Show_Addr.State_Addr);
	Set_Charging_Amimation(p_ch->Charger_Flag.Cartoon_Flag, p_ch->Charger_Show_Addr.Animation_Addr);
	/*没有设备时，不上报后台参数*/
	if (p_ch->Dev_Type != No_Device)
	{
		memcpy((uint8_t *)&Back_Para, (uint8_t *)&p_ch->Bac_ChargingPara, sizeof(Back_Para));
		/*小端数据转换成大端*/
		for (uint16_t i = 0; i < sizeof(Back_Para) / sizeof(float); i++)
		{
			Endian_Swap((uint8_t *)&Back_Para, i * 4U, sizeof(float));
		}
		/*上报后台数据*/
		Dwin_Write(p_ch->Charger_Show_Addr.Back_Para_Addr, (uint8_t *)&Back_Para, sizeof(Back_Para));
	}

	memcpy(&g_Cloud_Buffer[counter * sizeof(Report)], (uint8_t *)&Report, sizeof(Report));
	if (++counter == CHANNEL_MAX)
	{
		/*把数据上报到4G模块*/
		// MOD_46H(SLAVEADDRESS, 0x0000, sizeof(g_Cloud_Buffer) / 2U, sizeof(g_Cloud_Buffer), g_Cloud_Buffer);
		counter = 0U;
	}
}

/**
 * @brief	获取当前通道输出到DAC的值
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	BUSvoltage
 */
__inline uint16_t Get_Dac_Value(SPI_Channel ch, float voltage)
{
	// float temp_v;
	// /*硬件限制电压不能低于3.0V,否则反向增大*/
	// temp_v = (voltage <= MIN_HARDWARE_VOLTAGE) ? MIN_HARDWARE_VOLTAGE : voltage;

	return ((uint16_t)(Dac.SKx[ch] * voltage + Dac.SGx[ch]));
}


/**
 * @brief	获取当前通道BUS/BAT电压
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @param   type:VBUS/VBAT
 * @retval	BUSvoltage
 */
float Get_Vbus_Vbat(Charging_Typedef *const p_ch, S_Type type)
{
	SPI_Channel ch = p_ch->Channel_Id.Spi_Id;
	uint32_t site = ch * 3U + type;
	uint32_t value = Get_AdcValue(site);

	return (value ? (Adc.SPx[ch][type] * (float)value + Adc.SQx[ch][type]) : 0);
}

/**
 * @brief	获取当前通道BAT电流
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	BUSvoltage
 */
float Get_Ibat(Charging_Typedef *const p_ch)
{
	SPI_Channel ch = p_ch->Channel_Id.Spi_Id;
	uint32_t value = Get_AdcValue(p_ch->Factor.Ibat_Channel);

	return (value ? (Adc.SPx[ch][2U] * (float)value + Adc.SQx[ch][2U]) : 0);
}

/**
 * @brief	根据检测到的电压，获取电池区间
 * @details	电池区间以单节锂离子电池计算
 * @param	p_ch:当前通道
 * @retval	site
 */
uint32_t Get_Battery_Range(float persent_voltage)
{
	uint32_t length = (sizeof(Battery_Interval) / sizeof(float)) / 2U;

	for (uint32_t i = 0; i < length; i++)
	{
		if ((persent_voltage >= Battery_Interval[i][0]) && (persent_voltage <= Battery_Interval[i][1]))
		{
			return i;
		}
	}
	/*不在合理电压区间*/
	return INTERVAL_ERROR_CODE;
}

#define Get_Check_Port(ch) \
	((ch == Channel0) ? CHECK0_EN_GPIO_Port : ((ch == Channel1) ? CHECK1_EN_GPIO_Port : CHECK2_EN_GPIO_Port))

#define Get_Check_Pin(ch) \
	((ch == Channel0) ? CHECK0_EN_Pin : ((ch == Channel1) ? CHECK1_EN_Pin : CHECK2_EN_Pin))

typedef osStatus (*Func_t)(uint32_t);
/**
 * @brief	建立开始条件
 * @details
 * @param	p_ch:当前通道
 * @retval	None
 */
void Start_Check(Charging_Typedef *const p_ch, Func_t Delay_ms)
{
	GPIO_TypeDef *pGPIOx = Get_Check_Port(p_ch->Channel_Id.Spi_Id);
	uint16_t GPIO_Pinx = Get_Check_Pin(p_ch->Channel_Id.Spi_Id);

	HAL_GPIO_WritePin(pGPIOx, GPIO_Pinx, GPIO_PIN_RESET);
	// osDelay(2);
	Delay_ms(2);
	HAL_GPIO_WritePin(pGPIOx, GPIO_Pinx, GPIO_PIN_SET);
}

/**
 * @brief	设置check信号
 * @details
 * @param	p_ch:当前通道
 * @retval	None
 */
void Set_Check(Charging_Typedef *const p_ch, GPIO_PinState PinState)
{
	GPIO_TypeDef *pGPIOx = Get_Check_Port(p_ch->Channel_Id.Spi_Id);
	uint16_t GPIO_Pinx = Get_Check_Pin(p_ch->Channel_Id.Spi_Id);

	HAL_GPIO_WritePin(pGPIOx, GPIO_Pinx, PinState);
}

/**
 * @brief	电池设备充电处理
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	None
 */
void Cell_Handle(Charging_Typedef *const p_ch)
{
	float now_voltage = p_ch->For_ChargingPara.Voltage;

	if (p_ch->CState == End_Charging)
	{
		return;
	}

	p_ch->CState = (now_voltage < LOWEST_VOLTAGE ? (stdantby_Mode) : (now_voltage <= p_ch->Bac_ChargingPara.Trickle_Target_Voltage ? (Trickle_Mode) : (now_voltage <= p_ch->Bac_ChargingPara.ConstantVoltage_Target_Voltage ? (Constant_Current_Mode) : (Constant_Voltage_Mode))));
	switch (p_ch->CState)
	{
	case Trickle_Mode:
	{
		Get_Voltage_MicroCompensate(p_ch, p_ch->Bac_ChargingPara.Trickle_Target_Current);
	}break;
	case Constant_Current_Mode:
	{
		Get_Voltage_MicroCompensate(p_ch, p_ch->Bac_ChargingPara.ConstantCurrent_Target_Current);
	}break;
	case Constant_Voltage_Mode:
	{
		// p_ch->Charging_Voltage = p_ch->Bac_ChargingPara.ConstantVoltage_Target_Voltage;
	}break;
	}
	Set_Voltage(p_ch);
}

/**
 * @brief	DCDC设备充电处理
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	None
 */
void Dcdc_Handle(Charging_Typedef *const p_ch)
{	/*DCDC设备默认按恒压充电*/
	p_ch->CState = Constant_Voltage_Mode;
	/*DCDC设备可以增加电流调节算法：1A/2A来增加充电功率*/
}

/**
 * @brief	充电投票
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	None
 */
void Charging_Poll(Charging_Typedef *const p_ch)
{
	// /*读取当前通道BAT电压*/
	// p_ch->For_ChargingPara.Voltage = Get_BAT_Voltage(p_ch);
	// /*读取当前通道BAT电流*/
	// p_ch->For_ChargingPara.Current = Get_BAT_Current(p_ch);

	// p_ch->Charging_Voltage = 5.20F;
	// Set_Voltage(p_ch);
	// return;

	/*待机阶段*/
	if (p_ch->Charger_Flag.Is_ChargingFlag == false)
	{
		/*是否有设备*/
		if (Check_Connect(p_ch) == true)
		{
			/*获取电池充电参数*/
			if (Get_BackCharging_Parameters(p_ch) != INTERVAL_ERROR_CODE)
			{ /*不在电池合理区间，不工作*/
				/*打开充电开关*/
				Open_StartSignal(p_ch);
				p_ch->Charger_Flag.Is_ChargingFlag = true;
				/*检测阶段，不认为电量已满*/
				if (p_ch->CState != End_Charging)
				{
					p_ch->Charger_Flag.Cartoon_Flag = true;
				}
			}
		}
		else
		{ /*待机*/
			p_ch->CState = stdantby_Mode;
			/*关闭充电动画*/
			p_ch->Charger_Flag.Cartoon_Flag = false;
			/*无设备*/
			p_ch->Dev_Type = No_Device;
			/*关闭充电开关*/
			Close_StartSignal(p_ch);
		}
	}
	else
	/*充电阶段*/
	{
		/*获得当前电池充电状态*/
		switch (p_ch->Dev_Type)
		{ /*电池设备充电处理*/
		case Cell: Cell_Handle(p_ch); break;
		case Dcdc: Dcdc_Handle(p_ch); break;	
		case No_Device: break;	
		}
		/*监测断或者充电结束*/
		ChargingStage_Monitor(p_ch);
	}
}

/**
 * @brief	通道轮询采样
 * @details
 * @param	p_ch:当前通道
 * @retval	None
 */
void Charging_Sampling(Charging_Typedef *const p_ch)
{ /*在检测阶段，为了保障速度，不要求精度，不使用卡尔曼滤波*/
	if (!p_ch->Charger_Flag.Is_ChargingFlag)
	{
		/*读取当前通道BUS电压*/
		/*读取当前通道BAT电压*/
		p_ch->For_ChargingPara.Voltage = Get_Vbus_Vbat(p_ch, VBAT);
		/*读取当前通道BAT电流*/
		p_ch->For_ChargingPara.Current = Get_Ibat(p_ch);
	}
	else
	{
		/*读取当前通道BUS电压*/
		/*读取当前通道BAT电压*/
		p_ch->For_ChargingPara.Voltage = kalmanFilter(&p_ch->Factor.Vbat_Kfp_Filter, Get_Vbus_Vbat(p_ch, VBAT));
		/*读取当前通道BAT电流*/
		p_ch->For_ChargingPara.Current = kalmanFilter(&p_ch->Factor.Ibat_Kfp_Filter, Get_Ibat(p_ch));
	}
}

/**
 * @brief	电压输出
 * @details	采用采样的ADC值与实际输出电压建立模型，得到的公式
 * @param	p_ch:当前通道
 * @retval	None
 */
void Set_Voltage(Charging_Typedef *const p_ch)
{
	p_ch->Charging_Voltage = (p_ch->Charging_Voltage <= MIN_HARDWARE_VOLTAGE) ? MIN_HARDWARE_VOLTAGE : p_ch->Charging_Voltage;
	__IO uint16_t data = Get_Dac_Value(p_ch->Channel_Id.Spi_Id, p_ch->Charging_Voltage);

	// shellPrint(&shell,"charging voltage :%f\r\n", p_ch->Charging_Voltage);
	/*进入临界区*/
	taskENTER_CRITICAL();
	/*保证电压恒定输出：稳压源*/
	Mcp48xx_Write(data, &p_ch->Channel_Id);
	/*退出临界区*/
	taskEXIT_CRITICAL();
}

/**
 * Get x sign bit only for little-endian
 * if x >= 0 then  1
 * if x <  0 then -1
 */
#define MathUtils_SignBit(x) \
	(((signed char *)&x)[sizeof(x) - 1] >> 7 | 1)

/**
 * @brief	获取电压调整补偿
 * @details
 * @param	p_ch:当前通道
 * @param   UserCurrent:目标电流
 * @retval	None
 */
void Get_Voltage_MicroCompensate(Charging_Typedef *const p_ch, const float UserCurrent)
{
	float Difference = UserCurrent - p_ch->For_ChargingPara.Current;
	float Absolute = fabs(Difference);
	float CurrentGap = 0;
	float Coefficient = UserCurrent / CURRENT_RATIO;

	/*目标电流不可能是负数*/
	if (MathUtils_SignBit(UserCurrent) == -1)
	{
		return;
	}
	if (Absolute && (Absolute > Coefficient))
	{
		/*获取符号位*/
		CurrentGap = MathUtils_SignBit(Difference) * MIN_OFFSET_CURRENT;
	}
	else
	/*允许误差或者已经调节在点上*/
	{
		CurrentGap = 0;
	}
	p_ch->Charging_Voltage += CurrentGap;
	// shellPrint(&shell,"Charging_Voltage: %f\r\n", p_ch->Charging_Voltage);
}

/**
 * @brief	功率分配
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	BUSvoltage
 */
bool Power_Malloc(Charging_Typedef *const p_ch)
{ /*如果剩余功率不够分,则成倍递减，直到够分为止*/
	while (p_ch->Need_Power > g_Residual_Power)
	{
		p_ch->Need_Power /= 2.0F;
		/*如果需求功率已经小于功率下限，则分配失败*/
		if (p_ch->Need_Power < 2.0F)
		{
			return false;
		}
	}
	g_Residual_Power -= p_ch->Need_Power;

	return true;
}

/**
 * @brief	获取当前通道输出到DAC的值
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	BUSvoltage
 */
__inline void Power_Free(Charging_Typedef *const p_ch)
{
	g_Residual_Power += p_ch->Need_Power;
}

/**
 * @brief	检测是否有设备接入
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	false/true
 */
bool Check_Connect(Charging_Typedef *const p_ch)
{
	static uint32_t counter1[CHANNEL_MAX] = {0, 0, 0};
	static bool mutex[CHANNEL_MAX] = {false, false, false};

	/*设置电池默认容量*/
	p_ch->Bac_ChargingPara.Battery_Capacity = 2500U;

	/*如果已经检测到电压，则设置芯片合适参数*/
	if ((p_ch->For_ChargingPara.Voltage > LOWEST_VOLTAGE) &&
		(p_ch->For_ChargingPara.Voltage < MAX_HARDWARE_VOLTAGE) &&
		!mutex[p_ch->Channel_Id.Spi_Id])
	{	
		if (++counter1[p_ch->Channel_Id.Spi_Id] >= 5U)
		{
			counter1[p_ch->Channel_Id.Spi_Id] = 0U;
			/*在芯片输出关闭情况下能够检测到电压，说明是电池设备*/
			p_ch->Dev_Type = Cell;
			/*设置起充电压*/
			p_ch->Charging_Voltage = p_ch->For_ChargingPara.Voltage + 0.25F;
			Set_Voltage(p_ch);
			return true;
		}
	}
	else /*无电池接入、DCDC设备接入*/
	{	
		if (!mutex[p_ch->Channel_Id.Spi_Id])
		{
			mutex[p_ch->Channel_Id.Spi_Id] = true;
			/*建立检测条件：start信号失效*/
			// Start_Check(p_ch, &osDelay);
			Set_Check(p_ch, GPIO_PIN_SET);
		}

		if (++counter1[p_ch->Channel_Id.Spi_Id] >= 5U)
		{
			counter1[p_ch->Channel_Id.Spi_Id] = 0U;
			mutex[p_ch->Channel_Id.Spi_Id] = false;

			if (p_ch->Charger_Flag.Insert_Flag == true)
			{
				p_ch->Charger_Flag.Insert_Flag = false;
				shellPrint(&shell, "ch:%d,insert!\r\n", p_ch->Channel_Id.Spi_Id);

				// if (p_ch->For_ChargingPara.Current > CHECK_CURRENT)
				{
					p_ch->Dev_Type = Dcdc;
					p_ch->Charging_Voltage = 5.0F;
					Set_Voltage(p_ch);
					return true;
				}
			}
			else
			{
				Set_Check(p_ch, GPIO_PIN_RESET);
			}
		}
		// else
		// {
		// 	/*检测dcdc设备期间，输出最低电压*/
		// 	p_ch->Charging_Voltage = 0;
		// 	Set_Voltage(p_ch);
		// }
		 	

		// if (++counter1[p_ch->Channel_Id.Spi_Id] == 3U)
		// {
		// 	counter1[p_ch->Channel_Id.Spi_Id] = 0U;
		// 	mutex[p_ch->Channel_Id.Spi_Id] = false;
				
		// 	if (p_ch->Charger_Flag.Insert_Flag == true)
		// 	{
		// 		p_ch->Charger_Flag.Insert_Flag = false;

		// 		if (p_ch->For_ChargingPara.Current > CHECK_CURRENT)
		// 		{
		// 			p_ch->Dev_Type = Dcdc;
		// 			return true;
		// 		}
		// 	}
		// 	else /*释放Check_EN信号，使Start信号生效*/
		// 	{
		// 		Set_Check(p_ch, GPIO_PIN_RESET);
		// 		/*检测dcdc设备期间，输出最低电压*/
		// 		p_ch->Charging_Voltage = 0;
		// 		// Set_Voltage(p_ch);
		// 	}
		// }
		// else
		// {
		// 	/*此处只考虑了5v设备*/
		// 	p_ch->Charging_Voltage = 5.2F;
		// 	// Set_Voltage(p_ch);
		// 	// shellPrint(&shell,"charging voltage :%f\r\n", p_ch->Charging_Voltage);
		// }
	}
	// Set_Voltage(p_ch);

	return false;
}

/**
 * @brief	计算出当前通道下电池的后台信息
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	电池区间码
 */
uint32_t Get_BackCharging_Parameters(Charging_Typedef *const p_ch)
{ /*经过容量换算后实际电流*/
	float actual_current;
	// Shell *pShell = &shell;

	/*通过电池容量设置充电电流*/
	if (p_ch->Bac_ChargingPara.Battery_Capacity < MIN_CAPACITY)
	{ /*默认可充电池容量>500mA*/
		p_ch->Bac_ChargingPara.Battery_Capacity = MIN_CAPACITY;
	}
	else if (p_ch->Bac_ChargingPara.Battery_Capacity > MAX_CAPACITY)
	{
		p_ch->Bac_ChargingPara.Battery_Capacity = MAX_CAPACITY;
	}

	actual_current = (float)p_ch->Bac_ChargingPara.Battery_Capacity / 1000.0F;
	/*获取当前电压所处的电池节数*/
	p_ch->Batterry_Numbers = Get_Battery_Range(p_ch->For_ChargingPara.Voltage) + 1U;
	// shellPrint(&shell, "Batterry Number: %d, voltage: %f .\r\n", p_ch->Batterry_Numbers, p_ch->For_ChargingPara.Voltage);

	/*涓流目标电压*/
	p_ch->Bac_ChargingPara.Trickle_Target_Voltage = ONCE_MINVOLTAGE * (float)p_ch->Batterry_Numbers;
	/*涓流目标电流*/
	p_ch->Bac_ChargingPara.Trickle_Target_Current = ONCE_MINCURRENT * actual_current;
	/*恒流目标电压*/
	p_ch->Bac_ChargingPara.ConstantCurrent_Target_Voltage = ONCE_FULLVOLTAGE * (float)p_ch->Batterry_Numbers;
	/*恒流目标电流:利用动态总功率算出恒流电流*/
	p_ch->Bac_ChargingPara.ConstantCurrent_Target_Current = ONCE_STDCURRENT * actual_current;
	/*恒压目标电压*/
	p_ch->Bac_ChargingPara.ConstantVoltage_Target_Voltage = ONCE_MAXVOLTAGE * (float)p_ch->Batterry_Numbers;
	/*恒压目标电流*/
	p_ch->Bac_ChargingPara.ConstantVoltage_Target_Current = ONCE_MINCURRENT * actual_current; // END_CURRNRT(actual_current);

	return (p_ch->Batterry_Numbers);
}


/**
 * @brief	充电阶段监测
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	None
 */
void ChargingStage_Monitor(Charging_Typedef *const p_ch)
{
	static uint8_t eoc_counter[CHANNEL_MAX] = {0, 0, 0};

	/*电池设备处于恒压模式，且电流小于一定值*/
	if ((p_ch->For_ChargingPara.Current <= p_ch->Bac_ChargingPara.ConstantVoltage_Target_Current) ||
	(p_ch->For_ChargingPara.Time >= MAX_CHARGING_TIMES))
	{
		if (++eoc_counter[p_ch->Channel_Id.Spi_Id] >= OPENTIME)
		{
			eoc_counter[p_ch->Channel_Id.Spi_Id] = 0U;

			if (((p_ch->Dev_Type == Cell) && (p_ch->CState == Constant_Voltage_Mode)) ||
			(p_ch->Dev_Type == Dcdc) || (p_ch->For_ChargingPara.Time >= MAX_CHARGING_TIMES))
			{
				/*关闭充电动画*/
				p_ch->Charger_Flag.Cartoon_Flag = false;
				/*认为充电结束*/
				p_ch->CState = End_Charging;
				/*关闭充电开关*/
				Close_StartSignal(p_ch);
			}
		}
	}
	/*由于电流消失而造成的断路*/
	if (Check_DisConnect(p_ch) == true)
	{
		if (((p_ch->Dev_Type == Cell) && (p_ch->For_ChargingPara.Voltage < LOWEST_VOLTAGE)) ||
		((p_ch->Dev_Type == Dcdc) && (p_ch->Charger_Flag.Insert_Flag))
		|| (p_ch->CState != End_Charging))
		{
			p_ch->Charger_Flag.Insert_Flag = false;
			p_ch->Charger_Flag.Is_ChargingFlag = false;
			p_ch->For_ChargingPara.Time = 0;
			p_ch->Dev_Type = No_Device;
		}
	}
	shellPrint(&shell, "CH:%d,Device:%d,V:%f.\r\n", p_ch->Channel_Id.Spi_Id, p_ch->Dev_Type, p_ch->For_ChargingPara.Voltage);
}

/**
 * @brief	检测充电设备是否断开连接
 * @details	轮询处理每个充电通道
 * @param	p_ch:当前通道
 * @retval	None
 */
bool Check_DisConnect(Charging_Typedef *const p_ch)
{
	static uint32_t counter[CHANNEL_MAX] = {0, 0, 0};

	if (p_ch->For_ChargingPara.Current < CHECK_CURRENT)
	{
		if (++counter[p_ch->Channel_Id.Spi_Id] > OPENTIME / 10U)
		{
			counter[p_ch->Channel_Id.Spi_Id] = 0U;
			return true;
		}
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
		temp_value = 0x01 << 8U;
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
	uint16_t temp_value = ((uint8_t)state) << 8U;
	/*把充电图标地址和状态对应起来*/
	Dwin_Write(addr, (uint8_t *)&temp_value, sizeof(temp_value));
}

#if (KALMAN == 1)
/**
 *卡尔曼滤波器
 *@param KFP *kfp 卡尔曼结构体参数
 *   float input 需要滤波的参数的测量值（即传感器的采集值）
 *@return 滤波后的参数（最优值）
 */
float kalmanFilter(KFP *kfp, float input)
{
	/*预测协方差方程：k时刻系统估算协方差 = k-1时刻的系统协方差 + 过程噪声协方差*/
	kfp->Now_Covariance = kfp->Last_Covariance + kfp->Q;
	/*卡尔曼增益方程：卡尔曼增益 = k时刻系统估算协方差 / （k时刻系统估算协方差 + 观测噪声协方差）*/
	kfp->Kg = kfp->Now_Covariance / (kfp->Now_Covariance + kfp->R);
	/*更新最优值方程：k时刻状态变量的最优值 = 状态变量的预测值 + 卡尔曼增益 * （测量值 - 状态变量的预测值）*/
	kfp->Output = kfp->Output + kfp->Kg * (input - kfp->Output); //因为这一次的预测值就是上一次的输出值
	/*更新协方差方程: 本次的系统协方差付给 kfp->Last_Covariance 为下一次运算准备。*/
	kfp->Last_Covariance = (1 - kfp->Kg) * kfp->Now_Covariance;
	/*当kfp->Output不等于0时，负方向迭代导致发散到无穷小*/
	if (kfp->Output < 0.01F)
	{
		kfp->Kg = 0;
		kfp->Output = 0;
	}
	return kfp->Output;
}
#else
/**
 *滑动滤波器
 *@param SideParm *side 滑动结构体参数
 *   float input 需要滤波的参数的测量值（即传感器的采集值）
 *@return 滤波后的参数（最优值）
 */
float sidefilter(SideParm *side, float input)
{
	//第一次滤波
	if (side->First_Flag)
	{

		for (int i = 0; i < sizeof(side->SideBuff) / sizeof(float); i++)
		{
			side->SideBuff[i] = input;
		}

		side->First_Flag = false;
		side->Head = &side->SideBuff[0];
		side->Sum = input * (sizeof(side->SideBuff) / sizeof(float));
	}
	else
	{
		side->Sum = side->Sum - *side->Head + input;
		*side->Head = input;

		if (++side->Head > &side->SideBuff[sizeof(side->SideBuff) / sizeof(float) - 1])
		{
			side->Head = &side->SideBuff[0];
		}

		input = side->Sum / (1 << FILTER_SHIFT);
	}

	return input;
}
#endif
