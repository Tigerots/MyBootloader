
/**************************** (C) COPYRIGHT 2019 ****************************
* ���ʦ:   Tigerots
* ����ʱ��: 2019.10
* ��������: ������, ���񴴽�, ��������
*       
*****************************************************************************
* ********************************�޸���ʷ��¼********************************
* �޸�ʱ��: 
* �汾��:
* �޸�����:
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



//LED����״ָ̬ʾ��
//u32 LastRunTime;
//void LedRun(u8 s)
//{
//	if( s==0 )
//	{//��������״̬,����
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
//	{//˫��
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
//		{//��
//			GPIO_ResetBits(GPIOC, GPIO_Pin_2);
//		}
//	}	
//	else if( s==2 )
//	{//����
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
//	{//����
//		GPIO_ResetBits(GPIOC, GPIO_Pin_2);
//	}
//}

//��ȡң���ź�״̬
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
//��ȡ����ź�״̬
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

//    uSig = ~Sig;//Э���иߵ�ƽ��Ч
//    return uSig;
//}

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
        //LedRun(0);
        if(RxFlag == 1)//���յ�����
        {
            LastSendTimes = OSTimeGet();//�յ���Ϣ��,��ʱһ������
            
            memcpy( (u8 *)(&MyProStruct.Start[0]), (u8 *)(&Rxd1Buf[0]), Rx1Head-3);
            MyProStruct.Crc[0] = Rxd1Buf[Rx1Head-3];
            MyProStruct.Crc[1] = Rxd1Buf[Rx1Head-2];
            MyProStruct.End = Rxd1Buf[Rx1Head-1];
            RxFlag = 0;
            if(MyProStruct.ProVer == 0x10)
            {//Э��汾��
                if(MyProStruct.DataType == 0x00)
                {//��ҪӦ��
                    switch(MyProStruct.Data.Cmd)
                    {
                        case 'K':
                        {
                            if(MyProStruct.Data.CmdSub == '0')
                            {//ֹͣ�ϴ�
                                SendRealDataSwitch = 0;
                            }
                            else if(MyProStruct.Data.CmdSub == '1')
                            {//��ʼ�ϴ�
                                SendRealDataSwitch = 1;
                            }
                        }break;
                        case 'Q':
                        {
                            if(MyProStruct.Data.CmdSub == 'B')
                            {//��ѯ������Ϣ
                                ret = AskQuery(Txd1Buf, (u8 *)(&DevBasicInfo.DevType), sizeof(DevBasicInfo));
                                MySendNByteU1(Txd1Buf, ret);
                                
                            }
                        }break;
                        case 'R':
                        {//��ѯ����
                            if(MyProStruct.Data.CmdSub == 'J')
                            {//�̵���״̬
                                
                            }
                            else if(MyProStruct.Data.CmdSub == 'M')
                            {//����ģʽ
                                ret = AskQuery(Txd1Buf, (u8 *)(&DevRunInfo.WorkMode), 1);
                                MySendNByteU1(Txd1Buf, ret);
                            }
                            else if(MyProStruct.Data.CmdSub == 'T')
                            {//ͨ��ʱ����
                                ret = AskQuery(Txd1Buf, (u8 *)(&DevRunInfo.SendGap), 1);
                                MySendNByteU1(Txd1Buf, ret);
                            }
                            else if(MyProStruct.Data.CmdSub == 'P')
                            {//ͨ��Э������
                                ret = AskQuery(Txd1Buf, (u8 *)(&DevRunInfo.ProType), 1);
                                MySendNByteU1(Txd1Buf, ret);
                            }
                        }break;
                        case 'S':
                        {//��������
                            if(MyProStruct.Data.CmdSub == 'J')
                            {//�̵���״̬
                                if(DevRunInfo.WorkMode == 0)
                                {
                                    RelaySta1 = MyProStruct.Data.CmdData[0];//�̵���1
                                    RelaySta2 = MyProStruct.Data.CmdData[1];//�̵���2
                                    MyProStruct.DataType = 0x03;//0x03��Ӧ����ɣ��ɹ���
                                }
                                else
                                {
                                    MyProStruct.DataType = 0x04;//���ݴ���
                                }
                                ret = AskQuery(Txd1Buf, (u8 *)(&MyProStruct.Data.CmdData[0]), MyProStruct.DataLen[0]-2);
                                MySendNByteU1(Txd1Buf, ret);
                            }
                            else if(MyProStruct.Data.CmdSub == 'A')
                            {//�豸��ַ
                                DevBasicInfo.DevType= MyProStruct.Data.CmdData[0];//�豸��ַ
                                Write2Flash(DevBasicInfoAddr, (u8 *)&DevBasicInfo.DevType, sizeof(DevBasicInfo));
                                
                                MyProStruct.DataType = 0x03;//0x03��Ӧ����ɣ��ɹ���
                                ret = AskQuery(Txd1Buf, (u8 *)(&MyProStruct.Data.CmdData[0]), MyProStruct.DataLen[0]-2);
                                MySendNByteU1(Txd1Buf, ret);
                            }
                            else if(MyProStruct.Data.CmdSub == 'N')
                            {//�������к�
                                memcpy(DevBasicInfo.SN, (u8 *)(&MyProStruct.Data.CmdData[0]), 14);//�����������к�
                                Write2Flash(DevBasicInfoAddr, (u8 *)&DevBasicInfo.DevType, sizeof(DevBasicInfo));
                                
                                MyProStruct.DataType = 0x03;//0x03��Ӧ����ɣ��ɹ���
                                ret = AskQuery(Txd1Buf, (u8 *)(&MyProStruct.Data.CmdData[0]), MyProStruct.DataLen[0]-2);
                                MySendNByteU1(Txd1Buf, ret);
                            }
                            else if(MyProStruct.Data.CmdSub == 'M')
                            {//����ģʽ
                                DevRunInfo.WorkMode = MyProStruct.Data.CmdData[0];
                                Write2Flash(DevRunInfoAddrAddr, (u8 *)&DevRunInfo.SendGap, sizeof(DevRunInfo));
                                MyProStruct.DataType = 0x03;//0x03��Ӧ����ɣ��ɹ���
                                ret = AskQuery(Txd1Buf, (u8 *)(&MyProStruct.Data.CmdData[0]), MyProStruct.DataLen[0]-2);
                                MySendNByteU1(Txd1Buf, ret);
                            }
                            else if(MyProStruct.Data.CmdSub == 'P')
                            {//Э������
                                DevRunInfo.ProType = MyProStruct.Data.CmdData[0];
                                Write2Flash(DevRunInfoAddrAddr, (u8 *)&DevRunInfo.ProType, sizeof(DevRunInfo));
                                MyProStruct.DataType = 0x03;//0x03��Ӧ����ɣ��ɹ���
                                ret = AskQuery(Txd1Buf, (u8 *)(&MyProStruct.Data.CmdData[0]), MyProStruct.DataLen[0]-2);
                                MySendNByteU1(Txd1Buf, ret);
                            }
                            else if(MyProStruct.Data.CmdSub == 'T')
                            {//ͨ��ʱ����
                                if(MyProStruct.Data.CmdData[0]>=10)
                                {
                                    DevRunInfo.SendGap = MyProStruct.Data.CmdData[0];
                                    Write2Flash(DevRunInfoAddrAddr, (u8 *)&DevRunInfo.SendGap, sizeof(DevRunInfo));
                                    MyProStruct.DataType = 0x03;//0x03��Ӧ����ɣ��ɹ���
                                    ret = AskQuery(Txd1Buf, (u8 *)(&MyProStruct.Data.CmdData[0]), MyProStruct.DataLen[0]-2);
                                    MySendNByteU1(Txd1Buf, ret);
                                }
                                else
                                {
                                    MyProStruct.DataType = 0x04;//0x04�����ݴ��������ط�����ֹ
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
                {//����ҪӦ��
                    
                }
            }
            else
            {//Э��汾��
                
            }
            
        }
       
        if(DevRunInfo.ProType == 0)
        {//FSTЭ��
            if(SendRealDataSwitch == 1)
            {//ʵʱ�����ϴ�
                if((OSTimeGet()-LastSendTimes) >= DevRunInfo.SendGap)
                {
                    LastSendTimes = OSTimeGet();
                    Txd1Buf[0] = 0xEA;
                    Txd1Buf[1] = LightSta;//���1
                    Txd1Buf[2] = 0;
                    Txd1Buf[3] = RemoteSta;//ң��1
                    Txd1Buf[4] = 0;
                    Txd1Buf[5] = RelaySta1;//�̵���1
                    Txd1Buf[6] = RelaySta2;
                    Txd1Buf[7] = 0;//����
                    Txd1Buf[8] = 0;
                    Txd1Buf[9] = 0xEB;
                    MySendNByteU1(Txd1Buf, 10);
                }
            }
        }
        else if(DevRunInfo.ProType == 1)
        {//�³ǹ��Э��
            //if(SendRealDataSwitch == 1)
            {//ʵʱ�����ϴ�
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

//��������
u8 EnCnt = 0;
#define nFirstFlag 0x01//���ǵ�һ��дFlash��־
void StartUp(void)
{
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
            MyIapRxBuff.WriteFlag = 1;//������д�̼�����,ͨ��Э���ָ����λ,�˴�Ϊ����
            Led_status = 5;//����
        }
        RefreshDisplay();//ˢ����ʾ 
        OSTimeDlyHMSM(0,0,0,1);
	}
}
/**************************************************************************
//ͨ������,����λ������ͨ��

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
        //MyWifiTask();//Wifiģ������
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
    BSP_Init();//�弶��ʼ��	
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





