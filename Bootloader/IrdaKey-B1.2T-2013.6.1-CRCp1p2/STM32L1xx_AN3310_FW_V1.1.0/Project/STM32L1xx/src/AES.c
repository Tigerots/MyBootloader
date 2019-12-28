#include <include.h>
#include <string.h>   
#include "aes.h"  
#include "crc.h"  

//#define SETTED		1	//已经设置
//#define UNSETTED	0	//没有设置
   
#define BPOLY 0x1b //!< Lower 8 bits of (x^8+x^4+x^3+x+1), ie. (x^4+x^3+x+1).   
#define BLOCKSIZE 16 //!< Block size in number of bytes.    
   
#define KEYBITS 128 //!< Use AES128.   
#define ROUNDS 10 //!< Number of rounds.   
#define KEYLENGTH 16 //!< Key length in number of bytes.   

//固定密钥(KDF,KEY基本信息)
//const u8 AesKey_Fixed[16] = {0x34,0x78,0xEF,0x00,0xDC,0x90,0x69,0x32,0x53,0x54,0x43,0x5F,0x48,0x4A,0xB8,0xBA};//固定AES密钥
//const u8 Key_Aes_Auth[16] = {0xd3,0xaf,0xbc,0xd3,0xb5,0xe7,0xd7,0xd3,0xbf,0xc6,0xbc,0xbc,0x00,0x00,0x00,0x00};//初始写key认证密钥
unsigned char Key_Aes_Data[16];//密钥
u8 const Key_Aes_KDF[16]={0x34,0x78,0xEF,0x00,0xDC,0x90,0x69,0x32,0x53,0x54,0x43,0x5F,0x48,0x4A,0xB8,0xBA};	//固定AES密钥

   
u8 AES_block1[ 256 ]; //!< Workspace 1.   
u8 AES_block2[ 256 ]; //!< Worksapce 2.   
u8 AES_sBoxbuf[256];      

u8 * powTbl; //!< Final location of exponentiation lookup table.   
u8 * logTbl; //!< Final location of logarithm lookup table.   
u8 * sBox; //!< Final location of s-box.   
u8 * sBoxInv; //!< Final location of inverse s-box.   
u8 * expandedKey; //!< Final location of expanded key.   
   
  
   
void CalcPowLog( u8 * powTbl, u8 * logTbl )   
{   
        u8 i = 0;   
        u8 t = 1;   
   
        do {   
        // Use 0x03 as root for exponentiation and logarithms.   
            powTbl[i] = t;   
            logTbl[t] = i;   
            i++;   
   
        // Muliply t by 3 in GF(2^8).   
            t ^= (t << 1) ^ (t & 0x80 ? BPOLY : 0);   
        } while( t != 1 ); // Cyclic properties ensure that i < 255.   
           
        powTbl[255] = powTbl[0]; // 255 = '-0', 254 = -1, etc.   
}   
   
   
   
void CalcSBox( u8 * sBox )   
{   
        u8 i, rot;   
        u8 temp;   
        u8 result;   
   
    // Fill all entries of sBox[].   
    i = 0;   
    do {   
                // Inverse in GF(2^8).   
                if( i > 0 ) {   
                    temp = powTbl[ 255 - logTbl[i] ];   
            } else {   
                    temp = 0;   
        }   
   
                // Affine transformation in GF(2).   
                result = temp ^ 0x63; // Start with adding a vector in GF(2).   
                for( rot = 0; rot < 4; rot++ ) {   
                        // Rotate left.   
            temp = (temp<<1) | (temp>>7);   
   
            // Add rotated byte in GF(2).   
            result ^= temp;   
        }   
               
        // Put result in table.   
                sBox[i] = result;   
    } while( ++i != 0 );   
}      
   
   
   
void CalcSBoxInv( u8 * sBox, u8 * sBoxInv )   
{   
    u8 i = 0;   
    u8 j = 0;   
   
    // Iterate through all elements in sBoxInv using  i.   
    do {   
        // Search through sBox using j.   
        do {   
            // Check if current j is the inverse of current i.   
            if( sBox[ j ] == i ) {   
                // If so, set sBoxInc and indicate search finished.   
                sBoxInv[ i ] = j;   
                j = 255;   
            }   
        } while( ++j != 0 );   
    } while( ++i != 0 );   
}   
   
   
   
void CycleLeft( u8 * row )   
{   
    // Cycle 4 bytes in an array left once.   
    u8 temp = row[0];   
    row[0] = row[1];   
    row[1] = row[2];   
    row[2] = row[3];   
    row[3] = temp;   
}   
   
   
   
void InvMixColumn( u8 * column )   
{   
    u8 r0, r1, r2, r3;   
   
    r0 = column[1] ^ column[2] ^ column[3];   
    r1 = column[0] ^ column[2] ^ column[3];   
    r2 = column[0] ^ column[1] ^ column[3];   
    r3 = column[0] ^ column[1] ^ column[2];   
   
    column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);   
    column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);   
    column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);   
    column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);   
   
    r0 ^= column[0] ^ column[1];   
    r1 ^= column[1] ^ column[2];   
    r2 ^= column[2] ^ column[3];   
    r3 ^= column[0] ^ column[3];   
   
    column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);   
    column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);   
    column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);   
    column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);   
   
    r0 ^= column[0] ^ column[2];   
    r1 ^= column[1] ^ column[3];   
    r2 ^= column[0] ^ column[2];   
    r3 ^= column[1] ^ column[3];   
   
    column[0] = (column[0] << 1) ^ (column[0] & 0x80 ? BPOLY : 0);   
    column[1] = (column[1] << 1) ^ (column[1] & 0x80 ? BPOLY : 0);   
    column[2] = (column[2] << 1) ^ (column[2] & 0x80 ? BPOLY : 0);   
    column[3] = (column[3] << 1) ^ (column[3] & 0x80 ? BPOLY : 0);   
   
    column[0] ^= column[1] ^ column[2] ^ column[3];   
    r0 ^= column[0];   
    r1 ^= column[0];   
    r2 ^= column[0];   
    r3 ^= column[0];   
   
    column[0] = r0;   
    column[1] = r1;   
    column[2] = r2;   
    column[3] = r3;   
}   
   
u8 Multiply( unsigned char num, unsigned char factor )   
{   
    u8 mask = 1;   
    u8 result = 0;   
   
    while( mask != 0 ) {   
        // Check bit of factor given by mask.   
        if( mask & factor ) {   
            // Add current multiple of num in GF(2).   
            result ^= num;   
        }   
   
        // Shift mask to indicate next bit.   
        mask <<= 1;   
   
        // Double num.   
            num = (num << 1) ^ (num & 0x80 ? BPOLY : 0);   
    }   
   
    return result;   
}   
   
   
u8 DotProduct( unsigned char * vector1, unsigned char * vector2 )   
{   
    u8 result = 0;   
   
    result ^= Multiply( *vector1++, *vector2++ );   
    result ^= Multiply( *vector1++, *vector2++ );   
    result ^= Multiply( *vector1++, *vector2++ );   
    result ^= Multiply( *vector1  , *vector2   );   
   
    return result;   
}   
   
void MixColumn( u8 * column )   
{   
    u8 row[8] = {   
        0x02, 0x03, 0x01, 0x01,   
        0x02, 0x03, 0x01, 0x01   
    }; // Prepare first row of matrix twice, to eliminate need for cycling.   
   
    u8 result[4];   
       
    // Take dot products of each matrix row and the column vector.   
    result[0] = DotProduct( row+0, column );   
    result[1] = DotProduct( row+3, column );   
    result[2] = DotProduct( row+2, column );   
    result[3] = DotProduct( row+1, column );   
   
    // Copy temporary result to original column.   
    column[0] = result[0];   
    column[1] = result[1];   
    column[2] = result[2];   
    column[3] = result[3];   
}   
   
   
void SubBytes( u8 * bytes, u8 count )   
{   
    do {   
        *bytes = sBox[ *bytes ]; // Substitute every byte in state.   
        bytes++;   
    } while( --count );   
}   
   
   
   
void InvSubBytesAndXOR( u8 * bytes, u8 * key, u8 count )   
{   
    do {   
//      *bytes = sBoxInv[ *bytes ] ^ *key; // Inverse substitute every byte in state and add key.   
        *bytes = AES_block2[ *bytes ] ^ *key; // Use block2 directly. Increases speed.   
        bytes++;   
        key++;   
    } while( --count );   
}   
   
   
   
void InvShiftRows( u8 * state )   
{   
    u8 temp;   
   
    // Note: State is arranged column by column.   
   
    // Cycle second row right one time.   
    temp = state[ 1 + 3*4 ];   
    state[ 1 + 3*4 ] = state[ 1 + 2*4 ];   
    state[ 1 + 2*4 ] = state[ 1 + 1*4 ];   
    state[ 1 + 1*4 ] = state[ 1 + 0*4 ];   
    state[ 1 + 0*4 ] = temp;   
   
    // Cycle third row right two times.   
    temp = state[ 2 + 0*4 ];   
    state[ 2 + 0*4 ] = state[ 2 + 2*4 ];   
    state[ 2 + 2*4 ] = temp;   
    temp = state[ 2 + 1*4 ];   
    state[ 2 + 1*4 ] = state[ 2 + 3*4 ];   
    state[ 2 + 3*4 ] = temp;   
   
    // Cycle fourth row right three times, ie. left once.   
    temp = state[ 3 + 0*4 ];   
    state[ 3 + 0*4 ] = state[ 3 + 1*4 ];   
    state[ 3 + 1*4 ] = state[ 3 + 2*4 ];   
    state[ 3 + 2*4 ] = state[ 3 + 3*4 ];   
    state[ 3 + 3*4 ] = temp;   
}   
   
void ShiftRows( u8 * state )   
{   
    u8 temp;   
   
    // Note: State is arranged column by column.   
   
    // Cycle second row left one time.   
    temp = state[ 1 + 0*4 ];   
    state[ 1 + 0*4 ] = state[ 1 + 1*4 ];   
    state[ 1 + 1*4 ] = state[ 1 + 2*4 ];   
    state[ 1 + 2*4 ] = state[ 1 + 3*4 ];   
    state[ 1 + 3*4 ] = temp;   
   
    // Cycle third row left two times.   
    temp = state[ 2 + 0*4 ];   
    state[ 2 + 0*4 ] = state[ 2 + 2*4 ];   
    state[ 2 + 2*4 ] = temp;   
    temp = state[ 2 + 1*4 ];   
    state[ 2 + 1*4 ] = state[ 2 + 3*4 ];   
    state[ 2 + 3*4 ] = temp;   
   
    // Cycle fourth row left three times, ie. right once.   
    temp = state[ 3 + 3*4 ];   
    state[ 3 + 3*4 ] = state[ 3 + 2*4 ];   
    state[ 3 + 2*4 ] = state[ 3 + 1*4 ];   
    state[ 3 + 1*4 ] = state[ 3 + 0*4 ];   
    state[ 3 + 0*4 ] = temp;   
}   
   
   
void InvMixColumns( u8 * state )   
{   
    InvMixColumn( state + 0*4 );   
    InvMixColumn( state + 1*4 );   
    InvMixColumn( state + 2*4 );   
    InvMixColumn( state + 3*4 );   
}   
   
void MixColumns( u8 * state )   
{   
    MixColumn( state + 0*4 );   
    MixColumn( state + 1*4 );   
    MixColumn( state + 2*4 );   
    MixColumn( state + 3*4 );   
}   
   
   
void XORBytes( u8 * bytes1, u8 * bytes2, u8 count )   
{   
    do {   
        *bytes1 ^= *bytes2; // Add in GF(2), ie. XOR.   
        bytes1++;   
        bytes2++;   
    } while( --count );   
}   
   
   
   
void CopyBytes( u8 * to, u8 * from, u8 count )   
{   
    do {   
        *to = *from;   
        to++;   
        from++;   
    } while( --count );   
}   
   
   
   
void KeyExpansion( u8 * expandedKey )   
{   
    u8 temp[4];   
    u8 i;   
    u8 Rcon[4] = { 0x01, 0x00, 0x00, 0x00 }; // Round constant.   
       
    ////////////////////////////////////////////   
   
    // Copy key to start of expanded key.   
    for(i=0;i< KEYLENGTH;  i++)
    {   
		/*if(Use_NetWork_AES_Flag	==SETTED)
			*expandedKey = Net_Key_Aes_Data[i];   //网络传输用的密钥
		else if(Use_JiaoYi_Process_AES_Flag	==SETTED)
			*expandedKey = JiaoYi_Key_Aes_Data[i];   //交易过程用的密钥
		else*/
        	*expandedKey = Key_Aes_Data[i];   	  //读写卡用的密钥
        expandedKey++;   
    }
   
    // Prepare last 4 bytes of key in temp.   
    expandedKey -= 4;   
    temp[0] = *(expandedKey++);   
    temp[1] = *(expandedKey++);   
    temp[2] = *(expandedKey++);   
    temp[3] = *(expandedKey++);   
   
    // Expand key.   
    i = KEYLENGTH;   
    while( i < BLOCKSIZE*(ROUNDS+1) ) {   
        // Are we at the start of a multiple of the key size?   
        if( (i % KEYLENGTH) == 0 ) {   
            CycleLeft( temp ); // Cycle left once.   
            SubBytes( temp, 4 ); // Substitute each byte.   
            XORBytes( temp, Rcon, 4 ); // Add constant in GF(2).   
            *Rcon = (*Rcon << 1) ^ (*Rcon & 0x80 ? BPOLY : 0);   
        }   
   
        // Keysize larger than 24 bytes, ie. larger that 192 bits?   
        #if KEYLENGTH > 24   
        // Are we right past a block size?   
        else if( (i % KEYLENGTH) == BLOCKSIZE ) {   
            SubBytes( temp, 4 ); // Substitute each byte.   
        }   
        #endif   
   
        // Add bytes in GF(2) one KEYLENGTH away.   
        XORBytes( temp, expandedKey - KEYLENGTH, 4 );   
   
        // Copy result to current 4 bytes.   
        *(expandedKey++) = temp[ 0 ];   
        *(expandedKey++) = temp[ 1 ];   
        *(expandedKey++) = temp[ 2 ];   
        *(expandedKey++) = temp[ 3 ];   
   
        i += 4; // Next 4 bytes.   
    }      
}   
   
   
   
void InvCipher( u8 * block, u8 * expandedKey )   
{   
    u8 round = ROUNDS-1;   
    expandedKey += BLOCKSIZE * ROUNDS;   
   
    XORBytes( block, expandedKey, 16 );   
    expandedKey -= BLOCKSIZE;   
   
    do {   
        InvShiftRows( block );   
        InvSubBytesAndXOR( block, expandedKey, 16 );   
        expandedKey -= BLOCKSIZE;   
        InvMixColumns( block );   
    } while( --round );   
   
    InvShiftRows( block );   
    InvSubBytesAndXOR( block, expandedKey, 16 );   
}   
   
void Cipher( u8 * block, u8 * expandedKey )    //完成一个块(16字节，128bit)的加密   
{   
    u8 round = ROUNDS-1;   
   
    XORBytes( block, expandedKey, 16 );   
    expandedKey += BLOCKSIZE;   
   
    do {   
        SubBytes( block, 16 );   
        ShiftRows( block );   
        MixColumns( block );   
        XORBytes( block, expandedKey, 16 );   
        expandedKey += BLOCKSIZE;   
    } while( --round );   
   
    SubBytes( block, 16 );   
    ShiftRows( block );   
    XORBytes( block, expandedKey, 16 );   
}   
   
void aesInit( unsigned char * tempbuf )   
{   
    powTbl = AES_block1;   
    logTbl = AES_block2;   
    CalcPowLog( powTbl, logTbl );   
   
    sBox = tempbuf;   
    CalcSBox( sBox );   
   
    expandedKey = AES_block1;  //至此block1用来存贮密码表   
    KeyExpansion( expandedKey );   
       
    sBoxInv = AES_block2; // Must be block2. block2至此开始只用来存贮SBOXINV   
    CalcSBoxInv( sBox, sBoxInv );   
}      
   
   
//对一个16字节块解密,参数buffer是解密密缓存，chainBlock是要解密的块   
void aesDecrypt( unsigned char * buffer, unsigned char * chainBlock )   
{   
    //byte xdata temp[ BLOCKSIZE ];   
   
    //CopyBytes( temp, buffer, BLOCKSIZE );   
    CopyBytes(buffer,chainBlock,BLOCKSIZE);   
    InvCipher( buffer, expandedKey );   
    //XORBytes( buffer, chainBlock, BLOCKSIZE );   
    CopyBytes( chainBlock, buffer, BLOCKSIZE );   
}   
   
//对一个16字节块完成加密，参数buffer是加密缓存，chainBlock是要加密的块   
void aesEncrypt( unsigned char * buffer, unsigned char * chainBlock )   
{   
    CopyBytes( buffer, chainBlock, BLOCKSIZE );   
    //XORBytes( buffer, chainBlock, BLOCKSIZE );   
    Cipher( buffer, expandedKey );   
    CopyBytes( chainBlock, buffer, BLOCKSIZE ); 
}   
   
//加解密函数，参数为加解密标志，要加解密的数据缓存起始指针，要加解密的数据长度（如果解密运算，必须是16的整数倍。）   
unsigned char Data_AES(u8 flag,unsigned char *ChiperDataBuf,unsigned char DataLen)   
{   
    unsigned char  i;   
    unsigned char  Blocks;   
    unsigned char  tempbuf[16];   
   
	 
    aesInit(AES_sBoxbuf);     //初始化   
    if(flag==DECRYPT)      //解密   
    {   
       Blocks=DataLen/16;   
       for(i=0;i<Blocks;i++)   
       {   
            aesDecrypt(tempbuf,ChiperDataBuf+16*i);   
       }   
       return(DataLen);   
    }   
    else       //加密   
    {   
        if(DataLen%16!=0)   
       {     
           Blocks=DataLen/16+1;   
           //memset(ChiperDataBuf+4+Blocks*16-(DataLen%16),0x00,DataLen%16); //不足16字节的块补零处理   
       }   
        else   
       {   
           Blocks=DataLen/16;   
       }   
   
        for(i=0;i<Blocks;i++)   
       {   
            aesEncrypt(tempbuf,ChiperDataBuf+16*i);   
       }   
       return(Blocks*16);   
    }   
       
}   
//生成网络通信用的密钥
//卡号【4】+交易时间【6】+保留【6】（置0），组成AES密钥
/*
void Make_Net_Communication_AES_Key(void)
{
u32 iTemp;

	memset(Net_Key_Aes_Data,1,16);
	
	iTemp	=	System_Card_Param.Game_Hall_ID;
	Net_Key_Aes_Data[0]	=	iTemp&0xff;
	Net_Key_Aes_Data[1]	=	(iTemp>>8)&0xff;
	Net_Key_Aes_Data[2]	=	(iTemp>>16)&0xff;
	Net_Key_Aes_Data[3]	=	(iTemp>>24)&0xff;

	if(Current_Card_Attrib.Insert_Card_Flag==SETTED)//有卡状态
	{ 		
		Net_Key_Aes_Data[4]	=	Current_Card_Attrib.Card_ID[0];
		Net_Key_Aes_Data[5]	=	Current_Card_Attrib.Card_ID[1];
		Net_Key_Aes_Data[6]	=	Current_Card_Attrib.Card_ID[2];
		Net_Key_Aes_Data[7]	=	Current_Card_Attrib.Card_ID[3];	
	}
	else
	{
		Net_Key_Aes_Data[4]	=	0;
		Net_Key_Aes_Data[5]	=	0;
		Net_Key_Aes_Data[6]	=	0;
		Net_Key_Aes_Data[7]	=	0;
	}
} */ 
//店码+全0卡号,组成AES密钥
/*
void Make_TuoJi_JiaoYi_AES_Key(void)
{
u32 iTemp;

	memset(Net_Key_Aes_Data,1,16);
	
	iTemp	=	System_Card_Param.Game_Hall_ID;
	Net_Key_Aes_Data[0]	=	iTemp&0xff;
	Net_Key_Aes_Data[1]	=	(iTemp>>8)&0xff;
	Net_Key_Aes_Data[2]	=	(iTemp>>16)&0xff;
	Net_Key_Aes_Data[3]	=	(iTemp>>24)&0xff;
	Net_Key_Aes_Data[4]	=	0;
	Net_Key_Aes_Data[5]	=	0;
	Net_Key_Aes_Data[6]	=	0;
	Net_Key_Aes_Data[7]	=	0;

}  
//网络传输用的,加解密函数，参数为加解密标志，要加解密的数据缓存起始指针，要加解密的数据长度（如果解密运算，必须是16的整数倍。）   
unsigned char NetWork_Data_AES(u8 flag,unsigned char *ChiperDataBuf,unsigned char DataLen)
{
u8 len;
	Make_Net_Communication_AES_Key();
	Use_NetWork_AES_Flag	=SETTED;
	len	=	Data_AES(flag,ChiperDataBuf,DataLen);
	Use_NetWork_AES_Flag	=UNSETTED;
return len;
}
//网络传输用的,加解密函数，参数为加解密标志，要加解密的数据缓存起始指针，要加解密的数据长度（如果解密运算，必须是16的整数倍。）   
unsigned char NetWork_TuoJi_JiaoYi_AES(u8 flag,unsigned char *ChiperDataBuf,unsigned char DataLen)
{
u8 len;
	
	Make_TuoJi_JiaoYi_AES_Key();
	Use_NetWork_AES_Flag	=SETTED;
	len	=	Data_AES(flag,ChiperDataBuf,DataLen);
	Use_NetWork_AES_Flag	=UNSETTED;

return len;
}
//生成交易过程用的密钥
//卡号【4】+交易时间【6】+保留【6】（置0），组成AES密钥

void Make_JiaoYi_Process_AES_Key(void)
{
	memset(JiaoYi_Key_Aes_Data,0,16);
	
	JiaoYi_Key_Aes_Data[0]	=	Current_Card_Attrib.Card_ID[0];
	JiaoYi_Key_Aes_Data[1]	=	Current_Card_Attrib.Card_ID[1];
	JiaoYi_Key_Aes_Data[2]	=	Current_Card_Attrib.Card_ID[2];
	JiaoYi_Key_Aes_Data[3]	=	Current_Card_Attrib.Card_ID[3];

	JiaoYi_Key_Aes_Data[4]	=	JiaoYi_DateTime.year-2000;
	JiaoYi_Key_Aes_Data[5]	=	JiaoYi_DateTime.month;
	JiaoYi_Key_Aes_Data[6]	=	JiaoYi_DateTime.day;
	JiaoYi_Key_Aes_Data[7]	=	JiaoYi_DateTime.hour;
	JiaoYi_Key_Aes_Data[8]	=	JiaoYi_DateTime.minute;
	JiaoYi_Key_Aes_Data[9]	=	JiaoYi_DateTime.second;
}  
//交易过程用的,加解密函数，参数为加解密标志，要加解密的数据缓存起始指针，要加解密的数据长度（如果解密运算，必须是16的整数倍。）   
unsigned char JiaoYi_Data_AES(u8 flag,unsigned char *ChiperDataBuf,unsigned char DataLen)
{
u8 len;
	
	Make_JiaoYi_Process_AES_Key();//生成交易过程用的密钥
	Use_JiaoYi_Process_AES_Flag	=SETTED;
	len	=	Data_AES(flag,ChiperDataBuf,DataLen);
	Use_JiaoYi_Process_AES_Flag	=UNSETTED;
return len;
}
 */



