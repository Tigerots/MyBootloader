

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
* 功能描述: 软件复位
* 入口参数: 
* 函数返回: 
*****************************************************************************/
void my_soft_reset(void)
{
    System_Reset();
}


/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.28
* 功能描述: 启动初始化参数
* 入口参数: 
* 函数返回: 
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
                    MyIapRxBuff.WriteTime = OSTimeGet();//获取当前时间
                    MyIapRxBuff.WriteFlag = 2;
                }
                break;
            case 1://循环接收,写数据到缓存区
                if(MyIapRxBuff.WriteFlag == 2)
				{
					if(MyIapRxBuff.WriteStep == 1)
					{//接收完一组数据,写入
						Led_status = 2;//快闪
						MyIapRxBuff.WriteTime = OSTimeGet();//获取当前时间
						MyIapRxBuff.WriteStep = 2;//正在写入
						my_write_to_flash(MyIapRxBuff.WriteAddr, (uint8_t *)&MyIapRxBuff.WriteBuff[0], MyIapRxBuff.WriteSize);
						MyIapRxBuff.WriteAddr += MyIapRxBuff.WriteSize;
						MyIapRxBuff.WriteTatolCnt += MyIapRxBuff.WriteSize;
                        MyIapRxBuff.WriteSize = 0;
						MyIapRxBuff.WriteStep = 3;//写完成
					}
                    if(MyIapRxBuff.WriteStep == 3)
                    {//已经接收过数据,开始计时
                        if((OSTimeGet()-MyIapRxBuff.WriteTime) > 2000)
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
            case 2://置标志
				//固件写入完成,发送完最后一包通过协议设置,或通过超时时间设置
				MyIapFlag.UpdataFlag = 1;//更新升级标志
                MyIapFlag.FileSize = MyIapRxBuff.WriteTatolCnt;
				my_erase_write_to_flash(MyIapMap.ParaAddr-0x8008000, (u8 *)&MyIapFlag, sizeof(MyIapFlagType));
				iap_step = 3;
                break;
            case 3://重启
				my_soft_reset();//复位
                break;
            default:
                break;
        }
        OSTimeDlyHMSM(0, 0, 0, 100);
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
}


