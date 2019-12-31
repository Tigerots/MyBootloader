

/**************************** (C) COPYRIGHT 2019 ****************************
* 设计师:   Tigerots
* 创建时间: 2019.12.28
* 功能描述: Iap相关函数,需要移植时直接将iap.c和iap.h拷贝到对应项目,修改
*           使用的芯片类型,并调用初始化和跳转函数
*       
*****************************************************************************
* ********************************修改历史记录********************************
* 修改时间: 
* 版本号:
* 修改内容:
*****************************************************************************/
#define __MY_IAP

#include <rthw.h>
#include <rtthread.h>
#include "stm32f1xx_hal.h"

#include "my_iap.h"
#include "my_type.h"
#include "my_flash.h"
#include "my_uart.h"




//指示灯IO口配置
#define LD2_GPIO_PORT  GPIOC
#define LD2_PIN        GPIO_PIN_2

//测试按键IO配置
#define KEY_GPIO_PORT  GPIOA
#define KEY_PIN        GPIO_PIN_7

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
uint32_t JumpAddress;


/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.28
* 功能描述: 初始化map变量
* 入口参数: 
* 函数返回: 
*****************************************************************************/
void Iap_Map_Init(void)
{
    MyIapMap.BootAddr = c_BootAddr;//bootloader起始地址
    MyIapMap.BootSize = c_BootSize;//bootloader占用内存大小
    MyIapMap.UserAddr = c_UserAddr;//应用程序起始地址
    MyIapMap.UserSize = c_UserSize;//应用程序占用内存大小
    MyIapMap.BuffAddr = c_BuffAddr;//升级代码缓存起始地址
    MyIapMap.BuffSize = c_BuffSize;//升级代码缓存占用内存大小
    MyIapMap.ParaAddr = c_ParaAddr;//用户参数区起始地址
    MyIapMap.ParaSize = c_ParaSize;//应用程序占用内存大小 
}

/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.28
* 功能描述: 初始化指示灯IO口
            1. 打开对应端口时钟
            2. 初始化引脚
* 入口参数: 需改宏定义的指示灯IO
* 函数返回: 
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
    
     __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStruct.Pin   = KEY_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(KEY_GPIO_PORT, &GPIO_InitStruct);
}

/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.28
* 功能描述: 跳转到用户程序区执行
* 入口参数: 
            1. MyIapMap.UserAddr //应用程序起始地址
            2. 
* 函数返回: 
*****************************************************************************/
void iap_jump_to_user_app(void)
{
    u32 app_address = 0;

    //用户程序首地址
    app_address = MyIapMap.UserAddr;
    
    //判断用户程序第一个地址存放的栈顶地址是否合法
    if (((*(vu32*)app_address) & 0x2FFE0000 ) == 0x20000000)
    {
        //用户程序第二个地址空间存放的是复位中断向量(执行并跳转到main)
        JumpAddress = *(vu32*) (app_address + 4);
        //将改地址声明为函数(复位中断服务函数)
		Jump_To_Application = (pFunction) JumpAddress;
		//重新初始化用户程序的栈顶指针
		__set_MSP(*(vu32*) app_address);
        //跳转到用户程序(复位中断服务函数)并执行
		Jump_To_Application();
    }
}


/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.28
* 功能描述: 运行状态指示灯驱动, 指示状态如下
            Led_status = 0. 慢闪: 100ms亮,900ms灭, 常态 
            Led_status = 1. 双闪: 100ms亮*2,700ms灭, 正在转存程序
            Led_status = 2. 快闪: 100ms亮,100ms灭, 正在传输程序
            Led_status = 其他. 常亮: 故障
* 入口参数: 
* 函数返回: 
*****************************************************************************/
uint32_t LastRunTime;
#define Led_status_set(x) HAL_GPIO_WritePin(LD2_GPIO_PORT, LD2_PIN, x)
void LedRun(void)
{
	if( Led_status==0 )
	{//正常工作状态,慢闪
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
	{//双闪
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
		{//亮
			Led_status_set(GPIO_PIN_RESET);
		}
	}	
	else if( Led_status==2 )
	{//快闪
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
	{//常亮
		Led_status_set(GPIO_PIN_RESET);
	}
}

/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.28
* 功能描述: 软件复位
* 入口参数: 
* 函数返回: 
*****************************************************************************/
void my_soft_reset(void)
{
    __set_FAULTMASK(1); // 关闭所有中断
    NVIC_SystemReset(); // 复位
}

/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.28
* 功能描述: Led刷新线程
* 入口参数: 
* 函数返回: 
*****************************************************************************/
void led_display_entry(void *parameter)
{
    while (1)
    {
        LedRun();
        if( HAL_GPIO_ReadPin(KEY_GPIO_PORT, KEY_PIN)==0 )
        {
            MyIapRxBuff.WriteFlag = 1;//启动烧写固件过程,通过协议或指令置位,此处为测试
            Led_status = 5;//常亮
        }
        rt_thread_mdelay(10);
    }
}


/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.28
* 功能描述: 启动初始化参数
* 入口参数: 
* 函数返回: 
*****************************************************************************/
void start_up_init(void)
{
//    MyIapFlag.DevType = 0x12345;
//    MyIapFlag.ChipType = 0x103;
//    MyIapFlag.FileSize = 12040;
//    MyIapFlag.FileSizeMax = 0x19000;
//    MyIapFlag.FileSizeMin = 0x2000;
//    MyIapFlag.UpdataFlag = 0;
//    my_erase_write_to_flash(MyIapMap.ParaAddr, (u8 *)&MyIapFlag, sizeof(MyIapFlagType));
    my_read_from_flash(MyIapMap.ParaAddr, (u8 *)&MyIapFlag, sizeof(MyIapFlagType));
}

/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.28
* 功能描述: iap主线程
* 入口参数: 
* 函数返回: 
*****************************************************************************/
uint8_t iap_step = 0;
void iap_main_entry(void *parameter)
{
    start_up_init();//测试用
    Led_status = 0;//慢闪
    while (1)
    {
        switch(iap_step)
        {
            case 0://启动
                if(MyIapRxBuff.WriteFlag == 1)
                {
                    iap_step = 1;
                    MyIapRxBuff.WriteStep = 0;
                    MyIapRxBuff.WriteSize = 0;
                    MyIapRxBuff.WriteTatolCnt = 0;
                    MyIapRxBuff.WriteAddr = MyIapMap.BuffAddr;//写入首地址
                    my_erase_pages(MyIapMap.BuffAddr, MyIapMap.BuffSize);//全擦掉,因为擦除费时间
                    MyIapRxBuff.WriteTime = rt_tick_get();//获取当前时间
                    MyIapRxBuff.WriteFlag = 2;
                }
                break;
            case 1://循环接收,写数据到缓存区
                if(MyIapRxBuff.WriteFlag == 2)
				{
                    if(USART_RX_CNT >= 1000)
                    {
                        MyIapRxBuff.WriteSize = USART_RX_CNT;
                        USART_RX_CNT = 0;
                        memcpy((uint8_t *)&MyIapRxBuff.WriteBuff[0], (uint8_t *)&USART_RX_BUF[0], MyIapRxBuff.WriteSize);
                        MyIapRxBuff.WriteStep = 1;
                    }
					if(MyIapRxBuff.WriteStep == 1)
					{//接收完一组数据,写入
						Led_status = 2;//快闪
						MyIapRxBuff.WriteTime = rt_tick_get();//获取当前时间
						MyIapRxBuff.WriteStep = 2;//正在写入
						my_write_to_flash(MyIapRxBuff.WriteAddr, (uint8_t *)&MyIapRxBuff.WriteBuff[0], MyIapRxBuff.WriteSize);
						MyIapRxBuff.WriteAddr += MyIapRxBuff.WriteSize;
						MyIapRxBuff.WriteTatolCnt += MyIapRxBuff.WriteSize;
                        MyIapRxBuff.WriteSize = 0;
						MyIapRxBuff.WriteStep = 3;//写完成
					}
                    if(MyIapRxBuff.WriteStep == 3)
                    {//已经接收过数据,开始计时
                        if((rt_tick_get()-MyIapRxBuff.WriteTime) > 2000)
                        {//2S未收到数据,拷贝剩余字节
                            if(USART_RX_CNT > 0)
                            {
                                memcpy((uint8_t *)&MyIapRxBuff.WriteBuff[0], (uint8_t *)&USART_RX_BUF[0], USART_RX_CNT);
                                MyIapRxBuff.WriteSize = USART_RX_CNT;
                                //USART_RX_CNT = 0;
                                MyIapRxBuff.WriteStep = 2;//正在写入
                                my_write_to_flash(MyIapRxBuff.WriteAddr, (uint8_t *)&MyIapRxBuff.WriteBuff[0], MyIapRxBuff.WriteSize);
                                MyIapRxBuff.WriteAddr += MyIapRxBuff.WriteSize;
                                MyIapRxBuff.WriteTatolCnt += MyIapRxBuff.WriteSize;
                                MyIapRxBuff.WriteStep = 3;//写完成
                                Led_status = 1;//慢闪
                            }
                        }
                        if((rt_tick_get()-MyIapRxBuff.WriteTime) > 20000)
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
            case 2://置标志
				//固件写入完成,发送完最后一包通过协议设置,或通过超时时间设置
				MyIapFlag.UpdataFlag = 1;//更新升级标志
                MyIapFlag.FileSize = MyIapRxBuff.WriteTatolCnt;
				my_erase_write_to_flash(MyIapMap.ParaAddr, (u8 *)&MyIapFlag, sizeof(MyIapFlagType));
				iap_step = 3;
                break;
            case 3://重启
				my_soft_reset();//复位
                break;
            default:
                break;
        }
        rt_thread_mdelay(5);
    }
}

/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.28
* 功能描述: 创建IAP相关线程
* 入口参数: 
* 函数返回: 
*****************************************************************************/
void iap_thread_init(void) 
{
    
    Iap_Map_Init();//初始化map
    MX_GPIO_Init();//初始化指示灯
    my_iap_uart_init(115200);//初始化串口
        
    static rt_thread_t tid = RT_NULL;
	
	// 创建线程: 名称, 函数, 参数, 堆栈, 优先级, 时间片
    tid = rt_thread_create("th_led",
                            led_display_entry, RT_NULL,
                            256, 6, 10);
    // 如果获得线程控制块，启动这个线程
    if (tid != RT_NULL)
        rt_thread_startup(tid);
    
    // 创建线程: 名称, 函数, 参数, 堆栈, 优先级, 时间片
    tid = rt_thread_create("th_iap",
                            iap_main_entry, RT_NULL,
                            2048, 5, 10);
    // 如果获得线程控制块，启动这个线程
    if (tid != RT_NULL)
        rt_thread_startup(tid);
}


