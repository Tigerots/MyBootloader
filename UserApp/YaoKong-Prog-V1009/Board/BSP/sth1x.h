

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

__STH10 u8 SetCmd;//温度
__STH10 MyType_f32 SetData;//温度


//最终测量结果,上传时使用
__STH10 MyType_f32 Temperature;//温度
__STH10 MyType_f32 Humidity;//湿度
__STH10 MyType_f32 Pressure;//气压

//偏置值,标定时使用(Setxxx = 设置值-当前值)
__packed typedef struct      // 定义设备信息结构体类型
{
    MyType_f32 SetTemperature;//温度
    MyType_f32 SetHumidity;//湿度
    MyType_f32 SetPressure;//气压 
}Set_TYPE;
__STH10 Set_TYPE SetInfo;



//传感器的直接读取值
__STH10 MyType_f32 Sth10_Temperature;//温度
__STH10 MyType_f32 Sth10_Humidity;//湿度

__STH10 void InitSTH1x(void);
__STH10 float Sth1xMeasure(u8 mType);


#endif














