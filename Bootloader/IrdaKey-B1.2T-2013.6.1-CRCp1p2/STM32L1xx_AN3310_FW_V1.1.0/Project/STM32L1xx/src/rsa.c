// RsaTest1.cpp : Defines the entry point for the console application.
//

#include <include.h>
#include "rsa.h"

//公司共钥
/*
unsigned char const Gold_Public_Key[68]={
0xC4, 0xD2, 0x0B, 0x56, 0xB5, 0x93, 0xE7, 0x84, 0x3B, 0x40,
0xE2, 0x87, 0xA8, 0x19 ,0x8A ,0x9F ,0x5D ,0x7D ,0xF5 ,0x6A,
0xEC ,0x2F ,0x7E ,0x79 ,0xD2 ,0xD8 ,0xE7 ,0xD8 ,0x10 ,0x7E,
0x1B ,0x86 ,0x28 ,0x68 ,0x53 ,0x45 ,0xBD ,0x4B ,0x37 ,0x9E,
0x75 ,0xD2 ,0xAF ,0x82 ,0xC8 ,0x7C ,0x5D ,0xF0 ,0xCE ,0xC3,
0x53 ,0x7A ,0x04 ,0x21 ,0xF1 ,0xD7 ,0xA0 ,0xC8 ,0xC2 ,0xED,
0x83 ,0x68 ,0xFF ,0x11 ,0x01 ,0x00 ,0x01, 0xC8
};
*/

//公司共钥,游戏厅正式产品用
unsigned char const Gold_Public_Key[68]={
0xBE,0x71,0x0C,0x3C,0xA3,0x9A,0x7B,0x23,0x4A,0x08,
0xC2,0x8D,0x09,0x6A,0xA7,0x46,0xDB,0x46,0xBC,0xF6,
0x84,0xA0,0x41,0xD6,0x2E,0x59,0x21,0xF4,0x3E,0x0E,
0x9F,0x75,0x01,0x5A,0xBD,0x72,0xAC,0xB0,0xB1,0x35,
0x3C,0xA6,0x8A,0xD4,0x3E,0x49,0x30,0x02,0x00,0xA4,
0x41,0xFF,0x0E,0x5B,0x3D,0xD5,0xF5,0x30,0x97,0x71,
0x65,0x98,0x9D,0x15,0x01,0x00,0x01,0x5F
};

/*
//测试用
unsigned char const Gold_Public_Key[68]={
0xB1,0xD5,0x6C,0xF4,0x7C,0x2D,0x68,0x0D,0x2B,0x67, 
0x1D,0xC2,0x41,0x13,0x8B,0xCD,0x06,0x27,0x50,0x60,
0x97,0xC7,0xCA,0x2C,0x5A,0x42,0x01,0xBA,0xFF,0x4E,
0x09,0x20,0x26,0xEA,0x92,0x20,0x71,0x3D,0x8F,0xAC,
0x26,0x96,0x5F,0xA4,0x99,0x99,0xEA,0x17,0xBB,0x38, 
0xFE,0x92,0xCE,0xCF,0x8B,0x4E,0x7F,0x2B,0x48,0x1A,
0x64,0xB2,0x2B,0x61,0x01,0x00,0x01
};
*/

//#include "Winbase.h"
/* R_KEYGEN.C - key-pair generation for RSAREF
 */

/* Copyright (C) RSA Laboratories, a division of RSA Data Security,
     Inc., created 1991. All rights reserved.
 */

/*#include "global.h"
#include "rsaref.h"
#include "r_random.h"
#include "nn.h"
#include "prime.h"*/
/* GLOBAL.H - RSAREF types and constants

	Copyright (C) RSA Laboratories, a division of RSA Data Security,
     Inc., created 1991. All rights reserved.
 */

#ifndef _GLOBAL_H_
#define _GLOBAL_H_ 1

/* PROTOTYPES should be set to one if and only if the compiler supports
     function argument prototyping.
   The following makes PROTOTYPES default to 1 if it has not already been
     defined as 0 with C compiler flags.
 */
#ifndef PROTOTYPES
#define PROTOTYPES 1
#endif


#ifndef NULL_PTR
#define NULL_PTR ((POINTER)0)
#endif

#ifndef UNUSED_ARG
#define UNUSED_ARG(x) x = *(&x);
#endif

/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.
   If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
     returns an empty list.  
 */
#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif

#endif /* end _GLOBAL_H_ */

/* RSAREF.H - header file for RSAREF cryptographic toolkit
 */

/* Copyright (C) RSA Laboratories, a division of RSA Data Security,
     Inc., created 1991. All rights reserved.
 */

/* Random structures.
 */
int R_RandomInit PROTO_LIST ((R_RANDOM_STRUCT *));
int R_RandomUpdate PROTO_LIST
  ((R_RANDOM_STRUCT *, unsigned char *, unsigned int));
int R_GetRandomBytesNeeded PROTO_LIST ((unsigned int *, R_RANDOM_STRUCT *));
void R_RandomFinal PROTO_LIST ((R_RANDOM_STRUCT *));

/* Cryptographic procedures "by parts"
 */
int R_DigestInit PROTO_LIST ((R_DIGEST_CTX *, int));
int R_DigestUpdate PROTO_LIST
  ((R_DIGEST_CTX *, unsigned char *, unsigned int));
int R_DigestFinal PROTO_LIST
  ((R_DIGEST_CTX *, unsigned char *, unsigned int *));

int R_SignInit PROTO_LIST ((R_SIGNATURE_CTX *, int));
int R_SignUpdate PROTO_LIST
  ((R_SIGNATURE_CTX *, unsigned char *, unsigned int));
int R_SignFinal PROTO_LIST
  ((R_SIGNATURE_CTX *, unsigned char *, unsigned int *, R_RSA_PRIVATE_KEY *));

int R_VerifyInit PROTO_LIST ((R_SIGNATURE_CTX *, int));
int R_VerifyUpdate PROTO_LIST
  ((R_SIGNATURE_CTX *, unsigned char *, unsigned int));
int R_VerifyFinal PROTO_LIST
  ((R_SIGNATURE_CTX *, unsigned char *, unsigned int, R_RSA_PUBLIC_KEY *));

int R_SealInit PROTO_LIST
  ((R_ENVELOPE_CTX *, unsigned char **, unsigned int *, unsigned char [8],
    unsigned int, R_RSA_PUBLIC_KEY **, int, R_RANDOM_STRUCT *));
int R_SealUpdate PROTO_LIST
  ((R_ENVELOPE_CTX *, unsigned char *, unsigned int *, unsigned char *,
    unsigned int));
int R_SealFinal PROTO_LIST
  ((R_ENVELOPE_CTX *, unsigned char *, unsigned int *));

int R_OpenInit PROTO_LIST
  ((R_ENVELOPE_CTX *, int, unsigned char *, unsigned int, unsigned char [8],
    R_RSA_PRIVATE_KEY *));
int R_OpenUpdate PROTO_LIST
  ((R_ENVELOPE_CTX *, unsigned char *, unsigned int *, unsigned char *,
    unsigned int));
int R_OpenFinal PROTO_LIST
  ((R_ENVELOPE_CTX *, unsigned char *, unsigned int *));

/* Cryptographic enhancements by block.
 */
int R_SignPEMBlock PROTO_LIST
  ((unsigned char *, unsigned int *, unsigned char *, unsigned int *,
    unsigned char *, unsigned int, int, int, R_RSA_PRIVATE_KEY *));
int R_SignBlock PROTO_LIST 
  ((unsigned char *, unsigned int *, unsigned char *, unsigned int, int,
    R_RSA_PRIVATE_KEY *));
int R_VerifyPEMSignature PROTO_LIST
  ((unsigned char *, unsigned int *, unsigned char *, unsigned int,
    unsigned char *, unsigned int, int, int, R_RSA_PUBLIC_KEY *));
int R_VerifyBlockSignature PROTO_LIST
  ((unsigned char *, unsigned int, unsigned char *, unsigned int, int,
    R_RSA_PUBLIC_KEY *));
int R_SealPEMBlock PROTO_LIST
  ((unsigned char *, unsigned int *, unsigned char *, unsigned int *,
    unsigned char *, unsigned int *, unsigned char [8], unsigned char *,
    unsigned int, int, R_RSA_PUBLIC_KEY *, R_RSA_PRIVATE_KEY *,
    R_RANDOM_STRUCT *));
int R_OpenPEMBlock PROTO_LIST 
  ((unsigned char *, unsigned int *, unsigned char *, unsigned int,
    unsigned char *, unsigned int, unsigned char *, unsigned int,
    unsigned char [8], int, R_RSA_PRIVATE_KEY *, R_RSA_PUBLIC_KEY *));
int R_DigestBlock PROTO_LIST 
  ((unsigned char *, unsigned int *, unsigned char *, unsigned int, int));

/* Printable ASCII encoding and decoding.
 */
int R_EncodePEMBlock PROTO_LIST
  ((unsigned char *, unsigned int *, unsigned char *, unsigned int));
int R_DecodePEMBlock PROTO_LIST
  ((unsigned char *, unsigned int *, unsigned char *, unsigned int));
  
/* Key-pair generation.
 */
int R_GeneratePEMKeys PROTO_LIST
  ((R_RSA_PUBLIC_KEY *, R_RSA_PRIVATE_KEY *, R_RSA_PROTO_KEY *,
    R_RANDOM_STRUCT *));

/* Diffie-Hellman key agreement.
 */
int R_GenerateDHParams PROTO_LIST
  ((R_DH_PARAMS *, unsigned int, unsigned int, R_RANDOM_STRUCT *));
int R_SetupDHAgreement PROTO_LIST
  ((unsigned char *, unsigned char *, unsigned int, R_DH_PARAMS *,
    R_RANDOM_STRUCT *));
int R_ComputeDHAgreedKey PROTO_LIST
  ((unsigned char *, unsigned char *, unsigned char *, unsigned int,
    R_DH_PARAMS *));

/* Routines supplied by the implementor.
 */



/* PRIME.C - primality-testing routines
 */

/* Copyright (C) RSA Laboratories, a division of RSA Data Security,
     Inc., created 1991. All rights reserved.
 */
/*
#include "global.h"
#include "rsaref.h"
#include "r_random.h"
#include "nn.h"
#include "prime.h"*/

static unsigned int SMALL_PRIMES[] = { 3, 5, 7, 11 };
#define SMALL_PRIME_COUNT 4
typedef UINT4 NN_DIGIT;
typedef UINT2 NN_HALF_DIGIT;

static int ProbablePrime PROTO_LIST ((NN_DIGIT *, unsigned int));
static int SmallFactor PROTO_LIST ((NN_DIGIT *, unsigned int));
static int FermatTest PROTO_LIST ((NN_DIGIT *, unsigned int));

R_RSA_PUBLIC_KEY RSA_publicKey;                          // new RSA public key 

/* Generates a probable prime a between b and c such that a-1 is
   divisible by d.

   Lengths: a[digits], b[digits], c[digits], d[digits].
   Assumes b < c, digits < MAX_NN_DIGITS.
   
   Returns RE_NEED_RANDOM if randomStruct not seeded, RE_DATA if
   unsuccessful.
 */
/* Length of digit in bits */
#define NN_DIGIT_BITS 32
#define NN_HALF_DIGIT_BITS 16
/* Length of digit in bytes */
#define NN_DIGIT_LEN (NN_DIGIT_BITS / 8)
/* Maximum length in digits */
#define MAX_NN_DIGITS \
  ((MAX_RSA_MODULUS_LEN + NN_DIGIT_LEN - 1) / NN_DIGIT_LEN + 1)
/* Maximum digits */
#define MAX_NN_DIGIT 0xffffffff
#define MAX_NN_HALF_DIGIT 0xffff

/* Macros.
 */
#define LOW_HALF(x) ((x) & MAX_NN_HALF_DIGIT)
#define HIGH_HALF(x) (((x) >> NN_HALF_DIGIT_BITS) & MAX_NN_HALF_DIGIT)
#define TO_HIGH_HALF(x) (((NN_DIGIT)(x)) << NN_HALF_DIGIT_BITS)
#define DIGIT_MSB(x) (unsigned int)(((x) >> (NN_DIGIT_BITS - 1)) & 1)
#define DIGIT_2MSB(x) (unsigned int)(((x) >> (NN_DIGIT_BITS - 2)) & 3)

/* R_RANDOM.C - random objects for RSAREF
 */

/* Copyright (C) RSA Laboratories, a division of RSA Data Security,
     Inc., created 1991. All rights reserved.
 */


#define RANDOM_BYTES_NEEDED 256
void R_memset (
POINTER output,                                             /* output block */
int value,                                                         /* value */
unsigned int len)                                        /* length of block */
{
  if (len)
    memset (output, value, len);
}

void R_memcpy (
POINTER output,                                             /* output block */
POINTER input,                                               /* input block */
unsigned int len)                                       /* length of blocks */
{
  if (len)
    memcpy (output, input, len);
}

int R_memcmp (
POINTER firstBlock,                                          /* first block */
POINTER secondBlock,                                        /* second block */
unsigned int len)                                       /* length of blocks */
{
  if (len)
    return (memcmp (firstBlock, secondBlock, len));
  else
    return (0);
}
int R_RandomInit (R_RANDOM_STRUCT *randomStruct)
                      /* new random structure */
{
  randomStruct->bytesNeeded = RANDOM_BYTES_NEEDED;
  R_memset ((POINTER)randomStruct->state, 0, sizeof (randomStruct->state));
  randomStruct->outputAvailable = 0;
  
  return (0);
}
/* MD5C.C - RSA Data Security, Inc., MD5 message-digest algorithm
 */

/* Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
   rights reserved.

   License to copy and use this software is granted provided that it
   is identified as the "RSA Data Security, Inc. MD5 Message-Digest
   Algorithm" in all material mentioning or referencing this software
   or this function.

   License is also granted to make and use derivative works provided
   that such works are identified as "derived from the RSA Data
   Security, Inc. MD5 Message-Digest Algorithm" in all material
   mentioning or referencing the derived work.  
                                                                    
   RSA Data Security, Inc. makes no representations concerning either
   the merchantability of this software or the suitability of this
   software for any particular purpose. It is provided "as is"
   without express or implied warranty of any kind.  
                                                                    
   These notices must be retained in any copies of any part of this
   documentation and/or software.  
 */


/* Constants for MD5Transform routine.
 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

static void MD5Transform PROTO_LIST ((UINT4 [4], unsigned char [64]));
static void Encode PROTO_LIST
  ((unsigned char *, UINT4 *, unsigned int));
static void Decode PROTO_LIST
  ((UINT4 *, unsigned char *, unsigned int));
static void MD5_memcpy PROTO_LIST ((POINTER, POINTER, unsigned int));
static void MD5_memset PROTO_LIST ((POINTER, int, unsigned int));

static unsigned char PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
   Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
    (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) { \
    (a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
    (a) += H ((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
    (a) += I ((b), (c), (d)) + (x) + (UINT4)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
  }

/* MD5 initialization. Begins an MD5 operation, writing a new context.
 */
void MD5Init (MD5_CTX *context)
                                       /* context */
{
  context->count[0] = context->count[1] = 0;

  /* Load magic initialization constants.
   */
  context->state[0] = 0x67452301;
  context->state[1] = 0xefcdab89;
  context->state[2] = 0x98badcfe;
  context->state[3] = 0x10325476;
}

/* MD5 block update operation. Continues an MD5 message-digest
     operation, processing another message block, and updating the
     context.
 */
void MD5Update (
MD5_CTX *context,                                        /* context */
unsigned char *input,                                /* input block */
unsigned int inputLen)                     /* length of input block */
{
  unsigned int i, index, partLen;

  /* Compute number of bytes mod 64 */
  index = (unsigned int)((context->count[0] >> 3) & 0x3F);

  /* Update number of bits */
  if ((context->count[0] += ((UINT4)inputLen << 3))
      < ((UINT4)inputLen << 3))
    context->count[1]++;
  context->count[1] += ((UINT4)inputLen >> 29);
  
  partLen = 64 - index;
  
  /* Transform as many times as possible.
   */
  if (inputLen >= partLen) {
    MD5_memcpy
      ((POINTER)&context->buffer[index], (POINTER)input, partLen);
    MD5Transform (context->state, context->buffer);
  
    for (i = partLen; i + 63 < inputLen; i += 64)
      MD5Transform (context->state, &input[i]);
    
    index = 0;
  }
  else
    i = 0;
  
  /* Buffer remaining input */
  MD5_memcpy 
    ((POINTER)&context->buffer[index], (POINTER)&input[i],
     inputLen-i);
}

/* MD5 finalization. Ends an MD5 message-digest operation, writing the
     the message digest and zeroizing the context.
 */
void MD5Final (
unsigned char digest[16],                         /* message digest */
MD5_CTX *context)                                       /* context */
{
  unsigned char bits[8];
  unsigned int index, padLen;

  /* Save number of bits */
  Encode (bits, context->count, 8);

  /* Pad out to 56 mod 64.
   */
  index = (unsigned int)((context->count[0] >> 3) & 0x3f);
  padLen = (index < 56) ? (56 - index) : (120 - index);
  MD5Update (context, PADDING, padLen);
  
  /* Append length (before padding) */
  MD5Update (context, bits, 8);

  /* Store state in digest */
  Encode (digest, context->state, 16);
  
  /* Zeroize sensitive information.
   */
  MD5_memset ((POINTER)context, 0, sizeof (*context));
}

/* MD5 basic transformation. Transforms state based on block.
 */
static void MD5Transform (
UINT4 state[4],
unsigned char block[64])
{
  UINT4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];
  
  Decode (x, block, 64);

  /* Round 1 */
  FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
  FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
  FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
  FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
  FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
  FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
  FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
  FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
  FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
  FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
  FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
  FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
  FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
  FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
  FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
  FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

  /* Round 2 */
  GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
  GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
  GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
  GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
  GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
  GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
  GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
  GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
  GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
  GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
  GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
  GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
  GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
  GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
  GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
  GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

  /* Round 3 */
  HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
  HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
  HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
  HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
  HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
  HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
  HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
  HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
  HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
  HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
  HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
  HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
  HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
  HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
  HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
  HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

  /* Round 4 */
  II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
  II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
  II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
  II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
  II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
  II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
  II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
  II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
  II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
  II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
  II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
  II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
  II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
  II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
  II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
  II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;
  
  /* Zeroize sensitive information.
   */
  MD5_memset ((POINTER)x, 0, sizeof (x));
}

/* Encodes input (UINT4) into output (unsigned char). Assumes len is
     a multiple of 4.
 */
static void Encode (
unsigned char *output,
UINT4 *input,
unsigned int len)
{
  unsigned int i, j;

  for (i = 0, j = 0; j < len; i++, j += 4) {
    output[j] = (unsigned char)(input[i] & 0xff);
    output[j+1] = (unsigned char)((input[i] >> 8) & 0xff);
    output[j+2] = (unsigned char)((input[i] >> 16) & 0xff);
    output[j+3] = (unsigned char)((input[i] >> 24) & 0xff);
  }
}

/* Decodes input (unsigned char) into output (UINT4). Assumes len is
     a multiple of 4.
 */
static void Decode (
UINT4 *output,
unsigned char *input,
unsigned int len)
{
  unsigned int i, j;

  for (i = 0, j = 0; j < len; i++, j += 4)
    output[i] = ((UINT4)input[j]) | (((UINT4)input[j+1]) << 8) |
      (((UINT4)input[j+2]) << 16) | (((UINT4)input[j+3]) << 24);
}

/* Note: Replace "for loop" with standard memcpy if possible.
 */
static void MD5_memcpy (
POINTER output,
POINTER input,
unsigned int len)
{
  unsigned int i;
  
  for (i = 0; i < len; i++)
    output[i] = input[i];
}

/* Note: Replace "for loop" with standard memset if possible.
 */
static void MD5_memset (
POINTER output,
int value,
unsigned int len)
{
  unsigned int i;
  
  for (i = 0; i < len; i++)
    ((char *)output)[i] = (char)value;
}

int R_RandomUpdate (
R_RANDOM_STRUCT *randomStruct,                          /* random structure */
unsigned char *block,                          /* block of values to mix in */
unsigned int blockLen)                                   /* length of block */
{
  MD5_CTX context;
  unsigned char digest[16];
  unsigned int i, x;
  
  MD5Init (&context);
  MD5Update (&context, block, blockLen);
  MD5Final (digest, &context);

  /* add digest to state */
  x = 0;
  for (i = 0; i < 16; i++) {
    x += randomStruct->state[15-i] + digest[15-i];
    randomStruct->state[15-i] = (unsigned char)x;
    x >>= 8;
  }
  
  if (randomStruct->bytesNeeded < blockLen)
    randomStruct->bytesNeeded = 0;
  else
    randomStruct->bytesNeeded -= blockLen;
  
  /* Zeroize sensitive information.
   */
  R_memset ((POINTER)digest, 0, sizeof (digest));
  x = 0;
  
  return (0);
}

int R_GetRandomBytesNeeded (
unsigned int *bytesNeeded,                 /* number of mix-in bytes needed */
R_RANDOM_STRUCT *randomStruct)                          /* random structure */
{
  *bytesNeeded = randomStruct->bytesNeeded;
  
  return (0);
}

int R_GenerateBytes (
unsigned char *block,                                              /* block */
unsigned int blockLen,                                   /* length of block */
R_RANDOM_STRUCT *randomStruct)                          /* random structure */
{
  MD5_CTX context;
  unsigned int available, i;
  
  if (randomStruct->bytesNeeded)
    return (RE_NEED_RANDOM);
  
  available = randomStruct->outputAvailable;
  
  while (blockLen > available) {
    R_memcpy
      ((POINTER)block, (POINTER)&randomStruct->output[16-available],
       available);
    block += available;
    blockLen -= available;

    /* generate new output */
    MD5Init (&context);
    MD5Update (&context, randomStruct->state, 16);
    MD5Final (randomStruct->output, &context);
    available = 16;

    /* increment state */
    for (i = 0; i < 16; i++)
      if (randomStruct->state[15-i]++)
        break;
  }

  R_memcpy 
    ((POINTER)block, (POINTER)&randomStruct->output[16-available], blockLen);
  randomStruct->outputAvailable = available - blockLen;

  return (0);
}

void R_RandomFinal (R_RANDOM_STRUCT *randomStruct)
                          /* random structure */
{
  R_memset ((POINTER)randomStruct, 0, sizeof (*randomStruct));
}


/* NN.C - natural numbers routines
 */

/* Copyright (C) RSA Laboratories, a division of RSA Data Security,
     Inc., created 1991. All rights reserved.
 */

/* CHANGES MADE TO THIS FILE UNDER RSAREF 2.0 license clause 1(c):

   For the MIT PGP 2.5 distribution, this file was modified to permit
   replacement of the NN_ModExp routine by an equivalent routine contained 
   in the PGP 2.5 sources.  To enable this change, an #ifdef was added to this 
   file (search for #ifndef USEMPILIB below).  RSAREF *must* be compiled with 
   USEMPILIB defined for this change to occur.

   Change made May 8, 1994.  */


static NN_DIGIT NN_AddDigitMult PROTO_LIST 
  ((NN_DIGIT *, NN_DIGIT *, NN_DIGIT, NN_DIGIT *, unsigned int));
static NN_DIGIT NN_SubDigitMult PROTO_LIST 
  ((NN_DIGIT *, NN_DIGIT *, NN_DIGIT, NN_DIGIT *, unsigned int));

static unsigned int NN_DigitBits PROTO_LIST ((NN_DIGIT));

/* Decodes character string b into a, where character string is ordered
   from most to least significant.

   Lengths: a[digits], b[len].
   Assumes b[i] = 0 for i < len - digits * NN_DIGIT_LEN. (Otherwise most
   significant bytes are truncated.)
 */
void NN_Decode (
NN_DIGIT *a,unsigned int digits, 
unsigned char *b,
unsigned int len)
{
  NN_DIGIT t;
  int j;
  unsigned int i, u;
  
  for (i = 0, j = len - 1; i < digits && j >= 0; i++) {
    t = 0;
    for (u = 0; j >= 0 && u < NN_DIGIT_BITS; j--, u += 8)
      t |= ((NN_DIGIT)b[j]) << u;
    a[i] = t;
  }
  
  for (; i < digits; i++)
    a[i] = 0;
}

/* Encodes b into character string a, where character string is ordered
   from most to least significant.

   Lengths: a[len], b[digits].
   Assumes NN_Bits (b, digits) <= 8 * len. (Otherwise most significant
   digits are truncated.)
 */
void NN_Encode (
unsigned char *a, unsigned int len,NN_DIGIT *b,
unsigned int digits)
{
  NN_DIGIT t;
  int j;
  unsigned int i, u;

  for (i = 0, j = len - 1; i < digits && j >= 0; i++) {
    t = b[i];
    for (u = 0; j >= 0 && u < NN_DIGIT_BITS; j--, u += 8)
      a[j] = (unsigned char)(t >> u);
  }

  for (; j >= 0; j--)
    a[j] = 0;
}

/* Assigns a = b.

   Lengths: a[digits], b[digits].
 */
void NN_Assign (
NN_DIGIT *a, NN_DIGIT *b,
unsigned int digits)
{
  unsigned int i;

  for (i = 0; i < digits; i++)
    a[i] = b[i];
}

/* Assigns a = 0.

   Lengths: a[digits].
 */
void NN_AssignZero (
NN_DIGIT *a,
unsigned int digits)
{
  unsigned int i;

  for (i = 0; i < digits; i++)
    a[i] = 0;
}

/* Assigns a = 2^b.

   Lengths: a[digits].
   Requires b < digits * NN_DIGIT_BITS.
 */
void NN_Assign2Exp (
NN_DIGIT *a,
unsigned int b,unsigned int digits)
{
  NN_AssignZero (a, digits);

  if (b >= digits * NN_DIGIT_BITS)
    return;

  a[b / NN_DIGIT_BITS] = (NN_DIGIT)1 << (b % NN_DIGIT_BITS);
}

/* Computes a = b + c. Returns carry.

   Lengths: a[digits], b[digits], c[digits].
 */
NN_DIGIT NN_Add (
NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c,
unsigned int digits)
{
  NN_DIGIT ai, carry;
  unsigned int i;

  carry = 0;

  for (i = 0; i < digits; i++) {
    if ((ai = b[i] + carry) < carry)
      ai = c[i];
    else if ((ai += c[i]) < c[i])
      carry = 1;
    else
      carry = 0;
    a[i] = ai;
  }

  return (carry);
}

/* Computes a = b - c. Returns borrow.

   Lengths: a[digits], b[digits], c[digits].
 */
NN_DIGIT NN_Sub (
NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c,
unsigned int digits)
{
  NN_DIGIT ai, borrow;
  unsigned int i;

  borrow = 0;

  for (i = 0; i < digits; i++) {
    if ((ai = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
      ai = MAX_NN_DIGIT - c[i];
    else if ((ai -= c[i]) > (MAX_NN_DIGIT - c[i]))
      borrow = 1;
    else
      borrow = 0;
    a[i] = ai;
  }

  return (borrow);
}
/* Returns the significant length of a in digits.

   Lengths: a[digits].
 */
unsigned int NN_Digits (
NN_DIGIT *a,
unsigned int digits)
{
  int i;
  
  for (i = digits - 1; i >= 0; i--)
    if (a[i])
      break;

  return (i + 1);
}
/* Computes a = b * c.

   Lengths: a[2*digits], b[digits], c[digits].
   Assumes digits < MAX_NN_DIGITS.
 */
void NN_Mult (
NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c,
unsigned int digits)
{
  NN_DIGIT t[2*MAX_NN_DIGITS];
  unsigned int bDigits, cDigits, i;

  NN_AssignZero (t, 2 * digits);
  
  bDigits = NN_Digits (b, digits);
  cDigits = NN_Digits (c, digits);

  for (i = 0; i < bDigits; i++)
    t[i+cDigits] += NN_AddDigitMult (&t[i], &t[i], b[i], c, cDigits);
  
  NN_Assign (a, t, 2 * digits);
  
  /* Zeroize potentially sensitive information.
   */
  R_memset ((POINTER)t, 0, sizeof (t));
}

/* Computes a = b * 2^c (i.e., shifts left c bits), returning carry.

   Lengths: a[digits], b[digits].
   Requires c < NN_DIGIT_BITS.
 */
NN_DIGIT NN_LShift (
NN_DIGIT *a, NN_DIGIT *b,
unsigned int c, 
unsigned int digits)
{
  NN_DIGIT bi, carry;
  unsigned int i, t;
  
  if (c >= NN_DIGIT_BITS)
    return (0);
  
  t = NN_DIGIT_BITS - c;

  carry = 0;

  for (i = 0; i < digits; i++) {
    bi = b[i];
    a[i] = (bi << c) | carry;
    carry = c ? (bi >> t) : 0;
  }
  
  return (carry);
}

/* Computes a = c div 2^c (i.e., shifts right c bits), returning carry.

   Lengths: a[digits], b[digits].
   Requires: c < NN_DIGIT_BITS.
 */
NN_DIGIT NN_RShift (
NN_DIGIT *a, NN_DIGIT *b,
unsigned int c, unsigned int digits)
{
  NN_DIGIT bi, carry;
  int i;
  unsigned int t;
  
  if (c >= NN_DIGIT_BITS)
    return (0);
  
  t = NN_DIGIT_BITS - c;

  carry = 0;

  for (i = digits - 1; i >= 0; i--) {
    bi = b[i];
    a[i] = (bi >> c) | carry;
    carry = c ? (bi << t) : 0;
  }
  
  return (carry);
}

/* DIGIT.C - digit arithmetic routines
 */

/* Copyright (C) RSA Laboratories, a division of RSA Data Security,
      Inc., created 1991. All rights reserved.
 */


/* Computes a = b * c, where b and c are digits.

   Lengths: a[2].
 */
void NN_DigitMult (
NN_DIGIT a[2], NN_DIGIT b, NN_DIGIT c)
{
  NN_DIGIT t, u;
  NN_HALF_DIGIT bHigh, bLow, cHigh, cLow;

  bHigh = (NN_HALF_DIGIT)HIGH_HALF (b);
  bLow = (NN_HALF_DIGIT)LOW_HALF (b);
  cHigh = (NN_HALF_DIGIT)HIGH_HALF (c);
  cLow = (NN_HALF_DIGIT)LOW_HALF (c);

  a[0] = (NN_DIGIT)bLow * (NN_DIGIT)cLow;
  t = (NN_DIGIT)bLow * (NN_DIGIT)cHigh;
  u = (NN_DIGIT)bHigh * (NN_DIGIT)cLow;
  a[1] = (NN_DIGIT)bHigh * (NN_DIGIT)cHigh;
  
  if ((t += u) < u)
    a[1] += TO_HIGH_HALF (1);
  u = TO_HIGH_HALF (t);
  
  if ((a[0] += u) < u)
    a[1]++;
  a[1] += HIGH_HALF (t);
}

/* Sets a = b / c, where a and c are digits.

   Lengths: b[2].
   Assumes b[1] < c and HIGH_HALF (c) > 0. For efficiency, c should be
   normalized.
 */
void NN_DigitDiv (
NN_DIGIT *a, NN_DIGIT b[2], NN_DIGIT c)
{
  NN_DIGIT t[2], u, v;
  NN_HALF_DIGIT aHigh, aLow, cHigh, cLow;

  cHigh = (NN_HALF_DIGIT)HIGH_HALF (c);
  cLow = (NN_HALF_DIGIT)LOW_HALF (c);

  t[0] = b[0];
  t[1] = b[1];

  /* Underestimate high half of quotient and subtract.
   */
  if (cHigh == MAX_NN_HALF_DIGIT)
    aHigh = (NN_HALF_DIGIT)HIGH_HALF (t[1]);
  else
    aHigh = (NN_HALF_DIGIT)(t[1] / (cHigh + 1));
  u = (NN_DIGIT)aHigh * (NN_DIGIT)cLow;
  v = (NN_DIGIT)aHigh * (NN_DIGIT)cHigh;
  if ((t[0] -= TO_HIGH_HALF (u)) > (MAX_NN_DIGIT - TO_HIGH_HALF (u)))
    t[1]--;
  t[1] -= HIGH_HALF (u);
  t[1] -= v;

  /* Correct estimate.
   */
  while ((t[1] > cHigh) ||
         ((t[1] == cHigh) && (t[0] >= TO_HIGH_HALF (cLow)))) {
    if ((t[0] -= TO_HIGH_HALF (cLow)) > MAX_NN_DIGIT - TO_HIGH_HALF (cLow))
      t[1]--;
    t[1] -= cHigh;
    aHigh++;
  }

  /* Underestimate low half of quotient and subtract.
   */
  if (cHigh == MAX_NN_HALF_DIGIT)
    aLow = (NN_HALF_DIGIT)LOW_HALF (t[1]);
  else
    aLow = 
      (NN_HALF_DIGIT)((TO_HIGH_HALF (t[1]) + HIGH_HALF (t[0])) / (cHigh + 1));
  u = (NN_DIGIT)aLow * (NN_DIGIT)cLow;
  v = (NN_DIGIT)aLow * (NN_DIGIT)cHigh;
  if ((t[0] -= u) > (MAX_NN_DIGIT - u))
    t[1]--;
  if ((t[0] -= TO_HIGH_HALF (v)) > (MAX_NN_DIGIT - TO_HIGH_HALF (v)))
    t[1]--;
  t[1] -= HIGH_HALF (v);

  /* Correct estimate.
   */
  while ((t[1] > 0) || ((t[1] == 0) && t[0] >= c)) {
    if ((t[0] -= c) > (MAX_NN_DIGIT - c))
      t[1]--;
    aLow++;
  }
  
  *a = TO_HIGH_HALF (aHigh) + aLow;
}
/* Returns sign of a - b.

   Lengths: a[digits], b[digits].
 */
int NN_Cmp (
NN_DIGIT *a, NN_DIGIT *b,
unsigned int digits)
{
  int i;
  
  for (i = digits - 1; i >= 0; i--) {
    if (a[i] > b[i])
      return (1);
    if (a[i] < b[i])
      return (-1);
  }

  return (0);
}
/* Computes a = c div d and b = c mod d.

   Lengths: a[cDigits], b[dDigits], c[cDigits], d[dDigits].
   Assumes d > 0, cDigits < 2 * MAX_NN_DIGITS,
           dDigits < MAX_NN_DIGITS.
 */
void NN_Div (
NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c,unsigned int cDigits,  NN_DIGIT *d,
unsigned int dDigits)
{
  NN_DIGIT ai, cc[2*MAX_NN_DIGITS+1], dd[MAX_NN_DIGITS], t;
  int i;
  unsigned int ddDigits, shift;
  
  ddDigits = NN_Digits (d, dDigits);
  if (ddDigits == 0)
    return;
  
  /* Normalize operands.
   */
  shift = NN_DIGIT_BITS - NN_DigitBits (d[ddDigits-1]);
  NN_AssignZero (cc, ddDigits);
  cc[cDigits] = NN_LShift (cc, c, shift, cDigits);
  NN_LShift (dd, d, shift, ddDigits);
  t = dd[ddDigits-1];
  
  NN_AssignZero (a, cDigits);

  for (i = cDigits-ddDigits; i >= 0; i--) {
    /* Underestimate quotient digit and subtract.
     */
    if (t == MAX_NN_DIGIT)
      ai = cc[i+ddDigits];
    else
      NN_DigitDiv (&ai, &cc[i+ddDigits-1], t + 1);
    cc[i+ddDigits] -= NN_SubDigitMult (&cc[i], &cc[i], ai, dd, ddDigits);

    /* Correct estimate.
     */
    while (cc[i+ddDigits] || (NN_Cmp (&cc[i], dd, ddDigits) >= 0)) {
      ai++;
      cc[i+ddDigits] -= NN_Sub (&cc[i], &cc[i], dd, ddDigits);
    }
    
    a[i] = ai;
  }
  
  /* Restore result.
   */
  NN_AssignZero (b, dDigits);
  NN_RShift (b, cc, shift, ddDigits);

  /* Zeroize potentially sensitive information.
   */
  R_memset ((POINTER)cc, 0, sizeof (cc));
  R_memset ((POINTER)dd, 0, sizeof (dd));
}

/* Computes a = b mod c.

   Lengths: a[cDigits], b[bDigits], c[cDigits].
   Assumes c > 0, bDigits < 2 * MAX_NN_DIGITS, cDigits < MAX_NN_DIGITS.
 */
void NN_Mod (
NN_DIGIT *a, NN_DIGIT *b, unsigned int bDigits, NN_DIGIT *c,
unsigned int cDigits)
{
  NN_DIGIT t[2 * MAX_NN_DIGITS];
  
  NN_Div (t, a, b, bDigits, c, cDigits);
  
  /* Zeroize potentially sensitive information.
   */
  R_memset ((POINTER)t, 0, sizeof (t));
}

/* Computes a = b * c mod d.

   Lengths: a[digits], b[digits], c[digits], d[digits].
   Assumes d > 0, digits < MAX_NN_DIGITS.
 */
void NN_ModMult (
NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_DIGIT *d,
unsigned int digits)
{
  NN_DIGIT t[2*MAX_NN_DIGITS];

  NN_Mult (t, b, c, digits);
  NN_Mod (a, t, 2 * digits, d, digits);
  
  /* Zeroize potentially sensitive information.
   */
  R_memset ((POINTER)t, 0, sizeof (t));
}
#define NN_ASSIGN_DIGIT(a, b, digits) {NN_AssignZero (a, digits); a[0] = b;}
#define NN_EQUAL(a, b, digits) (! NN_Cmp (a, b, digits))
#define NN_EVEN(a, digits) (((digits) == 0) || ! (a[0] & 1))

/* Computes a = b^c mod d.

   Lengths: a[dDigits], b[dDigits], c[cDigits], d[dDigits].
   Assumes d > 0, cDigits > 0, dDigits < MAX_NN_DIGITS.
 */

/* PGP 2.5's mpilib contains a faster modular exponentiation routine, mp_modexp.
   If USEMPILIB is defined, NN_ModExp is replaced in the PGP 2.5 sources with a 
   stub call to mp_modexp.  If USEMPILIB is not defined, we'll get a pure (albeit
   slower) RSAREF implementation.

   The RSAREF 2.0 license, clause 1(c), permits "...modify[ing] the Program in any
   manner for porting or performance improvement purposes..." */

#ifndef USEMPILIB
void NN_ModExp (
NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, unsigned int cDigits, NN_DIGIT *d,
unsigned int dDigits)
{
  NN_DIGIT bPower[3][MAX_NN_DIGITS], ci, t[MAX_NN_DIGITS];
  int i;
  unsigned int ciBits, j, s;

  /* Store b, b^2 mod d, and b^3 mod d.
   */
  NN_Assign (bPower[0], b, dDigits);
  NN_ModMult (bPower[1], bPower[0], b, d, dDigits);
  NN_ModMult (bPower[2], bPower[1], b, d, dDigits);
  
  NN_ASSIGN_DIGIT (t, 1, dDigits);

  cDigits = NN_Digits (c, cDigits);
  for (i = cDigits - 1; i >= 0; i--) {
    ci = c[i];
    ciBits = NN_DIGIT_BITS;
    
    /* Scan past leading zero bits of most significant digit.
     */
    if (i == (int)(cDigits - 1)) {
      while (! DIGIT_2MSB (ci)) {
        ci <<= 2;
        ciBits -= 2;
      }
    }

    for (j = 0; j < ciBits; j += 2, ci <<= 2) {
      /* Compute t = t^4 * b^s mod d, where s = two MSB's of ci.
       */
      NN_ModMult (t, t, t, d, dDigits);
      NN_ModMult (t, t, t, d, dDigits);
      if ((s = DIGIT_2MSB (ci)) != 0)
        NN_ModMult (t, t, bPower[s-1], d, dDigits);
    }
  }
  
  NN_Assign (a, t, dDigits);
  
  /* Zeroize potentially sensitive information.
   */
  R_memset ((POINTER)bPower, 0, sizeof (bPower));
  R_memset ((POINTER)t, 0, sizeof (t));
}
#endif
/* Returns nonzero iff a is zero.

   Lengths: a[digits].
 */
int NN_Zero (
NN_DIGIT *a,
unsigned int digits)
{
  unsigned int i;
  
  for (i = 0; i < digits; i++)
    if (a[i])
      return (0);
    
  return (1);
}

/* Compute a = 1/b mod c, assuming inverse exists.
   
   Lengths: a[digits], b[digits], c[digits].
   Assumes gcd (b, c) = 1, digits < MAX_NN_DIGITS.
 */
void NN_ModInv (
NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c,
unsigned int digits)
{
  NN_DIGIT q[MAX_NN_DIGITS], t1[MAX_NN_DIGITS], t3[MAX_NN_DIGITS],
    u1[MAX_NN_DIGITS], u3[MAX_NN_DIGITS], v1[MAX_NN_DIGITS],
    v3[MAX_NN_DIGITS], w[2*MAX_NN_DIGITS];
  int u1Sign;

  /* Apply extended Euclidean algorithm, modified to avoid negative
     numbers.
   */
  NN_ASSIGN_DIGIT (u1, 1, digits);
  NN_AssignZero (v1, digits);
  NN_Assign (u3, b, digits);
  NN_Assign (v3, c, digits);
  u1Sign = 1;

  while (! NN_Zero (v3, digits)) {
    NN_Div (q, t3, u3, digits, v3, digits);
    NN_Mult (w, q, v1, digits);
    NN_Add (t1, u1, w, digits);
    NN_Assign (u1, v1, digits);
    NN_Assign (v1, t1, digits);
    NN_Assign (u3, v3, digits);
    NN_Assign (v3, t3, digits);
    u1Sign = -u1Sign;
  }
  
  /* Negate result if sign is negative.
    */
  if (u1Sign < 0)
    NN_Sub (a, c, u1, digits);
  else
    NN_Assign (a, u1, digits);

  /* Zeroize potentially sensitive information.
   */
  R_memset ((POINTER)q, 0, sizeof (q));
  R_memset ((POINTER)t1, 0, sizeof (t1));
  R_memset ((POINTER)t3, 0, sizeof (t3));
  R_memset ((POINTER)u1, 0, sizeof (u1));
  R_memset ((POINTER)u3, 0, sizeof (u3));
  R_memset ((POINTER)v1, 0, sizeof (v1));
  R_memset ((POINTER)v3, 0, sizeof (v3));
  R_memset ((POINTER)w, 0, sizeof (w));
}

/* Computes a = gcd(b, c).

   Lengths: a[digits], b[digits], c[digits].
   Assumes b > c, digits < MAX_NN_DIGITS.
 */
void NN_Gcd (
NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c,
unsigned int digits)
{
  NN_DIGIT t[MAX_NN_DIGITS], u[MAX_NN_DIGITS], v[MAX_NN_DIGITS];

  NN_Assign (u, b, digits);
  NN_Assign (v, c, digits);

  while (! NN_Zero (v, digits)) {
    NN_Mod (t, u, digits, v, digits);
    NN_Assign (u, v, digits);
    NN_Assign (v, t, digits);
  }

  NN_Assign (a, u, digits);

  /* Zeroize potentially sensitive information.
   */
  R_memset ((POINTER)t, 0, sizeof (t));
  R_memset ((POINTER)u, 0, sizeof (u));
  R_memset ((POINTER)v, 0, sizeof (v));
}




/* Returns the significant length of a in bits.

   Lengths: a[digits].
 */
unsigned int NN_Bits (
NN_DIGIT *a,
unsigned int digits)
{
  if ((digits = NN_Digits (a, digits)) == 0)
    return (0);

  return ((digits - 1) * NN_DIGIT_BITS + NN_DigitBits (a[digits-1]));
}



/* Computes a = b + c*d, where c is a digit. Returns carry.

   Lengths: a[digits], b[digits], d[digits].
 */
 NN_DIGIT NN_AddDigitMult (
NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT c, NN_DIGIT *d,
unsigned int digits)
{
  NN_DIGIT carry, t[2];
  unsigned int i;

  if (c == 0)
    return (0);

  carry = 0;
  for (i = 0; i < digits; i++) {
    NN_DigitMult (t, c, d[i]);
    if ((a[i] = b[i] + carry) < carry)
      carry = 1;
    else
      carry = 0;
    if ((a[i] += t[0]) < t[0])
      carry++;
    carry += t[1];
  }
  
  return (carry);
}

/* Computes a = b - c*d, where c is a digit. Returns borrow.

   Lengths: a[digits], b[digits], d[digits].
 */
 NN_DIGIT NN_SubDigitMult (
NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT c, NN_DIGIT *d,
unsigned int digits)
{
  NN_DIGIT borrow, t[2];
  unsigned int i;

  if (c == 0)
    return (0);

  borrow = 0;
  for (i = 0; i < digits; i++) {
    NN_DigitMult (t, c, d[i]);
    if ((a[i] = b[i] - borrow) > (MAX_NN_DIGIT - borrow))
      borrow = 1;
    else
      borrow = 0;
    if ((a[i] -= t[0]) > (MAX_NN_DIGIT - t[0]))
      borrow++;
    borrow += t[1];
  }
  
  return (borrow);
}

/* Returns the significant length of a in bits, where a is a digit.
 */
static unsigned int NN_DigitBits (NN_DIGIT a)

{
  unsigned int i;
  
  for (i = 0; i < NN_DIGIT_BITS; i++, a >>= 1)
    if (a == 0)
      break;
    
  return (i);
}


int GeneratePrime (NN_DIGIT *a, NN_DIGIT *b, NN_DIGIT *c, NN_DIGIT *d,unsigned int digits,R_RANDOM_STRUCT *randomStruct)
{
  int status;
  unsigned char block[MAX_NN_DIGITS * NN_DIGIT_LEN];
  NN_DIGIT t[MAX_NN_DIGITS], u[MAX_NN_DIGITS];

  /* Generate random number between b and c.
   */
  //if (status = R_GenerateBytes (block, digits * NN_DIGIT_LEN, randomStruct))
   status = R_GenerateBytes (block, digits * NN_DIGIT_LEN, randomStruct);
	if(status)    return (status);
  NN_Decode (a, digits, block, digits * NN_DIGIT_LEN);
  NN_Sub (t, c, b, digits);
  NN_ASSIGN_DIGIT (u, 1, digits);
  NN_Add (t, t, u, digits);
  NN_Mod (a, a, digits, t, digits);
  NN_Add (a, a, b, digits);

  /* Adjust so that a-1 is divisible by d.
   */
  NN_Mod (t, a, digits, d, digits);
  NN_Sub (a, a, t, digits);
  NN_Add (a, a, u, digits);
  if (NN_Cmp (a, b, digits) < 0)
    NN_Add (a, a, d, digits);
  if (NN_Cmp (a, c, digits) > 0)
    NN_Sub (a, a, d, digits);

  /* Search to c in steps of d.
   */
  NN_Assign (t, c, digits);
  NN_Sub (t, t, d, digits);

  while (! ProbablePrime (a, digits)) {
    if (NN_Cmp (a, t, digits) > 0)
      return (RE_DATA);
    NN_Add (a, a, d, digits);
  }

  return (0);
}

/* Returns nonzero iff a is a probable prime.

   Lengths: a[aDigits].
   Assumes aDigits < MAX_NN_DIGITS.
 */
static int ProbablePrime (
NN_DIGIT *a,
unsigned int aDigits)
{
  return (! SmallFactor (a, aDigits) && FermatTest (a, aDigits));
}

/* Returns nonzero iff a has a prime factor in SMALL_PRIMES.

   Lengths: a[aDigits].
   Assumes aDigits < MAX_NN_DIGITS.
 */
static int SmallFactor (
NN_DIGIT *a,
unsigned int aDigits)
{
  int status;
  NN_DIGIT t[1];
  unsigned int i;
  
  status = 0;
  
  for (i = 0; i < SMALL_PRIME_COUNT; i++) {
    NN_ASSIGN_DIGIT (t, SMALL_PRIMES[i], 1);
    if ((aDigits == 1) && ! NN_Cmp (a, t, 1))
      break;
    NN_Mod (t, a, aDigits, t, 1);
    if (NN_Zero (t, 1)) {
      status = 1;
      break;
    }
  }
  
  /* Zeroize sensitive information.
   */
  i = 0;
  R_memset ((POINTER)t, 0, sizeof (t));

  return (status);
}

/* Returns nonzero iff a passes Fermat's test for witness 2.
   (All primes pass the test, and nearly all composites fail.)
     
   Lengths: a[aDigits].
   Assumes aDigits < MAX_NN_DIGITS.
 */
static int FermatTest (
NN_DIGIT *a,
unsigned int aDigits)
{
  int status;
  NN_DIGIT t[MAX_NN_DIGITS], u[MAX_NN_DIGITS];
  
  NN_ASSIGN_DIGIT (t, 2, aDigits);
  NN_ModExp (u, t, a, aDigits, a, aDigits);
  
  status = NN_EQUAL (t, u, aDigits);
  
  /* Zeroize sensitive information.
   */
  R_memset ((POINTER)u, 0, sizeof (u));
  
  return (status);
}


static int RSAFilter PROTO_LIST
  ((NN_DIGIT *, unsigned int, NN_DIGIT *, unsigned int));
static int RelativelyPrime PROTO_LIST
  ((NN_DIGIT *, unsigned int, NN_DIGIT *, unsigned int));

/* Generates an RSA key pair with a given length and public exponent.
 */
int R_GeneratePEMKeys (
R_RSA_PUBLIC_KEY *publicKey,                          /* new RSA public key */
R_RSA_PRIVATE_KEY *privateKey,                       /* new RSA private key */
R_RSA_PROTO_KEY *protoKey,                             /* RSA prototype key */
R_RANDOM_STRUCT *randomStruct)                          /* random structure */
{
  NN_DIGIT d[MAX_NN_DIGITS], dP[MAX_NN_DIGITS], dQ[MAX_NN_DIGITS],
    e[MAX_NN_DIGITS], n[MAX_NN_DIGITS], p[MAX_NN_DIGITS], phiN[MAX_NN_DIGITS],
    pMinus1[MAX_NN_DIGITS], q[MAX_NN_DIGITS], qInv[MAX_NN_DIGITS],
    qMinus1[MAX_NN_DIGITS], t[MAX_NN_DIGITS], u[MAX_NN_DIGITS],
    v[MAX_NN_DIGITS];
  int status;
  unsigned int nDigits, pBits, pDigits, qBits;
  
  if ((protoKey->bits < MIN_RSA_MODULUS_BITS) || 
      (protoKey->bits > MAX_RSA_MODULUS_BITS))
    return (RE_MODULUS_LEN);
  nDigits = (protoKey->bits + NN_DIGIT_BITS - 1) / NN_DIGIT_BITS;
  pDigits = (nDigits + 1) / 2;
  pBits = (protoKey->bits + 1) / 2;
  qBits = protoKey->bits - pBits;

  /* NOTE: for 65537, this assumes NN_DIGIT is at least 17 bits. */
  NN_ASSIGN_DIGIT
    (e, protoKey->useFermat4 ? (NN_DIGIT)65537 : (NN_DIGIT)3, nDigits);

  /* Generate prime p between 3*2^(pBits-2) and 2^pBits-1, searching
       in steps of 2, until one satisfies gcd (p-1, e) = 1.
   */
  NN_Assign2Exp (t, pBits-1, pDigits);
  NN_Assign2Exp (u, pBits-2, pDigits);
  NN_Add (t, t, u, pDigits);
  NN_ASSIGN_DIGIT (v, 1, pDigits);
  NN_Sub (v, t, v, pDigits);
  NN_Add (u, u, v, pDigits);
  NN_ASSIGN_DIGIT (v, 2, pDigits);
  do {
		status = GeneratePrime (p, t, u, v, pDigits, randomStruct);
    if (status)  return (status);
  }
  while (! RSAFilter (p, pDigits, e, 1));
  
  /* Generate prime q between 3*2^(qBits-2) and 2^qBits-1, searching
       in steps of 2, until one satisfies gcd (q-1, e) = 1.
   */
  NN_Assign2Exp (t, qBits-1, pDigits);
  NN_Assign2Exp (u, qBits-2, pDigits);
  NN_Add (t, t, u, pDigits);
  NN_ASSIGN_DIGIT (v, 1, pDigits);
  NN_Sub (v, t, v, pDigits);
  NN_Add (u, u, v, pDigits);
  NN_ASSIGN_DIGIT (v, 2, pDigits);
  do {
  	status = GeneratePrime (q, t, u, v, pDigits, randomStruct);
    if (status)  return (status);
  }
  while (! RSAFilter (q, pDigits, e, 1));
  
  /* Sort so that p > q. (p = q case is extremely unlikely.)
   */
  if (NN_Cmp (p, q, pDigits) < 0) {
    NN_Assign (t, p, pDigits);
    NN_Assign (p, q, pDigits);
    NN_Assign (q, t, pDigits);
  }

  /* Compute n = pq, qInv = q^{-1} mod p, d = e^{-1} mod (p-1)(q-1),
     dP = d mod p-1, dQ = d mod q-1.
   */
  NN_Mult (n, p, q, pDigits);
  NN_ModInv (qInv, q, p, pDigits);
  
  NN_ASSIGN_DIGIT (t, 1, pDigits);
  NN_Sub (pMinus1, p, t, pDigits);
  NN_Sub (qMinus1, q, t, pDigits);
  NN_Mult (phiN, pMinus1, qMinus1, pDigits);

  NN_ModInv (d, e, phiN, nDigits);
  NN_Mod (dP, d, nDigits, pMinus1, pDigits);
  NN_Mod (dQ, d, nDigits, qMinus1, pDigits);
  
  publicKey->bits = privateKey->bits = protoKey->bits;
  NN_Encode (publicKey->modulus, MAX_RSA_MODULUS_LEN, n, nDigits);
  NN_Encode (publicKey->exponent, MAX_RSA_MODULUS_LEN, e, 1);
  R_memcpy 
    ((POINTER)privateKey->modulus, (POINTER)publicKey->modulus,
     MAX_RSA_MODULUS_LEN);
  R_memcpy
    ((POINTER)privateKey->publicExponent, (POINTER)publicKey->exponent,
     MAX_RSA_MODULUS_LEN);
  NN_Encode (privateKey->exponent, MAX_RSA_MODULUS_LEN, d, nDigits);
  NN_Encode (privateKey->prime[0], MAX_RSA_PRIME_LEN, p, pDigits);
  NN_Encode (privateKey->prime[1], MAX_RSA_PRIME_LEN, q, pDigits);
  NN_Encode (privateKey->primeExponent[0], MAX_RSA_PRIME_LEN, dP, pDigits);
  NN_Encode (privateKey->primeExponent[1], MAX_RSA_PRIME_LEN, dQ, pDigits);
  NN_Encode (privateKey->coefficient, MAX_RSA_PRIME_LEN, qInv, pDigits);
   
  /* Zeroize sensitive information.
   */
  R_memset ((POINTER)d, 0, sizeof (d));
  R_memset ((POINTER)dP, 0, sizeof (dP));
  R_memset ((POINTER)dQ, 0, sizeof (dQ));
  R_memset ((POINTER)p, 0, sizeof (p));
  R_memset ((POINTER)phiN, 0, sizeof (phiN));
  R_memset ((POINTER)pMinus1, 0, sizeof (pMinus1));
  R_memset ((POINTER)q, 0, sizeof (q));
  R_memset ((POINTER)qInv, 0, sizeof (qInv));
  R_memset ((POINTER)qMinus1, 0, sizeof (qMinus1));
  R_memset ((POINTER)t, 0, sizeof (t));
  
  return (0);
}

/* Returns nonzero iff GCD (a-1, b) = 1.

   Lengths: a[aDigits], b[bDigits].
   Assumes aDigits < MAX_NN_DIGITS, bDigits < MAX_NN_DIGITS.
 */
static int RSAFilter (
NN_DIGIT *a, 
unsigned int bDigits,
NN_DIGIT *b,
unsigned int aDigits)
{
  int status;
  NN_DIGIT aMinus1[MAX_NN_DIGITS], t[MAX_NN_DIGITS];
  
  NN_ASSIGN_DIGIT (t, 1, aDigits);
  NN_Sub (aMinus1, a, t, aDigits);
  
  status = RelativelyPrime (aMinus1, aDigits, b, bDigits);

  /* Zeroize sensitive information.
   */
  R_memset ((POINTER)aMinus1, 0, sizeof (aMinus1));
  
  return (status);
}

/* Returns nonzero iff a and b are relatively prime.

   Lengths: a[aDigits], b[bDigits].
   Assumes aDigits >= bDigits, aDigits < MAX_NN_DIGITS.
 */
static int RelativelyPrime (
NN_DIGIT *a, unsigned int aDigits, NN_DIGIT  *b,
unsigned int bDigits)
{
  int status;
  NN_DIGIT t[MAX_NN_DIGITS], u[MAX_NN_DIGITS];
  
  NN_AssignZero (t, aDigits);
  NN_Assign (t, b, bDigits);
  NN_Gcd (t, a, t, aDigits);
  NN_ASSIGN_DIGIT (u, 1, aDigits);

  status = NN_EQUAL (t, u, aDigits);
  
  /* Zeroize sensitive information.
   */
  R_memset ((POINTER)t, 0, sizeof (t));
  
  return (status);
}


/* RSA.C - RSA routines for RSAREF
 */

/* Copyright (C) RSA Laboratories, a division of RSA Data Security,
     Inc., created 1991. All rights reserved.
 */


 int RSAPublicBlock PROTO_LIST 
  ((unsigned char *, unsigned int *, unsigned char *, unsigned int,
    R_RSA_PUBLIC_KEY *));
 int RSAPrivateBlock PROTO_LIST 
  ((unsigned char *, unsigned int *, unsigned char *, unsigned int,
    R_RSA_PRIVATE_KEY *));

/* RSA public-key encryption, according to PKCS #1.
 */
int RSAPublicEncrypt
  (
unsigned char *output,                                      /* output block */
unsigned int *outputLen,                          /* length of output block */
unsigned char *input,                                        /* input block */
unsigned int inputLen,                             /* length of input block */
R_RSA_PUBLIC_KEY *publicKey,                              /* RSA public key */
R_RANDOM_STRUCT *randomStruct)                          /* random structure */
{
  int status;
  unsigned char byte, pkcsBlock[MAX_RSA_MODULUS_LEN];
  unsigned int i, modulusLen;
  
  modulusLen = (publicKey->bits + 7) / 8;
  if (inputLen + 11 > modulusLen)
    return (RE_LEN);
  
  pkcsBlock[0] = 0;
  /* block type 2 */
  pkcsBlock[1] = 2;

  for (i = 2; i < modulusLen - inputLen - 1; i++) {
    /* Find nonzero random byte.
     */
    do {
      R_GenerateBytes (&byte, 1, randomStruct);
    } while (byte == 0);
    pkcsBlock[i] = byte;
  }
  /* separator */
  pkcsBlock[i++] = 0;
  
  R_memcpy ((POINTER)&pkcsBlock[i], (POINTER)input, inputLen);
  
  status = RSAPublicBlock
    (output, outputLen, pkcsBlock, modulusLen, publicKey);
  
  /* Zeroize sensitive information.
   */
  byte = 0;
  R_memset ((POINTER)pkcsBlock, 0, sizeof (pkcsBlock));
  
  return (status);
}

/* RSA public-key decryption, according to PKCS #1.
 */
int RSAPublicDecrypt (
unsigned char *output,                                      /* output block */
unsigned int *outputLen,                          /* length of output block */
unsigned char *input,                                        /* input block */
unsigned int inputLen,                             /* length of input block */
R_RSA_PUBLIC_KEY *publicKey)                              /* RSA public key */
{
  int status;
  unsigned char pkcsBlock[MAX_RSA_MODULUS_LEN];
  unsigned int i, modulusLen, pkcsBlockLen;
  
  modulusLen = (publicKey->bits + 7) / 8;
  if (inputLen > modulusLen)
    return (RE_LEN);
  status = RSAPublicBlock(pkcsBlock, &pkcsBlockLen, input, inputLen, publicKey);
  if (status)    return (status);
  
  if (pkcsBlockLen != modulusLen)
    return (RE_LEN);
  
  /* Require block type 1.
   */
   //以下韩永修改，以适应PC机加密算法
  if ((pkcsBlock[0] != 1))
   return (RE_DATA);
	i=1; 
  	*outputLen = modulusLen - i;
  
  R_memcpy ((POINTER)output, (POINTER)&pkcsBlock[i], *outputLen);
  
  /* Zeroize potentially sensitive information.
   */
  R_memset ((POINTER)pkcsBlock, 0, sizeof (pkcsBlock));
  
  return (0);
}

/* RSA private-key encryption, according to PKCS #1.
 */
int RSAPrivateEncrypt (
unsigned char *output,                                      /* output block */
unsigned int *outputLen,                          /* length of output block */
unsigned char *input,                                        /* input block */
unsigned int inputLen,                             /* length of input block */
R_RSA_PRIVATE_KEY *privateKey)                           /* RSA private key */
{
  int status;
  unsigned char pkcsBlock[MAX_RSA_MODULUS_LEN];
  unsigned int i, modulusLen;
  
  modulusLen = (privateKey->bits + 7) / 8;
  if (inputLen + 11 > modulusLen)
    return (RE_LEN);
  
  pkcsBlock[0] = 0;
  /* block type 1 */
  pkcsBlock[1] = 1;

  for (i = 2; i < modulusLen - inputLen - 1; i++)
    pkcsBlock[i] = 0xff;

  /* separator */
  pkcsBlock[i++] = 0;
  
  R_memcpy ((POINTER)&pkcsBlock[i], (POINTER)input, inputLen);
  
  status = RSAPrivateBlock
    (output, outputLen, pkcsBlock, modulusLen, privateKey);

  /* Zeroize potentially sensitive information.
   */
  R_memset ((POINTER)pkcsBlock, 0, sizeof (pkcsBlock));

  return (status);
}

/* RSA private-key decryption, according to PKCS #1.
 */
int RSAPrivateDecrypt (
unsigned char *output,                                      /* output block */
unsigned int *outputLen,                          /* length of output block */
unsigned char *input,                                        /* input block */
unsigned int inputLen,                             /* length of input block */
R_RSA_PRIVATE_KEY *privateKey)                           /* RSA private key */
{
  int status;
  unsigned char pkcsBlock[MAX_RSA_MODULUS_LEN];
  unsigned int i, modulusLen, pkcsBlockLen;
  
  modulusLen = (privateKey->bits + 7) / 8;
  if (inputLen > modulusLen)
    return (RE_LEN);
  status = RSAPrivateBlock(pkcsBlock, &pkcsBlockLen, input, inputLen, privateKey);
  if (status)   return (status);
  
  if (pkcsBlockLen != modulusLen)
    return (RE_LEN);
  
  /* Require block type 2.
   */
  if ((pkcsBlock[0] != 0) || (pkcsBlock[1] != 2))
   return (RE_DATA);

  for (i = 2; i < modulusLen-1; i++)
    /* separator */
    if (pkcsBlock[i] == 0)
      break;
    
  i++;
  if (i >= modulusLen)
    return (RE_DATA);
    
  *outputLen = modulusLen - i;
  
  if (*outputLen + 11 > modulusLen)
    return (RE_DATA);

  R_memcpy ((POINTER)output, (POINTER)&pkcsBlock[i], *outputLen);
  
  /* Zeroize sensitive information.
   */
  R_memset ((POINTER)pkcsBlock, 0, sizeof (pkcsBlock));
  
  return (0);
}

/* Raw RSA public-key operation. Output has same length as modulus.

   Assumes inputLen < length of modulus.
   Requires input < modulus.
 */
 int RSAPublicBlock (
unsigned char *output,                                      /* output block */
unsigned int *outputLen,                          /* length of output block */
unsigned char *input,                                        /* input block */
unsigned int inputLen,                             /* length of input block */
R_RSA_PUBLIC_KEY *publicKey)                              /* RSA public key */
{
  NN_DIGIT c[MAX_NN_DIGITS], e[MAX_NN_DIGITS], m[MAX_NN_DIGITS],
    n[MAX_NN_DIGITS];
  unsigned int eDigits, nDigits;

  NN_Decode (m, MAX_NN_DIGITS, input, inputLen);
  NN_Decode (n, MAX_NN_DIGITS, publicKey->modulus, MAX_RSA_MODULUS_LEN);
  NN_Decode (e, MAX_NN_DIGITS, publicKey->exponent, MAX_RSA_MODULUS_LEN);
  nDigits = NN_Digits (n, MAX_NN_DIGITS);
  eDigits = NN_Digits (e, MAX_NN_DIGITS);
  
  if (NN_Cmp (m, n, nDigits) >= 0)
    return (RE_DATA);
  
  /* Compute c = m^e mod n.
   */
  NN_ModExp (c, m, e, eDigits, n, nDigits);

  *outputLen = (publicKey->bits + 7) / 8;
  NN_Encode (output, *outputLen, c, nDigits);
  
  /* Zeroize sensitive information.
   */
  R_memset ((POINTER)c, 0, sizeof (c));
  R_memset ((POINTER)m, 0, sizeof (m));

  return (0);
}

/* Raw RSA private-key operation. Output has same length as modulus.

   Assumes inputLen < length of modulus.
   Requires input < modulus.
 */
static int RSAPrivateBlock (
unsigned char *output,                                      /* output block */
unsigned int *outputLen,                          /* length of output block */
unsigned char *input,                                        /* input block */
unsigned int inputLen,                             /* length of input block */
R_RSA_PRIVATE_KEY *privateKey)                           /* RSA private key */
{
  NN_DIGIT c[MAX_NN_DIGITS], cP[MAX_NN_DIGITS], cQ[MAX_NN_DIGITS],
    dP[MAX_NN_DIGITS], dQ[MAX_NN_DIGITS], mP[MAX_NN_DIGITS],
    mQ[MAX_NN_DIGITS], n[MAX_NN_DIGITS], p[MAX_NN_DIGITS], q[MAX_NN_DIGITS],
    qInv[MAX_NN_DIGITS], t[MAX_NN_DIGITS];
  unsigned int cDigits, nDigits, pDigits;
  
  NN_Decode (c, MAX_NN_DIGITS, input, inputLen);
  NN_Decode (n, MAX_NN_DIGITS, privateKey->modulus, MAX_RSA_MODULUS_LEN);
  NN_Decode (p, MAX_NN_DIGITS, privateKey->prime[0], MAX_RSA_PRIME_LEN);
  NN_Decode (q, MAX_NN_DIGITS, privateKey->prime[1], MAX_RSA_PRIME_LEN);
  NN_Decode 
    (dP, MAX_NN_DIGITS, privateKey->primeExponent[0], MAX_RSA_PRIME_LEN);
  NN_Decode 
    (dQ, MAX_NN_DIGITS, privateKey->primeExponent[1], MAX_RSA_PRIME_LEN);
  NN_Decode (qInv, MAX_NN_DIGITS, privateKey->coefficient, MAX_RSA_PRIME_LEN);
  cDigits = NN_Digits (c, MAX_NN_DIGITS);
  nDigits = NN_Digits (n, MAX_NN_DIGITS);
  pDigits = NN_Digits (p, MAX_NN_DIGITS);

  if (NN_Cmp (c, n, nDigits) >= 0)
    return (RE_DATA);
  
  /* Compute mP = cP^dP mod p  and  mQ = cQ^dQ mod q. (Assumes q has
     length at most pDigits, i.e., p > q.)
   */
  NN_Mod (cP, c, cDigits, p, pDigits);
  NN_Mod (cQ, c, cDigits, q, pDigits);
  NN_ModExp (mP, cP, dP, pDigits, p, pDigits);
  NN_AssignZero (mQ, nDigits);
  NN_ModExp (mQ, cQ, dQ, pDigits, q, pDigits);
  
  /* Chinese Remainder Theorem:
       m = ((((mP - mQ) mod p) * qInv) mod p) * q + mQ.
   */
  if (NN_Cmp (mP, mQ, pDigits) >= 0)
    NN_Sub (t, mP, mQ, pDigits);
  else {
    NN_Sub (t, mQ, mP, pDigits);
    NN_Sub (t, p, t, pDigits);
  }
  NN_ModMult (t, t, qInv, p, pDigits);
  NN_Mult (t, t, q, pDigits);
  NN_Add (t, t, mQ, nDigits);

  *outputLen = (privateKey->bits + 7) / 8;
  NN_Encode (output, *outputLen, t, nDigits);

  // Zeroize sensitive information.
  R_memset ((POINTER)c, 0, sizeof (c));
  R_memset ((POINTER)cP, 0, sizeof (cP));
  R_memset ((POINTER)cQ, 0, sizeof (cQ));
  R_memset ((POINTER)dP, 0, sizeof (dP));
  R_memset ((POINTER)dQ, 0, sizeof (dQ));
  R_memset ((POINTER)mP, 0, sizeof (mP));
  R_memset ((POINTER)mQ, 0, sizeof (mQ));
  R_memset ((POINTER)p, 0, sizeof (p));
  R_memset ((POINTER)q, 0, sizeof (q));
  R_memset ((POINTER)qInv, 0, sizeof (qInv));
  R_memset ((POINTER)t, 0, sizeof (t));

  return (0);
}
/*******************************************************************************
* Function Name  : Load_RSA_PublicKey
* Description    : 加载RSA公钥
* Input          : None
* Output         : None
* Return         : FLASH_OK:成功;other：失败
*******************************************************************************/
char Load_RSA_PublicKey(void)
{

unsigned int i;
//unsigned char mybuffer[70];
unsigned char check;

//	读取RSA公钥

	check=0;
    for (i=0;i<67;i++)
 	  {
   		check	+=	Gold_Public_Key[i];
   	  }
	if(check==Gold_Public_Key[i])
			{
				RSA_publicKey.bits=512;
				for(i=0;i<MAX_RSA_MODULUS_LEN;i++)
						RSA_publicKey.modulus[i]	=Gold_Public_Key[i];
			
				for(i=0;i<MAX_RSA_MODULUS_LEN;i++)
						RSA_publicKey.exponent[i]	=0;
			
				RSA_publicKey.exponent[MAX_RSA_MODULUS_LEN-1]	=Gold_Public_Key[66];
				RSA_publicKey.exponent[MAX_RSA_MODULUS_LEN-3]	=Gold_Public_Key[64];
				return CMD_OK;	
			}
	else
			{
				RSA_publicKey.bits=0;
				for(i=0;i<MAX_RSA_MODULUS_LEN;i++)
						RSA_publicKey.modulus[i]	=0xff;
			
				for(i=0;i<MAX_RSA_MODULUS_LEN;i++)
						RSA_publicKey.exponent[i]	=0xff;
				return CMD_ERROR;
			}

}


