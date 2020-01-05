

#ifndef  __IAP_H__
#define  __IAP_H__
 

#ifndef  __MY_IAP
#define	 __IAPEXT  extern
#else
#define	 __IAPEXT 
#endif

#include "my_type.h"
//LEDָʾ����ʾ״̬
__IAPEXT unsigned char  Led_status;


#if FLASH_SIZE<0x40000
#define SECTOR_SIZE 1024
#else 
#define SECTOR_SIZE	2048
#endif		

//ʹ�õ�оƬ���� STM32F103RC
#define USE_STM32F103xC
#ifdef USE_STM32F103xC
    //Flash����ַ
    #define STM_FLASH_BASE (0x08000000)
    //Ƭ��Flash��С:256K
    #define FLASH_SIZE (0x40000)
    //������С:2K
    #define PAGE_SIZE (0x800)
    //Ĭ�ϵ�ַӳ��
    #define c_BootAddr 0x08000000
    #define c_BootSize 0x8000 //32K
    #define c_UserAddr 0x08008000
    #define c_UserSize 0x19000 //100K
    #define c_BuffAddr 0x08021000
    #define c_BuffSize 0x19000 //100K
    #define c_ParaAddr 0x0803A000
    #define c_ParaSize 0x6000 //24K
#endif
#ifdef USE_STM32F103xD
    //Flash����ַ
    #define STM_FLASH_BASE (0x08000000)
    //Ƭ��Flash��С:256K
    #define FLASH_SIZE (0x40000)
    //������С:2K
    #define PAGE_SIZE (0x800)
    //Ĭ�ϵ�ַӳ��
    #define c_BootAddr 0x08000000
    #define c_BootSize 0x8000 //32K
    #define c_UserAddr 0x08008000
    #define c_UserSize 0x29000 //164K
    #define c_BuffAddr 0x08031000
    #define c_BuffSize 0x29000 //164K
    #define c_ParaAddr 0x0805A000
    #define c_ParaSize 0x6000 //24K
#endif
#ifdef USE_STM32F103xE
    //Flash����ַ
    #define STM_FLASH_BASE (0x08000000)
    //Ƭ��Flash��С:256K
    #define FLASH_SIZE (0x40000)
    //������С:2K
    #define PAGE_SIZE (0x800)
    //Ĭ�ϵ�ַӳ��
    #define c_BootAddr 0x08000000
    #define c_BootSize 0x10000 //64K
    #define c_UserAddr 0x08010000
    #define c_UserSize 0x32000 //200K
    #define c_BuffAddr 0x08042000
    #define c_BuffSize 0x32000 //200K
    #define c_ParaAddr 0x08074000
    #define c_ParaSize 0xC000 //48K
#endif



//�ڴ��ַӳ���
__packed typedef struct
{
    unsigned long BootAddr;//bootloader��ʼ��ַ
    unsigned long BootSize;//bootloaderռ���ڴ��С
    unsigned long UserAddr;//Ӧ�ó�����ʼ��ַ
    unsigned long UserSize;//Ӧ�ó���ռ���ڴ��С
    unsigned long BuffAddr;//�������뻺����ʼ��ַ
    unsigned long BuffSize;//�������뻺��ռ���ڴ��С
    unsigned long ParaAddr;//�û���������ʼ��ַ
    unsigned long ParaSize;//Ӧ�ó���ռ���ڴ��С 
}MyIapMapType;
__IAPEXT MyIapMapType MyIapMap;



//������־���ݽṹ,�����ParaAddr��һ������,ռ��һ�������ռ�
__packed typedef struct
{
    unsigned long DevType;//���õ��豸����
    unsigned long ChipType;//���õ�оƬ����
    unsigned long FileSize;//�����ļ���С
    unsigned long FileSizeMax;//�����ļ����ֵ
    unsigned long FileSizeMin;//�����ļ���Сֵ
    
    unsigned char UpdataFlag;//������־ 
    //    0:�����������������
    //    1:��Ҫ����,�Ѿ������������� 
    //    2:�������,��������(Ӧ�ó��򽫱�־��0,���ѭ��)
    //    0x55:����ʧ��
    //    0xAA:�ָ���������
    unsigned char UpdataCnt;//����������¼
    unsigned char UpdataType;//��������
    //    0: ѡ������
    //    1: ǿ������
    unsigned int SoftFilter;//�̼��汾����, ���ڸð汾����Ҫ����
    unsigned int HardFilter;//��Ӧ��Ӳ���汾, ���ڸð汾����Ҫ����
    
    unsigned char FileType;//�ļ�����
    //    0: Hex�ļ�(������У��,����ϴ�,ascii��ʽ����)
    //    1: Bin�ļ�(���������ļ�,���κ�У��,���С,ʮ�����ƴ���)
    //    2: Fst�ļ�(�����ļ���Ϣ,����ϴ�,ascii��ʽ����)
    unsigned char CommType;//ͨ�Ž�����ʽ
    //    0: �㲥��ʽ,��Ӧ��
    //    1: �ʴ�ʽ,һ��һ��
    unsigned char CommGap;//ͨ�Ű����,��λms
    unsigned int  PackSize;//���ݵ�����С(4��������)
    
}MyIapFlagType;
__IAPEXT MyIapFlagType MyIapFlag;

//
__packed typedef struct
{
    unsigned char WriteFlag;//д��־
    //0:������
    //1:��������,��������
    //2:�������,��������
    //3:�������,дUpdataFlag��־Ϊ1,����
    unsigned char WriteStep;//д����
    //0:����
    //1:�����������
    //2:����д��
    //3:д���
    unsigned long WriteTime;//��һ��д��ʱ��
    unsigned long WriteAddr;//��ǰд��ַ
    unsigned long WriteSize;//��ǰ����С
    unsigned long WriteTatolCnt;//�Ѿ�д���ֽ�
    unsigned char WriteBuff[PAGE_SIZE];//д����
}MyIapRxBuffType;
__IAPEXT MyIapRxBuffType MyIapRxBuff;


//��������
__IAPEXT void Iap_Map_Init(void);
__IAPEXT void iap_jump_to_user_app(void);
__IAPEXT void MX_GPIO_Init(void);
__IAPEXT void LedRun(void);
__IAPEXT void led_display_entry(void *parameter);
__IAPEXT void iap_thread_init(void);

#endif







