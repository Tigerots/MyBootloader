
#define	UARTMODEL

#include <includes.h>

#include "Uart.h"
#include "MyApp.h"
#include "stm32f10x_lib.h"
#include "stm32f10x_usart.h"

#define zero 0


/************************************����1���ͺ���-Start************************************/
//����1����1���ֽ�����
void Usart1Send1Byte(u8 data)
{
    USART_ClearFlag(USART1,USART_FLAG_TC); 
	USART_SendData(USART1, data);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
    {//��������һ��ʱ,��Ҫ�ж��Ƿ������
        ;
    } 
}
//����1����N���ֽ�����
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
//����1�����ַ���
void MyPrintU1(char *pstr )
{
	char *ptr = pstr;
    while(*ptr)
    {
        Usart1Send1Byte( (u8)(*ptr++));
    }
}
//�Ӵ���1��ȡ����,�����ݷ���-1
//timeoutΪ��ʱʱ��, 1����λΪ1mS
s16 MyGet1ByteU1(void)   
{
    u8 tData = 0;
    u8 timeout = 2;
	
    while(timeout)
    {
        if(Rx1Head != Rx1Tail)
        {
            tData = Rxd1Buf[(Rx1Tail++)%Rxd1BufLen];//��ȡ����
            return (tData);
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
    return (-1);
}

//��մ���2���ջ�����
void Usart1ClrRxBuf(void)
{
	Rx1Tail = Rx1Head;
}
/************************************����2���ͺ���-End************************************/


/************************************����2���ͺ���-Start************************************/
//����2����1���ֽ�����
void Usart2Send1Byte(u8 data)
{
    USART_ClearFlag(USART2,USART_FLAG_TC); 
	USART_SendData(USART2, data);
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
    {//��������һ��ʱ,��Ҫ�ж��Ƿ������
        ;
    } 
}
//����2����N���ֽ�����
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
//����2�����ַ���
void MyPrintU2(char *pstr )
{
	char * ptr = pstr;
    while(*ptr)
    {
        Usart2Send1Byte( (u8)(*ptr++));
    }
}
//�Ӵ���2��ȡ����,�����ݷ���-1
//timeoutΪ��ʱʱ��, 1����λΪ1mS
s16 MyGet1ByteU2(void)   
{
    u8 tData = 0;
    u8 timeout = 2;
	
    while(timeout)
    {
        if(Rx2Head != Rx2Tail)
        {
            tData = Rxd2Buf[(Rx2Tail++)%Rxd2BufLen];//��ȡ����
            return (tData);
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
    return (-1);
}

//��մ���2���ջ�����
void Usart2ClrRxBuf(void)
{
	Rx2Tail = Rx2Head;
}
/************************************����2���ͺ���-End************************************/


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
//********************************************************************************************
