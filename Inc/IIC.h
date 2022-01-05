#ifndef __I2C_H
#define __I2C_H

#ifdef __cplusplus
extern "C" {
#endif
#include "config.h"

/*延时的个数，5即为延时5Us*/
#define IIC_DELAY 5U

#define I2C_Fail    0U
#define I2C_Succeed 1U

/*定义SCL输出的宏定义*/
sbit SCL_CHANNEL0 = P0^0;
sbit SCL_CHANNEL1 = P1^2;
sbit SCL_CHANNEL2 = P4^4;
/*定义SDA输出、读取的宏定义*/
sbit SDA_CHANNEL0 = P2^7;
sbit SDA_CHANNEL1 = P0^5;
sbit SDA_CHANNEL2 = P1^4;

#define SDA_READ_CHANNELX(CH) \
(CH == Channel0 ? SDA_CHANNEL0 : (CH == Channel1) ? SDA_CHANNEL1 : SDA_CHANNEL2)

/*IIC层封装*/
typedef enum 
{
    Channel0 = 0x00,
    Channel1,
    Channel2
}IIC_Channel;

typedef enum 
{
    Low  = 0x00,
    High = !Low,
}IIC_Pin_State;


extern void Delay_Us(uint16_t us);		//@11.0592MHz
extern void Delay_Ms(uint16_t ms);		//@11.0592MHz
void Delay_5Us(void);		//@27MHz


extern void I2C_Start(IIC_Channel channel); 
extern void I2C_Stop(IIC_Channel channel);
extern void I2C_SendByte(IIC_Channel channel, uint8_t sendByte);
extern uint8_t I2C_WaitAck(IIC_Channel channel, uint16_t timeout);
extern void I2C_SendAck(IIC_Channel channel);
extern void I2C_SendNAck(IIC_Channel channel);
extern uint8_t I2C_ReadByte(IIC_Channel channel); 
extern uint8_t I2C_Set_SCL(IIC_Channel channel, IIC_Pin_State sate); 
extern uint8_t I2C_Set_SDA(IIC_Channel channel, IIC_Pin_State sate); 

#ifdef __cplusplus
}
#endif

#endif /* __I2C_H */
