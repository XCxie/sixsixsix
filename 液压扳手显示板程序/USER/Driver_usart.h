#ifndef _DRIVER_UART__
#define _DRIVER_UART__

#include "stm32f10x_it.h"     //�����û�MCU�����޸�
#include "sys.h"
#include "crc16.h"
extern u8 Driver_flagFrame;
extern u8 RecordCount ;
extern float Collect_Pressure;
extern u16 Collect_Angle;
#define PTR2U16(PTR) ((((uint8 *)(PTR))[0]<<8)|((uint8 *)(PTR))[1])  //�ӻ�����ȡ16λ����
#define PTR2U32(PTR) ((((uint8 *)(PTR))[0]<<24)|(((uint8 *)(PTR))[1]<<16)|(((uint8 *)(PTR))[2]<<8)|((uint8 *)(PTR))[3])  //�ӻ�����ȡ32λ����

/****************************************************************************
* ��    �ƣ� UartInit()
* ��    �ܣ� ���ڳ�ʼ��
* ��ڲ����� ��
* ���ڲ����� ��
****************************************************************************/
void Driver_UartInit(uint32 Baudrate);

/*****************************************************************
* ��    �ƣ� Driver_SendChar()
* ��    �ܣ� ����1���ֽ�
* ��ڲ����� t  ���͵��ֽ�
* ���ڲ����� ��
*****************************************************************/
void  Driver_SendChar(uchar t);
void UartRxMonitor(u8 ms); //���ڽ��ռ��
void UartDriver(); 
u8 Driver_UartWrite(u8 *buf2 ,u8 len2);  //���ڷ�������
u8 Driver_UartRead(u8 *buf, u8 len) ;//���ڽ�������


#endif
