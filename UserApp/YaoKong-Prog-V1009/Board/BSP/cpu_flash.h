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

//STM32F103RC��ַ�ռ� 0x08000000-0x08040000
#define FlashBassAddr (0x08040000-0x4000)	//�����洢,1ҳ

#define FirstWriteAddr   (0)//��һ�ζ�д��־��ַ
#define DevBasicInfoAddr (PAGE_SIZE*1)//������Ϣ��ַ
#define DevRunInfoAddrAddr      (PAGE_SIZE*2)//





#define UPDATE_FLAG_ADDR			0x8009000	//������־��
#define UPDATE_FLAG_SIZE			0x10		//������־��Сʵ��Ϊ4�ֽ�
#define UPDATE_DATA_ADDR			0x8009400	//��������������
#define UPDATE_DATE_SIZE			0x6400		//�����������ߴ�26K����




/* Define the STM32F10x hardware depending on the used board */
#define PAGE_SIZE                      (0x800)//103RC 2K/page
#define FLASH_SIZE                     (0x10000) /* 64K */

extern unsigned long System_Software_Version;

extern void 			User_Erase_Update_Data(void);
extern unsigned char User_Write_Update_Data(int addr,unsigned char  *pt,unsigned int  n);
extern void 			Write_Update_Finish_Flag(void);
extern  unsigned long 				Read_Update_Finish_Flag(void);
extern void Write_Software_Version_Flag(void);
extern  unsigned long Read_Software_Version_Flag(void);
extern void Get_Unique_Device_ID(unsigned char *ID_Buffer);
extern void CMD_UpdateFW_Handle(void);

extern void Write2Flash(unsigned long Eaddr, unsigned char *Edat, unsigned short Elen);
extern void ReadFromFlash(unsigned long Eaddr, unsigned char *Edat, unsigned short Elen);

#endif /* _CPU_FLASH_H */

/******************* (C) COPYRIGHT 2009 HuaYuan *****END OF FILE****/
