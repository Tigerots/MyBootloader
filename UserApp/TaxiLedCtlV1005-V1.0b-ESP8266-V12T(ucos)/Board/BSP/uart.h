#ifndef UART_H
#define UART_H

#ifdef	UARTMODEL
	#define	__UART_GLOBE
#else
	#define	__UART_GLOBE		extern
#endif
	
#include    <stm32f10x_type.h>

    
#define USART_RX_MAX_LEN 2048
__UART_GLOBE u32 USART_RX_CNT;//接收的字节数 
__UART_GLOBE u32 USART_RX_BUF[USART_RX_MAX_LEN]; //接收缓存
    
    
    
    
    
    
// 定义USART1缓冲区
__UART_GLOBE  u8 RecStep;
#define Rxd1BufLen 50
__UART_GLOBE u8  Rxd1Buf[Rxd1BufLen];            
__UART_GLOBE u8  Rx1Head;
__UART_GLOBE u8  Rx1Tail;
__UART_GLOBE u8  RxFlag;

__UART_GLOBE u8   Txd1Buf[256]; 

// 定义USART2缓冲区
#define Rxd2BufLen 256	  
__UART_GLOBE u8   Rxd2Buf[Rxd2BufLen];            
__UART_GLOBE u16  Rx2Head;
__UART_GLOBE u16  Rx2Tail;

// 函数声明
__UART_GLOBE void USART1_Configuration(u32 BaudRate);
__UART_GLOBE void USART2_Configuration(u32 BaudRate);
    
#define MySendNByteU1 Usart1SendNByte
__UART_GLOBE void Usart1SendNByte(u8 *p, u8 len);
__UART_GLOBE void MyPrintU1(char *pstr);
__UART_GLOBE s16 MyGet1ByteU1(void);
__UART_GLOBE void Usart1ClrRxBuf(void);
    
#define MySendNByteU2 Usart2SendNByte
__UART_GLOBE void Usart2SendNByte(u8 *p, u8 len);
__UART_GLOBE void MyPrintU2(char *pstr);
__UART_GLOBE s16 MyGet1ByteU2(void);
__UART_GLOBE void Usart2ClrRxBuf(void);


#endif

