/******************** (C) COPYRIGHT 2008 HuaYuan ********************
* File Name          : main.c
* Author             : Hanyongliang
* Date First Issued  : 08/20/2008
* Description        : Main Application.
********************************************************************************
* History:
* 08/20/2008
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "flash.H"
#include "led.h"

/* Private typedef -----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef  void (*pFunction)(void);
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
pFunction Jump_To_Application;
u32 JumpAddress;

/*******************************************************************************
* Function Name  : WatchDog_Init
* Description    : ���Ź���ʼ��.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WatchDog_Init(void)
{
   /* Enable write access to IWDG_PR and IWDG_RLR registers */
  //IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);
}
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : System_Reset
* Description    : ϵͳ��λ.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void System_Reset(void)
{
 /* Enable WWDG clock */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
      /* Generate a system Reset to re-load the new option bytes: enable WWDG and set
        counter value to 0x4F, as T6 bit is cleared this will generate a WWDG reset */
      WWDG_Enable(0x4F);
}
/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks(ʹ���ڲ�ʱ��).
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{
 ErrorStatus HSEStartUpStatus;

  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------------*/
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);

    /* HCLK = SYSCLK  : RCC->CFGR (AHB div) */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* PCLK2 = HCLK RCC->CFGR (APB2 div) */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK/2 RCC->CFGR (APB1 div) */
    RCC_PCLK1Config(RCC_HCLK_Div2);


    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB| RCC_APB2Periph_GPIOC| RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE);
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{
	u32 iTemp;

	/* System Clocks Configuration */
	RCC_Configuration();
	//��ʼ��FLash
	Flash_GPIO_Configuration();
	TM1620_Init();
	//���Ź���ʼ��
	WatchDog_Init();
	//��������־,��ȷ���Ƿ�Ҫ����
	iTemp = Read_Update_Finish_Flag();
	//if(1)
	if(iTemp==UPDATE_FLAG_GO)
	{/*
		User_Read_Config_Info(&System_Config_Info);
		 //ת��������Ϣ
		Traslate_TouTui_Bi_Param();
		//���Ͷ�Ҽ���
		if(TouTui_Bi_Set.TouBi_Polor==NORMAL_OPEN)	  	TouBi_Out_OC();
			else 		 TouBi_Out_Low();
		//�����Ҽ���
		if(TouTui_Bi_Set.TuiBi_Polor== NORMAL_OPEN)		ChuBi_Out_OC();
			else 	ChuBi_Out_Low();
		*/
		LED_Display_Cache[0]	=	LED_NUMBER_U;
		LED_Display_Cache[1]	=	LED_NUMBER_P | LED_NUMBER_DOT;
		LED_Display_Cache[2]	=	LED_NUMBER_DOT;
		LED_Display_Cache[3]	=	LED_NUMBER_DOT;
		LED_Display_Cache[4]	=	LED_NUMBER_DOT;
		TM_Update_Display_Buffer();
		
		//��������Flash
		FLASH_Unlock();
		//���³���Flash
		Flash_Update_Program();
		//����Flash
		FLASH_Lock();	                                                                                                                                                                                                                                                                                                                                                                                                                               
		Write_Update_Finish_Flag();
		System_Reset();
	}
	/* Test if user code is programmed starting from address "ApplicationAddress" */
	if (((*(vu32*)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
	{ /* Jump to user application */
		JumpAddress = *(vu32*) (ApplicationAddress + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__MSR_MSP(*(vu32*) ApplicationAddress);
		Jump_To_Application();
	}
  	while (1)
  	{
  		;
  	}

}

//��ת���û�����ִ��
void iap_jump_to_user_app(u32 app_address)
{
    //�ж��û������һ����ַ��ŵ�ջ����ַ�Ƿ�Ϸ�
    if (((*(vu32*)app_address) & 0x2FFE0000 ) == 0x20000000)
    {
        //�û�����ڶ�����ַ�ռ��ŵ��Ǹ�λ�ж�����(ִ�в���ת��main)
        JumpAddress = *(vu32*) (ApplicationAddress + 4);
        //���ĵ�ַ����Ϊ����(��λ�жϷ�����)
		Jump_To_Application = (pFunction) JumpAddress;
		//���³�ʼ���û������ջ��ָ��
		__MSR_MSP(*(vu32*) ApplicationAddress);
        //��ת���û�����(��λ�жϷ�����)��ִ��
		Jump_To_Application();
    }
}


/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
