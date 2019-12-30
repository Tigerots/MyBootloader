

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

#include <rthw.h>
#include <rtthread.h>
#include "stm32f1xx_hal.h"

#include "my_iap.h"
#include "my_type.h"
#include "my_flash.h"
#include "my_uart.h"




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
* ��������: ��ʼ��ָʾ��IO��
            1. �򿪶�Ӧ�˿�ʱ��
            2. ��ʼ������
* ��ڲ���: ��ĺ궨���ָʾ��IO
* ��������: 
*****************************************************************************/
void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitStruct.Pin   = LD2_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LD2_GPIO_PORT, &GPIO_InitStruct);
    
    HAL_GPIO_WritePin(LD2_GPIO_PORT, LD2_PIN, GPIO_PIN_RESET);
}

/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.28
* ��������: ��ת���û�������ִ��
* ��ڲ���: 
            1. MyIapMap.UserAddr //Ӧ�ó�����ʼ��ַ
            2. 
* ��������: 
*****************************************************************************/
void iap_jump_to_user_app(void)
{
    u32 app_address = 0;

    //�û������׵�ַ
    app_address = MyIapMap.UserAddr;
    
    //�ж��û������һ����ַ��ŵ�ջ����ַ�Ƿ�Ϸ�
    if (((*(vu32*)app_address) & 0x2FFE0000 ) == 0x20000000)
    {
        //�û�����ڶ�����ַ�ռ��ŵ��Ǹ�λ�ж�����(ִ�в���ת��main)
        JumpAddress = *(vu32*) (app_address + 4);
        //���ĵ�ַ����Ϊ����(��λ�жϷ�����)
		Jump_To_Application = (pFunction) JumpAddress;
		//���³�ʼ���û������ջ��ָ��
		__set_MSP(*(vu32*) app_address);
        //��ת���û�����(��λ�жϷ�����)��ִ��
		Jump_To_Application();
    }
}


/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.28
* ��������: ����״ָ̬ʾ������, ָʾ״̬����
            Led_status = 0. ����: 100ms��,900ms��, ��̬ 
            Led_status = 1. ˫��: 100ms��*2,700ms��, ����ת�����
            Led_status = 2. ����: 100ms��,100ms��, ���ڴ������
            Led_status = ����. ����: ����
* ��ڲ���: 
* ��������: 
*****************************************************************************/
uint32_t LastRunTime;
#define Led_status_set(x) HAL_GPIO_WritePin(LD2_GPIO_PORT, LD2_PIN, x)
void LedRun(void)
{
	if( Led_status==0 )
	{//��������״̬,����
		if(rt_tick_get()-LastRunTime >= 1000)
		{
			LastRunTime = rt_tick_get();
		}
		else if(rt_tick_get()-LastRunTime >= 100)
		{
			Led_status_set(GPIO_PIN_SET);
		}
		else
		{
			Led_status_set(GPIO_PIN_RESET);
		}
	}
	else if( Led_status==1 )
	{//˫��
		if(rt_tick_get()-LastRunTime >= 1000)
		{
			LastRunTime = rt_tick_get();
		}
		else if(rt_tick_get()-LastRunTime >= 300)
		{
			Led_status_set(GPIO_PIN_SET);
		}
		else if(rt_tick_get()-LastRunTime >= 200)
		{
			Led_status_set(GPIO_PIN_RESET);
		}
		else if(rt_tick_get()-LastRunTime >= 100)
		{
			Led_status_set(GPIO_PIN_SET);
		}
		else
		{//��
			Led_status_set(GPIO_PIN_RESET);
		}
	}	
	else if( Led_status==2 )
	{//����
		if(rt_tick_get()-LastRunTime >= 200)
		{
			LastRunTime = rt_tick_get();
		}
		else if(rt_tick_get()-LastRunTime >= 100)
		{
			Led_status_set(GPIO_PIN_SET);
		}
		else
		{
			Led_status_set(GPIO_PIN_RESET);
		}
	}	
	else
	{//����
		Led_status_set(GPIO_PIN_RESET);
	}
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
    __set_FAULTMASK(1); // �ر������ж�
    NVIC_SystemReset(); // ��λ
}

/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.28
* ��������: Ledˢ���߳�
* ��ڲ���: 
* ��������: 
*****************************************************************************/
void led_display_entry(void *parameter)
{
    while (1)
    {
        LedRun();
        rt_thread_mdelay(10);
    }
}


/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.28
* ��������: ������ʼ������
* ��ڲ���: 
* ��������: 
*****************************************************************************/
void start_up_init(void)
{
    MyIapFlag.DevType = 0x12345;
    MyIapFlag.ChipType = 0x103;
    MyIapFlag.FileSize = 12040;
    MyIapFlag.FileSizeMax = 0x19000;
    MyIapFlag.FileSizeMin = 0x2000;
    MyIapFlag.UpdataFlag = 0;
    //my_erase_write_to_flash(MyIapMap.ParaAddr, (u8 *)&MyIapFlag, sizeof(MyIapFlagType));
    my_read_from_flash(MyIapMap.ParaAddr, (u8 *)&MyIapFlag, sizeof(MyIapFlagType));
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
                MyIapRxBuff.WriteFlag = 1;//������д�̼�����,ͨ��Э���ָ����λ,�˴�Ϊ����
                if(MyIapRxBuff.WriteFlag == 1)
                {
                    iap_step = 1;
                    MyIapRxBuff.WriteStep = 0;
                    MyIapRxBuff.WriteTatolCnt = 0;
                    MyIapRxBuff.WriteAddr = MyIapMap.BuffAddr;//д���׵�ַ
                    my_erase_pages(MyIapMap.BuffAddr, MyIapMap.BuffSize);//ȫ����,��Ϊ������ʱ��
                }
                break;
            case 1://ѭ������,д���ݵ�������
                if(MyIapRxBuff.WriteFlag == 1)
				{
					if(MyIapRxBuff.WriteStep == 1)
					{//������һ������,д��
						Led_status = 2;//����
						MyIapRxBuff.WriteTime = rt_tick_get();//��ȡ��ǰʱ��
						MyIapRxBuff.WriteStep = 2;//����д��
						my_write_to_flash(MyIapRxBuff.WriteAddr, (uint8_t *)&MyIapRxBuff.WriteBuff[0], MyIapRxBuff.WriteSize);
						MyIapRxBuff.WriteAddr += MyIapRxBuff.WriteSize;
						MyIapRxBuff.WriteTatolCnt += MyIapRxBuff.WriteSize;
						MyIapRxBuff.WriteStep = 0;//д���
					}
					if((rt_tick_get()-MyIapRxBuff.WriteTime) > 2000)
					{//2Sδ�յ�����,����ʣ���ֽ�
						memcpy((uint8_t *)&MyIapRxBuff.WriteBuff[0], (uint8_t *)&USART_RX_BUF[0], USART_RX_CNT);
						MyIapRxBuff.WriteSize = USART_RX_CNT;
						USART_RX_CNT = 0;
						MyIapRxBuff.WriteStep = 2;//����д��
						my_write_to_flash(MyIapRxBuff.WriteAddr, (uint8_t *)&MyIapRxBuff.WriteBuff[0], MyIapRxBuff.WriteSize);
						MyIapRxBuff.WriteAddr += MyIapRxBuff.WriteSize;
						MyIapRxBuff.WriteTatolCnt += MyIapRxBuff.WriteSize;
						MyIapRxBuff.WriteStep = 0;//д���
						Led_status = 1;//����
					}
					if((rt_tick_get()-MyIapRxBuff.WriteTime) > 8000)
					{
						MyIapRxBuff.WriteFlag = 2;
					}
				}
                else if(MyIapRxBuff.WriteFlag == 2)
                {
					iap_step = 2;
                }
                break;
            case 2://�ñ�־
				//�̼�д�����,���������һ��ͨ��Э������,��ͨ����ʱʱ������
				MyIapFlag.UpdataFlag = 1;//����������־
				my_erase_write_to_flash(MyIapMap.ParaAddr, (u8 *)&MyIapFlag, sizeof(MyIapFlagType));
				iap_step = 3;
                break;
            case 3://����
				my_soft_reset();//��λ
                break;
            default:
                break;
        }
        rt_thread_mdelay(5);
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
    MX_GPIO_Init();//��ʼ��ָʾ��
    my_iap_uart_init(115200);//��ʼ������
        
    static rt_thread_t tid = RT_NULL;
	
	// �����߳�: ����, ����, ����, ��ջ, ���ȼ�, ʱ��Ƭ
    tid = rt_thread_create("th_led",
                            led_display_entry, RT_NULL,
                            256, 6, 10);
    // �������߳̿��ƿ飬��������߳�
    if (tid != RT_NULL)
        rt_thread_startup(tid);
    
    // �����߳�: ����, ����, ����, ��ջ, ���ȼ�, ʱ��Ƭ
    tid = rt_thread_create("th_iap",
                            iap_main_entry, RT_NULL,
                            512, 5, 10);
    // �������߳̿��ƿ飬��������߳�
    if (tid != RT_NULL)
        rt_thread_startup(tid);
}


