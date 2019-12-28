#define MYFILE

/*********************************************************
*************************˵��*****************************

    �����ļ��������ļ��������EEPROM��, �ļ����ݴ����FLASH
��,Ҫ�����ļ��Ĵ���,ɾ��,����,д��,��ȡ�Ȳ���,��Ҫ�ȴ��ļ���
���ж�ȡ������Ϣ,�ڽ��в���,������ɺ��޸Ķ��ڵ�������Ϣ.
	���˲��������ļ��������ļ�,���ļ�(MF),Ŀ¼�ļ�(*DF)ͨ��
��Ҫ����Ӧ���޸�,���������ļ����ļ����ݵĶ�Ӧ.
**********************************************************/
#include "include.h"
#include "AES.h"


//Ƭ��EEPROM����,��СΪ4K�ֽ�
#define EEPROM_BASE_ADDR    0x08080000    
#define EEPROM_BYTE_SIZE    0x0FFF  

//key������Ϣ 16�ֽ�
__packed typedef struct
{
	uint8_t Version;//�汾1
	uint8_t KeyType;//�ƿ�����
	uint8_t Prdate[6];//�Ǽ�ʱ��,������ʱ�����һ���ֽ�
	uint8_t KeyId[4];//����
	uint8_t Reserve[2];//����	
	uint8_t uCRC[2];//CRC
}KeyBaseInfoType;
__MYFILE KeyBaseInfoType KeyBaseInfo;

//KDF��Ϣ(Կ�׿�) 48�ֽ�
__packed typedef struct
{
	uint8_t Version;//�汾1
	uint8_t keyid[4];//KeyId
	uint8_t VenueCode[4];//���ر���
	uint8_t VenueName[20];//��������
	uint8_t VenueDate[6];//����ע������, ������ʱ�����һ���ֽ�	
	uint8_t InitPassWD[4];//��������; 
	uint8_t DataPassWD[2];//������Կ����
	uint8_t Reserve[5];//����	
	uint8_t uCRC[2];//CRC
}KDFInfoType;
__MYFILE KDFInfoType KDFInfo;

extern u8 AESInitStatus;
extern FLASH_Status DATA_EEPROM_ProgramByte(uint32_t Address, u8 Data);
/*********************************************************
��������: 	��ȡƬ��EEPROM�е�����
��ڲ���: 	
���ڲ���: 	
�޸ļ�¼:

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
��������: 	������д�뵽EEPROM, ��д������н����ж�,����
			�ڳ��������,��Ҫд���ݵ�����Ӧ�õȴ�һ������
			д����ٽ�����һ�β���.
��ڲ���: 	
���ڲ���: 	
�޸ļ�¼:

**********************************************************/
void EEPROM_WriteBytes(u16 Addr, u8 *Buffer, u16 Length)  
{ 
	uint32_t wAddr; 

	//Length=Length/4;
	
    wAddr=(EEPROM_BASE_ADDR+Addr);  
	__disable_irq();	
    DATA_EEPROM_Unlock(); //����
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
��������: 	���� EEPROM
��ڲ���: 	
���ڲ���: 
�޸ļ�¼:

**********************************************************/
void EraseAllEeprom(void)
{
	uint32_t wAddr, Length, tLen;
	
	Length=(4096-256)/4; //4096�ֽ�=1024��  

	tLen = 512;
    wAddr=(EEPROM_BASE_ADDR+16);  //ǰ256�ֽڲ���ʽ��?
	__disable_irq();	
    DATA_EEPROM_Unlock(); //����
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
    DATA_EEPROM_Unlock(); //����
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
��������: 	��ȡMF�ṹ,Flashʹ�����, �����EEPROM��
��ڲ���: 	
���ڲ���: 	
�޸ļ�¼:

**********************************************************/
s8	ReadMFFlag(void)
{
	//���ڲ�EEPROM�ж�ȡMF������ʹ�����
	EEPROM_ReadBytes(MFFlagAddr, (u8 *)&MFFlag.SectorFlag, sizeof(MFFlag));
	
	return 0;
}
/*********************************************************
��������: 	��ȡMF�ṹ,Flashʹ�����, �����EEPROM��
��ڲ���: 	
���ڲ���: 	
�޸ļ�¼:

**********************************************************/
s8	WriteMFFlag(void)
{	
	//���ڲ�EEPROM�ж�ȡMF������ʹ�����
	EEPROM_WriteBytes(MFFlagAddr, (u8 *)&MFFlag.SectorFlag, sizeof(MFFlag));
	
	return 0;
}

/*********************************************************
��������: 	����ʣ��������
��ڲ���: 	
���ڲ���: 	Sector: ʣ���������, 0-511
�޸ļ�¼:

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
��������: 	�޸�����ʹ�ñ�־
��ڲ���: 	SectorCnt: ��Ҫʹ�õ�����������
���ڲ���: 	SectorNum: 0-511 ����Ҫ���,ʣ�����������ʼ��ַ
					   -1:   ������Ҫ�����������
�޸ļ�¼:

**********************************************************/
void SetMFSectorFlag(u16 SectorNum, u8 SectorCnt)
{
	u8 i=0, j=0, m=0, n=0, k=0;
	
	//���㿪ʼλ��
	m = SectorNum/8;
	n = SectorNum%8;
	//�������˼��������ֽڵı�־
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
	
	
	
	WriteMFFlag();//д��EEPROM
}
//��ն�Ӧ������־
void ClearMFSectorFlag(u16 SectorNum, u8 SectorCnt)
{
	u8 i=0, j=0, m=0, n=0, k=0;
	
	//���㿪ʼλ��
	m = SectorNum/8;
	n = SectorNum%8;
	//�������˼��������ֽڵı�־
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
	WriteMFFlag();//д��EEPROM
}
/*********************************************************
��������: 	����ʣ��������
��ڲ���: 	SectorCnt: ��Ҫʹ�õ�����������
���ڲ���: 	SectorNum: 0-511 ����Ҫ���,ʣ�����������ʼ��ַ
					   -1:   ������Ҫ�����������
�޸ļ�¼:

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
��������: 	�����ļ�(FID)
��ڲ���: 	FileID
���ڲ���: 	AttrFileAddrTmp: ��Ч��ַ 
			-1: 	�޸��ļ�
			-2:		���ļ��Ѿ�ɾ��
�޸ļ�¼:

**********************************************************/
s32	FindBinFile(u8 FileIDH, u8 FileIDL)
{
	u8 i;
	s32 AttrFileAddrTmp = 0;
	
	//��ȡ�����Լ�¼�ļ�,��Ҫʹ�����ļ���,��ʼ��ַ
	EEPROM_ReadBytes(BinFileDFAttributesAddr, (u8 *)&BinFileDFAttributes, sizeof(BinFileDFAttributesType));
	
	//�����ļ�ID
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
	return -1;//�޶�Ӧ�ļ�
}
/*********************************************************
��������: 	�����ļ�(FID)
��ڲ���: 	FileID
���ڲ���: 	AttrFileAddrTmp: ��Ч��ַ 
			-1: 	�޸��ļ�
			-2:		���ļ��Ѿ�ɾ��
�޸ļ�¼:

**********************************************************/
s32	FindRecordFile(u8 FileIDH, u8 FileIDL)
{
	u8 i;
	s32 ret = -1, AttrFileAddrTmp = 0;

	
	//��ȡ�����Լ�¼�ļ�,��Ҫʹ�����ļ���,��ʼ��ַ
	EEPROM_ReadBytes(RecordFileDFAttributesAddr, (u8 *)&RecordFileDFAttributes, sizeof(RecordFileDFAttributesType));
	//�����ļ�ID
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
	return ret;//�޶�Ӧ�ļ�
}

/*********************************************************
��������: 	�����ļ�(FID)
��ڲ���: 	FileID
���ڲ���: 	AttrFileAddrTmp: ��Ч��ַ 
			-1: 	�޸��ļ�
			-2:		���ļ��Ѿ�ɾ��
�޸ļ�¼:

**********************************************************/
s32	FindDelRecordFile(u8 FileIDH, u8 FileIDL)
{
	u8 i;
	s32 ret = -1, AttrFileAddrTmp = 0;

	
	//��ȡ�����Լ�¼�ļ�,��Ҫʹ�����ļ���,��ʼ��ַ
	EEPROM_ReadBytes(RecordFileDFAttributesAddr, (u8 *)&RecordFileDFAttributes, sizeof(RecordFileDFAttributesType));
	//�����ļ�ID
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
	return ret;//�޶�Ӧ�ļ�
}

/*********************************************************
��������: 	ɾ���ļ�
��ڲ���: 	FileID
���ڲ���: 	0: �ɹ�
			1: FID������
			2: 
			3: 
			-1: ��������
�޸ļ�¼:

**********************************************************/
s16	DeleteBinFile(u8 FileIDH, u8 FileIDL)
{
//	u16 RSector=0, NeedSector=0;
//	s16 SectorAddr=-1;
	s32 AttrFileAddrTmp = 0;
//	BinFileDFAttributesType BinFileDFAttributes;//�������ļ������Խṹ(��)
//	BinFileAttributesType BinFileAttributes;//�������ļ����Խṹ
	
//=========1. �����ļ�================================
	AttrFileAddrTmp = FindBinFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
	if((AttrFileAddrTmp == -1))
	{
		return NoFileIDErr;//�ļ�������
	}
	
//=========2. ȡ���ļ���Ч��־========================
	//--------�����½��������������ļ�--------
	EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&BinFileAttributes, sizeof(BinFileAttributesType));
	if(BinFileAttributes.FLags == 1)
	{
		BinFileAttributes.FLags = 0;//�ļ���Ч��־,0:��Ч
		BinFileAttributes.FID[0] = 0;//�ļ�ID
		BinFileAttributes.FID[1] = 0;
		//д��ǰ�������ļ��������ļ�
		EEPROM_WriteBytes(AttrFileAddrTmp, (u8 *)&BinFileAttributes, sizeof(BinFileAttributesType));
		
		//ȡ����ӦFlash��ʹ�ñ�־
	}
	else
	{
		return OtherErr;
	}
	
//=========3. �޸��������ļ�==========================
	//��ȡ�����Լ�¼�ļ�
	EEPROM_ReadBytes(BinFileDFAttributesAddr, (u8 *)&BinFileDFAttributes, sizeof(BinFileDFAttributesType));
	//--------�����޸Ķ������ܼ�¼�����ļ�--------
	if( BinFileDFAttributes.TotalAttrFile !=0 )
	{
		BinFileDFAttributes.TotalAttrFile = BinFileDFAttributes.TotalAttrFile - 1;//��Ч�ļ�����-1
		//BinFileDFAttributes.DeleAttrFile = BinFileDFAttributes.DeleAttrFile + 1;//ɾ���ļ�����+1
		BinFileDFAttributes.DeleAttrFile = 0;//��ʹ��ɾ���ļ�����
		//�������ļ���¼�����ļ�
		EEPROM_WriteBytes(BinFileDFAttributesAddr, (u8 *)&BinFileDFAttributes, sizeof(BinFileDFAttributesType));
	}
	else
	{
		return OtherErr;
	}
	
	return CmdOK;//�ɹ�
}
/*********************************************************
��������: 	ɾ���ļ�
��ڲ���: 	FileID
���ڲ���: 	0: �ɹ�
			1: FID������
			2: 
			3: 
			-1: ��������
�޸ļ�¼:

**********************************************************/
s16	DeleteRecordFile(u8 FileIDH, u8 FileIDL)
{
//	u16 RSector=0, NeedSector=0;
//	s16 SectorAddr=-1;
	s32 AttrFileAddrTmp = 0;
//	RecordFileDFAttributesType RecordFileDFAttributes;//��¼�ļ������Խṹ(��)
//	RecordFileAttributesType RecordFileAttributes;//��¼�ļ����Խṹ
	
//=========1. �����ļ�================================
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
	if((AttrFileAddrTmp == -1)||(AttrFileAddrTmp == -2))
	{
		return NoFileIDErr;//�ļ�������
	}
	
//=========2. ȡ���ļ���Ч��־========================
	//--------�����½��������������ļ�--------
	EEPROM_ReadBytes((u32)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	if(RecordFileAttributes.FLags == 1)
	{
		RecordFileAttributes.FLags = 0;//�ļ���Ч��־,0:��Ч
		RecordFileAttributes.FID[0] = 0;//�ļ�ID
		RecordFileAttributes.FID[1] = 0;
		//д��ǰ�������ļ��������ļ�
		EEPROM_WriteBytes(AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
		//��ն�ӦFlash����ʹ�ñ�־
		ClearMFSectorFlag(RecordFileAttributes.StartAddr/MF_SectorSize, 1);
	}
	else
	{
		return NoOKRecord;
	}
	
//=========3. �޸��������ļ�==========================
	//��ȡ�����Լ�¼�ļ�
	EEPROM_ReadBytes(RecordFileDFAttributesAddr, (u8 *)&RecordFileDFAttributes, sizeof(RecordFileDFAttributesType));
	//--------�����޸Ķ������ܼ�¼�����ļ�--------
	if( RecordFileDFAttributes.TotalAttrFile !=0 )
	{
		RecordFileDFAttributes.TotalAttrFile = RecordFileDFAttributes.TotalAttrFile;//��Ч�ļ�����-1
		RecordFileDFAttributes.DeleAttrFile = RecordFileDFAttributes.DeleAttrFile + 1;//ɾ���ļ�����+1
		//�������ļ���¼�����ļ�
		EEPROM_WriteBytes(RecordFileDFAttributesAddr, (u8 *)&RecordFileDFAttributes, sizeof(RecordFileDFAttributesType));
	}
	else
	{
		return NoOKRecord;
	}
	
	return CmdOK;//�ɹ�
}
/*********************************************************
��������: 	��ռ�¼
��ڲ���: 	FileID
���ڲ���: 	
�޸ļ�¼:

**********************************************************/
s16 ClearRecord(u8 FileIDH, u8 FileIDL)
{
	s32 AttrFileAddrTmp = 0;
	u16 NeedSector=0; u32 FileLen=0;
	u8 i=0;
	
	//=========1. �����ļ�================================
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
	if((AttrFileAddrTmp == -1)||(AttrFileAddrTmp == -2))
	{
		return NoFileIDErr;//�ļ�������
	}
	
	//=========2. ȡ���ļ�д������========================
	EEPROM_ReadBytes((u32)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	if(RecordFileAttributes.FLags == 1)
	{
		//RecordFileAttributes.FLags = 0;//�ļ���Ч��־,0:��Ч
		RecordFileAttributes.RecordIndex = 0;//�Ѿ���¼��������0
		//д��ǰ�������ļ��������ļ�
		if((u16)AttrFileAddrTmp < 256)
		{//���Ե�ַ����С��256
			return OtherErr;
		}
		EEPROM_WriteBytes(AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
		
		//=========3. ���Flash========================
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
��������: 	ɾ���ļ�
��ڲ���: 	FileID
���ڲ���: 	
�޸ļ�¼:

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
			return ret;//�ļ�������
		}
	}
	return CmdOK;//�ɹ�
}


/*********************************************************
��������: 	�����������ļ�
��ڲ���: 	FileID, FileLen
���ڲ���: 	0: �ɹ�
			1: �ռ䲻��
			2: �ļ�ID����
			3: �ļ�������Ч
			-1: ��������
�޸ļ�¼:

**********************************************************/
extern u8 SysCurrTime[6];
s16	CreatBinFile(u8 FileIDH, u8 FileIDL, u16 FileLen)
{
	u16 RSector=0, NeedSector=0;
	s16 SectorAddr=-1;
	s32 AttrFileAddrTmp = 0;
	
	//�����ļ�
	AttrFileAddrTmp = FindBinFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
	if((AttrFileAddrTmp != -1))
	{
		return FileIDExistErr;//�ļ��Ѿ�����
	}
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
	if((AttrFileAddrTmp != -1))
	{
		return FileIDExistErr;//�ļ��Ѿ�����
	}
	
	//��������ռ�
	NeedSector=FileLen/MF_SectorSize;
	if((FileLen%MF_SectorSize)!=0)
	{
		NeedSector = NeedSector+1;
	}
	
	RSector = RemainSector();//��ȡʣ��������
	if(RSector < NeedSector)//////////////////////////////////////////�ռ䲻��ʱ,ɾ����Ч�ļ�,�ͷſռ�,��ʱδ���
	{//ʣ��ռ䲻��
		return FileLenErr;//������Ч
	}
	SectorAddr = FindSector(NeedSector);//���ҿ�������
	if(SectorAddr == -1)
	{//������Ҫ����������
		return NoSpaceErr;
	}
	SPI_Flash_Erase_Sector((SectorAddr)*MF_SectorSize);
	if(FileLen > 256)
	{//�ļ�������Ч
		return FileLenErr;//������Ч0x0003
	}
	//��ȡ�����Լ�¼�ļ�
	EEPROM_ReadBytes(BinFileDFAttributesAddr, (u8 *)&BinFileDFAttributes, sizeof(BinFileDFAttributesType));
	//--------�����޸Ķ������ܼ�¼�����ļ�--------
	if(BinFileDFAttributes.TotalAttrFile==0)
	{
		BinFileDFAttributes.FirstAttrFileAddr = BinFileBassAttributesAddr;//�ӵ�һ�ļ���ַ��ʼ���
		BinFileDFAttributes.LastAttrFileAddr = BinFileBassAttributesAddr;//��ǰ�ļ����ǵ�һ���ļ�,�������һ���ļ�
		BinFileDFAttributes.TotalAttrFile = 1;
		BinFileDFAttributes.AttrFileSize = sizeof(BinFileAttributesType);
	}
	else
	{
		//�޸ĵ�ǰ�ļ�Ϊ��󴴽��Ķ������ļ�
		BinFileDFAttributes.LastAttrFileAddr = BinFileDFAttributes.LastAttrFileAddr + sizeof(BinFileAttributesType);
		BinFileDFAttributes.TotalAttrFile++;
		if((BinFileDFAttributes.TotalAttrFile+BinFileDFAttributes.DeleAttrFile) > MaxBinFIDdCnt)
		{
			return NoSpaceErr;
		}
	}
	//�������ļ���¼�����ļ�
	EEPROM_WriteBytes(BinFileDFAttributesAddr, (u8 *)&BinFileDFAttributes, sizeof(BinFileDFAttributesType));
	
	//--------�����½��������������ļ�--------
	BinFileAttributes.FID[0] = FileIDH;//�ļ�ID
	BinFileAttributes.FID[1] = FileIDL;
	BinFileAttributes.FileLenth = FileLen;//�ļ��ܳ���
	BinFileAttributes.StartAddr = (SectorAddr)*MF_SectorSize;//�ļ���ʼ��ַ,BassAddr, ��Ӧ��Flash�ռ�
	BinFileAttributes.OffsetAddr = 0;//�ļ���ǰ��ַ,������ʹ�õ�ַ,����ʱδʹ��
	BinFileAttributes.RemainSpace = FileLen;//ʣ��ռ�,=FileLenth-OffsetAddr
	memcpy(BinFileAttributes.FileCreadTime, SysCurrTime, 6);
	BinFileAttributes.FLags = 1;//�ļ���Ч��־
	//д��ǰ�������ļ��������ļ�
	EEPROM_WriteBytes(BinFileDFAttributes.LastAttrFileAddr, (u8 *)&BinFileAttributes, sizeof(BinFileAttributesType));
	
	//�޸�MF����
	SetMFSectorFlag(SectorAddr, NeedSector);
	
	return CmdOK;//�ɹ�
}
s16	CreatBinFileEeprom(u8 FileIDH, u8 FileIDL, u16 FileLen)
{
	s32 AttrFileAddrTmp = 0;
	
	//�����ļ�
	AttrFileAddrTmp = FindBinFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
	if((AttrFileAddrTmp != -1))
	{
		return FileIDExistErr;//�ļ��Ѿ�����
	}
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
	if((AttrFileAddrTmp != -1))
	{
		return FileIDExistErr;//�ļ��Ѿ�����
	}
	
	if(FileLen > 256)
	{//�ļ�������Ч
		return FileLenErr;//������Ч
	}
	//��ȡ�����Լ�¼�ļ�
	EEPROM_ReadBytes(BinFileDFAttributesAddr, (u8 *)&BinFileDFAttributes, sizeof(BinFileDFAttributesType));
	//--------�����޸Ķ������ܼ�¼�����ļ�--------
	if(BinFileDFAttributes.TotalAttrFile==0)
	{
		BinFileDFAttributes.FirstAttrFileAddr = BinFileBassAttributesAddr;//�ӵ�һ�ļ���ַ��ʼ���
		BinFileDFAttributes.LastAttrFileAddr = BinFileBassAttributesAddr;//��ǰ�ļ����ǵ�һ���ļ�,�������һ���ļ�
		BinFileDFAttributes.TotalAttrFile = 1;
		BinFileDFAttributes.AttrFileSize = sizeof(BinFileAttributesType);
	}
	else
	{
		//�޸ĵ�ǰ�ļ�Ϊ��󴴽��Ķ������ļ�
		BinFileDFAttributes.LastAttrFileAddr = BinFileDFAttributes.LastAttrFileAddr + sizeof(BinFileAttributesType);
		BinFileDFAttributes.TotalAttrFile++;
		if((BinFileDFAttributes.TotalAttrFile+BinFileDFAttributes.DeleAttrFile) > MaxBinFIDdCnt)
		{
			return NoSpaceErr;
		}
	}
	//�������ļ���¼�����ļ�
	EEPROM_WriteBytes(BinFileDFAttributesAddr, (u8 *)&BinFileDFAttributes, sizeof(BinFileDFAttributesType));
	
	//--------�����½��������������ļ�--------
	BinFileAttributes.FID[0] = FileIDH;//�ļ�ID
	BinFileAttributes.FID[1] = FileIDL;
	BinFileAttributes.FileLenth = FileLen;//�ļ��ܳ���
	BinFileAttributes.StartAddr = 0;//�����EEPROM,����Flash
	BinFileAttributes.OffsetAddr = 0;//�ļ���ǰ��ַ,������ʹ�õ�ַ,����ʱδʹ��
	BinFileAttributes.RemainSpace = FileLen;//ʣ��ռ�,=FileLenth-OffsetAddr
	memcpy(BinFileAttributes.FileCreadTime, SysCurrTime, 6);
	BinFileAttributes.FLags = 1;//�ļ���Ч��־
	//д��ǰ�������ļ��������ļ�
	
	if(BinFileDFAttributes.LastAttrFileAddr < 256)
	{//���Ե�ַ����С��256
		return OtherErr;
	}
	EEPROM_WriteBytes(BinFileDFAttributes.LastAttrFileAddr, (u8 *)&BinFileAttributes, sizeof(BinFileAttributesType));
	
	
	return CmdOK;//�ɹ�
}

/*********************************************************
��������: 	����������¼�ļ�
��ڲ���: 	FileID		�ļ�ID
			RecordLen	������¼����
			RecordNum	��¼����
���ڲ���: 	0: �ɹ�
			1: �ռ䲻��
			2: 
			-1: ��������
�޸ļ�¼:

**********************************************************/
s16	CreatRecordFile(u8 FileIDH, u8 FileIDL, u16 RecordNum, u8 RecordLen)
{
	u16 RSector=0, NeedSector=0; u32 FileLen=0;
	s16 SectorAddr=-1;
	s32 AttrFileAddrTmp = 0;
	
	//�����ļ�
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
	if(((s32)AttrFileAddrTmp != -1))
	{
		return FileIDExistErr;//�ļ��Ѿ�����
	}
	AttrFileAddrTmp = FindBinFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
	if(((s32)AttrFileAddrTmp != -1))
	{
		return FileIDExistErr;//�ļ��Ѿ�����
	}
	
	if(RecordLen >= 255)
	{
		return RecordLenErr;//��¼������Ч
	}
	if(RecordNum >= 10000)
	{
		return RecordNumErr;//��¼������Ч
	}
	
	//��������ռ�
	FileLen=RecordLen*RecordNum;
	NeedSector=FileLen/MF_SectorSize;
	if((FileLen%MF_SectorSize)!=0)
	{
		NeedSector = NeedSector+1;
	}
	
	RSector = RemainSector();//��ȡʣ��������
	if(RSector < NeedSector)
	{//ʣ��ռ䲻��
		return NoSpaceErr;
	}
	SectorAddr = FindSector(NeedSector);//���ҿ�������
	if(SectorAddr == -1)
	{//������Ҫ����������
		return NoSpaceErr;
	}
	//��ʽ������
	//RecordFileAttributes.StartAddr = (SectorAddr)*MF_SectorSize;
	SPI_Flash_Erase_Sector((SectorAddr)*MF_SectorSize);
	
	if(FileLen == 0)//RecordLen*RecordNum
	{//�ļ�������Ч
		return RecordLenErr;
	}
	//��ȡ�����Լ�¼�ļ�
	EEPROM_ReadBytes(RecordFileDFAttributesAddr, (u8 *)&RecordFileDFAttributes, sizeof(RecordFileDFAttributesType));
	//--------�����޸Ķ������ܼ�¼�����ļ�--------
	if(RecordFileDFAttributes.TotalAttrFile==0)
	{
		RecordFileDFAttributes.FirstAttrFileAddr = RecordFileBassAttributesAddr;//�ӵ�һ�ļ���ַ��ʼ���
		RecordFileDFAttributes.LastAttrFileAddr = RecordFileBassAttributesAddr;//��ǰ�ļ����ǵ�һ���ļ�,�������һ���ļ�
		RecordFileDFAttributes.TotalAttrFile = 1;
		RecordFileDFAttributes.AttrFileSize = sizeof(RecordFileAttributesType);
		AttrFileAddrTmp = RecordFileDFAttributes.FirstAttrFileAddr;
	}
	else
	{
		//�޸ĵ�ǰ�ļ�Ϊ��󴴽��Ķ������ļ�
		if(RecordFileDFAttributes.DeleAttrFile != 0)//������Ѿ�ɾ���ļ�,��ʹ���Ѿ�ɾ���ļ�������λ��
		{
			//���ҵ�һ��ɾ���ļ���λ��
			//�����ļ�,���ļ�����Ŀ������
			AttrFileAddrTmp = FindDelRecordFile(0,0);
			if(((s32)AttrFileAddrTmp == -1))
			{
				return FileIDExistErr;//�ļ��Ѿ�����
			}
			RecordFileDFAttributes.DeleAttrFile--;
		}
		else
		{
			RecordFileDFAttributes.LastAttrFileAddr = RecordFileDFAttributes.LastAttrFileAddr + sizeof(RecordFileAttributesType);
			RecordFileDFAttributes.TotalAttrFile++;
			if((RecordFileDFAttributes.TotalAttrFile) > MaxRecordFIDCnt)//��������ļ���Ŀ
			{
				return NoSpaceErr;
			}
			AttrFileAddrTmp = RecordFileDFAttributes.LastAttrFileAddr;
		}
	}
	//�������ļ���¼�����ļ�
	EEPROM_WriteBytes(RecordFileDFAttributesAddr, (u8 *)&RecordFileDFAttributes, sizeof(RecordFileDFAttributesType));
	
	//--------�����½��������������ļ�--------
	RecordFileAttributes.FID[0] = FileIDH;//�ļ�ID
	RecordFileAttributes.FID[1] = FileIDL;
	RecordFileAttributes.RecordMax = RecordNum;//��¼����
	RecordFileAttributes.RecordLenth = RecordLen;//ÿ����¼����
	RecordFileAttributes.StartAddr = (SectorAddr)*MF_SectorSize;//�ļ���ʼ��ַ,BassAddr, ��Ӧ��Flash�ռ�
	RecordFileAttributes.RecordIndex = 0;//�Ѿ���¼������, ����ʱΪ0
//	RecordFileAttributes.RemainAmount = RecordNum;//ʣ������,=FileLenth-OffsetAddr
	memcpy(RecordFileAttributes.FileCreadTime, SysCurrTime, 6);
	RecordFileAttributes.FLags = 1;///�ļ���Ч��־
	//д��ǰ�������ļ��������ļ�
	
	
	if((u16)AttrFileAddrTmp < 256)
	{//���Ե�ַ����С��256
		return OtherErr;
	}
	EEPROM_WriteBytes(AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	
	//�޸�MF����
	SetMFSectorFlag(SectorAddr, NeedSector);
	
	return 0x9000;//�ɹ�
}



/*********************************************************
��������: 	д�������ļ�
��ڲ���: 	FileID		�ļ�ID
			OffsetAddr	д��ƫ�Ƶ�ַ
			BinDatLen	д�����ݳ���
			* DatBuf	�����ļ�����
���ڲ���: 	
�޸ļ�¼:

**********************************************************/
u8  bBuftmp[256];
s16	WriteBinFile(u8 FileIDH, u8 FileIDL, u16 OffsetAddr, u8 BinDatLen, u8 *DatBuf)
{
	s32 AttrFileAddrTmp = 0;
	
	
	memset(bBuftmp, 0, 256);
	
//=========1. �����ļ�================================
	AttrFileAddrTmp = FindBinFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
	if(((s32)AttrFileAddrTmp == -1))
	{
		return NoFileIDErr;//�ļ�������
	}
	
	EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&BinFileAttributes, sizeof(BinFileAttributesType));
	if((OffsetAddr + BinDatLen) > BinFileAttributes.FileLenth)
	{
		return NoSpaceErr;
	}
	if((OffsetAddr+BinDatLen) < MF_SectorSize)
	{//��д������ͬһҳ
		SPI_Flash_Read_Block(bBuftmp, BinFileAttributes.StartAddr, 256);//��ȡ����
		SPI_Flash_Erase_Sector(BinFileAttributes.StartAddr+1);
		memcpy((void *)&bBuftmp[OffsetAddr], DatBuf, BinDatLen);//�޸�����
		SPI_FLASH_BufferWrite(bBuftmp, BinFileAttributes.StartAddr, 256);//��д����
		
	}
	if((u16)AttrFileAddrTmp < 256)
	{//���Ե�ַ����С��256
		return OtherErr;
	}
	EEPROM_WriteBytes((u16)AttrFileAddrTmp, (u8 *)&BinFileAttributes, sizeof(BinFileAttributesType));
	return CmdOK;
}

/*********************************************************
��������: 	���������ļ�
��ڲ���: 	FileID		�ļ�ID
			OffsetAddr	��ȡƫ�Ƶ�ַ
			BinDatLen	��ȡ���ݳ���
			* DatBuf	��ȡ�������ļ�����
���ڲ���: 	0: �ɹ�
			1: �ռ䲻��
			2: 
			-1: ��������
�޸ļ�¼:

**********************************************************/
s16	ReadBinFile(u8 FileIDH, u8 FileIDL, u16 OffsetAddr, u8 BinDatLen, u8 *DatBuf)
{
		s32 AttrFileAddrTmp = 0;
//	BinFileDFAttributesType BinFileDFAttributes;//�������ļ������Խṹ(��)
//	BinFileAttributesType BinFileAttributes;//�������ļ����Խṹ
	
//=========1. �����ļ�================================
	AttrFileAddrTmp = FindBinFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
	if(((s32)AttrFileAddrTmp == -1))
	{
		return NoFileIDErr;//�ļ�������
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
��������: 	д��¼�ļ�
��ڲ���: 	FileID		�ļ�ID
			Index		��¼������
			* DatBuf	�����ļ�����
���ڲ���: 
�޸ļ�¼: 2012.2.2
		  IndexL ��ʱ������¼����

**********************************************************/
s16	WriteRecordFile(u8 FileIDH, u8 FileIDL, u8 IndexH, u8 IndexL, u8 *DatBuf)
{
	s32 AttrFileAddrTmp = 0;
	//u16 tCrc=0;
//	RecordFileDFAttributesType tRecordFileDFAttributes;//��¼�ļ������Խṹ(��)
//	RecordFileAttributesType RecordFileAttributes;//��¼�ļ����Խṹ
	
//=========1. �����ļ�================================
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
	if(((s32)AttrFileAddrTmp == -1))
	{
		return NoFileIDErr;//�ļ�������
	}
	EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	if(RecordFileAttributes.RecordIndex >= RecordFileAttributes.RecordMax)
	{
		return NoSpaceErr;
	}
	
//�˴�����жϲ��뵽�м�ĳ�ռ�������Ƿ������ַҪ��?????????????????????????????????????????????????????????????????
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
	{//���Ե�ַ����С��256
		return OtherErr;
	}
	EEPROM_WriteBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	return CmdOK;
}
/*********************************************************
��������: 	д��¼�ļ�
��ڲ���: 	FileID		�ļ�ID
			Index		��¼������
			* DatBuf	�����ļ�����
���ڲ���: 
�޸ļ�¼: 2012.2.2
		  IndexL ��ʱ������¼����

**********************************************************/
s16	WriteRecordFileN(u8 FileIDH, u8 FileIDL, u8 rNum, u8 rLen, u8 *DatBuf)
{
	s32 AttrFileAddrTmp = 0;
//	RecordFileDFAttributesType tRecordFileDFAttributes;//��¼�ļ������Խṹ(��)
//	RecordFileAttributesType RecordFileAttributes;//��¼�ļ����Խṹ
	
//=========1. �����ļ�================================
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
	if(((s32)AttrFileAddrTmp == -1))
	{
		return NoFileIDErr;//�ļ�������
	}
	EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	if((RecordFileAttributes.RecordIndex+rNum-1) >= RecordFileAttributes.RecordMax)
	{
		return NoSpaceErr;
	}
	
	if((rNum*RecordFileAttributes.RecordLenth) > 128)
	{
		return RecordLenErr;//��¼���ȴ���
	}
	else
	{
		if(rLen < RecordFileAttributes.RecordLenth)
		{//дһ����ʱ���õ�,д����ʱ��ʾ����
			return RecordLenErr;//��¼���ȴ���
		}
		SPI_FLASH_BufferWrite(DatBuf, RecordFileAttributes.StartAddr+ RecordFileAttributes.RecordLenth*RecordFileAttributes.RecordIndex, RecordFileAttributes.RecordLenth*rNum);
	}
	//SPI_Flash_Read_Block(DatBuf, 0 , 16);
	RecordFileAttributes.RecordIndex =  RecordFileAttributes.RecordIndex+rNum ;
	
	if((u16)AttrFileAddrTmp < 256)
	{//���Ե�ַ����С��256
		return OtherErr;
	}
	EEPROM_WriteBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	return CmdOK;
}
/*********************************************************
��������: 	����¼�ļ�
��ڲ���: 	FileID		�ļ�ID
			Index		��¼������
			* DatBuf	�����ļ�����
���ڲ���: 	
�޸ļ�¼:

**********************************************************/
s16	ReadRecordFile(u8 FileIDH, u8 FileIDL, u8 IndexH, u8 IndexL, u8 *DatBuf)
{
	s32 AttrFileAddrTmp = 0;
//	RecordFileDFAttributesType tRecordFileDFAttributes;//��¼�ļ������Խṹ(��)
//	RecordFileAttributesType RecordFileAttributes;//��¼�ļ����Խṹ
	
//=========1. �����ļ�================================
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
	if(((s32)AttrFileAddrTmp == -1))
	{
		return NoFileIDErr;//�ļ�������
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
��������: 	����¼�ļ�
��ڲ���: 	FileID		�ļ�ID
			Index		��¼������
			* DatBuf	�����ļ�����
���ڲ���: 	
�޸ļ�¼:

**********************************************************/
s16	ReadRecordFileN(u8 FileIDH, u8 FileIDL, u8 IndexH, u8 IndexL, u8 rNum, u8 *DatBuf)
{
	s32 AttrFileAddrTmp = 0;
//	RecordFileDFAttributesType tRecordFileDFAttributes;//��¼�ļ������Խṹ(��)
//	RecordFileAttributesType RecordFileAttributes;//��¼�ļ����Խṹ
	
//=========1. �����ļ�================================
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
	if(((s32)AttrFileAddrTmp == -1))
	{
		return NoFileIDErr;//�ļ�������
	}
	EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	if((IndexH != 0xFF) && (IndexL != 0xFF))
	{//��������¼
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
	{//�����һ����¼
		SPI_Flash_Read_Block(DatBuf, RecordFileAttributes.StartAddr+RecordFileAttributes.RecordLenth*(RecordFileAttributes.RecordIndex-1), RecordFileAttributes.RecordLenth);
	}
	return CmdOK;
}
/*********************************************************
��������: 	���Ҽ�¼�ļ�
��ڲ���: 	FileID		�ļ�ID
			Offset		���˵�ַ
			KeyLenth	�ؼ��ֳ���
			* DatBuf	�ؼ���
���ڲ���: 	
�޸ļ�¼:

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
	
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
	if(((s32)AttrFileAddrTmp == -1))
	{
		return NoFileIDErr;//�ļ�������
	}
	EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	for(CurrIndex=0; CurrIndex<RecordFileAttributes.RecordIndex; CurrIndex++)
	{//�ӵ�0����¼��ʼ����
		//��ȡ��ǰ��¼����,���¼Ϊ64�ֽ�
		SPI_Flash_Read_Block((u8 *)&IrdaBuf[2], RecordFileAttributes.StartAddr+RecordFileAttributes.RecordLenth*(CurrIndex), RecordFileAttributes.RecordLenth);
		if(memcmp((u8 *)&IrdaBuf[2+Offset], kBuff, KeyLenth) == 0)
		{
			IrdaBuf[0]=CurrIndex & 0xff;
			IrdaBuf[1]=(CurrIndex>>8) & 0xff;
			return CmdOK;
		}
	}
	return NoOKRecord;//û�з��������ļ�¼
}
/*********************************************************
��������: 	���ļ�����
��ڲ���: 	FileID		�ļ�ID
���ڲ���: 	
�޸ļ�¼:

**********************************************************/
s16 ReadFileInfo(u8 FileIDH, u8 FileIDL)
{
	s32 AttrFileAddrTmp = 0;
	
	//�����ļ�,�Ƿ�Ϊ�������ļ�
	AttrFileAddrTmp = FindBinFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
	if(((s32)AttrFileAddrTmp != -1))
	{//�ļ�����
		EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&BinFileAttributes, sizeof(BinFileAttributesType));
		stFileAttribute.FileType = 1;//�������ļ�
		stFileAttribute.FileLen_RecordCount = BinFileAttributes.FileLenth;
		stFileAttribute.RecordLen = 0;
		memcpy(stFileAttribute.FileCreadTime, BinFileAttributes.FileCreadTime, 6);
		return CmdOK;
	}
	else
	{
		AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
		if(((s32)AttrFileAddrTmp != -1))
		{//�ļ�����
			EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
			stFileAttribute.FileType = 2;//��¼�ļ�
			stFileAttribute.FileLen_RecordCount = RecordFileAttributes.RecordIndex;
			stFileAttribute.RecordLen = RecordFileAttributes.RecordLenth;
			memcpy(stFileAttribute.FileCreadTime, RecordFileAttributes.FileCreadTime, 6);
			return CmdOK;
		}
	}
	stFileAttribute.FileType = 0;//��
	stFileAttribute.FileLen_RecordCount = 0;
	stFileAttribute.RecordLen = 0;
	memset(stFileAttribute.FileCreadTime, 0, 6);
	return NoFileIDErr;//�ļ�������
}
/*********************************************************
��������: 	����¼�ļ�
��ڲ���: 	FileID		�ļ�ID
			Index		��¼������
			* DatBuf	�����ļ�����
���ڲ���: 	
�޸ļ�¼:

**********************************************************/
extern u32 FLASH_PagesMask(u32 Size);
s16	DeleRecord(u8 FileIDH, u8 FileIDL, u8 IndexH, u8 IndexL)
{
	s32 AttrFileAddrTmp = 0;
	u8  rBuftmp[4096];
	u8  i, j;
	u16 k;
	memset(rBuftmp, 0, 4096);
	
	//=========1. �����ļ�================================
	AttrFileAddrTmp = FindRecordFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
	if(((s32)AttrFileAddrTmp == -1))
	{
		return NoFileIDErr;//�ļ�������
	}
	//�������ļ�
	EEPROM_ReadBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	if(((IndexH<<8)+IndexL) >= RecordFileAttributes.RecordIndex)
	{
		return NoOKRecord;
	}
	if( ((IndexH != 0xFF) && (IndexL != 0xFF)))
	{//�ȶ�������,�������д��,����޸������ļ�
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
		//��������
		SPI_Flash_Erase_Sector(RecordFileAttributes.StartAddr);
		//��д�������¼
		j=(RecordFileAttributes.RecordIndex-1)*RecordFileAttributes.RecordLenth / 512;
		k=(RecordFileAttributes.RecordIndex-1)*RecordFileAttributes.RecordLenth % 512;
		for(i=0; i<j; i++)
		{
			SPI_FLASH_BufferWrite(rBuftmp, RecordFileAttributes.StartAddr+512*i, 512);
		}
		SPI_FLASH_BufferWrite(rBuftmp, RecordFileAttributes.StartAddr+512*j, k);
		
	}
	else
	{//ɾ�����һ��
		//��ȡ���м�¼(�����һ��)
		SPI_Flash_Read_Block(rBuftmp, RecordFileAttributes.StartAddr, (RecordFileAttributes.RecordIndex-1)*RecordFileAttributes.RecordLenth);
		//��������
		SPI_Flash_Erase_Sector(RecordFileAttributes.StartAddr);
		//��д�������¼
		j=(RecordFileAttributes.RecordIndex-1)*RecordFileAttributes.RecordLenth / 512;
		k=(RecordFileAttributes.RecordIndex-1)*RecordFileAttributes.RecordLenth % 512;
		for(i=0; i<j; i++)
		{
			SPI_FLASH_BufferWrite(rBuftmp, RecordFileAttributes.StartAddr+512*i, 512);
		}
		SPI_FLASH_BufferWrite(rBuftmp, RecordFileAttributes.StartAddr+512*j, k);
	}
	RecordFileAttributes.RecordIndex =  RecordFileAttributes.RecordIndex-- ;//ɾ��һ����¼
	EEPROM_WriteBytes((u16)AttrFileAddrTmp, (u8 *)&RecordFileAttributes, sizeof(RecordFileAttributesType));
	return CmdOK;
}
/*********************************************************
��������: 	��ʽ��ϵͳ,����Ƭ��EEPROM��Ƭ��FLASH
��ڲ���: 	
���ڲ���: 
�޸ļ�¼:

**********************************************************/
void FormatSystem(void)
{
	EraseAllEeprom();//����EEPROM
	IWDG_ReloadCounter();
	SPI_Flash_Erase_Chip();//����Ƭ��FLASH
//	������ʼ�ļ�����ʼ��-----------
}


/*********************************************************
��������: 	дKey����洢����Ϣ,������Ϣ
��ڲ���: 	FileID		�ļ�ID
			OffsetAddr	д��ƫ�Ƶ�ַ
			BinDatLen	д�����ݳ���
			* DatBuf	�����ļ�����
���ڲ���: 	0: �ɹ�
			1: �ļ�������
			2: 
			-1: ��������
�޸ļ�¼:

**********************************************************/
s16	WriteKeySelfFile(u8 FileIDH, u8 FileIDL, u16 OffsetAddr, u8 BinDatLen, u8 *DatBuf)
{
	s32 AttrFileAddrTmp = 0;
	
	//=========1. �����ļ�================================
	if(FileIDH != 0xff)
	{
		AttrFileAddrTmp = FindBinFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
		if(((s32)AttrFileAddrTmp == -1))
		{
			return NoFileIDErr;//�ļ�������
		}
		if(AttrFileAddrTmp < 256)
		{//���Ե�ַ����С��256
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
��������: 	дKey����洢����Ϣ,������Ϣ
��ڲ���: 	FileID		�ļ�ID
			OffsetAddr	д��ƫ�Ƶ�ַ
			BinDatLen	д�����ݳ���
			* DatBuf	�����ļ�����
���ڲ���: 	0: �ɹ�
			1: �ļ�������
			2: 
			-1: ��������
�޸ļ�¼:

**********************************************************/
s16	ReadKeySelfFile(u8 FileIDH, u8 FileIDL, u16 OffsetAddr, u8 BinDatLen, u8 *DatBuf)
{
	u16 tCrc;
	s32 AttrFileAddrTmp = 0;
	
//=========1. �����ļ�================================
	if(FileIDH != 0xff)
	{
		AttrFileAddrTmp = FindBinFile(FileIDH, FileIDL);//���������ļ�,���ض�Ӧ��ַ
		if(((s32)AttrFileAddrTmp == -1))
		{
			return NoFileIDErr;//�ļ�������
		}
	}

	if(FileIDH == 0)
	{//��ȡָ���ļ�
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
	{//��ȡȫ���ļ�, ��ȡ����Ҫ����,Ȼ���ٴ���ṹ��
		EEPROM_ReadBytes((u16)KeyBaseInfoAddr, (u8 *)&IrdaBuf[0], sizeof(KeyBaseInfoType));
		//��ȡ�õ��̶���Կ
		Get_Key_Aes_Data(0);
		Data_AES(DECRYPT, (u8 *)&IrdaBuf[0], sizeof(KeyBaseInfo));//AES����
	
		tCrc = QuickCRC16((u8 *)&IrdaBuf[0], sizeof(KeyBaseInfo)-2);
		if((IrdaBuf[sizeof(KeyBaseInfo)-2]!=(u8)(tCrc&0xff)) || (IrdaBuf[sizeof(KeyBaseInfo)-1]!=(u8)((tCrc>>8)&0xff)))	
		{//KDF CRC����
			return CrcErr;
		}
		memcpy((u8 *)&KeyBaseInfo.Version, (u8 *)&IrdaBuf[0], sizeof(KeyBaseInfo)); 
		
		EEPROM_ReadBytes((u16)KDFInfoAddr, (u8 *)&IrdaBuf[0], sizeof(KDFInfoType));
		Get_Key_Aes_Data(0);
		Data_AES(DECRYPT, (u8 *)&IrdaBuf[0], sizeof(KDFInfo));//AES����
	
		tCrc = QuickCRC16((u8 *)&IrdaBuf[0], sizeof(KDFInfo)-2);
		if((IrdaBuf[sizeof(KDFInfo)-2]!=(u8)(tCrc&0xff)) || (IrdaBuf[sizeof(KDFInfo)-1]!=(u8)((tCrc>>8)&0xff)))	
		{//KDF CRC����
			return CrcErr;
		}
		memcpy((u8 *)&KDFInfo.Version, (u8 *)&IrdaBuf[0], sizeof(KDFInfo)); 
	}
	
	return CmdOK;
}
/*********************************************************
��������: 	ɾ���ļ�
��ڲ���: 	
�޸ļ�¼:

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
	return CmdOK;//�ɹ�
}

/*******************************************************************************
* Function Name  : Get_Key_Aes_Data
* Description    : ��ȡ�ӽ����õ���Կ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define 	KeyFixed		0//δ����ʼ��״̬,ʹ�ù̶���Կ
#define		KeyAuth			1//��֤�ó�ʼ��Կ(�̶�)
#define		KeyData			2//������Կ: ��Ϸ�����4+KeyID4+��������4+��Կ����2+CRC2
extern u8 const Key_Aes_KDF[16];
void Get_Key_Aes_Data(u8 AESInitStatus)
{
	u16 crc2;
	
	if(AESInitStatus == KeyFixed)//0
	{
		memcpy( Key_Aes_Data, Key_Aes_KDF, 16);//�̶���Կ
	}
	else if(AESInitStatus == KeyAuth)//1
	{
		//memcpy( Key_Aes_Data, Key_Aes_KDF, 16);
	}
	else if(AESInitStatus == KeyData)//2 ������Կ
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











