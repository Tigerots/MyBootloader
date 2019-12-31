/******************** (C) COPYRIGHT 2009 HuaYuan ********************
* File Name          : usb_config.c
* Author             : hanyongliang
* Version            : V1.0
* Date               : 10/08/2007
* Description        : USB Hardware Configuration & Setup
********************************************************************************
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_lib.h"
#include "usb_config.h"
#include "usb_lib.h"
#include "hw_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ErrorStatus HSEStartUpStatus;

/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : Set_USBClock
* Description    : Configures USB Clock input (48MHz).
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Set_USBClock(void)
{
  /* Select USBCLK source : 72MHz / 1.5 = 48MHz */
  RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);

  /* Enable USB clock */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);
}

/*******************************************************************************
* Function Name  : Enter_LowPowerMode.
* Description    : Power-off system clocks and power while entering suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Enter_LowPowerMode(void)
{
}

/*******************************************************************************
* Function Name  : Leave_LowPowerMode.
* Description    : Restores system clocks and power while exiting suspend mode.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Leave_LowPowerMode(void)
{
}

/*******************************************************************************
* Function Name  : USB_Cable_Config.
* Description    : Software Connection/Disconnection of USB Cable.
* Input          : NewState: new state.
* Output         : None.
* Return         : None
*******************************************************************************/
void USB_Cable_Config (FunctionalState NewState)
{
  if (NewState != DISABLE)
  {
    GPIO_ResetBits(GPIOC, GPIO_Pin_14);
  }
  else
  {
    GPIO_SetBits(GPIOC, GPIO_Pin_14);
  }
}


/*******************************************************************************
* Function Name : USB_Reader_SendMsg.
* Description   : USB读卡器发送消息给上位机.
* Input         : buffer:要发送数据的存储缓存; 
				　　 Len:要发送的长度
* Output        : None.
* Return value  : None.
*******************************************************************************/
void USB_Reader_SendMsg(u8 buffer[],u16 len)
{
u8 USB_Buffer[USB_SEND_BUFFER_SIZE];
u16 i;
  
  if(len>USB_SEND_BUFFER_SIZE) len=USB_SEND_BUFFER_SIZE;

  for(i=0;i<len;i++)
  {
  	 USB_Buffer[i]=	 buffer[i];
  }
  for(i=len;i<USB_SEND_BUFFER_SIZE;i++)
  {
  	 USB_Buffer[i]=	 0;
  }
  /*copy buffer in ENDP1 Tx Packet Memory Area*/
  UserToPMABufferCopy(USB_Buffer, GetEPTxAddr(ENDP1), USB_SEND_BUFFER_SIZE);

  SetEPTxCount(ENDP1, USB_SEND_BUFFER_SIZE);
  /* enable endpoint for transmission */
  SetEPTxValid(ENDP1);
}

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
