

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


//������
void start_up_init(void)
{
    MyIapFlag.DevType = 0x12345;
    MyIapFlag.ChipType = 0x103;
    MyIapFlag.FileSize = 12040;
    MyIapFlag.FileSizeMax = 0x19000;
    MyIapFlag.FileSizeMin = 0x2000;
    MyIapFlag.UpdataFlag = 0;
    my_erase_write_to_flash(MyIapMap.ParaAddr, (u8 *)&MyIapFlag, sizeof(MyIapFlagType));
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
    uint8_t ret = 0;
    
    //start_up_init();//������
    while (1)
    {
        Led_status = 1;//����
        rt_thread_mdelay(5000);
        
        //��ȡ������־����
        my_read_from_flash(MyIapMap.ParaAddr, (uint8_t *)&MyIapFlag, sizeof(MyIapFlagType));
        //�ж�������־
        if(MyIapFlag.UpdataFlag == 1)
        {//��Ҫ����
            Led_status = 1;//����
            ret = iap_re_write_user_code();//ת�������̼�
            if(ret == 0)
            {
                Led_status = 0;//����
                MyIapFlag.UpdataFlag = 2;//ת�����,׼������
                my_erase_write_to_flash(MyIapMap.ParaAddr, (uint8_t *)&MyIapFlag, sizeof(MyIapFlagType));
                iap_jump_to_user_app();//��ת����
            }
            else
            {
                Led_status = 1;//����
                MyIapFlag.UpdataFlag = 0x55;//ת��ʧ��
                my_erase_write_to_flash(MyIapMap.ParaAddr, (uint8_t *)&MyIapFlag, sizeof(MyIapFlagType));
            }
            my_erase_write_to_flash(MyIapMap.ParaAddr, (uint8_t *)&MyIapFlag, sizeof(MyIapFlagType));
        }
        else
        {
            rt_hw_interrupt_disable();//���ж�(����,�������׳����쳣����)
            iap_jump_to_user_app();//��ת����
        }
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


