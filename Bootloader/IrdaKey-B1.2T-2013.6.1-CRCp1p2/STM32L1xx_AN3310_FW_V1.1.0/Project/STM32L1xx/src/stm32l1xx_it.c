/**
  ******************************************************************************
  * @file    STM32L1xx_IAP/src/stm32l1xx_it.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    24-January-2012
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * FOR MORE INFORMATION PLEASE READ CAREFULLY THE LICENSE AGREEMENT FILE
  * LOCATED IN THE ROOT DIRECTORY OF THIS FIRMWARE PACKAGE.
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32l1xx_it.h"
#include "MyApp.h"

/** @addtogroup STM32L1xx_IAP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}


void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}
void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
}
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);	
		TimeCnt++;
	}
}

void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		Rxd1Buf[(Rx1Head++)]=USART_ReceiveData(USART1);
		Rx1Head = Rx1Head%Rxd1BufLen;
	}
	/* Clear the USART1 Receive interrupt */
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}

FlagStatus VddStatus;
void PVD_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line16) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_Line16);
		
		VddStatus = PWR_GetFlagStatus(PWR_FLAG_PVDO);
		if(VddStatus == RESET)//电压低于门限值
		{
			//PWR_PVDCmd(DISABLE);
			//PWR_PVDLevelConfig(PWR_PVDLevel_2V8);
			//BatteryVol = 0x22;
			//PWR_PVDCmd(ENABLE);
		}
	}
}
void RTC_WKUP_IRQHandler(void)
{

}
/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
