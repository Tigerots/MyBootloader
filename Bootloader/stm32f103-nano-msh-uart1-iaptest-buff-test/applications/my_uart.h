



#ifndef  __UART_H__
#define  __UART_H__
 

#ifndef  __MY_UART
#define	 __MY_UART_EXT  extern
#else
#define	 __MY_UART_EXT 
#endif

#include "stm32f1xx_hal.h"


__MY_UART_EXT void my_iap_uart_init(uint32_t bound);


#endif


