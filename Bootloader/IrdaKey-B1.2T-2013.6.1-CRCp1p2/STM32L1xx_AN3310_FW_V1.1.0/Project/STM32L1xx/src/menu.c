/**
  ******************************************************************************
  * @file    STM32L1xx_IAP/src/menu.c 
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    24-January-2012
  * @brief   This file provides the software which contains the main menu routine.
  *          The main menu gives the options of:
  *             - downloading a new binary file, 
  *             - uploading internal flash memory,
  *             - executing the binary file already loaded 
  *             - disabling the write protection of the pages where the user 
  *               loads his binary file.
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
#include "common.h"
#include "menu.h"

void Main_Menu(void)
{

  //while (1)
  {
    SerialPutString("\r\n================== Main Menu ============================\r\n\n");
    SerialPutString("  Download Image To the STM32L1xx Internal Flash ------- 1\r\n\n");
    SerialPutString("  Upload Image From the STM32L1xx Internal Flash ------- 2\r\n\n");
    SerialPutString("  Execute The New Program ------------------------------ 3\r\n\n");
	}

}
/*******************(C)COPYRIGHT 2012 STMicroelectronics *****END OF FILE******/
