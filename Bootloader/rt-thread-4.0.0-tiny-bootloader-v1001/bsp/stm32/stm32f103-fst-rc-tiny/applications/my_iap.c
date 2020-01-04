

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
#define USE_UCOS
void iap_jump_to_user_app(void)
{
    u32 app_address = 0;

    //用户程序首地址
    app_address = MyIapMap.UserAddr;

    rt_hw_interrupt_disable();//关中断(必须,否则容易出现异常死机)
    
    #ifdef USE_UCOS //使用ucos系统必须使用如下语句,否则任务无法跳转
    __set_PSP(*((volatile unsigned long int *)app_address));// 重新设置进程PSP堆栈地址,UCOS用
    __set_CONTROL(0);//UCOS必须有
    #endif
    
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
* 功能描述: Led刷新线程
* 入口参数: 
* 函数返回: 
*****************************************************************************/
void led_display_entry(void *parameter)
{
    while (1)
    {
        LedRun();
        rt_thread_mdelay(10);
    }
}


//测试用
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

/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2020.01.04
* 功能描述: 串口1中断回调函数,用于转存接收到的数据
* 入口参数: 
* 函数返回: 
*****************************************************************************/
uint32_t rxcnt = 0;
static rt_err_t dev_iap_uart_func(rt_device_t dev, rt_size_t size)
{
    uint8_t ch = 0;
    
    //while(1)  
    {  
        //rt_device_write(dev_iap_uart, 0, at_cmd_at, sizeof(at_cmd_at));  
        while (rt_device_read(dev, -1, &ch, 1) == 1)
        {
            rxcnt++;
        }
        rt_thread_mdelay(1);  
    } 
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
    uint8_t ret = 0;
    
    //start_up_init();//测试用
    
    //定义iap串口设备
    rt_device_t  dev_iap_uart; 
    //查找iap串口设备
    dev_iap_uart = rt_device_find("uart1");  
    if(dev_iap_uart!=RT_NULL)  
    {
        //打开iap使用的串口设备
        rt_device_open(dev_iap_uart, RT_DEVICE_FLAG_INT_RX);  
        //设置串口接收中断回调函数
        rt_device_set_rx_indicate(dev_iap_uart, dev_iap_uart_func);
    } 
    
    
    while (1)
    {
        Led_status = 1;//快闪
        rt_thread_mdelay(40000);
        
        //读取升级标志数据
        my_read_from_flash(MyIapMap.ParaAddr, (uint8_t *)&MyIapFlag, sizeof(MyIapFlagType));
        //判断升级标志
        if(MyIapFlag.UpdataFlag == 1)
        {//需要升级
            Led_status = 1;//快闪
            ret = iap_re_write_user_code();//转存升级固件
            if(ret == 0)
            {
                Led_status = 0;//慢闪
                MyIapFlag.UpdataFlag = 2;//转存完成,准备启动
                my_erase_write_to_flash(MyIapMap.ParaAddr, (uint8_t *)&MyIapFlag, sizeof(MyIapFlagType));
                iap_jump_to_user_app();//跳转运行
            }
            else
            {
                Led_status = 1;//常亮
                MyIapFlag.UpdataFlag = 0x55;//转存失败
                my_erase_write_to_flash(MyIapMap.ParaAddr, (uint8_t *)&MyIapFlag, sizeof(MyIapFlagType));
            }
            my_erase_write_to_flash(MyIapMap.ParaAddr, (uint8_t *)&MyIapFlag, sizeof(MyIapFlagType));
        }
        else
        {
            iap_jump_to_user_app();//跳转运行
        }
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
    //my_iap_uart_init(115200);//初始化串口
        
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
                            512, 5, 10);
    // 如果获得线程控制块，启动这个线程
    if (tid != RT_NULL)
        rt_thread_startup(tid);
}


