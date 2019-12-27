/******************** (C) COPYRIGHT 2008 HuaYuan ********************
* File Name          : app.c
* Author             : Shirenwei
* Date First Issued  : 2/1/2014
* Description        : User Application.
********************************************************************************

*******************************************************************************/

/*
*********************************************************************************************************
* INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>
#include "app.h"
#include "Uart.h"
#include "cpu_flash.h"
#include <includes.h>
#include "usb_istr.h"

/*
*********************************************************************************************************
*                                      APPLICATION GLOBALS
*********************************************************************************************************
*/
//任务堆栈
static  OS_STK          AppTaskStartStk[APP_TASK_START_STK_SIZE];
static  OS_STK          AppTaskDetectStk[APP_TASK_Detect_STK_SIZE];
static  OS_STK          AppTaskCommStk[APP_TASK_Comm_STK_SIZE];

//全局变量 
/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/
static  void            AppTaskStart                 (void *p_arg);
static  void            AppTask_Detect               (void *p_arg);
static  void            AppTask_Communicate          (void *p_arg);
static  void  			AppTaskCreate				 (void);



typedef  void (*pFunction)(void);
pFunction Jump_To_Application;
u32 JumpAddress;
#define ApplicationAddress    0x8008000		//应用程序Flash的起始地址



//LED工作状态指示灯
u32 LastRunTime;
void LedRun(u8 s)
{
	if( s==0 )
	{//正常工作状态,慢闪
		if(OSTimeGet()-LastRunTime >= 1000)
		{
			LastRunTime = OSTimeGet();
		}
		else if(OSTimeGet()-LastRunTime >= 100)
		{
			GPIO_SetBits(GPIOC, GPIO_Pin_2);
		}
		else
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_2);
		}
	}
	else if( s==1 )
	{//双闪
		if(OSTimeGet()-LastRunTime >= 1000)
		{
			LastRunTime = OSTimeGet();
		}
		else if(OSTimeGet()-LastRunTime >= 300)
		{
			GPIO_SetBits(GPIOC, GPIO_Pin_2);
		}
		else if(OSTimeGet()-LastRunTime >= 200)
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_2);
		}
		else if(OSTimeGet()-LastRunTime >= 100)
		{
			GPIO_SetBits(GPIOC, GPIO_Pin_2);
		}
		else
		{//亮
			GPIO_ResetBits(GPIOC, GPIO_Pin_2);
		}
	}	
	else if( s==2 )
	{//快闪
		if(OSTimeGet()-LastRunTime >= 200)
		{
			LastRunTime = OSTimeGet();
		}
		else if(OSTimeGet()-LastRunTime >= 100)
		{
			GPIO_SetBits(GPIOC, GPIO_Pin_2);
		}
		else
		{
			GPIO_ResetBits(GPIOC, GPIO_Pin_2);
		}
	}	
	else
	{//常亮
		GPIO_ResetBits(GPIOC, GPIO_Pin_2);
	}
}

//获取遥控信号状态
u8 GetTrigSig(void)
{
	u8 sig = 0;
	
	sig = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);
	if( sig != 0)
	{
		OSTimeDlyHMSM(0, 0, 0, 10); 
        sig = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);
        if( sig != 0)
        {
            return 0x01;
        }
	}
    sig = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5);
	if( sig != 0)
	{
		OSTimeDlyHMSM(0, 0, 0, 10); 
        sig = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5);
        if( sig != 0)
        {
            return 0x02;
        }
	}
    
    sig = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
	if( sig != 0)
	{
		OSTimeDlyHMSM(0, 0, 0, 10); 
        sig = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
        if( sig != 0)
        {
            return 0x04;
        }
	}
    sig = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
	if( sig != 0)
	{
		OSTimeDlyHMSM(0, 0, 0, 10); 
        sig = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
        if( sig != 0)
        {
            return 0x08;
        }
	}
    sig = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4);
	if( sig != 0)
	{
		OSTimeDlyHMSM(0, 0, 0, 10); 
        sig = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4);
        if( sig != 0)
        {
            return 0x10;
        }
	}
    sig = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5);
	if( sig != 0)
	{
		OSTimeDlyHMSM(0, 0, 0, 10); 
        sig = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5);
        if( sig != 0)
        {
            return 0x20;
        }
	}
    sig = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);
	if( sig != 0)
	{
		OSTimeDlyHMSM(0, 0, 0, 10); 
        sig = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);
        if( sig != 0)
        {
            return 0x40;
        }
	}
    sig = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
	if( sig != 0)
	{
		OSTimeDlyHMSM(0, 0, 0, 10); 
        sig = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
        if( sig != 0)
        {
            return 0x80;
        }
	}
//    sig = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);
//	if( sig != 0)
//	{
//		OSTimeDlyHMSM(0, 0, 0, 10); 
//        sig = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10);
//        if( sig != 0)
//        {
//            return 9;
//        }
//	}
//    sig = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);
//	if( sig != 0)
//	{
//		OSTimeDlyHMSM(0, 0, 0, 10); 
//        sig = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);
//        if( sig != 0)
//        {
//            return 10;
//        }
//	}
    
    
	return 0;
}
//获取光电信号状态
u8 GetLightSig(void)
{
	u8 Sig = 0, uSig = 0;
	
	Sig =        GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9) 
            |   (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) << 1)
            |   (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) << 2)
            |   (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) << 3)
            |   (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) << 4)
            |   (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) << 5)
            |   (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) << 6)
            |   (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) << 7);

    uSig = ~Sig;//协议中高电平有效
    return uSig;
}

/******************************************************************************************************
* Function Name  : AppTaskCreate
* Description    : 创建任务
* Input          : None
* Return         : None
******************************************************************************************************/
static  void  AppTaskCreate (void)
{
	 #if (OS_TASK_NAME_SIZE > 8)
	    CPU_INT08U      err;
	 #endif

     //测试任务,用作温度,湿度,气压检测
	 OSTaskCreateExt(AppTask_Detect,
                (void *)0,
                (OS_STK *)&AppTaskDetectStk[APP_TASK_Detect_STK_SIZE - 1],
                APP_TASK_Detect_PRIO,
                APP_TASK_Detect_PRIO,
                (OS_STK *)&AppTaskDetectStk[0],
                APP_TASK_Detect_STK_SIZE,
                (void *)0,
                OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

	 OSTaskCreateExt(AppTask_Communicate,
                (void *)0,
                (OS_STK *)&AppTaskCommStk[APP_TASK_Comm_STK_SIZE - 1],
                APP_TASK_Comm_PRIO,
                APP_TASK_Comm_PRIO,
                (OS_STK *)&AppTaskCommStk[0],
                APP_TASK_Comm_STK_SIZE,
                (void *)0,
                OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
}


//查询指令应答组包
//返回: 数据长度
u8 AskQuery(u8 *SendBuff, u8 *DataBuff, u8 DataLen)
{
	unsigned short tCRC;
    u8 t;
    
    //整理发送地址
    t = MyProStruct.Source;
    MyProStruct.Source = MyProStruct.Object;
    MyProStruct.Object = t;
    //整理数据长度
    MyProStruct.DataLen[0] = DataLen+2;
    MyProStruct.DataLen[1] = ~MyProStruct.DataLen[0];
    //拷贝数据包头
    memcpy((u8 *)(&SendBuff[0]), (u8 *)(&MyProStruct.Start[0]), 11);
    //拷贝数据内容
    memcpy((u8 *)(&SendBuff[11]), (u8 *)(&DataBuff[0]), DataLen);
    //计算CRC
    tCRC = QuickCRC16((u8 *)(&SendBuff[0]), MyProStruct.DataLen[0]+9);
    SendBuff[9+MyProStruct.DataLen[0]] = (u8)(tCRC&0xff);
    SendBuff[10+MyProStruct.DataLen[0]] = (u8)((tCRC>>8)&0xff);
    //添加包尾
    SendBuff[11+MyProStruct.DataLen[0]] = 0x7E;
    
    return MyProStruct.DataLen[0]+12;
}
/******************************************************************************************************
* Function Name  : AppTaskStart 
* Description    : This is an example of a startup task.  As mentioned in the book's text, you MUST
*                  initialize the ticker only once multitasking has started.
* Input          : p_arg   is the argument passed to 'AppTaskStart()' by 'OSTaskCreate()'.
* Return         : None
* Notes          : The first line of code is used to prevent a compiler warning because 'p_arg' is not
*                  used.  The compiler should not generate any code for this statement.
******************************************************************************************************/
u8 SysStatus = 0;//系统工作状态
u8 SendRealDataSwitch = 0;//实时数据上传使能
u32 LastSendTimes = 0;//上次发送时间


static  void  AppTaskStart (void *p_arg)
{//监视任务,负责创建任务,操作按键,状态显示,升级等操作
    
    u8 ret;
    
    (void)p_arg;

    BSP_Init();//板级初始化
	AppTaskCreate();//创建任务

	while (DEF_TRUE) 
	{
        LedRun(1);
        OSTimeDlyHMSM(0, 0, 0, 10); 
    }
}

//启动任务

u8 buff[256];
u8 EnCnt = 0;
#define nFirstFlag 0x01//不是第一次写Flash标志
void StartUp(void)
{
    BSP_Init();//板级初始化	
	
    
    ReadFromFlash(ApplicationAddress-FlashBassAddr, (u8 *)&buff, 100);
    
	ReadFromFlash(FirstWriteAddr, (u8 *)&EnCnt, 1);
	if(EnCnt != nFirstFlag)
	{//第一次烧写程序
		EnCnt = nFirstFlag;
		Write2Flash(FirstWriteAddr, (u8 *)&EnCnt, 2);
        
        
        //设备基本信息
        DevBasicInfo.DevType = 0xF4;
        DevBasicInfo.HardVer = 0x10;
        DevBasicInfo.SoftVer[1] = 0x04;
        DevBasicInfo.SoftVer[0] = 0x10;
        DevBasicInfo.ProVer = 0x10;
        memcpy(DevBasicInfo.SN,"FST-2017112301",14);
        Write2Flash(DevBasicInfoAddr, (u8 *)&DevBasicInfo.DevType, sizeof(DevBasicInfo));
        
        //时间间隔
        DevRunInfo.SendGap = 100;
        DevRunInfo.WorkMode = 1;//底盘间隙
        DevRunInfo.ProType = 0;//FST协议
        Write2Flash(DevRunInfoAddrAddr, (u8 *)&DevRunInfo.SendGap, sizeof(DevRunInfo));
	}
    else
    {
        ReadFromFlash(DevBasicInfoAddr, (u8 *)&DevBasicInfo.DevType, sizeof(DevBasicInfo));
        ReadFromFlash(DevRunInfoAddrAddr, (u8 *)&DevRunInfo.SendGap, sizeof(DevRunInfo));
    }
}
/**************************************************************************
//测试任务,用作温度,湿度,气压检测

***************************************************************************/
static  void  AppTask_Detect (void *p_arg)
{
    u8 OutPut = 0;
	(void)p_arg;
    
    while (DEF_TRUE) 
	{
        
        OSTimeDlyHMSM(0, 0, 10, 10);
	}
}


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
		__MSR_MSP(*(vu32*) app_address);
        //跳转到用户程序(复位中断服务函数)并执行
		Jump_To_Application();
        //while(1);
    }
}

/**************************************************************************
//通信任务,与上位机进行通信
***************************************************************************/
static  void  AppTask_Communicate (void *p_arg)
{
    u8 Light = 0;
    OS_CPU_SR  cpu_sr = 0;
    
	(void)p_arg;
    
    StartUp();
    OSTimeDlyHMSM(0, 0, 5, 10);
    
    //NVIC_SETPRIMASK();
    //NVIC_SETFAULTMASK();
    __disable_irq();
    
    iap_jump_to_user_app(ApplicationAddress);
    while (DEF_TRUE) 
	{
	    OSTimeDlyHMSM(0, 0, 0, 10);
	}
}
/*******************************************************************************
* Function Name  : main() 
* Description    : This is the standard entry point for C code.  It is assumed that your code will call
*                  main() once you have performed all necessary initialization.
* Input          : None
* Return         : None
*******************************************************************************/
int  main (void)
{
    BSP_IntDisAll(); 
  	RCC_Configuration(); 
  	NVIC_Configuration();
    OSInit();
    OSTaskCreateExt(AppTaskStart,/* Create the start task*/
			        (void *)0,
			        (OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE - 1],
			        APP_TASK_START_PRIO,
			        APP_TASK_START_PRIO,
			        (OS_STK *)&AppTaskStartStk[0],
			        APP_TASK_START_STK_SIZE,
			        (void *)0,
			        OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);
    OSStart();
}





