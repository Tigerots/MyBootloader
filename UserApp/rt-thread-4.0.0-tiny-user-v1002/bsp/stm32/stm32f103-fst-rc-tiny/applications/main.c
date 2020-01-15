/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "my_iap.h"


int main(void)
{
    //使用iap模块,只需调用该初始化函数
    iap_thread_init();
    
    while(1)
    {
        /*
        主函数内增加自己的功能
        */
        rt_thread_mdelay(500);
    }
}
