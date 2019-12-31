/******************** (C) COPYRIGHT 2008 HuaYuan ********************
* File Name          : bsp.c
* Author             : Han yongliang
* Date First Issued  : 03/23/2009
* Description        : board support program body
*					 : 该文件初始化硬件的频率；
                       LED灯及蜂鸣器底层函数；
                       中断设置；
*					   系统时钟节拍；板级初始化
********************************************************************************
* History:
* 03/23/2009: V0.1
*******************************************************************************/


/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/

#define  BSP_GLOBALS
#include <includes.h>
#include "app.h"


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  void  Tmr_TickInit  (void);

void Delay (int cnt)
{
    u8 i;
	while(cnt--)
    {    
        for(i=0; i<100; i++)
        {;}
    }
}


/*
*********************************************************************************************************
*                                   APPLICATION-DEFINED HOOKS
*
* Description: These functions are called by the hooks in os_cpu_c.c.
*
* Returns    : none
*********************************************************************************************************
*/

#if (OS_APP_HOOKS_EN > 0)
void  App_TaskCreateHook (OS_TCB *ptcb)
{
#if (uC_PROBE_OS_PLUGIN > 0) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TaskCreateHook(ptcb);
#endif
}

void  App_TaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
/*******************************************************************************
* Function Name  : App_TaskIdleHook 
* Description    : 空闲钩子任务（该任务在uCOS-II中必须包含）
*					该任务可用来在空闲状态时比如关闭电源节约功耗等功能
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{
//空闲任务是永远处于就绪态的，故不要在OSTaskIdleHook()中调用任何可以使任务挂起的PEND函数.
}
#endif


void  App_TaskStatHook (void)
{
}


#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{
#if (uC_PROBE_OS_PLUGIN > 0) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TaskSwHook();
#endif
}
#endif


#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif


#if OS_TIME_TICK_HOOK_EN > 0
void  App_TimeTickHook (void)
{
#if (uC_PROBE_OS_PLUGIN > 0) && (OS_PROBE_HOOKS_EN > 0)
    OSProbe_TickHook();
#endif

}
#endif
#endif
/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
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

    /* ADCCLK = PCLK2/6  RCC->CFGR (ADC div)*/
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

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
    
  /* Enable GPIOA,GPIOB and AFIO clocks */
   //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB, ENABLE);
   RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA 
						| RCC_APB2Periph_GPIOB 
						| RCC_APB2Periph_GPIOC 
						| RCC_APB2Periph_GPIOD 
						| RCC_APB2Periph_AFIO, ENABLE);
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
}
/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM  
      /* Set the Vector Table base location at 0x20000000 */ 
      NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
      /* Set the Vector Table base location at 0x08000000 */ 
      NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x8000);
#endif 

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQChannel;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
*********************************************************************************************************
*                                     DISABLE ALL INTERRUPTS
*
* Description : This function disables all interrupts from the interrupt controller.
* Arguments   : None.
* Returns     : None.
*********************************************************************************************************
*/
void  BSP_IntDisAll (void)
{
    CPU_IntDis();
}
/*
*********************************************************************************************************
*                                      GET THE CPU CLOCK FREQUENCY
*
* Description: This function reads CPU registers to determine the CPU clock frequency of the chip in KHz.
* Argument(s): None.
* Returns    : The CPU clock frequency, in Hz.
*********************************************************************************************************
*/

CPU_INT32U  BSP_CPU_ClkFreq (void)
{
    static  RCC_ClocksTypeDef  rcc_clocks;
 
    RCC_GetClocksFreq(&rcc_clocks);
   return ((CPU_INT32U)rcc_clocks.HCLK_Frequency);
}

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
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);

  /* IWDG counter clock: 32KHz(LSI) / 32 = 1KHz */
  IWDG_SetPrescaler(IWDG_Prescaler_32);

  /* Set counter reload value to 349 */
  IWDG_SetReload(0x800);

  /* Reload IWDG counter */
  IWDG_ReloadCounter();

  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
}
/*******************************************************************************
* Function Name  : FeedWtchDog
* Description    : 喂狗.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FeedWtchDog(void)
{
  /* Reload IWDG counter */
  IWDG_ReloadCounter();
}
/*******************************************************************************
* Function Name  : FeedWtchDog
* Description    : 喂狗.
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
/*
*********************************************************************************************************
*                                       TICKER INITIALIZATION
*
* Description : This function is called to initialize uC/OS-II's tick source (typically a timer generating
*               interrupts every 1 to 100 mS).
* Arguments   : none
* Note(s)     : 1) The timer is setup for output compare mode BUT 'MUST' also 'freerun' so that the timer
*                  count goes from 0x00000000 to 0xFFFFFFFF to ALSO be able to read the free running count.
*                  The reason this is needed is because we use the free-running count in uC/OS-View.
*********************************************************************************************************
*/

  void  Tmr_TickInit (void)
{
    RCC_ClocksTypeDef  rcc_clocks;
    CPU_INT32U         cnts;

    /* Configure the Priority Group to 2 bits:0~3级主优先级,0~3级次优先级 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
    /* Configure the SysTick handler priority:主优先级=0,次优先级=0 时钟服务为最高优先级*/
    NVIC_SystemHandlerPriorityConfig(SystemHandler_SysTick, 0, 0);
    
    /* Configure the PendSV handler priority:主优先级=3,次优先级=3,任务切换服务为最低优先级*/
    NVIC_SystemHandlerPriorityConfig(SystemHandler_PSV, 3, 3);
    
    RCC_GetClocksFreq(&rcc_clocks);

    cnts = (CPU_INT32U)rcc_clocks.HCLK_Frequency / OS_TICKS_PER_SEC;
    
    SysTick_SetReload(cnts);
    SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
    SysTick_CounterCmd(SysTick_Counter_Enable);
    SysTick_ITConfig(ENABLE);
}
/*
*********************************************************************************************************
*                                         TIMER IRQ HANDLER
*
* Description : This function handles the timer interrupt that is used to generate TICKs for uC/OS-II.
*
* Arguments   : none
*
* Note(s)     : 1) The timer is 'reloaded' with the count at compare + the time for the next interrupt.
*                  Since we are using 'unsigned' integer math, overflows are irrelevant.
*********************************************************************************************************
*/

void  Tmr_TickISR_Handler (void)
{
    OS_CPU_SR  cpu_sr;

    OS_ENTER_CRITICAL();                                        /* Tell uC/OS-II that we are starting an ISR                */
    OSIntNesting++;
    OS_EXIT_CRITICAL();
    OSTimeTick();                                               /* Call uC/OS-II's OSTimeTick()                             */
    OSIntExit();                                                /* Tell uC/OS-II that we are leaving the ISR                */
}

//IO口初始化函数
void GPIO_Pin_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//行控制引脚 Row1-3
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8
                                |  GPIO_Pin_11
                                |  GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
    //行控制引脚 Row4-6
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_10
                                |  GPIO_Pin_11
                                |  GPIO_Pin_12;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
    //行控制引脚 Row7
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  	GPIO_Init(GPIOD, &GPIO_InitStructure);
    //行控制引脚 Row8-12
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_5
                                |  GPIO_Pin_6
                                |  GPIO_Pin_7
                                |  GPIO_Pin_8
                                |  GPIO_Pin_9;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    //列 MBI5024驱动引脚
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_12  //GreednSDI
                                |  GPIO_Pin_13  //nOE1
                                |  GPIO_Pin_14  //RedSDI
                                |  GPIO_Pin_15; //CLK
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_6   //LE
                                |  GPIO_Pin_7;  //nOE2
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  	GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    //按键
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_7;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
  	GPIO_Init(GPIOA, &GPIO_InitStructure);

}

/****************************************************************************************
** 函数名称: UART_configuration
** 功能描述: UART配置
** 参    数: None
** 返 回 值: None       
** 作　  者: 
** 日  　期: 
**---------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
void UART1_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  USART_ClockInitTypeDef USART_ClockInitStructure;

  /* Configure USART1 Tx (PA9) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
    
  /* Configure USART1 Rx (PA10) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* Enable USART1 clocks */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

  USART_ClockInitStructure.USART_Clock=USART_Clock_Disable;
  USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
  USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
  USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
  USART_ClockInit(USART1, &USART_ClockInitStructure);
  
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);

  /* Enable USART1 Receive  interrupt */
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
  /* Enable USART1 */
  USART_Cmd(USART1, ENABLE);

} 

/*
*********************************************************************************************************
*                                         BSP INITIALIZATION
*
* Description : This function should be called by your application code before you make use of any of the
*               functions found in this module.
*
* Arguments   : none
*********************************************************************************************************
*/
void  BSP_Init (void)
{
	Tmr_TickInit(); //系统时钟初始化
    USART1_Configuration(115200);//串口初始化
	USART2_Configuration(115200);//74880
    GPIO_Pin_Init();//通用IO口初始化

}


























