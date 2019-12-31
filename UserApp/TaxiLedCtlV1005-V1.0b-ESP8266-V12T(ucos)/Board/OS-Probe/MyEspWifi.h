
/**************************** (C) COPYRIGHT 2019 ****************************
* 设计师:   Tigerots
* 创建时间: 2019.10.23
* 功能描述: Wifi模块ESP8266驱动, AT指令控制模式
*       
*****************************************************************************
* ********************************修改历史记录********************************
* 修改时间: 
* 版本号:
* 修改内容:
*****************************************************************************/

//宏定义防止重入
#ifndef  __MYESP_H__
#define  __MYESP_H__
 

#ifndef  MYESP
#define	 __MYESP_EXT__  extern
#else
#define	 __MYESP_EXT__ 
#endif


//头文件
#include "includes.h"
#include "MyEspWifi.h"



//全局变量定义
__packed typedef struct
{
    char RxCnt[5];//接收的数据长度
    char RxTag[10];//命令标识
    char RxBuff[100];//命令内容
}WIFIRXDATASTR;
__MYESP_EXT__ WIFIRXDATASTR WifiRxDataStr;//Wifi接收到的数据结构体



//函数声明
__MYESP_EXT__ void MyWifiTask(void);

#endif
