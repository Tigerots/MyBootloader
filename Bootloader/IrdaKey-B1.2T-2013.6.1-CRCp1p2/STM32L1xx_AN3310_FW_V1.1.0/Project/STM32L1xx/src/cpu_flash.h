/******************** (C) COPYRIGHT 2009 HuaYuan ********************
* File Name          : cpu_flash.h
* Author             : Han yongliang
* Date First Issued  : 04/05/2009
* Description        : Header for cpu_flash.c file.
********************************************************************************
* History:
* 05/11/2008: V0.1
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _CPU_FLASH_H
#define _CPU_FLASH_H
//程序Flash升级时用到的宏定义

#define UPDATE_FLAG_ADDR			0x8009000	//升级标志区
#define UPDATE_FLAG_SIZE			0x10		//升级标志大小实际为4字节

#define UPDATE_DATA_ADDR			0x8009400	//升级程序数据区
#define UPDATE_DATE_SIZE			0x6C00		//升级程序区尺寸26K左右

#define SOFTWARE_VERSION_ADDR		0x800FC00	//软件版本地址
#define SOFTWARE_VERSION_SIZE		0x40		//定义了64字节
#define SOFTWARE_VERSION			0x1001		//软件版本为1.0.0.1

#define UPDATE_FLAG_GO				0x01		//(应该)去升级标志
#define UPDATE_FLAG_FINISHED		0xAA55		//升级完成标志
#define FLASH_UPD_SEND_SIZE			48			//每包传输的字节数,应为4的整数倍，否则出错
#define APP_DEVICE_TYPE_HEAD		0xEF0D0001	//应用程序适用设备类型：DZG
#define APP_CHIP_TYPE_MASK			0xFFFE0000	//应用程序适合的芯片型号掩码
#define APP_MIN_SIZE				0x2000		//应用程序最小尺寸8K
#define APP_MAX_SIZE				0x6C00		//应用程序最大尺寸26K


/* Define the STM32F10x hardware depending on the used board */
#define PAGE_SIZE                      (0x400)
#define FLASH_SIZE                     (0x10000) /* 64K */

extern u32 System_Software_Version;

extern void 			User_Erase_Update_Data(void);
extern unsigned char User_Write_Update_Data(int addr,unsigned char  *pt,unsigned int  n);
extern void 			Write_Update_Finish_Flag(void);
extern u32 				Read_Update_Finish_Flag(void);
extern void Write_Software_Version_Flag(void);
extern u32 Read_Software_Version_Flag(void);
extern void Get_Unique_Device_ID(u8 *ID_Buffer);
extern void CMD_UpdateFW_Handle(void);
#endif /* _CPU_FLASH_H */

/******************* (C) COPYRIGHT 2009 HuaYuan *****END OF FILE****/
