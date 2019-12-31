
/**************************** (C) COPYRIGHT 2019 ****************************
* 设计师:   Tigerots
* 创建时间: 2019.10
* 功能描述: 主程序, 任务创建, 任务运行
*       
*****************************************************************************
* ********************************修改历史记录********************************
* 修改时间: 
* 版本号:
* 修改内容:
*****************************************************************************/


/*
*********************************************************************************************************
* INCLUDE FILES
*********************************************************************************************************
*/

#include <includes.h>
#include "app.h"
#include "Uart.h"
#include "cpu_flash.h"

#include "my_iap.h"

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



//LED工作状态指示灯
//u32 LastRunTime;
//void LedRun(u8 s)
//{
//	if( s==0 )
//	{//正常工作状态,慢闪
//		if(OSTimeGet()-LastRunTime >= 1000)
//		{
//			LastRunTime = OSTimeGet();
//		}
//		else if(OSTimeGet()-LastRunTime >= 100)
//		{
//			GPIO_SetBits(GPIOC, GPIO_Pin_2);
//		}
//		else
//		{
//			GPIO_ResetBits(GPIOC, GPIO_Pin_2);
//		}
//	}
//	else if( s==1 )
//	{//双闪
//		if(OSTimeGet()-LastRunTime >= 1000)
//		{
//			LastRunTime = OSTimeGet();
//		}
//		else if(OSTimeGet()-LastRunTime >= 300)
//		{
//			GPIO_SetBits(GPIOC, GPIO_Pin_2);
//		}
//		else if(OSTimeGet()-LastRunTime >= 200)
//		{
//			GPIO_ResetBits(GPIOC, GPIO_Pin_2);
//		}
//		else if(OSTimeGet()-LastRunTime >= 100)
//		{
//			GPIO_SetBits(GPIOC, GPIO_Pin_2);
//		}
//		else
//		{//亮
//			GPIO_ResetBits(GPIOC, GPIO_Pin_2);
//		}
//	}	
//	else if( s==2 )
//	{//快闪
//		if(OSTimeGet()-LastRunTime >= 200)
//		{
//			LastRunTime = OSTimeGet();
//		}
//		else if(OSTimeGet()-LastRunTime >= 100)
//		{
//			GPIO_SetBits(GPIOC, GPIO_Pin_2);
//		}
//		else
//		{
//			GPIO_ResetBits(GPIOC, GPIO_Pin_2);
//		}
//	}	
//	else
//	{//常亮
//		GPIO_ResetBits(GPIOC, GPIO_Pin_2);
//	}
//}

//获取遥控信号状态
//u8 GetTrigSig(void)
//{
//	u8 sig = 0;
//	
//	sig = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);
//	if( sig != 0)
//	{
//		OSTimeDlyHMSM(0, 0, 0, 10); 
//        sig = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4);
//        if( sig != 0)
//        {
//            return 0x01;
//        }
//	}
//    sig = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5);
//	if( sig != 0)
//	{
//		OSTimeDlyHMSM(0, 0, 0, 10); 
//        sig = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_5);
//        if( sig != 0)
//        {
//            return 0x02;
//        }
//	}
//    
//    sig = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
//	if( sig != 0)
//	{
//		OSTimeDlyHMSM(0, 0, 0, 10); 
//        sig = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6);
//        if( sig != 0)
//        {
//            return 0x04;
//        }
//	}
//    sig = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
//	if( sig != 0)
//	{
//		OSTimeDlyHMSM(0, 0, 0, 10); 
//        sig = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
//        if( sig != 0)
//        {
//            return 0x08;
//        }
//	}
//    sig = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4);
//	if( sig != 0)
//	{
//		OSTimeDlyHMSM(0, 0, 0, 10); 
//        sig = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_4);
//        if( sig != 0)
//        {
//            return 0x10;
//        }
//	}
//    sig = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5);
//	if( sig != 0)
//	{
//		OSTimeDlyHMSM(0, 0, 0, 10); 
//        sig = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_5);
//        if( sig != 0)
//        {
//            return 0x20;
//        }
//	}
//    sig = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);
//	if( sig != 0)
//	{
//		OSTimeDlyHMSM(0, 0, 0, 10); 
//        sig = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0);
//        if( sig != 0)
//        {
//            return 0x40;
//        }
//	}
//    sig = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
//	if( sig != 0)
//	{
//		OSTimeDlyHMSM(0, 0, 0, 10); 
//        sig = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1);
//        if( sig != 0)
//        {
//            return 0x80;
//        }
//	}
//	return 0;
//}
//获取光电信号状态
//u8 GetLightSig(void)
//{
//	u8 Sig = 0, uSig = 0;
//	
//	Sig =        GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_9) 
//            |   (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_8) << 1)
//            |   (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_7) << 2)
//            |   (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) << 3)
//            |   (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15) << 4)
//            |   (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14) << 5)
//            |   (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_13) << 6)
//            |   (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) << 7);

//    uSig = ~Sig;//协议中高电平有效
//    return uSig;
//}

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
        //LedRun(0);
        if(RxFlag == 1)//接收到数据
        {
            LastSendTimes = OSTimeGet();//收到信息后,延时一包发送
            
            memcpy( (u8 *)(&MyProStruct.Start[0]), (u8 *)(&Rxd1Buf[0]), Rx1Head-3);
            MyProStruct.Crc[0] = Rxd1Buf[Rx1Head-3];
            MyProStruct.Crc[1] = Rxd1Buf[Rx1Head-2];
            MyProStruct.End = Rxd1Buf[Rx1Head-1];
            RxFlag = 0;
            if(MyProStruct.ProVer == 0x10)
            {//协议版本号
                if(MyProStruct.DataType == 0x00)
                {//需要应答
                    switch(MyProStruct.Data.Cmd)
                    {
                        case 'K':
                        {
                            if(MyProStruct.Data.CmdSub == '0')
                            {//停止上传
                                SendRealDataSwitch = 0;
                            }
                            else if(MyProStruct.Data.CmdSub == '1')
                            {//开始上传
                                SendRealDataSwitch = 1;
                            }
                        }break;
                        case 'Q':
                        {
                            if(MyProStruct.Data.CmdSub == 'B')
                            {//查询基本信息
                                ret = AskQuery(Txd1Buf, (u8 *)(&DevBasicInfo.DevType), sizeof(DevBasicInfo));
                                MySendNByteU1(Txd1Buf, ret);
                                
                            }
                        }break;
                        case 'R':
                        {//查询命令
                            if(MyProStruct.Data.CmdSub == 'J')
                            {//继电器状态
                                
                            }
                            else if(MyProStruct.Data.CmdSub == 'M')
                            {//工作模式
                                ret = AskQuery(Txd1Buf, (u8 *)(&DevRunInfo.WorkMode), 1);
                                MySendNByteU1(Txd1Buf, ret);
                            }
                            else if(MyProStruct.Data.CmdSub == 'T')
                            {//通信时间间隔
                                ret = AskQuery(Txd1Buf, (u8 *)(&DevRunInfo.SendGap), 1);
                                MySendNByteU1(Txd1Buf, ret);
                            }
                            else if(MyProStruct.Data.CmdSub == 'P')
                            {//通信协议类型
                                ret = AskQuery(Txd1Buf, (u8 *)(&DevRunInfo.ProType), 1);
                                MySendNByteU1(Txd1Buf, ret);
                            }
                        }break;
                        case 'S':
                        {//设置命令
                            if(MyProStruct.Data.CmdSub == 'J')
                            {//继电器状态
                                if(DevRunInfo.WorkMode == 0)
                                {
                                    RelaySta1 = MyProStruct.Data.CmdData[0];//继电器1
                                    RelaySta2 = MyProStruct.Data.CmdData[1];//继电器2
                                    MyProStruct.DataType = 0x03;//0x03：应答完成（成功）
                                }
                                else
                                {
                                    MyProStruct.DataType = 0x04;//数据错误
                                }
                                ret = AskQuery(Txd1Buf, (u8 *)(&MyProStruct.Data.CmdData[0]), MyProStruct.DataLen[0]-2);
                                MySendNByteU1(Txd1Buf, ret);
                            }
                            else if(MyProStruct.Data.CmdSub == 'A')
                            {//设备地址
                                DevBasicInfo.DevType= MyProStruct.Data.CmdData[0];//设备地址
                                Write2Flash(DevBasicInfoAddr, (u8 *)&DevBasicInfo.DevType, sizeof(DevBasicInfo));
                                
                                MyProStruct.DataType = 0x03;//0x03：应答完成（成功）
                                ret = AskQuery(Txd1Buf, (u8 *)(&MyProStruct.Data.CmdData[0]), MyProStruct.DataLen[0]-2);
                                MySendNByteU1(Txd1Buf, ret);
                            }
                            else if(MyProStruct.Data.CmdSub == 'N')
                            {//生产序列号
                                memcpy(DevBasicInfo.SN, (u8 *)(&MyProStruct.Data.CmdData[0]), 14);//拷贝生产序列号
                                Write2Flash(DevBasicInfoAddr, (u8 *)&DevBasicInfo.DevType, sizeof(DevBasicInfo));
                                
                                MyProStruct.DataType = 0x03;//0x03：应答完成（成功）
                                ret = AskQuery(Txd1Buf, (u8 *)(&MyProStruct.Data.CmdData[0]), MyProStruct.DataLen[0]-2);
                                MySendNByteU1(Txd1Buf, ret);
                            }
                            else if(MyProStruct.Data.CmdSub == 'M')
                            {//工作模式
                                DevRunInfo.WorkMode = MyProStruct.Data.CmdData[0];
                                Write2Flash(DevRunInfoAddrAddr, (u8 *)&DevRunInfo.SendGap, sizeof(DevRunInfo));
                                MyProStruct.DataType = 0x03;//0x03：应答完成（成功）
                                ret = AskQuery(Txd1Buf, (u8 *)(&MyProStruct.Data.CmdData[0]), MyProStruct.DataLen[0]-2);
                                MySendNByteU1(Txd1Buf, ret);
                            }
                            else if(MyProStruct.Data.CmdSub == 'P')
                            {//协议类型
                                DevRunInfo.ProType = MyProStruct.Data.CmdData[0];
                                Write2Flash(DevRunInfoAddrAddr, (u8 *)&DevRunInfo.ProType, sizeof(DevRunInfo));
                                MyProStruct.DataType = 0x03;//0x03：应答完成（成功）
                                ret = AskQuery(Txd1Buf, (u8 *)(&MyProStruct.Data.CmdData[0]), MyProStruct.DataLen[0]-2);
                                MySendNByteU1(Txd1Buf, ret);
                            }
                            else if(MyProStruct.Data.CmdSub == 'T')
                            {//通信时间间隔
                                if(MyProStruct.Data.CmdData[0]>=10)
                                {
                                    DevRunInfo.SendGap = MyProStruct.Data.CmdData[0];
                                    Write2Flash(DevRunInfoAddrAddr, (u8 *)&DevRunInfo.SendGap, sizeof(DevRunInfo));
                                    MyProStruct.DataType = 0x03;//0x03：应答完成（成功）
                                    ret = AskQuery(Txd1Buf, (u8 *)(&MyProStruct.Data.CmdData[0]), MyProStruct.DataLen[0]-2);
                                    MySendNByteU1(Txd1Buf, ret);
                                }
                                else
                                {
                                    MyProStruct.DataType = 0x04;//0x04：数据错误，请求重发或终止
                                    ret = AskQuery(Txd1Buf, (u8 *)(&MyProStruct.Data.CmdData[0]), MyProStruct.DataLen[0]-2);
                                    MySendNByteU1(Txd1Buf, ret);
                                }
                            }
                        }break;
                        
                        default:
                            break;
                        
                    }
                }
                else
                {//不需要应答
                    
                }
            }
            else
            {//协议版本号
                
            }
            
        }
       
        if(DevRunInfo.ProType == 0)
        {//FST协议
            if(SendRealDataSwitch == 1)
            {//实时数据上传
                if((OSTimeGet()-LastSendTimes) >= DevRunInfo.SendGap)
                {
                    LastSendTimes = OSTimeGet();
                    Txd1Buf[0] = 0xEA;
                    Txd1Buf[1] = LightSta;//光电1
                    Txd1Buf[2] = 0;
                    Txd1Buf[3] = RemoteSta;//遥控1
                    Txd1Buf[4] = 0;
                    Txd1Buf[5] = RelaySta1;//继电器1
                    Txd1Buf[6] = RelaySta2;
                    Txd1Buf[7] = 0;//保留
                    Txd1Buf[8] = 0;
                    Txd1Buf[9] = 0xEB;
                    MySendNByteU1(Txd1Buf, 10);
                }
            }
        }
        else if(DevRunInfo.ProType == 1)
        {//新城光电协议
            //if(SendRealDataSwitch == 1)
            {//实时数据上传
                if((OSTimeGet()-LastSendTimes) >= DevRunInfo.SendGap)
                {
                    Txd1Buf[0] = 0x41;
                    Txd1Buf[1] = 0x44;
                    Txd1Buf[2] = ~LightSta;
                    Txd1Buf[3] = 0;
                    Txd1Buf[4] = 0;
                    Txd1Buf[5] = 0x43;
                    MySendNByteU1(Txd1Buf, 6);
                }
            }
        }
        OSTimeDlyHMSM(0, 0, 0, 10); 
    }
}

//启动任务
u8 EnCnt = 0;
#define nFirstFlag 0x01//不是第一次写Flash标志
void StartUp(void)
{
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
u8 OnOff = 0;
u8 LastKey = 0;
u32 LastTime = 0;
u32 CurrTime = 0;
static  void  AppTask_Detect (void *p_arg)
{
    u8 OutPut = 0;
	(void)p_arg;
    
    while (DEF_TRUE) 
	{
        OutPut = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_7);
        if(OutPut==0)
        {
            MyIapRxBuff.WriteFlag = 1;//启动烧写固件过程,通过协议或指令置位,此处为测试
            Led_status = 5;//常亮
        }
        RefreshDisplay();//刷新显示 
        OSTimeDlyHMSM(0,0,0,1);
	}
}
/**************************************************************************
//通信任务,与上位机进行通信

***************************************************************************/
extern void iap_main_entry(void *parameter);
static  void  AppTask_Communicate (void *p_arg)
{
//    u8 Light = 0;
    
	(void)p_arg;
    
    StartUp();
	iap_thread_init();
    while (DEF_TRUE) 
	{
        //MyWifiTask();//Wifi模块任务
        iap_main_entry(p_arg);
	    OSTimeDlyHMSM(0, 0, 0, 100);
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
  	NVIC_Configuration();
    BSP_IntDisAll(); 
  	RCC_Configuration(); 
    BSP_Init();//板级初始化	
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





