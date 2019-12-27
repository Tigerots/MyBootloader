/******************** (C) COPYRIGHT 2009 HuaYuan ********************
* File Name          : usb_proc.h
* Author             : everbright6666@126.com
* Version            : V1.0
* Date               : 3/30/2009
* Description        : Descriptor Header for USB Reader
********************************************************************************
* THE PRESENT SOFTWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_PROP_H
#define __USB_PROP_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef enum _HID_REQUESTS
{
  GET_REPORT = 1,
  GET_IDLE,
  GET_PROTOCOL,

  SET_REPORT = 9,
  SET_IDLE,
  SET_PROTOCOL
} HID_REQUESTS;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void USB_Reader_init(void);
void USB_Reader_Reset(void);
void USB_Reader_Status_In (void);
void USB_Reader_Status_Out (void);
RESULT USB_Reader_Data_Setup(u8);
RESULT USB_Reader_NoData_Setup(u8);
RESULT USB_Reader_Get_Interface_Setting(u8 Interface, u8 AlternateSetting);
u8 *USB_Reader_GetDeviceDescriptor(u16 );
u8 *USB_Reader_GetConfigDescriptor(u16);
u8 *USB_Reader_GetStringDescriptor(u16);
RESULT USB_Reader_SetProtocol(void);
u8 *USB_Reader_GetProtocolValue(u16 Length);
RESULT USB_Reader_SetProtocol(void);
u8 *USB_Reader_GetReportDescriptor(u16 Length);
u8 *USB_Reader_GetHIDDescriptor(u16 Length);

/* Exported define -----------------------------------------------------------*/
#define USB_Reader_GetConfiguration          NOP_Process
#define USB_Reader_SetConfiguration          NOP_Process
#define USB_Reader_GetInterface              NOP_Process
#define USB_Reader_SetInterface              NOP_Process
#define USB_Reader_GetStatus                 NOP_Process
#define USB_Reader_ClearFeature              NOP_Process
#define USB_Reader_SetEndPointFeature        NOP_Process
#define USB_Reader_SetDeviceFeature          NOP_Process
#define USB_Reader_SetDeviceAddress          NOP_Process

#define REPORT_DESCRIPTOR                  0x22

#endif /* __USB_PROP_H */

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
