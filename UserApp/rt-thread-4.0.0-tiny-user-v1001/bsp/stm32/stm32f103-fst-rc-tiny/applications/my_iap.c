

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


//ָʾ��IO������
#define LD2_GPIO_PORT  GPIOC
#define LD2_PIN        GPIO_PIN_2

//���԰���IO����
#define KEY_GPIO_PORT  GPIOB //GPIOA
#define KEY_PIN        GPIO_PIN_15 //GPIO_PIN_7

//�����ȡϵͳtick����
#define iap_tick_get    rt_tick_get

//����iap�����豸
rt_device_t  dev_iap_uart; 

/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.28
* ��������: ��ʼ��map����
* ��ڲ���: 
* ��������: 
*****************************************************************************/
void iap_map_init(void)
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
* ��������: ��ʼ��ָʾ��IO��, ��ֲʱ��Ҫ�޸�
            1. �򿪶�Ӧ�˿�ʱ��
            2. ��ʼ������
* ��ڲ���: ��ĺ궨���ָʾ��IO
* ��������: 
*****************************************************************************/
void iap_gpio_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    //ָʾ�����ų�ʼ��
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Pin   = LD2_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LD2_GPIO_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(LD2_GPIO_PORT, LD2_PIN, GPIO_PIN_RESET);
    
    //���԰������ų�ʼ��
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin   = KEY_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStruct);
}
/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.28
* ��������: LED��������, ��ֲʱ��Ҫ�޸�
* ��ڲ���: 
* ��������: 
*****************************************************************************/
void Led_status_set(u8 x)   
{
    if(x==0)
    {//��
        HAL_GPIO_WritePin(LD2_GPIO_PORT, LD2_PIN, GPIO_PIN_RESET);
    }
    else
    {
        HAL_GPIO_WritePin(LD2_GPIO_PORT, LD2_PIN, GPIO_PIN_SET);
    }
}

/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.28
* ��������: ����״ָ̬ʾ������, ָʾ״̬����
                SLOW = 0,//����: 100ms��,����2S
                NORMAL,  //����: 100ms��,����1S
                FAST,    //����: 100ms��,����200mS
                DOUBLE,  //˫��: 100ms��*2,����1S
                THREEBLE,//����: 100ms��*3,����2S
                LIGHT_PER90, //����: 900ms��,����1S
                LIGHT_ON, //����: һֱ��
                LIGHT_OFF //����: һֱ��
* ��ڲ���: 
* ��������: 
*****************************************************************************/
uint32_t LastRunTime;
void LedRun(void)
{
	if( MyIapLedStatus == SLOW )
	{//����
		if(iap_tick_get()-LastRunTime >= 1900)
		{
			LastRunTime = iap_tick_get();
		}
		else if(iap_tick_get()-LastRunTime >= 100)
		{
			Led_status_set(1);
		}
		else
		{
			Led_status_set(0);
		}
	}
    else if( MyIapLedStatus ==  NORMAL)
	{//����
		if(iap_tick_get()-LastRunTime >= 900)
		{
			LastRunTime = iap_tick_get();
		}
		else if(iap_tick_get()-LastRunTime >= 100)
		{
			Led_status_set(1);
		}
		else
		{
			Led_status_set(0);
		}
	}
	else if( MyIapLedStatus ==  DOUBLE )
	{//˫��
		if(iap_tick_get()-LastRunTime >= 1000)
		{
			LastRunTime = iap_tick_get();
		}
		else if(iap_tick_get()-LastRunTime >= 300)
		{
			Led_status_set(1);
		}
		else if(iap_tick_get()-LastRunTime >= 200)
		{
			Led_status_set(0);
		}
		else if(iap_tick_get()-LastRunTime >= 100)
		{
			Led_status_set(1);
		}
		else
		{//��
			Led_status_set(0);
		}
	}	
    else if( MyIapLedStatus ==  THREEBLE )
	{//����
		if(iap_tick_get()-LastRunTime >= 2000)
		{
			LastRunTime = iap_tick_get();
		}
		else if(iap_tick_get()-LastRunTime >= 500)
		{
			Led_status_set(1);
		}
		else if(iap_tick_get()-LastRunTime >= 400)
		{
			Led_status_set(0);
		}
		else if(iap_tick_get()-LastRunTime >= 300)
		{
			Led_status_set(1);
		}
		else if(iap_tick_get()-LastRunTime >= 200)
		{
			Led_status_set(0);
		}
		else if(iap_tick_get()-LastRunTime >= 100)
		{
			Led_status_set(1);
		}
		else
		{//��
			Led_status_set(0);
		}
	}	
	else if( MyIapLedStatus ==  FAST )
	{//����
		if(iap_tick_get()-LastRunTime >= 200)
		{
			LastRunTime = iap_tick_get();
		}
		else if(iap_tick_get()-LastRunTime >= 100)
		{
			Led_status_set(1);
		}
		else
		{
			Led_status_set(0);
		}
	}	
    	
	else if( MyIapLedStatus ==  LIGHT_PER90 )
	{//����
		if(iap_tick_get()-LastRunTime >= 900)
		{
			LastRunTime = iap_tick_get();
		}
		else if(iap_tick_get()-LastRunTime >= 100)
		{
			Led_status_set(0);
		}
		else
		{
			Led_status_set(1);
		}
	}
	else if( MyIapLedStatus ==  LIGHT_ON )
	{//����
		Led_status_set(0);
	}
    else
    {//����
        Led_status_set(1);
    }
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
#define USE_UCOS

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;

void iap_jump_to_user_app(void)
{
    u32 app_address = 0;

    //�û������׵�ַ
    app_address = MyIapMap.UserAddr;

    rt_hw_interrupt_disable();//���ж�(����,�������׳����쳣����)
    
    #ifdef USE_UCOS //ʹ��ucosϵͳ����ʹ���������,���������޷���ת
    __set_PSP(*((volatile unsigned long int *)app_address));// �������ý���PSP��ջ��ַ,UCOS��
    __set_CONTROL(0);//UCOS������
    #endif
    
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
* ��������: ������λ
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
* ����ʱ��: 2020.01.04
* ��������: iap�ϵ��ʼ����������, ������״���д����, ��Ҫд��Ĭ�ϲ���
* ��ڲ���: 
* ��������: 
*****************************************************************************/
void start_up_init(void)
{
    //��ȡ�豸iap����
    my_read_from_flash(MyIapMap.ParaAddr, (uint8_t *)&MyIapFlag, sizeof(MyIapFlagType));
    if(MyIapFlag.IsInitFlag == 0xFF)
    {//��һ����д,δ��ʼ��
        MyIapFlag.DevType = 0x12345;
        MyIapFlag.ChipType = 0x103;
        MyIapFlag.FileSize = 12040;
        MyIapFlag.FileSizeMax = 0x19000;
        MyIapFlag.FileSizeMin = 0x2000;
        MyIapFlag.UpdataFlag = 0;//������־
        //д���豸iap����
        my_erase_write_to_flash(MyIapMap.ParaAddr, (u8 *)&MyIapFlag, sizeof(MyIapFlagType));
    }
}

/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2020.01.04
* ��������: ����1�жϻص�����,����ת����յ�������
* ��ڲ���: 
* ��������: 
*****************************************************************************/
uint32_t rxcnt = 0;
static rt_err_t dev_iap_uart_func(rt_device_t dev, rt_size_t size)
{
    rt_err_t ret;
    
    rxcnt++;
    return ret;
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
    uint8_t ch = 0;
    
    while (1)
    {
        LedRun();
        
        if( HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_PIN)==0 )
        {
            if(MyIapRxBuff.WriteFlag == 0)
            {
                MyIapRxBuff.WriteFlag = 1;//������д�̼�����,ͨ��Э���ָ����λ,�˴�Ϊ����
                Led_status = LIGHT_ON;//����
            }
        }

        if(MyIapRxBuff.WriteStep != 2)
        {
            while (rt_device_read(dev_iap_uart, -1, &ch, 1) == 1)
            {
                MyIapRxBuff.WriteTime = rt_tick_get();//��ȡ��ǰʱ��
                MyIapRxBuff.WriteBuff[MyIapRxBuff.WriteSize++] = ch;
                
                if(MyIapRxBuff.WriteSize >= 1024)
                {
                    MyIapRxBuff.WriteStep = 1;//��ʼд��
                    break;
                }  
            }
            if( (MyIapRxBuff.WriteSize!=0) && (rt_tick_get()-MyIapRxBuff.WriteTime)>2000)
            {
                MyIapRxBuff.WriteTime = rt_tick_get();//��ȡ��ǰʱ��
                MyIapRxBuff.WriteStep = 1;//��ʼд��
            }
        }
        rt_thread_mdelay(10);
    }
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
    rt_err_t ret = 0;
    
    Led_status = NORMAL;//����
    start_up_init();//Ӧ�ó����ϵ��ʼ��
    
    //����iap�����豸
    dev_iap_uart = rt_device_find("uart1");  
    if(dev_iap_uart != RT_NULL)  
    {
        //��iapʹ�õĴ����豸
        ret = rt_device_open(dev_iap_uart, RT_DEVICE_FLAG_INT_RX);  
        if(ret == RT_EOK)
        {
            //���ô��ڽ����жϻص�����
            rt_device_set_rx_indicate(dev_iap_uart, dev_iap_uart_func);
        }
        else
        {
            rt_kprintf("�򿪴����豸ʧ��...\r");
        }
    } 
    else
    {
        rt_kprintf("û�з��ִ����豸...\r");
    }
    
    
    while (1)
    {
        //�����ΪӦ���û�����ʹ��
        #ifndef USE_AS_BOOTLOADER
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
                    MyIapRxBuff.WriteTime = rt_tick_get();//��ȡ��ǰʱ��
                    MyIapRxBuff.WriteFlag = 2;
                }
                break;
            case 1://ѭ������,д���ݵ�������
                if(MyIapRxBuff.WriteFlag == 2)
				{
					if(MyIapRxBuff.WriteStep == 1)
					{//������һ������,д��
						Led_status = FAST;//����
                        
						MyIapRxBuff.WriteStep = 2;//����д��
						my_write_to_flash(MyIapRxBuff.WriteAddr, (uint8_t *)&MyIapRxBuff.WriteBuff[0], MyIapRxBuff.WriteSize);
						MyIapRxBuff.WriteAddr += MyIapRxBuff.WriteSize;
						MyIapRxBuff.WriteTatolCnt += MyIapRxBuff.WriteSize;
                        MyIapRxBuff.WriteSize = 0;
						MyIapRxBuff.WriteStep = 3;//д���
					}
                    if(MyIapRxBuff.WriteStep == 3)
                    {//�Ѿ����չ�����,��ʼ��ʱ
                        if((rt_tick_get()-MyIapRxBuff.WriteTime) > 10000)
                        {
                            MyIapRxBuff.WriteFlag = 3;
                            iap_step = 2;
                        }
                    }
				}
                break;
            case 2://�ñ�־
				//�̼�д�����,���������һ��ͨ��Э������,��ͨ����ʱʱ������
				MyIapFlag.UpdataFlag = 1;//����������־
                MyIapFlag.FileSize = MyIapRxBuff.WriteTatolCnt;
				my_erase_write_to_flash(MyIapMap.ParaAddr, (u8 *)&MyIapFlag, sizeof(MyIapFlagType));
				iap_step = 3;
                break; 
            case 3://����
				my_soft_reset();//��λ
                break;
            default:                 
                break;
        }

        //�����ΪBootloader����ʹ��
        #else
        MyIapLedStatus = FAST;//����
        rt_thread_mdelay(4000);
        
        //��ȡ������־����
        my_read_from_flash(MyIapMap.ParaAddr, (uint8_t *)&MyIapFlag, sizeof(MyIapFlagType));
        //�ж�������־
        if(MyIapFlag.UpdataFlag == 1)
        {//��Ҫ����
            Led_status = FAST;//����
            ret = iap_re_write_user_code();//ת�������̼�
            if(ret == 0)
            {
                Led_status = SLOW;//����
                MyIapFlag.UpdataFlag = 2;//ת�����,׼������
                my_erase_write_to_flash(MyIapMap.ParaAddr, (uint8_t *)&MyIapFlag, sizeof(MyIapFlagType));
                iap_jump_to_user_app();//��ת����
            }
            else
            {
                Led_status = ALLON;//����
                MyIapFlag.UpdataFlag = 0x55;//ת��ʧ��
                my_erase_write_to_flash(MyIapMap.ParaAddr, (uint8_t *)&MyIapFlag, sizeof(MyIapFlagType));
            }
            my_erase_write_to_flash(MyIapMap.ParaAddr, (uint8_t *)&MyIapFlag, sizeof(MyIapFlagType));
        }
        else
        {
            iap_jump_to_user_app();//��ת����
        }
        #endif
        rt_thread_mdelay(10);
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
    iap_map_init();//��ʼ��map
    iap_gpio_init();//��ʼ��ָʾ��
    //my_iap_uart_init(115200);//��ʼ������
        
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

