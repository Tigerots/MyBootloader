
/*
用于存放STH1x的底层驱动


*/

#define __STH10_C
#include <includes.h>


#define SPI_SCK	  GPIO_Pin_12	  
#define SPI_DAT   GPIO_Pin_11	

/*******************************************************************************
* Function Name  : Flash_GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void STH_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
    
  GPIO_InitStructure.GPIO_Pin = SPI_DAT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;//双向口
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = SPI_SCK;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_ResetBits(GPIOC, SPI_SCK);
  GPIO_ResetBits(GPIOC, SPI_DAT);
}

/*******************************************************************************
* Function Name  : Flash_CS
* Description    : Spi Flash Chip Select
* Input          : NewState:DISABLE or ENABLE
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_CS(FunctionalState NewState)
{
	if (NewState != DISABLE)
	{
        //CS
	}
	else
	{
		GPIO_ResetBits(GPIOC, SPI_SCK);
		GPIO_ResetBits(GPIOC, SPI_DAT);
	}
}
/*******************************************************************************
* Function Name  : SPI_ReadByte
* Description    : Receive one Byte From FLASH controller 
* Input          : None
* Output         : None
* Return         : Data Readed
*******************************************************************************/
unsigned char SPI_ReadByte (void)
{
    unsigned char i=0;
    unsigned char data=0;


	for(i=0;i<8;i++)
	{
        data = (data<<1) ;
        GPIO_ResetBits(GPIOC, SPI_SCK);
        Delay(10);        
        GPIO_SetBits(GPIOC, SPI_SCK);
        
        if(GPIO_ReadInputDataBit(GPIOC, SPI_DAT))  
        {
            data |= 0x01;  
        }
	}
    GPIO_ResetBits(GPIOC, SPI_SCK);
	return data;
}
/*******************************************************************************
* Function Name  : SPI_SendByte
* Description    : Write one Byte to FLASH controller 
* Input          : Data
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_SendByte (unsigned char data_out)
{
    unsigned char i;

    for (i=0; i<8; i++)                 
    {
        
		if((data_out&0x80)==0)
        {
            GPIO_ResetBits(GPIOC, SPI_DAT);
        }
	    else
        {
            GPIO_SetBits(GPIOC, SPI_DAT);
        }
	    data_out<<=1;  
	 	GPIO_SetBits(GPIOC, SPI_SCK);
        Delay(10);
		GPIO_ResetBits(GPIOC, SPI_SCK);
        Delay(10);
    }
    for(i=0;i<100;i++)
    {
        Delay(10);
        if(0 == GPIO_ReadInputDataBit(GPIOC, SPI_DAT))  
        {
            break;
        }
    }
    GPIO_SetBits(GPIOC, SPI_SCK);
    Delay(10);
    GPIO_ResetBits(GPIOC, SPI_SCK);
    Delay(10);
}

//应答
void Sht_ACK(void)
{
    GPIO_ResetBits(GPIOC, SPI_SCK);
    Delay(10);
    GPIO_ResetBits(GPIOC, SPI_DAT);
    Delay(10);
    GPIO_SetBits(GPIOC, SPI_SCK);
    Delay(10);
    GPIO_ResetBits(GPIOC, SPI_SCK);
    Delay(10);
    GPIO_SetBits(GPIOC, SPI_DAT);
}

//非应答
void Sht_NOACK(void)
{
    GPIO_ResetBits(GPIOC, SPI_SCK);
    Delay(10);
    GPIO_SetBits(GPIOC, SPI_DAT);
    Delay(10);
    GPIO_SetBits(GPIOC, SPI_SCK);
    Delay(10);
    GPIO_ResetBits(GPIOC, SPI_SCK);
}

//启动传感器
void StartUPSth1x(void)
{
    GPIO_ResetBits(GPIOC, SPI_SCK);
    Delay(10);
    GPIO_SetBits(GPIOC, SPI_DAT);
    GPIO_SetBits(GPIOC, SPI_SCK);
    Delay(10);
    
    GPIO_ResetBits(GPIOC, SPI_DAT);
    Delay(10);
    GPIO_ResetBits(GPIOC, SPI_SCK);
    Delay(10);
    GPIO_SetBits(GPIOC, SPI_SCK);
    Delay(10);
    
    GPIO_SetBits(GPIOC, SPI_DAT);
    Delay(10);
    GPIO_ResetBits(GPIOC, SPI_SCK);
    Delay(10);
}

//DATA保持高，SCK时钟触发9次，发送启动传输，通迅即复位
void CommReset(void)
{
	unsigned char i;
    
	GPIO_SetBits(GPIOC, SPI_DAT);
	GPIO_ResetBits(GPIOC, SPI_SCK);
	for(i=0;i<9;i++)
	{
		GPIO_SetBits(GPIOC, SPI_SCK);
        Delay(10);
		GPIO_ResetBits(GPIOC, SPI_SCK);
        Delay(10);
	}
    StartUPSth1x();//启动传输
}

//软件复位
void SoftReset(void)
{    
	CommReset();
    StartUPSth1x();//启动传输
    SPI_SendByte(0x1e);//复位指令
}

//初始化
void InitSTH1x(void)
{
    STH_GPIO_Configuration();
    SoftReset();//芯片复位
    CommReset();//通信复位
    OSTimeDlyHMSM(0, 0, 0, 100);;//上电后11mS延时
}


float Sth1xReg(void)
{
    u32  p_value;
    u32  i;
    u32 p_checksum=0;

	StartUPSth1x();//传输开始
    SPI_SendByte(0x06);//测湿度
  
    for(i=0; i<65535; i++)
    {
        OSTimeDlyHMSM(0, 0, 0, 10);
        if(0 == GPIO_ReadInputDataBit(GPIOC, SPI_DAT)) 
        {
            break;
        }
    }
	SPI_SendByte(0);//测湿度
	for(i=0; i<65535; i++)
    {
        OSTimeDlyHMSM(0, 0, 0, 10);
        if(0 == GPIO_ReadInputDataBit(GPIOC, SPI_DAT)) 
        {
            break;
        }
    }
	
	
    return (1);
}



//读取温度,湿度采集值并转化成浮点数
float Sth1xMeasure(u8 mType)
{
    u32  p_value;
    u32  i;
    u32 p_checksum=0;
	
	
	Sth1xReg();
	
	
    StartUPSth1x();//传输开始
    switch(mType)
    {
        case 1:
             SPI_SendByte(0x03);//测温度
             break;
        case 2:
             SPI_SendByte(0x05);//测湿度
             break;
        default:
             break;
    }
    OSTimeDlyHMSM(0, 0, 1, 0);
    for(i=0; i<65535; i++)
    {
        OSTimeDlyHMSM(0, 0, 0, 10);
        if(0 == GPIO_ReadInputDataBit(GPIOC, SPI_DAT)) 
        {
            break;
        }
    }

    p_value=SPI_ReadByte();
    p_value=p_value<<8;
    Sht_ACK();
    p_value=p_value+SPI_ReadByte();
    Sht_ACK();
    p_checksum=SPI_ReadByte();
    Sht_NOACK();
    
    if(p_checksum == 0)
    {//暂时未做校验,后续添加
        return 0;
    }

	
    //下述公式需要验证
    if(mType==1)
    {
        Sth10_Temperature.f32 = p_value*0.01-39.7;
    }
    if(mType==2)
    {
//        Sth10_Humidity.f32 = (-0.0000015955)*p_value*p_value + (0.0367)*p_value - 2.0468;
//        Sth10_Humidity.f32 = Sth10_Humidity.f32 + (Sth10_Temperature.f32-25)*(0.01+0.00008*p_value);//温度补偿系数
        
        
		Sth10_Humidity.f32 = (-0.0000028)*p_value*p_value + (0.0405)*p_value - 4;
        Sth10_Humidity.f32 = Sth10_Humidity.f32 + (Sth10_Temperature.f32-25)*(0.01+0.00008*p_value);//温度补偿系数
	}
    return (1);
}






