

#ifndef  __STH1x_H__
#define  __STH1x_H__
 

#ifndef  __STH10_C
#define	 __STH10  extern
#else
#define	 __STH10 
#endif

__packed typedef union{
	float f32;
	unsigned char uc8[4];
}MyType_f32;

__STH10 u8 SetCmd;//�¶�
__STH10 MyType_f32 SetData;//�¶�


//���ղ������,�ϴ�ʱʹ��
__STH10 MyType_f32 Temperature;//�¶�
__STH10 MyType_f32 Humidity;//ʪ��
__STH10 MyType_f32 Pressure;//��ѹ

//ƫ��ֵ,�궨ʱʹ��(Setxxx = ����ֵ-��ǰֵ)
__packed typedef struct      // �����豸��Ϣ�ṹ������
{
    MyType_f32 SetTemperature;//�¶�
    MyType_f32 SetHumidity;//ʪ��
    MyType_f32 SetPressure;//��ѹ 
}Set_TYPE;
__STH10 Set_TYPE SetInfo;



//��������ֱ�Ӷ�ȡֵ
__STH10 MyType_f32 Sth10_Temperature;//�¶�
__STH10 MyType_f32 Sth10_Humidity;//ʪ��

__STH10 void InitSTH1x(void);
__STH10 float Sth1xMeasure(u8 mType);


#endif














