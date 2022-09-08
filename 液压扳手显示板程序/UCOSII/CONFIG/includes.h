/*
************************************************************************************************
主要的包含文件

文 件: INCLUDES.C ucos包含文件
作 者: 周
************************************************************************************************
*/

#ifndef __INCLUDES_H__
#define __INCLUDES_H__
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdarg.h>

#include "ucos_ii.h"
#include "os_cpu.h"
#include "os_cfg.h"

#include "sys.h"
#include "TASK.h"
#include "him_user_driver.h"
#include "him_user_process.h"
#include "Driver_usart.h"
#include "Driver_user_driver.h"
#include "USBType-C.h" 
#include "MX25L6406.h"
#include "adc.h"

 
#define ROUND_TO_UINT16(x)   ((uint16_t)(x)+0.5)>(x)? ((uint16_t)(x)):((uint16_t)(x)+1)   //四舍五入
#define ROUND_TO_UINT32(x)   ((uint32_t)(x)+0.5)>(x)? ((uint32_t)(x)):((uint32_t)(x)+1)   //四舍五入

#endif































