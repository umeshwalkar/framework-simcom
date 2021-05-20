/******************************************************************************
*        
*     TPL GSM Phase 2 Application Layer    
*     Copyright (c) 2008 SIMCom Ltd.       
*        
*******************************************************************************
*  file name:          
*  author:             wangzhengning
*  version:            1.00
*  file description:   
*******************************************************************************
*  revision history:    date               version                  author
*
*  change summary:
*
******************************************************************************/
#if defined(__MODEM_WMMP30_SIMCOM__)
#ifndef _WMMP_CRC_H_
#define _WMMP_CRC_H_

typedef union{
char m_ch[2];
u16 m_data;
} U16_INT;

typedef union{
	char m_ch[4];
	u32 m_data;
} U32_INT;

u32 CreateCRC32(char *databuf,u32 len);
char byAddCRC32(char *databuf,u32 len);
char byChkCRC32(char *databuf,u32 len);
u16 CreateCRC16(char* msg, u16 length);

#endif
#endif

