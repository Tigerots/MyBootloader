/******************** (C) COPYRIGHT 2009 HuaYuan ********************
* File Name          : System_.c
* Author             : Hanyongliang
* Date First Issued  : 11/20/2009
* Description        : ϵͳ�㺯��
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
bool USB_Insert_Flag	=	FALSE;	//����usb�߱�־

u8 USB_OutBuffer[USB_REC_BUFFER_SIZE];
u8 Update_Buffer[FLASH_UPD_SEND_SIZE];

extern u8 Disp_Buf[16];

u8 UpLoadflag;//������־
u32 UpLoadTime;//��ʼ����ʱ��

extern void Update_Program_Handle(void);


/*******************************************************************************
* Function Name  : System_PowerUp
* Description    : ϵͳ�ϵ�����
* Input          : None
* Output         : None
* Return         : OK:�ɹ�; other:����
*******************************************************************************/
u8 System_PowerUp(void)
{
	return CMD_OK;
}
/*******************************************************************************
* Function Name  : USB_Rec_Buffer_Flush
* Description    : ���USB���ջ�����
* Input          : None
* Return         : None
* Ӱ ��			 ��Rec_Buffer���
*******************************************************************************/
void  USB_Rec_Buffer_Flush(void)
{u16 i;			 
	for(i=0;i<USB_REC_BUFFER_SIZE;i++)
		USB_OutBuffer[i]	=	0;
}

/*******************************************************************************
* Function Name  : USB_CMD_CheckSum
* Description    : USB����У���
* Input          : None
* Return         : CMD_OK:�ɹ���CMD_ERROR:ʧ��
* Ӱ ��			 ��None
*******************************************************************************/
u8 USB_CMD_CheckSum(void)
{
u8 check;
u16 i,len;

	len	=	USB_OutBuffer[3];	//������
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
* Description    : USB������Ӧ
* Input          : cmd:������
*				   Result:���������ɹ���ʧ��	
* Return         : None
* Ӱ ��			 ��None
*******************************************************************************/
void USB_CMD_Respond(u8 cmd,u8 Result)
{
	u8 check;
	u8 response[0x10];
	
	response[0]	=	CMD_HEADER1;	//��������ͷ
	response[1]	=	CMD_HEADER2;
	response[2]	=	CMD_HEADER3;
	response[3]	=	0x02;		//��Ӧ���ȵ��ֽ�
	response[4]	=	0x00;		//��Ӧ���ȵ��ֽ�
	response[5]	=	0xFD;		//��Ӧ���ȵ��ֽڷ���
	response[6]	=	0xFF;		//��Ӧ���ȵ��ֽڷ���
	check	=cmd;
	response[7]	=	cmd;		//��Ӧ������
	check  += Result;
	response[8]	=	Result;		//��Ӧ���
	response[9]	=	check;		//�����
	response[10]	=	CMD_TAIL;	//����β

	USB_Reader_SendMsg(response,11);

}
/*******************************************************************************
* Function Name  : Update_Program_Handle
* Description    : ����������
* Input          : None
* Return         : None
* Ӱ ��			 ��None
*******************************************************************************/

/*******************************************************************************
* Function Name  : USB_CMD_Handle
* Description    : ���յ�USB�ڵ�������д���
* Input          : command:����
* Return         : None
* Ӱ ��			 ��None
*******************************************************************************/
void USB_CMD_Handle(u8 command)
{
//	u8 ret=0;
	//��������У����Ƿ���ȷ,����ȷ���˳�
	if(USB_CMD_CheckSum()==VERIFY_SUM_ERROR)
	{
		USB_CMD_Respond(command,VERIFY_SUM_ERROR);	
		USB_Rec_Buffer_Flush();
		return;
	}
	switch(command)
	{
		case 0xB5://UpdateFW �̼�����
			//Update_Program_Handle();	
		break;
		default:
			USB_CMD_Respond(command,UNSUPPORT_ERROR);	//��֧�ָ��������
			break;
	}
	USB_Rec_Buffer_Flush();
}
/*******************************************************************************
* Function Name  : Check_USB_CMD_Input
* Description    : ���USB�Ƿ�����������
* Input          : None
* Return         : �յ�������
* Ӱ ��			 ��None
*******************************************************************************/
u8 Check_USB_CMD_Input(void)
{
	u16 len2;
	u16 len1;

	if((USB_OutBuffer[0]==CMD_HEADER1)&&(USB_OutBuffer[1]==CMD_HEADER2)&&(USB_OutBuffer[2]==CMD_HEADER3))	  //�������ͷ
	{
		len1	=	USB_OutBuffer[4];	//������
		len1	=	(len1<<8)|USB_OutBuffer[3];

		len2	=	USB_OutBuffer[6];	//�����ȷ���
		len2	=	(len2<<8)|USB_OutBuffer[5];
		len2	=	~len2;
		if((len1==len2))//��鳤�ȼ��䷴��
		{
			//ָ��ȳ���������������
			if(len1>=(USB_REC_BUFFER_SIZE-CMD_HEADER_TAIL_LENGTH))
			{
				USB_Rec_Buffer_Flush();	
			    return CMD_NONE;
			}
			if(USB_OutBuffer[len1+8]==CMD_TAIL)	//�������β
			{
			   	return USB_OutBuffer[CMD_HEADER_LENGTH];
			}
		}
	}
	//������ݸ�ʽ����ȷ,���
	return CMD_NONE;
}
/*******************************************************************************
* Function Name  : EP1_OUT_Callback.
* Description    : EP1 OUT Callback Routine.���豸�������ݻػ�
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
	if(cmd!=CMD_NONE) //����յ�������������д���
	{
	   //FeedWtchDog();
	   USB_CMD_Handle(cmd);
	}
}






