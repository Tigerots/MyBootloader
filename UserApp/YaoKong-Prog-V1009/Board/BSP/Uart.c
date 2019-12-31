
#define	UARTMODEL

#include <includes.h>
#include "Uart.h"
#include "stm32f10x_lib.h"
#include "stm32f10x_usart.h"
#include "MyApp.h"

#define zero 0


void USART1_Transmit( INT8U data )
{
	USART_SendData(USART1, data);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {//当连续发一串时,需要判断是否发送完成
        ;
    } 
}

void USART1_Send(INT8U *p,INT8U length)
{

	INT8U i,j=0;
	i=length;
	
	while(i--)
	{
		USART1_Transmit(p[j++]);	
	}
}

void USART2_Transmit( INT8U data )
{
	USART_SendData(USART2, data);
}

void USART2_Send(INT8U *p,INT8U length)
{
	INT8U i,j=0;
	i=length;
	
	while(i--)
	{
		USART2_Transmit(p[j++]);		
	}
}

void ClearUsartBuf(void)
{
	Rx1Tail = Rx1Head;
}
//从串口获得数据,无数据返回-1
s16 my_getchar(u8 com)   
{
    u8 temp=0;
    u8 timeout = 2;
	if ( 0 == com )	  // USB
	{
	}
    else if( 1 == com )
    {
        while(timeout)
        {
            if(Rx1Head != Rx1Tail)
            {
				temp = Rxd1Buf[Rx1Tail++];
                Rx1Tail %= Rxd1BufLen;
                return (temp);                  // 读到数据，退出
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
    }
    else if ( 2 == com )
    {   
        while(timeout)
        {
            if(Rx2Head != Rx2Tail)
            {
				temp = Rxd2Buf[Rx2Tail++];
                Rx2Tail %= Rxd2BufLen;
                return (temp);                  // 读到数据，退出
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
    }
    else
    {
        return (-1);
    }
    return (-1);
}

//从串口发出数据
void my_putstr( INT8U com, const char *pstr )
{
	const char * ptr = pstr;
	if(com==1)
	{
		while ( *ptr )
		{
			USART1_Transmit( (INT8U)*ptr++ );
		}
	}
	if(com==2)
	{
		while ( *ptr )
		{
			USART2_Transmit( (INT8U)*ptr++ );
		}
	}
}
//从串口发出数据
void my_printf( INT8U com, u8 *pstr, u8 cnt )
{
	u8 * ptr = pstr;
	if(com==1)
	{
		while ( cnt-- )
		{
			USART1_Transmit( (INT8U)*ptr++ );
		}
	}
	if(com==2)
	{
		while ( cnt-- )
		{
			USART2_Transmit( (INT8U)*ptr++ );
		}
	}
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
















/////////////////////////
