/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-11-05     yangjie      First edition
 */

#include "stm32f1xx_hal.h"
#include <rtthread.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;
typedef volatile unsigned long  vu32;

#define LD2_GPIO_PORT  GPIOC
#define LD2_PIN        GPIO_PIN_2

static void MX_GPIO_Init(void);

typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
u32 JumpAddress;


//跳转到用户程序执行
void iap_jump_to_user_app(u32 app_address)
{
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


#define ApplicationAddress    0x8008000

int main(void)
{
    int i;
    
    MX_GPIO_Init();

    while (1)
    {
        for(i=0; i<10; i++)
        {
            HAL_GPIO_WritePin(LD2_GPIO_PORT, LD2_PIN, GPIO_PIN_SET);
            rt_thread_mdelay(100);
            HAL_GPIO_WritePin(LD2_GPIO_PORT, LD2_PIN, GPIO_PIN_RESET);
            rt_thread_mdelay(100);
        }
        iap_jump_to_user_app(ApplicationAddress);
    }
}

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOC_CLK_ENABLE();
    HAL_GPIO_WritePin(LD2_GPIO_PORT, LD2_PIN, GPIO_PIN_RESET);

    GPIO_InitStruct.Pin   = LD2_PIN;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LD2_GPIO_PORT, &GPIO_InitStruct);
}
