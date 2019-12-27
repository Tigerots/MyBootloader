

#ifndef  __IAP_H__
#define  __IAP_H__
 

#ifndef  IAP
#define	 __IAPEXT  extern
#else
#define	 __IAPEXT 
#endif


//ʹ�õ�оƬ���� STM32F103RC
#define USE_STM32F103xC
#ifdef USE_STM32F103xC
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
    unsigned long FileSizeMax;//�����ļ����ֵ
    unsigned long FileSizeMin;//�����ļ���Сֵ
    
    unsigned char UpdataFlag;//������־ 
    //    0:���������� 
    //    1:���ڻ����������� 
    //    2:�������뻺�����,�������� 
    //    3:�������,��������
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


#endif







