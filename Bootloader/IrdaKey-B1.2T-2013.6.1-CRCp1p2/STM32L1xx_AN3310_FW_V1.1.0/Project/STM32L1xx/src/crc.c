#include "crc.h"
// *****************************************************************************
// Design Notes:   ANSI CRC-16,x16 + x15 + x2 + 1
// -----------------------------------------------------------------------------
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
            if(Tmp)tCRC=(tCRC^0xA001);     
        }                                    
    }
return ((tCRC>>8)+(tCRC<<8))&0xffff;

}


















