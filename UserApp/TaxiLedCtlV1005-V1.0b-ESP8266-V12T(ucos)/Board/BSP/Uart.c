
#define	UARTMODEL

#include <includes.h>

#include "Uart.h"
#include "MyApp.h"
#include "stm32f10x_lib.h"
#include "stm32f10x_usart.h"

#define zero 0


/************************************串口1发送函数-Start************************************/
//串口1发送1个字节数据
void Usart1Send1Byte(u8 data)
{
    USART_ClearFlag(USART1,USART_FLAG_TC); 
	USART_SendData(USART1, data);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {//当连续发一串时,需要判断是否发送完成
        ;
    } 
}
//串口1发送N个字节数据
void Usart1SendNByte(u8 *p, u8 len)
{
	u8 i=0;
    u8 j=0;
    
	i=len;
	while(i--)
	{
		Usart1Send1Byte(p[j++]);		
	}
}
//串口1发送字符串
void MyPrintU1(char *pstr )
{
	char *ptr = pstr;
    while(*ptr)
    {
        Usart1Send1Byte( (u8)(*ptr++));
    }
}
//从串口1获取数据,无数据返回-1
//timeout为超时时间, 1个单位为1mS
s16 MyGet1ByteU1(void)   
{
    u8 tData = 0;
    u8 timeout = 2;
	
    while(timeout)
    {
        if(Rx1Head != Rx1Tail)
        {
            tData = Rxd1Buf[(Rx1Tail++)%Rxd1BufLen];//读取数据
            return (tData);
        }
        else
        {
            timeout--;                      // 缓冲区中没有数据，计时减1
            if (timeout != 0)
            {
                OSTimeDlyHMSM(0, 0, 0, 1);
            }
        }
    }
    return (-1);
}

//清空串口2接收缓冲区
void Usart1ClrRxBuf(void)
{
	Rx1Tail = Rx1Head;
}
/************************************串口2发送函数-End************************************/


/************************************串口2发送函数-Start************************************/
//串口2发送1个字节数据
void Usart2Send1Byte(u8 data)
{
    USART_ClearFlag(USART2,USART_FLAG_TC); 
	USART_SendData(USART2, data);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
    {//当连续发一串时,需要判断是否发送完成
        ;
    } 
}
//串口2发送N个字节数据
#define MySendNByteU2 Usart2SendNByte
void Usart2SendNByte(u8 *p, u8 len)
{
	u8 i=0;
    u8 j=0;
    
	i=len;
	while(i--)
	{
		Usart2Send1Byte(p[j++]);		
	}
}
//串口2发送字符串
void MyPrintU2(char *pstr )
{
	char * ptr = pstr;
    while(*ptr)
    {
        Usart2Send1Byte( (u8)(*ptr++));
    }
}
//从串口2获取数据,无数据返回-1
//timeout为超时时间, 1个单位为1mS
s16 MyGet1ByteU2(void)   
{
    u8 tData = 0;
    u8 timeout = 2;
	
    while(timeout)
    {
        if(Rx2Head != Rx2Tail)
        {
            tData = Rxd2Buf[(Rx2Tail++)%Rxd2BufLen];//读取数据
            return (tData);
        }
        else
        {
            timeout--;                      // 缓冲区中没有数据，计时减1
            if (timeout != 0)
            {
                OSTimeDlyHMSM(0, 0, 0, 1);
            }
        }
    }
    return (-1);
}

//清空串口2接收缓冲区
void Usart2ClrRxBuf(void)
{
	Rx2Tail = Rx2Head;
}
/************************************串口2发送函数-End************************************/


/****************************************************************************************
** 函数名称: UART_configuration
** 功能描述: UART1,2配置
** 参    数: None
** 返 回 值: None       
** 作　  者: 
** 日  　期: 2008年1月7日
**---------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
void USART1_Configuration(u32 BaudRate)
{
  USART_InitTypeDef USART_InitStructure;
  USART_ClockInitTypeDef USART_ClockInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  // Configure USARTx_T1 as alternate function push-pull 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	// 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	// 复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // Configure USARTx_R1 as input floating 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // 浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  USART_InitStructure.USART_BaudRate = BaudRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    
  USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;
  USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
  USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
  USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable; 
  
  USART_ClockInit(USART1, &USART_ClockInitStructure);

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//接收中断	  
  USART_Init(USART1, &USART_InitStructure);
  USART_Cmd(USART1, ENABLE);/* Enable USART1 */

}
/****************************************************************************************
** 函数名称: UART_configuration
** 功能描述: UART1,2配置
** 参    数: None
** 返 回 值: None       
** 作　  者: 
** 日  　期: 2008年1月7日
**---------------------------------------------------------------------------------------
** 修 改 人: 
** 日　  期: 
**--------------------------------------------------------------------------------------
****************************************************************************************/
void USART2_Configuration(u32 BaudRate)
{
  USART_InitTypeDef USART_InitStructure; 
  USART_ClockInitTypeDef USART2_ClockInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB1PeriphClockCmd( RCC_APB1Periph_USART2, ENABLE);
  // Configure USARTx_Tx as alternate function push-pull 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	// 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	// 复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
  // Configure USARTx_Rx as input floating 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 // 浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
   //==============================================================

  USART_InitStructure.USART_BaudRate = BaudRate;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
    
  USART2_ClockInitStructure.USART_Clock = USART_Clock_Disable;
  USART2_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
  USART2_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
  USART2_ClockInitStructure.USART_LastBit = USART_LastBit_Disable; 
  
  USART_ClockInit(USART2, &USART2_ClockInitStructure);
  
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
  USART_Init(USART2, &USART_InitStructure);
  USART_Cmd(USART2, ENABLE);/* Enable USART2 */
}  
//********************************************************************************************
