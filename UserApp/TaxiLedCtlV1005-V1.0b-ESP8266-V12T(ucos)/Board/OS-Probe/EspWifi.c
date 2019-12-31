
/**************************** (C) COPYRIGHT 2019 ****************************
* ���ʦ:   Tigerots
* ����ʱ��: 2019.10.23
* ��������: Wifiģ��ESP8266����, ATָ�����ģʽ
*       
*****************************************************************************
* ********************************�޸���ʷ��¼********************************
* �޸�ʱ��: 
* �汾��:
* �޸�����:
*****************************************************************************/

#define	 MYESP


#include "MyEspWifi.h"
#include "includes.h"

//�������ݵ�ģ�鴮��
#define EspSendStr MyPrintU2
#define EspRecChar MyGet1ByteU2
#define EspPrint MyPrintU1

/****************************************************************************
* ��������: 
* ����ʱ��: 2019.10.23
* ��������: ����Wifiģ�鷵��ָ��
* ��ڲ���: 
            sCmd: Ҫ���͵�����(AT...)
            sRet: ģ����ȷ��Ӧ�� 
            sSta: ģ�����ָ����״̬
            uTimeOut: δ�յ�Ӧ��ȴ�ʱ��
*****************************************************************************/
#define EspRxBuffLen    100
char EspRxBuff[EspRxBuffLen];//���ջ���
u16 EspRxBufCnt = 0;//���յ�������
u16 EspRxBufIndex = 0;//���ҵ����ַ�������
s16 EspRxData = 0;
u32 EspRxTimeLast = 0;


/****************************************************************************
* ��������: 
* ����ʱ��: 2019.10.23
* ��������: Wifiģ���������
* ��ڲ���: 
            sRet: ���յ������� 
            uTimeOut: δ�յ����ݳ�ʱʱ��
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
    {//��ʱʱ�䲻��Ϊ0
        uTimeOut = 10;
    }
    
    while((OSTimeGet()-EspRxTimeLast) <= uTimeOut)
    {//
        EspRxData = EspRecChar();
        
        if(EspRxData != -1)
        {//�н�������
            switch(sStaStep)
            {
                case 0:
                {
                    if(((char)EspRxData=='+') || ((char)EspRxData=='P'))
                    {
                        //��ʱ�������
                        EspRxBuff[(EspRxBufCnt++)%EspRxBuffLen] = (char)EspRxData;
                        sStaStep = 1;
                    }
                    
                }break;
                case 1:
                {
                    EspRxBuff[(EspRxBufCnt++)%EspRxBuffLen] = (char)EspRxData;
                    IPUindex = strstr(EspRxBuff, "+IPD,");
                    if(IPUindex != NULL)
                    {//���ҵ����յ����ݱ�ʶ
                        sStaStep = 2;
                    }
                    
                }break;
                case 2:
                {//���ճ����ֽ�
                    EspRxBuff[(EspRxBufCnt++)%EspRxBuffLen] = (char)EspRxData;
                    if((char)EspRxData==':')
                    {//��ȡ����
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
                {//������������
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
* ��������: 
* ����ʱ��: 2019.10.23
* ��������: Wifiģ�鷢��ָ��
* ��ڲ���: 
            sCmd: Ҫ���͵�����(AT...)
            sRet: ģ����ȷ��Ӧ�� 
            sSta: ģ�����ָ����״̬
            uTimeOut: δ�յ�Ӧ��ȴ�ʱ��
*****************************************************************************/
u8 EspWifiCmd(char *sCmd, char *sRet, char *sSta, u32 uTimeOut)
{
    u8 sStaFlag = 0;
    u8 sStaStep = 0;
    
    if(sSta != NULL)
    {
        sStaFlag = 1;
    }
    
    EspSendStr(sCmd);//��������
    EspRxBufCnt = 0;
    EspRxBufIndex = 0;
    memset(EspRxBuff, 0, sizeof(EspRxBuff));
    
    EspRxTimeLast = OSTimeGet();
    if(uTimeOut==0)
    {//��ʱʱ�䲻��Ϊ0
        uTimeOut = 10;
    }
    
    while((OSTimeGet()-EspRxTimeLast) <= uTimeOut)
    {//
        EspRxData = EspRecChar();
        if(EspRxData != -1)
        {//�н�������
            EspRxBuff[(EspRxBufCnt++)%EspRxBuffLen] = (u8)EspRxData;
        
            if(sStaStep == 0)
            {
                if(strstr(EspRxBuff, sRet) != NULL)
                {//���ҵ�Ӧ���ַ�,Ӧ��ɹ�
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
                {//���ҵ�Ӧ���ַ�,Ӧ��ɹ�
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
* ��������: 
* ����ʱ��: 2019.10.23
* ��������: Wifiģ���ϵ������ɹ��ж�, �ϵ�ɹ�, ģ�����:
*****************************************************************************/
    char IP[20];
u8 EspPowerUp(void)
{
    u8 uRet = 1;
    
    OSTimeDly(2000);
    //��λ
    uRet = EspWifiCmd("AT+RST\r\n", "ready", NULL, 5000);
    if(uRet == 0)
    {//δ���յ�Ӧ��
        EspPrint("Ask Error: RST");
        return uRet;
    }
    //����
    uRet = EspWifiCmd("AT\r\n","OK\r", NULL, 2000);
    if(uRet == 0)
    {//δ���յ�Ӧ��
        EspPrint("Ask Error: AT");
        return uRet;
    }
    //WiFiӦ��ģʽ
    uRet = EspWifiCmd("AT+CWMODE=1\r\n","OK\r", NULL, 2000);
    if(uRet == 0)
    {//δ���յ�Ӧ��
        EspPrint("Ask Error: CWMODE");
        return uRet;
    }
    //����·����
    uRet = EspWifiCmd("AT+CWJAP=\"Tigerots\",\"9955995599\"\r\n","WIFI CONNECTED", "OK", 20000);
    if(uRet == 0)
    {//δ���յ�Ӧ��
        EspPrint("Ask Error: CWJAP");
        return uRet;
    }
    //���ӵ�TCP������
    uRet = EspWifiCmd("AT+CIPSTART=\"TCP\",\"192.168.31.176\",9000\r\n","CONNECT", "OK", 5000);
    if(uRet == 0)
    {//δ���յ�Ӧ��
        EspPrint("Ask Error: CIPSTART");
        return uRet;
    }
//    //TCP��������
//    uRet = EspWifiCmd("AT+CIPSEND=5\r\n","OK", ">", 5000);
//    if(uRet == 0)
//    {//δ���յ�Ӧ��
//        EspPrint(" Ask Error: CIPSEND");
//        return uRet;
//    }
    
    return uRet;
}

/****************************************************************************
* ��������: 
* ����ʱ��: 2019.10.23
* ��������: ��������TCP������
*****************************************************************************/
u8 ReConnectToTcpSever(void)
{
    u8 uRet = 0;
    
    uRet = EspWifiCmd("AT+CIPSTATUS\r\n","STATUS:3", "OK", 2000);
    if(uRet == 0)
    {//����״̬Ϊ�Ƿ�����
        //�������ӵ�TCP������
        uRet = EspWifiCmd("AT+CIPSTART=\"TCP\",\"192.168.31.176\",9000\r\n","CONNECT", "OK", 5000);
        if(uRet == 0)
        {//δ���յ�Ӧ��
            EspPrint("Ask Error: CIPSTART");
            return 0;
        }
    }
    else
    {
        //����
        uRet = EspWifiCmd("AT\r\n","OK\r", NULL, 2000);
        if(uRet == 0)
        {//δ���յ�Ӧ��
            EspPrint("Ask Error: AT");
            return 0;
        }
    }
    return 1;
}

/****************************************************************************
* ��������: 
* ����ʱ��: 2019.10.23
* ��������: Wifiģ������������
* ��ڲ���:
* ���ڲ���:
* 
* ********************************�޸���ʷ��¼********************************
* �޸�ʱ��:
* �޸�����:
*****************************************************************************/
u32 Q_TcpStatusTimeLast = 0; //��ѯTCP����״̬ʱ���¼
char HZIndex[4][10] = {"����","ͣʻ","�ճ�","����"};
void MyWifiTask(void)
{
    u8 uRet = 0;
    u8 i = 0;
    
    do
    {
        uRet = EspPowerUp();//�ϵ��ʼ��
    }
    while(uRet == 0);
    
    while(1)
    {
        if((OSTimeGet()-Q_TcpStatusTimeLast)>=10000)
        {//��ʱ��ѯTcp����״̬
            Q_TcpStatusTimeLast = OSTimeGet();
            uRet = ReConnectToTcpSever();
            if(uRet == 0)
            {//δ���յ�Ӧ��
                EspPrint("ReConnect Tcp Sever Failed");
                return;
            }
        }
        memset(WifiRxDataStr.RxBuff, 0, sizeof(WifiRxDataStr.RxBuff));
        uRet = EspWifiRxData(EspRxBuff, 2000);
        if(uRet != 0)
        {//���յ�����,����������ִ����Ӧ����
            Q_TcpStatusTimeLast = OSTimeGet();//�յ���Ч����,���ò�ѯ״̬Ҳ����������
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
                {//δ���յ�Ӧ��
                    EspPrint("ReConnect Tcp Sever Failed");
                    return;
                }
            }
        }
    }
}


