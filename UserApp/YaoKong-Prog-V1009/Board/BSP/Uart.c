
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
    {//��������һ��ʱ,��Ҫ�ж��Ƿ������
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
//�Ӵ��ڻ������,�����ݷ���-1
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
                return (temp);                  // �������ݣ��˳�
            }
            else
            {
                timeout--;                      // ��������û�����ݣ���ʱ��1
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
                return (temp);                  // �������ݣ��˳�
            }
            else
            {
                timeout--;                      // ��������û�����ݣ���ʱ��1
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

//�Ӵ��ڷ�������
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
//�Ӵ��ڷ�������
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
** ��������: UART_configuration
** ��������: UART1,2����
** ��    ��: None
** �� �� ֵ: None       
** ����  ��: 
** ��  ����: 2008��1��7��
**---------------------------------------------------------------------------------------
** �� �� ��: 
** �ա�  ��: 
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
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	// �����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // Configure USARTx_R1 as input floating 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // ��������
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

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�����ж�	  
  USART_Init(USART1, &USART_InitStructure);
  USART_Cmd(USART1, ENABLE);/* Enable USART1 */

}
/****************************************************************************************
** ��������: UART_configuration
** ��������: UART1,2����
** ��    ��: None
** �� �� ֵ: None       
** ����  ��: 
** ��  ����: 2008��1��7��
**---------------------------------------------------------------------------------------
** �� �� ��: 
** �ա�  ��: 
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
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	// �����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);	
  // Configure USARTx_Rx as input floating 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	 // ��������
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
