
#ifndef  __BMP085_H__
#define  __BMP085_H__
 

#ifndef  __BMP085_C
#define	 __BMP085  extern
#else
#define	 __BMP085 
#endif


__BMP085   long  Bmp085_Temperature;
__BMP085   MyType_f32  Bmp085_Pressure;

__BMP085 void BMP085_Init(void);
__BMP085 u16 BMP085_ReadPressure(void);
__BMP085 u16 BMP085_ReadTemp(void);
__BMP085 void Bmp085_ConvertInit(void);
__BMP085 void Bmp085_Convert(void);


#endif












