
# 一. 仓库说明


- 该仓库存放STM32相关的IAP方式固件升级代码



# 二. 几种常见升级方式

- 详见<<单片机设备的几种常见升级方式.md>>


# 三. 笔记

## bootloader的主要操作

1. 校验栈顶地址
2. 读取复位中断向量地址
3. 关闭全局中断(使用ucos时,需增加PSP设置)
4. 跳转到复位中断服务函数执行
5. 如果升级失败,应具有直接烧写程序的功能

```c
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
```

## app升级部分的主要操作

1. MDK设置ROM地址和大小
2. 程序设置终端向量偏移
3. 生成HEX或Bin







