



#ifndef  __UART_H__
#define  __UART_H__
 

#ifndef  __MY_UART
#define	 __MY_UART_EXT  extern
#else
#define	 __MY_UART_EXT 
#endif

#include "stm32f1xx_hal.h"
#include "string.h"

__MY_UART_EXT uint8_t USART_RX_BUF[1024]; 
__MY_UART_EXT uint32_t USART_RX_CNT;//接收的字节数 



__MY_UART_EXT void my_iap_uart_init(uint32_t bound);


#endif


