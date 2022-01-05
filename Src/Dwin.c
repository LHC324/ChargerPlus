/*
 * Dwin.c
 *
 *  Created on: 2022年1月4日
 *      Author: play
 */

#include "Dwin.h"
#include "usart.h"

Dwin_T g_Dwin;

/*以下代码9级优化，速度优先*/
#pragma OPTIMIZE(9, speed)

/**
 * @brief  带CRC的发送数据帧
 * @param  _pBuf 数据缓冲区指针
 * @param  _ucLen 数据长度
 * @retval None
 */
void Dwin_SendWithCRC(uint8_t *_pBuf, uint16_t _ucLen)
{
	uint16_t crc = 0;
	uint8_t buf[256U] = {0};

	memcpy(buf, _pBuf, _ucLen);
	/*The first three bytes do not participate in verification*/
	crc = Get_Crc16(&_pBuf[3U], _ucLen - 3U, 0xffff);
	buf[_ucLen++] = crc;
	buf[_ucLen++] = crc >> 8U;
	
	Uartx_SendStr(&Uart1, buf, _ucLen, UART_BYTE_SENDOVERTIME);
}

/**
 * @brief  发送数据帧(不带CRC)
 * @param  _pBuf 数据缓冲区指针
 * @param  _ucLen 数据长度
 * @retval None
 */
void Dwin_Send(uint8_t *_pBuf, uint16_t _ucLen)
{
	Uartx_SendStr(&Uart1, _pBuf, _ucLen, UART_BYTE_SENDOVERTIME);
}


/**
 * @brief  写数据变量到指定地址并显示
 * @param  start_addr 开始地址
 * @param  dat 指向数据的指针
 * @param  length 数据长度
 * @retval None
 */
void Dwin_Write(uint16_t start_addr, uint8_t *dat, uint16_t length)
{
	uint8_t i = 0;
	g_Dwin.TxCount = 0;
	g_Dwin.TxBuf[g_Dwin.TxCount++] = 0x5A;
	g_Dwin.TxBuf[g_Dwin.TxCount++] = 0xA5;
#if (USING_CRC)
	/*Add two bytes CRC*/
	g_Dwin.TxBuf[g_Dwin.TxCount++] = length + 3U + 2U;
#else
	g_Dwin.TxBuf[g_Dwin.TxCount++] = length + 3U;
#endif
	g_Dwin.TxBuf[g_Dwin.TxCount++] = WRITE_CMD;
	g_Dwin.TxBuf[g_Dwin.TxCount++] = start_addr >> 8U;
	g_Dwin.TxBuf[g_Dwin.TxCount++] = start_addr;

	for (i = 0; i < length; i++)
	{
		g_Dwin.TxBuf[g_Dwin.TxCount++] = dat[i];
	}
#if (USING_CRC)
	Dwin_SendWithCRC(g_Dwin.TxBuf, g_Dwin.TxCount);
#else
	Dwin_Send(g_Dwin.TxBuf, g_Dwin.TxCount);
#endif
}

/**
 * @brief  读出指定地址指定长度数据
 * @param  start_addr 开始地址
 * @param  dat 指向数据的指针
 * @param  length 数据长度
 * @retval None
 */
void Dwin_Read(uint16_t start_addr, uint16_t words)
{
	g_Dwin.TxCount = 0;
	g_Dwin.TxBuf[g_Dwin.TxCount++] = 0x5A;
	g_Dwin.TxBuf[g_Dwin.TxCount++] = 0xA5;
#if (USING_CRC)
	/*Add two bytes CRC*/
	g_Dwin.TxBuf[g_Dwin.TxCount++] = 0x04 + 2U;
#else
	g_Dwin.TxBuf[g_Dwin.TxCount++] = 0x04;
#endif
	g_Dwin.TxBuf[g_Dwin.TxCount++] = READ_CMD;
	g_Dwin.TxBuf[g_Dwin.TxCount++] = start_addr >> 8;
	g_Dwin.TxBuf[g_Dwin.TxCount++] = start_addr;
	g_Dwin.TxBuf[g_Dwin.TxCount++] = words;

#if (USING_CRC)
	Dwin_SendWithCRC(g_Dwin.TxBuf, g_Dwin.TxCount);
#else
	Dwin_Send(g_Dwin.TxBuf, g_Dwin.TxCount);
#endif
}

/**
 * @brief  取得16bitCRC校验码
 * @param  ptr   当前数据串指针
 * @param  length  数据长度
 * @param  init_dat 校验所用的初始数据
 * @retval 16bit校验码
 */
uint16_t Get_Crc16(uint8_t *ptr, uint16_t length, uint16_t init_dat)
{
	uint16_t i = 0;
	uint16_t j = 0;
	uint16_t crc16 = init_dat;

	for (i = 0; i < length; i++)
	{
		crc16 ^= *ptr++;

		for (j = 0; j < 8; j++)
		{
			if (crc16 & 0x0001)
			{
				crc16 = (crc16 >> 1) ^ 0xa001;
			}
			else
			{
				crc16 = crc16 >> 1;
			}
		}
	}
	return (crc16);
}


