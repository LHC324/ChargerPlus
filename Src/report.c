#include "report.h"
#include "config.h"
#include "usart.h"


void Uart_PriorityProcess(EXT_COMM *comm,SEL_CHANNEL channel);

/*********************************************************************
 * Function:    void dataExchange(void);
 * Overview:    ͨ�����ݽ���
 * Input:       None
 * Output:      None
 * Note:        ���������ݽ���
 ********************************************************************/
void dataExchange(void)	//ͬ���ȼ�ÿ��ֻ���ж�һ��������ǰ�����˸��������ȼ���������ɶ�ʧ
{
	if(PLC_SaveData_Flag)
	{
		return; // ������ڶ�PLC�����ݣ���ֹ����ͨѶ��д
	}
	
	if(!gPlcFrame.frame && !gLteFrame.frame && !gRs485Frame.frame && !gLanFrame.frame) 
	{
		return;	// ������д��ڶ�û���յ��������ݰ������������ݸ��ƣ�����ֱ�ӷ���
	}
	
	if(gPlcFrame.frame)	// �յ�����PLC������Ӧ�������Ҫ�����ݷ��ظ���ʱ���ⲿ����
	{
		copyPlcToExt();
		return;								// PLCӦ������͸�����˿�
	}

	if(gPlcFrame.busy) 
		return;				// �����ǰPLC�˿�����æ������Ҫ�ȵȴ�PLCӦ���ʱ������ͷ�PLC�˿�

	
//	Uart_PriorityProcess(&gLanFrame,CHANNEL_LAN); //��̫���ڣ���ǰδ����
//	Uart_PriorityProcess(&gRs485Frame,CHANNEL_RS485);
	Uart_PriorityProcess(&gLteFrame,CHANNEL_LTE);
}



/*********************************************************************
 * Function:    void enableExtTx(unsigned char UartChannel);
 * Overview:    �ֶ����ͻ������ڵ�һ���ֽڣ��������ڵ��Զ������жϹ��̣������ֽ����жϳ���˳����ɣ�
 * Input:       UartChannel -- Ҫ�����Ĵ��ں�
 * Output:      None
 * Note:        ���������ݽ���
 ********************************************************************/
void enableExtTx(SEL_CHANNEL UartChannel)
{
	switch(UartChannel)
	{
		case CHANNEL_PLC:
		{
			Busy_Await(&Uart4,UART_BYTE_SENDOVERTIME);  // �÷��Ͷ˿�Ϊæ,�������ֽڼ���С���					
			S4BUF = *gPlcFrame.pBuffer++;				// ���͵�һ���ֽ�
			gPlcFrame.dataLenth--;						// ���������ݳ��ȼ�һ
			break;
		}
		case CHANNEL_LTE:
		{
			Busy_Await(&Uart2,UART_BYTE_SENDOVERTIME);  // �÷��Ͷ˿�Ϊæ,�������ֽڼ���С���
			S2BUF = *gLteFrame.pBuffer++;				// ���͵�һ���ֽ�
			gLteFrame.dataLenth--;						// ���������ݳ��ȼ�һ
			break;
		}
//		case CHANNEL_RS485:
//		{
//			Busy_Await(&Uart3,UART_BYTE_SENDOVERTIME);  // �÷��Ͷ˿�Ϊæ,�������ֽڼ���С���
//			S3BUF = *gRs485Frame.pBuffer++;				// ���͵�һ���ֽ�
//			gRs485Frame.dataLenth--;					// ���������ݳ��ȼ�һ
//			break;
//		}
		case CHANNEL_LAN:
		{
			Busy_Await(&Uart1,UART_BYTE_SENDOVERTIME);  // �÷��Ͷ˿�Ϊæ,�������ֽڼ���С���
			SBUF = *gLanFrame.pBuffer++;				// ���͵�һ���ֽ�			
			gLanFrame.dataLenth--;						// ���������ݳ��ȼ�һ
			break;
		}
		default : break;
	}
}

/*********************************************************************
 * Function:    void copyExtToPlc(unsigned char channel)
 * Overview:    �ⲿ�˿����ݸ��Ƶ�PLC
 * Input:       channel -- ͨ����
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
			gPlcFrame.dataLenth = gLteFrame.dataLenth; // �������ݳ��� 
			gPlcFrame.channelSel = CHANNEL_LTE; // ����PLCӦ��ͨ��								
			gLteFrame.pBuffer = &lteBuffer[0];  //LTE�ڽ�������ת����PLC�������Ϻ󣬸�λLTE��ָ������ݳ���
			gLteFrame.dataLenth = 0;			//LTE�ڽ�������ת����PLC�������Ϻ󣬸�λLTE��ָ������ݳ���
			gLteFrame.frame = 0;				//LTE�ڽ�������ת����PLC�������Ϻ󣬸�λLTE��ָ������ݳ���
			enableExtTx(CHANNEL_PLC);			// ����PLC���ڷ���
			break;
		}
//		case CHANNEL_RS485:
//		{

//			memcpy(plcBuffer, rs485Buffer,gRs485Frame.dataLenth);
//			gPlcFrame.dataLenth =gRs485Frame.dataLenth;
//			gPlcFrame.channelSel = CHANNEL_RS485;   // ���浱ǰPLC����ͨ��	
//			memset(&rs485Buffer, 0, sizeof(rs485Buffer)); //��ջ���
//			gRs485Frame.pBuffer = &rs485Buffer[0];  //485�ڽ�������ת����PLC�������Ϻ󣬸�λ485��ָ������ݳ���
//			gRs485Frame.dataLenth = 0;				//485�ڽ�������ת����PLC�������Ϻ󣬸�λ485��ָ������ݳ���
//			gRs485Frame.frame = 0;					//485�ڽ�������ת����PLC�������Ϻ󣬸�λ485��ָ������ݳ���
//			enableExtTx(CHANNEL_PLC);				// ����PLC���ڷ���
//			break;
//		}
		case CHANNEL_LAN:
		{
		    memcpy(plcBuffer, lanBuffer, gLanFrame.dataLenth);
			gPlcFrame.dataLenth =gLanFrame.dataLenth;	
			gPlcFrame.channelSel = CHANNEL_LAN; // ���浱ǰPLC����ͨ��							
			gLanFrame.pBuffer = &lanBuffer[0];  //LAN�ڽ�������ת����PLC�������Ϻ󣬸�λLAN��ָ������ݳ���
			gLanFrame.dataLenth = 0;			//LAN�ڽ�������ת����PLC�������Ϻ󣬸�λLAN��ָ������ݳ���
			gLanFrame.frame = 0;				//LAN�ڽ�������ת����PLC�������Ϻ󣬸�λLAN��ָ������ݳ���
			enableExtTx(CHANNEL_PLC);			// ����PLC���ڷ���  ���������⿼�ǽ���ŵ�gLanFrame.frame = 0; ֮�£�
			break;
		}
		default:break;
	}
	gPlcFrame.plcAnswerTimer = T_PLC_ANSWER;// ��������ת����PLC�������ȴ�PLCӦ��ʱ�� ��5.25�Ƿ��ڽ��յ�ʱ���ٶ�ʱ��
	gPlcFrame.frame = 0;					// �������PLCӦ������

}

/*********************************************************************
 * Function:    void copyExtToPlc(unsigned char channel)
 * Overview:    �ⲿ�˿����ݸ��Ƶ�PLC
 * Input:       channel -- ͨ����
 * Output:      None
 * Note:        None
 ********************************************************************/
void copyPlcToExt(void)
{
	switch(gPlcFrame.channelSel)
	{
		case CHANNEL_LTE:				// PLC��4Gͨ��
		{
			memcpy(lteBuffer, plcBuffer, gPlcFrame.dataLenth);
			gLteFrame.dataLenth = gPlcFrame.dataLenth;
			enableExtTx(CHANNEL_LTE);	// ����PLC���ڷ���
			break;
		}
//		case CHANNEL_RS485:				// PLC��RS485ͨ��
//		{
//			memcpy(rs485Buffer, plcBuffer, gPlcFrame.dataLenth);
//			gRs485Frame.dataLenth = gPlcFrame.dataLenth;
//			enableExtTx(CHANNEL_RS485);	// ����485���ڷ���
//			break;
//		}
		case CHANNEL_LAN:				// PLC����̫��ͨ��
		{
			memcpy(lanBuffer, plcBuffer, gPlcFrame.dataLenth);
			gLanFrame.dataLenth = gPlcFrame.dataLenth;
			enableExtTx(CHANNEL_LAN);	// ������̫�����ڷ���
			break;
		}
		default: break;
	}

	gPlcFrame.busy = 0;					// ���PLC����æ��־
	gPlcFrame.frame= 0;					// ��PLC����֡��ɱ�־
 	gPlcFrame.dataLenth = 0;			//PLCӦ������ɺ󣬸�λ���ݳ���
	gPlcFrame.pBuffer = &plcBuffer[0];  //�������ָ�븴λ
	memset(&plcBuffer, 0, sizeof(plcBuffer)); //��ջ���
	gPlcFrame.channelSel = CHANNEL_IDLE;           //������ɣ�ͨ�����ڿ���״̬

}


void Uart_PriorityProcess(EXT_COMM *comm,SEL_CHANNEL channel)
{	
	if(comm->uartStatus == READY) //�����źŵ���
	{
		comm->uartStatus = RUN;
		copyExtToPlc(channel);
		
		memset(&comm->pBuffer[0],0,MAX_BUFFER); //�����жϽ�������ն�Ӧ���ջ�����sizeof(comm->pBuffer)
		comm->dataLenth = 0; //�����ݳ�������
		comm->uartStatus = BLOCK;
	}
}





