/******************** (C) COPYRIGHT 2007 HuaYuan ********************
* File Name          : flash.h
* Author             : Han yongliang
* Date First Issued  : 05/11/2008
* Description        : Header for flash.c file.
********************************************************************************
* History:
* 05/11/2008: V0.1
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _FLASH_H
#define _FLASH_H

#define ApplicationAddress    0x8008000		//应用程序Flash的起始地址
#define USE_STM3210B_EVAL	1
#if !defined (USE_STM3210B_EVAL) &&  !defined (USE_STM3210E_EVAL)
 #define USE_STM3210B_EVAL
 //#define USE_STM3210E_EVAL
#endif

/* Define the STM32F10x hardware depending on the used evaluation board */
#ifdef USE_STM3210B_EVAL
  #define PAGE_SIZE                         (0x400)
  #define FLASH_SIZE                        (0x20000) /* 128K */
#elif defined USE_STM3210E_EVAL
  #define PAGE_SIZE                         (0x800)
  #define FLASH_SIZE                        (0x80000) /* 512K */
#endif /* USE_STM3210B_EVAL */

#define OK		0x00
#define FAIL	0x01
#define FLASH_OK	0
#define FLASH_ERROR	1


#define  AT26DF_FLASH_ID        0x1F440000
#define  EN25T80_FLASH_ID       0x1C13
#define  EN25T16_FLASH_ID       0x1C14

#define SPI_FLASH_PageSize      256
#define SPI_FLASH_SectorSize    0x1000			//扇区大小4K
#define HZK_TOTAL_PAGE			418				//汉字库占用页数 256*418= 104.5K

#define FLASH_HZK_START_ADDR	0x000000		//汉字库起始地址，共112K 
#define FLASH_HZK_END_ADDR		0x01CFFF		//汉字库结束地址，第28扇区结束

//一副整图包括10副小图，每副小图最多占用12K，否则想法解决
#define FLASH_PIC_MAX_NUM		10			//最多存储的图片数目
#define FLASH_BIG_PIC_SIZE		0x18000		//大图片尺寸,96K
#define FLASH_SMALL_PIC_SIZE	0x0C000		//小图片尺寸,48K
#define FLASH_Very_Small_PIC_SIZE	0x03000		//小图片尺寸,12K

#define FLASH_CARD_PARAM_ADDR	0x0C8000		//共4K,  第200扇区起始
#define FLASH_CONFIG_INFO_ADDR	0x0C9000		//共4K,  第201扇区起始

//升级程序占用的地址空间
#define FLASH_UPD_START_ADDR	0x0FD000		//升级文件存放起始地址,共256K,第141扇区起始,第204扇区结束
#define FLASH_UPD_END_ADDR		0x13CFFF		//升级文件存放结束地址,第204扇区
#define FLASH_UPD_FLAG_ADDR		0x13D000		//升级标志存储地址,第205扇区，占的空间有点多呀

#define MAX_PlayGame_RECORD_ITEMS	2000		//最大玩游戏记录数
#define MAX_JieZhang_RECORD_ITEMS	2000		//最大结账记录数
#define MAX_JiaoYi_RECORD_ITEMS		2000		//最大交易记录数
#define MAX_CardAccount_RECORD_ITEMS	2000		//最大会员账户记录数


//记录存放地址
#define FLASH_PLAY_GAME_RECORD_START_ADDR	0x13E000	//玩游戏记录地址第318扇区,按每条记录分配96字节，分配2000条记录，共192K
#define FLASH_PLAY_GAME_RECORD_END_ADDR		0x16DFFF	//玩游戏记录地址,第365扇区,按每条记录分配96字节，分配2000条记录，共192K

#define FLASH_JieZhang_RECORD_START_ADDR	0x16F000	//客户端结账记录,地址第367扇区，每条记录64字节，分配2000条,共128K
#define FLASH_JieZhang_RECORD_END_ADDR		0x18EFFF	//客户端结账记录,地址第398扇区，每条记录64字节，分配2000条

#define FLASH_JiaoYi_RECORD_START_ADDR		0x190000	//客户端交易记录,地址第400扇区，每条记录74字节，分配2000条，共144K
#define FLASH_JiaoYi_RECORD_END_ADDR		0x1B5FFF	//客户端交易记录,地址第433扇区，每条记录74字节，分配2000条，共144K

#define FLASH_CARD_ACCOUNT_RECORD_START_ADDR 0x1B6000	//客户端卡账户记录,地址第434扇区，每条记录100字节，分配2000条，共256K
#define FLASH_CARD_ACCOUNT_RECORD_END_ADDR   0x1F5FFF	//客户端卡账户记录,地址第498扇区，每条记录100字节，分配2000条，共256K

#define GUASHI_MAX_NUM_PER_PAGE				 50			//每页存储的最大数
#define FLASH_GuaShi_List_ADDR				 0x1F6000	//挂失列表地址

#define FLASH_Reserved_ADDR					 0x1F7000 	//第502扇区
#define FLASH_END_ADDR					 	 0x1FFFFF	//第511扇区，


#define UPDATE_FLAG_GO				0x01		//(应该)去升级标志
#define UPDATE_FLAG_FINISHED		0xAA55		//升级完成标志
#define FLASH_UPD_SEND_SIZE			48			//每包传输的字节数,应为4的整数倍，否则出错



#define APP_DEVICE_TYPE_MASK		0x00FFFFE0	//应用程序设备类型掩码
#define APP_DEVICE_TYPE_HEAD		0xEF0F0001	//设备类型
#define APP_DEVICE_TYPE_NUM			0x0000001F	//应用程序适用的设备类型个数
#define APP_CHIP_TYPE_MASK			0xFFFE0000	//应用程序适合的芯片型号掩码
#define APP_MIN_SIZE				0x2000		//应用程序最小尺寸8K
#define APP_MAX_SIZE				0x40000		//应用程序最大尺寸256K



//***************** SPI FlashROM command codes ******************************
// 25Xxx FlashROM description:
// SPI mode 0: SCK idle state = low, DIO sampled on rising edge, DO clocked on falling edge, MSB first
// 512kB memory, total 8 blocks per 64 kB, 1 block = 16 sectors per 4 kB, 1 sector = 16 pages per 256B
#define SPI_FLASH_WREN         0x06    // povol zapis
#define SPI_FLASH_WRDIS        0x04    // zakaz zapis
#define SPI_FLASH_RDSTAT       0x05    // cti status registr; S7:0 - bit7=stat reg protect, bit5=top/bot write protect, bit4:2=block protect bits, bit1=WE latch, bit0=busy
#define SPI_FLASH_WRSTAT       0x01    // zapis status registr; S7:0
#define SPI_FLASH_RDDATA       0x03    // cti data; A23:16, A15:A8, A7:0, D7:0, next DB till the end
#define SPI_FLASH_FASTRD       0x0B    // rychle cteni; A23:16, A15:A8, A7:0, dummyB, D7:0, next DB till the end
#define SPI_FLASH_FASTRD2      0x3B    // rychle cteni 2 linkami DO a DIO; A23:16, A15:A8, A7:0, dummyB, D7:0 bit interleaved, next DB till the end
#define SPI_FLASH_PAGEPGM      0x02    // programovani stranky; A23:16, A15:A8, A7:0, D7:0, next DB till the end
#define SPI_FLASH_BLKERASE     0xD8    // smazani bloku (64kB pro 25Xxx); A23:16, A15:A8, A7:0
#define SPI_FLASH_SECERASE     0x20    // smazani sektoru (4kB pro 25Xxx); A23:16, A15:A8, A7:0
#define SPI_FLASH_CHIPERASE    0xC7    // smazani cele FlashROM
#define SPI_FLASH_PWRDWN       0xB9    // uspani
#define SPI_FLASH_RELPWRDWN    0xAB    // 
#define SPI_FLASH_MIDDID       0x90    // ID vyrobce a zarizeni; dummyB, dummyB, 0, M7:0 (EFh=Winbond), ID7:0 (10h=X10, 11h=X20, 12h=X40, 13h=X80)
#define SPI_FLASH_JEDECID      0x9F    // JEDEC ID vyrobce; M7:0, ID15:8, ID7:0  (3011h=X10, 3012h=X20, 3013h=X40, 3014h=X80)

#define SPI_FLASH_JID_ATMEL    0x1F    // Atmel
#define SPI_FLASH_JID_STM      0x20    // ST Microelectronic
#define SPI_FLASH_JID_SST      0xBF    // SST (Silicon Storage Technology)
#define SPI_FLASH_JID_MACRONIX 0xC2    // Macronix
#define SPI_FLASH_JID_WINBOND  0xEF    // Winbond

#define SPI_FLASH_PAGE_SIZE    256     // velikost stranky, max. jednotka pro zapis
#define SPI_FLASH_SECTOR_SIZE  4096    // velikost sektoru, min. jednotka pro mazani
#define SPI_FLASH_BLOCK_SIZE   65536   // velikost bloku, vetsi jednotka pro mazani

/* Exported types ------------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
//系统配置信息结构
#define DEFAULT_SAVE_CARD_MAX_LIMIT_VALUE	40000	   //存卡上限值
typedef struct {
volatile		  char Flag;					//信息有效标志，不为0时有效
volatile unsigned char Game_Type[2];			//游戏类型
unsigned char Active_Valid_Minute[2];		//激活有效时间
unsigned char Card_Invalid_Second[2];		//离开失效里长
unsigned char Rsved[16];			//保留
volatile unsigned char mode;		 			//模式：投币钱包和接线模式
volatile unsigned char ShangFen_Value_Once[2];	//一次上分值
volatile unsigned char TouTui_Bi_Setting;		//投退币设置			
volatile unsigned char Dev_Num[2];				//机号
volatile unsigned char TouTui_Bi_SameTime_Flag;	//同时投退币标志
volatile unsigned  char  XiaFen_Value_Int;		//一次下分出币值整数部分
volatile unsigned  char  XiaFen_Value_Dot;		//一次下分出币值小数部分
unsigned char Card_Auth_mode;					//卡认证模式
unsigned char Save_Card_Limit_Value[2];	//存卡限止值,超过该值时需要账务卡
unsigned char New_User_Card_Valid_Flag;	//使新用户卡有效标志
unsigned char Save2SameCard_Flag;		//必须同张卡才可以“退币存卡”标志
unsigned char Check_Disturb_Flag;		//检查干扰标志
		 char Device_Status;			//设备状态：出厂状态或用户状态或未出厂状态 
//unsigned char check;					//校验和
}struct_Config_info;

//投退币设置信息结构
typedef struct {
		  char TouBi_Polor;				//投币极性
		  char TouBi_V;					//投币速度
		  char TuiBi_Polor;				//退币极性
		  char TuiBi_V;					//退币速度
		  char Support_ShiBi;			//是否支持退实币，为1时支持
		  char Line_mode;				//接线模式
		  char Tou_ShiBi_Normal_Level;	//投实币正常时的电平
		  char Tui_ShiBi_Normal_Level;	//退实币正常时的电平
		  char Disturb_Alarm;			//干扰报警
}struct_TouTui_Bi_Setting;

#define TOUBI_PULSE_POLOR_MASK		0x80	//投币脉冲极性掩码
#define TOUBI_V_MASK				0x70	//投币速度掩码
#define TUIBI_PULSE_POLOR_MASK		0x08	//退币脉冲极性掩码
#define TUIBI_V_MASK				0x07	//退币速度掩码

#define NORMAL_CLOSE				0	//常闭
#define NORMAL_OPEN					1	//常开


/* Exported functions ------------------------------------------------------- */
/*----- High layer function -----*/
void Flash_GPIO_Configuration(void);
void SPI_Flash_Read_Block(unsigned char * pBuffer,int start_address, int blk_size);
void SPI_FLASH_BlockWrite(unsigned char * pBuffer, unsigned int address, unsigned short blk_size);
void SPI_FLASH_BufferWrite(unsigned char * pBuffer, unsigned int address, unsigned short blk_size);
unsigned char SPI_Flash_Erase_Sector(int sector);
unsigned int SPI_FLASH_ReadID(void);
unsigned char User_Read_Config_Info(struct_Config_info *config_info);
extern void Delay (int cnt);

extern unsigned char User_Read_Update_Data(u16 addr,u8 *pt,u16 N);
extern unsigned char User_Write_Update_Data(int addr,unsigned char  *pt,unsigned int  n);
extern void 			User_Erase_Update_Data(void);
extern void 			Write_Update_Finish_Flag(void);
extern unsigned int	Read_Update_Finish_Flag(void);

void Flash_Update_Program(void);


#endif

/******************* (C) COPYRIGHT 2007 STMicroelectronics *****END OF FILE****/
