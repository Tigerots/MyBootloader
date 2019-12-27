/******************** (C) COPYRIGHT 2010 HuaYuan ********************
* File Name          : LED.h
* Author             : Han yongliang
* Date First Issued  : 07/06/2009
* Description        : LEDָʾ�����������ͷ�ļ�
********************************************************************************
* History:
* 07/02/2010: V1.0
*******************************************************************************/

#ifndef  __LED_H__
#define  __LED_H__

#define LED7SEG_NUMBER	5			//LED���ֹܵĸ���
#define LED_ROLL_CACHE_NUMBER	9	//LED���ֻ������

#define LED_BLINK_REASON_TIME	1	//LED��˸ԭ������ʾʱ��
#define LED_BLINK_REASON_CUNKA	2	//LED��˸ԭ���Ǵ濨��ʾ
#define LED_BLINK_REASON_SET_MONTH	3	//LED��˸ԭ����������
#define LED_BLINK_REASON_SET_DAY	4	//LED��˸ԭ����������
#define LED_BLINK_REASON_SET_HOUR	5	//LED��˸ԭ��������Сʱ
#define LED_BLINK_REASON_SET_MINUTE	6	//LED��˸ԭ�������÷���
#define LED_BLINK_REASON_ROLL_NUMBER 7	//LED��˸ԭ���ǹ�����ʾ����
#define LED_BLINK_REASON_JIEZHANG	 8	//LED��˸ԭ���ǽ�����ʾ
#define LED_BLINK_CLEAR_COINS_NOTE	 9	//LED��˸ԭ���������ʾ

#define ERROR_FLASH			1	//FLASH �洢��оƬ������
#define ERROR_FRAM			2 	//����洢��
#define ERROR_RF_Module		3 	//RF_Module��Ƶģ��
#define ERROR_RSA_Public	4 	//��Կ����ȷ
#define ERROR_RTC_Module	5 	//RTCģ������
#define ERROR_LINE_MODE		6	//��ʾͶ��ʧ�ܣ�����ģʽΪ�Ϸ�
#define ERROR_NO_Coins		7	//�豸���ϣ��ޱҴ������۴���
#define ERROR_UnSupport_Tui_ShiBi	8	//��֧����ʵ�Ҵ���
#define ERROR_SYSTEM_BUSY	9	//ϵͳ��æ
#define ERROR_NOT_INIT		10	//ϵͳ��δ��ʼ��
#define ERROR_Not_Param		11	//ϵͳ����δ����
#define ERROR_OLD_USER_CARD	12	//Ӧ��ԭ�����û���
#define ERROR_TUIBI_OVERFLOW	13	//�˱ҳ������������65000��
	#define MAX_TUI_E_BI_LIMIT	65000
#define ERROR_SAVE_CARD_AGAIN	14	//�ӱ𴦽��׹����������������濨����
#define ERROR_Get_DevID_Fail	15	//��������ȡ�豸IDʱʧ��

#define ERROR_DISTURB		22	//��ʾΪ����

#define ERROR_TOUBI_LINE	30	//Ͷ����δ��
#define ERROR_TUIBI_LINE	31	//�˱���δ��
#define ERROR_TUIBI_ERROR	32	//�˱Ҵ���
#define ERROR_TUIBI_COMMUNICATION	33	//�˱�ͨ�Ŵ���
#define ERROR_COMMUNICATION			34	//ͨ��ʧ��
#define ERROR_TUISHIBI				35	//��ʵ��ʧ��

#define ERROR_MUST_BE_SAME_CARD		40	//���ʱ����Ϊͬ�ſ�
#define ERROR_USER_CARD_IS_OVERTIME	42	//�û����Ѿ�ʧЧ����


//����ʱ�Ĵ�����
#define READ_ERROR_INVALID_USER_CARD	1	//�Ƿ��û���
#define READ_ERROR_OVERTIME				2	//��֤���ڣ��뵽��̨������֤
#define READ_ERROR_OVER_VALUE			3	//�ۼ�Ͷ��ֵ����,�뵽��̨����!
#define READ_ERROR_INVALID_CARD			4	//�Ƿ���
#define READ_ERROR_DATA					5	//���ݴ���
#define READ_ERROR_NOT_GAMEHALL			6	//��ʾ���Ǳ��꿨

#define READ_ERROR_Verify_OK			9	//�ϴβ���δ��ɣ����޸�
#define READ_DEVICEID_OK				0x10	//����10�󣬾Ͳ���ʾ��

//����ʧ����
#define FAIL_NOT_INIT		0	//������δ��ʼ��,ˢԿ�׿���ʼ��
#define FAIL_WRITE_CARD		1	//д��ʧ��
#define FAIL_NOT_LOCAL_GAMEHALL	2	//���Ǳ��꿨
#define FAIL_SUM_MAX_VALUE		3	//�ۼƶ�ȳ������
#define FAIL_NOT_Enough			 4	//��ֵ����
#define FAIL_CUNKA_eDu_OVER		 5	//�濨�������
#define FAIL_SAVE_CARD			 6	//�濨ʧ��
#define FAIL_JIEZHANG			 7	//����ʧ��
#define FAIL_CUNBI_OVER			 8	//���ֵ����,��ֹ�濨
#define FAIL_NetWork_OverTime	 9	//���糬ʱ
#define LIGHT_ON	1	//��
#define LIGHT_OFF	0	//��
//�����ǻ��Ƿ�
#define LED_DISPLAY_YES	1	
#define LED_DISPLAY_NO	2

//////CMD1 ������ʾ//////
#define	TM_CMD1_INC 0x40 // ��ַ�Զ���1ģʽ
#define	TM_CMD1_STEP 0x44 // ��ַ�̶� 
//////CMD2 ���õ�ַ//////////
#define	TM_CMD2	0xC0	//���õ�ַ

/////CMD3 ��ʾ����////////
#define TM_CMD3_OFF	0x80   	//��ʾ��
#define TM_CMD3_ON	0x88	//��ʾ�� ��Ĭ���������

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
#define LED_NUMBER_U	0x3E	//��ʾ����'U'
#define LED_NUMBER_V	LED_NUMBER_U	//��ʾ����'U'
#define LED_NUMBER_N	0x37
#define LED_NUMBER_n	0x54
#define LED_NUMBER_o	0x5C
#define LED_NUMBER_o1	0x63
#define LED_NUMBER_Y	0x6E	
#define LED_NUMBER_P	0x73	//��ʾ����'P'
#define LED_NUMBER_S	LED_NUMBER_5	//��ʾ����'S'
#define LED_NUMBER_t	0x78	//��ʾ����'t'
#define LED_NUMBER_DOT	0x80	//��ʾ����'.'
#define LED_NUMBER_LINE	0x40	//��ʾ����'-'
#define LED_NUMBER_2LINE	0x48	//��ʾ����'='


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

extern u8 LED_Display_Cache[10];	//LED��ʾ����
extern u8  DispFlashBuf[10];//LED��ʾ����


#endif   


