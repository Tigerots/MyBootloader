/******************** (C) COPYRIGHT 2009 HuaYuan ********************
* File Name          : System_.c
* Author             : Hanyongliang
* Date First Issued  : 11/20/2009
* Description        : 系统层函数
********************************************************************************
* History:
* 09/23/2009
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <string.h> 
#include "stm32f10x_lib.h"
#include "usb_lib.h"
#include "usb_pwr.h"
#include "usb_config.h"
#include "bsp.h"
#include "hw_config.h"
//#include "led.h"
#include "cpu_flash.h"

#define VERSION	SOFTWARE_VERSION


/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


bool USB_Connected_Flag	=	FALSE;
bool USB_Insert_Flag	=	FALSE;	//插入usb线标志

u8 USB_OutBuffer[USB_REC_BUFFER_SIZE];
u8 Update_Buffer[FLASH_UPD_SEND_SIZE];

extern u8 Disp_Buf[16];

u8 UpLoadflag;//升级标志
u32 UpLoadTime;//开始升级时间

extern void Update_Program_Handle(void);


/*******************************************************************************
* Function Name  : System_PowerUp
* Description    : 系统上电启动
* Input          : None
* Output         : None
* Return         : OK:成功; other:错误
*******************************************************************************/
u8 System_PowerUp(void)
{
	return CMD_OK;
}
/*******************************************************************************
* Function Name  : USB_Rec_Buffer_Flush
* Description    : 清除USB接收缓冲区
* Input          : None
* Return         : None
* 影 响			 ：Rec_Buffer清空
*******************************************************************************/
void  USB_Rec_Buffer_Flush(void)
{u16 i;			 
	for(i=0;i<USB_REC_BUFFER_SIZE;i++)
		USB_OutBuffer[i]	=	0;
}

/*******************************************************************************
* Function Name  : USB_CMD_CheckSum
* Description    : USB命令校验和
* Input          : None
* Return         : CMD_OK:成功；CMD_ERROR:失败
* 影 响			 ：None
*******************************************************************************/
u8 USB_CMD_CheckSum(void)
{
u8 check;
u16 i,len;

	len	=	USB_OutBuffer[3];	//包长度
	check	=	0;
	for(i=0;i<len;i++)	
	{
		check	+=	USB_OutBuffer[i+CMD_HEADER_LENGTH];
	}
	if(check==USB_OutBuffer[len+CMD_HEADER_LENGTH])
		return CMD_OK;
	return VERIFY_SUM_ERROR;
}
/*******************************************************************************
* Function Name  : USB_CMD_Respond
* Description    : USB命令响应
* Input          : cmd:命令字
*				   Result:处理结果，成功或失败	
* Return         : None
* 影 响			 ：None
*******************************************************************************/
void USB_CMD_Respond(u8 cmd,u8 Result)
{
	u8 check;
	u8 response[0x10];
	
	response[0]	=	CMD_HEADER1;	//发送命令头
	response[1]	=	CMD_HEADER2;
	response[2]	=	CMD_HEADER3;
	response[3]	=	0x02;		//响应长度低字节
	response[4]	=	0x00;		//响应长度低字节
	response[5]	=	0xFD;		//响应长度低字节反码
	response[6]	=	0xFF;		//响应长度低字节反码
	check	=cmd;
	response[7]	=	cmd;		//响应命令字
	check  += Result;
	response[8]	=	Result;		//响应结果
	response[9]	=	check;		//检验和
	response[10]	=	CMD_TAIL;	//命令尾

	USB_Reader_SendMsg(response,11);

}
/*******************************************************************************
* Function Name  : Update_Program_Handle
* Description    : 升级程序处理
* Input          : None
* Return         : None
* 影 响			 ：None
*******************************************************************************/

/*******************************************************************************
* Function Name  : USB_CMD_Handle
* Description    : 对收到USB口的命令进行处理
* Input          : command:命令
* Return         : None
* 影 响			 ：None
*******************************************************************************/
void USB_CMD_Handle(u8 command)
{
//	u8 ret=0;
	//检查命令的校验和是否正确,不正确则退出
	if(USB_CMD_CheckSum()==VERIFY_SUM_ERROR)
	{
		USB_CMD_Respond(command,VERIFY_SUM_ERROR);	
		USB_Rec_Buffer_Flush();
		return;
	}
	switch(command)
	{
		case 0xB5://UpdateFW 固件升级
			//Update_Program_Handle();	
		break;
		default:
			USB_CMD_Respond(command,UNSUPPORT_ERROR);	//不支持该命令错误
			break;
	}
	USB_Rec_Buffer_Flush();
}
/*******************************************************************************
* Function Name  : Check_USB_CMD_Input
* Description    : 检查USB是否有命令输入
* Input          : None
* Return         : 收到的命字
* 影 响			 ：None
*******************************************************************************/
u8 Check_USB_CMD_Input(void)
{
	u16 len2;
	u16 len1;

	if((USB_OutBuffer[0]==CMD_HEADER1)&&(USB_OutBuffer[1]==CMD_HEADER2)&&(USB_OutBuffer[2]==CMD_HEADER3))	  //检查命令头
	{
		len1	=	USB_OutBuffer[4];	//包长度
		len1	=	(len1<<8)|USB_OutBuffer[3];

		len2	=	USB_OutBuffer[6];	//包长度反码
		len2	=	(len2<<8)|USB_OutBuffer[5];
		len2	=	~len2;
		if((len1==len2))//检查长度及其反码
		{
			//指令长度超过缓冲区不处理
			if(len1>=(USB_REC_BUFFER_SIZE-CMD_HEADER_TAIL_LENGTH))
			{
				USB_Rec_Buffer_Flush();	
			    return CMD_NONE;
			}
			if(USB_OutBuffer[len1+8]==CMD_TAIL)	//检查命令尾
			{
			   	return USB_OutBuffer[CMD_HEADER_LENGTH];
			}
		}
	}
	//如果数据格式不正确,清空
	return CMD_NONE;
}
/*******************************************************************************
* Function Name  : EP1_OUT_Callback.
* Description    : EP1 OUT Callback Routine.从设备接收数据回环
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP1_OUT_Callback(void)
{
	u8 cmd;
	u16 DataLen;
	
	DataLen = GetEPRxCount(ENDP1);
	PMAToUserBufferCopy(USB_OutBuffer, ENDP1_RXADDR, DataLen);
	SetEPRxValid(ENDP1);
	
	cmd=Check_USB_CMD_Input();
	if(cmd!=CMD_NONE) //如果收到命令，则对命令进行处理
	{
	   //FeedWtchDog();
	   USB_CMD_Handle(cmd);
	}
}






