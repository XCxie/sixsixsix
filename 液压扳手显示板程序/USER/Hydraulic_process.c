#include "Hydraulic_process.h"      
/*! 
*  \brief     获得螺栓型号序号

*  \param  type    扳手型号  
*  \param  Standard  扳手规格
*  \param  value     返回值 扳手序号
*/              
u8 Bolt_Correspondence(uint8 Standard_Type,uint8 Diameter,uint8 Garder)
{
        u8 value;	
		if(Standard_Type == Imperial)     //图表标准  英制
		{          
				switch(Garder)
				{
						case  0:
							value = Diameter+1;     //1-20
						break;
						case  1:
							if(Diameter>=15) Diameter=15;	               
							value = Diameter+21;   //21-36
						break;
						case  2:
							value = Diameter+37;   //37-56
						break;
						case  3:
							value = Diameter+57;   //57-76
						break;           					
				}
                }else                           //图表标准  公制
                {
				switch(Garder)
				{
						case  0:
							value = Diameter+77;   //77-103
						break;
						case  1:
							value = Diameter+104;   //104-130
						break;
						case  2:
							value = Diameter+131;   //131-157
						break;
						case  3:
							value = Diameter+158;   //158-184
						break;
						case  4:
							value = Diameter+185;  //185-211 
						break;
				}		
		} 
    return value; 
}


void Set_ArrayCount(u8 *Databuf,u32 value)
{
    Databuf[0] = (value+1)>>24;
    Databuf[1] = (value+1)>>16;
    Databuf[2] = (value+1)>>8;
    Databuf[3] = (value+1);
}



