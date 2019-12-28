					 
#ifndef AES_H 
#define AES_H 
 
#define ENCRYPT 0  //���� 
#define DECRYPT 1  //����

//--����Ϊ������, ����AES���㺯��ʱ��Ҫ�������±�������Ϊ��ǰ��Ϣ

extern unsigned char Key_Aes_Data[16];
//extern unsigned char const Key_Aes_KDF[16];
//extern u8 const GIVEN_AES_KEY[16];
//extern DateTime_Struct JiaoYi_DateTime;	//--��ȡʱ���õ���ʱ��ṹ����

extern void aesInit( unsigned char * tempbuf ); 
extern void aesDecrypt(unsigned char *buffer, unsigned char *chainBlock); 
extern void aesEncrypt( unsigned char * buffer, unsigned char * chainBlock ); 
 
extern void aesInit( unsigned char * tempbuf ); 
extern void aesDecrypt( unsigned char * buffer, unsigned char * chainBlock ); 
extern void aesEncrypt( unsigned char * buffer, unsigned char * chainBlock ); 

extern void Make_Net_Communication_AES_Key(void);
extern void Make_JiaoYi_Process_AES_Key(void);
 
extern unsigned char Data_AES(unsigned char flag,unsigned char *ChiperDataBuf,unsigned char DataLen); 
extern unsigned char NetWork_Data_AES(u8 flag,unsigned char *ChiperDataBuf,unsigned char DataLen); 
extern unsigned char NetWork_TuoJi_JiaoYi_AES(u8 flag,unsigned char *ChiperDataBuf,unsigned char DataLen);
extern unsigned char JiaoYi_Data_AES(u8 flag,unsigned char *ChiperDataBuf,unsigned char DataLen);
extern void Get_Key_Aes_Data(u8 AESInitStatus);
extern u8 Use_NetWork_AES_Flag; 
#endif // AES_H 

