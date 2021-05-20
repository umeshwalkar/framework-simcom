#ifndef _SIMCOM_WMMP_UTILITY_H
#define _SIMCOM_WMMP_UTILITY_H

#if defined(__SIMCOM_EAT_WMMP__)


/******************************************************************************
* Include Statements
******************************************************************************/

/******************************************************************************
* Include Files
******************************************************************************/
//#ifdef __MOD_SMSAL__
//#include "l4c2smsal_struct.h"
//#endif

//#include "custom_data_account.h"
//#include "l4c_common_enum.h"
#include "simcom_wmmp_nvram_define.h"
//#include "soc_consts.h" //for IP_ADDR_LEN
#include "wmmp_basic_type.h"

/******************************************************************************
* Macros
******************************************************************************/
#define WMMP_MIN_VALUE(a,b) ((a)<(b))? (a):(b)


#define	MAX_SIMCOM_WMMP_EVENT_TIMER	100

#define WMMP_APP_INFO_STRING_BASE            (7991)
#define WMMP_APP_INFO_ICON_BASE               (8991)

#define MAX_APPWMMPTCPIP_DNS_NAME_LEN       100

/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  midify begin*/
#ifdef __SIMCOM_WMMP_PRIVATE_APN__
#define MAX_SIMCOM_WMMP_CONTEXT_NUM         	3
#else
#define MAX_SIMCOM_WMMP_CONTEXT_NUM         	2
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  modify end*/
#define SIMCOM_WMMP_CSD_ACCOUNT_START      	0
#define SIMCOM_WMMP_GPRS_ACCOUNT_START    	CUSTOM_DEFINED_MAX_CSD_PROFILE_NUM
#define SIMCOM_WMMP_GPRS_CONTEXT_ID           	1  /*it MUST equals SIMCOM_WMMP_GPRS_ACCOUNT_START - CUSTOM_DEFINED_MAX_CSD_PROFILE_NUM + 1*/
#define SIMCOM_WMMP_SERVER_SOCKET          		0
#define MAX_SIMCOM_WMMP_CLIENT_NUM         	5 /*this is for server to listen to client*/
#define SIMCOM_WMMP_CLIENT_SOCKET          		1  /*only for single tcpip connection*/
#define MAX_SIMCOM_WMMP_SOCKET_NUM         	6  /*The maximum number of socket allowed to create, at present, it is 6*/ 

#define SIMCOM_WMMP_BUFFER_SIZE            		(0x1000)  /*the buffer size of the send buffer and receive buffer*/
#define SIMCOM_WMMP_BUFFER_HWT             		(0x9B8)  /*The high wait value of the buffer*/ /*APPTCPIP_BUFFER_SIZE-1460-MAX_APPTCPIP_HEADER_SIZE*/
#define SIMCOM_WMMP_BUFFER_LWT             		(0x600)   /*The low wait value of the buffer*/
#define MAX_SIMCOM_WMMP_HEADER_SIZE        	(100)     /*The max header size for the received data*/
#define MAX_SIMCOM_WMMP_PDU_SIZE           		(1024)    
#define SIMCOM_WMMP_WAIT_INTERVAL          		(100)


#define	IMEI_ID_ASCII_LEN						16

#define  IP_ADDR_LEN    (4)    

/******************************************************************************
* Types
******************************************************************************/
typedef enum WmmpIpConfigTypeTag
{
    WMMP_IP_CSD_CONFIG,
    WMMP_IP_GPRS_CONFIG   
}WmmpIpConfigType_e;


typedef struct 
{
//    ActivateWmmpTCPIPState      state;
//    WmmpContextState_e  contextState;
    u8               app_id;
    u32              accout_id;
//    u32              obtainAccountId;
//    l4c_source_id_enum      entity;
    s16            addrLen;
    u8               ip_addr[IP_ADDR_LEN]; /*local ip address*/
    
    u16              tcpPort;  /*local port for tcp server*/
    u16              udpPort;  /*local port for udp server*/
    WmmpIpConfigType_e          bearerType;

    Wmmp_mmi_abm_account_info_struct *p_account;//wmmp account infor    
}WmmpNetWorkContext_t;

typedef enum WMMPAPNTypeTag
{
    WMMP_APN_UNKNOWN   = 0,
    WMMP_APN_CMNET,
    WMMP_APN_CMWAP, 
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_PRIVATE_APN__
    WMMP_APN_PRIVATE, 
#endif
 /*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
   WMMP_APN_MAX
}WMMPAPNType;


typedef enum AppPONStepTag
{
    WMMP_PON_INIT   = 0,
    WMMP_PON_NVRAM_RDY,
    WMMP_PON_REG_SEARCHING, /*找网*/
    WMMP_PON_SMS_BEGIN,
    WMMP_PON_SMS_RDY,
    WMMP_PON_MAX
}AppPONStep;

typedef enum WmmpATEventTypeTag
{
    WMMP_AET_INVALID,
    WMMP_AET_LIB_STATE = 1,
    WMMP_AET_SET_TAG = 2,
    WMMP_AET_REMOTE_CTRL = 3,
    WMMP_AET_TRANSPARENT_DATA =4,
    WMMP_AET_URL = 6,
    WMMP_AET_GET_TAG = 10,
    WMMP_AET_LIB_INITIALIZED = 20,
    WMMP_AET_LIB_TERMINATED = 21,
    WMMP_AET_LIB_REGISTERED = 22,
    WMMP_AET_LIB_LOG_IN = 23,
    WMMP_AET_LIB_LOG_OUT = 24,
    WMMP_AET_BEARER_TYPE = 25,
    WMMP_AET_BEARER_LINK = 26,
    WMMP_AET_TRANSPARENT_CMD_ACK = 30,
    WMMP_AET_TRANS_ID = 31,    
    WMMP_AET_MAX

}WmmpATEventType;
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_ADC_GPIO__
typedef enum GPIO_dir
{
  GPIO_IN=0,
  GPIO_OUT
}GPIO_dir;
typedef enum GPIO_Status
{
  LOW=0,
  HIGH
}GPIO_Status;
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/

typedef struct
{
	AppPONStep power_on_step; 
	WmmpNetWorkContext_t    networkContext[MAX_SIMCOM_WMMP_CONTEXT_NUM];
	//AppWmmpNetWorkInfo 	nw_info;	
}AppWmmpContext_t;



/*SIMCOM zhengwei for MKBug00015002 20130121*//*begin*/
/* maobin@20130306 del
typedef struct 
{
    LOCAL_PARA_HDR
}WmmpGprsConfReq;
*/
typedef struct 
{
    /*LOCAL_PARA_HDR maobin@20130306 del*/
    u8  connectType;
    u32 ipAddr;
    u16 ipPort;
    u8 connRef;
}WmmpGprsCreateConnectReq;

typedef struct
{
    /*LOCAL_PARA_HDR maobin@20130306 del*/
    void *  buff;
    u16 len;
    u8 connRef;
}WmmpGprsSendDataReq;
/*SIMCOM zhengwei for MKBug00015002 20130121*//*end*/

typedef	struct{
/* maobin@20130306 del
	l4csmsal_send_cnf_struct	orgL4csmsalsendcnf;
*/
	bool result;   	
   	u16 cause;
  	 u8 src_id;
}WmmpSmsSendedRspStruct;

typedef struct WMMPLogoutReqTag
{
// TaskId     task;
// unsigned int     task;
 u8		reason;
}WMMPLogoutReq;

typedef struct
{
    s32 sendlen; //本次发送的数据长度
    u8 socket; //使用的socket id
}WmmpAppSendNext_t;

extern AppWmmpContext_t* ptrToAppWmmpContext (void);

typedef	void (*WPAStatusIndFucPtr)(u16,u16);
typedef	void (*WPATrapAckFucPtr)(u8,u16);
typedef	void (*WPAReqAckFucPtr)(u8,u16);
typedef	void (*WPADataAckFucPtr)(u8);
typedef	void (*WPADownNotifyFucPtr)(char *);
typedef	void (*WPAFileAckFucPtr)(char *,char *,u32,u16,char *,u8);
typedef	void (*WPAServerDataReqFucPtr)(char ,char *,char *,u16);
typedef	void (*WPAAppRecvFucPtr)(char *,u16);
#endif	/* __SIMCOM_WMMP__ */

#endif  /*_SIMCOM_WMMP_UTILITY_H*/
