

#define IAP

#include <includes.h>
#include "iap.h"


void Iap_Map_Init(void)
{
    MyIapMap.BootAddr = c_BootAddr;//bootloader��ʼ��ַ
    MyIapMap.BootSize = c_BootSize;//bootloaderռ���ڴ��С
    MyIapMap.UserAddr = c_UserAddr;//Ӧ�ó�����ʼ��ַ
    MyIapMap.UserSize = c_UserSize;//Ӧ�ó���ռ���ڴ��С
    MyIapMap.BuffAddr = c_BuffAddr;//�������뻺����ʼ��ַ
    MyIapMap.BuffSize = c_BuffSize;//�������뻺��ռ���ڴ��С
    MyIapMap.ParaAddr = c_ParaAddr;//�û���������ʼ��ַ
    MyIapMap.ParaSize = c_ParaSize;//Ӧ�ó���ռ���ڴ��С 
}







