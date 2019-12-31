
/*
用于存放应用程序(非底层驱动)


*/

#define	 MYAPP

#include <includes.h>


//多项式: ANSI CRC-16,x16+x15+x2+1
unsigned short QuickCRC16( unsigned char * pMsg, unsigned short iSize )
{
    unsigned short tCRC=0xFFFF;
    unsigned char j,Tmp=0;
    int i;

    for(i=0;i<iSize;i++)
    {     
        tCRC^=pMsg[i];                      
        for (j=0;j<8;j++)                    
        {                                    
            Tmp=tCRC&0x01;              
            tCRC=tCRC>>1;                  
            if(Tmp)
            {
                tCRC=(tCRC^0xA001);   
            }                
        }                                    
    }
    //return ((tCRC>>8)+(tCRC<<8))&0xffff;//低字节在前
    return tCRC&0xffff;//高字节在前
}
//整数转换成字符串
const INT8U table[] = "0123456789ABCDEF";
char * my_itoa( INT32S value, char *pstr, INT8U radix )
{
	INT32S    i, d;
    INT8U     flag = 0;
    char      *ptr = pstr;
    /* This implementation  works for decimal numbers and hex numbers. */
    if ( (radix != 10) && (radix != 16) )
    {
        *ptr = 0;
        return pstr;
    }

    if ( value == 0 )
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return pstr;
    }

	if ( radix == 10 )
	{
	    /* if this is a negative value insert the minus sign. */
	    if (value < 0)
	    {
	        *ptr++ = '-';
	        /* Make the value positive. */
	        value *= -1;
	    }
	    for (i = 1000000000; i > 0; i /= 10)
	    {
	        d = value / i;
	        if (d || flag)
	        {
	            *ptr++ = (char)(d + 0x30);
	            value -= (d * i);
	            flag = 1;
	        }
	    }
	}
	else
	{
		for ( i = 7; i >= 0; i-- )
		{
			d = ( value >> ( i * 4 ) ) & 0x0f;
			if ( d || flag )
			{
				*ptr++ = table[d];
				flag = 1;
			}
		}
	}
    /* Null terminate the string. */
    *ptr = 0;
    return pstr;
}

//浮点数转化为ASCII
char  *gcvt(char *buf, INT8U nInt, INT8U ndigit, float number) 
{ 
	long    int_part = (long)number; //整数部分
	float   float_part = number-int_part;//小数部分 
	int	    i=0;
    
    if(nInt !=0 )
    {//对整数部分的要求
        if(nInt == 5)
        {
            i = sprintf((char *)&buf[i], "%05d", int_part);
        }
        else if(nInt == 4)
        {
            i = sprintf((char *)&buf[i], "%04d", int_part);
        }
        else if(nInt == 3)
        {
            i = sprintf((char *)&buf[i], "%03d", int_part);
        }
        else if(nInt == 2)
        {
            i = sprintf((char *)&buf[i], "%02d", int_part);
        }
        else if(nInt == 1)
        {
            i = sprintf((char *)&buf[i], "%01d", int_part);
        }
        else
        {
             i = sprintf((char *)&buf[i], "%d", int_part);
        }
    }
    else
    {
        i = sprintf((char *)&buf[i], "%d", int_part);
    }        
	if(float_part<0)
	{
		float_part = -float_part;
	}
	
	if(ndigit>0)//小数位数
	{ 
	    buf[i++]= '.'; 
	    while(ndigit-->0) 
	    { 
	        float_part=float_part*10;
	        my_itoa(((int)float_part)%10, (char*)&buf[i++], 10); 
	    } 
	} 
	buf[i]=0; 
	return   buf; 
} 

//字符串转换为浮点数
int my_atoi(char const *string, INT8U len)
{
	int value;
	value = 0;
	
	while((*string >= '0' && *string <= '9')) 
	{
		if(len-- == 0)
		{
			break;
		}
		value *= 10;
		value += *string - '0';
		++string;
	}
	return value;
}


