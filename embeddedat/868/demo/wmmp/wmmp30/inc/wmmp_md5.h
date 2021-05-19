/**************************************************************************
 * TTPCom Software Copyright (c) 1997-2005 TTPCom Ltd
 * Licensed to Simcom Ltd
 **************************************************************************
 *   $Id: //central/releases/baseline/Branch_release_16.2.0/tplgsm/utinc/ut_md5.h#1 $
 *   $Revision: #1 $
 *   $DateTime: 2007/04/11 12:59:07 $
 **************************************************************************
 * File Description :
 *
 * Header for MD5 algorithm, based on example in RFC1321.  RSA disclaimer:
 *
 *
 * Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 * rights reserved.
 *
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function.
 *
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 *
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 *
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 **************************************************************************/
#if defined(__MODEM_WMMP30_SIMCOM__)
#if !defined (WMMP_MD5_H)
#define       WMMP_MD5_H

/******************************************************************************
 * Include Files
 *****************************************************************************/


/******************************************************************************
 * Types
 *****************************************************************************/

/* MD5 context. */
typedef struct Md5ContextTag
{
    u32 state[4];        /* state (ABCD) */
    u32 count[2];        /* number of bits, modulo 2^64 (lsb first) */
    u8  buffer[64];      /* input buffer */
}
Md5Context;

/* Length of a digest */
#define MD5_DIGEST_LENGTH 16

#define MD5_BLOCK_LENGTH 64

/******************************************************************************
 * Function Prototypes
 *****************************************************************************/
/*
void Md5Init   (Md5Context *context);
void Md5Update (Md5Context *context, u8 *input, u32 inputLength);
void Md5Final  (u8 *digest, Md5Context *context);
*/
void wmmp_md5(u8 *data, u32 datalen, unsigned char *result);

#endif
#endif
/* END OF FILE */
