/*
���ڴ��STH1x�ĵײ�����


*/

#define __BMP085_C
#include <includes.h>


#define BMP085_I2C_SCL	  GPIO_Pin_6//PB6
#define BMP085_I2C_SDA	  GPIO_Pin_7//PB7

//���ݴ���λ�� �豸��ַ(+W/R)+�Ĵ�����ַ+������
#define BMP085_SLAVE_ADDRESS    0xEE//ģ���ַ+W
//�Ĵ�����ַ˵��
/*
�¶�              0x2E        4.5ms
ѹ������0         0x34        4.5ms
ѹ������1         0x74        7.5ms
ѹ������2         0xB4        13.5ms
ѹ������3         0xF4        25.5ms
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
* Description    : ��ʼ��I2CӲ���豸
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
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		//�ٶȵͣ�����EMI
  	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = BMP085_I2C_SDA;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;		//�ٶȵͣ�����EMI
  	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, BMP085_I2C_SCL);
	GPIO_SetBits(GPIOB, BMP085_I2C_SDA);
    
    Delay(50);
    Bmp085_ConvertInit();
}
/*********************************************
�ڲ�������SDA��ƽ����
********************************************/
void BMP085_I2C_SDA_Out(u8 c)
{
	if(c)   GPIO_SetBits(GPIOB, BMP085_I2C_SDA);
	else    GPIO_ResetBits(GPIOB, BMP085_I2C_SDA);
		
}
/*********************************************
�ڲ�������SCL��ƽ����
********************************************/
void BMP085_I2C_SCL_Out(u8 c)
{
	if(c)   GPIO_SetBits(GPIOB, BMP085_I2C_SCL);
	else    GPIO_ResetBits(GPIOB, BMP085_I2C_SCL);
}
/*********************************************
�ڲ�������I2C��ʼ
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
�ڲ�������I2C����
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
�ڲ����������ACK ,ÿ���ֽڴ�����ɣ����ack=0,���������ݣ�ack=1;
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
�ڲ��������ȴ�ACK
********************************************/
void BMP085_I2C_WaitACK()
{   
    u8 errtime=50;
    BMP085_I2C_SDA_Out(1);
    Delay(50); /*��ACK*/
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
�ڲ�����.��Devд�����ֽ�
���:B=����
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
    BMP085_I2C_WaitACK();      //I2C������ͨѶ���������˳�I2CͨѶ
}
/********************************************
�ڲ�����:��Dev�������ֽ�
����:B
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
* Description    : ��ָ����ַд���������
* Input          : ָ����ַ������
* Output         : None
* Return         : None
*******************************************************************************/
void BMP085_WriteCmd(u8 address,u8 mdata)
{
    BMP085_I2C_Start();
    BMP085_I2C_WriteByte(BMP085_SLAVE_ADDRESS);//�豸��ַ+д����
    BMP085_I2C_WriteByte(address);//д�Ĵ�����ַ
    BMP085_I2C_WriteByte(mdata);
    //д�Ĵ�������
    BMP085_I2C_Stop();
}
/*******************************************************************************
* Function Name  : BMP085_ReadData
* Description    : ��ָ����ַ����һ���ֽڵ�����
* Input          : ָ����ַ
* Output         : None
* Return         : None
*******************************************************************************/
u8 BMP085_ReadData(u8 address) 
{   
    u8 rdata;
    BMP085_I2C_Start();
    BMP085_I2C_WriteByte(BMP085_SLAVE_ADDRESS);//�豸��ַ+д����
    BMP085_I2C_WriteByte(address); //д�Ĵ�����ַ
  
	BMP085_I2C_Start();
    BMP085_I2C_WriteByte(BMP085_SLAVE_ADDRESS+1); 	//������
    rdata	=	BMP085_I2C_Readbyte();
    BMP085_I2C_WriteACK(1);
    BMP085_I2C_Stop();
    return rdata;
}
/*******************************************************************************
* Function Name  : BMP085_ReadDataBuffer
* Description    : ��ָ����ַ��������ֽڵ�����
* Input          : address:ָ����ַ
*					 count:Ҫ�������ٸ��ֽ�
                    buff:���ݴ洢����
* Output         : None
* Return         : None
*******************************************************************************/
void BMP085_ReadDataBuffer(u8 address, u8 count, u8 *buff)
{   
    u8 i;
    BMP085_I2C_Start();
    BMP085_I2C_WriteByte(BMP085_SLAVE_ADDRESS);//д����
    BMP085_I2C_WriteByte(address);//д��ַ
    
    BMP085_I2C_Start();
    BMP085_I2C_WriteByte(BMP085_SLAVE_ADDRESS+1);	//������
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
    BMP085_I2C_WriteByte(BMP085_SLAVE_ADDRESS);//д����
    BMP085_I2C_WriteByte(address);//д��ַ

    BMP085_I2C_Start();
    BMP085_I2C_WriteByte(BMP085_SLAVE_ADDRESS+1);	//������

    buff[1]=BMP085_I2C_Readbyte();
    BMP085_I2C_WriteACK(0);
    buff[0]=BMP085_I2C_Readbyte();
    BMP085_I2C_WriteACK(1);
    BMP085_I2C_Stop();
}

/****************************************************************************
* ��  ��: ��ȡ�¶�ֵ
* ��  ��: 
* ��  ��: 
* ��  ��: �¶�ֵ
*******************************************************************************/
u16 BMP085_ReadTemp(void)
{
    u8 Temp[5];
    u16 ret;
    
    BMP085_I2C_Start();
    BMP085_I2C_WriteByte(BMP085_SLAVE_ADDRESS);//д����
    BMP085_I2C_WriteByte(0xF4);//�Ĵ�����ַ
    BMP085_I2C_WriteByte(0x2E);//�Ĵ�������
    BMP085_I2C_Stop();
    
     OSTimeDlyHMSM(0, 0, 0, 100);//�����ʱ4.5ms
    
    BMP085_ReadDataBuffer(0xF6, 2, (u8 *)&Temp[0]);
    
    ret = (Temp[0]<<8) | Temp[1];
    return ret;    
}

/****************************************************************************
* ��  ��: ��ȡ��ֵ
* ��  ��: 
* ��  ��: 
* ��  ��: �¶�ֵ
*******************************************************************************/
u16 BMP085_ReadPressure(void)
{
    u8 Temp[5];
    u16 ret;
    
    BMP085_I2C_Start();
    BMP085_I2C_WriteByte(BMP085_SLAVE_ADDRESS);//д����
    BMP085_I2C_WriteByte(0xF4);//�Ĵ�����ַ
    BMP085_I2C_WriteByte(0xF4);//�Ĵ�������
    BMP085_I2C_Stop();
    
    OSTimeDlyHMSM(0, 0, 0, 100);//�����ʱ25.5ms
    
    BMP085_ReadDataBuffer(0xF6, 2, (u8 *)&Temp[0]);
    
    ret = (Temp[0]<<8) | Temp[1];
    return ret;   
}

//��оƬ�е�ϵ��
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


//����ѹ���¶�ֵ
#define OSS 0
void Bmp085_Convert(void)
{
    unsigned int ut;
    unsigned long up;
    long x1, x2, b5, b6, x3, b3, p;
    unsigned long b4, b7;
	
	ut = BMP085_ReadTemp();	   // ��ȡ�¶�
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
�������֣�����BMP��ѹ�Ʋ������θ߶�ʱ ��POW�������������˵����⣬���Ѿ� ����������������������������֣������ѹ�뺣�λ��㹫ʽ
altitude = 44330 * (1.0 - pow(P / P0, 0.1903)); P0�Ǻ��δ���ѹ��PΪ���ĵ��ش���ѹ��������ʱ�ò�����ȷ�Ľ�������������
�����P �����Ǹ��������������ȷ�Ľ��������мɣ�
����KEIL���룬��֪�������ı���᲻����������Ͳ������ˡ�


*/



//------------------------end-----------------------------------
