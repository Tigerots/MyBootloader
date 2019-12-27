/*
用于存放STH1x的底层驱动


*/

#define __BMP085_C
#include <includes.h>


#define BMP085_I2C_SCL	  GPIO_Pin_6//PB6
#define BMP085_I2C_SDA	  GPIO_Pin_7//PB7

//数据传输位置 设备地址(+W/R)+寄存器地址+控制字
#define BMP085_SLAVE_ADDRESS    0xEE//模块地址+W
//寄存器地址说明
/*
温度              0x2E        4.5ms
压力精度0         0x34        4.5ms
压力精度1         0x74        7.5ms
压力精度2         0xB4        13.5ms
压力精度3         0xF4        25.5ms
*/
short ac1;
short ac2; 
short ac3; 
unsigned short ac4;
unsigned short ac5;
unsigned short ac6;
short b1; 
short b2;
short mb;
short mc;
short md;

/*******************************************************************************
* Function Name  : BMP085_Init
* Description    : 初始化I2C硬件设备
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void BMP085_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin  = BMP085_I2C_SCL;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		//速度低，改善EMI
  	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = BMP085_I2C_SDA;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		//速度低，改善EMI
  	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, BMP085_I2C_SCL);
	GPIO_SetBits(GPIOB, BMP085_I2C_SDA);
    
    Delay(50);
    Bmp085_ConvertInit();
}
/*********************************************
内部函数，SDA电平控制
********************************************/
void BMP085_I2C_SDA_Out(u8 c)
{
	if(c)   GPIO_SetBits(GPIOB, BMP085_I2C_SDA);
	else    GPIO_ResetBits(GPIOB, BMP085_I2C_SDA);
		
}
/*********************************************
内部函数，SCL电平控制
********************************************/
void BMP085_I2C_SCL_Out(u8 c)
{
	if(c)   GPIO_SetBits(GPIOB, BMP085_I2C_SCL);
	else    GPIO_ResetBits(GPIOB, BMP085_I2C_SCL);
}
/*********************************************
内部函数，I2C开始
********************************************/
void BMP085_I2C_Start()
{ 
	BMP085_I2C_SDA_Out(1);
	Delay(10);
	BMP085_I2C_SCL_Out(1);
    Delay(40);
    BMP085_I2C_SDA_Out(0);
    Delay(40);
    BMP085_I2C_SCL_Out(0);
	Delay(10);
}
/********************************************
内部函数，I2C结束
********************************************/
void BMP085_I2C_Stop()
{
    BMP085_I2C_SDA_Out(0);
    Delay(10);
    BMP085_I2C_SCL_Out(1);
    Delay(40);
	BMP085_I2C_SDA_Out(1);
    Delay(40);

}
/********************************************
内部函数，输出ACK ,每个字节传输完成，输出ack=0,结束读数据，ack=1;
********************************************/
void BMP085_I2C_WriteACK(u8 ack)
{
	BMP085_I2C_SDA_Out(ack);
    Delay(30);
    BMP085_I2C_SCL_Out(1);
    Delay(30);
    BMP085_I2C_SCL_Out(0);
	Delay(20);
}
/********************************************
内部函数，等待ACK
********************************************/
void BMP085_I2C_WaitACK()
{   
    u8 errtime=50;
    BMP085_I2C_SDA_Out(1);
    Delay(50); /*读ACK*/
    BMP085_I2C_SCL_Out(1);
    Delay(50);
    while(GPIO_ReadInputDataBit(GPIOB, BMP085_I2C_SDA))
    {   
        errtime--;
        if(errtime==0)
        {
            BMP085_I2C_Stop();
            return;
        }
    }
	BMP085_I2C_SCL_Out(0);
    Delay(50);
}
/********************************************
内部函数.向Dev写数据字节
入口:B=数据
********************************************/
void BMP085_I2C_WriteByte(u8 wdata)
{
    u8 i;
    for(i=0;i<8;i++)
    {
        if(wdata&0x80) 
            BMP085_I2C_SDA_Out(1);
        else 
            BMP085_I2C_SDA_Out(0);
        Delay(20);
		wdata<<=1;
        BMP085_I2C_SCL_Out(1);
        Delay(40);
        BMP085_I2C_SCL_Out(0);
		Delay(20);
    }
    BMP085_I2C_WaitACK();      //I2C器件或通讯出错，将会退出I2C通讯
}
/********************************************
内部函数:从Dev读数据字节
出口:B
********************************************/
u8 BMP085_I2C_Readbyte()
{
    u8 i=0,bytedata=0;

    BMP085_I2C_SDA_Out(1);
    for(i=0;i<8;i++)
    {
	   Delay(10);
	   BMP085_I2C_SCL_Out(0);
       Delay(10);
	   BMP085_I2C_SCL_Out(1); 
	   Delay(20);
       bytedata<<=1;
	   if(GPIO_ReadInputDataBit(GPIOB, BMP085_I2C_SDA))
       		bytedata|=0x01;
       Delay(40);
    }
	BMP085_I2C_SCL_Out(0);
	Delay(10);
    return bytedata;
}
/*******************************************************************************
* Function Name  : BMP085_WriteCmd
* Description    : 向指定地址写命令或数据
* Input          : 指定地址及数据
* Output         : None
* Return         : None
*******************************************************************************/
void BMP085_WriteCmd(u8 address,u8 mdata)
{
    BMP085_I2C_Start();
    BMP085_I2C_WriteByte(BMP085_SLAVE_ADDRESS);//设备地址+写命令
    BMP085_I2C_WriteByte(address);//写寄存器地址
    BMP085_I2C_WriteByte(mdata);
    //写寄存器数据
    BMP085_I2C_Stop();
}
/*******************************************************************************
* Function Name  : BMP085_ReadData
* Description    : 从指定地址读出一个字节的数据
* Input          : 指定地址
* Output         : None
* Return         : None
*******************************************************************************/
u8 BMP085_ReadData(u8 address) 
{   
    u8 rdata;
    BMP085_I2C_Start();
    BMP085_I2C_WriteByte(BMP085_SLAVE_ADDRESS);//设备地址+写命令
    BMP085_I2C_WriteByte(address); //写寄存器地址
  
	BMP085_I2C_Start();
    BMP085_I2C_WriteByte(BMP085_SLAVE_ADDRESS+1); 	//读命令
    rdata	=	BMP085_I2C_Readbyte();
    BMP085_I2C_WriteACK(1);
    BMP085_I2C_Stop();
    return rdata;
}
/*******************************************************************************
* Function Name  : BMP085_ReadDataBuffer
* Description    : 从指定地址读出多个字节的数据
* Input          : address:指定地址
*					 count:要读出多少个字节
                    buff:数据存储缓存
* Output         : None
* Return         : None
*******************************************************************************/
void BMP085_ReadDataBuffer(u8 address, u8 count, u8 *buff)
{   
    u8 i;
    BMP085_I2C_Start();
    BMP085_I2C_WriteByte(BMP085_SLAVE_ADDRESS);//写命令
    BMP085_I2C_WriteByte(address);//写地址
    
    BMP085_I2C_Start();
    BMP085_I2C_WriteByte(BMP085_SLAVE_ADDRESS+1);	//读命令
    for(i=0; i<count; i++)
    {
         buff[i]=BMP085_I2C_Readbyte();
         if(i<count-1) 
             BMP085_I2C_WriteACK(0);
    }
   BMP085_I2C_WriteACK(1);
   BMP085_I2C_Stop();
}

void BMP085_ReadData2Byte(u8 address, u8 count, u8 *buff)
{   
    BMP085_I2C_Start();
    BMP085_I2C_WriteByte(BMP085_SLAVE_ADDRESS);//写命令
    BMP085_I2C_WriteByte(address);//写地址

    BMP085_I2C_Start();
    BMP085_I2C_WriteByte(BMP085_SLAVE_ADDRESS+1);	//读命令

    buff[1]=BMP085_I2C_Readbyte();
    BMP085_I2C_WriteACK(0);
    buff[0]=BMP085_I2C_Readbyte();
    BMP085_I2C_WriteACK(1);
    BMP085_I2C_Stop();
}

/****************************************************************************
* 功  能: 读取温度值
* 输  入: 
* 输  出: 
* 返  回: 温度值
*******************************************************************************/
u16 BMP085_ReadTemp(void)
{
    u8 Temp[5];
    u16 ret;
    
    BMP085_I2C_Start();
    BMP085_I2C_WriteByte(BMP085_SLAVE_ADDRESS);//写命令
    BMP085_I2C_WriteByte(0xF4);//寄存器地址
    BMP085_I2C_WriteByte(0x2E);//寄存器数据
    BMP085_I2C_Stop();
    
     OSTimeDlyHMSM(0, 0, 0, 100);//最大延时4.5ms
    
    BMP085_ReadDataBuffer(0xF6, 2, (u8 *)&Temp[0]);
    
    ret = (Temp[0]<<8) | Temp[1];
    return ret;    
}

/****************************************************************************
* 功  能: 读取度值
* 输  入: 
* 输  出: 
* 返  回: 温度值
*******************************************************************************/
u16 BMP085_ReadPressure(void)
{
    u8 Temp[5];
    u16 ret;
    
    BMP085_I2C_Start();
    BMP085_I2C_WriteByte(BMP085_SLAVE_ADDRESS);//写命令
    BMP085_I2C_WriteByte(0xF4);//寄存器地址
    BMP085_I2C_WriteByte(0xF4);//寄存器数据
    BMP085_I2C_Stop();
    
    OSTimeDlyHMSM(0, 0, 0, 100);//最大延时25.5ms
    
    BMP085_ReadDataBuffer(0xF6, 2, (u8 *)&Temp[0]);
    
    ret = (Temp[0]<<8) | Temp[1];
    return ret;   
}

//读芯片中的系数
void Bmp085_ConvertInit(void)
{
    BMP085_ReadData2Byte(0xAA, 2, (u8 *)&ac1);Delay(10);
    BMP085_ReadData2Byte(0xAC, 2, (u8 *)&ac2);Delay(10);
    BMP085_ReadData2Byte(0xAE, 2, (u8 *)&ac3);Delay(10);
    BMP085_ReadData2Byte(0xB0, 2, (u8 *)&ac4);Delay(10);
    BMP085_ReadData2Byte(0xB2, 2, (u8 *)&ac5);Delay(10);
    BMP085_ReadData2Byte(0xB4, 2, (u8 *)&ac6);Delay(10);
    BMP085_ReadData2Byte(0xB6, 2, (u8 *)&b1);Delay(10);
    BMP085_ReadData2Byte(0xB8, 2, (u8 *)&b2);Delay(10);
    BMP085_ReadData2Byte(0xBA, 2, (u8 *)&mb);Delay(10);
    BMP085_ReadData2Byte(0xBC, 2, (u8 *)&mc);Delay(10);
    BMP085_ReadData2Byte(0xBE, 2, (u8 *)&md);Delay(10);
}


//读气压和温度值
#define OSS 0
void Bmp085_Convert(void)
{
    unsigned int ut;
    unsigned long up;
    long x1, x2, b5, b6, x3, b3, p;
    unsigned long b4, b7;
	
	ut = BMP085_ReadTemp();	   // 读取温度
//    ut = 27898;
	
	x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
	x2 = ((long) mc << 11) / (x1 + md);
	b5 = x1 + x2;
	Bmp085_Temperature = ((b5 + 8) >> 4);

    
    up = BMP085_ReadPressure();
    b6 = b5 - 4000;
    // Calculate B3
    x1 = (b2 * (b6 * b6)>>12)>>11;
    x2 = (ac2 * b6)>>11;
    x3 = x1 + x2;
    b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;

    // Calculate B4
    x1 = (ac3 * b6)>>13;
    x2 = (b1 * ((b6 * b6)>>12))>>16;
    x3 = ((x1 + x2) + 2)>>2;
    b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;

    b7 = ((unsigned long)(up - b3) * (50000>>OSS));
    if (b7 < 0x80000000)
    p = (b7<<1)/b4;
    else
    p = (b7/b4)<<1;

    x1 = (p>>8) * (p>>8);
    x1 = (x1 * 3038)>>16;
    x2 = (-7357 * p)>>16;
    Bmp085_Pressure.f32 = p+((x1 + x2 + 3791)>>4);
    Bmp085_Pressure.f32 = Bmp085_Pressure.f32/1000;
}



/*
我是新手，在用BMP气压计测量海拔高度时 用POW（）函数遇到了点问题，现已经 解决。把这个分享给像我这样的新手，这个气压与海拔换算公式
altitude = 44330 * (1.0 - pow(P / P0, 0.1903)); P0是海拔大气压，P为你测的当地大气压。可是有时得不到正确的结果！问题在这里，
这里的P 必须是浮点数才能算出正确的结果，大家切忌！
我是KEIL编译，不知道其他的编译会不会这样，这就不讨论了。


*/



//------------------------end-----------------------------------
