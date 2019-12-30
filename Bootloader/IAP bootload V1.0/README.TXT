实验器材:
	NANO STM32F103开发板V1版本
	
实验目的:
	学习STM32的IAP功能,掌握IAP的使用.
	
硬件资源:
	1,DS0(连接在PC0) 
	2,串口1(波特率:25600)
	3,按键KEY0(PC8)/KEY1(PC9)/KEY_UP(PA0,也称之为WK_UP)  
		
实验现象:
	本实验开机的时候先显示提示信息，然后等待串口输入接收APP程序（无校验，一次性接收），在串口接收
	到APP程序之后，即可执行IAP。如果是SRAM APP，通过按下KEY0即可执行这个收到的SRAM APP程序。如果
	是FLASH APP，则需要先按下KEY_UP按键，将串口接收到的APP程序存放到STM32的FLASH，之后再按KEY1即
	可以执行这个FLASH APP程序。
	
注意事项:  
	1,本实验串口1波特率需要设置为：25600！！！
	2,本实验下载成功后,需用串口调试助手发送SRAM APP/FLASH APP等APP代码(.bin文件)验证IAP功能. 
	3,SRAM APP代码的起始地址必须是：0X20001000，FLASH APP代码的起始地址必须是：0x08002800.
	4,大家在创建APP代码的时候，切记要设置中断向量偏移量！！
	  

					正点原子@ALIENTEK
					2018-3-28
					广州市星翼电子科技有限公司
					电话：020-38271790
					传真：020-36773971
					购买：http://shop62103354.taobao.com
					http://shop62057469.taobao.com
					公司网站：www.alientek.com
					技术论坛：www.openedv.com
					
					
					
					
					
					
					
					
					