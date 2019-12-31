#ifndef UART_H
#define UART_H

#ifdef	UARTMODEL
	#define	__UART_GLOBE
#else
	#define	__UART_GLOBE		extern
#endif
	
#include    <stm32f10x_type.h>

#define CMDOk 		0x00	//成功
#define CMDFail 	0x01	//失败
#define NotSuport	0x02	//不支持的命令
#define ErrCheck	0x03	//检验和错误
#define ErrLen		0x04	//长度超范围错误
#define Err05		0x05	//函数使用不符错误
#define ErrLenCheck	0x06	//命令长度校验和错误
#define ErrFormat	0x07	//命令参数格式错误
#define ErrPaper	0x08	//打印机无纸
#define ErrPara		0x0A	//传递的参数错误

	
	

//设备类型定义
#define MQD		0x10//鸣泉灯
#define MQW		0x11//鸣泉尾气
#define FLD		0x20//福立灯
#define FLW		0x21//福立尾气
//设备类型
__UART_GLOBE u8 DevType;

//默认系数为1.0,限值:0.5, 0.8,结果根据实际要求进位或取整
__UART_GLOBE float CoefA;



// 定义USART1缓冲区
__UART_GLOBE u8  DealEnFlag;//处理使能标志
__UART_GLOBE u8  RealDataFlag;//实时数据获取指令


__UART_GLOBE  u8 RecStep;
#define Rxd1BufLen 50
__UART_GLOBE u8   Rxd1Buf[Rxd1BufLen];            
__UART_GLOBE u8  Rx1Head;
__UART_GLOBE u8  Rx1Tail;
__UART_GLOBE u8  RxFlag;

__UART_GLOBE u8   Txd1Buf[256]; 

// 定义USART2缓冲区
#define Rxd2BufLen 50	  
__UART_GLOBE u8   Rxd2Buf[Rxd2BufLen];            
__UART_GLOBE u16  Rx2Head;
__UART_GLOBE u16  Rx2Tail;


__UART_GLOBE void USART1_Send(u8 *p, u8 length);
__UART_GLOBE s16 my_getchar(u8 com);
__UART_GLOBE void my_putstr( u8 com, const char *pstr );
__UART_GLOBE void my_printf( u8 com, u8 *pstr, u8 cnt );
__UART_GLOBE void USART1_Configuration(u32 BaudRate);
__UART_GLOBE void USART2_Configuration(u32 BaudRate);
__UART_GLOBE void ClearUsartBuf(void);
#endif

