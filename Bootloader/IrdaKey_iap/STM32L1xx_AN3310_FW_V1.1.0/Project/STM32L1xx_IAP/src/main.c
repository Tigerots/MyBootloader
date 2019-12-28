/**
  ******************************************************************************
  * @file    STM32L1xx_IAP/src/main.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    24-January-2012
  * @brief   Main program body
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

/** @addtogroup STM32L1xx_IAP
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "menu.h"

#ifdef USE_STM32L152_EVAL
	#include "stm32l152_eval.h"
#elif defined USE_STM32L152D_EVAL
	#include "stm32l152d_eval.h"
#endif

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
pFunction Jump_To_Application;
uint32_t JumpAddress;

void System_Reset(void)
{
 /* Enable WWDG clock */
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
      /* Generate a system Reset to re-load the new option bytes: enable WWDG and set
        counter value to 0x4F, as T6 bit is cleared this will generate a WWDG reset */
      WWDG_Enable(0x4F);
}
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{ 
	u32 iTemp;
	/* Unlock the Flash Program Erase controller */
	FLASH_If_Init();

	//读升级标志,以确定是否要升级
	iTemp = Read_Update_Finish_Flag();
	if(iTemp==UPDATE_FLAG_GO)
	{
		//更新程序Flash
		Flash_Update_Program();
		Write_Update_Finish_Flag();
		System_Reset();
	}
	/* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */
	if (((*(volatile unsigned long*)APPLICATION_ADDRESS) & 0x2FFE0000 ) == 0x20000000)
	{ 
		/* Jump to user application */
		JumpAddress = *(__IO uint32_t*) (APPLICATION_ADDRESS + 4);
		Jump_To_Application = (pFunction) JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
		Jump_To_Application();
	}


	while (1)
	{}
}

/**
  * @brief  Initialize the IAP: Configure USART.
  * @param  None
  * @retval None
  */


#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */

/******************* (C) COPYRIGHT 2012 STMicroelectronics *****END OF FILE****/
