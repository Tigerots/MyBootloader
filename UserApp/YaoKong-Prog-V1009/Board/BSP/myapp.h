

#ifndef  __MYAPP_H__
#define  __MYAPP_H__
 

#ifndef  MYAPP
#define	 __MYAPP  extern
#else
#define	 __MYAPP 
#endif

#define FLASH_OK			CMD_OK
#define FLASH_ERROR			1

#define CMD_OK				0
#define CMD_FAIL			1


#define Def_HardVerSion 0x1001//Ӳ���汾��
#define Def_HardVerSion_SIZE		0x10
#define Def_HardVerSion_ADDR		(0x0803F000)

#define Def_SoftVersion 0x1001//����汾��


//����Flash����ʱ�õ��ĺ궨��,�����Ƭ��Flash��
//#define UPDATE_FLAG_ADDR			0x0803E000	//������־��
//#define UPDATE_FLAG_SIZE			0x10		//������־��Сʵ��Ϊ4�ֽ�

#define USER_DATA_ADDR			0x0803D000
#define USER_DATE_SIZE			246		


__MYAPP char  *gcvt(char *buf, INT8U nInt, INT8U ndigit, float number);
// 


#endif






