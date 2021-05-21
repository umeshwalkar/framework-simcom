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
#ifndef _WMMP_DECODE_H_
#define _WMMP_DECODE_H_
#include <wmmp_typ.h> 
WmmpDecodeRecvResult_e RecvCommand(WmmpTransType_e type, u16 length);
#endif
#endif

