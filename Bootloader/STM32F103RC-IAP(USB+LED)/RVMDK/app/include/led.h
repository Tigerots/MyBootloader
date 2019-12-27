/******************** (C) COPYRIGHT 2010 HuaYuan ********************
* File Name          : LED.h
* Author             : Han yongliang
* Date First Issued  : 07/06/2009
* Description        : LED指示及数码管驱动头文件
********************************************************************************
* History:
* 07/02/2010: V1.0
*******************************************************************************/

#ifndef  __LED_H__
#define  __LED_H__

#define LED7SEG_NUMBER	5			//LED数字管的个数
#define LED_ROLL_CACHE_NUMBER	9	//LED数字缓存个数

#define LED_BLINK_REASON_TIME	1	//LED闪烁原因是显示时间
#define LED_BLINK_REASON_CUNKA	2	//LED闪烁原因是存卡提示
#define LED_BLINK_REASON_SET_MONTH	3	//LED闪烁原因是设置月
#define LED_BLINK_REASON_SET_DAY	4	//LED闪烁原因是设置日
#define LED_BLINK_REASON_SET_HOUR	5	//LED闪烁原因是设置小时
#define LED_BLINK_REASON_SET_MINUTE	6	//LED闪烁原因是设置分钟
#define LED_BLINK_REASON_ROLL_NUMBER 7	//LED闪烁原因是滚动显示数字
#define LED_BLINK_REASON_JIEZHANG	 8	//LED闪烁原因是结账提示
#define LED_BLINK_CLEAR_COINS_NOTE	 9	//LED闪烁原因是清币提示

#define ERROR_FLASH			1	//FLASH 存储器芯片有问题
#define ERROR_FRAM			2 	//铁电存储器
#define ERROR_RF_Module		3 	//RF_Module射频模块
#define ERROR_RSA_Public	4 	//公钥不正确
#define ERROR_RTC_Module	5 	//RTC模块有误
#define ERROR_LINE_MODE		6	//提示投币失败，接线模式为上分
#define ERROR_NO_Coins		7	//设备故障，无币错误或光眼错误
#define ERROR_UnSupport_Tui_ShiBi	8	//不支持退实币错误
#define ERROR_SYSTEM_BUSY	9	//系统正忙
#define ERROR_NOT_INIT		10	//系统尚未初始化
#define ERROR_Not_Param		11	//系统参数未设置
#define ERROR_OLD_USER_CARD	12	//应放原来的用户卡
#define ERROR_TUIBI_OVERFLOW	13	//退币超限溢出，超过65000了
	#define MAX_TUI_E_BI_LIMIT	65000
#define ERROR_SAVE_CARD_AGAIN	14	//从别处交易过，或存过卡，又来存卡错误
#define ERROR_Get_DevID_Fail	15	//开机卡获取设备ID时失败

#define ERROR_DISTURB		22	//提示为干扰

#define ERROR_TOUBI_LINE	30	//投币线未接
#define ERROR_TUIBI_LINE	31	//退币线未接
#define ERROR_TUIBI_ERROR	32	//退币错误
#define ERROR_TUIBI_COMMUNICATION	33	//退币通信错误
#define ERROR_COMMUNICATION			34	//通信失败
#define ERROR_TUISHIBI				35	//退实币失败

#define ERROR_MUST_BE_SAME_CARD		40	//存币时必须为同张卡
#define ERROR_USER_CARD_IS_OVERTIME	42	//用户卡已经失效错误


//读卡时的错误码
#define READ_ERROR_INVALID_USER_CARD	1	//非法用户卡
#define READ_ERROR_OVERTIME				2	//认证过期，请到吧台重新认证
#define READ_ERROR_OVER_VALUE			3	//累计投币值超限,请到吧台解锁!
#define READ_ERROR_INVALID_CARD			4	//非法卡
#define READ_ERROR_DATA					5	//数据错误
#define READ_ERROR_NOT_GAMEHALL			6	//提示不是本店卡

#define READ_ERROR_Verify_OK			9	//上次操作未完成，已修复
#define READ_DEVICEID_OK				0x10	//超过10后，就不显示了

//操作失败码
#define FAIL_NOT_INIT		0	//卡功能未初始化,刷钥匙卡初始化
#define FAIL_WRITE_CARD		1	//写卡失败
#define FAIL_NOT_LOCAL_GAMEHALL	2	//不是本店卡
#define FAIL_SUM_MAX_VALUE		3	//累计额度超过额度
#define FAIL_NOT_Enough			 4	//币值不足
#define FAIL_CUNKA_eDu_OVER		 5	//存卡额度用完
#define FAIL_SAVE_CARD			 6	//存卡失败
#define FAIL_JIEZHANG			 7	//结账失败
#define FAIL_CUNBI_OVER			 8	//存币值超限,禁止存卡
#define FAIL_NetWork_OverTime	 9	//网络超时
#define LIGHT_ON	1	//亮
#define LIGHT_OFF	0	//灭
//定义是还是否
#define LED_DISPLAY_YES	1	
#define LED_DISPLAY_NO	2

//////CMD1 设置显示//////
#define	TM_CMD1_INC 0x40 // 地址自动加1模式
#define	TM_CMD1_STEP 0x44 // 地址固定 
//////CMD2 设置地址//////////
#define	TM_CMD2	0xC0	//设置地址

/////CMD3 显示控制////////
#define TM_CMD3_OFF	0x80   	//显示关
#define TM_CMD3_ON	0x88	//显示开 ，默认亮度最低

extern void TM_MDis(unsigned char addr,unsigned char num,unsigned char *dat);

#define LED_NUMBER_0	0x3f
#define LED_NUMBER_1	0x06
#define LED_NUMBER_2	0x5b
#define LED_NUMBER_3	0x4f
#define LED_NUMBER_4	0x66
#define LED_NUMBER_5	0x6d
#define LED_NUMBER_6	0x7d 
#define LED_NUMBER_7	0x07
#define LED_NUMBER_8	0x7f
#define LED_NUMBER_9	0x6f
#define LED_NUMBER_A	0x77
#define LED_NUMBER_B	0x7c
#define LED_NUMBER_C	0x39
#define LED_NUMBER_D	0x3f
#define LED_NUMBER_E	0x79
#define LED_NUMBER_F	0x71
#define LED_NUMBER_H	0x76
#define LED_NUMBER_L	0x38
#define LED_NUMBER_b	0x7C
#define LED_NUMBER_R	LED_NUMBER_r	//0x31
#define LED_NUMBER_r	0x50
#define LED_NUMBER_c	0x58
#define LED_NUMBER_d	0x5e
#define LED_NUMBER_J	0x0e
#define LED_NUMBER_I	0x30
#define LED_NUMBER_i	0x10
#define LED_NUMBER_II	0x36
#define LED_NUMBER_U	0x3E	//显示的是'U'
#define LED_NUMBER_V	LED_NUMBER_U	//显示的是'U'
#define LED_NUMBER_N	0x37
#define LED_NUMBER_n	0x54
#define LED_NUMBER_o	0x5C
#define LED_NUMBER_o1	0x63
#define LED_NUMBER_Y	0x6E	
#define LED_NUMBER_P	0x73	//显示的是'P'
#define LED_NUMBER_S	LED_NUMBER_5	//显示的是'S'
#define LED_NUMBER_t	0x78	//显示的是't'
#define LED_NUMBER_DOT	0x80	//显示的是'.'
#define LED_NUMBER_LINE	0x40	//显示的是'-'
#define LED_NUMBER_2LINE	0x48	//显示的是'='


#define LED_NUMBER_NULL 0

void TM1620_Init(void);
void TM_MDis(u8 addr,u8 num,u8 *dat);
void TM_Display_Light_On(void);
void TM_Display_Light_Off(void);

void TM_Display_Number(u32 number);
void TM_Display_Number_Roll(u32 number);
void TM_Update_Display_Buffer(void);
void TM_Display_Device_Num(u8 dev_num);
void TM_Display_Number_GDx(u8 dtype,u32 number);

extern u8 LED_Display_Cache[10];	//LED显示缓存
extern u8  DispFlashBuf[10];//LED显示缓存


#endif   


