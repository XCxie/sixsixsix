#ifndef	_Hydraulic_process_H_
#define	_Hydraulic_process_H_
#include "includes.h"
/*! 
*  \brief     获得螺栓型号序号
*  \param  type    扳手型号  
*  \param  Standard  扳手规格
*  \param  value     返回值 扳手序号
*/
u8 Bolt_Correspondence(uint8 Standard_Type,uint8 Diameter,uint8 Garder);
void Set_ArrayCount(u8 *Databuf,u32 value);
#endif