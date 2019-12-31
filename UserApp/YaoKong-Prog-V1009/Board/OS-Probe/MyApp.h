


#ifndef  __MYAPP_H__
#define  __MYAPP_H__
 

#ifndef  MYAPP
#define	 __MYAPP  extern
#else
#define	 __MYAPP 
#endif



__MYAPP unsigned char LightSta;//光电状态
__MYAPP unsigned char RemoteSta;//遥控状态
__MYAPP unsigned char RelaySta1;//继电器状态
__MYAPP unsigned char RelaySta2;//继电器状态

__packed typedef struct{

    unsigned char Cmd;//命令编号
    unsigned char CmdSub;//子命令编号
    unsigned char CmdData[50];//命令数据
    
}MyCmdStructType;//协议结构体

__packed typedef struct
{
	unsigned char  Start[3];//起始位
	unsigned char  ProVer;//协议版本号
    unsigned char  Source;//数据源地址
    unsigned char  Object;//数据目标地址
    unsigned char  DataType;//数据包类型
    /*
    数据包发起方：
    0x00：命令需要应答
    0x01：命令不需要应答

    数据包应答方：
    0x02：正在应答（后续还有数据包）
    0x03：应答完成（成功）
    0x04：数据错误，请求重发或终止
    0x05：设备忙，无法响应
    */
    unsigned char DataLen[2];//数据体长度  
    MyCmdStructType Data;//数据体
    unsigned char Crc[2];
    unsigned char End;
    
}MyProStructType;//协议结构体


__MYAPP MyProStructType MyProStruct;//定义协议接收结构体
//__MYAPP MyProStructType MySendStruct;//数据发送


__packed typedef struct
{
	unsigned char  DevType;//设备地址/类型
	unsigned char  HardVer;//硬件版本号
    unsigned char  SoftVer[2];//软件版本号
    unsigned char  ProVer;//协议版本号
    char           SN[14];//生产序列号
    unsigned char  Reserved[4];
    
}DevBasicInfoType;//设备基本信息
__MYAPP DevBasicInfoType DevBasicInfo;


__packed typedef struct
{
	unsigned char  SendGap;//实时数据发送时间间隔
	unsigned char  WorkMode;//工作模式，0: 通用IO, 1:底盘间隙
    unsigned char  ProType;//协议类型，0：FST协议IO协议，1：新城光电协议
    
}DevRunInfoType;//设备基本信息
__MYAPP DevRunInfoType DevRunInfo;



__MYAPP unsigned short QuickCRC16( unsigned char * pMsg, unsigned short iSize );

#endif


