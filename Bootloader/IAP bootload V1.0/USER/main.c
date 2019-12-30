#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "stmflash.h"   
#include "iap.h" 

/************************************************
 ALIENTEK NANO��STM32F103������ ʵ��25
 ����IAPʵ��-HAL��汾
 ����֧�֣�www.openedv.com
 �Ա����̣� http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/

//FLASH����: 0X0800 0000~0x0800 2800,��IAPʹ��,��10K�ֽ�,FLASH APP����128-10=118KB����.
// SRAM����: 0X2000 1000����ʼ,���ڴ��SRAM IAP����,��16K�ֽڿ���,�û��������ж�16K�ռ����ROM��RAM���ķ���
//           �ر�ע�⣬SRAM APP��ROMռ�������ܴ���10K�ֽڣ���Ϊ�����������һ�ν���10K�ֽڣ����ܳ���������ơ�

int main(void)
{
	u8 t;
	u8 key;
	u16 oldcount=0;	//�ϵĴ��ڽ�������ֵ
	u16 applenth=0;	//���յ���app���볤��

    HAL_Init();                    	//��ʼ��HAL��    
    Stm32_Clock_Init(RCC_PLL_MUL9); //����ʱ��,72M
    delay_init(72);                 //��ʼ����ʱ����
	uart_init(256000);	 	        //���ڳ�ʼ��Ϊ256000
	LED_Init();		  		        //��ʼ����LED���ӵ�Ӳ���ӿ�
 	KEY_Init();				        //������ʼ��    
	printf("NANO STM32\r\n"); 
	printf("IAP TEST\r\n"); 
    printf("WK_UP:Copy APP2FLASH\r\n"); 
    printf("KEY0:Run SRAM APP\r\n"); 
    printf("KEY1:Run FLASH APP\r\n"); 	
	while(1)
	{
		if(USART_RX_CNT)
		{
			if(oldcount==USART_RX_CNT)//��������,û���յ��κ�����,��Ϊ�������ݽ������.
			{
				applenth=USART_RX_CNT;
				oldcount=0;
				USART_RX_CNT=0;
				printf("�û�����������!\r\n");
				printf("���볤��:%dBytes\r\n",applenth);
			}else oldcount=USART_RX_CNT;			
		}
		t++;
		delay_ms(10);
		if(t==30)
		{
			LED0=!LED0;//LED0��˸��ʾ����������
			t=0;
		}	  	 
		key=KEY_Scan(0);
		if(key==WKUP_PRES) //WK_UP��������
		{
			if(applenth)
			{
				printf("��ʼ���¹̼�...\r\n");	
 				if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
				{	 
					iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);//����FLASH����   
					printf("�̼��������!\r\n");	
				}else 
				{	   
					printf("��FLASHӦ�ó���!\r\n");
				}
 			}else 
			{
				printf("û�п��Ը��µĹ̼�!\r\n");
			}									 
		} 
		if(key==KEY1_PRES) //KEY1��������
		{
			printf("��ʼִ��FLASH�û�����!!\r\n");
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
			{	 
				iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
			}else 
			{
				printf("��FLASHӦ�ó���,�޷�ִ��!\r\n"); 
                printf("\r\n");				
			}									   
		}
		if(key==KEY0_PRES) //KEY0��������
		{
			printf("��ʼִ��SRAM�û�����!!\r\n");
			if(((*(vu32*)(0X20001000+4))&0xFF000000)==0x20000000)//�ж��Ƿ�Ϊ0X20XXXXXX.
			{	 
				iap_load_app(0X20001000);//SRAM��ַ
			}else 
			{
				printf("��SRAMӦ�ó���,�޷�ִ��!\r\n");	
                printf("\r\n");					
			}									 	 
		}				   
		
	}
    
}


