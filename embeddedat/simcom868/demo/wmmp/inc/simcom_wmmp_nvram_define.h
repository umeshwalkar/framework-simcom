/***************************************************************************
*
*                  simcom_wmmp_nvram_define.h
*
*                Copyright (c) 2007 SIMCOM
*
*****************************************************************************
*  模块名	： pub                       
*  文件名	： simcom_wmmp_nvram_define.h
*  实现功能：
*  作者		： wenjie.lai
*****************************************************************************
*  修改记录:
*
*  日  期             修改人	      修改内容
*  2013/01/10         wenjie.lai		Create  Fix MKBug00014691 for change nvram interface
*****************************************************************************/	
#ifndef __SIMCOM_WMMP_NVRAM_DEFINE_H__
#define __SIMCOM_WMMP_NVRAM_DEFINE_H__

/***************************************************************************
* Include Files
***************************************************************************/
//#include "simcom_adapter_type.h"

#include "wmmp_typ.h"
#include "eat_nvram.h"
/***************************************************************************
* Macros
***************************************************************************/
#define MAX_GPRS_USER_NAME_LEN 32
#define MAX_GPRS_PASSWORD_LEN  32
#define MAX_GPRS_APN_LEN       100
#define MAX_GPRS_IP_ADDR       4 /* IP address length */


/***************************************************************************
* External Variables
***************************************************************************/

/***************************************************************************
* Types
***************************************************************************/
typedef enum NvramNameTag
{
    NVRAM_EF_MIN_LID=1,
    NVRAM_EF_WMMP_WOPEN_LID = NVRAM_EF_MIN_LID,
#ifdef __SIMCOM_EAT_WMMP__
    NVRAM_EF_WMMP_TLV_CFG_LID,
    NVRAM_EF_WMMP_TLV_UPDATE_LID,
    NVRAM_EF_WMMP_TLV_STAT_LID,
    NVRAM_EF_WMMP_TLV_STATUS_LID,
    NVRAM_EF_WMMP_TLV_CTRL_LID,
    NVRAM_EF_WMMP_TLV_SECURITY_LID,
    NVRAM_EF_WMMP_TLV_CUSTOM_LID,
    NVRAM_EF_WMMP_TLV_CUSTOM2_LID, 
#ifdef __SIMCOM_WMMP_ADC_GPIO__
    NVRAM_EF_WMMP_TLV_CUSTOM3_LID, 
#endif
    NVRAM_EF_WMMP_ACCOUNT_LID,
#endif
    NVRAM_EF_MAX_LID 
}NvramName_e;

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
} Wmmp_atc_value_struct;

#if defined(__SIMCOM_EAT_WMMP__)
    typedef struct WmmpCfgContextTag
    {
        char     GPRSDailNum[20];
        char     M2MAPN[20]; 
        char     GPRSUserId[20];
        char     GPRSPassword[20];
        char     SMSCentreNum[20];
        char     AppSrvTransKey[20];
        char     M2MSMSpecialNum[20];
        char     M2MURLAddr[20];
        u32      M2MIPAddr;
        u32      M2MPort;
        char     AppSMSCentreNum[20];
        char     AppSrvURLAddr[20];
        u32      AppSrvIPAddr;
        u32      AppSrvPort;
        //reserve
        char     DNSAddr[20];
        u32      Period;
        char     AppUSSDSMSSpecialNum[20];
        char     M2MAppAPN[20];
        char     BackupDNSAddr[20];
        char     TLVSyncTagValue[20];// max 10 Sync TLV ;
        u16      SMSRecvOvertime;
        u8       MaxLoginRetryTimes;
        char     DefaultECCode[20];
        u16      LocalPTRValue;
        u8       LocalPortMode;
        u8       LocalPorthalt;
        u8       LocalPortVerify;
        u16      LocalPortScanPeriod;
        //reserve
        u32      DataPackRespOvertime;
        u8       TransFailRetryTimes;
        u8       GateLimit;
    }WmmpCfgContext_t;

    typedef struct WmmpUpdateContextTag
    {
        u8       UpdateMode;
        char     UpdateSrvURLAddr[20];
        char     UpdateSrvIPAddr[20];
        u16      UpdateSrvPort;
        char     UpdateSrvLoginPassword[20];
        char     UpdateFileRev[20];/*0x1005*/
        u32      UpdateFileLength;
        u32      needDLFileLen;
        u16      CRC16Check;
        u32      CRC32Check;
        u16      UpdateAffairTransId;
        char     FileReleaseData[12];
        u8       ForcetoUpdateParam;
        char     CurrentFileRev[20];/*0x100c*/
        u8       UpdateFileMajorRev;
        u8       UpdateFileMinorRev;
        u8       adFileStatus;
        u32      adFileFileReadPoint;
        u8       adFileFlag;
        //u8     adFileEndFileReqFlag;
        char     TransId[4];
    }WmmpUpdateContext_t;


    typedef struct WmmpStatisticContextTag
    {
        u8       StatInfoReport;
        u32      ReportTime;
        u32      PeriodReport;
        u8       MonthlyOperationSign;
        u8       ClearStatData;
        u32      StatStartTime;
        u32      statFinishTime;
        u32      SMSDeliverySucc;
        u32      SMSDeliveruFail;
        u32      SMSReceivedNum;
        u32      GRPSDataSize;
        u32      USSDSuccNum;
        u32      USSDFailNum;
        u32      USSDReceivedNum;
        char     SMSStatParam[12];
        char     SMSStatAutoReportParam[12];
        char     M2MMutualSMSStat[MAX_STAT_NUM][8];
        char     OperationMutualSMSStat[MAX_STAT_NUM][8];
        char     GPRSOperationStatParam[12];
        char     GPRSOperationStatReportParam[12];
        char     M2MMutualGPRSDataSize[MAX_STAT_NUM][12];
        char     OperationMutualGPRSDataSize[MAX_STAT_NUM][12];
        char     USSDStatParam[12];
        char     USSDStatReportParam[12];
        char     M2MMutualUSSDStat[MAX_STAT_NUM][12];
        char     OperationMutualUSSDStat[MAX_STAT_NUM][12];
    }WmmpStatisticContext_t;

    typedef struct WmmpTermStatusContextTag
    {
        char     ProtocolRev[3];
        char     TerminalSeriesNum[17];
        char     TerminalSeriesNum_Bak[17];
        char     IMSINum[16];
        char     FirmwareRev[9];
        u32      PrevPeriod;
        u32      CellularId;
        u8       SignalLevel;
        u8       InitParaFlag;
        u32      SystemTime;
        //preserve
        u8       AlarmStatus;
        u8       AlarmType;
        u16      AlarmCode;
        char     RestoreAlarm[20];
        char     DEVStatus[20];
        char     SupportedDev[20];
        u32      PackLostNum;
        u32      FailWarning;
        u8       Majorver;
        u8       MinorVer;
        u8       SupportedDev_Len;
        char     wcomDEVStatus[MIN_STR_PARAM_LEN];
    }WmmpTermStatusContext_t;
    
    typedef struct WmmControlContextTag
    {
        u8       SetCommand;
        u8       ConnectMode;
        u8       DataTransMode;
        u16      Period;
        u8       CollectDataTransMode;
        char     M2MConnectModeOnIP[3];
        char     UserDataID[20];
        char     SubPackModeParam[20];
        u8       PlatformParamOpt;
        //
        char     ExecuteParamOpt[6];
        char     DesSeriesNum[17];
        char     SrcSeriesNum[17];
        char     DesECCode[20];
        char     SrcECCode[20];
        char     TransCtrlCommandStaticParam[20];
        char     VerifyTLVByCRC32[5];
        char     ExternalDevCtrl[20];
    }WmmpControlContext_t;
    
    typedef struct WmmpSecurityContextTag
    {
        //pin 1 
        char     SIMPin1[9];                 
        char     SIMPin1MD5[17];             
        u8       SIMPin1Active;          
    
        //pin 2
        char     SIMPin2[9];                  
        char     SIMPin2MD5[17];             
        u8       SIMPin2Active;          
    
        //puk 1
        char     SIMPuk1[9];                 
        char     SIMPuk1MD5[17];         
        u8       SIMPuk1Active;          
    
        //puk 2
        char     SIMPuk2[9];                 
        char     SIMPuk2MD5[17];         
        u8       SIMPuk2Active;          
    
        //M2M Terminal - SIM card binding TLV
        u8       BindParaActive;         //activate SIM-Equipment binding parameters
        char     BindIMEI[17];                   //IMEI 
        char     BindIMEIMD5[17];                //IMEI MD5
        char     BindIMEIOrig[17];               //original IMEI
        char     BindIMEIOrigMD5[17];            //original IMEI MD5
        char     BindIMSI[16];                   //IMSI
        char     BindIMSIMD5[17];                //IMSI MD5
        char     BindIMSIOrig[16];               //original IMSI
        char     BindIMSIOrigMD5[17];            //original IMSI MD5
        char     BindCurrentMD5[17];             //current binding MD5(termial serial number+IMEI+IMSI)
        char     BindOriginalMD5[17];                //original binding MD5
        u16      Pin1ErrorAllow;         //number of pin 1 error allowd, and errors occured      
        u16      Pin2ErrorAllow;         //number of pin 2 error allowd, and errors occured
        u16      Puk1ErrorAllow;         //number of puk 1 error allowd,and errors occured
        u16      Puk2ErrorAllow;         //number of puk 2 error allowd, and errors occured
    
        //M2M terminal security parameters clear indicator
        u8       M2MPassClearPara;       
    
        //M2M session initiate parameters
        char     InitPara[17];                   //MD5 for session initiate security parameters
        u8       InitParaUsed;               //initiation parameters used 
        char     InitCommand[20];                //command_id of the initation command
        char     InitTLVactTag[20];              //the TLV tag being authanticated
    
        //Uplink connect parameters
        char     uplinkPassword[9];              
        char     uplinkPasswordMD5[17];  
        u32      uplinkPasswordExpDate;      //password experation date
    
        //downlink connect parameters
        char     downlinkPassword[9];
        char     downlinkPasswordMD5[16];        
        u32      downlinkPasswordExpDate;    
    
        //connection encryption parameters
        u8       encryptionSupport;
        u8       encryptionUsed;
        char     encryptionCommand[20];
        char     encrtyptionTag[20];
        u8       encryptionAlgorithm;
    
        //encryption keys parameters
        char     publickey[32];//20 too short by zyc
        char     privatekey[20];
        u32      publicKeyExpDate;
        u8       encryptionKeyAlgorithm;
        char        encryptionKeyMD5[17];
    
        //conversation keys parameters
        char     convPublicKey[32];//20 too short by zyc
        char     convPrivateKey[20];
        u32      convKeyExpDate;
        u8       convEncryptionAlog;
        char     convEncryptionMD5[17];
        char     convEncryptionMsgBody[20];
        u8       convAuthFailReport;
        char     convSingleEncryptBody[20];
        char     convMultiEncryptBody[20];
        char     convPackedEncryptBody[20];
    
        //bool       LoginStatus;
    }WmmpSecurityContext_t;
    
    
    typedef struct WmmpCustomContextTag
    {
        u8      TLVSyncTagNum;   //the num of 0x0025
        u16     MaxPDULength;
        char    UnregTerminalNum[17];
    
        //供统计功能使用的变量
        u32     GPRSM2MRecvStat;   
        u32     GPRSM2MSendStat;
        u32     GPRSAppRecvStat;
        u32     GPRSAppSendStat;
        u16     SMSM2MRecvStat;   
        u16     SMSM2MSendStat;
        u16     SMSAppRecvStat;
        u16     SMSAppSendStat; 
    
        u8      GPRSM2MStatRecID;
        u8      SMSM2MStatRecID;
    
        u32     GPRSM2MStatRecFirstTime; //记录第一次使用当前记录号的时间
        u32     SMSM2MStatRecFirstTime;
        u32     GPRSM2MStatTrapFirstTime;
        u32     SMSM2MStatTrapFirstTime;
    
    
        u8      LastTrappedMonth;  //本月首次业务上报
        bool isCurrentMonthTrapped;   //这2个变量需要保存,应该转移到自定义tlv中
        //char      CustomGenStrParam[MAX_PARAM_NUMBER][GENERAL_STR_PARAM_LEN];
        //u8     custGenStrParamCount;
        //u32    CustomGenStrParamTagId[MAX_PARAM_NUMBER];
        u8      remoteCtrlTraceEnabel;  /*RMCTRLTRACEENABLE*/
        
    }WmmpCustomContext_t;
    
    typedef struct WmmpCustom2ContextTag
    {   
        u8      AppSerConnMode[MAX_WMMP_APP_NUM];
        u32     AppSerAddr[MAX_WMMP_APP_NUM];
        u16     AppSerPort[MAX_WMMP_APP_NUM];
        u8      AppSerType[MAX_WMMP_APP_NUM];  /*04---GPRS*/    
        char    AppServerString[MAX_WMMP_APP_NUM][MAX_SERVER_ADDR];/*SIMCOM luyong 2012-07-13 Fix MKBug00013342   modify*/  
    }WmmpCustom2Context_t;
    
    typedef struct WmmpCustom3ContextTag
    {   
    
        char     rmCtrlTraceATNParmCfglist[MIN_STR_RMCTRL_TRACE_AT];             /*RMCTRLTRACEATCFGLISTN*/
        
        char     rmCtrlTraceATNParmCfg[MID_STR_RMCTRL_TRACE_AT]; /*modify 20110721 MAX_STR_RMCTRL_TRACE_AT*/ /*RMCTRLTRACEATCFGLISTN*/
        
        char     rmCtrlTraceATNParmStat[MIN_STR_RMCTRL_TRACE_AT];        /*RMCTRLTRACEATM2MSTAN*/
    
        char     rmCtrlTraceATNParmTrap[MID_STR_RMCTRL_TRACE_AT];    /*modify 20110721 MAX_STR_RMCTRL_TRACE_AT*/ /*RMCTRLTRACEATTRAPN*/
    
        char     rmCtrlTraceATNParmAppopen[MIN_STR_RMCTRL_TRACE_AT];     /*RMCTRLTRACEATTRAPN*/
        
        char     rmCtrlTraceATAckAppopen[MIN_STR_RMCTRL_TRACE_AT];               /*RMCTRLTRACEATTRAPRET*/        
        char     rmCtrlTraceATNParmAppclose[MIN_STR_RMCTRL_TRACE_AT];        /*RMCTRLTRACEATTRAPN*/
    
        char     rmCtrlTraceATNParmAppcloseAlready[MIN_STR_RMCTRL_TRACE_AT];
    
        char     remoteCtrlTraceATRecvData[MIN_STR_RMCTRL_TRACE_AT];
        char     remoteCtrlTraceATSocDestroy[MIN_STR_RMCTRL_TRACE_AT];
        char     remoteCtrlTraceATNamParm[MIN_STR_RMCTRL_TRACE_AT];  /*RMCTRLTRACEATNAME*/       
    
        char     remoteCtrlTraceATAck[MIN_STR_RMCTRL_TRACE_AT];                  /*RMCTRLTRACEATACK*/
        char     remoteCtrlTraceATMsg[MAX_STR_RMCTRL_TRACE_AT];                  /*RMCTRLTRACEATMSG*/
        char     remoteCtrlTraceATNamParmBhv[MIN_STR_RMCTRL_TRACE_AT];/*RMCTRLTRACEATBHVNAME*/
    
        char     rmCtrlTraceATNamParmbrOpen[MIN_STR_RMCTRL_TRACE_AT];/*RMCTRLTRACEATBROPENNAME*/
    
        char     rmCtrlTraceATNamParmbrSta[MIN_STR_RMCTRL_TRACE_AT];/*RMCTRLTRACEATBROPENNAME*/
    
        char     rmCtrlTraceATNamParmcgpAddr[MIN_STR_RMCTRL_TRACE_AT];/*RMCTRLTRACEATBROPENNAME*/
    
        char     remoteCtrlTraceATAckack[MIN_STR_RMCTRL_TRACE_AT];           /*RMCTRLTRACEATACKACK*/
        char     remoteCtrlTraceATMsgClosedInd[MIN_STR_RMCTRL_TRACE_AT];     /*RMCTRLTRACEATMSGCLOSEDIND*/
        char     rmtTraceATMsgClosedIndTimeOut[MIN_STR_RMCTRL_TRACE_AT]; /*RMCTRLTRACEATMSGCLOSEDINDTOUT*/
    /*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_ADC_GPIO__
        u32     wmmp_adc_value;
        u32     wmmp_gpio_input;
        u32     wmmp_gpio_status;
#endif
    /*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
    
        
    }WmmpCustom3Context_t;
#endif      /*__SIMCOM_WMMP__*/


/******************************************************************************
* Constants
******************************************************************************/

#define		NVRAM_EF_WMMP_TLV_CFG_SIZE			sizeof(WmmpCfgContext_t) //1000

#define		NVRAM_EF_WMMP_TLV_UPDATE_SIZE			sizeof(WmmpUpdateContext_t)   //1000

#define		NVRAM_EF_WMMP_TLV_STAT_SIZE			sizeof(WmmpStatisticContext_t)  //1000

#define		NVRAM_EF_WMMP_TLV_STATUS_SIZE			sizeof(WmmpTermStatusContext_t)  //1000

#define		NVRAM_EF_WMMP_TLV_CTRL_SIZE			sizeof(WmmpControlContext_t)  //1000

#define		NVRAM_EF_WMMP_TLV_SECURITY_SIZE			sizeof(WmmpSecurityContext_t)   //1000

#define		NVRAM_EF_WMMP_TLV_CUSTOM_SIZE				sizeof(WmmpCustomContext_t)  //1000

#define		NVRAM_EF_WMMP_TLV_CUSTOM2_SIZE				sizeof(WmmpCustom2Context_t)  //1000

#define		NVRAM_EF_WMMP_TLV_CUSTOM3_SIZE				sizeof(WmmpCustom3Context_t)  //1000

#define     NVRAM_EF_WMMP_ACCOUNT_SIZE         (sizeof(Wmmp_mmi_abm_account_info_struct))

#define     NVRAM_EF_WMMP_WOPEN_SIZE         (sizeof(Wmmp_atc_value_struct))

/***************************************************************************
* External Functions Prototypes
***************************************************************************/

/***************************************************************************
* Local Functions Prototypes
***************************************************************************/

#endif//__SIMCOM_WMMP_NVRAM_DEFINE_H__

