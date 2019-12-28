
#include "common.h"
#include "MyApp.h"

void SerialPutChar(uint8_t c)
{
  USART_SendData(EVAL_COM1, c);
  while (USART_GetFlagStatus(EVAL_COM1, USART_FLAG_TXE) == RESET)
  {
  }
}

void Serial_PutString(uint8_t *s)
{
  while (*s != '\0')
  {
    SerialPutChar(*s);
    s++;
  }
}
void MyPrintf1(uint8_t *s, uint8_t cnt)
{
  while (cnt--)
  {
    SerialPutChar(*s++);
  }
}

//从串口获得数据,无数据返回-1
int16_t my_getchar(u8 com)   
{
    u8 temp=0;
    u8 timeout = 3;
	if ( 0 == com )	  // USB
	{
	}
    else if( 1 == com )
    {
        while(timeout)
        {
            if(Rx1Head != Rx1Tail)
            {
				temp = Rxd1Buf[Rx1Tail++];
                Rx1Tail %= Rxd1BufLen;
                return (temp);                  // 读到数据，退出
            }
            else
            {
                timeout--;                      // 缓冲区中没有数据，计时减1
                if (timeout != 0)
				{
				    delay20ms();
            	}
			}
        }
    }
    else
    {
        return (-1);
    }
    return (-1);
}
/******************************************************************************************************
* Description    : 遥控指令获取
* Input          : None
* Return         : None
******************************************************************************************************/
int16_t IrdaGetData(void)
{
	int16_t ret = 0;
	u8 IrdaBufCnt = 0;
	while(1)
	{
		ret = my_getchar(1);
		if(ret != -1)
		{//有数据
			IrdaBuf[IrdaBufCnt++]=(u8)ret;
		}
		else
		{
			if(IrdaBufCnt!=0)
			{
				return IrdaBufCnt;
			}
			else
			{
				return -1;
			}
		}
	}
}


