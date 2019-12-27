#ifndef UART_H
#define UART_H

#ifdef	UARTMODEL
	#define	__UART_GLOBE
#else
	#define	__UART_GLOBE		extern
#endif
	
#include    <stm32f10x_type.h>

#define CMDOk 		0x00	//�ɹ�
#define CMDFail 	0x01	//ʧ��
#define NotSuport	0x02	//��֧�ֵ�����
#define ErrCheck	0x03	//����ʹ���
#define ErrLen		0x04	//���ȳ���Χ����
#define Err05		0x05	//����ʹ�ò�������
#define ErrLenCheck	0x06	//�����У��ʹ���
#define ErrFormat	0x07	//���������ʽ����
#define ErrPaper	0x08	//��ӡ����ֽ
#define ErrPara		0x0A	//���ݵĲ�������

	
	

//�豸���Ͷ���
#define MQD		0x10//��Ȫ��
#define MQW		0x11//��Ȫβ��
#define FLD		0x20//������
#define FLW		0x21//����β��
//�豸����
__UART_GLOBE u8 DevType;

//Ĭ��ϵ��Ϊ1.0,��ֵ:0.5, 0.8,�������ʵ��Ҫ���λ��ȡ��
__UART_GLOBE float CoefA;



// ����USART1������
__UART_GLOBE u8  DealEnFlag;//����ʹ�ܱ�־
__UART_GLOBE u8  RealDataFlag;//ʵʱ���ݻ�ȡָ��


__UART_GLOBE  u8 RecStep;
#define Rxd1BufLen 50
__UART_GLOBE u8   Rxd1Buf[Rxd1BufLen];            
__UART_GLOBE u8  Rx1Head;
__UART_GLOBE u8  Rx1Tail;
__UART_GLOBE u8  RxFlag;

__UART_GLOBE u8   Txd1Buf[256]; 

// ����USART2������
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

