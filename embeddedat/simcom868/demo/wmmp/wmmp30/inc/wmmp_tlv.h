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
#ifndef _WMMP_TLV_H_
#define _WMMP_TLV_H_
/*****************************************************************
* Include Files
*****************************************************************/
/*****************************************************************
* Type Definitions
*****************************************************************/





/*****************************************************************
*  Macros
*****************************************************************/

/*****************************************************************
*  Function Prototypes
*****************************************************************/
void ReadTLV(u16 Tag_Id ,TLVRetVal_t* ret);

void WriteTLV(u16 Tag_Id, u16 length, char* value);

//void ClearSecurityTLVParam(u8 Number);

bool CheckTLV(u16 Tag_Id, u16 length, char* value);

bool CheckTLVTag(u16 Tag_Id);

//void vgWMMPWriteNvram (const Anrm2DataName name,u16 commandRef);
void WriteAllTLVParam2Flash(void);

//void CheckTagScopeToWriteFlash(u16 Tag_Id);
#endif
#endif
/***********End of File******************************************/

