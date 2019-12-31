

/**************************** (C) COPYRIGHT 2019 ****************************
* ���ʦ:   Tigerots
* ����ʱ��: 2019.12.28
* ��������: Iap��غ���,��Ҫ��ֲʱֱ�ӽ�iap.c��iap.h��������Ӧ��Ŀ,�޸�
*           ʹ�õ�оƬ����,�����ó�ʼ������ת����
*       
*****************************************************************************
* ********************************�޸���ʷ��¼********************************
* �޸�ʱ��: 
* �汾��:
* �޸�����:
*****************************************************************************/
#define __MY_IAP

#ifdef USE_RTTHREAD
#include <rthw.h>
#include <rtthread.h>
#include "stm32f1xx_hal.h"

#include "my_type.h"
#include "my_flash.h"
#include "my_uart.h"
#else

#include <includes.h>
#include "cpu_flash.h"

#define uint32_t u32
#define uint16_t u16
#define uint8_t u8

#endif

#include "my_iap.h"



//ָʾ��IO������
#define LD2_GPIO_PORT  GPIOC
#define LD2_PIN        GPIO_PIN_2

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;


/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.28
* ��������: ��ʼ��map����
* ��ڲ���: 
* ��������: 
*****************************************************************************/
void Iap_Map_Init(void)
{
    MyIapMap.BootAddr = c_BootAddr;//bootloader��ʼ��ַ
    MyIapMap.BootSize = c_BootSize;//bootloaderռ���ڴ��С
    MyIapMap.UserAddr = c_UserAddr;//Ӧ�ó�����ʼ��ַ
    MyIapMap.UserSize = c_UserSize;//Ӧ�ó���ռ���ڴ��С
    MyIapMap.BuffAddr = c_BuffAddr;//�������뻺����ʼ��ַ
    MyIapMap.BuffSize = c_BuffSize;//�������뻺��ռ���ڴ��С
    MyIapMap.ParaAddr = c_ParaAddr;//�û���������ʼ��ַ
    MyIapMap.ParaSize = c_ParaSize;//Ӧ�ó���ռ���ڴ��С 
}


/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.28
* ��������: �����λ
* ��ڲ���: 
* ��������: 
*****************************************************************************/
void my_soft_reset(void)
{
    System_Reset();
}


/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.28
* ��������: ������ʼ������
* ��ڲ���: 
* ��������: 
*****************************************************************************/
#define     my_read_from_flash  ReadFromFlash
#define     my_write_to_flash   User_Write_Update_Data
#define     my_erase_write_to_flash     Write2Flash
void start_up_init(void)
{
//    MyIapFlag.DevType = 0x12345;
//    MyIapFlag.ChipType = 0x103;
//    MyIapFlag.FileSize = 12040;
//    MyIapFlag.FileSizeMax = 0x19000;
//    MyIapFlag.FileSizeMin = 0x2000;
//    MyIapFlag.UpdataFlag = 0;
//    my_erase_write_to_flash(MyIapMap.ParaAddr, (u8 *)&MyIapFlag, sizeof(MyIapFlagType));
    my_read_from_flash(MyIapMap.ParaAddr-0x8008000, (u8 *)&MyIapFlag, sizeof(MyIapFlagType));
}

/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.28
* ��������: iap���߳�
* ��ڲ���: 
* ��������: 
*****************************************************************************/
uint8_t iap_step = 0;
void iap_main_entry(void *parameter)
{
    start_up_init();//������
    Led_status = 0;//����
    while (1)
    {
        switch(iap_step)
        {
            case 0://����
                if(MyIapRxBuff.WriteFlag == 1)
                {
                    iap_step = 1;
                    MyIapRxBuff.WriteStep = 0;
                    MyIapRxBuff.WriteSize = 0;
                    MyIapRxBuff.WriteTatolCnt = 0;
                    MyIapRxBuff.WriteAddr = MyIapMap.BuffAddr;//д���׵�ַ
                    my_erase_pages(MyIapMap.BuffAddr, MyIapMap.BuffSize);//ȫ����,��Ϊ������ʱ��
                    MyIapRxBuff.WriteTime = OSTimeGet();//��ȡ��ǰʱ��
                    MyIapRxBuff.WriteFlag = 2;
                }
                break;
            case 1://ѭ������,д���ݵ�������
                if(MyIapRxBuff.WriteFlag == 2)
				{
					if(MyIapRxBuff.WriteStep == 1)
					{//������һ������,д��
						Led_status = 2;//����
						MyIapRxBuff.WriteTime = OSTimeGet();//��ȡ��ǰʱ��
						MyIapRxBuff.WriteStep = 2;//����д��
						my_write_to_flash(MyIapRxBuff.WriteAddr, (uint8_t *)&MyIapRxBuff.WriteBuff[0], MyIapRxBuff.WriteSize);
						MyIapRxBuff.WriteAddr += MyIapRxBuff.WriteSize;
						MyIapRxBuff.WriteTatolCnt += MyIapRxBuff.WriteSize;
                        MyIapRxBuff.WriteSize = 0;
						MyIapRxBuff.WriteStep = 3;//д���
					}
                    if(MyIapRxBuff.WriteStep == 3)
                    {//�Ѿ����չ�����,��ʼ��ʱ
                        if((OSTimeGet()-MyIapRxBuff.WriteTime) > 2000)
                        {//2Sδ�յ�����,����ʣ���ֽ�
                            if(USART_RX_CNT > 0)
                            {
                                memcpy((uint8_t *)&MyIapRxBuff.WriteBuff[0], (uint8_t *)&USART_RX_BUF[0], USART_RX_CNT);
                                MyIapRxBuff.WriteSize = USART_RX_CNT;
                                //USART_RX_CNT = 0;
                                MyIapRxBuff.WriteStep = 2;//����д��
                                my_write_to_flash(MyIapRxBuff.WriteAddr, (uint8_t *)&MyIapRxBuff.WriteBuff[0], MyIapRxBuff.WriteSize);
                                MyIapRxBuff.WriteAddr += MyIapRxBuff.WriteSize;
                                MyIapRxBuff.WriteTatolCnt += MyIapRxBuff.WriteSize;
                                MyIapRxBuff.WriteStep = 3;//д���
                                Led_status = 1;//����
                            }
                        }
                        if((OSTimeGet()-MyIapRxBuff.WriteTime) > 20000)
                        {
                            MyIapRxBuff.WriteFlag = 3;
                        }
                    }
				}
                else if(MyIapRxBuff.WriteFlag == 3)
                {
					iap_step = 2;
                }
                break;
            case 2://�ñ�־
				//�̼�д�����,���������һ��ͨ��Э������,��ͨ����ʱʱ������
				MyIapFlag.UpdataFlag = 1;//����������־
                MyIapFlag.FileSize = MyIapRxBuff.WriteTatolCnt;
				my_erase_write_to_flash(MyIapMap.ParaAddr-0x8008000, (u8 *)&MyIapFlag, sizeof(MyIapFlagType));
				iap_step = 3;
                break;
            case 3://����
				my_soft_reset();//��λ
                break;
            default:
                break;
        }
        OSTimeDlyHMSM(0, 0, 0, 100);
    }
}
/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.28
* ��������: ����IAP����߳�
* ��ڲ���: 
* ��������: 
*****************************************************************************/
void iap_thread_init(void) 
{
    Iap_Map_Init();//��ʼ��map
}


