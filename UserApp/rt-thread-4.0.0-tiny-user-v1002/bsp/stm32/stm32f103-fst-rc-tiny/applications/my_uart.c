

/**************************** (C) COPYRIGHT 2019 ****************************
* 设计师:   Tigerots
* 创建时间: 2019.12.28
* 功能描述: iap串口初始化函数
*       
*****************************************************************************
* ********************************修改历史记录********************************
* 修改时间: 
* 版本号:
* 修改内容:
*****************************************************************************/


#define __MY_UART

#include "my_uart.h"	
#include "stm32f1xx_hal.h"
  

#define USART_RX_MAX_LEN 2048
uint32_t USART_RX_CNT=0;//接收的字节数 
uint8_t USART_RX_BUF[USART_RX_MAX_LEN]; //接收缓存

uint8_t My_RxBuffer[1];//HAL库使用的串口接收缓冲
UART_HandleTypeDef UART1_Handler; //UART句柄
  
/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.29
* 功能描述: iap用串口初始化
* 入口参数: 波特率
* 函数返回: 
*****************************************************************************/
//void my_iap_uart_init(uint32_t bound)
//{	
//	//UART 初始化设置
//	UART1_Handler.Instance=USART1;					    //USART1
//	UART1_Handler.Init.BaudRate=bound;				    //波特率
//	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //字长为8位数据格式
//	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //一个停止位
//	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //无奇偶校验位
//	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //无硬件流控
//	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
//	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()会使能UART1
//	//该函数会开启接收中断,(HAL库没有单独的开中断操作)
//    //设置接收缓冲地址以及接收到多少数据产生中断(重要)
//    //每次接收完成,必须需要手动使能中断(重要)
//	HAL_UART_Receive_IT(&UART1_Handler, (uint8_t *)&My_RxBuffer, 1);
//}


///**********************************函数描述***********************************
//* 创建人:   侍任伟
//* 创建时间: 2019.12.29
//* 功能描述: 串口底层初始化, 时钟使能，引脚配置，中断配置
//            此函数会被HAL_UART_Init()调用, 无需手动调用
//* 入口参数: 串口句柄
//* 函数返回: 
//*****************************************************************************/
//void HAL_UART_MspInit(UART_HandleTypeDef *huart)
//{
//    //GPIO端口设置
//	GPIO_InitTypeDef GPIO_Initure;
//	
//	if(huart->Instance==USART1)//如果是串口1，进行串口1 MSP初始化
//	{
//		__HAL_RCC_GPIOA_CLK_ENABLE();			//使能GPIOA时钟
//		__HAL_RCC_USART1_CLK_ENABLE();			//使能USART1时钟
//		__HAL_RCC_AFIO_CLK_ENABLE();
//	
//		GPIO_Initure.Pin=GPIO_PIN_9;			//PA9
//		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
//		GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
//		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//高速
//		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA9

//		GPIO_Initure.Pin=GPIO_PIN_10;			//PA10
//		GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;	//模式要设置为复用输入模式！	
//		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//初始化PA10
//		
//		HAL_NVIC_EnableIRQ(USART1_IRQn);        //使能USART1中断通道
//		HAL_NVIC_SetPriority(USART1_IRQn,3,3);  //抢占优先级3，子优先级3	
//	}
//}


/**********************************函数描述***********************************
* 创建人:   侍任伟
* 创建时间: 2019.12.29
* 功能描述: 串口1回调函数, 如果USART1_IRQHandler中直接读取USART1->DR,则该
            回调函数无法读到数据, 固二者选其中一种接收方法
* 入口参数: 
* 函数返回: 
*****************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)//如果是串口1
	{
        USART_RX_BUF[USART_RX_CNT] = My_RxBuffer[0] ;
        USART_RX_CNT = (USART_RX_CNT+1) % USART_RX_MAX_LEN;
	}
}
	


