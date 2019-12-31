

/**************************** (C) COPYRIGHT 2019 ****************************
* 设计师:   Tigerots
* 创建时间: 2019.10.23
* 功能描述: 数据类型转换函数, CRC计算函数等
*       
*****************************************************************************
* ********************************修改历史记录********************************
* 修改时间: 
* 版本号:
* 修改内容:
*****************************************************************************/


#ifndef  __MYCONVERT_H__
#define  __MYCONVERT_H__
 

#ifndef  MYCONVERT
#define	 __MYCONVERT  extern
#else
#define	 __MYCONVERT 
#endif


__MYCONVERT unsigned short QuickCRC16( unsigned char * pMsg, unsigned short iSize );

#endif


