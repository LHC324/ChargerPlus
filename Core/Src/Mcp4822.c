/*
 * MCP48xx.c
 *
 *  Created on: Jan 11, 2021
 *      Author: LHC
 */

#include "Mcp4822.h"
#include "spi.h"
#include "charger.h"
#include "usart.h"


/**
 * @brief	对MCP48xx写数据
 * @details	
 * @param	data:写入的数据
 * @param   p_ch :当前充电通道指针
 * @retval	None
 */
void Mcp48xx_Write(uint16_t data, Charger_Channel *p_ch)
{
	SPI_HandleTypeDef Spi_Handle;
	GPIO_TypeDef *pGPIOx;
	uint16_t GPIO_Pinx;

	data &= 0x0FFF;
	/*选择mcp4822通道*/
	data |= (p_ch->Mcpxx_Id ? INPUT_B : INPUT_A);
	/*选择硬件spi句柄*/
	Spi_Handle = ((p_ch->Spi_Id == Channel2) ? hspi2 : hspi1);
	// pGPIOx = CS1_GPIO_Port;
	switch(p_ch->Spi_Id)
	{
		case Channel0: 
		case Channel1: pGPIOx = CS1_GPIO_Port; GPIO_Pinx = CS1_Pin; break;
		case Channel2: pGPIOx = CS2_GPIO_Port; GPIO_Pinx = CS2_Pin; break;
	}
#if (DEBUGGING == 1U)
	// Usart1_Printf("cs1 = %d, pGPIOx = %d\r\n", CS1_GPIO_Port, pGPIOx);
#endif
	/*软件拉低CS信号*/
	HAL_GPIO_WritePin(pGPIOx, GPIO_Pinx, GPIO_PIN_RESET);
	/*调用硬件spi发送函数*/
	HAL_SPI_Transmit(&Spi_Handle, (uint8_t *)&data, sizeof(data), SPI_TIMEOUT);
	/*软件拉高CS信号*/
	HAL_GPIO_WritePin(pGPIOx, GPIO_Pinx, GPIO_PIN_SET);
}

/**
 * @brief	输出比较器参考电压Ref：1-2v
 * @details	
 * @param	data:写入的数据
 * @param   p_ch :当前充电通道指针
 * @retval	None
 */
void Output_Ref(void)
{	/*1.5V*/
	uint16_t data = 2000U;//2559U2857U
	data &= 0x0FFF;
	data |= INPUT_B;

	/*Set channel B of SPI2 to output a 1.5V voltage*/
	HAL_GPIO_WritePin(CS2_GPIO_Port, CS2_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi2, (uint8_t *)&data, sizeof(data), SPI_TIMEOUT);
	HAL_GPIO_WritePin(CS2_GPIO_Port, CS2_Pin, GPIO_PIN_SET);
}


