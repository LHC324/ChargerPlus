#include "clibration.h"
// #include "charger.h"
#include "usart.h"
// #include "adc.h"
#include "Mcp4822.h"
#include "shell.h"
#include "shell_port.h"
#include "FreeRTOS.h"
#include "cmsis_os.h"


ADC_Calibration_HandleTypeDef Adc = {0};
DAC_Calibration_HandleTypeDef Dac = {0};

static const char *clibrationText[] =
	{
		[CLIBRATION_OK] = "Success: Calibration successful !\r\n",
		[PARSING_SUCCEEDED] = "Success: Parsing succeeded !\r\n",
		[USER_CANCEL] = "Note: User cancel !\r\n",
		[DATA_TOO_LONG] = "Warning: data is too long !\r\n",
		[INPUT_ILLEGAL] = "Warning: Illegal input !\r\n",
		// [RE_ENTER] = "\r\nNote: Please re-enter !\r\n"
};

/**
 * @brief	对终端输入的浮点数进行解析
 * @details
 * @param	None
 * @retval	None
 */
Clibration_Error Input_FloatPaser(Shell *shell, float *pFloat)
{
	char recive_data = '\0';
	uint8_t arry[BYTE_SIZE] = {0};
	uint16_t len = 0;
	uint16_t site = 0;

	while (1)
	{
		// recive_data = '\0';
		// if (HAL_UART_Receive(&RECEIVE_TARGET_UART, (uint8_t *)recive_data, 0x01, 0xFFFF) == HAL_OK)
		if (shell->read(&recive_data, 0x01))
		{
			switch (recive_data)
			{
			case ESC_CODE:
			{
				// shellWriteString(shell, "User Cancel !\r\n");
				return USER_CANCEL;
			}
			case BACKSPACE_CODE:
			{
				if (len)
				{
					len--;
				}
				shellDeleteCommandLine(shell, 0x01);
				break;
			}
			case ENTER_CODE:
			{ /*首次就输入了小数点*/
				if (!site)
				{
					return INPUT_ILLEGAL;
				}

				// SHELL_LOCK(shell);
				// shellPrint(shell, "len:%d,data:%S\r\n", len, arry);
				// SHELL_UNLOCK(shell);

				for (uint16_t i = 0; i < len; i++)
				{
					if (i < site)
					{
						*pFloat += ((float)arry[i]) * powf(10.0F, site - (i + 1U));
					}
					else
					{
						*pFloat += ((float)arry[i]) * powf(10.0F, -1.0F * (float)(i - site + 1U));
					}
					// Usart1_Printf("pFloat:%f\r\n", *pFloat);
					// break;
				}
				SHELL_LOCK(shell);
				shellPrint(shell, "\r\npFloat:%f\r\n", *pFloat);
				SHELL_UNLOCK(shell);
				return PARSING_SUCCEEDED;
			}
			case SPOT_CODE: /*跳过小数点*/
			{				/*记录小数点位置*/
				site = len;
				// SHELL_LOCK(shell);
				// shellPrint(shell, "site:%d\r\n", site);
				// SHELL_UNLOCK(shell);
				break;
			}
			default:
			{
				if ((recive_data >= '0') && (recive_data <= '9'))
				{
					arry[len] = recive_data - '0';
					// SHELL_LOCK(shell);
					// shellPrint(shell, "recive data[%d]:%d\r\n", len, arry[len]);
					// SHELL_UNLOCK(shell);
					if (++len > BYTE_SIZE)
					{
						return DATA_TOO_LONG;
					}
				}
				else
				{
					return INPUT_ILLEGAL;
				}
				// Usart1_Printf("recive data[%d]:%d\r\n", recive_data);
				break;
			}
			}

			if (recive_data != BACKSPACE_CODE)
			{
				/*写回到控制台*/
				shell->write(&recive_data, 0x01);
			}
		}
	}
}

/**
 * @brief	ADC与采集电压间自动校准
 * @details	通过lettershell自动映射
 * @param	None
 * @retval	None
 */
bool Adc_Clibration(void)
{
	Shell *pShell = &shell;
	uint16_t temp_data = 0;
	float p_sum = 0;
	float q_sum = 0;
	Clibration_Error error;
	float float_value = 0;

	/*校准前挂起所有无关任务,只保留运行指示灯*/
	// osThreadSuspend(LedHandle);
	// osThreadSuspend(ChargingHandle);
	// osThreadSuspend(SamplingHandle);
	// /*停止上报定时器*/
	// osTimerStop(ReportHandle);
	osThreadSuspendAll();

	for (uint16_t i = 0; i < CHANNEL_MAX; i++)
	{
		/*校准前打开充电开关*/
		Open_StartSignal(&g_Charger[i]);
		for (uint16_t j = 0; j < ADC_DMA_CHANNEL / 3U; j++)
		{
			for (uint16_t k = 0; k < ADC_POINTS; k++)
			{
				if (k)
				{
					temp_data = 512U;
					temp_data = temp_data * k - 1U;
				}
				else
				{
					temp_data = 0;
					p_sum = q_sum = 0;
				}
				Mcp48xx_Write(temp_data, &g_Charger[i].Channel_Id);
				Adc.DAC_Out[k] = temp_data;
				HAL_Delay(500);
				Adc.Xx[i][j][k] = Get_AdcValue(i * 3U + j);
				SHELL_LOCK(pShell);
				shellPrint(pShell, "\r\nADC_CH[%d]S[%d]P[%d]:%d,Dac:%d\r\n", i, j, k, Adc.Xx[i][j][k], temp_data);
				// Usart1_Printf("data[%d]:%d,ADC_CH[%d]:%d\r\n", j, temp_data, i, Get_AdcValue(j));
				SHELL_UNLOCK(pShell);
			start:
				error = Input_FloatPaser(pShell, &float_value);
				shellWriteString(pShell, clibrationText[error]);
				if ((error == DATA_TOO_LONG) || (error == INPUT_ILLEGAL))
				{
					goto start;
				} /*用户取消，校准失败*/
				else if (error == USER_CANCEL)
				{
					
					return false;
				}
				Adc.Yx[i][j][k] = float_value;
				/*清除前一次值*/
				float_value = 0;
				/*当检测到第二个点时，可以算出第一段系数*/
				if (k)
				{
					Adc.Px[i][j][k - 1U] = (Adc.Yx[i][j][k] - Adc.Yx[i][j][k - 1U]) / (float)(Adc.Xx[i][j][k] - Adc.Xx[i][j][k - 1U]);
					p_sum += Adc.Px[i][j][k - 1U];
					Adc.Qx[i][j][k - 1U] = Adc.Yx[i][j][k - 1U] - Adc.Px[i][j][k - 1U] * (float)Adc.Xx[i][j][k - 1U];
					q_sum += Adc.Qx[i][j][k - 1U];
					SHELL_LOCK(pShell);
					shellPrint(pShell, "Px[%d][%d][%d]:%f, Qx[%d][%d][%d]:%f\r\n", i, j, k - 1U, Adc.Px[i][j][k - 1U], i, j, k - 1U, Adc.Qx[i][j][k - 1U]);
					SHELL_UNLOCK(pShell);
					// Usart1_Printf("Px[%d][%d][%d]:%f, Qx[%d][%d][%d]:%f\r\n", i, j, k - 1U, Adc.Px[i][j][k - 1U], i, j, k - 1U, Adc.Qx[i][j][k - 1U]);
				}
			}
			/*对ADC_POINTS求取平均值*/
			Adc.SPx[i][j] = p_sum / (ADC_POINTS - 1U);
			Adc.SQx[i][j] = q_sum / (ADC_POINTS - 1U);

			SHELL_LOCK(pShell);
			shellPrint(pShell, "SPx[%d][%d]:%f,SQx[%d][%d]:%f\r\n", i, j, Adc.SPx[i][j], i, j, Adc.SQx[i][j]);
			SHELL_UNLOCK(pShell);
			// Usart1_Printf("SPx[%d][%d]:%f,SQx[%d][%d]:%f\r\n", i, j, Adc.SPx[i][j], i, j, Adc.SQx[i][j]);
		}
		/*校准完一个通道后输出最低电压*/
		Mcp48xx_Write(0, &g_Charger[i].Channel_Id);
		/*校准完成后关闭充电开关*/
		Close_StartSignal(&g_Charger[i]);
	}
#if (USING_CRC)
	Adc.Crc16 = Get_Crc16((uint8_t *)&Adc.DAC_Out, sizeof(Adc) - sizeof(Adc.Yx) * 4U - sizeof(float), 0xFFFF);
#else
	/*flash确认标志*/
	Adc.Finish_Flag = SURE_CODE;
#endif
	/*校准完成后写入校准参数到flah*/
	FLASH_Write(ADC_CLIBRATION_SAVE_ADDR, (uint16_t *)&Adc.DAC_Out, sizeof(Adc) - sizeof(Adc.Yx) * 4U);
	/*校准完成后恢复任务和定时器*/
	// osThreadResume(LedHandle);
	// osThreadResume(ChargingHandle);
	// osThreadResume(SamplingHandle);
	// osTimerStart(ReportHandle, 1000);
	osThreadResumeAll();

	return true;
}
#if (DEBUGGING == 1U)
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), adc_clibration, Adc_Clibration, clibration);
#endif

/**
 * @brief	DAC与输出电压间自动校准
 * @details	通过lettershell自动映射
 * @param	None
 * @retval	None
 */
bool Dac_Clibration(void)
{
	float k_sum = 0;
	float g_sum = 0;

	/*校准前挂起所有无关任务,只保留运行指示灯*/
	// osThreadSuspend(LedHandle);
	// osThreadSuspend(ChargingHandle);
	// osThreadSuspend(SamplingHandle);
	// /*停止上报定时器*/
	// osTimerStop(ReportHandle);
	osThreadSuspendAll();

	if (!Adc.DAC_Out[1U])
	{	
		// Usart1_Printf("ADC not calibrated !\r\n");
		shellPrint(&shell, "ADC not calibrated !\r\n");
		return false;
	}
	/*仅仅校正对Vbat端输出*/
	for (uint16_t i = 0; i < CHANNEL_MAX; i++)
	{ /*清除上一次累加和*/
		k_sum = g_sum = 0;
		for (uint16_t k = 0; k < ADC_POINTS - 1U; k++)
		{
			Dac.Kx[i][k] = (float)(Adc.DAC_Out[k + 1U] - Adc.DAC_Out[k]) / (Adc.Yx[i][1U][k + 1U] - Adc.Yx[i][1U][k]);
			k_sum += Dac.Kx[i][k];
			Dac.Gx[i][k] = (float)Adc.DAC_Out[k] - Dac.Kx[i][k] * Adc.Yx[i][1U][k];
			g_sum += Dac.Gx[i][k];
			// Usart1_Printf("Px[%d][%d]:%f, Qx[%d][%d]:%f\r\n", i, k, Dac.Kx[i][k], i, k, Dac.Gx[i][k]);
			shellPrint(&shell, "Px[%d][%d]:%f, Qx[%d][%d]:%f\r\n", i, k, Dac.Kx[i][k], i, k, Dac.Gx[i][k]);
		}
		/*对DAC_POINTS求取平均值*/
		Dac.SKx[i] = k_sum / (ADC_POINTS - 1U);
		Dac.SGx[i] = g_sum / (ADC_POINTS - 1U);
		// Usart1_Printf("SKx[%d]:%f,SGx[%d]:%f\r\n", i, Dac.SKx[i], i, Dac.SGx[i]);
		shellPrint(&shell, "SKx[%d]:%f,SGx[%d]:%f\r\n", i, Dac.SKx[i], i, Dac.SGx[i]);
	}
#if (USING_CRC)
	Dac.Crc16 = Get_Crc16((uint8_t *)&Dac, sizeof(Dac) - sizeof(float), 0xFFFF);
#else
	/*flash确认标志*/
	Dac.Finish_Flag = SURE_CODE;
#endif
	/*校准完成后写入校准参数到flah*/
	FLASH_Write(DAC_CLIBRATION_SAVE_ADDR, (uint16_t *)&Dac, sizeof(Dac));
	/*校准完成后恢复任务和定时器*/
	// osThreadResume(LedHandle);
	// osThreadResume(ChargingHandle);
	// osThreadResume(SamplingHandle);
	// osTimerStart(ReportHandle, 1000);
	osThreadResumeAll();

	return true;
}

#if (DEBUGGING == 1U)
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), dac_clibration, Dac_Clibration, clibration);
#endif

/**
 * @brief	查看ADC校准系数
 * @details	通过lettershell自动映射
 * @param	None
 * @retval	None
 */
void See_ADC_Param(void)
{
	shellPrint(&shell, "\tSPx\t\tSQx\r\n");
	for (uint16_t i = 0; i < CHANNEL_MAX; i++)
	{
		for (uint16_t j = 0; j < ADC_DMA_CHANNEL / 3U; j++)
		{
			shellPrint(&shell, "CH[%d][%d]:%f\t\t%f\r\n", i, j, Adc.SPx[i][j], Adc.SQx[i][j]);
		}
		// shellPrint(&shell,"\r\n");
	}
}
#if (DEBUGGING == 1U)
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), see_adc, See_ADC_Param, see);
#endif

/**
 * @brief	查看DAC校准系数
 * @details	通过lettershell自动映射
 * @param	None
 * @retval	None
 */
void See_DAC_Param(void)
{
	shellPrint(&shell, "\tSKx\t\tSGx\r\n");
	for (uint16_t i = 0; i < CHANNEL_MAX; i++)
	{
		shellPrint(&shell, "CH[%d]:%f\t\t%f\r\n", i, Dac.SKx[i], Dac.SGx[i]);
	}
}
#if (DEBUGGING == 1U)
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), see_dac, See_DAC_Param, see);
#endif

/**
 * @brief	检查DAC、ADC校准后精度是否满足
 * @details	通过lettershell自动映射
 * @param	None
 * @retval	None
 */
void Check_ADC(uint8_t ch, uint8_t id)
{
	Shell *pShell = &shell;
	Charging_Typedef *p_ch;
	S_Type type;
	Clibration_Error error;
	float value;
	float temp_current = 0;

	/*挂起充电调节任务*/
	// osThreadSuspend(ChargingHandle);
	osThreadSuspendAll();
	shellPrint(pShell, "ch: %d, Sid: %d.\r\n", ch, id);
	if (ch >= CHANNEL_MAX)
	{
		shellPrint(pShell, "Error: Target channel does not exist !\r\n");
		return;
	}
	if (id >= (ADC_DMA_CHANNEL / 3U))
	{
		shellPrint(pShell, "Error: Sampling channel does not exist !\r\n");
		return;
	}

	p_ch = ch ? ((ch == 1U) ? (&g_Charger[1]) : (&g_Charger[2])) : (&g_Charger[0]);
	type = id ? ((id == 1U) ? VBAT : IBAT) : (VBUS);
	shellPrint(pShell, "Type:%d\r\n", type);
start:
	if (type == IBAT)
	{
		shellPrint(pShell, "Please enter the target current value .\r\n");
	}
	else
	{
		shellPrint(pShell, "Please enter the target voltage value .\r\n");
	}
	
	error = Input_FloatPaser(pShell, &value);
	shellWriteString(pShell, clibrationText[error]);
	if ((error == DATA_TOO_LONG) || (error == INPUT_ILLEGAL))
	{
		goto start;
	} /*用户取消，校准失败*/
	else if (error == USER_CANCEL)
	{
		return;
	}

	/*校准前打开充电开关*/
	Open_StartSignal(&g_Charger[ch]);
	if (type == IBAT)
	{ /*电流过大，重新输入*/
		if (value > IBAT_MAX_CURRENT)
		{
			shellPrint(pShell, "Error: Overcurrent, please re-enter !\r\n");
			value = 0;
			goto start;
		}
		/*目标电流*/
		// p_ch->For_ChargingPara.Current = value;
		/*采集当前Vbat电压为起始条件*/
		p_ch->Charging_Voltage = Get_Vbus_Vbat(p_ch, VBAT);
		while (1)
		{
			temp_current = Get_Ibat(p_ch);
			p_ch->For_ChargingPara.Current = temp_current;
			HAL_Delay(100);
			Get_Voltage_MicroCompensate(p_ch, value);
			Set_Voltage(p_ch);
			shellPrint(pShell, "Charging_Voltage:%f\r\n", p_ch->Charging_Voltage);
			/*采集电流在允许误差内*/
			if ((temp_current >= value - PERMIT_ERROR) &&
				(temp_current <= value + PERMIT_ERROR))
			{
				break;
			}
		}
		// shellPrint(pShell, "Charging_Voltage:%f\r\n", p_ch->Charging_Voltage);
		shellPrint(pShell, "ICH[%d][%d]:%f\r\n", ch, id, temp_current);
	}
	else
	{
		/*设置输出电压*/
		p_ch->Charging_Voltage = value;
		Set_Voltage(p_ch);
		HAL_Delay(500);
		shellPrint(pShell, "VCH[%d][%d]:%f\r\n", ch, id, Get_Vbus_Vbat(p_ch, type));
	}

	shellPrint(pShell, "Please input the confirmation signal .\r\n");

	char recive_data = '\0';
	/*等接收到确认信号*/
	while(1)
	{
		pShell->read(&recive_data, 0x01);
		if (recive_data == ENTER_CODE)
		{
			break;
		}
	}
	/*校准完成后关闭充电开关*/
	Close_StartSignal(&g_Charger[ch]);
	/*恢复充电调节任务*/
	// osThreadResume(ChargingHandle);
	osThreadResumeAll();
}
#if (DEBUGGING == 1U)
SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), check_adc, Check_ADC, check);
#endif
