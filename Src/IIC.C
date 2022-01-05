 #include "IIC.h"

/*以下代码9级优化，速度优先*/
// #pragma OPTIMIZE(9, speed)

 /**
 * @name	void I2C_Start(void)
 * @brief	发送I2C起始信号
 * @details	当SCL为高时，SDA由高拉低，就会形成起始信号
 * @param	channel:通道号
 * @retval	None
 */
 void I2C_Start(IIC_Channel channel) 
 {	/*SDA拉高*/
	I2C_Set_SDA(channel, High);											
	/*SCL拉高	*/									
	I2C_Set_SCL(channel, High);
	/*延时*/
 	Delay_5Us();				
	/*SDA拉低*/
 	I2C_Set_SDA(channel, Low);
	/*延时*/											
 	Delay_5Us();				
	/*SCL拉低，钳住*/
 	I2C_Set_SCL(channel, Low);										
 }

 /**
 * @brief	发送I2C结束信号
 * @details	当SCL为高时，SDA由低拉高，就会形成结束信号
 * @param	channel:通道号
 * @retval	None
 */
 void I2C_Stop(IIC_Channel channel) 
 {
	/*SDA拉低*/
 	I2C_Set_SDA(channel, Low); 			
 	/*SCL拉高	*/									
	I2C_Set_SCL(channel, High);
	/*延时*/
 	Delay_Us(IIC_DELAY);				
 	/*SDA拉高*/
	I2C_Set_SDA(channel, High);	
 }
 /**
 * @brief	通过I2C发送（写）数据
 * @details	当SCL为高时，保持SDA（高为1，低位0）稳定，即可传输一位数据
 * @param	channel:通道号，byte：想要发送的数据（1字节，8位）
 * @retval	None
 */
 void I2C_SendByte(IIC_Channel channel, uint8_t sendByte) 
 {
 	uint8_t i = 0;
	 /*SCL拉低，钳住*/
 	I2C_Set_SCL(channel, Low);
	/*延时*/
 	Delay_5Us();

 	for(i = 0; i < sizeof(uint8_t) * 8U; i++)
 	{	/*如果高位为1则拉高，否则拉低*/
 		if(sendByte & 0x80) 
		{					
 			/*SDA拉高*/
			I2C_Set_SDA(channel, High);
 		}
 		else 
		{
 			/*SDA拉低*/
 			I2C_Set_SDA(channel, Low);
 		}
		/*数据左移1位*/
 		sendByte <<= 1U;
		/*延时*/							
 		Delay_5Us();			
 		/*SCL拉高*/									
		I2C_Set_SCL(channel, High);
		/*延时*/
 		Delay_5Us();			
 		/*SCL拉低，钳住*/
 		I2C_Set_SCL(channel, Low);
 	}
 }
 /**
 * @brief	等待从机应答信号
 * @details	主机将SCL和SDA都拉高之后，释放SDA，然后读取SDA，为低则表示接收到从机的应答信号
 * @param	channel:通道号，timeout: 在规定时间内等待应答信号
 * @retval	I2C_WaitAck_Succeed：	成功等到应答信号
 * @retval	I2C_WaitAck_Fail：		在规定时间内未等到应答信号
 */
 uint8_t I2C_WaitAck(IIC_Channel channel, uint16_t timeout) 
 {
 	/*SDA拉高*/
	I2C_Set_SDA(channel, High);
	/*延时*/
 	Delay_5Us();				
 	/*SCL拉高*/									
	I2C_Set_SCL(channel, High);
	/*延时*/
 	Delay_5Us();				

 	/*开始循环查询SDA线上是否有应答信号*/
 	while(SDA_READ_CHANNELX(channel))
 	{	/*如果为1，则说明未产生应答*/
 		if (!(--timeout)) {
 			/*如果时间超时*/
 			/*如果为非主机测量，则此处不能Stop*/
			/*发出终止信号*/
 			// I2C_Stop(channel);
			/*返回失败信息*/							
 			return I2C_Fail;					
 		}
 		/*如果时间未超时*/
 		Delay_Us(1);						
 	}
 	//如果收到了应答信号，将SCL拉低
 	/*SCL拉低，钳住*/
 	I2C_Set_SCL(channel, Low);
	 /*返回成功信息*/
 	return I2C_Succeed;							
 }

 /**
 * @brief	向从机发送应答信号
 * @details	在SDA持续为低时，SCL产生一个正脉冲即表示产生一个应答信号
 * @param	channel:通道号
 * @retval	None
 */
 void I2C_SendAck(IIC_Channel channel) 
 {
	/*SDA拉低:CPU驱动SDA = 0*/
 	I2C_Set_SDA(channel, Low);
	/*延时*/
 	Delay_Us(IIC_DELAY);				
 	/*SCL拉高*/									
	I2C_Set_SCL(channel, High);	/* CPU产生1个时钟 */
	/*延时*/
 	Delay_Us(IIC_DELAY);				
 	/*SCL拉低，钳住*/
 	I2C_Set_SCL(channel, Low);
	/*延时*/
 	Delay_Us(IIC_DELAY); 
	/*SDA拉高：CPU释放SDA总线*/
 	I2C_Set_SDA(channel, High);
 }
 /**
 * @brief	向从机发送非应答信号
 * @details	在SDA持续为高时，SCL产生一个正脉冲即表示产生一个非应答信号
 * @param	channel:通道号
 * @retval	None
 */
 void I2C_SendNAck(IIC_Channel channel) 
 {
 	/*SDA拉高*/
	I2C_Set_SDA(channel, High);
 	/*SCL拉低，钳住*/
 	I2C_Set_SCL(channel, Low);
	/*延时*/
 	Delay_5Us();				
 	/*SCL拉高*/									
	I2C_Set_SCL(channel, High);
	/*延时*/
 	Delay_5Us();				
 	/*SCL拉低，钳住*/
 	I2C_Set_SCL(channel, Low);
 }

 /**
 * @brief	读取SDA线上的数据
 * @details	将SDA拉高后，不断产生正脉冲后读取SDA的高低，即为从机发送的数据
 * @param	channel:通道号
 * @retval	readByte：读到的一字节数据
 */
 uint8_t I2C_ReadByte(IIC_Channel channel) 
 {
 	uint8_t readByte = 0;
	uint8_t i = 0;
 	/*SDA拉高*/
	I2C_Set_SDA(channel, High);

 	for(i = 0; i < sizeof(uint8_t) * 8U; i++) {
 		// /*SCL拉低，钳住*/
 		// I2C_Set_SCL(channel, Low);
 		readByte <<= 1U;	
		/*SCL拉高*/									
		I2C_Set_SCL(channel, High);	
		/*延时*/
 		Delay_5Us();
 		if(SDA_READ_CHANNELX(channel)) 
		{
 			readByte |= 1U;
 		}
		/*SCL拉低，钳住*/
 		I2C_Set_SCL(channel, Low);
		/*延时*/
 		Delay_5Us();			
 	}
 	return readByte;
 }

/**
 * @brief	设置SCL线上的电平状态
 * @details	Low/High
 * @param	channel:通道号，sate：状态
 * @retval	设置后状态
 */
 static uint8_t I2C_Set_SCL(IIC_Channel channel, IIC_Pin_State sate) 
 {
	 uint8_t ret = I2C_Succeed;

	 switch(channel)
	 {
		case Channel0: SCL_CHANNEL0 = sate;
			 break;
		case Channel1: SCL_CHANNEL1 = sate;
			 break;
		case Channel2: SCL_CHANNEL2 = sate;
			 break;
		default : ret = I2C_Fail; break;
	 }
 	return ret;
 }

/**
 * @brief	设置SDA线上的电平状态
 * @details	Low/High
 * @param	channel:通道号，sate：状态
 * @retval	设置后状态
 */
  static uint8_t I2C_Set_SDA(IIC_Channel channel, IIC_Pin_State sate) 
 {
	 uint8_t ret = I2C_Succeed;

	 switch(channel)
	 {
		case Channel0: SDA_CHANNEL0 = sate;
			 break;
		case Channel1: SDA_CHANNEL1 = sate;
			 break;
		case Channel2: SDA_CHANNEL2 = sate;
			 break;
		default : ret = I2C_Fail; break;
	 }
 	return ret;
 }

/*禁止编译器优化该模块*/
#pragma OPTIMIZE(0)
 /**
 * @brief	us级别的延时
 * @details	IIC的SDA、SCL引脚切换需要一定延时
 * @param	us，需要延长的us数
 * @retval	None
 */
 void Delay_Us(uint16_t us)		//@11.0592MHz
{
	do
	{
		_nop_();
		_nop_();
		_nop_();
	}while(--us);
}


 /**
 * @brief	@27MHz下5us延时
 * @details	该延时函数的精度将会受到编译器优化模式和时钟频率的影响
 * @param	None
 * @retval	None
 */
void Delay_5Us(void)		
{
	uint8_t us = 5U;
	while(--us);
}

/*不优化*/
// #pragma OPTIMIZE(0)
 /**
 * @brief	ms级别的延时
 * @details	IIC的SDA、SCL引脚切换需要一定延时
 * @param	ms，需要延长的us数
 * @retval	None
 */
 void Delay_Ms(uint16_t ms)		//@27MHz
{
	uint8_t i = 0, j = 0;

	do
	{	/* code */
		_nop_();
		_nop_();
		// _nop_();
		i = 36;
		j = 13;
		do
		{
			while (--j);
		} while (--i);
	} while (--ms);
}

