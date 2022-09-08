#ifndef	_Hydraulic_control_H_
#define	_Hydraulic_control_H_
#include "includes.h"
//typedef struct 
//{
//   char Pro_type;
//   int Pro_value;
//}_Product_Dat;
//extern _Product_Dat Product;
extern u16 Pressure_temp;
extern u8 Motor_status;    
extern u8 SolenoidValve_status;
extern u8 Pressure_status;
void Hydraulic_control(void);

#endif