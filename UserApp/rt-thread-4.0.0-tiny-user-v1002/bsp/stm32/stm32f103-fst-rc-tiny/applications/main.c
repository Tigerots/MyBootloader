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
    //ʹ��iapģ��,ֻ����øó�ʼ������
    iap_thread_init();
    
    while(1)
    {
        /*
        �������������Լ��Ĺ���
        */
        rt_thread_mdelay(500);
    }
}
