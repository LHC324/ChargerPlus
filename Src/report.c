#include "report.h"
#include "config.h"
#include "usart.h"


void Uart_PriorityProcess(EXT_COMM *comm,SEL_CHANNEL channel);

/*********************************************************************
 * Function:    void dataExchange(void);
 * Overview:    通信数据交换
 * Input:       None
 * Output:      None
 * Note:        各串口数据交换
 ********************************************************************/
void dataExchange(void)	//同优先级每次只能判断一个，因提前设置了各串口优先级，不会造成丢失
{
	if(PLC_SaveData_Flag)
	{
		return; // 如果正在对PLC存数据，禁止其他通讯口写
	}
	
	if(!gPlcFrame.frame && !gLteFrame.frame && !gRs485Frame.frame && !gLanFrame.frame) 
	{
		return;	// 如果所有串口都没有收到完整数据包，则无需数据复制，程序直接返回
	}
	
	if(gPlcFrame.frame)	// 收到来自PLC的完整应答包，需要将数据返回给来时的外部串口
	{
		copyPlcToExt();
		return;								// PLC应答包发送给外设端口
	}

	if(gPlcFrame.busy) 
		return;				// 如果当前PLC端口正在忙，则需要先等待PLC应答或超时后才能释放PLC端口

	
//	Uart_PriorityProcess(&gLanFrame,CHANNEL_LAN); //以太网口，当前未启用
//	Uart_PriorityProcess(&gRs485Frame,CHANNEL_RS485);
	Uart_PriorityProcess(&gLteFrame,CHANNEL_LTE);
}



/*********************************************************************
 * Function:    void enableExtTx(unsigned char UartChannel);
 * Overview:    手动发送缓冲区内第一个字节，启动串口的自动发送中断过程（后续字节由中断程序顺序完成）
 * Input:       UartChannel -- 要启动的串口号
 * Output:      None
 * Note:        各串口数据交换
 ********************************************************************/
void enableExtTx(SEL_CHANNEL UartChannel)
{
	switch(UartChannel)
	{
		case CHANNEL_PLC:
		{
			Busy_Await(&Uart4,UART_BYTE_SENDOVERTIME);  // 置发送端口为忙,并保持字节间最小间隔					
			S4BUF = *gPlcFrame.pBuffer++;				// 发送第一个字节
			gPlcFrame.dataLenth--;						// 待发送数据长度减一
			break;
		}
		case CHANNEL_LTE:
		{
			Busy_Await(&Uart2,UART_BYTE_SENDOVERTIME);  // 置发送端口为忙,并保持字节间最小间隔
			S2BUF = *gLteFrame.pBuffer++;				// 发送第一个字节
			gLteFrame.dataLenth--;						// 待发送数据长度减一
			break;
		}
//		case CHANNEL_RS485:
//		{
//			Busy_Await(&Uart3,UART_BYTE_SENDOVERTIME);  // 置发送端口为忙,并保持字节间最小间隔
//			S3BUF = *gRs485Frame.pBuffer++;				// 发送第一个字节
//			gRs485Frame.dataLenth--;					// 待发送数据长度减一
//			break;
//		}
		case CHANNEL_LAN:
		{
			Busy_Await(&Uart1,UART_BYTE_SENDOVERTIME);  // 置发送端口为忙,并保持字节间最小间隔
			SBUF = *gLanFrame.pBuffer++;				// 发送第一个字节			
			gLanFrame.dataLenth--;						// 待发送数据长度减一
			break;
		}
		default : break;
	}
}

/*********************************************************************
 * Function:    void copyExtToPlc(unsigned char channel)
 * Overview:    外部端口数据复制到PLC
 * Input:       channel -- 通道号
 * Output:      None
 * Note:        None
 ********************************************************************/
void copyExtToPlc(SEL_CHANNEL channel)
{
	switch(channel)
	{
		case CHANNEL_LTE:
		{									
		    memcpy(plcBuffer, lteBuffer,gLteFrame.dataLenth);
			gPlcFrame.dataLenth = gLteFrame.dataLenth; // 复制数据长度 
			gPlcFrame.channelSel = CHANNEL_LTE; // 保持PLC应答通道								
			gLteFrame.pBuffer = &lteBuffer[0];  //LTE口接收数据转发到PLC缓存区上后，复位LTE口指针和数据长度
			gLteFrame.dataLenth = 0;			//LTE口接收数据转发到PLC缓存区上后，复位LTE口指针和数据长度
			gLteFrame.frame = 0;				//LTE口接收数据转发到PLC缓存区上后，复位LTE口指针和数据长度
			enableExtTx(CHANNEL_PLC);			// 启动PLC串口发送
			break;
		}
//		case CHANNEL_RS485:
//		{

//			memcpy(plcBuffer, rs485Buffer,gRs485Frame.dataLenth);
//			gPlcFrame.dataLenth =gRs485Frame.dataLenth;
//			gPlcFrame.channelSel = CHANNEL_RS485;   // 保存当前PLC交互通道	
//			memset(&rs485Buffer, 0, sizeof(rs485Buffer)); //清空缓存
//			gRs485Frame.pBuffer = &rs485Buffer[0];  //485口接收数据转发到PLC缓存区上后，复位485口指针和数据长度
//			gRs485Frame.dataLenth = 0;				//485口接收数据转发到PLC缓存区上后，复位485口指针和数据长度
//			gRs485Frame.frame = 0;					//485口接收数据转发到PLC缓存区上后，复位485口指针和数据长度
//			enableExtTx(CHANNEL_PLC);				// 启动PLC串口发送
//			break;
//		}
		case CHANNEL_LAN:
		{
		    memcpy(plcBuffer, lanBuffer, gLanFrame.dataLenth);
			gPlcFrame.dataLenth =gLanFrame.dataLenth;	
			gPlcFrame.channelSel = CHANNEL_LAN; // 保存当前PLC交互通道							
			gLanFrame.pBuffer = &lanBuffer[0];  //LAN口接收数据转发到PLC缓存区上后，复位LAN口指针和数据长度
			gLanFrame.dataLenth = 0;			//LAN口接收数据转发到PLC缓存区上后，复位LAN口指针和数据长度
			gLanFrame.frame = 0;				//LAN口接收数据转发到PLC缓存区上后，复位LAN口指针和数据长度
			enableExtTx(CHANNEL_PLC);			// 启动PLC串口发送  （若有问题考虑将其放到gLanFrame.frame = 0; 之下）
			break;
		}
		default:break;
	}
	gPlcFrame.plcAnswerTimer = T_PLC_ANSWER;// 外设数据转发到PLC，启动等待PLC应答定时器 （5.25是否在接收的时候再定时）
	gPlcFrame.frame = 0;					// 允许接收PLC应答数据

}

/*********************************************************************
 * Function:    void copyExtToPlc(unsigned char channel)
 * Overview:    外部端口数据复制到PLC
 * Input:       channel -- 通道号
 * Output:      None
 * Note:        None
 ********************************************************************/
void copyPlcToExt(void)
{
	switch(gPlcFrame.channelSel)
	{
		case CHANNEL_LTE:				// PLC和4G通信
		{
			memcpy(lteBuffer, plcBuffer, gPlcFrame.dataLenth);
			gLteFrame.dataLenth = gPlcFrame.dataLenth;
			enableExtTx(CHANNEL_LTE);	// 启动PLC串口发送
			break;
		}
//		case CHANNEL_RS485:				// PLC和RS485通信
//		{
//			memcpy(rs485Buffer, plcBuffer, gPlcFrame.dataLenth);
//			gRs485Frame.dataLenth = gPlcFrame.dataLenth;
//			enableExtTx(CHANNEL_RS485);	// 启动485串口发送
//			break;
//		}
		case CHANNEL_LAN:				// PLC和以太网通信
		{
			memcpy(lanBuffer, plcBuffer, gPlcFrame.dataLenth);
			gLanFrame.dataLenth = gPlcFrame.dataLenth;
			enableExtTx(CHANNEL_LAN);	// 启动以太网串口发送
			break;
		}
		default: break;
	}

	gPlcFrame.busy = 0;					// 解除PLC串口忙标志
	gPlcFrame.frame= 0;					// 清PLC接收帧完成标志
 	gPlcFrame.dataLenth = 0;			//PLC应答发送完成后，复位数据长度
	gPlcFrame.pBuffer = &plcBuffer[0];  //发送完成指针复位
	memset(&plcBuffer, 0, sizeof(plcBuffer)); //清空缓存
	gPlcFrame.channelSel = CHANNEL_IDLE;           //发送完成，通道置于空闲状态

}


void Uart_PriorityProcess(EXT_COMM *comm,SEL_CHANNEL channel)
{	
	if(comm->uartStatus == READY) //就绪信号到达
	{
		comm->uartStatus = RUN;
		copyExtToPlc(channel);
		
		memset(&comm->pBuffer[0],0,MAX_BUFFER); //发送中断结束后，清空对应接收缓冲区sizeof(comm->pBuffer)
		comm->dataLenth = 0; //把数据长度清零
		comm->uartStatus = BLOCK;
	}
}





