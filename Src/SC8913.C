#include "SC8913.h"

/*9级优化，速度优先*/
// #pragma OPTIMIZE(9, speed)

/*存储SC8913所有寄存器值*/
data uint8_t g_Sc8913_Registers[CHANNEL_MAX][REGISTER_MAX] = {0};

/*SC8913寄存器组数据初始化*/
const uint8_t Init_RegMap_Value[] = 
{
    0x01, SET_VBUSREF_I_SET(5.0F), 0xC0, 0x7C, 0xC0, SET_IBUS_LIMT(IBUS_MAX_CURRENT), SET_IBAT_LIMT(IBAT_MAX_CURRENT), 
    SET_VINREG_SET(IBUS_THRESHOLD_VOLTAGE), SET_RATIO(1U), SET_CTRL0_SET(0U), SET_CTRL1_SET(1U), SET_CTRL2_SET(1U),
    SET_CTRL3_SET(1)
};


/**
 * @brief	初始化电源管理芯片SC8913
 * @details	轮询模式，寄存器采用连续写模式
 * @param	None
 * @retval	None
 */
uint8_t Sc8913_Init(void)
{
    uint8_t j = 0;
    uint8_t ret = REG_OK; 

    for(j = 0; j < CHANNEL_MAX; j++)
    {   
        /*初始化SC8913寄存器组*/
        if(!Sc8913_Write_Register(j, VBAT_SET_ADDR, Init_RegMap_Value, WRITE_OPTION_REGS)) 
        {
            ret = REG_FAILE;
        }
    }
    return ret;
}

/**
 * @brief	读取当前设备的10bitADC采样寄存器
 * @details	读取到的值直接存储到union对象对应的高低位寄存器中
 * @param	channel：当前通道
 * @retval	读取结果
 */
uint8_t Read_Register_Value(IIC_Channel channel) 
{
    uint8_t ret = REG_OK;

    /*读取每个通道电源管理芯片SC8913所有寄存器值*/
    if(Sc8913_Read_Register(channel, VBAT_SET_ADDR, (uint8_t *)&g_Sc8913_Registers[channel][0], REGISTER_MAX) == I2C_Fail)
    {
        ret = REG_FAILE;
    }

    return ret;
}

/*以下代码9级优化，速度优先*/
//#pragma OPTIMIZE(9, speed)

/**
 * @brief	设置对应设备充电通道芯片引脚的开启/关闭
 * @details	高电平导通/低电平关闭
 * @param	*driverx：当前设备指针;state:对应设备相应引脚的状态;
 * @retval	None
 */
void Set_Sc8913_Stop(IIC_Channel channel, IIC_Pin_State state)
{
    switch(channel)
    {
        case Channel0: STOP_CHANNEL0 = state;
             break;
        case Channel1: STOP_CHANNEL1 = state;
             break; 
        case Channel2: STOP_CHANNEL2 = state;
             break;
        default : break;    
    }
}

#if (USING_CHARGEING_PIN)
/**
 * @brief	设置对应设备充电通道物理线路的开启/关闭
 * @details	高电平导通/低电平关闭
 * @param	*driverx：当前设备指针;state:对应设备相应引脚的状态;
 * @retval	None
 */
void Set_Sc8913_charger(IIC_Channel channel, IIC_Pin_State state)
{
    switch(channel)
    {
        case Channel0: CHARGER_CHANNEL0 = state;
             break;
        case Channel1: CHARGER_CHANNEL1 = state;
             break; 
        case Channel2: CHARGER_CHANNEL2 = state;
             break;
        default : break;    
    }
}
#endif

/**
 * @brief	写入1个或连续的多个对应通道下IIC设备内部寄存器
 * @details	支持连续地址的多个寄存器值的读取
 * @param	*driverx：当前设备指针;start_addr:开始寄存器地址;*dat:读出的寄存器值回调指针;length:读取的数据长度
 * @retval	总线操作的结果
 */
uint8_t Sc8913_Write_Register(IIC_Channel channel, uint8_t start_addr, uint8_t *dat, uint8_t length)
{
    uint8_t i = 0;
    /*建立IIC通讯信号*/
    I2C_Start(channel);
    /*发送寄存器操作类型命令*/
    I2C_SendByte(channel, SAVLE_WRITE_ADDRESS);
    /*等待应答信号*/
    if(I2C_WaitAck(channel, 0x100) == I2C_Fail)
    {
        return I2C_Fail;
    }
    /*发送操作寄存器地址*/
    I2C_SendByte(channel, start_addr);
    /*等待应答信号*/
    if(I2C_WaitAck(channel, 0x100) == I2C_Fail)
    {
        return I2C_Fail;
    }
    /*写入多个数据*/
    for(i = 0; i < length; i++)
    {
        /*发送数据*/
        I2C_SendByte(channel, dat[i]);
        /*等待应答信号*/
        if(I2C_WaitAck(channel, 0x100) == I2C_Fail)
        {
            return I2C_Fail;
        }
    }
    /*停止IIC通讯*/
    I2C_Stop(channel);

    return I2C_Succeed;
}

/**
 * @brief	读取1个或连续的多个对应通道下IIC设备内部寄存器值
 * @details	支持连续地址的多个寄存器值的读取
 * @param	*driverx：当前设备指针;start_addr:开始寄存器地址;*dat:读出的寄存器值回调指针;length:读取的数据长度
 * @retval	总线操作的结果
 */
uint8_t Sc8913_Read_Register(IIC_Channel channel, uint8_t start_addr, uint8_t *dat, uint8_t length)
{
    uint8_t g = 0;
    /*建立IIC通讯信号*/
    I2C_Start(channel);
    /*发送写寄存器命令*/
    I2C_SendByte(channel, SAVLE_WRITE_ADDRESS);
    /*等待应答信号*/
    if(I2C_WaitAck(channel, 0x100) == I2C_Fail)
    {
        return I2C_Fail;
    }
    /*发送需要开始读取的地址*/
    I2C_SendByte(channel, start_addr);
    /*等待应答信号*/
    if(I2C_WaitAck(channel, 0x100) == I2C_Fail)
    {
        return I2C_Fail;
    }

    /*重新建立IIC通讯信号*/
    I2C_Start(channel);
    /*发送读取寄存器命令*/
    I2C_SendByte(channel, SAVLE_READ_ADDRESS);
    /*等待应答信号*/
    if(I2C_WaitAck(channel, 0x100) == I2C_Fail)
    {
        return I2C_Fail;
    }
    /*读取多个数据*/
    for(g = 0; g < length; g++)
    {
        /*发送数据*/
        *dat++ = I2C_ReadByte(channel) ;
        /*在连续发送模式下*/
        if(length > sizeof(uint8_t))
        {   /*是否发送的n-1位*/
            if(g < length - 1U)
            /*主机发送应答信号*/
            I2C_SendAck(channel);
        }  
    }
    /*读取完毕后发送NACK指令*/
    I2C_SendNAck(channel);
    /*停止IIC通讯*/
    I2C_Stop(channel);

    return I2C_Succeed;
}

