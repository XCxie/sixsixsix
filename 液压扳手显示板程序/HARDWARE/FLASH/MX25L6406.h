#ifndef _MX25L6406_H
#define _MX25L6406_H
#include "sys.h"
#include "delay.h"
#include "stdlib.h"
#include "Data_process.h"
#include "USBType-C.h" 
#include "timer.h"
#define MX25L6406_CS_H GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define MX25L6406_CS_L GPIO_ResetBits(GPIOA,GPIO_Pin_4)

extern u32 RecorddataCount;
extern u8 RecordFlag;


//extern u16 Flash_count;
extern u8 Databuff[120];
extern u8 DriveControl_status;
void Flash_Model_parameters(u8* Storage);
void  Get_Flash_Parameters(void);
void Data_Record_Storage(u8 *RecordDatabuf,u32 DataCount,u8 length);

void MX25L6406_WriteU16(u16 data,u32 addr);
u16  MX25L6406_ReadU16(u32 addr);
void MX25L6406_WriteU32(u32 data,u32 addr);
u32  MX25L6406_ReadU32(u32 addr);




void MX25L6406_Init(void);
u8 SPI1_ReadWriteByte(u8 send_data);
void MX25L6406_Write_EN(void);
void MX25L6406_Read_status(void);
void MX25L6406_Read(u8 *data,u32 addr,u32 n);
void MX25L6406_Write_data(u32 addr,u8 data);
void MX25L6406_Erase(u32 addr,u8 cmd);
void MX25L6406_Write(u8 *ftrBuffer,u32 addr, u32 ftrLen);
void SPI_Flash_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);   
void SPI_Flash_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
#define	Sector	0x20
#define	Block	0xD8
#define	Chip	0xC7


#endif

