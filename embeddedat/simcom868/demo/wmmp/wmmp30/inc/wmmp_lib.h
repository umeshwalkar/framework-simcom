#if defined(__MODEM_WMMP30_SIMCOM__)
#ifndef _WMMP_LIB_H_
#define _WMMP_LIB_H_
/*****************************************************************
* Include Files
*****************************************************************/
/*SIMCOM Laiwenjie 2013-01-17 Fix MKBug00014691 for change nvram interface BEGIN*/
#ifdef __SIMCOM_WMMP__
#include "simcom_wmmp_nvram_define.h"
#endif
/*SIMCOM Laiwenjie 2013-01-17 Fix MKBug00014691 for change nvram interface END*/

#define LOGIN_COMMAND 0x0001			
#define LOGIN_ACK_COMMAND 0x8001 
#define LOGOUT_COMMAND 0x0002
#define LOGOUT_ACK_COMMAND 0x8002
#define HEART_BEAT_COMMAND 0x0003
#define HEART_BEAT_ACK_COMMAND 0x8003 
#define TRANSPARENT_DATA_COMMAND 0x0004 
#define TRANSPARENT_DATA_ACK_COMMAND 0x8004 
#define CONFIG_GET_COMMAND 0x0005 
#define CONFIG_GET_ACK_COMMAND 0x8005 
#define CONFIG_SET_COMMAND 0x0006 
#define CONFIG_SET_ACK_COMMAND 0x8006 
#define CONFIG_TRAP_COMMAND 0x0007
#define CONFIG_TRAP_ACK_COMMAND 0x8007 
#define REGISTER_COMMAND 0x0008 
#define REGISTER_ACK 0x8008
#define CONFIG_REQ_COMMAND 0x000A 
#define CONFIG_REQ_ACK_COMMAND 0x800A 
#define REMOTE_CTRL_COMMAND 0x000B 
#define REMOTE_CTRL_ACK_COMMAND 0x800B 
#define DOWNLOAD_INFO_COMMAND 0x000C
#define DOWNLOAD_INFO_ACK_COMMAND 0x800C 
#define FILE_REQ_COMMAND 0x000D 
#define FILE_REQ_ACK_COMMAND 0x800D 
#define SECURITY_CONFIG_COMMAND 0x000E
#define SECURITY_CONFIG_ACK_COMMAND 0x800E
#define TRANSPARENT_CMD_COMMAND 0x000F 
#define TRANSPARENT_CMD_ACK_COMMAND 0x800F 

#define CONTINUE_SEND_COMMAND 0x8888 //user define

#define 	PROTOCOL_VERSION 				0x0300			/*wmmp lib*/
#define 	TERMINAL_SOFTWARE_VERSION 		("SIMCOM10")		/*wmmp app*/
#define	WMMP_MAJOR_VER					1
#define	WMMP_MINOR_VER					1

#ifdef __SIMCOM_WMMP_TEST_PLATFORM__
#define	UNREGISTER_TERMINAL_NUM			("A301030200000000")
#else	
/*�����ƶ���ʽƽ̨�ն����к�ǰ8λ��A9720300*/
#define	UNREGISTER_TERMINAL_NUM			("A972030000000000")
#endif 
//#define	FILE_START_NUMBER					6
#define	FILE_START_NUMBER						0

#define MAX_COMMANDINFO_NUM 					(50)			/*(100)*/		/*(25)*/			/*15*/

#define MAX_SEND_BUFFER (10*1024)
#define MAX_RECEIVE_BUFFER (10*1024)


#define PDU_HEADER_LENGTH (28)  //����ͷ����
#define PDU_ABSTRACT_LENGTH (20) //ժҪ����
//#define UNREGISTER_TERMINAL_NUM   "A014077700000000"  //δע������к�
//#define UNREGISTER_TERMINAL_NUM   "A301300100000000"  //δע������к�

/*   add by cbc@20091216 for WMMP 3.0  Э��: �ն˺�ƽ̨��ǰ��Э������״̬ BEGIN*/
		/*WMMP3.0 Э��:�ն���������,�ȴ�M2M ��������Ӧ�����и�״̬ */
typedef enum simcom_m2m_terminal_run_status_tag
{
	M2M_TERMINAL_RUN_STATUS_INIT,

	/* ������������״̬*/	
	STATUS_REGISTER_COMMAND =0x0008, 
	STATUS_REGISTER_ACK =0x8008,
	STATUS_LOGIN_COMMAND =0x0001	,		
	STATUS_LOGIN_ACK_COMMAND = 0x8001 ,
       STATUS_LOGOUT_COMMAND = 0x0002,
	STATUS_LOGOUT_ACK_COMMAND =0x8002,
	STATUS_HEART_BEAT_COMMAND= 0x0003,
	STATUS_HEART_BEAT_ACK_COMMAND =0x8003 ,	
	STATUS_CONFIG_TRAP_COMMAND =0x0007,
	STATUS_CONFIG_TRAP_ACK_COMMAND =0x8007 ,
	STATUS_CONFIG_REQ_COMMAND =0x000A ,
	STATUS_CONFIG_REQ_ACK_COMMAND = 0x800A ,
	STATUS_FILE_REQ_COMMAND =0x000D ,
	STATUS_FILE_REQ_ACK_COMMAND =0x800D, 

	/* ˫����������״̬*/
	STATUS_TRANSPARENT_DATA_COMMAND= 0x0004 ,
	STATUS_TRANSPARENT_DATA_ACK_COMMAND= 0x8004 ,
	STATUS_SECURITY_CONFIG_COMMAND = 0x000E,
	STATUS_SECURITY_CONFIG_ACK_COMMAND= 0x800E,
	STATUS_TRANSPARENT_CMD_COMMAND= 0x000F ,
	STATUS_TRANSPARENT_CMD_ACK_COMMAND= 0x800F ,

	M2M_TERMINAL_RUN_STATUS_INVALID = 0XFFFF
}simcom_m2m_terminal_run_status;

	/*WMMP3.0 Э��:M2M ������ƽ̨��������,�ȴ��ն���Ӧ�����и�״̬ */
typedef enum simcom_m2m_platform_run_status_tag
{
	M2M_PLATFORM_RUN_STATUS_INIT = 0,	

	/* ������������״̬*/	
	STATUS_CONFIG_GET_COMMAND= 0x0005 ,
	STATUS_CONFIG_GET_ACK_COMMAND =0x8005 ,
	STATUS_CONFIG_SET_COMMAND =0x0006, 
	STATUS_CONFIG_SET_ACK_COMMAND =0x8006 ,	
	STATUS_REMOTE_CTRL_COMMAND =0x000B, 
	STATUS_REMOTE_CTRL_ACK_COMMAND =0x800B ,
	STATUS_DOWNLOAD_INFO_COMMAND =0x000C,
	STATUS_DOWNLOAD_INFO_ACK_COMMAND =0x800C ,	

	/* ˫����������״̬*/
	STATUS_M2M_SECURITY_CONFIG_COMMAND = 0x000E,
	STATUS_M2M_SECURITY_CONFIG_ACK_COMMAND= 0x800E,
	STATUS_M2M_TRANSPARENT_DATA_COMMAND= 0x0004 ,
	STATUS_M2M_TRANSPARENT_DATA_ACK_COMMAND= 0x8004 ,	
	STATUS_M2M_TRANSPARENT_CMD_COMMAND= 0x000F ,
	STATUS_M2M_TRANSPARENT_CMD_ACK_COMMAND= 0x800F ,
	
	M2M_PLATFORM_RUN_STATUS_INVALID = 0XFFFF
}simcom_m2m_platform_run_status;
/*   add by cbc@20091216 for WMMP 3.0  Э��: �ն˺�ƽ̨��ǰ��Э������״̬ END*/



typedef enum WmmpPDUUsageTag
{
	WMMP_PDU_UNKNOWN =0,
	WMMP_PDU_ATCOMMAND,  /*at ����*/
	WMMP_PDU_TRAP_SIGMA_TIME_STAT,  /*//�ۼ�ͳ�ƶ�ʱ�ϱ�*/
      WMMP_PDU_TRAP_SIGMA_STAT,  /*�ۼ�ͳ���ϱ�*/
	WMMP_PDU_TRAP_SMS_STAT,  /*����ͳ�ƶ����ϱ�*/
	WMMP_PDU_TRAP_GPRS_STAT, /*����ͳ��GPRS�ϱ�*/
	WMMP_PDU_LOGOUT_TERM,  /*�ǳ�����,ֱ�ӹر�Э��ջ,��at$m2mterm���� */
	WMMP_PDU_SECURITY_LOGOUT,  /*security ����,Ҫ��ǳ������ò���,Ȼ���ص�¼*/
	WMMP_SHORT_CONNECT,   /*������*/
	WMMP_KEY_OUT_OF_DATE,  /*cbc220100521:��������Կ�ĸ��º� ��Ч�ڴ����й�,Ȼ���ص�¼*/
	WMMP_TRANS_ABNORMAL,  /*�����쳣*/
	WMMP_TLV_REBOOT,        /*11*/   /*  cbc@20100521:ƽ̨���·������ӵ�IP��ַ��˿�, ��Ҫ�������ӷ�����������Ч*/
	WMMP_REMOTE_CONTROL ,/*Զ�̿���*/     
	WMMP_SIM_SECURITY_LOGOUT  , /*  0XE00D = 2, ƽ̨���SIM����ȫ����,�����ǳ�*/
	WMMP_LOCAL_CONFIG_REBOOT ,  /* add  by cbc@20100409 end*/ /*���ز���������Ҫ�������ϱ�TRAP ֪ͨ  */	
	WMMP_SECURITY_REBOOT , /* CBC@20100518:���������������Կ�޸���Ҫ�ص�½ */
	WMMP_REMOTE_CONTROL_REBOOT,   /*bob add 20101122 */
	WMMP_CONFIG_SET_APN_REBOOT,   /*bob add 20101122 */
	WMMP_RMCON_REFACTORY_REBOOT,/*18*/
	WMMP_RMCTRL_TRAP_TRACE,/*19*/	/*bob add 20110301*/
      WMMP_PDU_USAGE_END
	
}WmmpPDUUsage_e;

typedef struct RegisterInfoTag
{
	u8 operation;
}RegisterInfo_t;

typedef struct LoginOutInfoTag
{
	u8 reason;
}LoginOutInfo_t;

typedef struct LoginInfoTag
{
	bool clearSecurity;
}LoginInfo_t;

typedef struct SecurityAckInfoTag
{
    u8 retcode;
    u8 operation;
	u16 *tags;
	u8 tagnum;
}SecurityAckInfo_t;

typedef struct SecurityInfoTag
{
    u8 operation;
	u16 *tags;
	u8 tagnum;
}SecurityInfo_t;

typedef struct CfgTrapInfoTag 
{
    u16* TLVTags; 
    u16 TagNum;   
}CfgTrapInfo_t;

typedef struct CfgReqInfoTag 
{
    u16* TLVTags; 
    u16 TagNum;   
}CfgReqInfo_t;

typedef struct CfgGetAckInfoTag
{
    u16* TLVTags; 
    u8 TagNum;
    u8 ret;
}CfgGetAckInfo_t;

typedef struct CfgSetAckInfoTag
{
 	u8 ret;
	u16* errortag;
	u8 errorTagNum;
}CfgSetAckInfo_t;

typedef struct RemoteCtrlAckInfoTag
{
    u16* errortag;
    u8 errorTagNum;
    u8 ret;
}RemoteCtrlAckInfo_t;

typedef struct DownloadAckInfoTag
{
    u8 ret;
}DownloadAckInfo_t;

typedef struct FileReqInfoTag
{
    char TransID[4];
    u8 Status;
    u32 FileReadPoint;
    u16 FileBlockSize;
}FileReqInfo_t;

typedef struct TransparentDataInfoTag
{
    char Type;
    char *Data;
    u16 DateLen;
	char destination[30];
}TransparentDataInfo_t;

typedef struct ContinueSendInfoTag
{
    char transID[2];
    u16 partnum;
    u16 partallnum;
    u16 CommandID;
}ContinueSendInfo_t;
typedef union CommandExInfoTag
{
	LoginInfo_t  LoginInfo;
	RegisterInfo_t RegisterInfo;
	LoginOutInfo_t LoginOutInfo;
    SecurityAckInfo_t SecurityAckInfo;
    CfgTrapInfo_t ConfigTrapInfo;
    CfgReqInfo_t    ConfigReqInfo;
    CfgGetAckInfo_t    ConfigGetAckInfo;
    CfgSetAckInfo_t     ConfigSetAckInfo;
    ContinueSendInfo_t  ContinueSendInfo;
    RemoteCtrlAckInfo_t RemoteCtrlAckInfo;
    DownloadAckInfo_t DownloadAckInfo;
    FileReqInfo_t   FileReqInfo;
    TransparentDataInfo_t TransparentDataInfo;
	SecurityInfo_t		SecurityInfo;
}CommandExInfo_t;

typedef struct CommandInfoTag
{
	u16 CommandID;
	u8 SecuritySet;
	WmmpTransType_e TransType;
	CommandExInfo_t CommandExInfo;
	WmmpPDUUsage_e PDUUsage;
}CommandInfo_t;

typedef struct CommandInfoBufferTag
{
    bool set;
    CommandInfo_t CommandInfo;
}CommandInfoBuffer;




typedef enum WmmpEncodeSendResultTag
{
	WMMP_ENCODE_OK,
	WMMP_ENCODE_ERROR,
	WMMP_ENCODE_CONTINUE
}WmmpEncodeSendResult_e;

typedef enum WmmStackStateTag
{
	WMMP_STACK_INIT=0,
	WMMP_STACK_SERVER_CONTINUE,   /*ģ���������𣬵ȴ�������Ӧ��,�������*/
	WMMP_STACK_CLIENT_CONTINUE,	   /*�������������𣬵ȴ�ģ��Ӧ��,�������	*/
	WMMP_STACK_SERVER_WAITING,     /*ģ���������𣬵ȴ�������Ӧ��*/
	WMMP_STACK_CLIENT_WAITING       /*�������������𣬵ȴ�ģ��Ӧ��	*/
}WmmStackState_e;

typedef enum WmmPDULinkTag
{
	WMMP_UPLINK,    //���б���
	WMMP_DOWNLINK   //���б���
}WmmPDULink_e;

typedef enum wmmpTimeUserIDTag
{
  NETWORK_CONNECT_TIMEOUT_TIMER = 0,   // /*�������ӳ�ʱ��ʱ��*/
  SEND_PDU_TIMEOUT_TIMER,              //    /*���ķ��ͳ�ʱ��ʱ��*/
  HEART_BEAT_TIMER,                          //   /* ������ʱ��*/
  SHORT_CONNECT_MODE_TIMER,        //    /*�����ӳ�ʱ��ʱ��*/
  SIGMA_STAT_TIMER,                          //  /*����ͳ�ƶ�ʱ��*/
  STAT_TIMER,                               //        /*��ϸͳ�ƶ�ʱ��*/
  PDU_INTERVAL_TIMER,  /*6*/           // /* cbc220100416:  ���ͻ������Ƿ���δ���ͱ��� */
  SOCKET_CLOSE_TIMEOUT_TIMER,/*7*/
  RESIGTER_KEY_DOWN_TIMEOUT_TIMER,  /*8*///*   CBC@20100525: ע��ʱ�ն˵ȴ�ƽ̨�·����볬ʱ  */
  TIMING_REGISTER_TIMER,		//  /*TIMING REGISTER TIMER*/
  TIMING_LOGIN_TIMER,
  TRAPABORTWARN_TIIMER,  
  NUM_OF_WMMP_TIMER
}WmmpTimeUserID_e;


typedef struct WmmpDataInterfaceTag
{
  u32   totalLen; //�����ܳ���
  u32   remLen;  //ʣ��δ���͵����ݳ���
  u32  sendLen;
  char    *dataBuffer_p;
}WmmpDataInterface_t; 


typedef struct PDUContextTag
{
  u16 CommandID;
  //u16 CommandIDAck;
  //char *Head;   //����ʱָ��ƴװ����׵�ַ
  char *BodyConst;
  char *BodyTLV;
  bool NeedDispart; /*��¼�Ƿ���Ҫ�ְ�*/
  u16  CurrentPartNo;
  u16   SubPartCount;
  u16 BodyLength;//didn't include abstract length,include body const part and body TLV part
  u16 Totallength;
  //char *DispartHead;  //����ʱ��ָ��ÿ���յ�������
  u16 SendLength;  //����ʱ������ÿ���յ��ĳ���
  u8  SecuritySet;
  //WmmPDULink_e  PDULink;	
  //CommandExInfo_t CommandExInfo;  

  u16 partallnum;/*cbc@20100423: �ְ��ܰ��� */
  u16 partnum; /*cbc@20100419:��ǰ�Ķ���յ�����ƽ̨�ĵڼ��� */
  char TransID[2];/*cbc@20100419:��ǰ�Ķ���յ��� TransID*/
  u8  dispartFlag;/* ����˳���־  */
  char oper;/*��������� */
  u8  adFileEndFileReqFlag;  
}PDUContext_t;




typedef struct WmmpContextTag
{

    //��Ҫ����Ĳ�������������
    WmmpCfgContext_t		ConfigContext;
    WmmpUpdateContext_t		UpdateContext;
    WmmpStatisticContext_t		StatisticContext;
    WmmpTermStatusContext_t	StatusContext;
    WmmpControlContext_t		ControlContext;
    WmmpSecurityContext_t	SecurityContext;

    //custom TLV
    WmmpCustomContext_t		CustomContext;
    WmmpCustom2Context_t		Custom2Context;
    WmmpCustom3Context_t		Custom3Context;

    StatusNumber_t	Wmmp_status;


    //������ˮ��
    u32 FrameNo;
    //ʱ���
    char TimeStamp[4+1];
    //GPRS���ӳ�ʱ��ʱ��


    //��wmmp�������ӵ�socket
    u32 					wmmpSocket;
    PDUContext_t  PDUContext; //����������
    PDUContext_t  PDUContext_receive; //���ձ���������

    WmmStackState_e   StackState;  //��ǰЭ��ջ��ʹ�����
    WmmpTransType_e  TransType;
    WmmpPDUUsage_e PDUUsage;  //ָ���ñ��ĵľ�����;

    CommandInfo_t CommandInfoBuffer[MAX_COMMANDINFO_NUM];
    u8 CommandBufferFront ;     /*//ѭ������ͷָ��*/
    u8 CommandBufferRear ;    /* ѭ������βָ�� */
    u8 CommandBufferCount ;    /*//ѭ�����е�ǰ��ŵ�����*/


    //�����ȼ����Ķ���
    CommandInfo_t CommandInfoBuffer_High[MAX_COMMANDINFO_NUM];
    u8 CommandBufferFront_High ;    
    u8 CommandBufferRear_High ;    
    u8 CommandBufferCount_High ;   


    //���յ��ı�����ˮ��
    char ReceiveFrameNo[4];
    //�ְ��ĵ�һ�������ڴ���tlv4010
    char FisrtFrameNo[4];

    char ReceiveBuffer[MAX_RECEIVE_BUFFER];/*cbc220100402:  ����  */
    char SendBuffer[MAX_SEND_BUFFER];    /*cbc220100402: ���ְ����� BUFFER  */
    char IOBuffer[MAX_IO_BUFFER];/*cbc220100402:  �ְ����͵�BUFFER  */
    u32    wmmpOpenSkt[MAX_WMMP_APP_NUM]; 
    // bool wmmpOpenSend;
    
//    u32	rxDataSize[MAX_WMMP_APP_NUM];
//    u32	resendSize[MAX_WMMP_APP_NUM];  /*not used*/

    u8 	dataBufferFlag ; //1��ʾ�������ݵ�buffer��ʹ��
//    WmmpDataInterface_t dataInterface;	/*compile error*/	/*bob add 20110128*/
//    bool    issending;							/*bob add 20110129*/

    u8 maxOpenGPRSCount;
    u8 maxPDUTimeOutCount;
    bool m2mswitch; //�ܿ���
    u8 ReSendTime;     /*cbc220100402:   Ĭ��20S */
    u8 maxRegisterTimeOutCount;
    //u8	maxLoginTimeOutCount;
    //u8 maxregisterTrapAbortWarn;
    u8 maxTrapAbortWarn;
    u8	maxRegSecuSmsTimeOutCount;
    u8 maxHeartTimeOutCount;
    u32	autoLoginuTime;

    u8 LoginFaildCount;  //��¼ʧ�ܴ���/*cbc220100402:  Ĭ�� 3 COUNT */
    bool isM2MStatTime;  //������ͳ��ʹ��,��ǰʱ���Ƿ������ۼ�ͳ��ʱ��

//    u8      v_GprsAvailability;   /*bob add 20110118*/
//    u8		v_rmCtl_reboot_flag;		/*bob add 20110122*/

    char *TLVtemp;  //����ָ����Ҫ�ǳ���������õ�tlv�������ڵ��ڴ�
    u16 TLVtempLen;



    //���ڲ����ع�����ʱ������3���������лع���������
    u32 		M2MIPAddr_b;
    u32		M2MPort_b;
    char 		AppSMSCentreNum_b[20];
    char		uplinkPassword_b[9];	
    char		downlinkPassword_b[9];
    char		publickey_b[32];


    void (*WPIEventFucPtr)(WPIEventType,WPIEventData_t *);

    //Timer
    u32 TimerTicks;
    bool TimerRun[NUM_OF_WMMP_TIMER];
    u32 TimerStopTick[NUM_OF_WMMP_TIMER];

    bool enableSend;

    //���ڽ�������б��ĳ�ͻ
    char CriticalBuffer[MAX_IO_BUFFER];/*cbc220100402:    */
    bool isCriticalUsed;
    u16 CriticalBufferLen;
    WmmpTransType_e CriticalTransType;

    /* add by cbc@0091216: ��ס��ǰ�ն˺�M2Mƽ̨�������������������״̬:BEGIN*/
    simcom_m2m_terminal_run_status terminal_run_status;
    simcom_m2m_terminal_run_status last_terminal_run_status;
    simcom_m2m_platform_run_status platform_run_status;
    /* add by cbc@0091216: ��ס��ǰ�ն˺�M2Mƽ̨�������������������״̬:end*/
    wmmpSendCallback    gprsSendCallBack;/*�첽���ͺ�Ļص�����*/
    wmmpCreateConnectCallback createConnectCallBack; /*�������Ӻ�Ļص�����*/
}WmmpContext_t;

void wmmpProcessConnectTimer(void);
void wmmpProcessPDUIntervalTimer(void);
void wmmpProcessPDUTimer(void);
void wmmpProcessHeartTimer(void);
void wmmpProcessShortConnectTimer(void);
void wmmpProcessSigmaStatTimer(void);
void wmmpProcessStatTimer(void);
void wmmpProcessError(void);
void wmmpProcessErrorRoll(void);

void wmmpProcessServerLogoutReq(u8 reason);
void wmmpProcessLoginAck(u8 result);
void wmmpProcessRegAck(u8 result);
void  wmmpProcessTrapCnf(void);
void  wmmpProcessReqCnf(u8 ret,u16 tag);
void wmmpProcessSecurtiyCfgAck(u8 result);
void wmmpProcessSecurtiyCfgReq(u8 operation,u8 retcode,u16 *tags,u8 tagnum);
void wmmpProcessConfigGetReq(u16 *TLVTag, u8 Tagnum,u8 ret);
void wmmpProcessConfigSetReq(u8 ret,u16 *tags, u8 tagnum,char *PDU_TLV, u16 TLVLen);
void wmmpProcessRemotCtrlReq(u8 ret,u8 paramId, u16 *errortag, u8 errortagnum);
void wmmpProcessDownloadReq(char* url,u8 ret);
void wmmpProcessTransDataReq(char type, char* des, char *data, u16 datalen);
void wmmpProcessLogoutCnf(void);
void wmmpServerContinueSend(u16 CommandID, char* transID,u16 partnum,u16 partallnum);

void wmmpStartInterTimer(WmmpTimeUserID_e UserValue, u32 timeoutPeriod);
void wmmpStopInterTimer(WmmpTimeUserID_e UserValue);
bool wmmpisInterTimerRun(WmmpTimeUserID_e UserValue);

void wmmpTrapAbortWarn(u8 AlarmStatus,u8	AlarmType,u16	AlarmCode);

WmmpContext_t*  ptrToWmmpContext(void);
WmmpDataInterface_t* ptrToWmmpDataInterface(void);
u16 Readu16(char *p);
u32 Readu32(char *p);
char * Writeu16(char *p,u16 i);
char *Writeu32(char *p,u32 i);

void SaveToCriticalBuffer(char * data, u16 len,WmmpTransType_e type);
void DecodeCriticalBuffer(void);

bool  wmmpRegisterReq(WmmpTransType_e TransTypeValue);
bool  wmmpLoginReq(WmmpTransType_e TransTypeValue);

bool wmmpCheckHasAppData(u8 uAppCount);

#ifndef max
#define max(a,b) (a>b)? a:b
#endif 
#ifndef min
#define min(a,b) (a<b)? a:b
#endif 


#endif
#endif
