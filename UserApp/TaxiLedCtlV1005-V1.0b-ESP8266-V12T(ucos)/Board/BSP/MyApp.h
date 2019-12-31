


#ifndef  __MYAPP_H__
#define  __MYAPP_H__
 

#ifndef  MYAPP
#define	 __MYAPP  extern
#else
#define	 __MYAPP 
#endif



__MYAPP unsigned char LightSta;//���״̬
__MYAPP unsigned char RemoteSta;//ң��״̬
__MYAPP unsigned char RelaySta1;//�̵���״̬
__MYAPP unsigned char RelaySta2;//�̵���״̬

__packed typedef struct{

    unsigned char Cmd;//������
    unsigned char CmdSub;//��������
    unsigned char CmdData[50];//��������
    
}MyCmdStructType;//Э��ṹ��

__packed typedef struct
{
	unsigned char  Start[3];//��ʼλ
	unsigned char  ProVer;//Э��汾��
    unsigned char  Source;//����Դ��ַ
    unsigned char  Object;//����Ŀ���ַ
    unsigned char  DataType;//���ݰ�����
    /*
    ���ݰ����𷽣�
    0x00��������ҪӦ��
    0x01�������ҪӦ��

    ���ݰ�Ӧ�𷽣�
    0x02������Ӧ�𣨺����������ݰ���
    0x03��Ӧ����ɣ��ɹ���
    0x04�����ݴ��������ط�����ֹ
    0x05���豸æ���޷���Ӧ
    */
    unsigned char DataLen[2];//�����峤��  
    MyCmdStructType Data;//������
    unsigned char Crc[2];
    unsigned char End;
    
}MyProStructType;//Э��ṹ��


__MYAPP MyProStructType MyProStruct;//����Э����սṹ��
//__MYAPP MyProStructType MySendStruct;//���ݷ���


__packed typedef struct
{
	unsigned char  DevType;//�豸��ַ/����
	unsigned char  HardVer;//Ӳ���汾��
    unsigned char  SoftVer[2];//����汾��
    unsigned char  ProVer;//Э��汾��
    char           SN[14];//�������к�
    unsigned char  Reserved[4];
    
}DevBasicInfoType;//�豸������Ϣ
__MYAPP DevBasicInfoType DevBasicInfo;


__packed typedef struct
{
	unsigned char  SendGap;//ʵʱ���ݷ���ʱ����
	unsigned char  WorkMode;//����ģʽ��0: ͨ��IO, 1:���̼�϶
    unsigned char  ProType;//Э�����ͣ�0��FSTЭ��IOЭ�飬1���³ǹ��Э��
    
}DevRunInfoType;//�豸������Ϣ
__MYAPP DevRunInfoType DevRunInfo;



__MYAPP unsigned short QuickCRC16( unsigned char * pMsg, unsigned short iSize );

#endif


