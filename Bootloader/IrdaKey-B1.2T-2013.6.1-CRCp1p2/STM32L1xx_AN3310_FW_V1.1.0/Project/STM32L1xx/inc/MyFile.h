#ifndef  __MYAPP_H__
#define  __MYAPP_H__
 

#ifndef  MYFILE
	#define	 __MYFILE  extern
#else
	#define	 __MYFILE 
#endif

//#define __USECRCSEND
#define __USECRC


//定义返回的错误类型代码
#define		CmdOK				0x9000
#define		HandleErr			0x0101
#define		P1P2Err				0x0102
#define		FileLenErr			0x0103
#define		RecordLenErr		0x0104	
#define		RecordNumErr		0x0105
#define		NoOKRecord			0x0106
#define		FileIDExistErr		0x0107//文件已经存在,更换FID
#define		NoFileIDErr			0x0108//文件不存在,先创建
#define		NoSpaceErr			0x0109//空间不足,请格式化卡
#define		HaveExistErr		0x010A//已经存在该记录,不可写入
#define		CrcErr				0x010B//CRC错误
#define		WriteOnlyErr		0x010C//禁止读取


#define		OtherErr			0x01FF

__MYFILE u8 SysKeyID[4];

//===================信息结构=========================
//公司签名信息 63字节, 加密后为64字节???
__packed typedef struct
{
	uint8_t Version;//版本1
	uint8_t keyid[4];//KeyId
	uint8_t keyNo[20];//字符串,key打印序列号;       
	uint8_t PrDate[6];//生产日期, 年月日时分秒各一个字节
	uint8_t VenueCode[4];//场地编码
	uint8_t VenueName[20];//场地名称
	uint8_t AppType;//应用类型
	uint8_t KeyType;//制卡类型
	uint8_t Reserve[4];//保留	
	uint8_t uCRC[2];//CRC
}CompanyInfoType;


//===================记录结构=========================
//LTP: Lottery Ticket Printer
//彩票机初始化记录 16字节
__packed typedef struct
{
	uint8_t Version;//版本1
	uint8_t InitDate[6];//初始化时间
	uint8_t LTPCode[4];//彩票机编码
	uint8_t LTPNo[2];//彩票机机号
	uint8_t Reserve[1];//保留	
	uint8_t uCRC[2];//CRC
}LTPInitRecordType;

//彩票机设置记录 32字节
__packed typedef struct
{
	uint8_t Version;//版本1
	uint8_t SetDate[6];//设置时间
	uint8_t LTPCode[4];//彩票机编码
	uint8_t LTPNo[2];//彩票机机号
	uint8_t RTPol[1];//退票极性 0:NO 1:NC Recede Ticket Polarity
	uint8_t RTSpeed[1];//退票速度 0-7
	uint8_t RTTime[2];//退票倍率
	uint8_t AlarmSW[1];//报警开关
	uint8_t Reserve[12];//保留	
	uint8_t uCRC[2];//CRC
}LTPSetRecordType;

//彩票机开机记录 16字节
__packed typedef struct
{
	uint8_t Version;//版本1
	uint8_t InitDate[6];//初始化时间
	uint8_t LTPCode[4];//彩票机编码
	uint8_t Reserve[3];//保留	
	uint8_t uCRC[2];//CRC
}LTPBootRecordType;

//彩票机结账记录 48字节
__packed typedef struct
{
	uint8_t Version;//版本1
	uint8_t LTPCode[4];//彩票机编码
	uint8_t LTPNo[2];//彩票机机号
	uint8_t StartDate[6];//账目开始时间
	uint8_t BillDate[6];//结账时间
	uint8_t LastOverTicket[4];//上期余票
	uint8_t AddTicketTimes[2];//本期加票次数
	uint8_t AddTicket[4];//本期加票
	uint8_t RetTicketTimes[2];//本期退票次数
	uint8_t PrintTicketTime[2];//本期打票次数
	uint8_t RetTicket[4];//本期退票
	uint8_t OverTicket[4];//本期余票
	uint8_t Reserve[5];//保留	
	uint8_t uCRC[2];//CRC
}LTPBillRecordType;

//彩票机加票申请记录 64字节
__packed typedef struct
{
	uint8_t Version;//版本1
	uint8_t LTPCode[4];//彩票机编码
	uint8_t LTPNo[2];//彩票机机号
	uint8_t ApplyNo[4];//申请单号
	uint8_t ApplyDate[6];//申请时间
	uint8_t LastApplyNo[4];//上次申请单号
	uint8_t LastApplyDate[6];//上次申请时间
	uint8_t LastOverTicket[4];//上次余票
	uint8_t LastOverAmount[2];//上次余票张数
	uint8_t LastAddTicket[4];//上次加票
	uint8_t LastAddAmount[2];//上次加票张数
	uint8_t PrintTicket[4];//本期出票
	uint8_t PrintAmount[2];//本期出票张数
	uint8_t OverTicket[4];//余票
	uint8_t OverAmount[2];//余票张数
	uint8_t Reserve[11];//保留	
	uint8_t uCRC[2];//CRC
}LTPAddTicketApplyRecordType;

//彩票机出票记录 64字节
__packed typedef struct
{
	uint8_t Version;//版本1
	uint8_t LTPCode[4];//彩票机编码
	uint8_t LTPNo[2];//彩票机机号
	uint8_t TicketNo[4];//票号
	uint8_t LastOverTicket[4];//出票前余额
	uint8_t PrintTicket[4];//出票
	uint8_t RTOverTicket[4];//余额
	uint8_t RTStartDate[6];//开始退票时间
	uint8_t RTTimes[2];//退票次数
	uint8_t RTEndDate[6];//结束退票时间
	uint8_t PrintDate[6];//打印时间
	uint8_t LastPrintDate[6];//上次打印时间
	uint8_t AddTicketNo[4];//本期加票单号
	uint8_t Reserve[9];//保留	
	uint8_t uCRC[2];//CRC
}LTPPrintTicketRecordType;

//彩票机预加票记录 32字节
__packed typedef struct
{
	uint8_t Version;//版本1
	uint8_t LTPCode[4];//彩票机编码
	uint8_t LTPNo[2];//彩票机机号
	uint8_t AddTicketNo[4];//加票单号
	uint8_t ApplyNo[4];//申请单号
	uint8_t OverTicket[4];//余票
	uint8_t OverAmount[2];//余票张数
	uint8_t AddTicket[4];//加票
	uint8_t AddAmount[2];//加票张数
	uint8_t TicketNoID[2];//票号文件ID
	uint8_t Reserve[1];//保留	
	uint8_t uCRC[2];//CRC
}LTPAddTicketRecordType;

//彩票加票票号记录 6字节
__packed typedef struct
{
	uint8_t TicketNo[4];//票号	
	uint8_t uCRC[2];//CRC
}LTPAddTicketNoRecordType;

//__MYFILE u8 LTPAddTicketNoDelFlag[1250];//票号记录删除标志,0:删除, 1:未删除(擦除后默认)


//彩票机运行参数 32字节
__packed typedef struct
{
	uint8_t Version;//版本1
	uint8_t InitDate[6];//初始化时间
	uint8_t LTPNo[2];//彩票机机号
	uint8_t RTPol[1];//退票极性 0:NO 1:NC Recede Ticket Polarity
	uint8_t RTSpeed[1];//退票速度 0-7
	uint8_t RTTime[2];//退票倍率
	uint8_t AlarmSW[1];//报警开关
	uint8_t LastSetDate[6];//最后设置时间
	uint8_t Reserve[10];//保留	
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


//===================文件属性,存放于片内EEPROM中=========================

	
#define MF_SectorSize 	4096	//最小4K字节扇区(擦写最小单位)
#define MF_MaxSector 	512		//最大512扇区
#define MF_Endaddress	2097152 //Flash结束地址

#define FlashBinFileBassAddr			0 //二进制文件在Flash中存放的起始地址

//前16字节用作标志位
#define	FirstWriteAddr				0 	//1字节
#define	FacInitAddr					1 	//1字节
__MYFILE u8 FacInitFlag;//工厂初始化标志

#define ResetFlagAddr				5	//

#define	CompanyInfoAddr				16 	//63字节
#define	KeyBaseInfoAddr				(16+64)	//16字节
#define	KDFInfoAddr					(16+80)	//48字节(剩余空间保留(-256))
//#define 


#define MFFlagAddr						 256	//扇区使用标准起始地址, 64字节
#define BinFileDFAttributesAddr			(256+64)	//二进制文件总属性起始地址, 16字节	
#define BinFileBassAttributesAddr		(256+128)	//二进制文件属性存放基址, 2048 - (256+128)
#define BinFileAttributesLen			(sizeof(BinFileAttributesType))//二进制文件属性结构占用空间(长度)

#define RecordFileDFAttributesAddr		2048 		//记录文件总属性起始地址,0x800(2048), 16字节	
#define RecordFileBassAttributesAddr	(2048+64)	//记录文件属性存放基址
#define RecordFileAttributesLen			(sizeof(RecordFileAttributesType))//记录文件属性结构占用空间(长度)

#define MaxBinFIDdCnt					((2048-128)/(sizeof(BinFileAttributesType)) )
#define MaxRecordFIDCnt					((2048-64)/(sizeof(RecordFileAttributesType)))

//MF结构,Flash使用情况
__packed typedef struct
{
	uint8_t SectorFlag[64];	//扇区已使用标志,bit:1,已经使用,64字节代表512扇区
							//没使用一个扇区,置位对应标志,擦除扇区,清除标志
}MFFlagType;
__MYFILE MFFlagType MFFlag;

//二进制文件总定义
__packed typedef struct
{
	uint8_t 	TotalAttrFile;//现有文件总数
	uint8_t 	DeleAttrFile;//删除文件总数, 删除后, 并不删除属性文件和对应数据文件,可进行恢复, 格式化后彻底删除
	uint8_t 	AttrFileSize;//属性文件大小
	uint32_t 	FirstAttrFileAddr;//第一个文件属性存放位置(EEPROM)
	uint32_t 	LastAttrFileAddr; //最后一个文件属性存放位置(EEPROM)
	//uint8_t		Reserve[1];
}BinFileDFAttributesType;
__MYFILE BinFileDFAttributesType BinFileDFAttributes;//二进制文件夹属性结构(总)

//二进制文件属性结构定义
__packed typedef struct
{
	uint8_t 	FID[2];//文件ID
	uint16_t 	FileLenth;//文件总长度
	uint32_t 	StartAddr;//文件起始地址,BassAddr
	uint32_t 	OffsetAddr;//文件当前地址,等于已使用地址
	uint32_t 	RemainSpace;//剩余空间,=FileLenth-OffsetAddr
	uint8_t		FileCreadTime[6];
	uint8_t 	FLags;//标志
	
}BinFileAttributesType;
__MYFILE BinFileAttributesType BinFileAttributes;//二进制文件属性结构


//记录文件总定义
__packed typedef struct
{
	uint8_t 	TotalAttrFile;//现有文件总数
	uint8_t 	DeleAttrFile;//删除文件总数, 删除后, 并不删除属性文件和对应数据文件,可进行恢复, 格式化后彻底删除
	uint8_t 	AttrFileSize;//属性文件大小
	uint32_t 	FirstAttrFileAddr;//第一个文件属性存放位置(EEPROM)
	uint32_t 	LastAttrFileAddr; //最后一个文件属性存放位置(EEPROM)
	//uint8_t		Reserve[1];
}RecordFileDFAttributesType;
__MYFILE RecordFileDFAttributesType RecordFileDFAttributes;//记录文件夹属性结构(总)
//记录文件属性结构定义
__packed typedef struct
{
	uint8_t 	FID[2];//文件ID
	uint16_t 	RecordMax;//记录最大条数
	uint8_t 	RecordLenth;//每条记录长度
	uint32_t 	StartAddr;//文件起始地址,BassAddr
	uint32_t 	RecordIndex;//文件当前记录索引,已经记录的条数
	uint8_t		FileCreadTime[6];
	uint8_t 	FLags;//标志
}RecordFileAttributesType;
__MYFILE RecordFileAttributesType RecordFileAttributes;//记录文件属性结构






//外部调用函数声明
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
