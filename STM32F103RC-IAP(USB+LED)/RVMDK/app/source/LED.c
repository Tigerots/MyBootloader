/****************************************Copyright (c)**************************************************
**                                 HuaYuan Co.,LTD.
**                                     
**                             http://www.goldenkey.com.cn
**
**--------------File Info-------------------------------------------------------------------------------
** File name:			led.c
** Last modified Date:	2010-07-02
** Last Version:	V1.0
** Descriptions:	LEDָʾ�Ƽ��������ʾ����
**
********************************************************************************************************/

#include "stm32f10x_lib.h"
#include "led.h"
#include "stdlib.h"
#include "string.h"

#define LightLevel 1


#define GPIOB_TM1620_DIN	  GPIO_Pin_9	 /* GPIOB Pin 9	   */ 
#define GPIOB_TM1620_SCK	  GPIO_Pin_8	 /* GPIOB Pin 8    */ 
#define GPIOB_TM1620_CS		  GPIO_Pin_11	 /* GPIOC Pin 11   */ 


u8 	const Disp_Buf[16] = {LED_NUMBER_0,LED_NUMBER_1,LED_NUMBER_2,LED_NUMBER_3,LED_NUMBER_4,LED_NUMBER_5,LED_NUMBER_6,LED_NUMBER_7,LED_NUMBER_8,LED_NUMBER_9,LED_NUMBER_A,LED_NUMBER_B,LED_NUMBER_C,LED_NUMBER_d,LED_NUMBER_E,LED_NUMBER_F};
u8 	LED_Display_Cache[10];	//LED��ʾ����
u8  DispFlashBuf[10];//LED��ʾ����

u8 LED_Seg_Blink_Flag;	//LED�������˸��־
u32 LED_Seg_Blink_Week;	//LED�������˸����
u32 LED_Seg_Blink_Reason;//LED��˸ԭ��


u8 LED_Seg_Light_Flag;		//LED���������־
u16 LED_GIF_Display_Index;		//LED_GIF��ʾ����
u32 Last_GIF_Display_Time;	//���GIF������ʾʱ��


//����ʱ
extern void Delay (int cnt);
/*******************************************************************************
* Function Name  : TM1620_Init
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TM1620_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin  = GPIOB_TM1620_SCK|GPIOB_TM1620_DIN;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = GPIOB_TM1620_CS;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);

	TM_Display_Light_Off();
}


/*******************************************************************************
* Function Name  : TM_Send_Byte
* Description    : TM1620��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TM_Send_Byte(u8 dat)
{
u8 i;
	
	GPIO_ResetBits(GPIOC, GPIOB_TM1620_CS);
	Delay(20);
	
	for(i=0;i<8;i++)
	{
		GPIO_ResetBits(GPIOB, GPIOB_TM1620_SCK);
		Delay(30);
		if(dat&1)
			GPIO_SetBits(GPIOB, GPIOB_TM1620_DIN);
		else
			GPIO_ResetBits(GPIOB, GPIOB_TM1620_DIN);
		Delay(20);
		GPIO_SetBits(GPIOB, GPIOB_TM1620_SCK);
		Delay(20);
		dat >>= 1;
	}
}
// ������ʾ num ���� 
void TM_MDis(u8 addr,u8 num,u8 *dat)
{
u8 _data;

	GPIO_SetBits(GPIOC, GPIOB_TM1620_CS);
	GPIO_SetBits(GPIOB, GPIOB_TM1620_SCK);
	GPIO_SetBits(GPIOB, GPIOB_TM1620_DIN);

	TM_Send_Byte(0x02); //��ʾģʽ���ã�����Ϊ6��GRID��8��SEG

	GPIO_SetBits(GPIOC, GPIOB_TM1620_CS);
	TM_Send_Byte(TM_CMD1_INC); //д���ݵ���ʾ�Ĵ��������õ�ַ�Զ���һģʽ 

	GPIO_SetBits(GPIOC, GPIOB_TM1620_CS);
	TM_Send_Byte(TM_CMD2 | (addr&0x0f));//��ʾ�Ĵ�����addr��Ԫ��ʼ
	while(num--)
	{
		_data=*dat++;
		TM_Send_Byte(_data);	//���Ϳ����ݣ���Ϊ��оƬ����Ч��ַ��0 2 4 6 8 10
		TM_Send_Byte(_data);
	}

	GPIO_SetBits(GPIOC, GPIOB_TM1620_CS);
	TM_Send_Byte(TM_CMD3_ON | LightLevel); //��ʾ���Ʋ���������������Ļ�����ȿ���ͨ���ı����λ����

}
/*******************************************************************************
* Function Name  : TM_Display_Light_On
* Description    : TM��ʾ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TM_Display_Light_On(void)
{
	GPIO_SetBits(GPIOC, GPIOB_TM1620_CS);
	GPIO_SetBits(GPIOB, GPIOB_TM1620_SCK);
	GPIO_SetBits(GPIOB, GPIOB_TM1620_DIN);
	TM_Send_Byte(TM_CMD3_ON | LightLevel); //��ʾ���Ʋ���������������Ļ�����ȿ���ͨ���ı����λ����	
	
	LED_Seg_Light_Flag=LIGHT_ON;

}
/*******************************************************************************
* Function Name  : TM_Display_Light_Off
* Description    : TM��ʾ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TM_Display_Light_Off(void)
{
	GPIO_SetBits(GPIOC, GPIOB_TM1620_CS);
	GPIO_SetBits(GPIOB, GPIOB_TM1620_SCK);
	GPIO_SetBits(GPIOB, GPIOB_TM1620_DIN);
	TM_Send_Byte(TM_CMD3_OFF); //��ʾ��

	LED_Seg_Light_Flag=LIGHT_OFF;

}
/*******************************************************************************
* Function Name  : TM_Display_number
* Description    : TM��ʾ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TM_Display_Number(u32 number)
{
u32 iTemp;
u8 display_flag;

	
	display_flag=0;
	
	//��λ
	iTemp	=	 (number/10000)%10;
	LED_Display_Cache[0]	=	0;	
	if((iTemp!=0)||(display_flag!=0))
	{
		LED_Display_Cache[0]	=	Disp_Buf[iTemp];	
		display_flag=1;
	}
	//ǧλ
	iTemp	=	 (number/1000)%10;
	LED_Display_Cache[1]	=	0;	
	if((iTemp!=0)||(display_flag!=0))
	{
		LED_Display_Cache[1]	=	Disp_Buf[iTemp];	
		display_flag=1;
	}
	//��ʾ��λ
	iTemp	=	 (number%1000);
	iTemp	=	 iTemp/100;
	LED_Display_Cache[2]	=	0;	
	if((iTemp!=0)||(display_flag!=0))
	{
		LED_Display_Cache[2]	=	Disp_Buf[iTemp];	
		display_flag=1;
	}
	//��ʾʮλ
	iTemp	=	 (number%100);
	iTemp	=	 iTemp/10;
	LED_Display_Cache[3]	=	0;	
	if((iTemp!=0)||(display_flag!=0))
	{
		LED_Display_Cache[3]	=	Disp_Buf[iTemp];	
		display_flag=1;
	}
	//��ʾ��λ
	iTemp	=	 (number%10);
	LED_Display_Cache[4]	=	Disp_Buf[iTemp];	
	TM_Update_Display_Buffer();
}

/*******************************************************************************
* Function Name  : TM_Display_Device_Num
* Description    : TM��ʾ��̨��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TM_Display_Device_Num(u8 dev_num)
{
	u8 num;

	LED_Display_Cache[0]	=	LED_NUMBER_LINE;
	num	=	dev_num/100;
	LED_Display_Cache[1]	=	Disp_Buf[num];
	num	=	(dev_num%100)/10;
	LED_Display_Cache[2]	=	Disp_Buf[num];
	num	=	dev_num%10;
	LED_Display_Cache[3]	=	Disp_Buf[num];
	LED_Display_Cache[4]	=	LED_NUMBER_LINE;
	
	TM_Update_Display_Buffer();
}
/*******************************************************************************
* Function Name  : TM_Update_Display_Buffer
* Description    : TM������ʾ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TM_Update_Display_Buffer(void)
{
	TM_MDis(0,5, LED_Display_Cache);
}

