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
//����Flash����ʱ�õ��ĺ궨��

#define UPDATE_FLAG_ADDR			0x8009000	//������־��
#define UPDATE_FLAG_SIZE			0x10		//������־��Сʵ��Ϊ4�ֽ�

#define UPDATE_DATA_ADDR			0x8009400	//��������������
#define UPDATE_DATE_SIZE			0x6C00		//�����������ߴ�26K����

#define SOFTWARE_VERSION_ADDR		0x800FC00	//����汾��ַ
#define SOFTWARE_VERSION_SIZE		0x40		//������64�ֽ�
#define SOFTWARE_VERSION			0x1001		//����汾Ϊ1.0.0.1

#define UPDATE_FLAG_GO				0x01		//(Ӧ��)ȥ������־
#define UPDATE_FLAG_FINISHED		0xAA55		//������ɱ�־
#define FLASH_UPD_SEND_SIZE			48			//ÿ��������ֽ���,ӦΪ4�����������������
#define APP_DEVICE_TYPE_HEAD		0xEF0D0001	//Ӧ�ó��������豸���ͣ�DZG
#define APP_CHIP_TYPE_MASK			0xFFFE0000	//Ӧ�ó����ʺϵ�оƬ�ͺ�����
#define APP_MIN_SIZE				0x2000		//Ӧ�ó�����С�ߴ�8K
#define APP_MAX_SIZE				0x6C00		//Ӧ�ó������ߴ�26K


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
