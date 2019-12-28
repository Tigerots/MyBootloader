#ifndef  __MYAPP_H__
#define  __MYAPP_H__
 

#ifndef  MYFILE
	#define	 __MYFILE  extern
#else
	#define	 __MYFILE 
#endif

//#define __USECRCSEND
#define __USECRC


//���巵�صĴ������ʹ���
#define		CmdOK				0x9000
#define		HandleErr			0x0101
#define		P1P2Err				0x0102
#define		FileLenErr			0x0103
#define		RecordLenErr		0x0104	
#define		RecordNumErr		0x0105
#define		NoOKRecord			0x0106
#define		FileIDExistErr		0x0107//�ļ��Ѿ�����,����FID
#define		NoFileIDErr			0x0108//�ļ�������,�ȴ���
#define		NoSpaceErr			0x0109//�ռ䲻��,���ʽ����
#define		HaveExistErr		0x010A//�Ѿ����ڸü�¼,����д��
#define		CrcErr				0x010B//CRC����
#define		WriteOnlyErr		0x010C//��ֹ��ȡ


#define		OtherErr			0x01FF

__MYFILE u8 SysKeyID[4];

//===================��Ϣ�ṹ=========================
//��˾ǩ����Ϣ 63�ֽ�, ���ܺ�Ϊ64�ֽ�???
__packed typedef struct
{
	uint8_t Version;//�汾1
	uint8_t keyid[4];//KeyId
	uint8_t keyNo[20];//�ַ���,key��ӡ���к�;       
	uint8_t PrDate[6];//��������, ������ʱ�����һ���ֽ�
	uint8_t VenueCode[4];//���ر���
	uint8_t VenueName[20];//��������
	uint8_t AppType;//Ӧ������
	uint8_t KeyType;//�ƿ�����
	uint8_t Reserve[4];//����	
	uint8_t uCRC[2];//CRC
}CompanyInfoType;


//===================��¼�ṹ=========================
//LTP: Lottery Ticket Printer
//��Ʊ����ʼ����¼ 16�ֽ�
__packed typedef struct
{
	uint8_t Version;//�汾1
	uint8_t InitDate[6];//��ʼ��ʱ��
	uint8_t LTPCode[4];//��Ʊ������
	uint8_t LTPNo[2];//��Ʊ������
	uint8_t Reserve[1];//����	
	uint8_t uCRC[2];//CRC
}LTPInitRecordType;

//��Ʊ�����ü�¼ 32�ֽ�
__packed typedef struct
{
	uint8_t Version;//�汾1
	uint8_t SetDate[6];//����ʱ��
	uint8_t LTPCode[4];//��Ʊ������
	uint8_t LTPNo[2];//��Ʊ������
	uint8_t RTPol[1];//��Ʊ���� 0:NO 1:NC Recede Ticket Polarity
	uint8_t RTSpeed[1];//��Ʊ�ٶ� 0-7
	uint8_t RTTime[2];//��Ʊ����
	uint8_t AlarmSW[1];//��������
	uint8_t Reserve[12];//����	
	uint8_t uCRC[2];//CRC
}LTPSetRecordType;

//��Ʊ��������¼ 16�ֽ�
__packed typedef struct
{
	uint8_t Version;//�汾1
	uint8_t InitDate[6];//��ʼ��ʱ��
	uint8_t LTPCode[4];//��Ʊ������
	uint8_t Reserve[3];//����	
	uint8_t uCRC[2];//CRC
}LTPBootRecordType;

//��Ʊ�����˼�¼ 48�ֽ�
__packed typedef struct
{
	uint8_t Version;//�汾1
	uint8_t LTPCode[4];//��Ʊ������
	uint8_t LTPNo[2];//��Ʊ������
	uint8_t StartDate[6];//��Ŀ��ʼʱ��
	uint8_t BillDate[6];//����ʱ��
	uint8_t LastOverTicket[4];//������Ʊ
	uint8_t AddTicketTimes[2];//���ڼ�Ʊ����
	uint8_t AddTicket[4];//���ڼ�Ʊ
	uint8_t RetTicketTimes[2];//������Ʊ����
	uint8_t PrintTicketTime[2];//���ڴ�Ʊ����
	uint8_t RetTicket[4];//������Ʊ
	uint8_t OverTicket[4];//������Ʊ
	uint8_t Reserve[5];//����	
	uint8_t uCRC[2];//CRC
}LTPBillRecordType;

//��Ʊ����Ʊ�����¼ 64�ֽ�
__packed typedef struct
{
	uint8_t Version;//�汾1
	uint8_t LTPCode[4];//��Ʊ������
	uint8_t LTPNo[2];//��Ʊ������
	uint8_t ApplyNo[4];//���뵥��
	uint8_t ApplyDate[6];//����ʱ��
	uint8_t LastApplyNo[4];//�ϴ����뵥��
	uint8_t LastApplyDate[6];//�ϴ�����ʱ��
	uint8_t LastOverTicket[4];//�ϴ���Ʊ
	uint8_t LastOverAmount[2];//�ϴ���Ʊ����
	uint8_t LastAddTicket[4];//�ϴμ�Ʊ
	uint8_t LastAddAmount[2];//�ϴμ�Ʊ����
	uint8_t PrintTicket[4];//���ڳ�Ʊ
	uint8_t PrintAmount[2];//���ڳ�Ʊ����
	uint8_t OverTicket[4];//��Ʊ
	uint8_t OverAmount[2];//��Ʊ����
	uint8_t Reserve[11];//����	
	uint8_t uCRC[2];//CRC
}LTPAddTicketApplyRecordType;

//��Ʊ����Ʊ��¼ 64�ֽ�
__packed typedef struct
{
	uint8_t Version;//�汾1
	uint8_t LTPCode[4];//��Ʊ������
	uint8_t LTPNo[2];//��Ʊ������
	uint8_t TicketNo[4];//Ʊ��
	uint8_t LastOverTicket[4];//��Ʊǰ���
	uint8_t PrintTicket[4];//��Ʊ
	uint8_t RTOverTicket[4];//���
	uint8_t RTStartDate[6];//��ʼ��Ʊʱ��
	uint8_t RTTimes[2];//��Ʊ����
	uint8_t RTEndDate[6];//������Ʊʱ��
	uint8_t PrintDate[6];//��ӡʱ��
	uint8_t LastPrintDate[6];//�ϴδ�ӡʱ��
	uint8_t AddTicketNo[4];//���ڼ�Ʊ����
	uint8_t Reserve[9];//����	
	uint8_t uCRC[2];//CRC
}LTPPrintTicketRecordType;

//��Ʊ��Ԥ��Ʊ��¼ 32�ֽ�
__packed typedef struct
{
	uint8_t Version;//�汾1
	uint8_t LTPCode[4];//��Ʊ������
	uint8_t LTPNo[2];//��Ʊ������
	uint8_t AddTicketNo[4];//��Ʊ����
	uint8_t ApplyNo[4];//���뵥��
	uint8_t OverTicket[4];//��Ʊ
	uint8_t OverAmount[2];//��Ʊ����
	uint8_t AddTicket[4];//��Ʊ
	uint8_t AddAmount[2];//��Ʊ����
	uint8_t TicketNoID[2];//Ʊ���ļ�ID
	uint8_t Reserve[1];//����	
	uint8_t uCRC[2];//CRC
}LTPAddTicketRecordType;

//��Ʊ��ƱƱ�ż�¼ 6�ֽ�
__packed typedef struct
{
	uint8_t TicketNo[4];//Ʊ��	
	uint8_t uCRC[2];//CRC
}LTPAddTicketNoRecordType;

//__MYFILE u8 LTPAddTicketNoDelFlag[1250];//Ʊ�ż�¼ɾ����־,0:ɾ��, 1:δɾ��(������Ĭ��)


//��Ʊ�����в��� 32�ֽ�
__packed typedef struct
{
	uint8_t Version;//�汾1
	uint8_t InitDate[6];//��ʼ��ʱ��
	uint8_t LTPNo[2];//��Ʊ������
	uint8_t RTPol[1];//��Ʊ���� 0:NO 1:NC Recede Ticket Polarity
	uint8_t RTSpeed[1];//��Ʊ�ٶ� 0-7
	uint8_t RTTime[2];//��Ʊ����
	uint8_t AlarmSW[1];//��������
	uint8_t LastSetDate[6];//�������ʱ��
	uint8_t Reserve[10];//����	
	uint8_t uCRC[2];//CRC
}LTPRunParaType;

__packed typedef struct
{
	u8 FileType;
	u16 FileLen_RecordCount;
	u8 RecordLen;
	u8 FileCreadTime[6];
}stFileAttributeType;
__MYFILE stFileAttributeType stFileAttribute;


//===================�ļ�����,�����Ƭ��EEPROM��=========================

	
#define MF_SectorSize 	4096	//��С4K�ֽ�����(��д��С��λ)
#define MF_MaxSector 	512		//���512����
#define MF_Endaddress	2097152 //Flash������ַ

#define FlashBinFileBassAddr			0 //�������ļ���Flash�д�ŵ���ʼ��ַ

//ǰ16�ֽ�������־λ
#define	FirstWriteAddr				0 	//1�ֽ�
#define	FacInitAddr					1 	//1�ֽ�
__MYFILE u8 FacInitFlag;//������ʼ����־

#define ResetFlagAddr				5	//

#define	CompanyInfoAddr				16 	//63�ֽ�
#define	KeyBaseInfoAddr				(16+64)	//16�ֽ�
#define	KDFInfoAddr					(16+80)	//48�ֽ�(ʣ��ռ䱣��(-256))
//#define 


#define MFFlagAddr						 256	//����ʹ�ñ�׼��ʼ��ַ, 64�ֽ�
#define BinFileDFAttributesAddr			(256+64)	//�������ļ���������ʼ��ַ, 16�ֽ�	
#define BinFileBassAttributesAddr		(256+128)	//�������ļ����Դ�Ż�ַ, 2048 - (256+128)
#define BinFileAttributesLen			(sizeof(BinFileAttributesType))//�������ļ����Խṹռ�ÿռ�(����)

#define RecordFileDFAttributesAddr		2048 		//��¼�ļ���������ʼ��ַ,0x800(2048), 16�ֽ�	
#define RecordFileBassAttributesAddr	(2048+64)	//��¼�ļ����Դ�Ż�ַ
#define RecordFileAttributesLen			(sizeof(RecordFileAttributesType))//��¼�ļ����Խṹռ�ÿռ�(����)

#define MaxBinFIDdCnt					((2048-128)/(sizeof(BinFileAttributesType)) )
#define MaxRecordFIDCnt					((2048-64)/(sizeof(RecordFileAttributesType)))

//MF�ṹ,Flashʹ�����
__packed typedef struct
{
	uint8_t SectorFlag[64];	//������ʹ�ñ�־,bit:1,�Ѿ�ʹ��,64�ֽڴ���512����
							//ûʹ��һ������,��λ��Ӧ��־,��������,�����־
}MFFlagType;
__MYFILE MFFlagType MFFlag;

//�������ļ��ܶ���
__packed typedef struct
{
	uint8_t 	TotalAttrFile;//�����ļ�����
	uint8_t 	DeleAttrFile;//ɾ���ļ�����, ɾ����, ����ɾ�������ļ��Ͷ�Ӧ�����ļ�,�ɽ��лָ�, ��ʽ���󳹵�ɾ��
	uint8_t 	AttrFileSize;//�����ļ���С
	uint32_t 	FirstAttrFileAddr;//��һ���ļ����Դ��λ��(EEPROM)
	uint32_t 	LastAttrFileAddr; //���һ���ļ����Դ��λ��(EEPROM)
	//uint8_t		Reserve[1];
}BinFileDFAttributesType;
__MYFILE BinFileDFAttributesType BinFileDFAttributes;//�������ļ������Խṹ(��)

//�������ļ����Խṹ����
__packed typedef struct
{
	uint8_t 	FID[2];//�ļ�ID
	uint16_t 	FileLenth;//�ļ��ܳ���
	uint32_t 	StartAddr;//�ļ���ʼ��ַ,BassAddr
	uint32_t 	OffsetAddr;//�ļ���ǰ��ַ,������ʹ�õ�ַ
	uint32_t 	RemainSpace;//ʣ��ռ�,=FileLenth-OffsetAddr
	uint8_t		FileCreadTime[6];
	uint8_t 	FLags;//��־
	
}BinFileAttributesType;
__MYFILE BinFileAttributesType BinFileAttributes;//�������ļ����Խṹ


//��¼�ļ��ܶ���
__packed typedef struct
{
	uint8_t 	TotalAttrFile;//�����ļ�����
	uint8_t 	DeleAttrFile;//ɾ���ļ�����, ɾ����, ����ɾ�������ļ��Ͷ�Ӧ�����ļ�,�ɽ��лָ�, ��ʽ���󳹵�ɾ��
	uint8_t 	AttrFileSize;//�����ļ���С
	uint32_t 	FirstAttrFileAddr;//��һ���ļ����Դ��λ��(EEPROM)
	uint32_t 	LastAttrFileAddr; //���һ���ļ����Դ��λ��(EEPROM)
	//uint8_t		Reserve[1];
}RecordFileDFAttributesType;
__MYFILE RecordFileDFAttributesType RecordFileDFAttributes;//��¼�ļ������Խṹ(��)
//��¼�ļ����Խṹ����
__packed typedef struct
{
	uint8_t 	FID[2];//�ļ�ID
	uint16_t 	RecordMax;//��¼�������
	uint8_t 	RecordLenth;//ÿ����¼����
	uint32_t 	StartAddr;//�ļ���ʼ��ַ,BassAddr
	uint32_t 	RecordIndex;//�ļ���ǰ��¼����,�Ѿ���¼������
	uint8_t		FileCreadTime[6];
	uint8_t 	FLags;//��־
}RecordFileAttributesType;
__MYFILE RecordFileAttributesType RecordFileAttributes;//��¼�ļ����Խṹ






//�ⲿ���ú�������
__MYFILE void EEPROM_ReadBytes(u16 Addr, u8 *Buffer, u16 Length);
__MYFILE void EEPROM_WriteBytes(u16 Addr, u8 *Buffer, u16 Length);
__MYFILE int8_t	WriteMFFlag(void);
__MYFILE int8_t	ReadMFFlag(void);
__MYFILE s16	CreatBinFile(u8 FileIDH, u8 FileIDL, u16 FileLen);
__MYFILE void 	EEPROM_WriteBytes(u16 Addr, u8 *Buffer, u16 Length);
__MYFILE s16	CreatRecordFile(u8 FileIDH, u8 FileIDL, u16 RecordNum, u8 RecordLen);
__MYFILE void 	FormatSystem(void);
__MYFILE s16	DeleteFile(u8 FileIDH, u8 FileIDL);
__MYFILE s16	DeleteSelfFile(u8 FileIDH, u8 FileIDL);
__MYFILE s16 	ClearRecord(u8 FileIDH, u8 FileIDL);
__MYFILE s16	WriteBinFile(u8 FileIDH, u8 FileIDL, u16 OffsetAddr, u8 BinDatLen, u8 *DatBuf);
__MYFILE s16	ReadBinFile(u8 FileIDH, u8 FileIDL, u16 OffsetAddr, u8 BinDatLen, u8 *DatBuf);
__MYFILE s16	ReadRecordFile(u8 FileIDH, u8 FileIDL, u8 IndexH, u8 IndexL, u8 *DatBuf);
__MYFILE s16	WriteRecordFile(u8 FileIDH, u8 FileIDL, u8 IndexH, u8 IndexL, u8 *DatBuf);
__MYFILE s16	WriteKeySelfFile(u8 FileIDH, u8 FileIDL, u16 OffsetAddr, u8 BinDatLen, u8 *DatBuf);
__MYFILE s16	ReadKeySelfFile(u8 FileIDH, u8 FileIDL, u16 OffsetAddr, u8 BinDatLen, u8 *DatBuf);
__MYFILE s16	FindRecord(u8 FileIDH, u8 FileIDL, u8 Offset, u8 KeyLenth, u8 *DatBuf);
__MYFILE s16	DeleRecord(u8 FileIDH, u8 FileIDL, u8 IndexH, u8 IndexL);
__MYFILE s16    ReadFileInfo(u8 FileIDH, u8 FileIDL);
__MYFILE s16	CreatBinFileEeprom(u8 FileIDH, u8 FileIDL, u16 FileLen);

__MYFILE s16	ReadRecordFileN(u8 FileIDH, u8 FileIDL, u8 IndexH, u8 IndexL, u8 rNum, u8 *DatBuf);
__MYFILE s16	WriteRecordFileN(u8 FileIDH, u8 FileIDL, u8 rNum, u8 rLen, u8 *DatBuf);

__MYFILE s32	FindDelRecordFile(u8 FileIDH, u8 FileIDL);

#endif
