
#define	 MYMBI5024

#include <includes.h>
#include "MBI5024Dev.h"

void RowCtl(u8 Row, u8 sig)
{
    switch(Row)
    {
        case 1:
            if(sig == 0)
            {
                GPIO_ResetBits(GPIOA, GPIO_Pin_8);
            }
            else
            {
                GPIO_SetBits(GPIOA, GPIO_Pin_8);
            }
        break;
        case 2:
            if(sig == 0)
            {
                GPIO_ResetBits(GPIOA, GPIO_Pin_11);
            }
            else
            {
                GPIO_SetBits(GPIOA, GPIO_Pin_11);
            }
        break;
        case 3:
            if(sig == 0)
            {
                GPIO_ResetBits(GPIOA, GPIO_Pin_12);
            }
            else
            {
                GPIO_SetBits(GPIOA, GPIO_Pin_12);
            }
        break;
        case 4:
            if(sig == 0)
            {
                GPIO_ResetBits(GPIOC, GPIO_Pin_10);
            }
            else
            {
                GPIO_SetBits(GPIOC, GPIO_Pin_10);
            }
        break;    
        case 5:
            if(sig == 0)
            {
                GPIO_ResetBits(GPIOC, GPIO_Pin_11);
            }
            else
            {
                GPIO_SetBits(GPIOC, GPIO_Pin_11);
            }
        break;
        case 6:
            if(sig == 0)
            {
                GPIO_ResetBits(GPIOC, GPIO_Pin_12);
            }
            else
            {
                GPIO_SetBits(GPIOC, GPIO_Pin_12);
            }
        break;    
        case 7:
            if(sig == 0)
            {
                GPIO_ResetBits(GPIOD, GPIO_Pin_2);
            }
            else
            {
                GPIO_SetBits(GPIOD, GPIO_Pin_2);
            }
        break;   
        case 8:
            if(sig == 0)
            {
                GPIO_ResetBits(GPIOB, GPIO_Pin_5);
            }
            else
            {
                GPIO_SetBits(GPIOB, GPIO_Pin_5);
            }
        break;
        case 9:
            if(sig == 0)
            {
                GPIO_ResetBits(GPIOB, GPIO_Pin_6);
            }
            else
            {
                GPIO_SetBits(GPIOB, GPIO_Pin_6);
            }
        break;
        case 10:
            if(sig == 0)
            {
                GPIO_ResetBits(GPIOB, GPIO_Pin_7);
            }
            else
            {
                GPIO_SetBits(GPIOB, GPIO_Pin_7);
            }
        break;
        case 11:
            if(sig == 0)
            {
                GPIO_ResetBits(GPIOB, GPIO_Pin_8);
            }
            else
            {
                GPIO_SetBits(GPIOB, GPIO_Pin_8);
            }
        break;
        case 12:
            if(sig == 0)
            {
                GPIO_ResetBits(GPIOB, GPIO_Pin_9);
            }
            else
            {
                GPIO_SetBits(GPIOB, GPIO_Pin_9);
            }
        break;
        default:
            break;
    }
}

void RedSDI(u8 sig)
{
    if(sig == 0)
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_12);
    }
    else
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_12);
    }
}
void GreenSDI(u8 sig)
{
    if(sig == 0)
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_14);
    }
    else
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_14);
    }
}
void nOERed(u8 sig)
{
    if(sig == 0)
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_13);
    }
    else
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_13);
    }
}
void nOEGreen(u8 sig)
{
    if(sig == 0)
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_7);
    }
    else
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_7);
    }
}
void CLK(u8 sig)
{
    if(sig == 0)
    {
        GPIO_ResetBits(GPIOB, GPIO_Pin_15);
    }
    else
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_15);
    }
}
void LE(u8 sig)
{
    if(sig == 0)
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_6);
    }
    else
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_6);
    }
}

void MBI5024WriteByteRed(u16 RedDat)
{
    u8 i;
    for(i=0;i<16;i++)
    {
        RedSDI((RedDat & 0x8000) ? 1:0);
        CLK(1);
        RedDat <<= 1;
        CLK(0);
    }
//    LE(1);
//    OSTimeDlyHMSM(0,0,0,10);
//    LE(0);
}

void MBI5024WriteByteGreen(u16 GreenDat)
{
    u8 i;
    for(i=0;i<16;i++)
    {
        GreenSDI((GreenDat & 0x8000) ? 1:0);
        CLK(1);
        GreenDat <<= 1;
        CLK(0);
    }
//    LE(1);
//    OSTimeDlyHMSM(0,0,0,10);
//    LE(0);
}

void DisplyRed(u16 RedDat1, u16 RedDat2)
{
    nOERed(1);   
    MBI5024WriteByteRed(RedDat1);
    MBI5024WriteByteRed(RedDat2);
    LE(1);
    LE(0);
    nOERed(0); 
}
void DisplyGreen(u16 GreenDat1, u16 GreenDat2)
{
    nOEGreen(1);   
    MBI5024WriteByteGreen(GreenDat1);
    MBI5024WriteByteGreen(GreenDat2);
    LE(1);
    LE(0);
    nOEGreen(0); 
}

void DisEnRed(void)
{
    nOERed(1);
}
void DisEnGreen(void)
{
    nOEGreen(1);
}


//��(0) ��(1) ͣ(2) ʻ(3) ��(4) ��(5) ��(6) ��(7)
//���� ���� ˳�� ʮ������ C51��ʽ ����Ĭ��ֵ
//���ֽ�һ����,12����,��һ���ֽڵ���λû������
u8 HZ[16][24] = 
{
    {0x11,0x00,0x21,0x00,0x47,0xC0,0x91,0x00,0x2F,0xE0,0x60,0x80,0xAF,0xE0,0x20,0x80,0x24,0x80,0x22,0x80,0x20,0x80,0x23,0x80},/*"��",0*/
    {0x20,0x00,0x27,0x80,0x24,0x80,0xF4,0x80,0x24,0x80,0x74,0x80,0x6C,0x80,0xA4,0x80,0x24,0x80,0x24,0xA0,0x28,0xA0,0x30,0x60},/*"��",1*/
    {0x11,0x00,0x1F,0xE0,0x20,0x00,0x27,0xC0,0x64,0x40,0xA7,0xC0,0x20,0x00,0x2F,0xE0,0x28,0x20,0x27,0xC0,0x21,0x00,0x23,0x00},/*"ͣ",2*/
    {0x00,0x80,0xF0,0x80,0x13,0xE0,0x52,0xA0,0x52,0xA0,0x52,0xA0,0x7B,0xE0,0x0C,0x80,0x2A,0x80,0xC9,0x00,0x0A,0x80,0x34,0x60},/*"ʻ",3*/
    {0x08,0x00,0x04,0x00,0xFF,0xE0,0x91,0x20,0x20,0x80,0x40,0x40,0x00,0x00,0x7F,0xC0,0x04,0x00,0x04,0x00,0x04,0x00,0xFF,0xE0},/*"��",4*/
    {0x08,0x00,0x08,0x00,0xFF,0xE0,0x10,0x00,0x24,0x00,0x44,0x00,0x7F,0xC0,0x04,0x00,0x04,0x00,0xFF,0xE0,0x04,0x00,0x04,0x00},/*"��",5*/
    {0x08,0x00,0x08,0x00,0xFF,0xE0,0x10,0x00,0x3F,0xC0,0x60,0x40,0xBF,0xC0,0x20,0x40,0x3F,0xC0,0x20,0x40,0x20,0x40,0x20,0xC0},/*"��",6*/
    {0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00,0x04,0x00,0x0A,0x00,0x0A,0x00,0x11,0x00,0x11,0x00,0x20,0x80,0x40,0x40,0x80,0x20},/*"��",7*/
        
    {0x19,0x80,0x1F,0xF0,0x30,0x00,0x37,0xE0,0x76,0x60,0xF7,0xE0,0x30,0x00,0x3F,0xF0,0x3C,0x30,0x37,0xE0,0x31,0x80,0x33,0x80},/*"ͣ",0*/
    {0x00,0xC0,0xF8,0xC0,0x1B,0xF0,0x7B,0xF0,0x7B,0xF0,0x7B,0xF0,0x7F,0xF0,0x0E,0xC0,0x3F,0xC0,0xED,0x80,0x0F,0xC0,0x3E,0x70},/*"ʻ",1*/
    {0x0C,0x00,0x06,0x00,0xFF,0xF0,0xD9,0xB0,0x30,0xC0,0x60,0x60,0x00,0x00,0x7F,0xE0,0x06,0x00,0x06,0x00,0x06,0x00,0xFF,0xF0},/*"��",2*/
    {0x0C,0x00,0x0C,0x00,0xFF,0xF0,0x18,0x00,0x36,0x00,0x66,0x00,0x7F,0xE0,0x06,0x00,0x06,0x00,0xFF,0xF0,0x06,0x00,0x06,0x00},/*"��",3*/
    {0x19,0x80,0x31,0x80,0x67,0xE0,0xD9,0x80,0x3F,0xF0,0x70,0xC0,0xFF,0xF0,0x30,0xC0,0x36,0xC0,0x33,0xC0,0x30,0xC0,0x33,0xC0},/*"��",4*/
    {0x30,0x00,0x37,0xC0,0x36,0xC0,0xFE,0xC0,0x36,0xC0,0x7E,0xC0,0x7E,0xC0,0xF6,0xC0,0x36,0xC0,0x36,0xF0,0x3C,0xF0,0x38,0x70},/*"��",5*/
    {0x00,0x00,0x7F,0xE0,0x06,0x00,0x06,0x00,0x06,0x00,0xFF,0xF0,0x0F,0x00,0x0F,0x00,0x1B,0x00,0x33,0x30,0x63,0x30,0xC1,0xF0},/*"��",6*/
    {0x06,0x00,0x06,0x00,0x06,0x00,0x06,0x00,0x06,0x00,0x0F,0x00,0x0F,0x00,0x19,0x80,0x19,0x80,0x30,0xC0,0x60,0x60,0xC0,0x30},/*"��",7*/
};


u16 DisplayBuf[2][12];//��ʾ����
u8 DispColor = 0;

void ReadCodingToBuf(u8 id)
{
    u8 j=0;
    u16 HzBuf1;
    
    //��ȡ�ַ����뵽��ʵ����
    for(j=0; j<12; j++)
    {
        HzBuf1 = (HZ[id][2*j]<<4) + (HZ[id][2*j+1]>>4);
        DisplayBuf[0][j] = HzBuf1;
        
        HzBuf1 = (HZ[id+1][2*j]<<4) + (HZ[id+1][2*j+1]>>4);
        DisplayBuf[1][j] = HzBuf1;
    }
}

void LedDisplay(u8 color, u8 Codeid)
{   
    DisEnGreen(); //��ֹ��ʾ
    DisEnRed(); 
    
    DispColor = color;//0:��ɫ 1:��ɫ
    ReadCodingToBuf(Codeid);
   
}

u32 RunLedTimeLast = 0;
u8 RunFlag = 0;
void RefreshDisplay(void)
{
    u8 i=0;
    
    //���е�
    if((OSTimeGet()-RunLedTimeLast)>=800)
    {
        RunLedTimeLast = OSTimeGet();
        RunFlag = (RunFlag+1)%2;
    }
    //��ʾ�����ַ�
    for(i=0; i<12; i++)
    {
        if(DispColor == 0)
        {
            DisEnRed(); 
            if(RunFlag == 0)
            {
                DisplayBuf[1][0] = DisplayBuf[1][0] | 0x001;
                DisplayBuf[1][11] = DisplayBuf[1][11] | 0x001;
                
                DisplayBuf[0][0] = DisplayBuf[0][0] | 0x800;
                DisplayBuf[0][11] = DisplayBuf[0][11] | 0x800;
            }
            else
            {
                DisplayBuf[1][0] = DisplayBuf[1][0] & (~0x001);
                DisplayBuf[1][11] = DisplayBuf[1][11] & (~0x001);
                
                DisplayBuf[0][0] = DisplayBuf[0][0]  & (~0x800);
                DisplayBuf[0][11] = DisplayBuf[0][11]  & (~0x800);
            }
            DisplyRed(DisplayBuf[0][i], DisplayBuf[1][i]);
        }
        else
        {
            DisEnGreen(); 
            if(RunFlag == 0)
            {
                DisplayBuf[1][0] = DisplayBuf[1][0] | 0x001;
                DisplayBuf[1][11] = DisplayBuf[1][11] | 0x001;
                
                DisplayBuf[0][0] = DisplayBuf[0][0] | 0x800;
                DisplayBuf[0][11] = DisplayBuf[0][11] | 0x800;
            }
            else
            {
                DisplayBuf[1][0] = DisplayBuf[1][0] & (~0x001);
                DisplayBuf[1][11] = DisplayBuf[1][11] & (~0x001);
                
                DisplayBuf[0][0] = DisplayBuf[0][0]  & (~0x800);
                DisplayBuf[0][11] = DisplayBuf[0][11]  & (~0x800);
            }
            DisplyGreen(DisplayBuf[0][i], DisplayBuf[1][i]);
        }
        
        RowCtl(i+1, 0);//�мӵ�
        OSTimeDlyHMSM(0,0,0,1);
        RowCtl(i+1, 1);//�жϵ�
    } 
}




