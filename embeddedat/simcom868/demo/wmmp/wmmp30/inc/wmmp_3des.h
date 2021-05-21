#if defined(__MODEM_WMMP30_SIMCOM__)
#ifndef _3DES_H_
#define _3DES_H_

typedef struct
{
    u32 esk[32];     /* DES encryption subkeys */
    u32 dsk[32];     /* DES decryption subkeys */
}
DES_CONTEXT;

typedef struct
{
    u32 esk[96];     /* Triple-DES encryption subkeys */
    u32 dsk[96];     /* Triple-DES decryption subkeys */
}
DES3_CONTEXT;

u16 m2m_3des_set_2key(const u8 *key);
u32 m2m_3des_encrypt(const u8 *input, u32 ilen, u8 *output, u32 olen);
bool m2m_3des_decrypt(const u8 *input, u8 *output, u32 len);

#endif
#endif
