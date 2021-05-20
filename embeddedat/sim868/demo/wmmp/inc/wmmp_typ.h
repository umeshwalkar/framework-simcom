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
#if defined(__SIMCOM_WMMP__)
#ifndef _WMMP_TYP_H_
#define _WMMP_TYP_H_
/*****************************************************************
* Include Files
*****************************************************************/
#include "wmmp_basic_type.h"


/*****************************************************************
* Type Definitions
*****************************************************************/

#define WMMP_AT_EVENT_STR "$M2MMSG: "
#define WMMP_AT_EVENT_ACK_STR "$M2MACK: "
#define WMMP_APP_EVENT_MSG_STR "$M2M$MSG: "

#define WMMP_APP_ORIGINAL_ACCOUNT_ID  (0x3e50)

#define	MAX_STAT_NUM						10
#define 	MAX_IO_BUFFER 						(3000) 	/*1500 to 3000  for test */
#define   MAX_SERVER_ADDR       				50         //255
#define 	MAX_WMMP_SOCKET_NUM      		8
#define	MAX_WMMP_APP_NUM			      (MAX_WMMP_SOCKET_NUM-1)
#define	MAX_BHV_MODE						6
#define	TLV_M2MDEVSTATUS_LEN				3		/*0x300f*/
/*bob modify 20110212*/
//#define	TLV_M2MDEVSTATUS_LEN				5		/*0x300f*/ 
#define	TLV_SUPPORTEDDEV_LEN				6		/*0x3010*/
#define	MAX_PARAM_NUMBER				32
#define	GENERAL_STR_PARAM_LEN			50		/*100*/
#define	MIN_STR_PARAM_LEN				25
#define	MAX_DEV_NUM						10

//M2MAPPSEND max data length by net
#define WMMP_MAX_NET_DATA_SIZE 10240
#define WMMP_MAX_NET_BUFFER_SIZE (WMMP_MAX_NET_DATA_SIZE+2)
#define WMMP_BLOCK_BUFFER_MAX 10240 /*统一内存块大小，用于超过2K的内存分配*/

#define	MAX_APPSEND_LEN				WMMP_MAX_NET_DATA_SIZE 
//通过网络接口，一次发送的最大数据量，这样组成tcp/udp包时不需要再次分包
#define	MAX_SEND_BSD_LEN				950	

#define 	MAX_HEX_UART_OUTPUT				(AKI_MAX_SIZE_MOBI_MIS_DATA_TO_SEND_REQ/2-1)
#define	MAX_BLOCK_SIZE_WMMP_FILEDL		800

#define	MIN_STR_RMCTRL_TRACE_AT_NamParm		20
#define	MAX_STR_RMCTRL_TRACE_AT_NamParm		40

#define	MIN_STR_RMCTRL_TRACE_AT					60
#define	MID_STR_RMCTRL_TRACE_AT					100
#define	MAX_STR_RMCTRL_TRACE_AT					500

#define INVALID_SOCKET  (0xFFFFFFFFL)
#define SOCKET_ERROR    (0xFFFFFFFFL)

typedef enum wmmp_data_send_enum_tag
{
	WMMP_SMS_SEND_ENUM,
	WMMP_SOCKET_SEND_ENUM,
	WMMP_DATA_SEND_NUM
}wmmp_data_send_enum;

/* FuncPtr is a pointer to a func returns void */
typedef void (*ExpiryFuncPtr) (void);

typedef enum WmmpTransTypeTag
{
	WMMP_TRANS_GPRS,
	WMMP_TRANS_SMS,
	WMMP_TRANS_DEFAULT   //根据0x4002来选择
}WmmpTransType_e;


typedef enum WmmpSocketTypeTag
{
  WMMP_TCP_CONNECT,
  WMMP_UDP_CONNECT
}WmmpSocketType_e;


typedef enum WmmpTLVNameTag
{
  WMMP_TLV_CFG,
  WMMP_TLV_UPDATE,
  WMMP_TLV_STAT,
  WMMP_TLV_STATUS,
  WMMP_TLV_CTRL,
  WMMP_TLV_SECURITY,
  WMMP_TLV_CUSTOM,
  WMMP_TLV_CUSTOM2,  
}WmmpTLVName_e;

typedef enum WmmSecurityOperStateTag
{
	WMMP_SECURITY_OPER_INIT = 0,
	WMMP_SECURITY_OPER_AUTO,
	WMMP_SECURITY_OPER_CHANGE,  
	WMMP_SECURITY_OPER_ENABLE,
	WMMP_SECURITY_OPER_DISABLE
}WmmSecurityOperState_e;

typedef enum WmmSecurityOperErrorTag
{
	WMMP_SECURITY_OPER_ERROR_OK = 0,
	WMMP_SECURITY_OPER_ERROR_ALREADY,
	WMMP_SECURITY_OPER_ERROR_CONTROL_FAIL,
	WMMP_SECURITY_OPER_ERROR_CPIN_FAIL,//解pin失败
	WMMP_SECURITY_OPER_ERROR_CPIN_LIMIT,//密码到了极限
	WMMP_SECURITY_OPER_ERROR_CPIN_NOPIN,//不需要解pin状态
	WMMP_SECURITY_OPER_ERROR_CPIN_PUK,//PUK不能解pin
	WMMP_SECURITY_OPER_ERROR_SIM_NO_READY,//sim卡没有好，无法关闭，开启修改。
	WMMP_SECURITY_OPER_ERROR_CHANGE_FAIL,//修改pin码错误
	WMMP_SECURITY_OPER_ERROR_ENABLE_FAIL,
	WMMP_SECURITY_OPER_ERROR_DISABLE_FAIL,
	WMMP_SECURITY_OPER_ERROR_OTHER
}WmmSecurityOperError_e;

typedef struct WPITrapAckEventTag
{
	u8 ret;
	u16 tag;
}WPITrapAckEvent_t;

typedef struct WPIReqAckEventTag
{
	u8 ret;
	u16 tag;
}WPIReqAckEvent_t;

typedef struct WPIDataAckEventTag
{
	u8 ret;
}WPIDataAckEvent_t;

typedef struct WPIFileAckEventTag
{
	char* data;
	char* CRC16;
	u32 readpoint;
	u16 blocksize;
	char transID[4];
	u8 status;
}WPIFileAckEvent_t;

typedef struct WPISetReqEventTag
{
	u8 ret;
	u16 *tags;
	u8 tagnum;
}WPISetReqEvent_t;

typedef struct WPIRemoteReqEventTag
{
	u8 ret;
	u8 paramId;
	u16 *errortag;
	u8 errortagnum;
}WPIRemoteReqEvent_t;

typedef struct WPIDownReqEventTag
{
	char* url;
	u8 ret;
}WPIDownReqEvent_t;

typedef struct WPIDataReqEventTag
{
	char type;
	char des[20];
	char *data;
	u16 datalen;
}WPIDataReqEvent_t;

typedef struct WPISocketCreateEventTag
{
	u32 socket;
	WmmpSocketType_e type;
}WPISocketCreateEvent_t;

typedef struct WPIAppSockReadEventTag
{
	char *data;
	u16 datalength;
    u16 linkindex; //maobin@20120606 add 用于多路链接
}WPIAppSockReadEvent_t;

typedef struct WPIStatusIndEventTag
{
	u8 type;
	u16 value;
	u16 reason;
}WPIStatusIndEvent_t;

typedef union WPIEventDataTag
{
/*
	WPIRegAckEvent_t WPIRegAckEvent;
	WPILoginAckEvent_t WPILoginAckEvent;*/
	WPITrapAckEvent_t WPITrapAckEvent;
	WPIReqAckEvent_t WPIReqAckEvent;
	WPIDataAckEvent_t WPIDataAckEvent;
	WPIFileAckEvent_t WPIFileAckEvent;
	WPISetReqEvent_t WPISetReqEvent;
	WPIRemoteReqEvent_t WPIRemoteReqEvent;
	WPIDownReqEvent_t WPIDownReqEvent;
	WPIDataReqEvent_t WPIDataReqEvent;
//	WPILogoutAckEvent_t WPILogoutAckEvent;
	WPISocketCreateEvent_t WPISocketCreateEvent;
	WPIAppSockReadEvent_t WPIAppSockReadEvent;
	WPIStatusIndEvent_t	WPIStatusIndEvent;
}WPIEventData_t;

typedef enum WPIEventTypeTag
{
	//WPI_REG_ACK_EVENT,
	//WPI_LOGIN_ACK_EVENT,
	WPI_REQ_ACK_EVENT,
	WPI_TRAP_ACK_EVENT,
	WPI_DATA_ACK_EVENT,
	WPI_FILE_ACK_EVENT,
	WPI_SET_REQ_EVENT,/*2*/
	WPI_REMOTE_REQ_EVENT,
	WPI_DOWN_REQ_EVENT,
	WPI_DATA_REQ_EVENT,
	//WPI_LOGOUT_ACK_EVENT,
	WPI_SOCKET_CREATE_EVENT,
	WPI_APPSOCKET_READ_EVENT,
	WPI_STATUS_IND_EVENT,/*10*/
	WPI_APPSOCKET_CREATE_EVENT,
	NUM_OF_WPI_EVENT
}WPIEventType;

typedef enum WPAEventTypeTag
{
	WPA_SOCKET_CONNECT_EVENT ,
	//WPA_SOCKET_READ_EVENT ,  
	WPA_SOCKET_CLOSE_EVENT ,     
	WPA_SOCKET_DESTROY_EVENT  ,   
	WPA_GPRS_ACTIVE_EVENT  ,    
	WPA_GPRS_DEACTIVE_EVENT,      
	WPA_SMS_SENDED_EVENT ,   
	/*
	WPA_PDU_DECODE_EVENT ,  
	WPA_NVRAM_READ_EVENT ,      */
	WPA_CPIN_EVENT  ,        
	WPA_TIMER_EVENT,
	WPA_SOCKET_RECV_EVENT ,  

	NUM_OF_WPA_EVENT
}WPAEventType;

typedef struct WPASocketEventTag
{
  u32 SocketID;
  s16 error;  
}WPASocketEvent_t;

typedef struct WPASocketRecvEventTag
{
  u32 SocketID;
  char *data;
  u32 size;
}WPASocketRecvEvent_t;

typedef struct WPACPinEventTag
{
	WmmSecurityOperState_e  operation;
	WmmSecurityOperError_e   error;
}WPACPinEvent_t;

typedef struct WPAGPRSActiveEventTag
{
	bool ret;
}WPAGPRSActiveEvent_t;

typedef struct WPASMSEventTag
{
	bool ret;
}WPASMSEvent_t;


typedef union WPAEventDataTag
{
	WPASocketEvent_t WPASocketEvent;
	WPACPinEvent_t WPACPinEvent;
	WPAGPRSActiveEvent_t WPAGPRSActiveEvent;
	WPASMSEvent_t WPASMSEvent;
	WPASocketRecvEvent_t WPASocketRecvEvent;
}WPAEventData_t;



typedef enum WmmSecuritySimStateTag
{
	WMMP_SECURITY_SIM_STATE_OK = 0,
	WMMP_SECURITY_SIM_STATE_PIN,
	WMMP_SECURITY_SIM_STATE_PUK,
	WMMP_SECURITY_SIM_STATE_OTHER
}WmmSecuritySimState_e;

//---------------------------

typedef enum StatusNumTag
{
  NET_STATUS = 0,
  PROTOCOL_STATUS,
  PARAM_STATUS,
  APP_STATUS,
  NUM_OF_STATUS_NUM
} StatusNum_e;

typedef enum NetStatusTag
{
	LINK_FAILED,
	LINK_IN_PROGRESS,
	LINK_OK = 4
}NetStatus_e;

 /*bob 20110221 for m2mstatus -s*/
/*参见 附件一：M2M模组规范（评审稿）.pdf */
typedef enum ProtocolStatusTag
{	
	WMMP_STATE_NOTINIT = 0x0000,		/*"WMMP-LIB Not initialized"*/
	WMMP_STATE_READY_FOR_SIM_CHANGE = 0x0001,	/*"WMMP-LIB Not initialized, ready for SIM change"*/
	WMMP_STATE_AT_READY = 0x0002,	/*"WMMP-LIB AT commands ready (interfaces ready)"*/

	
	WMMP_STATE_INIT_START = 0x0010,   /*"Initialization started"*/  /*INTIAL*/
	WMMP_STATE_INIT_BEARER = 0x0011,			/* "Initiating bearer services"  */
	WMMP_STATE_DETACH_BEARER = 0x0012,		/* "Detaching bearer services"*/
	WMMP_STATE_NEWTORK_WAITING = 0x0013,	/* "Waiting for network registration" */
	WMMP_STATE_LINK_READY = 0x0020,			/* "WMMP-T link to platform is ready" */

	
	WMMP_STATE_REGISTERING_NO_TERMID_BEGIN = 0x0100,	/*"Registering: without preset Terminal ID"*/	/*NOSERIALNUMBER*/
	WMMP_STATE_REGISTERING_TERMID_BEGIN = 0x0101,  /*"Registering: with preset Terminal ID"*/	/*SERIALNUMBERPRESET*/
	/*"Registering: with preset Terminal ID, access keys and base key"*/
	WMMP_STATE_REGISTERING_TERMID_ACCESSKEY_BASEKEY_BEGIN = 0x0201,	 
	WMMP_STATE_REGISTERING_WAIT_SECURITY_AUXDATA = 0x0202,  /*"Registering: waiting for security configuration (AUX)"*/ /*SERIALNUMBERPRESETWAIT*/
	WMMP_STATE_REGISTERING_TERMID_SIMBIND_BEGIN = 0x0203,	/*"Registering: with preset Terminal ID, bind SIM to platform"*/
	/*SERIALNUMBERPRESETANDKEY,*/
	/*SERIALNUMBERPRESETANDALLKEY,*/
	REGISTERFAIL,/*"Registration failed"*/
	PIDLE,/*"Registration ok, waiting for activation"*/


	REGISTERSUCCUSSFUL = 0x0300,/*WMMP_STATE_REGISTERED_WAIT_LOGIN*/ /*"Registered: ready for login"*/
	WMMP_STATE_LOGGING_IN_FIRST_TIME = 0x0301,/*"Logging-in: the first time"*/
	WMMP_STATE_LOGGING_IN_NORMAL = 0x0302,/*"Logging-in: normal login"*/
	WMMP_STATE_LOGGING_IN_SIMPIN_CLEARED = 0x0303,/*"Logging-in: re-login since SIM parameters were cleared"*/
	WMMP_STATE_LOGGEDIN_TRAPPING_CONFIG = 0x0401,/*"Logged-in: login-trap with TLVs synchronization"*/
	WMMP_STATE_LOGGEDIN_TRAPPING_EMPTY = 0x0402,/*"Logged-in: login-trap"*/

	
	
	LOGINOK = 0x0500,/*WMMP_STATE_LOGGEDIN*/ /*"Logged-in: ready for operations"*/
	WMMP_STATE_LOGGING_OUT_BEGIN = 0x0501,	/*"Logging-out"*/
	/*LOGOUTING = 0x0510,	*/


	
	WMMP_STATE_ABNORMAL_TRAPPING = 0x0600,/*"Abnormal: trapping"*/
	WMMP_STATE_ABNORMAL_LOGIN_RETRYING = 0x0601,/*"Abnormal: retrying to login"*/
	WMMP_STATE_ABNORMAL_LOGGING_OUT = 0x0602,	/*"Abnormal: logging-out"*/
	WMMP_STATE_ALARM_MSK = 0x0700,/*"alarm state = WMMP_STATE_ALARM_MSK + alarm code"*/
	/*
	ABNORMAL =0x0800,
	ABNORMALLOGINAGAIN,
	ABNORMALRESENDMAX,
	ALARM,
	*/
	WMMP_STATE_ALARM_ACTIVE_GPRS_FAILED = 0x0720,	/*Alarm: Bearer service, failed to attach*/
	
	ALARMLOGINFAILED = 0x08FF,	/*Alarm, login failed*/
	WMMP_STATE_REBINDING_KEY_CLEARED = 0x0900,/*"Re-binding: update keys"*/
	WMMP_STATE_REBINDING_WAIT_SECURITY_AUXDATA = 0x0901,/*"Re-binding: waiting for security configuration (AUX)"*/
	WMMP_STATE_REBINDING_WAIT_SECURITY_ECDATA = 0x0902,/*"Re-binding: waiting for security configuration (encrypted)"*/
	WMMP_STATE_REBINDING_CKEY_ONLY = 0x0903,/*"Re-binding: update communication key"*/
	WMMP_STATE_REBINDING_WAIT_SIM_READY = 0x0904,/*"Re-binding: waiting for SIM readiness to update PIN configurations"*/
	WMMP_STATE_REBINDING_PIN1_APPLY = 0x0905,/*"Re-binding: apply PIN1 configurations"*/
	WMMP_STATE_REBINDING_PIN2_APPLY = 0x0906,/*"Re-binding: apply PIN2 configurations"*/
	WMMP_STATE_REBINDING_PIN1_CLEAR = 0x0907,/*"Re-binding: reset PIN1 configurations to default"*/
	WMMP_STATE_REBINDING_PIN2_CLEAR = 0x0908,/*"Re-binding: reset PIN2 configurations to default"*/	
	/*STOP =0x0A00,*/
	WMMP_STATE_TERMINATING = 0x0A02,/*"Terminating"*/
	WMMP_STATE_MAX
}ProtocolStatus_e;


typedef enum ParamStatusTag
{
	PARAMSYNC = 0,
	PARAMNOTSYNC
}ParamStatus_e;

/*注册过程分注册、收到安全短信、登录3个步骤
  * 重庆要求在登录成功后保存安全短信的信息，
*  以防登录不成功时，在收到安全短信就保存到NV中，导致再次注册的operator =3
*/
typedef enum RegisterStatusTag
{
    WMMP_REGISTER = 0,
    WMMP_RECV_SECU_SMS,
    WMMP_REGISTER_COMPLETE 
}RegisterStatus_e;

typedef enum AppStatusTag
{	
	APPLINK_FAILED = 0,
	APPLINK_OK = 1   /*SIMCOM luyong 2012-07-13 Fix MKBug00013342   modify*/
}AppStatus_e;

typedef enum ENUM_WMMP_RMCTRL_CODE_Tag{
	WMMP_RCTRL_SUSPEND = 0x0, /* Suspend EC application */
	WMMP_RCTRL_RESUME = 0x1, /* Resume EC application */
	WMMP_RCTRL_REBOOT = 0x2, /* Reboot */
	WMMP_RCTRL_REQUEST_LOGIN = 0x3, /* Request to login if not logged in */
	WMMP_RCTRL_REPORT_STATISTICS = 0x4, /* Report statistics TLVs 0x2004, 0x2008-0x200E */
	WMMP_RCTRL_RESET_STATISTICS = 0x5, /* Reset statistics */
	WMMP_RCTRL_REPORT_MONITOR_DATA = 0x6, /* Report monitor data */
	WMMP_RCTRL_UPLOAD_APP_DATA = 0x7, /* Upload application data */
	WMMP_RCTRL_REPORT_DATACOM_CONFIG = 0x8, /* Report data communication settings TLVs 0x4005, 0x4006 */
	WMMP_RCTRL_SWITCH_APN_TO_PLATFORM = 0x9, /* Switch APN to the Platform */
	WMMP_RCTRL_RESET_FACTORY_DEFAULT = 0xA, /* Reset configurations to factory default, except Terminal ID and registration configurations */
	WMMP_RCTRL_REPORT_SYNC_TLV = 0xB, /* Report the sync. TLVs defined by TLV 0x0025 */
	WMMP_RCTRL_SWITCH_APN_TO_APP = 0xC, /* Switch APN to the application */
	WMMP_RCTRL_RELOGIN_DATACOM_ENCRYPT = 0xD, /* Re-login with data encryption enabled */
	WMMP_RCTRL_MAX
}ENUM_WMMP_RMCTRL_CODE_e;





typedef struct StatusNumberTag
{
    NetStatus_e		net_status;
    ProtocolStatus_e	protocol_status;
    ParamStatus_e		param_status;
    RegisterStatus_e  register_status;
    AppStatus_e		app_status[MAX_WMMP_APP_NUM];
}StatusNumber_t;


/*SIMCOM Laiwenjie 2013-01-16 Fix MKBug00014691 for change nvram interface BEGIN*/
/*delete*/
/*SIMCOM Laiwenjie 2013-01-16 Fix MKBug00014691 for change nvram interface END*/







typedef enum WmmpTLVTagId
{
//config
	GPRSDAILNUM = 0x0001,
	M2MAPN,
	GPRSUSERID,
	GPRSPASSWORD,
	SMSCENTRENUM,
	APPSRVTRANSKEY,
	M2MSMSPECIALNUM,
	M2MURLADDR,
	M2MIPADDR,
	M2MPORT,
	APPSMSCENTRENUM,
	APPSRVURLADDR,
	APPSRVIPADDR,
	APPSRVPORT,
reserve1,
	DNSADDR,
	PERIOD_CFG,
	APPUSSDSMSSPECIALNUM,
	M2MAPPAPN,
	BACKUPDNSADDR,
	
	TLVSYNCTAGVALUE = 0x0025,
	SMSRECVOVERTIME,
	MAXLOGINRETRYTIMES,
	DEFAULTECCODE,
	
	LOCALPTRVALUE = 0x0101,
	LOCALPORTMODE,
	LOCALPORTHALT,
	LOCALPORTVERIFY,
	LOCALPORTSCANPERIOD,
reserve2,
	DATAPACKRESPOVERTIME,
	TRANSFAILRETRYTIMES,
	GATELIMIT,

//update
	UPDATEMODE = 0x1000,
	UPDATESRVURLADDR,
	UPDATESRVIPADDR,
	UPDATESRVPORT,
	UPDATESRVLOGINPASSWORD,
	UPDATEFILEREV,
	UPDATEFILELENGTH,
	CRC16CHECK,
	CRC32CHECK,
	UPDATEAFFAIRTRANSID,
	FILERELEASEDATA,
	FORCETOUPDATEPARAM,
	CURRENTFILEREV,

//statistic
	STATINFOREPORT = 0x2001,
	REPORTTIME,
	PERIODREPORT,
	MONTHLYOPERATIONSIGN,
	CLEARSTATDATA,
	STATSTARTTIME,
	STATFINISHTIME,
	SMSDELIVERYSUCC,
	SMSDELIVERUFAIL,
	SMSRECEIVEDNUM,
	GRPSDATASIZE,
	USSDSUCCNUM,
	USSDFAILNUM,
	USSDRECEIVEDNUM,
	
	SMSSTATPARAM = 0x2011,
	SMSSTATAUTOREPORTPARAM,
	M2MMUTUALSMSSTAT,
	OPERATIONMUTUALSMSSTAT,
	GPRSOPERATIONSTATPARAM,
	GPRSOPERATIONSTATREPORTPARAM,
	M2MMUTUALGPRSDATASIZE,
	OPERATIONMUTUALGPRSDATASIZE,
	USSDSTATPARAM,
	USSDSTATREPORTPARAM,
	M2MMUTUALUSSDSTAT,
	OPERATIONMUTUALUSSDSTAT,

//status
	PROTOCOLREV = 0x3001,
	TERMINALSERIESNUM,
	IMSINUM,
	FIRMWAREREV,
	PREVPERIOD,
	CELLULARID,
	SIGNALLEVEL,
	INITPARAFLAG,
	SYSTEMTIME,
reserve3,
	ALARMSTATUS,
	ALARMTYPE,
	ALARMCODE,
	RESTOREALARM,
	M2MDEVSTATUS,/*0x300f*/
	SUPPORTEDDEV,
	PACKLOSTNUM,
	FAILWARNING,

//control
	SETCOMMAND = 0x4001,
	CONNECTMODE,
	DATATRANSMODE,
	PERIOD_CTRL,
	COLLECTDATATRANSMODE,
	M2MCONNECTMODEONIP,
	USERDATAID,
	SUBPACKMODEPARAM,
	PLATFORMPARAMOPT,

	EXECUTEPARAMOPT = 0x4010,
lengthblank,	
	DESSERIESNUM,
	SRCSERIESNUM,
	DESECCODE,
	SRCECCODE,
	TRANSCTRLCOMMANDSTATICPARAM,
	VERIFYTLVBYCRC32,
	
	EXTERNALDEVCTRL = 0x4021,

//security
	SIMPIN1 = 0xE001,
	SIMPIN1MD5,
	SIMPIN1ACTIVE,
	SIMPIN2,
	SIMPIN2MD5,
	SIMPIN2ACTIVE,
	SIMPUK1,
	SIMPUK1MD5,
	SIMPUK1ACTIVE,
	SIMPUK2,
	SIMPUK2MD5,
	SIMPUK2ACTIVE,
	BINDPARAACTIVE,
	BINDIMEI,
	BINDIMEIMD5,
	BINDIMEIORIG,
	BINDIMEIORIGMD5,
	BINDIMSI,
	BINDIMSIMD5,
	BINDIMSIORIG,
	BINDIMSIORIGMD5,
	BINDCURRENTMD5,
	BINDORIGINALMD5,
	PIN1ERRORALLOW,
	PIN2ERRORALLOW,
	PUK1ERRORALLOW,
	PUK2ERRORALLOW,

	M2MPASSCLEARPARA = 0xE020,
	INITPARA,
	INITPARAUSED,
	INITCOMMAND,
	INITTLVACTTAG,
	UPLINKPASSWORD,
	UPLINKPASSWORDMD5,
	UPLINKPASSWORDEXPDATE,
	DOWNLINKPASSWORD,
	DOWNLINKPASSWORDMD5,
	DOWNLINKPASSWORDEXPDATE,

	ENCRYPTIONSUPPORT = 0xE031,
	ENCRYPTIONUSED,
	ENCRYPTIONCOMMAND,
	ENCRTYPTIONTAG,
	ENCRYPTIONALGORITHM,
	PUBLICKEY,
	PRIVATEKEY,
	PUBLICKEYEXPDATE,
	ENCRYPTIONKEYALGORITHM,
	ENCRYPTIONKEYMD5,
	CONVPUBLICKEY,
	CONVPRIVATEKEY,
	CONVKEYEXPDATE,
	CONVENCRYPTIONALOG,
	CONVENCRYPTIONMD5,
	CONVENCRYPTIONMSGBODY,
	CONVAUTHFAILREPORT,
	CONVSINGLEENCRYPTBODY,
	CONVMULTIENCRYPTBODY,
	CONVPACKEDENCRYPTBODY,

//custom
	TLVSYNCTAGNUM = 0x8001,
	MAXPDULENGHT,
	INITTERMINALNUM,
	RMCTRLTRACEENABLE = 0x81F0,

//custom3
	RMCTRLTRACEATNAME,/*APPSEND*/
	RMCTRLTRACEATRET,
	RMCTRLTRACEATACK,
	RMCTRLTRACEATMSG,
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_ADC_GPIO__
// adc gpio
	CUSTOMADC,
	CUSTOMGPIOINPUT,   /*TLV81F6 用于设置GPIO脚的输入状态,  数据长度2个字节，16位，
	                                    每一位表示一个GPIO的状态,从低到高以此表示GPIO1-GPIO16
	                                    （只是GPIO标号并非实际电路原理图标示的GPIO脚）
	                                    如果设置某个GPIO为输入，将该位置0，其他位为1；
	                                    可同时设置多个GPIO脚*/
	CUSTOMGPIOOUTPUT, /*TLV81F7 有两个功能
	                                    1.用于设置GPIO的输出状态及高低电平状态
	                                    2.用于查询GPIO的状态信息
	                                    该TLV数据长度四个字节，32位，该值的高十六位表示16个GPIO的输入输出状态，
	                                    低十六位表示16个GPIO的高低电平状态
	                                    如果高十六位中某位为0，表示该GPIO脚为输入；为1，表示该GPIO脚为输出
	                                    如果低十六位中某位为0，表示该GPIO脚为低电平；为1，表示该GPIO脚为高电平
	                                    可同时设置多个GPIO脚*/
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
	RMCTRLTRACEATCFGLISTN = 0x82f0,/*0x82f0*/
	RMCTRLTRACEATCFGN,
	RMCTRLTRACEATM2MSTAN,
	RMCTRLTRACEATTRAPN,
	RMCTRLTRACEATAPPOPENN,
	RMCTRLTRACEATAPPCLOSEN,	
	RMCTRLTRACEATBHVNAME,
	RMCTRLTRACEATBROPENNAME,
	RMCTRLTRACEATBRSTAN,
	RMCTRLTRACEATCGPADDRN,
	RMCTRLTRACEATAPPCLOSENALREADY,
	

	RMCTRLTRACEATCFGLISTRET = 0x83f0,
	RMCTRLTRACEATCFGRET,
	RMCTRLTRACEATM2MSTARET,
	RMCTRLTRACEATTRAPRET,
	RMCTRLTRACEATAPPOPENRET,
	RMCTRLTRACEATAPPCLOSERET,
	RMCTRLTRACEATBHVRET,	
	RMCTRLTRACEATBROPENRET,
	RMCTRLTRACEATBRSTARET,
	RMCTRLTRACEATCGPADDRRET,

	RMCTRLTRACEATACKAPPOPEN = 0x84f0,
	RMCTRLTRACEATACKACK,

	RMCTRLTRACEATMSGRECVDATA = 0x85f0,
	RMCTRLTRACEATMSGSOCDESTR,
	RMCTRLTRACEATMSGCLOSEDIND,
	RMCTRLTRACEATMSGCLOSEDINDTOUT,

	RMCTRLTRACEATMAX,
	


//custom2
   APPSERVER0IPPORT = 0xD001,
   APPSERVER1IPPORT,
   APPSERVER2IPPORT,
   APPSERVER3IPPORT,
   APPSERVER4IPPORT,
   APPSERVER5IPPORT,
   APPSERVER6IPPORT,
   APPSERVER7IPPORT, 
   
   
	MAX_TLV_TAG_ID
}WmmpTLV_Id;

typedef enum TLVTypeTag
{
  TLV_STRING,
  TLV_INT8,
  TLV_INT16,
  TLV_INT32,
  TLV_DEFAULT
}TLVType_e;

typedef struct TLVRetValTag
{
	char* addr;
	u16 length;
	TLVType_e TLVType;
}TLVRetVal_t;

typedef enum WmmpDecodeRecvResultTag
{
	WMMP_DECODE_OK,
	WMMP_DECODE_ERROR,    //报文解析错误
	WMMP_DECODE_NOPDU,   //非wmmp 报文
	WMMP_DECODE_FRAME_ERROR,
	WMMP_STACK_STATUS_ERROR, //由于冲突缓冲区中的报文是网络延时收到的造成
	WMMP_DECODE_CONTINUE
}WmmpDecodeRecvResult_e;

typedef enum HasAppDataTypeTag		 /*bob add 20101122 */
{
	WMMP_UNKNOWN_HASAPPDATA =0,
	WMMP_CONFIGSETAPN_HASAPPDATA,
	WMMP_TLVREBOOT_HASAPPDATA,
	WMMP_REMOTE_CONTROL_REBOOT_HASAPPDATA,  
	WMMP_HAS_APP_DATA_TYPE_END	
}HasAppDataType_e;

/*Timer id define*/
typedef enum 
{
    WMMP_CALLBACK_TIMER = 0,
    WMMP_APP_NET_TIMER = 1, //app net 数据分包发送时使用
    WMMP_MAX_TIMER
}WMMP_TIMER_ID;

/*SIMCOM zhengwei for MKBug00015002 20130121*//*begin*/

#define CONNREF_INVALID   0xFF

typedef enum
{
	WMMP_CONNREF_WMMP,
	WMMP_CONNREF_APP_0,
	WMMP_CONNREF_APP_1,
	WMMP_CONNREF_APP_2,
	WMMP_CONNREF_APP_3,
	WMMP_CONNREF_APP_4,
	WMMP_CONNREF_APP_5,
	WMMP_CONNREF_APP_7,
	 NUM_OF_WMMP_CONNREF,
}Simcom_wmmp_connRef;

typedef void (*wmmpSendCallback)(u32 sizeProcessed);

typedef void (* wmmpCreateConnectCallback)(bool success, Simcom_wmmp_connRef connRef);
/*SIMCOM zhengwei for MKBug00015002 20130121*//*end*/

#endif
#endif

