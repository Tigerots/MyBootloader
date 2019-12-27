/******************** (C) COPYRIGHT 2007 HuaYuan ********************
* File Name          : hw_config.h
* Author             : Han yongliang
* Date First Issued  : 03/30/2008
* Description        : Header for all file.
********************************************************************************
* History:
* 05/11/2008: V0.1
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _HW_CONFIG_H
#define _HW_CONFIG_H


//*********宏定义************************************
//以下定义升级标志
#define UPD_START_FLAG			1			//开始升级标志
#define UPD_NORMAL_FLAG			2			//正在升级标志
#define UPD_END_FLAG			3			//结束升级标志

//****以下与USB通信有关******************************

#define USB_REC_BUFFER_SIZE		64			//接收缓冲长度
#define REC_BUFFER_SIZE_REP		0x40			//接收缓冲长度,报告描述时使用

#define USB_SEND_BUFFER_SIZE	64			//发送缓冲长度
#define SEND_BUFFER_SIZE_REP 	0x40			//发送缓冲长度,报告描述时使用

//#define LenIs1
#ifdef	LenIs1
	#define CMD_HEADER_LENGTH		0x05		//命令头长度,包括命令头+命令长度及校验码   0x07
	#define CMD_HEADER_TAIL_LENGTH 	0x07		//命令头和尾的总长度:包括命令头+命令长度及校验码+校验码+命令尾	0x09
#else
	#define CMD_HEADER_LENGTH		0x07		//命令头长度,包括命令头+命令长度及校验码   0x07
	#define CMD_HEADER_TAIL_LENGTH 	0x09		//命令头和尾的总长度:包括命令头+命令长度及校验码+校验码+命令尾	0x09
#endif

#define CMD_HEADER1			0xA2			//USB通信命令头第一字节
#define	CMD_HEADER2			0x13			//USB通信命令头第二字节
#define CMD_HEADER3			0xB4			//USB通信命令头第三字节
#define CMD_TAIL			0x00			//USB通信命令尾


#define CMD_NONE			0x00			//无命令
#define CMD_Read_Status		0xC3			//读状态
#define CMD_Disp_One_Line	0xC4			//显示一行命令
#define CMD_Disp_All		0xC5			//显示所有行命令
#define CMD_LED_Clear		0xC6			//清显示命令
#define CMD_Update_Program	0xB5			//固件升级
		#define CMD_UpdateFW			0xB5		//固件升级
	    #define UPD_START_FLAG			1			//开始升级标志
		#define UPD_NORMAL_FLAG			2			//正在升级标志
		#define UPD_END_FLAG			3			//结束升级标
		#define CMD_UPD_FILE_SIZE_ERROR	0x14	//升级文件大小有误
		#define CMD_UPD_FILE_ERROR		0x15	//升级文件格式错误
		#define CMD_UPD_CHECKSUM_ERROR	0x16	//升级文件校验和不对
		#define UPD_FLAG_ERROR			0x17	//升级标志错误
		#define UPD_LENGTH_ERROR		0x18	//升级数据长度错误
		#define FILE_WRITE_ERROR		0x19		//写文件错误



#define FLASH_OK			CMD_OK
#define FLASH_ERROR			1

#define CMD_OK				0
#define CMD_FAIL			1
#define CMD_TIMEOUT			1
#define UNSUPPORT_ERROR		2				//不支持该命令
#define VERIFY_SUM_ERROR		0x05		//检验和错误
#define LENGTH_VERIFY_SUM_ERROR	0x06		//长度校验和错误
#define CMD_PARAM_ERROR			0x07		//命令参数格式错误

#define UPDATE_FILE_SIZE_ERROR		0x14		//升级文件大小错误
#define UPDATE_FILE_FORMAT_ERROR 	0x15		//升级文件格式错误，无效升级文件
#define UPDATE_FILE_WRITE_ERROR		0x16		//升级文件写错误
#define UPDATE_FILE_SUM_ERROR		0x18		//升级文件校验和错误

#define SETTED		1	//已经设置
#define UNSETTED	0	//没有设置



void Read_Device_Status(u8 Interface_Type);

void Check_USB_Handle_Task(void);
u8 Check_USB_CMD_Input(void);
void USB_CMD_Handle(u8 command);
void USB_Module_Handle(void);
u8 System_PowerUp(void);



#endif

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
