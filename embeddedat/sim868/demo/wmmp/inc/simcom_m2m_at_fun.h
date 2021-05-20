#if defined(__SIMCOM_WMMP__)

#ifndef __SIMCOM_M2M_AT_FUN_H__ 
#define __SIMCOM_M2M_AT_FUN_H__

#include "wmmp_typ.h"

#ifdef SIMCOM_TD_M2M_COMMAND_ENABLE
#define SIMCOM_M2M_MAX_PDPINDEX_NUM  5
#define SIMCOM_M2M_MIN_PDPINDEX_NUM  0
#endif /*SIMCOM_TD_M2M_COMMAND_ENABLE*/

#define LOCAL_PARA_HDR \
   u8	ref_count; \
   u16	msg_len;

#define PEER_BUFF_HDR \
   u16	pdu_len; \
   u8	ref_count; \
   u8   pb_resvered; \
   u16	free_header_space; \
   u16	free_tail_space;

typedef struct 
{
  LOCAL_PARA_HDR
  u8 *senddata;
  u32 datalength;
}wmmp_data_send_req_struct;
typedef struct
{
    LOCAL_PARA_HDR
    bool            result;
}wmmp_data_send_cnf_struct;

typedef enum SIMCOM_WMMP_AT_ENUM_TAG
{
	VG_AT_M2MC_BEGIN, 
		
	VG_AT_M2MC_INIT,
	VG_AT_M2MC_TERM,
	VG_AT_M2MC_CFG,
	VG_AT_M2MC_CFGLIST,

	VG_AT_M2MC_LOGIN,
	VG_AT_M2MC_LOGOUT,
	VG_AT_M2MC_SECURITY,	

	VG_AT_M2MC_STATUS,
	VG_AT_M2MC_TRAP,
	VG_AT_M2MC_REQ,
	VG_AT_M2MC_DATA,
	VG_AT_M2MC_FILE,
	VG_AT_M2MC_MAPPOPEN,
	VG_AT_M2MC_MAPPCLOSE,
	VG_AT_M2MC_MAPPSEND,
	
	VG_AT_M2MC_TLV,
	VG_AT_M2MC_TEST,

	#ifdef SIMCOM_TD_M2M_COMMAND_ENABLE
	VG_AT_M2M_IPSEND,
	VG_AT_M2M_IPENTRANS,
	#endif
	
	VG_AT_M2MC_END
}SIMCOM_WMMP_AT_ENUM;

#ifdef SIMCOM_TD_M2M_COMMAND_ENABLE
typedef struct 
{
   bool limintEnable;
   s32 voltLow;
   s32 voltHigh;
}voltLimit;

typedef struct
{
 bool csSupport;
 bool psSupport;
 bool simLock;
 bool simInsert;
}serverState;


typedef struct 
{
     u8 linkId;
     u32 sockId;
     u16 localPort;
     u16 desPort;
     bool isConnected;
}LinkInfor;

typedef struct
{
    LinkInfor  links[5];
}PDPLinkInfor;

typedef struct 
{
    u8 PDPIndex_currentUse;
    u8 linkId_currentUse;
    PDPLinkInfor pdpLinkInfor[SIMCOM_M2M_MAX_PDPINDEX_NUM+1];
}PDPLinkContext;  


typedef struct 
{
        u8 PDPIndex;
        u8 sockType;
        char desIpAddr[40];   
        u16 desPort;
        u16 localPort;

        u8 linkId;   
}SOCKET_OPEN_STRUCT;

#endif /*SIMCOM_TD_M2M_COMMAND_ENABLE*/

 
 /***************************************************************************
 * External Functions Prototypes
 ***************************************************************************/
	
#endif

#endif /*__SIMCOM_WMMP_PROTOCOL_H__*/
