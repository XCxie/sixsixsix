#ifndef	_Hydraulic_process_H_
#define	_Hydraulic_process_H_
#include "includes.h"
/*! 
*  \brief     �����˨�ͺ����
*  \param  type    �����ͺ�  
*  \param  Standard  ���ֹ��
*  \param  value     ����ֵ �������
*/
u8 Bolt_Correspondence(uint8 Standard_Type,uint8 Diameter,uint8 Garder);
void Set_ArrayCount(u8 *Databuf,u32 value);
#endif