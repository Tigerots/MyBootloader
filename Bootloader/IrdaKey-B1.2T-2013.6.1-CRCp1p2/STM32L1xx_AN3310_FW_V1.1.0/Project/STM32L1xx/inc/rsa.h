/******************** (C) COPYRIGHT 2007 STMicroelectronics ********************
* File Name          : Lcd.h
* Author             : Han yongliang
* Date First Issued  : 2008.5.23
* Description        : LCD functions
*       
********************************************************************************
* History   :
* 2008.5.23 : Version 0.1
*******************************************************************************/
#ifndef  __RSA_H__
#define  __RSA_H__


#define false	1
#define true	0

/* POINTER defines a generic pointer type */
typedef unsigned char *POINTER;

/* UINT2 defines a two byte word */
typedef unsigned short int UINT2;

/* UINT4 defines a four byte word */
typedef unsigned long int UINT4;


/* Message-digest algorithms.
 */

/* Message-digest algorithms.
 */
#define DA_MD2 3
#define DA_MD5 5

/* Encryption algorithms to be ored with digest algorithm in Seal and Open.
 */
#define EA_DES_CBC 1
#define EA_DES_EDE2_CBC 2
#define EA_DES_EDE3_CBC 3
#define EA_DESX_CBC 4

/* RSA key lengths.
 */
#define MIN_RSA_MODULUS_BITS 508
#define MAX_RSA_MODULUS_BITS 512
#define MAX_RSA_MODULUS_LEN ((MAX_RSA_MODULUS_BITS + 7) / 8)
#define MAX_RSA_PRIME_BITS ((MAX_RSA_MODULUS_BITS + 1) / 2)
#define MAX_RSA_PRIME_LEN ((MAX_RSA_PRIME_BITS + 7) / 8)

/* Maximum lengths of encoded and encrypted content, as a function of
   content length len. Also, inverse functions.
 */
#define ENCODED_CONTENT_LEN(len) (4*(len)/3 + 3)
#define ENCRYPTED_CONTENT_LEN(len) ENCODED_CONTENT_LEN ((len)+8)
#define DECODED_CONTENT_LEN(len) (3*(len)/4 + 1)
#define DECRYPTED_CONTENT_LEN(len) (DECODED_CONTENT_LEN (len) - 1)

/* Maximum lengths of signatures, encrypted keys, encrypted
   signatures, and message digests.
 */
#define MAX_SIGNATURE_LEN MAX_RSA_MODULUS_LEN
#define MAX_PEM_SIGNATURE_LEN ENCODED_CONTENT_LEN (MAX_SIGNATURE_LEN)
#define MAX_ENCRYPTED_KEY_LEN MAX_RSA_MODULUS_LEN
#define MAX_PEM_ENCRYPTED_KEY_LEN ENCODED_CONTENT_LEN (MAX_ENCRYPTED_KEY_LEN)
#define MAX_PEM_ENCRYPTED_SIGNATURE_LEN \
  ENCRYPTED_CONTENT_LEN (MAX_SIGNATURE_LEN)
#define MAX_DIGEST_LEN 16

/* Maximum length of Diffie-Hellman parameters.
 */
#define DH_PRIME_LEN(bits) (((bits) + 7) / 8)

/* Error codes.
 */
#define RE_CONTENT_ENCODING 0x0400
#define RE_DATA 0x0401
#define RE_DIGEST_ALGORITHM 0x0402
#define RE_ENCODING 0x0403
#define RE_KEY 0x0404
#define RE_KEY_ENCODING 0x0405
#define RE_LEN 0x0406
#define RE_MODULUS_LEN 0x0407
#define RE_NEED_RANDOM 0x0408
#define RE_PRIVATE_KEY 0x0409
#define RE_PUBLIC_KEY 0x040a
#define RE_SIGNATURE 0x040b
#define RE_SIGNATURE_ENCODING 0x040c
#define RE_ENCRYPTION_ALGORITHM 0x040d

/* Random structure.
 */
typedef struct {
  unsigned int bytesNeeded;
  unsigned char state[16];
  unsigned int outputAvailable;
  unsigned char output[16];
} R_RANDOM_STRUCT;

/* RSA public and private key.
 */
typedef struct {
  unsigned int bits;                           /* length in bits of modulus */
  unsigned char modulus[MAX_RSA_MODULUS_LEN];                    /* modulus */
  unsigned char exponent[MAX_RSA_MODULUS_LEN];           /* public exponent */
} R_RSA_PUBLIC_KEY;

typedef struct {
  unsigned int bits;                           /* length in bits of modulus */
  unsigned char modulus[MAX_RSA_MODULUS_LEN];                    /* modulus */
  unsigned char publicExponent[MAX_RSA_MODULUS_LEN];     /* public exponent */
  unsigned char exponent[MAX_RSA_MODULUS_LEN];          /* private exponent */
  unsigned char prime[2][MAX_RSA_PRIME_LEN];               /* prime factors */
  unsigned char primeExponent[2][MAX_RSA_PRIME_LEN];   /* exponents for CRT */
  unsigned char coefficient[MAX_RSA_PRIME_LEN];          /* CRT coefficient */
} R_RSA_PRIVATE_KEY;

/* RSA prototype key.
 */
typedef struct {
  unsigned int bits;                           /* length in bits of modulus */
  int useFermat4;                        /* public exponent (1 = F4, 0 = 3) */
} R_RSA_PROTO_KEY;

/* Diffie-Hellman parameters.
 */
typedef struct {
  unsigned char *prime;                                            /* prime */
  unsigned int primeLen;                                 /* length of prime */
  unsigned char *generator;                                    /* generator */
  unsigned int generatorLen;                         /* length of generator */
} R_DH_PARAMS;
typedef struct {
  unsigned char state[16];                                 /* state */
  unsigned char checksum[16];                           /* checksum */
  unsigned int count;                 /* number of bytes, modulo 16 */
  unsigned char buffer[16];                         /* input buffer */
} MD2_CTX;

/* MD5 context. */
typedef struct {
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  unsigned char buffer[64];                         /* input buffer */
} MD5_CTX;

typedef struct {
  int digestAlgorithm;
  union {
    MD2_CTX md2;
    MD5_CTX md5;
  } context;
} R_DIGEST_CTX;

typedef struct {
  R_DIGEST_CTX digestContext;
} R_SIGNATURE_CTX;


typedef struct {
  UINT4 subkeys[32];                                             /* subkeys */
  UINT4 iv[2];                                       /* initializing vector */
  UINT4 originalIV[2];                        /* for restarting the context */
  int encrypt;                                               /* encrypt flag */
} DES_CBC_CTX;

typedef struct {
  UINT4 subkeys[32];                                             /* subkeys */
  UINT4 iv[2];                                       /* initializing vector */
  UINT4 inputWhitener[2];                                 /* input whitener */
  UINT4 outputWhitener[2];                               /* output whitener */
  UINT4 originalIV[2];                        /* for restarting the context */
  int encrypt;                                              /* encrypt flag */
} DESX_CBC_CTX;

typedef struct {
  UINT4 subkeys[3][32];                     /* subkeys for three operations */
  UINT4 iv[2];                                       /* initializing vector */
  UINT4 originalIV[2];                        /* for restarting the context */
  int encrypt;                                              /* encrypt flag */
} DES3_CBC_CTX;

typedef struct {
  int encryptionAlgorithm;
  union {
    DES_CBC_CTX des;
    DES3_CBC_CTX des3;
    DESX_CBC_CTX desx;
  } cipherContext;
  
  unsigned char buffer[8];
  unsigned int bufferLen;
} R_ENVELOPE_CTX;

//********************************************************************
//输出函数(全局函数)
//********************************************************************
//公钥加密
int RSAPublicEncrypt
  (
unsigned char *output,                                      /* output block */
unsigned int *outputLen,                          /* length of output block */
unsigned char *input,                                        /* input block */
unsigned int inputLen,                             /* length of input block */
R_RSA_PUBLIC_KEY *publicKey,                              /* RSA public key */
R_RANDOM_STRUCT *randomStruct);                          /* random structure */
//公钥解密
int RSAPublicDecrypt (
unsigned char *output,                                      /* output block */
unsigned int *outputLen,                          /* length of output block */
unsigned char *input,                                        /* input block */
unsigned int inputLen,                             /* length of input block */
R_RSA_PUBLIC_KEY *publicKey);                              /* RSA public key */
//私钥加密
int RSAPrivateEncrypt (
unsigned char *output,                                      /* output block */
unsigned int *outputLen,                          /* length of output block */
unsigned char *input,                                        /* input block */
unsigned int inputLen,                             /* length of input block */
R_RSA_PRIVATE_KEY *privateKey);                           /* RSA private key */
//私钥解密
int RSAPrivateDecrypt (
unsigned char *output,                                      /* output block */
unsigned int *outputLen,                          /* length of output block */
unsigned char *input,                                        /* input block */
unsigned int inputLen,                             /* length of input block */
R_RSA_PRIVATE_KEY *privateKey);                           /* RSA private key */

char Load_RSA_PublicKey(void);

extern R_RSA_PUBLIC_KEY RSA_publicKey;                          // new RSA public key 
#endif
