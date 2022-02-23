/*
 * MCP48xx.h
 *
 *  Created on: Jan 11, 2021
 *      Author: play
 */

#ifndef INC_MCP4822_H_
#define INC_MCP4822_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "charger.h"

#define MULTI_SLAVE			1

#define INPUT_A			    0x3000
#define INPUT_B				0xB000
#define SPI_TIMEOUT         0xFFFF

#define MCP_SCK_PORT  SPI_SCK_GPIO_Port
#define MCP_SCK_PIN	  SPI_SCK_Pin
#define MCP_SDI_PORT  SPI_MOSI_GPIO_Port
#define MCP_SDI_PIN	  SPI_MOSI_Pin

#if (MULTI_SLAVE)

#define MCP_CS_PORT	  SPI_CS1_GPIO_Port
#define MCP_CS_PIN    SPI_CS1_Pin

#define MCP_CS2_PORT  SPI_CS2_GPIO_Port
#define MCP_CS2_PIN	  SPI_CS2_Pin

#define CS_1   1
#define CS_2   2

#else

#define MCP_CS_PORT	  SPI_CS1_GPIO_Port
#define MCP_CS_PIN    SPI_CS1_Pin

#define CS_1   1

#endif

extern void Mcp48xx_Write(uint16_t data, Charger_Channel *p_ch);
extern void Output_Ref(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_MCP4822_H_ */
