#ifndef  __CH375DRIV_H__
#define  __CH375DRIV_H__

#include "MX25L6406.h"
void  CH374_Init( void );
void  mStopIfError( u8 iError );
void StructToStr(char buf[],char databuf[]);
void WriteTail(void);
#endif

