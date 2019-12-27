/******************** (C) COPYRIGHT 2009 HuaYuan ********************
* File Name          : usb_desc.c
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

/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_config.h"
#include "hw_config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/* USB Standard Device Descriptor */
const u8 DeviceDescriptor[SIZ_DEVICE_DESC] =
  {
    0x12,                       /*bLength */
    USB_DEVICE_DESCRIPTOR_TYPE, /*bDescriptorType*/
    0x10,                       /*bcdUSB USB2.0 */
    0x01,
    0x00,                       /*bDeviceClass*/
    0x00,                       /*bDeviceSubClass*/
    0x00,                       /*bDeviceProtocol*/
    0x40,                       /*bMaxPacketSize40*/
    ID_VENDOR_L,                /*idVendor (0x0483)这个是需要跟USB组织申请的ID号，表示厂商代号*/
    ID_VENDOR_H,
    ID_PRODUCT_L,               /*idProduct = 0x1245该产品的编号，跟厂商编号一起配合使用，让主机注册该设备并加载相应的驱动程序*/
    ID_PRODUCT_H,
    0x00,                       /*bcdDevice rel. 1.00*/
    0x01,
    1,                          /*Index of string descriptor describing
                                              manufacturer */
    2,                          /*Index of string descriptor describing
                                             product*/
    3,                          /*Index of string descriptor describing the
                                             device serial number */
    0x01                        /*bNumConfigurations*/
  }
  ; /* DeviceDescriptor */


/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
const u8 ConfigDescriptor[SIZ_CONFIG_DESC] =
  {
    0x09, /* bLength: Configuation Descriptor size */
    USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType: Configuration */
    SIZ_CONFIG_DESC,
    /* wTotalLength: Bytes returned */
    0x00,
    0x01,         /*bNumInterfaces: 1 interface*/
    0x01,         /*bConfigurationValue: Configuration value*/
    0x00,         /*iConfiguration: Index of string descriptor describing
                                 the configuration*/
    0xA0,         /*bmAttributes: 总线供电，支持远程唤醒 */
    0xC8,         /*MaxPower 400 mA: this current is used for detecting Vbus*/

    /************** Descriptor of interface ****************/
    /* 09 */
    0x09,         /*bLength: Interface Descriptor size*/
    USB_INTERFACE_DESCRIPTOR_TYPE,/*bDescriptorType: Interface descriptor type*/
    0x00,         /*bInterfaceNumber: Number of Interface*/
    0x00,         /*bAlternateSetting: Alternate setting*/
    0x02,         /*bNumEndpoints*/
    0x03,         /*bInterfaceClass: HID*/
    0x00,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
    0x00,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
    0,            /*iInterface: Index of string descriptor*/
    /******************** Descriptor of HID ********************/
    /* 18 */
    0x09,         /*bLength: HID Descriptor size*/
    HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
    0x00,         /*bcdHID: HID Class Spec release number*/
    0x01,
    0x00,         /*bCountryCode: Hardware target country*/
    0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
    0x22,         /*bDescriptorType*/
    SIZ_REPORT_DESC,/*wItemLength: Total length of Report descriptor*/
    0x00,
    /******************** Descriptor of  endpoint ********************/
    /* 27 */
    0x07,          /*bLength: Endpoint Descriptor size*/
    USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/

    0x81,          /*bEndpointAddress: Endpoint Address (IN)*/
    0x03,          /*bmAttributes: Interrupt endpoint*/
    USB_SEND_BUFFER_SIZE,          /*wMaxPacketSize: 64 Byte max */
    0,
    0x0A,          /*bInterval: Polling Interval (10 ms)*/
    /* 34 */
    /******************** Descriptor of  endpoint ********************/
    /* 27 */
    0x07,          /*bLength: Endpoint Descriptor size*/
    USB_ENDPOINT_DESCRIPTOR_TYPE, /*bDescriptorType:*/

    0x01,          /*bEndpointAddress: Endpoint Address (OUT)*/
    0x03,          /*bmAttributes: Interrupt endpoint*/
    USB_REC_BUFFER_SIZE,         /*wMaxPacketSize: 280 Byte max */
    0,
    0x0A,          /*bInterval: Polling Interval (10 ms)*/
    /* 34 */
  }
  ; /* ConfigDescriptor */
const u8 ReportDescriptor[SIZ_REPORT_DESC] =
  {
    0x05, 0xFF,                    // USAGE_PAGE(User define) Device Use=USAGE_PAGE*255+USAGE=0xff*255+USAGE
    0x09, 0x30,                    // USAGE(User define)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x01,                    // USAGE_PAGE(1)
    0x19, 0x00,                    //   USAGE_MINIMUM(0)
    0x29, 0xFF,                    //   USAGE_MAXIMUM(255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0xFF,                    //   LOGICAL_MAXIMUM (255)
    0x75, 0x08,                    //   REPORT_SIZE (08)
    0x95, SEND_BUFFER_SIZE_REP,    //   REPORT_COUNT (64)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x05, 0x02,                    // USAGE_PAGE(2)
    0x19, 0x00,                    //   USAGE_MINIMUM (0)
    0x29, 0xFF,                    //   USAGE_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0xFF,                    //   LOGICAL_MAXIMUM (255)
    0x95, REC_BUFFER_SIZE_REP,     //   REPORT_COUNT (64)
    0x75, 0x08,     			   //   REPORT_SIZE (08)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0xc0                           // END_COLLECTION
  }; /* ReportDescriptor */

/* USB String Descriptors (optional) */
const u8 StringLangID[SIZ_STRING_LANGID] =
{
    SIZ_STRING_LANGID,
    USB_STRING_DESCRIPTOR_TYPE,
    0x09,
    0x04
}
  ; /* LangID = 0x0409: U.S. English */

const u8 StringVendor[SIZ_STRING_VENDOR] =
{
    SIZ_STRING_VENDOR, /* Size of Vendor string */
    USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType*/
	//盈加电子 http://www.gm16.com
    0xC8,0x76,0xa0,0x52,0x35,0x75,0x50,0x5b,' ',0,'h',0,'t',0,
    't',0,'p',0,':',0,'/',0,'/',0,'w',0,'w',0,'w',0,'.',0,'g',0,'m',0,'1',0,'6',0,
    '.',0,'c',0,'o',0,'m',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0,' ',0
};

const u8 StringProduct[SIZ_STRING_PRODUCT] =
{
    SIZ_STRING_PRODUCT,          /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,  /* bDescriptorType */
	//盈加电子: LED客显
	0xC8,0x76,0xa0,0x52,0x35,0x75,0x50,0x5b,':', 0, 'L', 0, 'E', 0,
    'D' ,0x00,0xA2,0x5b,0x3E,0x66,' ' ,0x00,'V', 0, 'e', 0, 'r', 0,
    's', 0, 'i', 0, 'o', 0, 'n', 0, '1', 0, '.', 0, '0', 0
};
const u8 StringSerial[SIZ_STRING_SERIAL] =
{
    SIZ_STRING_SERIAL,           /* bLength */
    USB_STRING_DESCRIPTOR_TYPE,        /* bDescriptorType */
	//yJ_USB_KX_V1
    'Y', 0, 'J', 0, ' ', 0, 'E', 0, 'W', 0, 'D', 0, ' ', 0, 'V', 0,
    'e', 0, 'r', 0, '1', 0, '0', 0
};

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/

