/** \file
 *  \
 */
/*
******************************************************************************
*                 |
*  File Name      | simcom_wmmp_nvram.c
*-----------------|-----------------------------------------------------------
*  Project        | SIMCOM800V
*-----------------|-----------------------------------------------------------
*  Created        | 03 -- 12 , 2013
*-----------------|-----------------------------------------------------------
*  Description    | This file contains nvram functions.
*-----------------|-----------------------------------------------------------
*                 | Copyright (c) 2013 SIMCOM Corp.
*                 | All Rights Reserved.
*                 |
******************************************************************************
*/

/*************************************************************************
* Include Statements
 *************************************************************************/
#include "stdarg.h"
#include "simcom_wmmp_nvram_define.h"
#include "simcom_wmmp_utility.h"
#include "wmmp_interface.h"

#include "eat_nvram.h"
#include "eat_fs_errcode.h" 
#include "eat_fs_type.h" 
#include "eat_fs.h"

/********************************************************************
* Macros
 ********************************************************************/

/********************************************************************
* Types
 ********************************************************************/
extern Wmmp_atc_value_struct atcValueNv;

/********************************************************************
* Extern Variables (Extern /Global)
 ********************************************************************/
extern u8 gwopenValue;
/********************************************************************
* Local Variables:  STATIC
 ********************************************************************/
#if defined(__SIMCOM_WMMP__) /*SIMCOM Laiwenjie 2013-01-15 Fix MKBug00014691 for change nvram interface*/
WmmpCfgContext_t const	NVRAM_EF_WMMP_TLV_CFG_DEFAULT =
{
	{""},  		 	//GPRSDailNum[20];
	{"CMNET"}, 	//M2MAPN[20];	
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  modify end*/
	{""}, 	//GPRSUserId[20];
	{""}, 	//GPRSPassword[20];
	{""}, 	//SMSCentreNum[20];
	{""}, 	//AppSrvTransKey[20];
//		{"1065840436"},		//{"13795339057"}, 	//M2MSMSpecialNum[20];
#ifdef __SIMCOM_WMMP_TEST_PLATFORM__
	{"1065717341"},// chongqin test platform sms port number
#else 
	{"106571734"},// chongqin normal platform sms port number
#endif

	{""}, 	//M2MURLAddr[20];
	//		0xDAC92D63,	//218.201.45.99  chongqing {0x00000000}, 		//M2MIPAddr;
	//		0x26FD26FD, 	//udp 9981 tcp 9981 	//M2MPort;	
#ifdef __SIMCOM_WMMP_TEST_PLATFORM__
	 0xDAC92D63, // chongqin test platform ip address //M2MIPAddr;
	 0x26FD0000,// chongqin test  platform port //M2MPort;
#else
	 0xDACE050F, // chongqin normal platform ip address //M2MIPAddr;
	 0x26FD0000,// chongqin normal platform port //M2MPort;
#endif

	{""}, 	//AppSMSCentreNum[20];
	{""}, 	//AppSrvURLAddr[20];
	0x00000000, 		//AppSrvIPAddr;
	0x00000000, 		//AppSrvPort;
	//reserve
	{""}, 	//DNSAddr[20];
	0x0000001E, 	//chongqin normal platform    	//Period;
	{""}, 	//AppUSSDSMSSpecialNum[20];
	{""}, 	//M2MAppAPN[20];
	{""}, 	//BackupDNSAddr[20];
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  modify begin*/
#ifdef __SIMCOM_WMMP_PRIVATE_APN__
	{0x00, 0x07, 0x00, 0x09, 0x00, 0x0A, 0x00, 0x11, 0x00, 0x00,},		//TLVSyncTagValue[20];// max 10 Sync TLV ;
#else
	{0x00, 0x02, 0x00, 0x07, 0x00, 0x09, 0x00, 0x0A, 0x00, 0x11, 0x00, 0x00,},		//TLVSyncTagValue[20];// max 10 Sync TLV ;
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  modify end*/
	0x005A,	//SMSRecvOvertime;
	0x03,		//MaxLoginRetryTimes;
	{""}, 	//DefaultECCode[20];
	0x0000,	//LocalPTRValue;
	0x00,		//LocalPortMode;
	0x00,		//LocalPorthalt;
	0x00,		//LocalPortVerify;
	0x0000,	//LocalPortScanPeriod;
	//reserve
	0x00000000, 		//DataPackRespOvertime;
	0x00,		//TransFailRetryTimes;
	0x00		//GateLimit;		
};

WmmpUpdateContext_t const	NVRAM_EF_WMMP_TLV_UPDATE_DEFAULT = 
{
	0x00,		//UpdateMode;
	{""},   		//UpdateSrvURLAddr[20];
	{""},   	//UpdateSrvIPAddr[20];
	0x0000,	//UpdateSrvPort;
	{""},   	//UpdateSrvLoginPassword[20];
	{""},   	//UpdateFileRev[20];
	0x00000000, 		//UpdateFileLength;     /*0x1005*/
	0x00000000,                    //UINT32       needDLFileLen;
	0x0000,	//CRC16Check;
	0x00000000, 		//CRC32Check;
	0x0000,	        //UpdateAffairTransId;
	{""},   	    //FileReleaseData[12];
	0x00,		    //ForcetoUpdateParam;
	{""},             //Char        CurrentFileRev[20];/*0x100c*/
	0x00,           //UINT8        UpdateFileMajorRev;
	0x00,           //UINT8        UpdateFileMinorRev;
	0x00,           //UINT8        adFileStatus;
	0x00000000,     //UINT32   adFileFileReadPoint;
	0x00,           //UINT8        adFileFlag;
	{""},             //Char    TransId[4];
};
	
WmmpStatisticContext_t const	NVRAM_EF_WMMP_TLV_STAT_DEFAULT = 
{
	0xff,		//StatInfoReport;
	0xffffffff, 		//ReportTime;
	0x00000000, 		//PeriodReport;
	0x00,		//MonthlyOperationSign;
	0x00,		//ClearStatData;
	0xffffffff, 		//StatStartTime;
	0xffffffff, 		//statFinishTime;
	0x00000000, 		//SMSDeliverySucc;
	0x00000000, 		//SMSDeliveruFail;
	0x00000000, 		//SMSReceivedNum;
	0x00000000, 		//GRPSDataSize;
	0x00000000, 		//USSDSuccNum;
	0x00000000, 		//USSDFailNum;
	0x00000000, 		//USSDReceivedNum;
	{""},   				//SMSStatParam[12];
	{""},   				//SMSStatAutoReportParam[12];
	{"", "", "","","","", "", "","",""},   	//M2MMutualSMSStat[MAX_STAT_NUM][8];
	{"", "", "","","","", "", "","",""},   	//OperationMutualSMSStat[MAX_STAT_NUM][8];
	{""},   							//GPRSOperationStatParam[12];
	{""},   							//GPRSOperationStatReportParam[12];
	{"", "", "","","","", "", "","",""},   	//M2MMutualGPRSDataSize[MAX_STAT_NUM][12];
	{"", "", "","","","", "", "","",""},  	//OperationMutualGPRSDataSize[MAX_STAT_NUM][12];
	{""},   							//USSDStatParam[12];
	{""},   							//USSDStatReportParam[12];
	{"", "", "","","","", "", "","",""} ,   	//M2MMutualUSSDStat[MAX_STAT_NUM][12];
	{"", "", "","","","", "", "","",""}   	//OperationMutualUSSDStat[MAX_STAT_NUM][12];

};

WmmpTermStatusContext_t	const	NVRAM_EF_WMMP_TLV_STATUS_DEFAULT =
{
	"30",   				    //Char ProtocolRev[3];
	//"A301300109000007",   	//cmcc lab test  //TerminalSeriesNum[17];
	//("A311030100000000") 重移测试  ("A972030000000000") 重移正式
#ifdef __SIMCOM_WMMP_TEST_PLATFORM__		
	"A301030200000000",         //chongqin: test platform  Char        TerminalSeriesNum[17];
	"A301030200000000",         //chongqin: test platform Char        TerminalSeriesNum_Bak[17];
#else        
	"A972030000000000",         //chongqin: normal platform registed TerminalSeriesNum[17] A972030011000015; unregisted A972030011000000
	"A972030000000000",         //registed TerminalSeriesNum_Bak[17];
#endif

	"",   					//IMSINum[16];
	"0.01",   					//FirmwareRev[9];	"%d.%02d"	
	0x00000000, 		//PrevPeriod;
	0x00000000, 		//CellularId;
	0x00,		//SignalLevel;
	0x00,		//InitParaFlag;
	0x00000000, 		//SystemTime;
	//preserve
	0x00,		//AlarmStatus;
	0x00,		//AlarmType;
	0x0000,		//AlarmCode;
	"",		   	//RestoreAlarm[20];
	"000100000300",   			//DEVStatus[20];
	"000100020003",   			//SupportedDev[20];
	0x00000000, 		//PackLostNum;
	0x00000000, 		//FailWarning;
	0x00,           //UINT8            Majorver;
	0x01,           //UINT8            MinorVer;
	0x0C,           //UINT8            SupportedDev_Len;
	"01,0,03,0"     //Char wcomDEVStatus[MIN_STR_PARAM_LEN]  MIN_STR_PARAM_LEN=25;
};

WmmpControlContext_t	 const	NVRAM_EF_WMMP_TLV_CTRL_DEFAULT=
{
	0x00,		//SetCommand;
	0x01,		//ConnectMode;   //0---udp short link 1---udp long link  2--tcp short link 3--tcp long link 4-- sms
	0x00,		//DataTransMode;
	0x0000,	//Period;
	0x00,		//CollectDataTransMode;
	{""},   	//M2MConnectModeOnIP[3];
	{""},   	//UserDataID[20];
	{""},   	//SubPackModeParam[20];
	0x00,		//PlatformParamOpt;
	//
	{""},   	//ExecuteParamOpt[6];
	{""},   	//DesSeriesNum[17];
	{""},   	//SrcSeriesNum[17];
	{""},   	//DesECCode[20];
	{""},   	//SrcECCode[20];
	{""},   	//TransCtrlCommandStaticParam[20];
	{""},   	//VerifyTLVByCRC32[5];
	{""}   	//ExternalDevCtrl[20];		
};

WmmpSecurityContext_t const 	NVRAM_EF_WMMP_TLV_SECURITY_DEFAULT= 
{
	//pin 1 
	"12345678",   	//SIMPin1[9];					
	"",   	//SIMPin1MD5[17];				
	0x00,		//SIMPin1Active;			

	//pin 2
	"",   	//SIMPin2[9];					 
	"",   	//SIMPin2MD5[17];				
	0x00,		//SIMPin2Active;			

	//puk 1
	"",   	//SIMPuk1[9];					
	"",   	//SIMPuk1MD5[17];			
	0x00,		//SIMPuk1Active;			

	//puk 2
	"",   	//SIMPuk2[9];					
	"",   	//SIMPuk2MD5[17];			
	0x00,		//SIMPuk2Active;			

	//M2M Terminal - SIM card binding TLV
	0x00,		//BindParaActive;			//activate SIM-Equipment binding parameters
	"",   	//BindIMEI[17];					//IMEI 
	"",   	//BindIMEIMD5[17];				//IMEI MD5
	"",   	//BindIMEIOrig[17];				//original IMEI
	"",   	//BindIMEIOrigMD5[17];			//original IMEI MD5
	"",   	//BindIMSI[16];					//IMSI
	"",   	//BindIMSIMD5[17];				//IMSI MD5
	//"460008054641287", "460001574129022",  	//BindIMSIOrig[16];				//original IMSI
	"",  //460007914127292      
	"",   	//BindIMSIOrigMD5[17];			//original IMSI MD5
	"",   	//BindCurrentMD5[17];				//current binding MD5(termial serial number+IMEI+IMSI)
	"",   	//BindOriginalMD5[17];				//original binding MD5
	0x0000,	//Pin1ErrorAllow;			//number of pin 1 error allowd, and errors occured		
	0x0000,	//Pin2ErrorAllow;			//number of pin 2 error allowd, and errors occured
	0x0000,	//Puk1ErrorAllow;			//number of puk 1 error allowd,and errors occured
	0x0000,	//Puk2ErrorAllow;			//number of puk 2 error allowd, and errors occured

	//M2M terminal security parameters clear indicator
	0x00,		//M2MPassClearPara;		

	//M2M session initiate parameters
	"",   	//InitPara[17];					//MD5 for session initiate security parameters
	0x00,		//InitParaUsed;				//initiation parameters used 
	"",   	//InitCommand[20];				//command_id of the initation command
	"",   	//InitTLVactTag[20];				//the TLV tag being authanticated

	//Uplink connect parameters
	"12345678",   	//uplinkPassword[9];				
	"",   	//uplinkPasswordMD5[17];	
	0xffffffff, 		//uplinkPasswordExpDate;		//password experation date

	//downlink connect parameters
	"12345678",   	//downlinkPassword[9];
	"",   	//downlinkPasswordMD5[16];		
	0xffffffff, 		//downlinkPasswordExpDate;	

	//connection encryption parameters
	0x00,		//encryptionSupport;
	0x00,		//encryptionUsed;
	"",   	//encryptionCommand[20];
	"",   	//encrtyptionTag[20];
	0x00,		//encryptionAlgorithm;

	//encryption keys parameters
	"",   	//publickey[32];//20 too short by zyc
	"",   	//privatekey[20];
	0xffffffff, 		//publicKeyExpDate;
	0x00,		//encryptionKeyAlgorithm;
	"",   	//encryptionKeyMD5[17];

	//conversation keys parameters
	"",   	//convPublicKey[32];//20 too short by zyc
	"",   	//convPrivateKey[20];
	0x00000000, 		//convKeyExpDate;
	0x00,		//convEncryptionAlog;
	"",   	//convEncryptionMD5[17];
	"",   	//convEncryptionMsgBody[20];
	0x00,		//convAuthFailReport;
	"",   	//convSingleEncryptBody[20];
	"",   	//convMultiEncryptBody[20];
	""  	//convPackedEncryptBody[20];				
	//Boolean		LoginStatus;		
};
	  
WmmpCustomContext_t const	NVRAM_EF_WMMP_TLV_CUSTOM_DEFAULT =
{
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  modify begin*/
#ifdef __SIMCOM_WMMP_PRIVATE_APN__
	0x04,		            //TLVSyncTagNum;   //the num of 0x0025
#else
	0x05,		            //TLVSyncTagNum;   //the num of 0x0025
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  modify end*/
	0x0384,	                //900	// MaxPDULength;
	//		"A307030100000000",     //Char  UnregTerminalNum[17];
	//             "A972030000000000", //Char  chongqi normal platform  UnregTerminalNum[17];

#ifdef __SIMCOM_WMMP_TEST_PLATFORM__		
	"A301030200000000", //Char  chongqing test platform UnregTerminalNum[17];
#else   
	"A972030000000000", //Char  chongqing normal platform  UnregTerminalNum[17];
#endif

	//供统计功能使用的变量
	0x00000000, 		// GPRSM2MRecvStat;   
	0x00000000, 		// GPRSM2MSendStat;
	0x00000000, 		//GPRSAppRecvStat;
	0x00000000, 		//GPRSAppSendStat;
	0x0000,		// SMSM2MRecvStat;   
	0x0000,		// SMSM2MSendStat;
	0x0000,		//SMSAppRecvStat;
	0x0000,		//SMSAppSendStat; 

	0x00,		// GPRSM2MStatRecID;
	0x00,		// SMSM2MStatRecID;

	0x00000000, 		//GPRSM2MStatRecFirstTime; //记录第一次使用当前记录号的时间
	0x00000000, 		//SMSM2MStatRecFirstTime;
	0x00000000, 		//GPRSM2MStatTrapFirstTime;
	0x00000000, 		//SMSM2MStatTrapFirstTime;

	0x00,		// LastTrappedMonth;  //本月首次业务上报
	0,			//Boolean isCurrentMonthTrapped;   //这2个变量需要保存,应该转移到自定义tlv?	
	0x00,           //UINT8        remoteCtrlTraceEnabel;  /*RMCTRLTRACEENABLE*/
};

/*SIMCOM luyong 2012-07-13 Fix MKBug00013342   begin*/
WmmpCustom2Context_t    NVRAM_EF_WMMP_TLV_CUSTOM2_DEFAULT = 
{
   	{0x00,0x00,0x00,0x00,0x00,0x00,0x00},//UINT8  	AppSerConnMode[MAX_APP_NUM];
   	{0xDACE18C3,0xDB860AC7,0xD2B040CC,0x74E4DD33,0xDAC90AC7,0x74ECDD4B,0x74ECDD4B},//UINT32  	AppSerAddr[MAX_APP_NUM];
   	{0x139D,0xEB14,0x13B4,0x15B3,0x304e,0x1A72,0x1A6E},//UINT16  	AppSerPort[MAX_APP_NUM];
   	{0x04,0x04,0x04,0x04,0x04,0x04,0x04},//UINT8   	AppSerType[MAX_APP_NUM];  /*04---GPRS*/		
	{"4tcp://218.206.24.195:5021","4tcp://219.134.185.69:60180","4tcp://210.176.64.204:5044","4tcp://116.228.221.51:5555","4tcp://218.201.10.199:12366","4tcp://116.231.221.75:6770","4tcp://116.228.221.51:6766"},       //Char 	AppServerString[MAX_APP_NUM][MAX_SERVER_ADDR];
};
/*SIMCOM luyong 2012-07-13 Fix MKBug00013342   end*/

WmmpCustom3Context_t    NVRAM_EF_WMMP_TLV_CUSTOM3_DEFAULT =
{
    	"",     //Char		rmCtrlTraceATNParmCfglist[MIN_STR_RMCTRL_TRACE_AT];				/*RMCTRLTRACEATCFGLISTN*/    	
    	"",     //Char		rmCtrlTraceATNParmCfg[MID_STR_RMCTRL_TRACE_AT];	/*modify 20110721 MAX_STR_RMCTRL_TRACE_AT*/	/*RMCTRLTRACEATCFGLISTN*/    	
    	"",     //Char		rmCtrlTraceATNParmStat[MIN_STR_RMCTRL_TRACE_AT];		/*RMCTRLTRACEATM2MSTAN*/
    	"",     //Char		rmCtrlTraceATNParmTrap[MID_STR_RMCTRL_TRACE_AT];	/*modify 20110721 MAX_STR_RMCTRL_TRACE_AT*/	/*RMCTRLTRACEATTRAPN*/
	    "",     //Char		rmCtrlTraceATNParmAppopen[MIN_STR_RMCTRL_TRACE_AT];		/*RMCTRLTRACEATTRAPN*/    	
    	"",     //Char		rmCtrlTraceATAckAppopen[MIN_STR_RMCTRL_TRACE_AT];				/*RMCTRLTRACEATTRAPRET*/		
    	"",     //Char		rmCtrlTraceATNParmAppclose[MIN_STR_RMCTRL_TRACE_AT];		/*RMCTRLTRACEATTRAPN*/
    	"",     //Char		rmCtrlTraceATNParmAppcloseAlready[MIN_STR_RMCTRL_TRACE_AT];
    	"",     //Char		remoteCtrlTraceATRecvData[MIN_STR_RMCTRL_TRACE_AT];
	    "",     //Char		remoteCtrlTraceATSocDestroy[MIN_STR_RMCTRL_TRACE_AT];
    	"",     //Char		remoteCtrlTraceATNamParm[MIN_STR_RMCTRL_TRACE_AT];	/*RMCTRLTRACEATNAME*/
       	"",     //Char		remoteCtrlTraceATAck[MIN_STR_RMCTRL_TRACE_AT];					/*RMCTRLTRACEATACK*/
    	"",     //Char		remoteCtrlTraceATMsg[MAX_STR_RMCTRL_TRACE_AT];					/*RMCTRLTRACEATMSG*/
    	"",     //Char		remoteCtrlTraceATNamParmBhv[MIN_STR_RMCTRL_TRACE_AT];/*RMCTRLTRACEATBHVNAME*/
    	"",     //Char		rmCtrlTraceATNamParmbrOpen[MIN_STR_RMCTRL_TRACE_AT];/*RMCTRLTRACEATBROPENNAME*/
    	"",     //Char		rmCtrlTraceATNamParmbrSta[MIN_STR_RMCTRL_TRACE_AT];/*RMCTRLTRACEATBROPENNAME*/
    	"",     //Char		rmCtrlTraceATNamParmcgpAddr[MIN_STR_RMCTRL_TRACE_AT];/*RMCTRLTRACEATBROPENNAME*/
    	"",     //Char		remoteCtrlTraceATAckack[MIN_STR_RMCTRL_TRACE_AT];			/*RMCTRLTRACEATACKACK*/
    	"",     //Char		remoteCtrlTraceATMsgClosedInd[MIN_STR_RMCTRL_TRACE_AT];		/*RMCTRLTRACEATMSGCLOSEDIND*/
    	"",     //Char		rmtTraceATMsgClosedIndTimeOut[MIN_STR_RMCTRL_TRACE_AT];	/*RMCTRLTRACEATMSGCLOSEDINDTOUT*/	
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_ADC_GPIO__
	0x0,
	0x0,
	0xffff0000
#endif
 /*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/
};

static const Wmmp_mmi_abm_account_info_struct NVRAM_EF_WMMP_ACCOUNT_DEFAULT[] = 
{
	/*Context 0*/
	{
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /*authentication_type, dns, sec_dns*/
	        
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //ps_account_info->user_name
	        
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //pwword
	        
	        0x63, 0x6d, 0x6e, 0x65, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	        
	        0x00, 0x00, 0x00, 0x00,  //apn: cmnet
	        0x05, //apn_length
	        0x00, 
	        0x00,
	        0x00, 
	        0x00,  
	        0x00, 0x00, 0x00, 0x00, 
	        
	        0x31, 0x37, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//csd_account_info->user_id
	        
	        0x31, 0x37, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//user_pw
	        
	        0x31, 0x37, 0x32, 0x30, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //addr_str
	        0x02, 
	        0x00, 
	        0x00, 
	},
	
	/*Context 1*/
	{
	        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /*authentication_type, dns, sec_dns*/
	        
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //ps_account_info->user_name
	        
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //pwword
	        
	        0x63, 0x6d, 0x77, 0x61, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	        
	        0x00, 0x00, 0x00, 0x00,  //apn: cmwap
	        0x05, //apn_length
	        0x00, 
	        0x00,
	        0x00, 
	        0x00,  
	        0x00, 0x00, 0x00, 0x00, 
	        
	        0x31, 0x37, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//csd_account_info->user_id
	        
	        0x31, 0x37, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//user_pw
	        
	        0x31, 0x37, 0x32, 0x30, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //addr_str
	        0x00, 
	        0x00, 
	        0x00, 
	},	
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_PRIVATE_APN__
	/*Context 2*/
	{
	        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  /*authentication_type, dns, sec_dns*/
	        
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //ps_account_info->user_name
	        
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //pwword
	        
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	        
	        0x00, 0x00, 0x00, 0x00,  //apn:
	        0x00, //apn_length
	        0x00, 
	        0x00,
	        0x00, 
	        0x00,  
	        0x00, 0x00, 0x00, 0x00, 
	        
	        0x31, 0x37, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//csd_account_info->user_id
	        
	        0x31, 0x37, 0x32, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,//user_pw
	        
	        0x31, 0x37, 0x32, 0x30, 0x31, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  //addr_str
	        0x00, 
	        0x00, 
	        0x00, 
	}
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/
};

static const u8 NVRAM_EF_WMMP_WOPEN_DEFAULT =
{
        0x01
};
#endif      /*__SIMCOM_WMMP__*/

/********************************************************************
* External Functions declaration
 ********************************************************************/

/********************************************************************
* Local Function declaration
 ********************************************************************/
void wmmpProcessSave(u16 ef_id, void *data_ptr, u16 length);

/********************************************************************
* Local Function
 ********************************************************************/
void wmmp_nvram_init()
{

    AppWmmpContext_t *appWmmpContext_p = ptrToAppWmmpContext();
    WmmpNetWorkContext_t        *networkContext_p;

    eat_trace("wmmp_nvram_init()entry");

    //将默认值读入全局变量
    
    memcpy((void *)&atcValueNv,(void *)&NVRAM_EF_WMMP_WOPEN_DEFAULT,sizeof(Wmmp_atc_value_struct));
    gwopenValue = atcValueNv.wopenValueNv; 

    memcpy((void *)WPA_ptrToConfigTLV(), (WmmpCfgContext_t *)&NVRAM_EF_WMMP_TLV_CFG_DEFAULT, sizeof(WmmpCfgContext_t));
    memcpy((void *)WPA_ptrToUpdateTLV(),(WmmpUpdateContext_t *)&NVRAM_EF_WMMP_TLV_UPDATE_DEFAULT ,sizeof(WmmpUpdateContext_t));
    
    memcpy((void *)WPA_ptrToStatisticTLV(),(WmmpStatisticContext_t *)&NVRAM_EF_WMMP_TLV_STAT_DEFAULT ,sizeof(WmmpStatisticContext_t));
    memcpy((void *)WPA_ptrToStatusTLV(),(WmmpTermStatusContext_t *)&NVRAM_EF_WMMP_TLV_STATUS_DEFAULT,sizeof(WmmpTermStatusContext_t));
    memcpy((void *)WPA_ptrToControlTLV(),(WmmpControlContext_t *)(&NVRAM_EF_WMMP_TLV_CTRL_DEFAULT),sizeof(WmmpControlContext_t));
    memcpy((void *)WPA_ptrToSecurityTLV(),(WmmpSecurityContext_t *)(&NVRAM_EF_WMMP_TLV_SECURITY_DEFAULT),sizeof(WmmpSecurityContext_t));
    memcpy((void *)WPA_ptrToCustomTLV(),(WmmpCustomContext_t *)(&NVRAM_EF_WMMP_TLV_CUSTOM_DEFAULT),sizeof(WmmpCustomContext_t));
    memcpy((void *)WPA_ptrToCustom2TLV(),(WmmpCustom2Context_t *)(&NVRAM_EF_WMMP_TLV_CUSTOM2_DEFAULT),sizeof(WmmpCustom2Context_t));

#ifdef __SIMCOM_WMMP_ADC_GPIO__
    memcpy((void *)WPA_ptrToCustom3TLV(),(WmmpCustom3Context_t *)(&NVRAM_EF_WMMP_TLV_CUSTOM3_DEFAULT),sizeof(WmmpCustom3Context_t));
#endif
    networkContext_p = &(appWmmpContext_p->networkContext[0]);
    memcpy((void*)networkContext_p->p_account,(void*)(&NVRAM_EF_WMMP_ACCOUNT_DEFAULT[0]), sizeof(Wmmp_mmi_abm_account_info_struct));

    networkContext_p = &(appWmmpContext_p->networkContext[1]);
    memcpy((void*)networkContext_p->p_account,(void*)(&NVRAM_EF_WMMP_ACCOUNT_DEFAULT[1]), sizeof(Wmmp_mmi_abm_account_info_struct));
#ifdef __SIMCOM_WMMP_PRIVATE_APN__
    networkContext_p = &(appWmmpContext_p->networkContext[2]);
    memcpy((void*)networkContext_p->p_account,(void*)(&NVRAM_EF_WMMP_ACCOUNT_DEFAULT[2]), sizeof(Wmmp_mmi_abm_account_info_struct));
#endif
    eat_trace("wmmp_nvram_init() write nvram");

    //将默认值存入nvram
    wmmpProcessSave(NVRAM_EF_WMMP_TLV_CFG_LID,(WmmpCfgContext_t*)&NVRAM_EF_WMMP_TLV_CFG_DEFAULT, sizeof(WmmpCfgContext_t));  

    wmmpProcessSave(NVRAM_EF_WMMP_TLV_UPDATE_LID,(WmmpUpdateContext_t*)&NVRAM_EF_WMMP_TLV_UPDATE_DEFAULT, sizeof(WmmpUpdateContext_t));  
    wmmpProcessSave(NVRAM_EF_WMMP_TLV_STAT_LID,(WmmpStatisticContext_t*)&NVRAM_EF_WMMP_TLV_STAT_DEFAULT, sizeof(WmmpStatisticContext_t));  
    wmmpProcessSave(NVRAM_EF_WMMP_TLV_STATUS_LID,(WmmpTermStatusContext_t*)&NVRAM_EF_WMMP_TLV_STATUS_DEFAULT, sizeof(WmmpTermStatusContext_t));  
    wmmpProcessSave(NVRAM_EF_WMMP_TLV_CTRL_LID,(WmmpControlContext_t*)&NVRAM_EF_WMMP_TLV_CTRL_DEFAULT, sizeof(WmmpControlContext_t));  
    wmmpProcessSave(NVRAM_EF_WMMP_TLV_SECURITY_LID,(WmmpSecurityContext_t*)&NVRAM_EF_WMMP_TLV_SECURITY_DEFAULT, sizeof(WmmpSecurityContext_t));  
    wmmpProcessSave(NVRAM_EF_WMMP_TLV_CUSTOM_LID,(WmmpCustomContext_t*)&NVRAM_EF_WMMP_TLV_CUSTOM_DEFAULT, sizeof(WmmpCustomContext_t));  
    wmmpProcessSave(NVRAM_EF_WMMP_TLV_CUSTOM2_LID,(WmmpCustom2Context_t*)&NVRAM_EF_WMMP_TLV_CUSTOM2_DEFAULT, sizeof(WmmpCustom2Context_t));  
#ifdef __SIMCOM_WMMP_ADC_GPIO__
    wmmpProcessSave(NVRAM_EF_WMMP_TLV_CUSTOM3_LID,(WmmpCustom2Context_t*)&NVRAM_EF_WMMP_TLV_CUSTOM3_DEFAULT, sizeof(WmmpCustom2Context_t));  
#endif

#ifdef __SIMCOM_WMMP_PRIVATE_APN__
    wmmpProcessSave(NVRAM_EF_WMMP_ACCOUNT_LID, (Wmmp_mmi_abm_account_info_struct*)&NVRAM_EF_WMMP_ACCOUNT_DEFAULT[2], sizeof(Wmmp_mmi_abm_account_info_struct));  
#endif
    wmmpProcessSave(NVRAM_EF_WMMP_WOPEN_LID,(Wmmp_atc_value_struct*)&NVRAM_EF_WMMP_WOPEN_DEFAULT, sizeof(Wmmp_atc_value_struct));  
    eat_trace("wmmp_nvram_init()out");

}

/*****************************************************************************
*	函数名	      : wmmp_nvram_read_req
* 作  者        : 
*	功能描述      : 请求读NVRAM
*	输入参数说明  : 无
* 输出参数说明  : 无
*	返回值说明    : 无
*****************************************************************************/
void wmmp_nvram_read_req(u16 para, u16 rec_amount)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    eat_bool ret_read = EAT_FALSE;
    int i = 0;
    AppWmmpContext_t *appWmmpContext_p = ptrToAppWmmpContext();
    WmmpNetWorkContext_t        *networkContext_p;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ret_read = WPI_ReadNvram(NVRAM_EF_WMMP_WOPEN_LID,(void *)&atcValueNv,sizeof(Wmmp_atc_value_struct));
    if(EAT_FALSE == ret_read)
    {
         WPI_WriteNvram(NVRAM_EF_WMMP_WOPEN_LID,(void *)&NVRAM_EF_WMMP_WOPEN_DEFAULT,sizeof(Wmmp_atc_value_struct));
         memcpy((void *)&atcValueNv,(void *)&NVRAM_EF_WMMP_WOPEN_DEFAULT,sizeof(Wmmp_atc_value_struct));
    }
    
    gwopenValue = atcValueNv.wopenValueNv; 
    
    ret_read = WPI_ReadNvram(NVRAM_EF_WMMP_TLV_CFG_LID,(void *)WPA_ptrToConfigTLV(),sizeof(WmmpCfgContext_t));
    if(EAT_FALSE == ret_read)
    {
        WPI_WriteNvram(NVRAM_EF_WMMP_TLV_CFG_LID,(void *)&NVRAM_EF_WMMP_TLV_CFG_DEFAULT,sizeof(WmmpCfgContext_t));
        memcpy((void *)WPA_ptrToConfigTLV(), (WmmpCfgContext_t *)&NVRAM_EF_WMMP_TLV_CFG_DEFAULT, sizeof(WmmpCfgContext_t));
    }

    ret_read = WPI_ReadNvram(NVRAM_EF_WMMP_TLV_UPDATE_LID,(void *)WPA_ptrToUpdateTLV(),sizeof(WmmpUpdateContext_t));
    if(EAT_FALSE == ret_read)
    {
         WPI_WriteNvram(NVRAM_EF_WMMP_TLV_UPDATE_LID,(void *)&NVRAM_EF_WMMP_TLV_UPDATE_DEFAULT,sizeof(WmmpUpdateContext_t));
        memcpy((void *)WPA_ptrToUpdateTLV(), (WmmpUpdateContext_t *)&NVRAM_EF_WMMP_TLV_UPDATE_DEFAULT, sizeof(WmmpUpdateContext_t));
    }

    ret_read = WPI_ReadNvram(NVRAM_EF_WMMP_TLV_STAT_LID,(void *)WPA_ptrToStatisticTLV(),sizeof(WmmpStatisticContext_t));
    if(EAT_FALSE == ret_read)
    {
        WPI_WriteNvram(NVRAM_EF_WMMP_TLV_STAT_LID,(void *)&NVRAM_EF_WMMP_TLV_STAT_DEFAULT,sizeof(WmmpStatisticContext_t));
        memcpy((void *)WPA_ptrToStatisticTLV(), (WmmpStatisticContext_t *)&NVRAM_EF_WMMP_TLV_STAT_DEFAULT, sizeof(WmmpStatisticContext_t));
    }

    ret_read = WPI_ReadNvram(NVRAM_EF_WMMP_TLV_STATUS_LID,(void *)WPA_ptrToStatusTLV(),sizeof(WmmpTermStatusContext_t));
    if(EAT_FALSE == ret_read)
    {
         WPI_WriteNvram(NVRAM_EF_WMMP_TLV_STATUS_LID,(void *)&NVRAM_EF_WMMP_TLV_STATUS_DEFAULT,sizeof(WmmpTermStatusContext_t));
        memcpy((void *)WPA_ptrToStatusTLV(), (WmmpTermStatusContext_t *)&NVRAM_EF_WMMP_TLV_STATUS_DEFAULT, sizeof(WmmpTermStatusContext_t));
    }

    ret_read = WPI_ReadNvram(NVRAM_EF_WMMP_TLV_CTRL_LID,(void *)WPA_ptrToControlTLV(),sizeof(WmmpControlContext_t));
    if(EAT_FALSE == ret_read)
    {
        WPI_WriteNvram(NVRAM_EF_WMMP_TLV_CTRL_LID,(void *)&NVRAM_EF_WMMP_TLV_CTRL_DEFAULT,sizeof(WmmpControlContext_t));
        memcpy((void *)WPA_ptrToControlTLV(), (WmmpControlContext_t *)&NVRAM_EF_WMMP_TLV_CTRL_DEFAULT, sizeof(WmmpControlContext_t));
    }

    ret_read = WPI_ReadNvram(NVRAM_EF_WMMP_TLV_SECURITY_LID,(void *)WPA_ptrToSecurityTLV(),sizeof(WmmpSecurityContext_t));
    if(EAT_FALSE == ret_read)
    {
        WPI_WriteNvram(NVRAM_EF_WMMP_TLV_SECURITY_LID,(void *)&NVRAM_EF_WMMP_TLV_SECURITY_DEFAULT,sizeof(WmmpSecurityContext_t));
        memcpy((void *)WPA_ptrToSecurityTLV(), (WmmpSecurityContext_t *)&NVRAM_EF_WMMP_TLV_SECURITY_DEFAULT, sizeof(WmmpSecurityContext_t));
    }

    ret_read = WPI_ReadNvram(NVRAM_EF_WMMP_TLV_CUSTOM_LID,(void *)WPA_ptrToCustomTLV(),sizeof(WmmpCustomContext_t));
    if(EAT_FALSE == ret_read)
    {
        WPI_WriteNvram(NVRAM_EF_WMMP_TLV_CUSTOM_LID,(void *)&NVRAM_EF_WMMP_TLV_CUSTOM_DEFAULT,sizeof(WmmpCustomContext_t));
        memcpy((void *)WPA_ptrToCustomTLV(), (WmmpCustomContext_t *)&NVRAM_EF_WMMP_TLV_CUSTOM_DEFAULT, sizeof(WmmpCustomContext_t));
    }

    ret_read = WPI_ReadNvram(NVRAM_EF_WMMP_TLV_CUSTOM2_LID,(void *)WPA_ptrToCustom2TLV(),sizeof(WmmpCustom2Context_t));
    if(EAT_FALSE == ret_read)
    {
        WPI_WriteNvram(NVRAM_EF_WMMP_TLV_CUSTOM2_LID,(void *)&NVRAM_EF_WMMP_TLV_CUSTOM2_DEFAULT,sizeof(WmmpCustom2Context_t));
        memcpy((void *)WPA_ptrToCustom2TLV(), (WmmpCustom2Context_t *)&NVRAM_EF_WMMP_TLV_CUSTOM2_DEFAULT, sizeof(WmmpCustom2Context_t));
    }

#ifdef __SIMCOM_WMMP_ADC_GPIO__
    ret_read = WPI_ReadNvram(NVRAM_EF_WMMP_TLV_CUSTOM3_LID,(void *)WPA_ptrToCustom3TLV(),sizeof(WmmpCustom3Context_t));
    if(EAT_FALSE == ret_read)
    {
        WPI_WriteNvram(NVRAM_EF_WMMP_TLV_CUSTOM3_LID,(void *)&NVRAM_EF_WMMP_TLV_CUSTOM3_DEFAULT,sizeof(WmmpCustom3Context_t));
        memcpy((void *)WPA_ptrToCustom3TLV(), (WmmpCustom3Context_t *)&NVRAM_EF_WMMP_TLV_CUSTOM3_DEFAULT, sizeof(WmmpCustom3Context_t));
    }

#endif

    networkContext_p = &(appWmmpContext_p->networkContext[0]);
    memcpy((void*)networkContext_p->p_account,(void*)(&NVRAM_EF_WMMP_ACCOUNT_DEFAULT[0]), sizeof(Wmmp_mmi_abm_account_info_struct));

    networkContext_p = &(appWmmpContext_p->networkContext[1]);
    memcpy((void*)networkContext_p->p_account,(void*)(&NVRAM_EF_WMMP_ACCOUNT_DEFAULT[1]), sizeof(Wmmp_mmi_abm_account_info_struct));

#ifdef __SIMCOM_WMMP_PRIVATE_APN__
    {
        //Wmmp_mmi_abm_account_info_struct *context_p  = NULL; 
        networkContext_p = &(appWmmpContext_p->networkContext[2]); 
        ret_read = WPI_ReadNvram(NVRAM_EF_WMMP_ACCOUNT_LID,(void *)networkContext_p->p_account,sizeof(Wmmp_mmi_abm_account_info_struct));
        if(EAT_FALSE == ret_read)
        {
            memcpy((void*)networkContext_p->p_account,(Wmmp_mmi_abm_account_info_struct *)&NVRAM_EF_WMMP_ACCOUNT_DEFAULT[2] ,sizeof(Wmmp_mmi_abm_account_info_struct));
            WPI_WriteNvram(NVRAM_EF_WMMP_ACCOUNT_LID,(void *)networkContext_p->p_account,sizeof(Wmmp_mmi_abm_account_info_struct));
        }
        else
        {
            //networkContext_p = &(appWmmpContext_p->networkContext[2]);
            //memcpy((void*)networkContext_p->p_account,context_p,sizeof(Wmmp_mmi_abm_account_info_struct));
            eat_trace("account apn[%d]= %s",networkContext_p->p_account->ps_account_info.apn_length,networkContext_p->p_account->ps_account_info.apn);
        }
    }
#endif
    

}

/*SIMCOM Laiwenjie 2012-12-19 Fix MKBug00014691 for change nvram interface END*/


/*****************************************************************************
*函数名	      : wmmpProcessSave
* 作  者        : 
* 功能描述      : 保存wmmp相关参数
* 输入参数说明  : 
   u16 ef_id---nvram id
   void *data_ptr ----write data
   u16 length ----write data length
* 输出参数说明  : 无
* 返回值说明    : 无
*****************************************************************************/
void wmmpProcessSave(u16 ef_id, void *data_ptr, u16 length)
 {
     /*----------------------------------------------------------------*/
     /* Local Variables                                                */
     /*----------------------------------------------------------------*/
    eat_bool ret_val = FALSE;
     
    u8 uSrcId = simcom_at_parser_get_src_id();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    WMMP_DBG_TEMP("wmmpProcessSave ef_id =%d length %d",ef_id,length);

    WPI_WriteNvram(ef_id, data_ptr, length);
    return;
}


