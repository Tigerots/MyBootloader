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
* Description    : 看门狗初始化.
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
* Description    : 系统复位.
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
* Description    : Configures the different system clocks(使用内部时钟).
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
	//初始化FLash
	Flash_GPIO_Configuration();
	TM1620_Init();
	//看门狗初始化
	WatchDog_Init();
	//读升级标志,以确定是否要升级
	iTemp = Read_Update_Finish_Flag();
	//if(1)
	if(iTemp==UPDATE_FLAG_GO)
	{/*
		User_Read_Config_Info(&System_Config_Info);
		 //转换参数信息
		Traslate_TouTui_Bi_Param();
		//检查投币极性
		if(TouTui_Bi_Set.TouBi_Polor==NORMAL_OPEN)	  	TouBi_Out_OC();
			else 		 TouBi_Out_Low();
		//检查出币极性
		if(TouTui_Bi_Set.TuiBi_Polor== NORMAL_OPEN)		ChuBi_Out_OC();
			else 	ChuBi_Out_Low();
		*/
		LED_Display_Cache[0]	=	LED_NUMBER_U;
		LED_Display_Cache[1]	=	LED_NUMBER_P | LED_NUMBER_DOT;
		LED_Display_Cache[2]	=	LED_NUMBER_DOT;
		LED_Display_Cache[3]	=	LED_NUMBER_DOT;
		LED_Display_Cache[4]	=	LED_NUMBER_DOT;
		TM_Update_Display_Buffer();
		
		//解锁程序Flash
		FLASH_Unlock();
		//更新程序Flash
		Flash_Update_Program();
		//上锁Flash
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

//跳转到用户程序执行
void iap_jump_to_user_app(u32 app_address)
{
    //判断用户程序第一个地址存放的栈顶地址是否合法
    if (((*(vu32*)app_address) & 0x2FFE0000 ) == 0x20000000)
    {
        //用户程序第二个地址空间存放的是复位中断向量(执行并跳转到main)
        JumpAddress = *(vu32*) (ApplicationAddress + 4);
        //将改地址声明为函数(复位中断服务函数)
		Jump_To_Application = (pFunction) JumpAddress;
		//重新初始化用户程序的栈顶指针
		__MSR_MSP(*(vu32*) ApplicationAddress);
        //跳转到用户程序(复位中断服务函数)并执行
		Jump_To_Application();
    }
}


/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
