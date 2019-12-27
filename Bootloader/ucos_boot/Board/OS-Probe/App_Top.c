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
//�����ջ
static  OS_STK          AppTaskStartStk[APP_TASK_START_STK_SIZE];
static  OS_STK          AppTaskDetectStk[APP_TASK_Detect_STK_SIZE];
static  OS_STK          AppTaskCommStk[APP_TASK_Comm_STK_SIZE];

//ȫ�ֱ��� 
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
#define ApplicationAddress    0x8008000		//Ӧ�ó���Flash����ʼ��ַ



//LED����״ָ̬ʾ��
u32 LastRunTime;
void LedRun(u8 s)
{
	if( s==0 )
	{//��������״̬,����
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
	{//˫��
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
		{//��
			GPIO_ResetBits(GPIOC, GPIO_Pin_2);
		}
	}	
	else if( s==2 )
	{//����
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
	{//����
		GPIO_ResetBits(GPIOC, GPIO_Pin_2);
	}
}

//��ȡң���ź�״̬
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
//��ȡ����ź�״̬
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

    uSig = ~Sig;//Э���иߵ�ƽ��Ч
    return uSig;
}

/******************************************************************************************************
* Function Name  : AppTaskCreate
* Description    : ��������
* Input          : None
* Return         : None
******************************************************************************************************/
static  void  AppTaskCreate (void)
{
	 #if (OS_TASK_NAME_SIZE > 8)
	    CPU_INT08U      err;
	 #endif

     //��������,�����¶�,ʪ��,��ѹ���
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


//��ѯָ��Ӧ�����
//����: ���ݳ���
u8 AskQuery(u8 *SendBuff, u8 *DataBuff, u8 DataLen)
{
	unsigned short tCRC;
    u8 t;
    
    //�����͵�ַ
    t = MyProStruct.Source;
    MyProStruct.Source = MyProStruct.Object;
    MyProStruct.Object = t;
    //�������ݳ���
    MyProStruct.DataLen[0] = DataLen+2;
    MyProStruct.DataLen[1] = ~MyProStruct.DataLen[0];
    //�������ݰ�ͷ
    memcpy((u8 *)(&SendBuff[0]), (u8 *)(&MyProStruct.Start[0]), 11);
    //������������
    memcpy((u8 *)(&SendBuff[11]), (u8 *)(&DataBuff[0]), DataLen);
    //����CRC
    tCRC = QuickCRC16((u8 *)(&SendBuff[0]), MyProStruct.DataLen[0]+9);
    SendBuff[9+MyProStruct.DataLen[0]] = (u8)(tCRC&0xff);
    SendBuff[10+MyProStruct.DataLen[0]] = (u8)((tCRC>>8)&0xff);
    //��Ӱ�β
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
u8 SysStatus = 0;//ϵͳ����״̬
u8 SendRealDataSwitch = 0;//ʵʱ�����ϴ�ʹ��
u32 LastSendTimes = 0;//�ϴη���ʱ��


static  void  AppTaskStart (void *p_arg)
{//��������,���𴴽�����,��������,״̬��ʾ,�����Ȳ���
    
    u8 ret;
    
    (void)p_arg;

    BSP_Init();//�弶��ʼ��
	AppTaskCreate();//��������

	while (DEF_TRUE) 
	{
        LedRun(1);
        OSTimeDlyHMSM(0, 0, 0, 10); 
    }
}

//��������

u8 buff[256];
u8 EnCnt = 0;
#define nFirstFlag 0x01//���ǵ�һ��дFlash��־
void StartUp(void)
{
    BSP_Init();//�弶��ʼ��	
	
    
    ReadFromFlash(ApplicationAddress-FlashBassAddr, (u8 *)&buff, 100);
    
	ReadFromFlash(FirstWriteAddr, (u8 *)&EnCnt, 1);
	if(EnCnt != nFirstFlag)
	{//��һ����д����
		EnCnt = nFirstFlag;
		Write2Flash(FirstWriteAddr, (u8 *)&EnCnt, 2);
        
        
        //�豸������Ϣ
        DevBasicInfo.DevType = 0xF4;
        DevBasicInfo.HardVer = 0x10;
        DevBasicInfo.SoftVer[1] = 0x04;
        DevBasicInfo.SoftVer[0] = 0x10;
        DevBasicInfo.ProVer = 0x10;
        memcpy(DevBasicInfo.SN,"FST-2017112301",14);
        Write2Flash(DevBasicInfoAddr, (u8 *)&DevBasicInfo.DevType, sizeof(DevBasicInfo));
        
        //ʱ����
        DevRunInfo.SendGap = 100;
        DevRunInfo.WorkMode = 1;//���̼�϶
        DevRunInfo.ProType = 0;//FSTЭ��
        Write2Flash(DevRunInfoAddrAddr, (u8 *)&DevRunInfo.SendGap, sizeof(DevRunInfo));
	}
    else
    {
        ReadFromFlash(DevBasicInfoAddr, (u8 *)&DevBasicInfo.DevType, sizeof(DevBasicInfo));
        ReadFromFlash(DevRunInfoAddrAddr, (u8 *)&DevRunInfo.SendGap, sizeof(DevRunInfo));
    }
}
/**************************************************************************
//��������,�����¶�,ʪ��,��ѹ���

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


//��ת���û�����ִ��
void iap_jump_to_user_app(u32 app_address)
{
    //�ж��û������һ����ַ��ŵ�ջ����ַ�Ƿ�Ϸ�
    if (((*(vu32*)app_address) & 0x2FFE0000 ) == 0x20000000)
    {
        //�û�����ڶ�����ַ�ռ��ŵ��Ǹ�λ�ж�����(ִ�в���ת��main)
        JumpAddress = *(vu32*) (app_address + 4);
        //���ĵ�ַ����Ϊ����(��λ�жϷ�����)
		Jump_To_Application = (pFunction) JumpAddress;
		//���³�ʼ���û������ջ��ָ��
		__MSR_MSP(*(vu32*) app_address);
        //��ת���û�����(��λ�жϷ�����)��ִ��
		Jump_To_Application();
        //while(1);
    }
}

/**************************************************************************
//ͨ������,����λ������ͨ��
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





