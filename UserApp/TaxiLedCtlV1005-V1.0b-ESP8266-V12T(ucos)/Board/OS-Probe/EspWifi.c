
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

#define	 MYESP


#include "MyEspWifi.h"
#include "includes.h"

//发送数据到模块串口
#define EspSendStr MyPrintU2
#define EspRecChar MyGet1ByteU2
#define EspPrint MyPrintU1

/****************************************************************************
* 函数名称: 
* 创建时间: 2019.10.23
* 功能描述: 接收Wifi模块返回指令
* 入口参数: 
            sCmd: 要发送的命令(AT...)
            sRet: 模块正确的应答 
            sSta: 模块接收指令后的状态
            uTimeOut: 未收到应答等待时间
*****************************************************************************/
#define EspRxBuffLen    100
char EspRxBuff[EspRxBuffLen];//接收缓存
u16 EspRxBufCnt = 0;//接收到的数量
u16 EspRxBufIndex = 0;//查找到的字符串索引
s16 EspRxData = 0;
u32 EspRxTimeLast = 0;


/****************************************************************************
* 函数名称: 
* 创建时间: 2019.10.23
* 功能描述: Wifi模块接收数据
* 入口参数: 
            sRet: 接收到的数据 
            uTimeOut: 未收到数据超时时间
*****************************************************************************/
u8 EspWifiRxData(char *sRet, u32 uTimeOut)
{
    u8 sStaStep = 0;
    char *IPUindex, *cTemp1, *cTemp2;
    
    EspRxBufCnt = 0;
    EspRxBufIndex = 0;
    memset(EspRxBuff, 0, sizeof(EspRxBuff));
    
    EspRxTimeLast = OSTimeGet();
    if(uTimeOut==0)
    {//超时时间不能为0
        uTimeOut = 10;
    }
    
    while((OSTimeGet()-EspRxTimeLast) <= uTimeOut)
    {//
        EspRxData = EspRecChar();
        
        if(EspRxData != -1)
        {//有接收数据
            switch(sStaStep)
            {
                case 0:
                {
                    if(((char)EspRxData=='+') || ((char)EspRxData=='P'))
                    {
                        //临时存放数据
                        EspRxBuff[(EspRxBufCnt++)%EspRxBuffLen] = (char)EspRxData;
                        sStaStep = 1;
                    }
                    
                }break;
                case 1:
                {
                    EspRxBuff[(EspRxBufCnt++)%EspRxBuffLen] = (char)EspRxData;
                    IPUindex = strstr(EspRxBuff, "+IPD,");
                    if(IPUindex != NULL)
                    {//查找到接收到数据标识
                        sStaStep = 2;
                    }
                    
                }break;
                case 2:
                {//接收长度字节
                    EspRxBuff[(EspRxBufCnt++)%EspRxBuffLen] = (char)EspRxData;
                    if((char)EspRxData==':')
                    {//截取长度
                        cTemp1 = strchr(IPUindex,',');
                        if(cTemp1 != NULL)
                        {
                            cTemp2 = strchr(cTemp1, ':');
                            if(cTemp2 != NULL)
                            {
                                memcpy(WifiRxDataStr.RxCnt, (cTemp1+1), (cTemp2-cTemp1-1));
                                sStaStep = 3;
                            }
                        }
                    }
                }break;
                case 3:
                {//接收数据内容
                    EspRxBuff[(EspRxBufCnt++)%EspRxBuffLen] = (char)EspRxData;
                    if((char)EspRxData==0x0D)
                    {
                        EspPrint("Log, Received Data:");
                        EspPrint(EspRxBuff);EspPrint("\r\n");
                        
                        cTemp1 = strstr(EspRxBuff,"display:");
                        if(cTemp1 != NULL)
                        {
                            cTemp2 = strchr(cTemp1, '\r');
                            
                            if(cTemp2 != NULL)
                            {
                                memcpy(WifiRxDataStr.RxBuff, (cTemp1+strlen("display:")), (cTemp2-cTemp1-strlen("display:")));
                                EspRxBufCnt = 0;
                                EspRxBufIndex = 0;
                                memset(EspRxBuff, 0, sizeof(EspRxBuff));
                                
                                sStaStep = 0;
                                return 1;
                            }
                        }
                    }
                }break;
                default:
                {
                    EspRxBufCnt = 0;
                    EspRxBufIndex = 0;
                    memset(EspRxBuff, 0, sizeof(EspRxBuff));
                }break;
            }
        }
        else
        {
            OSTimeDlyHMSM(0,0,0,1);
        }
    }
    return 0;
}



/****************************************************************************
* 函数名称: 
* 创建时间: 2019.10.23
* 功能描述: Wifi模块发送指令
* 入口参数: 
            sCmd: 要发送的命令(AT...)
            sRet: 模块正确的应答 
            sSta: 模块接收指令后的状态
            uTimeOut: 未收到应答等待时间
*****************************************************************************/
u8 EspWifiCmd(char *sCmd, char *sRet, char *sSta, u32 uTimeOut)
{
    u8 sStaFlag = 0;
    u8 sStaStep = 0;
    
    if(sSta != NULL)
    {
        sStaFlag = 1;
    }
    
    EspSendStr(sCmd);//发送命令
    EspRxBufCnt = 0;
    EspRxBufIndex = 0;
    memset(EspRxBuff, 0, sizeof(EspRxBuff));
    
    EspRxTimeLast = OSTimeGet();
    if(uTimeOut==0)
    {//超时时间不能为0
        uTimeOut = 10;
    }
    
    while((OSTimeGet()-EspRxTimeLast) <= uTimeOut)
    {//
        EspRxData = EspRecChar();
        if(EspRxData != -1)
        {//有接收数据
            EspRxBuff[(EspRxBufCnt++)%EspRxBuffLen] = (u8)EspRxData;
        
            if(sStaStep == 0)
            {
                if(strstr(EspRxBuff, sRet) != NULL)
                {//查找到应答字符,应答成功
                    if(sStaFlag==0)
                    {
                        return 1;
                    }
                    else
                    {
                        sStaStep = 1;
                    }
                }
            }
            else
            {
                if(strstr(EspRxBuff, sSta) != NULL)
                {//查找到应答字符,应答成功
                    return 2;
                }
            }
        }
        else
        {
            OSTimeDlyHMSM(0,0,0,1);
        }
    }
    return 0;
}

/****************************************************************************
* 函数名称: 
* 创建时间: 2019.10.23
* 功能描述: Wifi模块上电启动成功判断, 上电成功, 模块输出:
*****************************************************************************/
    char IP[20];
u8 EspPowerUp(void)
{
    u8 uRet = 1;
    
    OSTimeDly(2000);
    //复位
    uRet = EspWifiCmd("AT+RST\r\n", "ready", NULL, 5000);
    if(uRet == 0)
    {//未接收到应答
        EspPrint("Ask Error: RST");
        return uRet;
    }
    //握手
    uRet = EspWifiCmd("AT\r\n","OK\r", NULL, 2000);
    if(uRet == 0)
    {//未接收到应答
        EspPrint("Ask Error: AT");
        return uRet;
    }
    //WiFi应用模式
    uRet = EspWifiCmd("AT+CWMODE=1\r\n","OK\r", NULL, 2000);
    if(uRet == 0)
    {//未接收到应答
        EspPrint("Ask Error: CWMODE");
        return uRet;
    }
    //连接路由器
    uRet = EspWifiCmd("AT+CWJAP=\"Tigerots\",\"9955995599\"\r\n","WIFI CONNECTED", "OK", 20000);
    if(uRet == 0)
    {//未接收到应答
        EspPrint("Ask Error: CWJAP");
        return uRet;
    }
    //连接到TCP服务器
    uRet = EspWifiCmd("AT+CIPSTART=\"TCP\",\"192.168.31.176\",9000\r\n","CONNECT", "OK", 5000);
    if(uRet == 0)
    {//未接收到应答
        EspPrint("Ask Error: CIPSTART");
        return uRet;
    }
//    //TCP发送数据
//    uRet = EspWifiCmd("AT+CIPSEND=5\r\n","OK", ">", 5000);
//    if(uRet == 0)
//    {//未接收到应答
//        EspPrint(" Ask Error: CIPSEND");
//        return uRet;
//    }
    
    return uRet;
}

/****************************************************************************
* 函数名称: 
* 创建时间: 2019.10.23
* 功能描述: 重新连接TCP服务器
*****************************************************************************/
u8 ReConnectToTcpSever(void)
{
    u8 uRet = 0;
    
    uRet = EspWifiCmd("AT+CIPSTATUS\r\n","STATUS:3", "OK", 2000);
    if(uRet == 0)
    {//连接状态为非非连接
        //重新连接到TCP服务器
        uRet = EspWifiCmd("AT+CIPSTART=\"TCP\",\"192.168.31.176\",9000\r\n","CONNECT", "OK", 5000);
        if(uRet == 0)
        {//未接收到应答
            EspPrint("Ask Error: CIPSTART");
            return 0;
        }
    }
    else
    {
        //握手
        uRet = EspWifiCmd("AT\r\n","OK\r", NULL, 2000);
        if(uRet == 0)
        {//未接收到应答
            EspPrint("Ask Error: AT");
            return 0;
        }
    }
    return 1;
}

/****************************************************************************
* 函数名称: 
* 创建时间: 2019.10.23
* 功能描述: Wifi模块运行主任务
* 入口参数:
* 出口参数:
* 
* ********************************修改历史记录********************************
* 修改时间:
* 修改内容:
*****************************************************************************/
u32 Q_TcpStatusTimeLast = 0; //查询TCP连接状态时间记录
char HZIndex[4][10] = {"待机","停驶","空车","有人"};
void MyWifiTask(void)
{
    u8 uRet = 0;
    u8 i = 0;
    
    do
    {
        uRet = EspPowerUp();//上电初始化
    }
    while(uRet == 0);
    
    while(1)
    {
        if((OSTimeGet()-Q_TcpStatusTimeLast)>=10000)
        {//定时查询Tcp连接状态
            Q_TcpStatusTimeLast = OSTimeGet();
            uRet = ReConnectToTcpSever();
            if(uRet == 0)
            {//未接收到应答
                EspPrint("ReConnect Tcp Sever Failed");
                return;
            }
        }
        memset(WifiRxDataStr.RxBuff, 0, sizeof(WifiRxDataStr.RxBuff));
        uRet = EspWifiRxData(EspRxBuff, 2000);
        if(uRet != 0)
        {//接收到数据,根据命令编号执行相应程序
            Q_TcpStatusTimeLast = OSTimeGet();//收到有效数据,不用查询状态也是链接正常
            //if( strstr(WifiRxDataStr.RxTag,"display") )
            if(1)
            {//
                for(i=0; i<4; i++)
                {
                    if(strstr( HZIndex[i], WifiRxDataStr.RxBuff ) != NULL)
                    {
                        if(i==2 || i==0)
                        {
                            LedDisplay(1, 2*i); 
                        }
                        else
                        {
                            LedDisplay(0, 2*i); 
                        }
                    }
                }
            }
            else if( strstr(WifiRxDataStr.RxTag,"CLOSED") )
            {
                uRet = ReConnectToTcpSever();
                if(uRet == 0)
                {//未接收到应答
                    EspPrint("ReConnect Tcp Sever Failed");
                    return;
                }
            }
        }
    }
}


