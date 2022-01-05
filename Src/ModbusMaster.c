/*
 * ModbusMaster.c
 *
 *  Created on: 2022年1月4日
 *      Author: play
 */
#include "ModbusMaster.h"
#include "usart.h"
#include "Dwin.h"

MODS_T g_tModS;

static void MODS_SendWithCRC(uint8_t *_pBuf, uint8_t _ucLen);

/**
 * @brief  有人云自定义46指令
 * @param  slaveaddr 从站地址
 * @param  regaddr 寄存器开始地址
 * @param  reglength 寄存器长度
 * @param  dat 数据
 * @retval None
 */
void MOD_46H(uint8_t slaveaddr, uint16_t regaddr, uint16_t reglength, uint8_t datalength, uint8_t* dat)
{
    uint8_t i;

    g_tModS.TxCount = 0;
    g_tModS.TxBuf[g_tModS.TxCount++] = slaveaddr;
    g_tModS.TxBuf[g_tModS.TxCount++] = 0x46;
    g_tModS.TxBuf[g_tModS.TxCount++] = regaddr >> 8;
    g_tModS.TxBuf[g_tModS.TxCount++] = regaddr;
    g_tModS.TxBuf[g_tModS.TxCount++] = reglength >> 8;
    g_tModS.TxBuf[g_tModS.TxCount++] = reglength;
    g_tModS.TxBuf[g_tModS.TxCount++] = datalength;

    for(i = 0; i < datalength; i++)
    {
        g_tModS.TxBuf[g_tModS.TxCount++] = dat[i];
    }

    MODS_SendWithCRC(g_tModS.TxBuf, g_tModS.TxCount);
}

/**
 * @brief  带CRC的发送从站数据
 * @param  _pBuf 数据缓冲区指针
 * @param  _ucLen 数据长度
 * @retval None
 */
void MODS_SendWithCRC(uint8_t *_pBuf, uint8_t _ucLen)
{
    uint16_t crc;
    uint8_t buf[MOD_TX_BUF_SIZE];

    memcpy(buf, _pBuf, _ucLen);
    crc = Get_Crc16(_pBuf, _ucLen, 0xffff);
    buf[_ucLen++] = crc;
    buf[_ucLen++] = crc >> 8;
    
    Uartx_SendStr(&Uart2, buf, _ucLen, UART_BYTE_SENDOVERTIME);
}
