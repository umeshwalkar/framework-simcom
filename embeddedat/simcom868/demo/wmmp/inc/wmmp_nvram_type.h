/*****************************************************************************
*  Copyright Statement:
*  --------------------

*****************************************************************************
 *
 * Filename:
 * ---------
 * wmmp_nvram_type.h
 *
 * Project:
 * --------
 *   SIM800
 *
 * Description:
 * ------------
 *   This file is intends for ¡K.
 *
 * Author: 
 * -------
 * -------
 *
 *============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * removed!
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

/********************************************************************************
*  Copyright Statement:
*  --------------------
*
*  This product has been developed using a protocol stack
*  developed by SIMCOM Communication Technologies Limited.
*
********************************************************************************/
#ifndef _WMMP_NVRAM_TYPE_H
#define _WMMP_NVRAM_TYPE_H


#ifdef __SIMCOM_WMMP__

/******************************************************************************
* Include Files
******************************************************************************/
#ifndef _MMI2ABM_STRUCT_H
//#include "mmi2abm_struct.h"
#endif

#include "wmmp_typ.h"


/******************************************************************************
* Macros
******************************************************************************/
#define MAX_GPRS_USER_NAME_LEN 32
#define MAX_GPRS_PASSWORD_LEN  32
#define MAX_GPRS_APN_LEN       100
#define MAX_GPRS_IP_ADDR       4 /* IP address length */

/******************************************************************************
* Types
******************************************************************************/
    
	
typedef struct {
    char user_id[32];
    char user_pw[32];    
    char addr_str[23];
    char csd_ur;
    char csd_type;
    char csd_module;
} Wmmp_csd_account_info_struct;

typedef struct {
    char user_name[MAX_GPRS_USER_NAME_LEN];
    char password[MAX_GPRS_PASSWORD_LEN];    
    char apn[MAX_GPRS_APN_LEN];
    char apn_length;
    char dcomp_algo; // Fixed value: SND_NO_DCOMP
    char hcomp_algo; // Fixed value: SND_NO_PCOMP
    char pdp_type; // Fixed value: IPV4_ADDR_TYPE 0x21 for IPv4
    char pdp_addr_len; // Fixed value: NULL_PDP_ADDR_LEN = 0x01, if use fixed IP address, this should be IPV4_ADDR_LEN = 0x04
    char pdp_addr_val[MAX_GPRS_IP_ADDR]; // should be all 0
} Wmmp_ps_account_info_struct;

typedef struct {
  char authentication_type;
  char dns[MAX_GPRS_IP_ADDR];
  char sec_dns[MAX_GPRS_IP_ADDR];
  Wmmp_ps_account_info_struct ps_account_info;
  Wmmp_csd_account_info_struct csd_account_info;  
} Wmmp_mmi_abm_account_info_struct;

typedef struct {
  char wopenValueNv;     /*gwopenValue*/
  //char RemoteCtrlFlagNv;  /*gRemoteCtrlFlag*/ 
  //char  rev1;
  //char rev2;
} Wmmp_atc_value_struct;



/******************************************************************************
* Constants
******************************************************************************/

#define		NVRAM_EF_WMMP_TLV_CFG_LID_VERNO	"001"
#define		NVRAM_EF_WMMP_TLV_CFG_SIZE			sizeof(WmmpCfgContext_t) //1000
#define		NVRAM_EF_WMMP_TLV_CFG_TOTAL		1


#define		NVRAM_EF_WMMP_TLV_UPDATE_LID_VERNO	"001"
#define		NVRAM_EF_WMMP_TLV_UPDATE_SIZE			sizeof(WmmpUpdateContext_t)   //1000
#define		NVRAM_EF_WMMP_TLV_UPDATE_TOTAL		1

#define		NVRAM_EF_WMMP_TLV_STAT_LID_VERNO		"001"
#define		NVRAM_EF_WMMP_TLV_STAT_SIZE			sizeof(WmmpStatisticContext_t)  //1000
#define		NVRAM_EF_WMMP_TLV_STAT_TOTAL			1

#define		NVRAM_EF_WMMP_TLV_STATUS_LID_VERNO	"001"
#define		NVRAM_EF_WMMP_TLV_STATUS_SIZE			sizeof(WmmpTermStatusContext_t)  //1000
#define		NVRAM_EF_WMMP_TLV_STATUS_TOTAL		1

#define		NVRAM_EF_WMMP_TLV_CTRL_LID_VERNO		"001"
#define		NVRAM_EF_WMMP_TLV_CTRL_SIZE			sizeof(WmmpControlContext_t)  //1000
#define		NVRAM_EF_WMMP_TLV_CTRL_TOTAL			1

#define		NVRAM_EF_WMMP_TLV_SECURITY_LID_VERNO		"001"
#define		NVRAM_EF_WMMP_TLV_SECURITY_SIZE			sizeof(WmmpSecurityContext_t)   //1000
#define		NVRAM_EF_WMMP_TLV_SECURITY_TOTAL			1

#define		NVRAM_EF_WMMP_TLV_CUSTOM_LID_VERNO		"001"
#define		NVRAM_EF_WMMP_TLV_CUSTOM_SIZE				sizeof(WmmpCustomContext_t)  //1000
#define		NVRAM_EF_WMMP_TLV_CUSTOM_TOTAL			1

#define		NVRAM_EF_WMMP_TLV_CUSTOM2_LID_VERNO		"001"
#define		NVRAM_EF_WMMP_TLV_CUSTOM2_SIZE				sizeof(WmmpCustom2Context_t)  //1000
#define		NVRAM_EF_WMMP_TLV_CUSTOM2_TOTAL			1

#define		NVRAM_EF_WMMP_TLV_CUSTOM3_LID_VERNO		"001"
#define		NVRAM_EF_WMMP_TLV_CUSTOM3_SIZE				sizeof(WmmpCustom3Context_t)  //1000
#define		NVRAM_EF_WMMP_TLV_CUSTOM3_TOTAL			1

#define NVRAM_EF_WMMP_ACCOUNT_LID_VERNO   "001"
#define NVRAM_EF_WMMP_ACCOUNT_SIZE         (sizeof(Wmmp_mmi_abm_account_info_struct))
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  modify begin*/
#ifdef __SIMCOM_WMMP_PRIVATE_APN__
#define NVRAM_EF_WMMP_ACCOUNT_TOTAL        3
#else
#define NVRAM_EF_WMMP_ACCOUNT_TOTAL        2
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  modify begin*/
#define NVRAM_EF_WMMP_WOPEN_LID_VERNO   "001"
#define NVRAM_EF_WMMP_WOPEN_SIZE         (sizeof(Wmmp_atc_value_struct))
#define NVRAM_EF_WMMP_WOPEN_TOTAL        1






/******************************************************************************
* Variables (Extern ¡¢Global and Static)
******************************************************************************/

/******************************************************************************
* Process
******************************************************************************/
#endif /*__SIMCOM_WMMP__*/
#endif /*_WMMP_NVRAM_TYPE_H*/




