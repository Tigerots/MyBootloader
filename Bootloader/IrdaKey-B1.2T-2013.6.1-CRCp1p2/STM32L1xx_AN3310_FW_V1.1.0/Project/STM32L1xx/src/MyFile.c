#define MYFILE

/*********************************************************
*************************说明*****************************

    所以文件的属性文件都存放于EEPROM中, 文件内容存放于FLASH
中,要进行文件的创建,删除,查找,写入,读取等操作,需要先从文件属
性中读取属性信息,在进行操作,操作完成后修改对于的属性信息.
	除了操作对于文件的属性文件,根文件(MF),目录文件(*DF)通样
需要做相应的修改,保持属性文件与文件内容的对应.
**********************************************************/
#include "include.h"
#include "AES.h"


//片内EEPROM定义,大小为4K字节
#define EEPROM_BASE_ADDR    0x08080000    
#define EEPROM_BYTE_SIZE    0x0FFF  

//key基本信息 16字节
__packed typedef struct
{
	uint8_t Version;//版本1
	uint8_t KeyType;//制卡类型
	uint8_t Prdate[6];//登记时间,年月日时分秒各一个字节
	uint8_t KeyId[4];//保留
	uint8_t Reserve[2];//保留	
	uint8_t uCRC[2];//CRC
}KeyBaseInfoType;
__MYFILE KeyBaseInfoType KeyBaseInfo;

//KDF信息(钥匙卡) 48字节
__packed typedef struct
{
	uint8_t Version;//版本1
	uint8_t keyid[4];//KeyId
	uint8_t VenueCode[4];//场地编码
	uint8_t VenueName[20];//场地名称
	uint8_t VenueDate[6];//场地注册日期, 年月日时分秒各一个字节	
	uint8_t InitPassWD[4];//发卡密码; 
	uint8_t DataPassWD[2];//数据密钥因子
	uint8_t Reserve[5];//保留	
	uint8_t uCRC[2];//CRC
}KDFInfoType;
__MYFILE KDFInfoType KDFInfo;

extern u8 AESInitStatus;
extern FLASH_Status DATA_EEPROM_ProgramByte(uint32_t Address, u8 Data);
/*********************************************************
函数描述: 	读取片内EEPROM中的数据
入口参数: 	
出口参数: 	
修改记录:

**********************************************************/ 
void EEPROM_ReadBytes(u16 Addr, u8 *Buffer, u16 Length)  
{ 
    //Length=Length/4;
	while(Length--)
	{
		*Buffer=*(u8 *)(EEPROM_BASE_ADDR+Addr);
		Buffer = Buffer+1;
		Addr= Addr + 1;		
    }     
} 
/*********************************************************
函数描述: 	将数据写入到EEPROM, 在写入过程中禁用中断,所以
			在程序设计中,需要写数据的流程应该等待一包数据
			写完后再进行下一次操作.
入口参数: 	
出口参数: 	
修改记录:

**********************************************************/
void EEPROM_WriteBytes(u16 Addr, u8 *Buffer, u16 Length)  
{ 
	uint32_t wAddr; 

	//Length=Length/4;
	
    wAddr=(EEPROM_BASE_ADDR+Addr);  
	__disable_irq();	
    DATA_EEPROM_Unlock(); //解锁
    while(FLASH->PECR&FLASH_PECR_PELOCK);  
		FLASH->PECR |= FLASH_PECR_FTDW;        //not fast write  
    while(Length--)
	{  
        DATA_EEPROM_ProgramByte(wAddr, *Buffer);
		//DATA_EEPROM_ProgramWord(wAddr, *Buffer);
		wAddr = wAddr+1;		
		Buffer = Buffer+1; 
    }  
    DATA_EEPROM_Lock(); 
	__enable_irq(); 
} 

/*********************************************************
函数描述: 	擦除 EEPROM
入口参数: 	
出口参数: 
修改记录:

**********************************************************/
void EraseAllEeprom(void)
{
	uint32_t wAddr, Length, tLen;
	
	Length=(4096-256)/4; //4096字节=1024字  

	tLen = 512;
    wAddr=(EEPROM_BASE_ADDR+16);  //前256字节不格式化?
	__disable_irq();	
    DATA_EEPROM_Unlock(); //解锁
    while(FLASH->PECR&FLASH_PECR_PELOCK);  
		FLASH->PECR|=FLASH_PECR_FTDW;        //not fast write  
    while(tLen--)
	{  
        DATA_EEPROM_EraseWord(wAddr);
		wAddr=wAddr+4;
    }  
    DATA_EEPROM_Lock(); 
	__enable_irq(); 
	
	IWDG_ReloadCounter(); 
	wAddr=(EEPROM_BASE_ADDR+16+512*4); 
	tLen = Length-512;
	
	__disable_irq();	
    DATA_EEPROM_Unlock(); //解锁
    while(FLASH->PECR&FLASH_PECR_PELOCK);  
		FLASH->PECR|=FLASH_PECR_FTDW;        //not fast write  
    while(tLen--)
	{  
        DATA_EEPROM_EraseWord(wAddr);
		wAddr=wAddr+4;
    }  
    DATA_EEPROM_Lock(); 
	__enable_irq(); 
	
	IWDG_ReloadCounter(); 
}


/*********************************************************
函数描述: 	读取MF结构,Flash使用情况, 存放于EEPROM中
入口参数: 	
出口参数: 	
修改记录:

**********************************************************/
s8	ReadMFFlag(void)
{
	//从内部EEPROM中读取MF的扇区使用情况
	EEPROM_ReadBytes(MFFlagAddr, (u8 *)&MFFlag.SectorFlag, sizeof(MFFlag));
	
	return 0;
}
/*********************************************************
函数描述: 	读取MF结构,Flash使用情况, 存放于EEPROM中
入口参数: 	
出口参数: 	
修改记录:

**********************************************************/
s8	WriteMFFlag(void)
{	
	//从内部EEPROM中读取MF的扇区使用情况
	EEPROM_WriteBytes(MFFlagAddr, (u8 *)&MFFlag.SectorFlag, sizeof(MFFlag));
	
	return 0;
}

/*********************************************************
函数描述: 	计算剩余扇区数
入口参数: 	
出口参数: 	Sector: 剩余的扇区数, 0-511
修改记录:

**********************************************************/
u16 RemainSector(void)
{
	u16 Sector=0,i=0;
	u8 j=0;
	
	ReadMFFlag();
	
	for(i=0; i<64; i++)
	{
		for(j=0; j<8; j++)
		{
			if(((MFFlag.SectorFlag[i]>>j)&1)==0)
				Sector++;
		}
	}
	return Sector;
}
/*********************************************************
函数描述: 	修改扇区使用标志
入口参数: 	SectorCnt: 需要使用的连续扇区数
出口参数: 	SectorNum: 0-511 满足要求的,剩余的扇区的起始地址
					   -1:   无满足要求的连续扇区
修改记录:

**********************************************************/
void SetMFSectorFlag(u16 SectorNum, u8 SectorCnt)
{
	u8 i=0, j=0, m=0, n=0, k=0;
	
	//计算开始位置
	m = SectorNum/8;
	n = SectorNum%8;
	//计算用了几个完整字节的标志
	if((SectorCnt > (8-n)))
	{
		i = (SectorCnt-(8-n))/8;
		j = (SectorCnt-(8-n))%8;
		
		for(k=n; k<8; k++)
		{
			MFFlag.SectorFlag[m] = MFFlag.SectorFlag[m] | (1<<k);
		}
		if(i!=0)
		{
			memset((u8 *)&MFFlag.SectorFlag[m+1], 0xFF, i);
		}
		for(k=0; k<j; k++)
		{
			MFFlag.SectorFlag[m+1+i] = MFFlag.SectorFlag[m+1+i] | (1<<k);
		}
	}
	else
	{
		i = 0;
		j = n+SectorCnt;
		for(k=n; k<j; k++)
		{
			MFFlag.SectorFlag[m] = MFFlag.SectorFlag[m] | (1<<k);
		}
	}
	
	
	
	WriteMFFlag();//写入EEPROM
}
//清空对应扇区标志
void ClearMFSectorFlag(u16 SectorNum, u8 SectorCnt)
{
	u8 i=0, j=0, m=0, n=0, k=0;
	
	//计算开始位置
	m = SectorNum/8;
	n = SectorNum%8;
	//计算用了几个完整字节的标志
	if((SectorCnt > (8-n)))
	{
		i = (SectorCnt-(8-n))/8;
		j = (SectorCnt-(8-n))%8;
		
		for(k=n; k<8; k++)
		{
			MFFlag.SectorFlag[m] = MFFlag.SectorFlag[m] & (~(1<<k));
		}
		if(i!=0)
		{
			memset((u8 *)&MFFlag.SectorFlag[m+1], 0xFF, i);
		}
		for(k=0; k<j; k++)
		{
			MFFlag.SectorFlag[m+1+i] = MFFlag.SectorFlag[m+1+i] & (~(1<<k));
		}
	}
	else
	{
		i = 0;
		j = n+SectorCnt;
		for(k=n; k<j; k++)
		{
			MFFlag.SectorFlag[m] = MFFlag.SectorFlag[m] & (~(1<<k));
		}
	}
	WriteMFFlag();//写入EEPROM
}
/*********************************************************
函数描述: 	计算剩余扇区数
入口参数: 	SectorCnt: 需要使用的连续扇区数
出口参数: 	SectorNum: 0-511 满足要求的,剩余的扇区的起始地址
					   -1:   无满足要求的连续扇区
修改记录:

**********************************************************/
s16 FindSector(u8 SectorCnt)
{
	s16 i=0,k=0;
	s16 SectorNum=-1;
	u8 j=0, step = 0;
	
	for(i=0; i<64; i++)
	{
		for(j=0; j<8; j++)
		{
			if(((MFFlag.SectorFlag[i]>>j)&1)==0)
			{
				if(step==0)
				{
					SectorNum = i*8 + j;
					step=1;					
				}
				k++;
				if(k>=SectorCnt)
				{
					return SectorNum;
				}
			}
			else
			{
				step=0;
				k=0;
				SectorNum = -1;
			}
		}
	}
	return SectorNum;
}
/*********************************************************
函数描述: 	查找文件(FID)
入口参数: 	FileID
出口参数: 	AttrFileAddrTmp: 有效地址 
			-1: 	无该文件
			-2:		该文件已经删除
修改记录:

**********************************************************/
s32	FindBinFile(u8 FileIDH, u8 FileIDL)
{
	u8 i;
	s32 AttrFileAddrTmp = 0;
	
	//读取总属性记录文件,需要使用总文件数,起始地址
	EEPROM_ReadBytes(BinFileDFAttributesAddr, (u8 *)&BinFileDFAttributes, sizeof(BinFileDFAttributesType));
	
	//查找文件ID
	for(i=0; i<( MaxBinFIDdCnt ); i++)
	{
		AttrFileAddrTmp = BinFileDFAttributes.FirstAttrFileAddr + i*(sizeof(BinFileAttributesType));
		EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&BinFileAttributes, sizeof(BinFileAttributesType));
		if((BinFileAttributes.FID[0] == FileIDH)&&(BinFileAttributes.FID[1] == FileIDL))
		{
			if(BinFileAttributes.FLags == 1)
			{
				return AttrFileAddrTmp;
			}
		}
	}
	return -1;//无对应文件
}
/*********************************************************
函数描述: 	查找文件(FID)
入口参数: 	FileID
出口参数: 	AttrFileAddrTmp: 有效地址 
			-1: 	无该文件
			-2:		该文件已经删除
修改记录:

**********************************************************/
s32	FindRecordFile(u8 FileIDH, u8 FileIDL)
{
	u8 i;
	s32 ret = -1, AttrFileAddrTmp = 0;

	
	//读取总属性记录文件,需要使用总文件数,起始地址
	EEPROM_ReadBytes(RecordFileDFAttributesAddr, (u8 *)&RecordFileDFAttributes, sizeof(RecordFileDFAttributesType));
	//查找文件ID
	for(i=0; i<(MaxRecordFIDCnt); i++)
	{
		AttrFileAddrTmp = RecordFileDFAttributes.FirstAttrFileAddr + i*(sizeof(RecordFileAttributesType));
		EEPROM_ReadBytes((u32)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
		if((RecordFileAttributes.FID[0] == FileIDH)&&(RecordFileAttributes.FID[1] == FileIDL))
		{
			if(RecordFileAttributes.FLags == 0)
			{
				ret = -1;
			}
			if(RecordFileAttributes.FLags == 1)
			{
				return AttrFileAddrTmp;
			}
		}
	}
	return ret;//无对应文件
}

/*********************************************************
函数描述: 	查找文件(FID)
入口参数: 	FileID
出口参数: 	AttrFileAddrTmp: 有效地址 
			-1: 	无该文件
			-2:		该文件已经删除
修改记录:

**********************************************************/
s32	FindDelRecordFile(u8 FileIDH, u8 FileIDL)
{
	u8 i;
	s32 ret = -1, AttrFileAddrTmp = 0;

	
	//读取总属性记录文件,需要使用总文件数,起始地址
	EEPROM_ReadBytes(RecordFileDFAttributesAddr, (u8 *)&RecordFileDFAttributes, sizeof(RecordFileDFAttributesType));
	//查找文件ID
	for(i=0; i<(MaxRecordFIDCnt); i++)
	{
		AttrFileAddrTmp = RecordFileDFAttributes.FirstAttrFileAddr + i*(sizeof(RecordFileAttributesType));
		EEPROM_ReadBytes((u32)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
		if((RecordFileAttributes.FID[0] == FileIDH)&&(RecordFileAttributes.FID[1] == FileIDL))
		{
			if(RecordFileAttributes.FLags != 0)
			{
				ret = -1;
			}
			if(RecordFileAttributes.FLags == 0)
			{
				return AttrFileAddrTmp;
			}
		}
	}
	return ret;//无对应文件
}

/*********************************************************
函数描述: 	删除文件
入口参数: 	FileID
出口参数: 	0: 成功
			1: FID不存在
			2: 
			3: 
			-1: 其他错误
修改记录:

**********************************************************/
s16	DeleteBinFile(u8 FileIDH, u8 FileIDL)
{
//	u16 RSector=0, NeedSector=0;
//	s16 SectorAddr=-1;
	s32 AttrFileAddrTmp = 0;
//	BinFileDFAttributesType BinFileDFAttributes;//二进制文件夹属性结构(总)
//	BinFileAttributesType BinFileAttributes;//二进制文件属性结构
	
//=========1. 查找文件================================
	AttrFileAddrTmp = FindBinFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
	if((AttrFileAddrTmp == -1))
	{
		return NoFileIDErr;//文件不存在
	}
	
//=========2. 取消文件有效标志========================
	//--------以下新建立二进制属性文件--------
	EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&BinFileAttributes, sizeof(BinFileAttributesType));
	if(BinFileAttributes.FLags == 1)
	{
		BinFileAttributes.FLags = 0;//文件有效标志,0:无效
		BinFileAttributes.FID[0] = 0;//文件ID
		BinFileAttributes.FID[1] = 0;
		//写当前二进制文件的属性文件
		EEPROM_WriteBytes(AttrFileAddrTmp, (u8 *)&BinFileAttributes, sizeof(BinFileAttributesType));
		
		//取消对应Flash已使用标志
	}
	else
	{
		return OtherErr;
	}
	
//=========3. 修改总属性文件==========================
	//读取总属性记录文件
	EEPROM_ReadBytes(BinFileDFAttributesAddr, (u8 *)&BinFileDFAttributes, sizeof(BinFileDFAttributesType));
	//--------以下修改二进制总记录属性文件--------
	if( BinFileDFAttributes.TotalAttrFile !=0 )
	{
		BinFileDFAttributes.TotalAttrFile = BinFileDFAttributes.TotalAttrFile - 1;//有效文件总数-1
		//BinFileDFAttributes.DeleAttrFile = BinFileDFAttributes.DeleAttrFile + 1;//删除文件总数+1
		BinFileDFAttributes.DeleAttrFile = 0;//不使用删除文件总数
		//保存总文件记录属性文件
		EEPROM_WriteBytes(BinFileDFAttributesAddr, (u8 *)&BinFileDFAttributes, sizeof(BinFileDFAttributesType));
	}
	else
	{
		return OtherErr;
	}
	
	return CmdOK;//成功
}
/*********************************************************
函数描述: 	删除文件
入口参数: 	FileID
出口参数: 	0: 成功
			1: FID不存在
			2: 
			3: 
			-1: 其他错误
修改记录:

**********************************************************/
s16	DeleteRecordFile(u8 FileIDH, u8 FileIDL)
{
//	u16 RSector=0, NeedSector=0;
//	s16 SectorAddr=-1;
	s32 AttrFileAddrTmp = 0;
//	RecordFileDFAttributesType RecordFileDFAttributes;//记录文件夹属性结构(总)
//	RecordFileAttributesType RecordFileAttributes;//记录文件属性结构
	
//=========1. 查找文件================================
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
	if((AttrFileAddrTmp == -1)||(AttrFileAddrTmp == -2))
	{
		return NoFileIDErr;//文件不存在
	}
	
//=========2. 取消文件有效标志========================
	//--------以下新建立二进制属性文件--------
	EEPROM_ReadBytes((u32)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	if(RecordFileAttributes.FLags == 1)
	{
		RecordFileAttributes.FLags = 0;//文件有效标志,0:无效
		RecordFileAttributes.FID[0] = 0;//文件ID
		RecordFileAttributes.FID[1] = 0;
		//写当前二进制文件的属性文件
		EEPROM_WriteBytes(AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
		//清空对应Flash扇区使用标志
		ClearMFSectorFlag(RecordFileAttributes.StartAddr/MF_SectorSize, 1);
	}
	else
	{
		return NoOKRecord;
	}
	
//=========3. 修改总属性文件==========================
	//读取总属性记录文件
	EEPROM_ReadBytes(RecordFileDFAttributesAddr, (u8 *)&RecordFileDFAttributes, sizeof(RecordFileDFAttributesType));
	//--------以下修改二进制总记录属性文件--------
	if( RecordFileDFAttributes.TotalAttrFile !=0 )
	{
		RecordFileDFAttributes.TotalAttrFile = RecordFileDFAttributes.TotalAttrFile;//有效文件总数-1
		RecordFileDFAttributes.DeleAttrFile = RecordFileDFAttributes.DeleAttrFile + 1;//删除文件总数+1
		//保存总文件记录属性文件
		EEPROM_WriteBytes(RecordFileDFAttributesAddr, (u8 *)&RecordFileDFAttributes, sizeof(RecordFileDFAttributesType));
	}
	else
	{
		return NoOKRecord;
	}
	
	return CmdOK;//成功
}
/*********************************************************
函数描述: 	情空记录
入口参数: 	FileID
出口参数: 	
修改记录:

**********************************************************/
s16 ClearRecord(u8 FileIDH, u8 FileIDL)
{
	s32 AttrFileAddrTmp = 0;
	u16 NeedSector=0; u32 FileLen=0;
	u8 i=0;
	
	//=========1. 查找文件================================
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
	if((AttrFileAddrTmp == -1)||(AttrFileAddrTmp == -2))
	{
		return NoFileIDErr;//文件不存在
	}
	
	//=========2. 取消文件写入内容========================
	EEPROM_ReadBytes((u32)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	if(RecordFileAttributes.FLags == 1)
	{
		//RecordFileAttributes.FLags = 0;//文件有效标志,0:无效
		RecordFileAttributes.RecordIndex = 0;//已经记录的条数清0
		//写当前二进制文件的属性文件
		if((u16)AttrFileAddrTmp < 256)
		{//属性地址不能小于256
			return OtherErr;
		}
		EEPROM_WriteBytes(AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
		
		//=========3. 清空Flash========================
		FileLen=RecordFileAttributes.RecordLenth * RecordFileAttributes.RecordMax;
		NeedSector=FileLen/MF_SectorSize;
		if((FileLen%MF_SectorSize)!=0)
		{
			NeedSector = NeedSector+1;
		}
		for(i=0; i<NeedSector; i++)
		{
			SPI_Flash_Erase_Sector(RecordFileAttributes.StartAddr + i*MF_SectorSize);
		}
	}
	else
	{
		return NoOKRecord;
	}
	return CmdOK;
}
/*********************************************************
函数描述: 	删除文件
入口参数: 	FileID
出口参数: 	
修改记录:

**********************************************************/
s16	DeleteFile(u8 FileIDH, u8 FileIDL)
{
	u16 ret;
	
	ret = DeleteBinFile(FileIDH,FileIDL);
	if(ret != CmdOK )
	{
		ret = DeleteRecordFile(FileIDH,FileIDL);
		if(ret !=CmdOK )
		{
			return ret;//文件不存在
		}
	}
	return CmdOK;//成功
}


/*********************************************************
函数描述: 	创建二进制文件
入口参数: 	FileID, FileLen
出口参数: 	0: 成功
			1: 空间不足
			2: 文件ID错误
			3: 文件长度无效
			-1: 其他错误
修改记录:

**********************************************************/
extern u8 SysCurrTime[6];
s16	CreatBinFile(u8 FileIDH, u8 FileIDL, u16 FileLen)
{
	u16 RSector=0, NeedSector=0;
	s16 SectorAddr=-1;
	s32 AttrFileAddrTmp = 0;
	
	//查找文件
	AttrFileAddrTmp = FindBinFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
	if((AttrFileAddrTmp != -1))
	{
		return FileIDExistErr;//文件已经存在
	}
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
	if((AttrFileAddrTmp != -1))
	{
		return FileIDExistErr;//文件已经存在
	}
	
	//计算所需空间
	NeedSector=FileLen/MF_SectorSize;
	if((FileLen%MF_SectorSize)!=0)
	{
		NeedSector = NeedSector+1;
	}
	
	RSector = RemainSector();//读取剩余扇区数
	if(RSector < NeedSector)//////////////////////////////////////////空间不足时,删除无效文件,释放空间,暂时未添加
	{//剩余空间不足
		return FileLenErr;//长度无效
	}
	SectorAddr = FindSector(NeedSector);//查找可用扇区
	if(SectorAddr == -1)
	{//无满足要求连续扇区
		return NoSpaceErr;
	}
	SPI_Flash_Erase_Sector((SectorAddr)*MF_SectorSize);
	if(FileLen > 256)
	{//文件长度无效
		return FileLenErr;//长度无效0x0003
	}
	//读取总属性记录文件
	EEPROM_ReadBytes(BinFileDFAttributesAddr, (u8 *)&BinFileDFAttributes, sizeof(BinFileDFAttributesType));
	//--------以下修改二进制总记录属性文件--------
	if(BinFileDFAttributes.TotalAttrFile==0)
	{
		BinFileDFAttributes.FirstAttrFileAddr = BinFileBassAttributesAddr;//从第一文件地址开始存放
		BinFileDFAttributes.LastAttrFileAddr = BinFileBassAttributesAddr;//当前文件即是第一个文件,又是最后一个文件
		BinFileDFAttributes.TotalAttrFile = 1;
		BinFileDFAttributes.AttrFileSize = sizeof(BinFileAttributesType);
	}
	else
	{
		//修改当前文件为最后创建的二进制文件
		BinFileDFAttributes.LastAttrFileAddr = BinFileDFAttributes.LastAttrFileAddr + sizeof(BinFileAttributesType);
		BinFileDFAttributes.TotalAttrFile++;
		if((BinFileDFAttributes.TotalAttrFile+BinFileDFAttributes.DeleAttrFile) > MaxBinFIDdCnt)
		{
			return NoSpaceErr;
		}
	}
	//保存总文件记录属性文件
	EEPROM_WriteBytes(BinFileDFAttributesAddr, (u8 *)&BinFileDFAttributes, sizeof(BinFileDFAttributesType));
	
	//--------以下新建立二进制属性文件--------
	BinFileAttributes.FID[0] = FileIDH;//文件ID
	BinFileAttributes.FID[1] = FileIDL;
	BinFileAttributes.FileLenth = FileLen;//文件总长度
	BinFileAttributes.StartAddr = (SectorAddr)*MF_SectorSize;//文件起始地址,BassAddr, 对应于Flash空间
	BinFileAttributes.OffsetAddr = 0;//文件当前地址,等于已使用地址,创建时未使用
	BinFileAttributes.RemainSpace = FileLen;//剩余空间,=FileLenth-OffsetAddr
	memcpy(BinFileAttributes.FileCreadTime, SysCurrTime, 6);
	BinFileAttributes.FLags = 1;//文件有效标志
	//写当前二进制文件的属性文件
	EEPROM_WriteBytes(BinFileDFAttributes.LastAttrFileAddr, (u8 *)&BinFileAttributes, sizeof(BinFileAttributesType));
	
	//修改MF属性
	SetMFSectorFlag(SectorAddr, NeedSector);
	
	return CmdOK;//成功
}
s16	CreatBinFileEeprom(u8 FileIDH, u8 FileIDL, u16 FileLen)
{
	s32 AttrFileAddrTmp = 0;
	
	//查找文件
	AttrFileAddrTmp = FindBinFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
	if((AttrFileAddrTmp != -1))
	{
		return FileIDExistErr;//文件已经存在
	}
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
	if((AttrFileAddrTmp != -1))
	{
		return FileIDExistErr;//文件已经存在
	}
	
	if(FileLen > 256)
	{//文件长度无效
		return FileLenErr;//长度无效
	}
	//读取总属性记录文件
	EEPROM_ReadBytes(BinFileDFAttributesAddr, (u8 *)&BinFileDFAttributes, sizeof(BinFileDFAttributesType));
	//--------以下修改二进制总记录属性文件--------
	if(BinFileDFAttributes.TotalAttrFile==0)
	{
		BinFileDFAttributes.FirstAttrFileAddr = BinFileBassAttributesAddr;//从第一文件地址开始存放
		BinFileDFAttributes.LastAttrFileAddr = BinFileBassAttributesAddr;//当前文件即是第一个文件,又是最后一个文件
		BinFileDFAttributes.TotalAttrFile = 1;
		BinFileDFAttributes.AttrFileSize = sizeof(BinFileAttributesType);
	}
	else
	{
		//修改当前文件为最后创建的二进制文件
		BinFileDFAttributes.LastAttrFileAddr = BinFileDFAttributes.LastAttrFileAddr + sizeof(BinFileAttributesType);
		BinFileDFAttributes.TotalAttrFile++;
		if((BinFileDFAttributes.TotalAttrFile+BinFileDFAttributes.DeleAttrFile) > MaxBinFIDdCnt)
		{
			return NoSpaceErr;
		}
	}
	//保存总文件记录属性文件
	EEPROM_WriteBytes(BinFileDFAttributesAddr, (u8 *)&BinFileDFAttributes, sizeof(BinFileDFAttributesType));
	
	//--------以下新建立二进制属性文件--------
	BinFileAttributes.FID[0] = FileIDH;//文件ID
	BinFileAttributes.FID[1] = FileIDL;
	BinFileAttributes.FileLenth = FileLen;//文件总长度
	BinFileAttributes.StartAddr = 0;//存放于EEPROM,无需Flash
	BinFileAttributes.OffsetAddr = 0;//文件当前地址,等于已使用地址,创建时未使用
	BinFileAttributes.RemainSpace = FileLen;//剩余空间,=FileLenth-OffsetAddr
	memcpy(BinFileAttributes.FileCreadTime, SysCurrTime, 6);
	BinFileAttributes.FLags = 1;//文件有效标志
	//写当前二进制文件的属性文件
	
	if(BinFileDFAttributes.LastAttrFileAddr < 256)
	{//属性地址不能小于256
		return OtherErr;
	}
	EEPROM_WriteBytes(BinFileDFAttributes.LastAttrFileAddr, (u8 *)&BinFileAttributes, sizeof(BinFileAttributesType));
	
	
	return CmdOK;//成功
}

/*********************************************************
函数描述: 	创建定长记录文件
入口参数: 	FileID		文件ID
			RecordLen	单条记录长度
			RecordNum	记录条数
出口参数: 	0: 成功
			1: 空间不足
			2: 
			-1: 其他错误
修改记录:

**********************************************************/
s16	CreatRecordFile(u8 FileIDH, u8 FileIDL, u16 RecordNum, u8 RecordLen)
{
	u16 RSector=0, NeedSector=0; u32 FileLen=0;
	s16 SectorAddr=-1;
	s32 AttrFileAddrTmp = 0;
	
	//查找文件
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
	if(((s32)AttrFileAddrTmp != -1))
	{
		return FileIDExistErr;//文件已经存在
	}
	AttrFileAddrTmp = FindBinFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
	if(((s32)AttrFileAddrTmp != -1))
	{
		return FileIDExistErr;//文件已经存在
	}
	
	if(RecordLen >= 255)
	{
		return RecordLenErr;//记录长度无效
	}
	if(RecordNum >= 10000)
	{
		return RecordNumErr;//记录长度无效
	}
	
	//计算所需空间
	FileLen=RecordLen*RecordNum;
	NeedSector=FileLen/MF_SectorSize;
	if((FileLen%MF_SectorSize)!=0)
	{
		NeedSector = NeedSector+1;
	}
	
	RSector = RemainSector();//读取剩余扇区数
	if(RSector < NeedSector)
	{//剩余空间不足
		return NoSpaceErr;
	}
	SectorAddr = FindSector(NeedSector);//查找可用扇区
	if(SectorAddr == -1)
	{//无满足要求连续扇区
		return NoSpaceErr;
	}
	//格式化扇区
	//RecordFileAttributes.StartAddr = (SectorAddr)*MF_SectorSize;
	SPI_Flash_Erase_Sector((SectorAddr)*MF_SectorSize);
	
	if(FileLen == 0)//RecordLen*RecordNum
	{//文件长度无效
		return RecordLenErr;
	}
	//读取总属性记录文件
	EEPROM_ReadBytes(RecordFileDFAttributesAddr, (u8 *)&RecordFileDFAttributes, sizeof(RecordFileDFAttributesType));
	//--------以下修改二进制总记录属性文件--------
	if(RecordFileDFAttributes.TotalAttrFile==0)
	{
		RecordFileDFAttributes.FirstAttrFileAddr = RecordFileBassAttributesAddr;//从第一文件地址开始存放
		RecordFileDFAttributes.LastAttrFileAddr = RecordFileBassAttributesAddr;//当前文件即是第一个文件,又是最后一个文件
		RecordFileDFAttributes.TotalAttrFile = 1;
		RecordFileDFAttributes.AttrFileSize = sizeof(RecordFileAttributesType);
		AttrFileAddrTmp = RecordFileDFAttributes.FirstAttrFileAddr;
	}
	else
	{
		//修改当前文件为最后创建的二进制文件
		if(RecordFileDFAttributes.DeleAttrFile != 0)//如果有已经删除文件,则使用已经删除文件的属性位置
		{
			//查找第一个删除文件的位置
			//创建文件,但文件总数目不增加
			AttrFileAddrTmp = FindDelRecordFile(0,0);
			if(((s32)AttrFileAddrTmp == -1))
			{
				return FileIDExistErr;//文件已经存在
			}
			RecordFileDFAttributes.DeleAttrFile--;
		}
		else
		{
			RecordFileDFAttributes.LastAttrFileAddr = RecordFileDFAttributes.LastAttrFileAddr + sizeof(RecordFileAttributesType);
			RecordFileDFAttributes.TotalAttrFile++;
			if((RecordFileDFAttributes.TotalAttrFile) > MaxRecordFIDCnt)//超过最大文件数目
			{
				return NoSpaceErr;
			}
			AttrFileAddrTmp = RecordFileDFAttributes.LastAttrFileAddr;
		}
	}
	//保存总文件记录属性文件
	EEPROM_WriteBytes(RecordFileDFAttributesAddr, (u8 *)&RecordFileDFAttributes, sizeof(RecordFileDFAttributesType));
	
	//--------以下新建立二进制属性文件--------
	RecordFileAttributes.FID[0] = FileIDH;//文件ID
	RecordFileAttributes.FID[1] = FileIDL;
	RecordFileAttributes.RecordMax = RecordNum;//记录条数
	RecordFileAttributes.RecordLenth = RecordLen;//每条记录长度
	RecordFileAttributes.StartAddr = (SectorAddr)*MF_SectorSize;//文件起始地址,BassAddr, 对应于Flash空间
	RecordFileAttributes.RecordIndex = 0;//已经记录的条数, 创建时为0
//	RecordFileAttributes.RemainAmount = RecordNum;//剩余条数,=FileLenth-OffsetAddr
	memcpy(RecordFileAttributes.FileCreadTime, SysCurrTime, 6);
	RecordFileAttributes.FLags = 1;///文件有效标志
	//写当前二进制文件的属性文件
	
	
	if((u16)AttrFileAddrTmp < 256)
	{//属性地址不能小于256
		return OtherErr;
	}
	EEPROM_WriteBytes(AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	
	//修改MF属性
	SetMFSectorFlag(SectorAddr, NeedSector);
	
	return 0x9000;//成功
}



/*********************************************************
函数描述: 	写二进制文件
入口参数: 	FileID		文件ID
			OffsetAddr	写入偏移地址
			BinDatLen	写入数据长度
			* DatBuf	数据文件内容
出口参数: 	
修改记录:

**********************************************************/
u8  bBuftmp[256];
s16	WriteBinFile(u8 FileIDH, u8 FileIDL, u16 OffsetAddr, u8 BinDatLen, u8 *DatBuf)
{
	s32 AttrFileAddrTmp = 0;
	
	
	memset(bBuftmp, 0, 256);
	
//=========1. 查找文件================================
	AttrFileAddrTmp = FindBinFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
	if(((s32)AttrFileAddrTmp == -1))
	{
		return NoFileIDErr;//文件不存在
	}
	
	EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&BinFileAttributes, sizeof(BinFileAttributesType));
	if((OffsetAddr + BinDatLen) > BinFileAttributes.FileLenth)
	{
		return NoSpaceErr;
	}
	if((OffsetAddr+BinDatLen) < MF_SectorSize)
	{//所写数据在同一页
		SPI_Flash_Read_Block(bBuftmp, BinFileAttributes.StartAddr, 256);//读取数据
		SPI_Flash_Erase_Sector(BinFileAttributes.StartAddr+1);
		memcpy((void *)&bBuftmp[OffsetAddr], DatBuf, BinDatLen);//修改数据
		SPI_FLASH_BufferWrite(bBuftmp, BinFileAttributes.StartAddr, 256);//重写数据
		
	}
	if((u16)AttrFileAddrTmp < 256)
	{//属性地址不能小于256
		return OtherErr;
	}
	EEPROM_WriteBytes((u16)AttrFileAddrTmp, (u8 *)&BinFileAttributes, sizeof(BinFileAttributesType));
	return CmdOK;
}

/*********************************************************
函数描述: 	读二进制文件
入口参数: 	FileID		文件ID
			OffsetAddr	读取偏移地址
			BinDatLen	读取数据长度
			* DatBuf	读取的数据文件内容
出口参数: 	0: 成功
			1: 空间不足
			2: 
			-1: 其他错误
修改记录:

**********************************************************/
s16	ReadBinFile(u8 FileIDH, u8 FileIDL, u16 OffsetAddr, u8 BinDatLen, u8 *DatBuf)
{
		s32 AttrFileAddrTmp = 0;
//	BinFileDFAttributesType BinFileDFAttributes;//二进制文件夹属性结构(总)
//	BinFileAttributesType BinFileAttributes;//二进制文件属性结构
	
//=========1. 查找文件================================
	AttrFileAddrTmp = FindBinFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
	if(((s32)AttrFileAddrTmp == -1))
	{
		return NoFileIDErr;//文件不存在
	}
	EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&BinFileAttributes, sizeof(BinFileAttributesType));

	if( ((OffsetAddr+BinDatLen)<=BinFileAttributes.FileLenth)) 
	{
		SPI_Flash_Read_Block(DatBuf, BinFileAttributes.StartAddr+OffsetAddr, BinDatLen);
		return CmdOK;
	}
	return FileLenErr;
}
/*********************************************************
函数描述: 	写记录文件
入口参数: 	FileID		文件ID
			Index		记录索引号
			* DatBuf	数据文件内容
出口参数: 
修改记录: 2012.2.2
		  IndexL 暂时用作记录长度

**********************************************************/
s16	WriteRecordFile(u8 FileIDH, u8 FileIDL, u8 IndexH, u8 IndexL, u8 *DatBuf)
{
	s32 AttrFileAddrTmp = 0;
	//u16 tCrc=0;
//	RecordFileDFAttributesType tRecordFileDFAttributes;//记录文件夹属性结构(总)
//	RecordFileAttributesType RecordFileAttributes;//记录文件属性结构
	
//=========1. 查找文件================================
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
	if(((s32)AttrFileAddrTmp == -1))
	{
		return NoFileIDErr;//文件不存在
	}
	EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	if(RecordFileAttributes.RecordIndex >= RecordFileAttributes.RecordMax)
	{
		return NoSpaceErr;
	}
	
//此处如何判断插入到中间某空间的数据是否满足地址要求?????????????????????????????????????????????????????????????????
	if( ((IndexH != 0xFF)) )
	{
		if(((IndexH<<8)+IndexL) >= RecordFileAttributes.RecordMax)
		{
			return RecordNumErr;
		}
		if(((IndexH<<8)+IndexL) < RecordFileAttributes.RecordIndex)
		{
			return RecordNumErr;
		}
		SPI_FLASH_BufferWrite(DatBuf, RecordFileAttributes.StartAddr+RecordFileAttributes.RecordLenth*((IndexH<<8)+IndexL), RecordFileAttributes.RecordLenth);
	}
	else
	{
		if((IndexL != RecordFileAttributes.RecordLenth) && (IndexL !=0xff))
		{
			return RecordLenErr;
		}
		SPI_FLASH_BufferWrite(DatBuf, RecordFileAttributes.StartAddr+ RecordFileAttributes.RecordLenth*RecordFileAttributes.RecordIndex, RecordFileAttributes.RecordLenth);
	}
	//SPI_Flash_Read_Block(DatBuf, 0 , 16);
	RecordFileAttributes.RecordIndex =  RecordFileAttributes.RecordIndex++ ;
	if((u16)AttrFileAddrTmp < 256)
	{//属性地址不能小于256
		return OtherErr;
	}
	EEPROM_WriteBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	return CmdOK;
}
/*********************************************************
函数描述: 	写记录文件
入口参数: 	FileID		文件ID
			Index		记录索引号
			* DatBuf	数据文件内容
出口参数: 
修改记录: 2012.2.2
		  IndexL 暂时用作记录长度

**********************************************************/
s16	WriteRecordFileN(u8 FileIDH, u8 FileIDL, u8 rNum, u8 rLen, u8 *DatBuf)
{
	s32 AttrFileAddrTmp = 0;
//	RecordFileDFAttributesType tRecordFileDFAttributes;//记录文件夹属性结构(总)
//	RecordFileAttributesType RecordFileAttributes;//记录文件属性结构
	
//=========1. 查找文件================================
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
	if(((s32)AttrFileAddrTmp == -1))
	{
		return NoFileIDErr;//文件不存在
	}
	EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	if((RecordFileAttributes.RecordIndex+rNum-1) >= RecordFileAttributes.RecordMax)
	{
		return NoSpaceErr;
	}
	
	if((rNum*RecordFileAttributes.RecordLenth) > 128)
	{
		return RecordLenErr;//记录长度错误
	}
	else
	{
		if(rLen < RecordFileAttributes.RecordLenth)
		{//写一条的时候用的,写多条时提示错误
			return RecordLenErr;//记录长度错误
		}
		SPI_FLASH_BufferWrite(DatBuf, RecordFileAttributes.StartAddr+ RecordFileAttributes.RecordLenth*RecordFileAttributes.RecordIndex, RecordFileAttributes.RecordLenth*rNum);
	}
	//SPI_Flash_Read_Block(DatBuf, 0 , 16);
	RecordFileAttributes.RecordIndex =  RecordFileAttributes.RecordIndex+rNum ;
	
	if((u16)AttrFileAddrTmp < 256)
	{//属性地址不能小于256
		return OtherErr;
	}
	EEPROM_WriteBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	return CmdOK;
}
/*********************************************************
函数描述: 	读记录文件
入口参数: 	FileID		文件ID
			Index		记录索引号
			* DatBuf	数据文件内容
出口参数: 	
修改记录:

**********************************************************/
s16	ReadRecordFile(u8 FileIDH, u8 FileIDL, u8 IndexH, u8 IndexL, u8 *DatBuf)
{
	s32 AttrFileAddrTmp = 0;
//	RecordFileDFAttributesType tRecordFileDFAttributes;//记录文件夹属性结构(总)
//	RecordFileAttributesType RecordFileAttributes;//记录文件属性结构
	
//=========1. 查找文件================================
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
	if(((s32)AttrFileAddrTmp == -1))
	{
		return NoFileIDErr;//文件不存在
	}
	EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	if((IndexH != 0xFF) && (IndexL != 0xFF))
	{
		if(((IndexH<<8)+IndexL) >= RecordFileAttributes.RecordIndex)
		{
			return RecordNumErr;
		}
		SPI_Flash_Read_Block(DatBuf, RecordFileAttributes.StartAddr+RecordFileAttributes.RecordLenth*(u16)((IndexH<<8)+IndexL), RecordFileAttributes.RecordLenth);
	}
	else
	{
		SPI_Flash_Read_Block(DatBuf, RecordFileAttributes.StartAddr+RecordFileAttributes.RecordLenth*(RecordFileAttributes.RecordIndex-1), RecordFileAttributes.RecordLenth);
	}
	return CmdOK;
}
/*********************************************************
函数描述: 	读记录文件
入口参数: 	FileID		文件ID
			Index		记录索引号
			* DatBuf	数据文件内容
出口参数: 	
修改记录:

**********************************************************/
s16	ReadRecordFileN(u8 FileIDH, u8 FileIDL, u8 IndexH, u8 IndexL, u8 rNum, u8 *DatBuf)
{
	s32 AttrFileAddrTmp = 0;
//	RecordFileDFAttributesType tRecordFileDFAttributes;//记录文件夹属性结构(总)
//	RecordFileAttributesType RecordFileAttributes;//记录文件属性结构
	
//=========1. 查找文件================================
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
	if(((s32)AttrFileAddrTmp == -1))
	{
		return NoFileIDErr;//文件不存在
	}
	EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	if((IndexH != 0xFF) && (IndexL != 0xFF))
	{//读多条记录
		if((((IndexH<<8)+IndexL)+rNum-1 )>= RecordFileAttributes.RecordIndex)
		{
			return RecordNumErr;
		}
		if((rNum * RecordFileAttributes.RecordLenth) >128)
		{
			return RecordLenErr;
		}
		SPI_Flash_Read_Block(DatBuf, RecordFileAttributes.StartAddr+RecordFileAttributes.RecordLenth*(u16)((IndexH<<8)+IndexL), RecordFileAttributes.RecordLenth*rNum);
	}
	else
	{//读最后一条记录
		SPI_Flash_Read_Block(DatBuf, RecordFileAttributes.StartAddr+RecordFileAttributes.RecordLenth*(RecordFileAttributes.RecordIndex-1), RecordFileAttributes.RecordLenth);
	}
	return CmdOK;
}
/*********************************************************
函数描述: 	查找记录文件
入口参数: 	FileID		文件ID
			Offset		便宜地址
			KeyLenth	关键字长度
			* DatBuf	关键字
出口参数: 	
修改记录:

**********************************************************/
s16	FindRecord(u8 FileIDH, u8 FileIDL, u8 Offset, u8 KeyLenth, u8 *DatBuf)
{
	s32 AttrFileAddrTmp = 0;
	u16 CurrIndex = 0;
	u8 kBuff[10];
	
	if(KeyLenth<=10)
	{
		memcpy(kBuff, DatBuf, KeyLenth);
	}
	
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
	if(((s32)AttrFileAddrTmp == -1))
	{
		return NoFileIDErr;//文件不存在
	}
	EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	for(CurrIndex=0; CurrIndex<RecordFileAttributes.RecordIndex; CurrIndex++)
	{//从第0条记录开始查找
		//读取当前记录内容,最长记录为64字节
		SPI_Flash_Read_Block((u8 *)&IrdaBuf[2], RecordFileAttributes.StartAddr+RecordFileAttributes.RecordLenth*(CurrIndex), RecordFileAttributes.RecordLenth);
		if(memcmp((u8 *)&IrdaBuf[2+Offset], kBuff, KeyLenth) == 0)
		{
			IrdaBuf[0]=CurrIndex & 0xff;
			IrdaBuf[1]=(CurrIndex>>8) & 0xff;
			return CmdOK;
		}
	}
	return NoOKRecord;//没有符合条件的记录
}
/*********************************************************
函数描述: 	读文件属性
入口参数: 	FileID		文件ID
出口参数: 	
修改记录:

**********************************************************/
s16 ReadFileInfo(u8 FileIDH, u8 FileIDL)
{
	s32 AttrFileAddrTmp = 0;
	
	//查找文件,是否为二进制文件
	AttrFileAddrTmp = FindBinFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
	if(((s32)AttrFileAddrTmp != -1))
	{//文件存在
		EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&BinFileAttributes, sizeof(BinFileAttributesType));
		stFileAttribute.FileType = 1;//二进制文件
		stFileAttribute.FileLen_RecordCount = BinFileAttributes.FileLenth;
		stFileAttribute.RecordLen = 0;
		memcpy(stFileAttribute.FileCreadTime, BinFileAttributes.FileCreadTime, 6);
		return CmdOK;
	}
	else
	{
		AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
		if(((s32)AttrFileAddrTmp != -1))
		{//文件存在
			EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
			stFileAttribute.FileType = 2;//记录文件
			stFileAttribute.FileLen_RecordCount = RecordFileAttributes.RecordIndex;
			stFileAttribute.RecordLen = RecordFileAttributes.RecordLenth;
			memcpy(stFileAttribute.FileCreadTime, RecordFileAttributes.FileCreadTime, 6);
			return CmdOK;
		}
	}
	stFileAttribute.FileType = 0;//无
	stFileAttribute.FileLen_RecordCount = 0;
	stFileAttribute.RecordLen = 0;
	memset(stFileAttribute.FileCreadTime, 0, 6);
	return NoFileIDErr;//文件不存在
}
/*********************************************************
函数描述: 	读记录文件
入口参数: 	FileID		文件ID
			Index		记录索引号
			* DatBuf	数据文件内容
出口参数: 	
修改记录:

**********************************************************/
extern u32 FLASH_PagesMask(u32 Size);
s16	DeleRecord(u8 FileIDH, u8 FileIDL, u8 IndexH, u8 IndexL)
{
	s32 AttrFileAddrTmp = 0;
	u8  rBuftmp[4096];
	u8  i, j;
	u16 k;
	memset(rBuftmp, 0, 4096);
	
	//=========1. 查找文件================================
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
	if(((s32)AttrFileAddrTmp == -1))
	{
		return NoFileIDErr;//文件不存在
	}
	//读属性文件
	EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	if(((IndexH<<8)+IndexL) >= RecordFileAttributes.RecordIndex)
	{
		return NoOKRecord;
	}
	if( ((IndexH != 0xFF) && (IndexL != 0xFF)))
	{//先读出数据,整理后在写入,最后修改属性文件
		if(((IndexH<<8)+IndexL) >= RecordFileAttributes.RecordMax)
		{
			return RecordNumErr;
		}
		if( ((IndexH == 0) && (IndexL == 0)))
		{
			SPI_Flash_Read_Block(rBuftmp, RecordFileAttributes.StartAddr+RecordFileAttributes.RecordLenth, 4096-RecordFileAttributes.RecordLenth);
		}
		else
		{
			SPI_Flash_Read_Block(rBuftmp, RecordFileAttributes.StartAddr, ((IndexH<<8)+IndexL)*RecordFileAttributes.RecordLenth);
			SPI_Flash_Read_Block((u8 *)&rBuftmp[((IndexH<<8)+IndexL)*RecordFileAttributes.RecordLenth], RecordFileAttributes.StartAddr+((IndexH<<8)+IndexL+1)*RecordFileAttributes.RecordLenth, 4096-((IndexH<<8)+IndexL+1)*RecordFileAttributes.RecordLenth);
		}
		//擦除扇区
		SPI_Flash_Erase_Sector(RecordFileAttributes.StartAddr);
		//回写调整后记录
		j=(RecordFileAttributes.RecordIndex-1)*RecordFileAttributes.RecordLenth / 512;
		k=(RecordFileAttributes.RecordIndex-1)*RecordFileAttributes.RecordLenth % 512;
		for(i=0; i<j; i++)
		{
			SPI_FLASH_BufferWrite(rBuftmp, RecordFileAttributes.StartAddr+512*i, 512);
		}
		SPI_FLASH_BufferWrite(rBuftmp, RecordFileAttributes.StartAddr+512*j, k);
		
	}
	else
	{//删除最后一条
		//读取所有记录(除最后一条)
		SPI_Flash_Read_Block(rBuftmp, RecordFileAttributes.StartAddr, (RecordFileAttributes.RecordIndex-1)*RecordFileAttributes.RecordLenth);
		//擦除扇区
		SPI_Flash_Erase_Sector(RecordFileAttributes.StartAddr);
		//回写调整后记录
		j=(RecordFileAttributes.RecordIndex-1)*RecordFileAttributes.RecordLenth / 512;
		k=(RecordFileAttributes.RecordIndex-1)*RecordFileAttributes.RecordLenth % 512;
		for(i=0; i<j; i++)
		{
			SPI_FLASH_BufferWrite(rBuftmp, RecordFileAttributes.StartAddr+512*i, 512);
		}
		SPI_FLASH_BufferWrite(rBuftmp, RecordFileAttributes.StartAddr+512*j, k);
	}
	RecordFileAttributes.RecordIndex =  RecordFileAttributes.RecordIndex-- ;//删除一条记录
	EEPROM_WriteBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	return CmdOK;
}
/*********************************************************
函数描述: 	格式化系统,包括片内EEPROM和片外FLASH
入口参数: 	
出口参数: 
修改记录:

**********************************************************/
void FormatSystem(void)
{
	EraseAllEeprom();//擦除EEPROM
	IWDG_ReloadCounter();
	SPI_Flash_Erase_Chip();//擦除片外FLASH
//	创建初始文件并初始化-----------
}


/*********************************************************
函数描述: 	写Key程序存储区信息,自身信息
入口参数: 	FileID		文件ID
			OffsetAddr	写入偏移地址
			BinDatLen	写入数据长度
			* DatBuf	数据文件内容
出口参数: 	0: 成功
			1: 文件不存在
			2: 
			-1: 其他错误
修改记录:

**********************************************************/
s16	WriteKeySelfFile(u8 FileIDH, u8 FileIDL, u16 OffsetAddr, u8 BinDatLen, u8 *DatBuf)
{
	s32 AttrFileAddrTmp = 0;
	
	//=========1. 查找文件================================
	if(FileIDH != 0xff)
	{
		AttrFileAddrTmp = FindBinFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
		if(((s32)AttrFileAddrTmp == -1))
		{
			return NoFileIDErr;//文件不存在
		}
		if(AttrFileAddrTmp < 256)
		{//属性地址不能小于256
			return OtherErr;
		}
	}
	if(FileIDH == 0)
	{
		if(FileIDL == 0x01)
		{
			if(BinDatLen > (sizeof(CompanyInfoType)+1))
			{
				return RecordLenErr;
			}
			EEPROM_WriteBytes((u16)CompanyInfoAddr, DatBuf, (sizeof(CompanyInfoType)+1));
		}
		else if(FileIDL == 0x02)
		{
			if(BinDatLen > sizeof(KeyBaseInfoType))
			{
				return RecordLenErr;
			}
			EEPROM_WriteBytes((u16)KeyBaseInfoAddr, DatBuf, sizeof(KeyBaseInfoType));
		}
		else if(FileIDL == 0x03)
		{
			if(BinDatLen > sizeof(KDFInfoType))
			{
				return RecordLenErr;
			}
			EEPROM_WriteBytes((u16)KDFInfoAddr, DatBuf, sizeof(KDFInfoType));
		}
	}
	return 0x9000;
}

/*********************************************************
函数描述: 	写Key程序存储区信息,自身信息
入口参数: 	FileID		文件ID
			OffsetAddr	写入偏移地址
			BinDatLen	写入数据长度
			* DatBuf	数据文件内容
出口参数: 	0: 成功
			1: 文件不存在
			2: 
			-1: 其他错误
修改记录:

**********************************************************/
s16	ReadKeySelfFile(u8 FileIDH, u8 FileIDL, u16 OffsetAddr, u8 BinDatLen, u8 *DatBuf)
{
	u16 tCrc;
	s32 AttrFileAddrTmp = 0;
	
//=========1. 查找文件================================
	if(FileIDH != 0xff)
	{
		AttrFileAddrTmp = FindBinFile(FileIDH, FileIDL);//查找属性文件,返回对应地址
		if(((s32)AttrFileAddrTmp == -1))
		{
			return NoFileIDErr;//文件不存在
		}
	}

	if(FileIDH == 0)
	{//读取指定文件
		if(FileIDL == 0x01)
		{
			EEPROM_ReadBytes((u16)CompanyInfoAddr, DatBuf, BinDatLen);
		}
		else if(FileIDL == 0x02)
		{
			EEPROM_ReadBytes((u16)KeyBaseInfoAddr, DatBuf, BinDatLen);
		}
		else if(FileIDL == 0x03)
		{
			if(KeyBaseInfo.KeyType == 0x02)
			{
				EEPROM_ReadBytes((u16)KDFInfoAddr, DatBuf, BinDatLen);
			}
			else
			{
				return WriteOnlyErr;
			}
		}
		else
		{
			return OtherErr;
		}
	}
	else if((FileIDH == 0xFF))
	{//读取全部文件, 读取后需要解密,然后再存入结构体
		EEPROM_ReadBytes((u16)KeyBaseInfoAddr, (u8 *)&IrdaBuf[0], sizeof(KeyBaseInfoType));
		//获取得到固定密钥
		Get_Key_Aes_Data(0);
		Data_AES(DECRYPT, (u8 *)&IrdaBuf[0], sizeof(KeyBaseInfo));//AES解密
	
		tCrc = QuickCRC16((u8 *)&IrdaBuf[0], sizeof(KeyBaseInfo)-2);
		if((IrdaBuf[sizeof(KeyBaseInfo)-2]!=(u8)(tCrc&0xff)) || (IrdaBuf[sizeof(KeyBaseInfo)-1]!=(u8)((tCrc>>8)&0xff)))	
		{//KDF CRC错误
			return CrcErr;
		}
		memcpy((u8 *)&KeyBaseInfo.Version, (u8 *)&IrdaBuf[0], sizeof(KeyBaseInfo)); 
		
		EEPROM_ReadBytes((u16)KDFInfoAddr, (u8 *)&IrdaBuf[0], sizeof(KDFInfoType));
		Get_Key_Aes_Data(0);
		Data_AES(DECRYPT, (u8 *)&IrdaBuf[0], sizeof(KDFInfo));//AES解密
	
		tCrc = QuickCRC16((u8 *)&IrdaBuf[0], sizeof(KDFInfo)-2);
		if((IrdaBuf[sizeof(KDFInfo)-2]!=(u8)(tCrc&0xff)) || (IrdaBuf[sizeof(KDFInfo)-1]!=(u8)((tCrc>>8)&0xff)))	
		{//KDF CRC错误
			return CrcErr;
		}
		memcpy((u8 *)&KDFInfo.Version, (u8 *)&IrdaBuf[0], sizeof(KDFInfo)); 
	}
	
	return CmdOK;
}
/*********************************************************
函数描述: 	删除文件
入口参数: 	
修改记录:

**********************************************************/
s16	DeleteSelfFile(u8 FileIDH, u8 FileIDL)
{
	s16 ret;
	CompanyInfoType CompanyInfo;
	
	if(FileIDH != 0xff)
	{
		ret = DeleteBinFile(FileIDH,FileIDL);
		if(ret != CmdOK )
		{
			return ret;
		}
	}
	
	if(FileIDH == 0)
	{
		if(FileIDL == 0x01)
		{
			memset((u8 *)&CompanyInfo, 0, sizeof(CompanyInfoType));
			EEPROM_WriteBytes((u16)CompanyInfoAddr, (u8 *)&CompanyInfo, sizeof(CompanyInfoType));
		}
		else if(FileIDL == 0x02)
		{
			memset((u8 *)&KeyBaseInfo, 0, sizeof(KeyBaseInfoType));
			EEPROM_WriteBytes((u16)KeyBaseInfoAddr, (u8 *)&KeyBaseInfo, sizeof(KeyBaseInfoType));
		}
		else if(FileIDL == 0x03)
		{
			memset((u8 *)&KDFInfo, 0, sizeof(KDFInfoType));
			EEPROM_WriteBytes((u16)KDFInfoAddr, (u8 *)&KDFInfo, sizeof(KDFInfoType));
		}
		else
		{
			return -1;
		}
	}
	return CmdOK;//成功
}

/*******************************************************************************
* Function Name  : Get_Key_Aes_Data
* Description    : 获取加解密用的密钥
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define 	KeyFixed		0//未被初始化状态,使用固定密钥
#define		KeyAuth			1//认证用初始密钥(固定)
#define		KeyData			2//数据密钥: 游戏厅编号4+KeyID4+发卡密码4+密钥因子2+CRC2
extern u8 const Key_Aes_KDF[16];
void Get_Key_Aes_Data(u8 AESInitStatus)
{
	u16 crc2;
	
	if(AESInitStatus == KeyFixed)//0
	{
		memcpy( Key_Aes_Data, Key_Aes_KDF, 16);//固定密钥
	}
	else if(AESInitStatus == KeyAuth)//1
	{
		//memcpy( Key_Aes_Data, Key_Aes_KDF, 16);
	}
	else if(AESInitStatus == KeyData)//2 工作密钥
	{
		Key_Aes_Data[0]	=	KDFInfo.VenueCode[0];
		Key_Aes_Data[1]	=	KDFInfo.VenueCode[1];
		Key_Aes_Data[2]	=	KDFInfo.VenueCode[2];
		Key_Aes_Data[3]	=	KDFInfo.VenueCode[3];  

		Key_Aes_Data[4]	=	KeyBaseInfo.KeyId[0];
		Key_Aes_Data[5]	=	KeyBaseInfo.KeyId[1];
		Key_Aes_Data[6]	=	KeyBaseInfo.KeyId[2];
		Key_Aes_Data[7]	=	KeyBaseInfo.KeyId[3];

		Key_Aes_Data[8]	=	KDFInfo.InitPassWD[0];
		Key_Aes_Data[9]	=	KDFInfo.InitPassWD[1];
		Key_Aes_Data[10]=	KDFInfo.InitPassWD[2];
		Key_Aes_Data[11]=	KDFInfo.InitPassWD[3];

		Key_Aes_Data[12]	=	KDFInfo.DataPassWD[0];
		Key_Aes_Data[13]	=	KDFInfo.DataPassWD[1];
		
		crc2	=	QuickCRC16(Key_Aes_Data,14);
		Key_Aes_Data[14]	=	crc2&0xff;
		Key_Aes_Data[15]	=	(crc2>>8)&0xff;	
	}
} 











