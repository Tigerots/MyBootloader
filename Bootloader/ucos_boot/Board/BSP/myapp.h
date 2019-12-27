

#ifndef  __MYAPP_H__
#define  __MYAPP_H__
 

#ifndef  MYAPP
#define	 __MYAPP  extern
#else
#define	 __MYAPP 
#endif

#define FLASH_OK			CMD_OK
#define FLASH_ERROR			1

#define CMD_OK				0
#define CMD_FAIL			1


#define Def_HardVerSion 0x1001//硬件版本号
#define Def_HardVerSion_SIZE		0x10
#define Def_HardVerSion_ADDR		(0x0803F000)

#define Def_SoftVersion 0x1001//软件版本号


//程序Flash升级时用到的宏定义,存放于片内Flash中
//#define UPDATE_FLAG_ADDR			0x0803E000	//升级标志区
//#define UPDATE_FLAG_SIZE			0x10		//升级标志大小实际为4字节

#define USER_DATA_ADDR			0x0803D000
#define USER_DATE_SIZE			246		


__MYAPP char  *gcvt(char *buf, INT8U nInt, INT8U ndigit, float number);
// 


#endif






