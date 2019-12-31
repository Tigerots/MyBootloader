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


//*********�궨��************************************
//���¶���������־
#define UPD_START_FLAG			1			//��ʼ������־
#define UPD_NORMAL_FLAG			2			//����������־
#define UPD_END_FLAG			3			//����������־

//****������USBͨ���й�******************************

#define USB_REC_BUFFER_SIZE		64			//���ջ��峤��
#define REC_BUFFER_SIZE_REP		0x40			//���ջ��峤��,��������ʱʹ��

#define USB_SEND_BUFFER_SIZE	64			//���ͻ��峤��
#define SEND_BUFFER_SIZE_REP 	0x40			//���ͻ��峤��,��������ʱʹ��

//#define LenIs1
#ifdef	LenIs1
	#define CMD_HEADER_LENGTH		0x05		//����ͷ����,��������ͷ+����ȼ�У����   0x07
	#define CMD_HEADER_TAIL_LENGTH 	0x07		//����ͷ��β���ܳ���:��������ͷ+����ȼ�У����+У����+����β	0x09
#else
	#define CMD_HEADER_LENGTH		0x07		//����ͷ����,��������ͷ+����ȼ�У����   0x07
	#define CMD_HEADER_TAIL_LENGTH 	0x09		//����ͷ��β���ܳ���:��������ͷ+����ȼ�У����+У����+����β	0x09
#endif

#define CMD_HEADER1			0xA2			//USBͨ������ͷ��һ�ֽ�
#define	CMD_HEADER2			0x13			//USBͨ������ͷ�ڶ��ֽ�
#define CMD_HEADER3			0xB4			//USBͨ������ͷ�����ֽ�
#define CMD_TAIL			0x00			//USBͨ������β


#define CMD_NONE			0x00			//������
#define CMD_Read_Status		0xC3			//��״̬
#define CMD_Disp_One_Line	0xC4			//��ʾһ������
#define CMD_Disp_All		0xC5			//��ʾ����������
#define CMD_LED_Clear		0xC6			//����ʾ����
#define CMD_Update_Program	0xB5			//�̼�����
		#define CMD_UpdateFW			0xB5		//�̼�����
	    #define UPD_START_FLAG			1			//��ʼ������־
		#define UPD_NORMAL_FLAG			2			//����������־
		#define UPD_END_FLAG			3			//����������
		#define CMD_UPD_FILE_SIZE_ERROR	0x14	//�����ļ���С����
		#define CMD_UPD_FILE_ERROR		0x15	//�����ļ���ʽ����
		#define CMD_UPD_CHECKSUM_ERROR	0x16	//�����ļ�У��Ͳ���
		#define UPD_FLAG_ERROR			0x17	//������־����
		#define UPD_LENGTH_ERROR		0x18	//�������ݳ��ȴ���
		#define FILE_WRITE_ERROR		0x19		//д�ļ�����



#define FLASH_OK			CMD_OK
#define FLASH_ERROR			1

#define CMD_OK				0
#define CMD_FAIL			1
#define CMD_TIMEOUT			1
#define UNSUPPORT_ERROR		2				//��֧�ָ�����
#define VERIFY_SUM_ERROR		0x05		//����ʹ���
#define LENGTH_VERIFY_SUM_ERROR	0x06		//����У��ʹ���
#define CMD_PARAM_ERROR			0x07		//���������ʽ����

#define UPDATE_FILE_SIZE_ERROR		0x14		//�����ļ���С����
#define UPDATE_FILE_FORMAT_ERROR 	0x15		//�����ļ���ʽ������Ч�����ļ�
#define UPDATE_FILE_WRITE_ERROR		0x16		//�����ļ�д����
#define UPDATE_FILE_SUM_ERROR		0x18		//�����ļ�У��ʹ���

#define SETTED		1	//�Ѿ�����
#define UNSETTED	0	//û������



void Read_Device_Status(u8 Interface_Type);

void Check_USB_Handle_Task(void);
u8 Check_USB_CMD_Input(void);
void USB_CMD_Handle(u8 command);
void USB_Module_Handle(void);
u8 System_PowerUp(void);



#endif

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
