#ifndef _USER_DRIVE_
#define _USER_DRIVE_
#include "includes.h"
#include "Driver_usart.h"	
void Driver_Send32DataValue(uint8 function,uint32 value);
void Driver_Send8DataValue(uint8 function,u8 value);
/*! 
*  \brief     �����ַ�������
*  \param  function    ����λ
*  \param  pbuffer    �ַ���
*  \param  length    ����
*/
void Driver_SendChardata(uint8 function,char *pbuffer,u8 length);
void Send_DriverParameter();
#endif  