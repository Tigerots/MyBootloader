

/**************************** (C) COPYRIGHT 2019 ****************************
* ���ʦ:   Tigerots
* ����ʱ��: 2019.12.28
* ��������: iap���ڳ�ʼ������
*       
*****************************************************************************
* ********************************�޸���ʷ��¼********************************
* �޸�ʱ��: 
* �汾��:
* �޸�����:
*****************************************************************************/


#define __MY_UART

#include "stm32f1xx_hal.h"
#include <rtthread.h>
#include "my_iap.h"
#include "my_type.h"
#include "my_flash.h"
#include "my_uart.h"
#include "cmsis_armcc.h"
  

uint8_t My_RxBuffer[1];//HAL��ʹ�õĴ��ڽ��ջ���
UART_HandleTypeDef UART1_Handler; //UART���
  
/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.29
* ��������: iap�ô��ڳ�ʼ��
* ��ڲ���: ������
* ��������: 
*****************************************************************************/
void my_iap_uart_init(uint32_t bound)
{	
	//UART ��ʼ������
	UART1_Handler.Instance=USART1;					    //USART1
	UART1_Handler.Init.BaudRate=bound;				    //������
	UART1_Handler.Init.WordLength=UART_WORDLENGTH_8B;   //�ֳ�Ϊ8λ���ݸ�ʽ
	UART1_Handler.Init.StopBits=UART_STOPBITS_1;	    //һ��ֹͣλ
	UART1_Handler.Init.Parity=UART_PARITY_NONE;		    //����żУ��λ
	UART1_Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;   //��Ӳ������
	UART1_Handler.Init.Mode=UART_MODE_TX_RX;		    //�շ�ģʽ
	HAL_UART_Init(&UART1_Handler);					    //HAL_UART_Init()��ʹ��UART1
	//�ú����Ὺ�������ж�,(HAL��û�е����Ŀ��жϲ���)
    //���ý��ջ����ַ�Լ����յ��������ݲ����ж�(��Ҫ)
    //ÿ�ν������,������Ҫ�ֶ�ʹ���ж�(��Ҫ)
	HAL_UART_Receive_IT(&UART1_Handler, (uint8_t *)My_RxBuffer, 1);
}


/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.29
* ��������: ���ڵײ��ʼ��, ʱ��ʹ�ܣ��������ã��ж�����
            �˺����ᱻHAL_UART_Init()����, �����ֶ�����
* ��ڲ���: ���ھ��
* ��������: 
*****************************************************************************/
void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    //GPIO�˿�����
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance==USART1)//����Ǵ���1�����д���1 MSP��ʼ��
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();			//ʹ��GPIOAʱ��
		__HAL_RCC_USART1_CLK_ENABLE();			//ʹ��USART1ʱ��
		__HAL_RCC_AFIO_CLK_ENABLE();
	
		GPIO_Initure.Pin=GPIO_PIN_9;			//PA9
		GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//�����������
		GPIO_Initure.Pull=GPIO_PULLUP;			//����
		GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;//����
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA9

		GPIO_Initure.Pin=GPIO_PIN_10;			//PA10
		GPIO_Initure.Mode=GPIO_MODE_AF_INPUT;	//ģʽҪ����Ϊ��������ģʽ��	
		HAL_GPIO_Init(GPIOA,&GPIO_Initure);	   	//��ʼ��PA10
		
		HAL_NVIC_EnableIRQ(USART1_IRQn);        //ʹ��USART1�ж�ͨ��
		HAL_NVIC_SetPriority(USART1_IRQn,3,1);  //��ռ���ȼ�3�������ȼ�3	
	}
}

/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.29
* ��������: ����1�жϷ������
* ��ڲ���: 
* ��������: 
*****************************************************************************/
void USART1_IRQHandler(void)                	
{ 
    #ifdef USE_UART_Rx_Callback
	uint8_t Res;
    
	if((__HAL_UART_GET_FLAG(&UART1_Handler,UART_FLAG_RXNE)!=RESET))
	{//ֱ�Ӵ���
		Res=USART1->DR; 
		if(USART_RX_CNT < 1024)
		{
			USART_RX_BUF[USART_RX_CNT]=Res;
			USART_RX_CNT++;			 									     
		}
	}
    #endif
	HAL_UART_IRQHandler(&UART1_Handler);	
    //ÿ�ν������,������Ҫ�ֶ�ʹ���ж�
    HAL_UART_Receive_IT(&UART1_Handler, (uint8_t *)My_RxBuffer, 1);
} 

/**********************************��������***********************************
* ������:   ����ΰ
* ����ʱ��: 2019.12.29
* ��������: ����1�ص�����, ���USART1_IRQHandler��ֱ�Ӷ�ȡUSART1->DR,���
            �ص������޷���������, �̶���ѡ����һ�ֽ��շ���
* ��ڲ���: 
* ��������: 
*****************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance==USART1)//����Ǵ���1
	{
        USART_RX_BUF[USART_RX_CNT] = My_RxBuffer[0] ;
        USART_RX_CNT++;
        if(USART_RX_CNT >= 1024)
        {
            memcpy((uint8_t *)&MyIapRxBuff.WriteBuff[0], (uint8_t *)&USART_RX_BUF[0], USART_RX_CNT);
            MyIapRxBuff.WriteStep = 1;
            MyIapRxBuff.WriteSize = USART_RX_CNT;
            USART_RX_CNT = 0;
        }
	}
}
	


