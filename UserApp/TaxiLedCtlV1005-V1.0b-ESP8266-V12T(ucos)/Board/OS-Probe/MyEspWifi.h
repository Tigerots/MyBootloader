
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

//�궨���ֹ����
#ifndef  __MYESP_H__
#define  __MYESP_H__
 

#ifndef  MYESP
#define	 __MYESP_EXT__  extern
#else
#define	 __MYESP_EXT__ 
#endif


//ͷ�ļ�
#include "includes.h"
#include "MyEspWifi.h"



//ȫ�ֱ�������
__packed typedef struct
{
    char RxCnt[5];//���յ����ݳ���
    char RxTag[10];//�����ʶ
    char RxBuff[100];//��������
}WIFIRXDATASTR;
__MYESP_EXT__ WIFIRXDATASTR WifiRxDataStr;//Wifi���յ������ݽṹ��



//��������
__MYESP_EXT__ void MyWifiTask(void);

#endif
