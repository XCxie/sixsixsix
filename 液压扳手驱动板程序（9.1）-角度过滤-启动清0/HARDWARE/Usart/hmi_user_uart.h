#ifndef _USER_UART__
#define _USER_UART__
#include "stm32f10x_it.h"     //�����û�MCU�����޸�
#include "includes.h"
extern u8 flagFrame;
extern char Password[8];//�趨������
extern char Unlock_Password[8];  //��������

#define cmd_head   0xA5
#define PTR2U16(PTR) ((((uint8 *)(PTR))[0]<<8)|((uint8 *)(PTR))[1])  //�ӻ�����ȡ16λ����
#define PTR2U32(PTR) ((((uint8 *)(PTR))[0]<<24)|(((uint8 *)(PTR))[1]<<16)|(((uint8 *)(PTR))[2]<<8)|((uint8 *)(PTR))[3])  //�ӻ�����ȡ32λ����

/****************************************************************************
* ��    �ƣ� UartInit()
* ��    �ܣ� ���ڳ�ʼ��
* ��ڲ����� ��
* ���ڲ����� ��
****************************************************************************/
void Uart4Init(uint32 Baudrate);

/*****************************************************************
* ��    �ƣ� SendChar()
* ��    �ܣ� ����1���ֽ�
* ��ڲ����� t  ���͵��ֽ�
* ���ڲ����� ��
*****************************************************************/
void  SendChar(uchar t);
void UartRxMonitor(u8 ms); //���ڽ��ռ��
void UartDriver(); //������������
u8 UartWrite(u8 *buf2 ,u8 len2);  //���ڷ�������
u8 UartRead(u8 *buf, u8 len) ;
extern u8 Controlflag;
extern u8 Passwordflag;
#endif
