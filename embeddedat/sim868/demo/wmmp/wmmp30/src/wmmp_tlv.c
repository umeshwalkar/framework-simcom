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
/*****************************************************************
* Include Files
*****************************************************************/
#include <stdio.h>
#include <string.h>

#include <stdarg.h>

//#include "wmmp_util.h"
#include "wmmp_typ.h"
#include "wmmp_tlv.h"
#include "wmmp_lib.h"
#include "wmmp_interface.h"

#include "simcom_wmmp_utility.h"
//#include "nvram_user_defs.h"

/*****************************************************************
* Manifest Constants
*****************************************************************/
static const u16 LENGTH_OF_INT8 = 1;
static const u16 LENGTH_OF_INT16 =2;
static const u16 LENGTH_OF_INT32 = 4;

const  char  SIZE_OF_INT8	=   sizeof (u8);
const  char  SIZE_OF_INT16   =   sizeof (u16);
const  char  SIZE_OF_INT32   =   sizeof (u32);

/*wangzhengning_simcom 2008-11-27, begin*/
//WmmpCfgContext_t			wmmpSavedCfgContext;
//WmmpUpdateContext_t			wmmpSavedUpdateContext;
//WmmpStatisticContext_t		wmmpSavedStatisticContext;
//WmmpTermStatusContext_t		wmmpSavedStatusContext;
//WmmpControlContext_t		wmmpSavedControlContext;
//WmmpSecurityContext_t		wmmpSavedSecurityContext;
/*wangzhengning_simcom 2008-11-27, end*/

/*****************************************************************
* Types
*****************************************************************/

/*****************************************************************
* Variables (Extern 、Global and Static)
*****************************************************************/
//extern char SecurityContextIMEI_p[17];
//extern WmmpMainContext_t  WmmpMainContext;   
/*****************************************************************
* Macros
*****************************************************************/
#define WMMP_LIB_DBG_TEMP(...)
/*****************************************************************
* External Functions Prototypes
*****************************************************************/


/*****************************************************************
* Local Functions Prototypes
*****************************************************************/

/*****************************************************************
* Local functions
*****************************************************************/

/******************************************************************************
* Function:   get_8    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
/*
u8  get_8(char   * cmd)
{
	u8   ret;
	int  i;
	for  (ret  =   0 , i  =  SIZE_OF_INT8  -  1 ; i  >=   0 ;  -- i)
	{
		ret   =  ret  <<   8 ;
		ret  |=  cmd[i];       
	}    
	return  ret;
}
*/
/******************************************************************************
* Function:   get_16    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
/*
u16  get_16(char   * cmd)
{
	u16   ret;
	int  i;
	for  (ret  =   0 , i  =  SIZE_OF_INT16  -  1 ; i  >=   0 ;  -- i)
	{
		ret   =  ret  <<   8 ;
		ret  |=  cmd[i];       
	}    
	return  ret;
}
*/
/******************************************************************************
* Function:   get_32    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
/*
u32  get_32(char   * cmd)
{
	u32   ret;
	int  i;
	for  (ret  =   0 , i  =  SIZE_OF_INT32  -  1 ; i  >=   0 ;  -- i)
	{
		ret   =  ret  <<   8 ;
		ret  |=  cmd[i];       
	}    
	return  ret;
}
*/
/******************************************************************************
* Function:   ReadTLVcfg    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
static void  ReadTLVcfg(u16 Tag_Id,TLVRetVal_t* ret)
{
    u16	loopi = 0;
	
	WmmpCfgContext_t  *CfgContext_p = &(ptrToWmmpContext()->ConfigContext);
	WmmpCustomContext_t *CusContext_p = &(ptrToWmmpContext()->CustomContext);
	ret->addr = PNULL;
	ret->length = 0;
	ret->TLVType = TLV_STRING;
	
	WMMP_LIB_DBG_TEMP("ReadTLVcfg Tag_Id 0x%04x",Tag_Id);
	switch(Tag_Id)
	{
		case GPRSDAILNUM://0x0001:
			ret->addr = (char*)CfgContext_p->GPRSDailNum;
			ret->length = strlen((const char *)CfgContext_p->GPRSDailNum);
			ret->TLVType = TLV_STRING;
			//return ret;
			break;
		case M2MAPN://0x0002:
			ret->addr = (char*)CfgContext_p->M2MAPN;
			ret->length = strlen((const char *)CfgContext_p->M2MAPN);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case GPRSUSERID://0x0003:
			ret->addr = (char*)CfgContext_p->GPRSUserId;
			ret->length = strlen((const char *)CfgContext_p->GPRSUserId);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case GPRSPASSWORD://0x0004:
			ret->addr = (char*)CfgContext_p->GPRSPassword;
			ret->length = strlen((const char *)CfgContext_p->GPRSPassword);
			ret->TLVType = TLV_STRING;
			//return ret;				
			break;
		case SMSCENTRENUM://0x0005:
			ret->addr = (char*)CfgContext_p->SMSCentreNum;
			ret->length = strlen((const char *)CfgContext_p->SMSCentreNum);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case APPSRVTRANSKEY://0x0006:
			ret->addr = (char*)CfgContext_p->AppSrvTransKey;
			ret->length = strlen((const char *)CfgContext_p->AppSrvTransKey);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case M2MSMSPECIALNUM://0x0007:
			ret->addr = (char*)CfgContext_p->M2MSMSpecialNum;
			ret->length = strlen((const char *)CfgContext_p->M2MSMSpecialNum);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case M2MURLADDR://0x0008:
			ret->addr = (char*)CfgContext_p->M2MURLAddr;
			ret->length = strlen((const char *)CfgContext_p->M2MURLAddr);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case M2MIPADDR://0x0009:
			ret->addr = (char*)&CfgContext_p->M2MIPAddr;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
		case M2MPORT://0x000A:
			ret->addr = (char*)&CfgContext_p->M2MPort;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
		case APPSMSCENTRENUM://0x000B:
			ret->addr = (char*)CfgContext_p->AppSMSCentreNum;
			ret->length = strlen((const char *)CfgContext_p->AppSMSCentreNum);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case APPSRVURLADDR://0x000C:
			ret->addr = (char*)CfgContext_p->AppSrvURLAddr;
			ret->length = strlen((const char *)CfgContext_p->AppSrvURLAddr);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case APPSRVIPADDR://0x000D:
			ret->addr = (char*)&CfgContext_p->AppSrvIPAddr;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
		case APPSRVPORT://0x000E:
			ret->addr = (char*)&CfgContext_p->AppSrvPort;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
			
		case reserve1://0x000F:
			//reserve
			break;	

		case DNSADDR://0x0010:
			ret->addr = (char*)CfgContext_p->DNSAddr;
			ret->length = strlen((const char *)CfgContext_p->DNSAddr);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case PERIOD_CFG://0x0011:
			ret->addr = (char*)&CfgContext_p->Period;
			ret->length =LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
		case APPUSSDSMSSPECIALNUM://0x0012:
			ret->addr = (char*)CfgContext_p->AppUSSDSMSSpecialNum;
			ret->length = strlen((const char *)CfgContext_p->AppUSSDSMSSpecialNum);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case M2MAPPAPN://0x0013:
			ret->addr = (char*)CfgContext_p->M2MAppAPN;
			ret->length = strlen((const char *)CfgContext_p->M2MAppAPN);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case BACKUPDNSADDR://0x0014:
			ret->addr = (char*)CfgContext_p->BackupDNSAddr;
			ret->length = strlen((const char *)CfgContext_p->BackupDNSAddr);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;

		case TLVSYNCTAGVALUE://0x0025:
			WMMP_LIB_DBG_TEMP("ReadTLVcfg:TLVSyncTagNum %d",CusContext_p->TLVSyncTagNum);	
			ret->addr = (char*)CfgContext_p->TLVSyncTagValue;
			//ret->length = strlen(CfgContext_p->TLVSyncTagValue);
			ret->length = CusContext_p->TLVSyncTagNum*2;
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case SMSRECVOVERTIME://0x0026:
			ret->addr = (char*)&CfgContext_p->SMSRecvOvertime;
			ret->length = LENGTH_OF_INT16;
			ret->TLVType = TLV_INT16;
			 WMMP_LIB_DBG_TEMP("ReadTLVcfg:tag=0x%x,val=%d,%d,len=%d\n",SMSRECVOVERTIME,*(ret->addr),CfgContext_p->SMSRecvOvertime,ret->length);	
	
			//return ret;			
			break;
		case MAXLOGINRETRYTIMES://0x0027:
			ret->addr = (char*)&CfgContext_p->MaxLoginRetryTimes;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case DEFAULTECCODE://0x0028:
			ret->addr = (char*)CfgContext_p->DefaultECCode;
			ret->length = strlen((const char *)CfgContext_p->DefaultECCode);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;

		case LOCALPTRVALUE://0x0101:
			ret->addr = (char*)&CfgContext_p->LocalPTRValue;
			ret->length = LENGTH_OF_INT16;
			ret->TLVType = TLV_INT16;
			//return ret;			
			break;
		case LOCALPORTMODE://0x0102:
			ret->addr = (char*)&CfgContext_p->LocalPortMode;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case LOCALPORTHALT://0x0103:
			ret->addr = (char*)&CfgContext_p->LocalPortMode;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case LOCALPORTVERIFY://0x0104:
			ret->addr = (char*)&CfgContext_p->LocalPortVerify;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case LOCALPORTSCANPERIOD://0x0105:
			ret->addr = (char*)&CfgContext_p->LocalPortScanPeriod;
			ret->length = LENGTH_OF_INT16;
			ret->TLVType = TLV_INT16;
			//return ret;			
			break;
			
		case reserve2://0x0106:
			//reserve
			break;
			
		case DATAPACKRESPOVERTIME://0x0107:
			ret->addr = (char*)&CfgContext_p->DataPackRespOvertime;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
		case TRANSFAILRETRYTIMES://0x0108:
			ret->addr = (char*)&CfgContext_p->TransFailRetryTimes;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case GATELIMIT://0x0109:
			ret->addr = (char*)&CfgContext_p->GateLimit;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;				
		default:
			break;
	}

	WMMP_LIB_DBG_TEMP("ReadTLVcfg ret->TLVType %d",ret->TLVType);
	WMMP_LIB_DBG_TEMP("ReadTLVcfg ret->length %d",ret->length);
  #if 0
	for(loopi = 0;loopi<ret->length;loopi++)
	{
		WMMP_LIB_DBG_TEMP("ReadTLVcfg ret->addr[%d] 0x%x",loopi,ret->addr[loopi]);
	}
#endif
}

/******************************************************************************
* Function:   ReadTLVUpdate    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
static void ReadTLVUpdate(u16 Tag_Id,TLVRetVal_t* ret)
{
    u16	loopi = 0;
	WmmpUpdateContext_t  *UpdateContext_p = &(ptrToWmmpContext()->UpdateContext);
//	memcpy(UpdateContext_p,&wmmpSavedUpdateContext,sizeof(WmmpUpdateContext_t));
	ret->addr = PNULL;
	ret->length = 0;
	ret->TLVType = TLV_STRING;
	
	WMMP_LIB_DBG_TEMP("ReadTLVUpdate Tag_Id 0x%04x",Tag_Id);
	switch(Tag_Id)
	{
		case UPDATEMODE://0x1000:
			ret->addr = (char*)&UpdateContext_p->UpdateMode;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case UPDATESRVURLADDR://0x1001:
			ret->addr = (char*)UpdateContext_p->UpdateSrvURLAddr;
			ret->length = strlen((const char *)UpdateContext_p->UpdateSrvURLAddr);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case UPDATESRVIPADDR://0x1002:
			ret->addr = (char*)UpdateContext_p->UpdateSrvIPAddr;
			ret->length = strlen((const char *)UpdateContext_p->UpdateSrvIPAddr);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case UPDATESRVPORT://0x1003:
			ret->addr = (char*)&UpdateContext_p->UpdateSrvPort;
			ret->length = LENGTH_OF_INT16;
			ret->TLVType = TLV_INT16;
			//return ret;			
			break;
		case UPDATESRVLOGINPASSWORD://0x1004:
			ret->addr = (char*)UpdateContext_p->UpdateSrvLoginPassword;
			ret->length = strlen((const char *)UpdateContext_p->UpdateSrvLoginPassword);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case UPDATEFILEREV://0x1005:
			ret->addr = (char*)UpdateContext_p->UpdateFileRev;
			ret->length = strlen((const char *)UpdateContext_p->UpdateFileRev);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case UPDATEFILELENGTH://0x1006:
			ret->addr = (char*)&UpdateContext_p->UpdateFileLength;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
		case CRC16CHECK://0x1007:
			ret->addr = (char*)&UpdateContext_p->CRC16Check;
			ret->length = LENGTH_OF_INT16;
			ret->TLVType = TLV_INT16;
			//return ret;			
			break;
		case CRC32CHECK://0x1008:
			ret->addr = (char*)&UpdateContext_p->CRC32Check;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
		case UPDATEAFFAIRTRANSID://0x1009:
			ret->addr = (char*)&UpdateContext_p->UpdateAffairTransId;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
		case FILERELEASEDATA://0x100A:
			ret->addr = (char*)UpdateContext_p->FileReleaseData;
			ret->length = strlen((const char *)UpdateContext_p->FileReleaseData);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case FORCETOUPDATEPARAM://0x100B:
			ret->addr = (char*)&UpdateContext_p->ForcetoUpdateParam;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;	
		case CURRENTFILEREV://0x100C
			ret->addr = (char*)UpdateContext_p->CurrentFileRev;
			ret->length = strlen((const char *)UpdateContext_p->CurrentFileRev);
			ret->TLVType = TLV_STRING;			
			break;
		default:
			break;			
	}

	WMMP_LIB_DBG_TEMP("ReadTLVUpdate ret->TLVType %d",ret->TLVType);
	WMMP_LIB_DBG_TEMP("ReadTLVUpdate ret->length %d",ret->length);
  #if 0
	for(loopi = 0;loopi<ret->length;loopi++)
	{
		WMMP_LIB_DBG_TEMP("ReadTLVUpdate ret->addr[%d] 0x%x",loopi,ret->addr[loopi]);
	}
  #endif
}

/******************************************************************************
* Function:   ReadTLVStatistic    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
static void ReadTLVStatistic(u16 Tag_Id,TLVRetVal_t* ret)
{
    u16	loopi = 0;
	WmmpStatisticContext_t  *StatisticContext_p = &(ptrToWmmpContext()->StatisticContext);
//	memcpy(StatisticContext_p,&wmmpSavedStatisticContext,sizeof(WmmpStatisticContext_t));
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	ret->addr = PNULL;
	ret->length = 0;
	ret->TLVType = TLV_STRING;
	
	WMMP_LIB_DBG_TEMP("ReadTLVStatistic Tag_Id 0x%04x",Tag_Id);
	switch(Tag_Id)
	{
		case STATINFOREPORT://0x2001:
			ret->addr = (char*)&StatisticContext_p->StatInfoReport;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case REPORTTIME://0x2002:
			ret->addr = (char*)&StatisticContext_p->ReportTime;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;
			break;
		case PERIODREPORT://0x2003:
			ret->addr = (char*)&StatisticContext_p->PeriodReport;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;
			break;
		case MONTHLYOPERATIONSIGN://0x2004:
			ret->addr = (char*)&StatisticContext_p->MonthlyOperationSign;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case CLEARSTATDATA://0x2005:
			ret->addr = (char*)&StatisticContext_p->ClearStatData;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case STATSTARTTIME://0x2006:
			ret->addr = (char*)&StatisticContext_p->StatStartTime;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
		case STATFINISHTIME://0x2007:
			ret->addr = (char*)&StatisticContext_p->statFinishTime;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
		case SMSDELIVERYSUCC://0x2008:
			ret->addr = (char*)&StatisticContext_p->SMSDeliverySucc;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
		case SMSDELIVERUFAIL://0x2009:
			ret->addr = (char*)&StatisticContext_p->SMSDeliveruFail;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
		case SMSRECEIVEDNUM://0x200A:
			ret->addr = (char*)&StatisticContext_p->SMSReceivedNum;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
		case GRPSDATASIZE://0x200B:
			ret->addr = (char*)&StatisticContext_p->GRPSDataSize;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;	
		case USSDSUCCNUM://0x200C:
			ret->addr = (char*)&StatisticContext_p->USSDSuccNum;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;	
		case USSDFAILNUM://0x200D:
			ret->addr = (char*)&StatisticContext_p->USSDFailNum;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;	
		case USSDRECEIVEDNUM://0x200E:
			ret->addr = (char*)&StatisticContext_p->USSDReceivedNum;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;	
			
		case SMSSTATPARAM://0x2011:
			ret->addr = (char*)StatisticContext_p->SMSStatParam;
			ret->length = strlen((const char *)StatisticContext_p->SMSStatParam);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case SMSSTATAUTOREPORTPARAM://0x2012:
			ret->addr = (char*)StatisticContext_p->SMSStatAutoReportParam;
			ret->length = strlen((const char *)StatisticContext_p->SMSStatAutoReportParam);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case M2MMUTUALSMSSTAT://0x2013:
			ret->addr = (char*)StatisticContext_p->M2MMutualSMSStat;
			//ret->length = strlen(StatisticContext_p->M2MMutualSMSStat);
			ret->length = 8*mainContext_p->CustomContext.SMSM2MStatRecID;
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case OPERATIONMUTUALSMSSTAT://0x2014:
			ret->addr = (char*)StatisticContext_p->OperationMutualSMSStat;
			//ret->length = strlen(StatisticContext_p->OperationMutualSMSStat);
			ret->length = 0; //temp
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case GPRSOPERATIONSTATPARAM://0x2015:
			ret->addr = (char*)StatisticContext_p->GPRSOperationStatParam;
			ret->length = strlen((const char *)StatisticContext_p->GPRSOperationStatParam);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case GPRSOPERATIONSTATREPORTPARAM://0x2016:
			ret->addr = (char*)StatisticContext_p->GPRSOperationStatReportParam;
			ret->length = strlen((const char *)StatisticContext_p->GPRSOperationStatReportParam);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	 
		case M2MMUTUALGPRSDATASIZE://0x2017:
			ret->addr = (char*)StatisticContext_p->M2MMutualGPRSDataSize;
			//ret->length = strlen(StatisticContext_p->M2MMutualGPRSDataSize);
			ret->length = 12*mainContext_p->CustomContext.GPRSM2MStatRecID;
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case OPERATIONMUTUALGPRSDATASIZE://0x2018:
			ret->addr = (char*)StatisticContext_p->OperationMutualGPRSDataSize;
			//ret->length = strlen(StatisticContext_p->OperationMutualGPRSDataSize);
			ret->length = 0;  //temp
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case USSDSTATPARAM://0x2019:
			ret->addr = (char*)StatisticContext_p->USSDStatParam;
			ret->length = strlen((const char *)StatisticContext_p->USSDStatParam);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case USSDSTATREPORTPARAM://0x201A:
			ret->addr = (char*)StatisticContext_p->USSDStatReportParam;
			ret->length = strlen((const char *)StatisticContext_p->USSDStatReportParam);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case M2MMUTUALUSSDSTAT://0x201B:
			ret->addr = (char*)StatisticContext_p->M2MMutualUSSDStat;
			ret->length = strlen((const char *)StatisticContext_p->M2MMutualUSSDStat);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case OPERATIONMUTUALUSSDSTAT://0x201C:
			ret->addr = (char*)StatisticContext_p->OperationMutualUSSDStat;
			ret->length = strlen((const char *)StatisticContext_p->OperationMutualUSSDStat);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
			
		default:
			break;			
	}

	WMMP_LIB_DBG_TEMP("ReadTLVStatistic ret->TLVType %d",ret->TLVType);
	WMMP_LIB_DBG_TEMP("ReadTLVStatistic ret->length %d",ret->length);
  #if 0
	for(loopi = 0;loopi<ret->length;loopi++)
	{
		WMMP_LIB_DBG_TEMP("ReadTLVStatistic ret->addr[%d] 0x%x",loopi,ret->addr[loopi]);
	}
  #endif
}

/******************************************************************************
* Function:   ReadTLVStatus    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
static void ReadTLVStatus(u16 Tag_Id,TLVRetVal_t* ret)
{
    u16	loopi = 0;
	WmmpTermStatusContext_t  *StatusContext_p = &(ptrToWmmpContext()->StatusContext);
//	memcpy(StatusContext_p,&wmmpSavedStatusContext,sizeof(WmmpTermStatusContext_t));
	ret->addr = PNULL;
	ret->length = 0;
	ret->TLVType = TLV_STRING;
	
	WMMP_LIB_DBG_TEMP("ReadTLVStatus Tag_Id 0x%04x",Tag_Id);
	switch(Tag_Id)
	{
		case PROTOCOLREV://0x3001:
			ret->addr = (char*)StatusContext_p->ProtocolRev;
			ret->length = strlen((const char *)StatusContext_p->ProtocolRev);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case TERMINALSERIESNUM://0x3002:
			ret->addr = (char*)StatusContext_p->TerminalSeriesNum;
			WMMP_LIB_DBG_TEMP("ReadTLVStatus TERMINALSERIESNUM  %s ",StatusContext_p->TerminalSeriesNum);
			ret->length = strlen((const char *)StatusContext_p->TerminalSeriesNum);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case IMSINUM://0x3003:
			ret->addr = (char*)StatusContext_p->IMSINum;
			ret->length = strlen((const char *)StatusContext_p->IMSINum);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case FIRMWAREREV://0x3004:
			ret->addr = (char*)StatusContext_p->FirmwareRev;
			ret->length = strlen((const char *)StatusContext_p->FirmwareRev);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case PREVPERIOD://0x3005:
			ret->addr = (char*)&StatusContext_p->PrevPeriod;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
		case CELLULARID://0x3006:
			ret->addr = (char*)&StatusContext_p->CellularId;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
		case SIGNALLEVEL://0x3007:
			ret->addr = (char*)&StatusContext_p->SignalLevel;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case INITPARAFLAG://0x3008:
			ret->addr = (char*)&StatusContext_p->InitParaFlag;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case SYSTEMTIME://0x3009:
			ret->addr = (char*)&StatusContext_p->SystemTime;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
			
		case reserve3://0x300A:
			//reserve		
			break;
			
		case ALARMSTATUS://0x300B:
			ret->addr = (char*)&StatusContext_p->AlarmStatus;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			WMMP_LIB_DBG_TEMP("ReadTLVStatus 0x300B AlarmStatus =%x",StatusContext_p->AlarmStatus);
			break;	
		case ALARMTYPE://0x300C:
			ret->addr = (char*)&StatusContext_p->AlarmType;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			WMMP_LIB_DBG_TEMP("ReadTLVStatus  0x300c AlarmType=%x",StatusContext_p->AlarmType);
			break;	
		case ALARMCODE://0x300D:
			ret->addr = (char*)&StatusContext_p->AlarmCode;
			ret->length = LENGTH_OF_INT16;
			ret->TLVType = TLV_INT16;
			//return ret;			
			WMMP_LIB_DBG_TEMP("ReadTLVStatus 0x300d AlarmCode =%x",StatusContext_p->AlarmCode);
			break;	
		case RESTOREALARM://0x300E:
			ret->addr = (char*)StatusContext_p->RestoreAlarm;
			ret->length = strlen((const char *)StatusContext_p->RestoreAlarm);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case M2MDEVSTATUS: /*0x300f*/
			ret->addr = (char*)StatusContext_p->DEVStatus;			
			ret->length = strlen((const char *)StatusContext_p->DEVStatus);
			//ret->length = TLV_M2MDEVSTATUS_LEN*2;			
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case SUPPORTEDDEV://0x3010:
			ret->addr = (char*)StatusContext_p->SupportedDev;
			ret->length = strlen((const char *)StatusContext_p->SupportedDev);
			//ret->length = TLV_SUPPORTEDDEV_LEN;    // 2;
			//ret->length = StatusContext_p->SupportedDev_Len;
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case PACKLOSTNUM://0x3011:
			ret->addr = (char*)&StatusContext_p->PackLostNum;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;	
		case FAILWARNING://0x3012:
			ret->addr = (char*)&StatusContext_p->FailWarning;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
			
		default:
			break;			
	}

	WMMP_LIB_DBG_TEMP("ReadTLVStatus ret->TLVType %d",ret->TLVType);
	WMMP_LIB_DBG_TEMP("ReadTLVStatus ret->length %d",ret->length);
  #if 0
	for(loopi = 0;loopi<ret->length;loopi++)
	{
		WMMP_LIB_DBG_TEMP("ReadTLVStatus ret->addr[%d] 0x%x",loopi,ret->addr[loopi]);
	}
  #endif
}

/******************************************************************************
* Function:   ReadTLVControl    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
static void ReadTLVControl(u16 Tag_Id,TLVRetVal_t* ret)
{
    u16	loopi = 0;
	WmmpControlContext_t  *ControlContext_p = &(ptrToWmmpContext()->ControlContext);
//	memcpy(ControlContext_p,&wmmpSavedControlContext,sizeof(WmmpControlContext_t));
	ret->addr = PNULL;
	ret->length = 0;
	ret->TLVType = TLV_STRING;
	
	WMMP_LIB_DBG_TEMP("ReadTLVControl Tag_Id 0x%04x",Tag_Id);
	switch(Tag_Id)
	{
		case SETCOMMAND://0x4001:
			ret->addr = (char*)&ControlContext_p->SetCommand;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case CONNECTMODE://0x4002:
			ret->addr = (char*)&ControlContext_p->ConnectMode;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case DATATRANSMODE://0x4003:
			ret->addr = (char*)&ControlContext_p->DataTransMode;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case PERIOD_CTRL://0x4004:
			ret->addr = (char*)&ControlContext_p->Period;
			ret->length = LENGTH_OF_INT16;
			ret->TLVType = TLV_INT16;
			//return ret;			
			break;
		case COLLECTDATATRANSMODE://0x4005:
			ret->addr = (char*)&ControlContext_p->CollectDataTransMode;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case M2MCONNECTMODEONIP://0x4006:
			ret->addr = (char*)ControlContext_p->M2MConnectModeOnIP;
			ret->length = strlen((const char *)ControlContext_p->M2MConnectModeOnIP);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case USERDATAID://0x4007:
			ret->addr = (char*)ControlContext_p->UserDataID;
			ret->length = strlen((const char *)ControlContext_p->UserDataID);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case SUBPACKMODEPARAM://0x4008:
			ret->addr = (char*)ControlContext_p->SubPackModeParam;
			ret->length = strlen((const char *)ControlContext_p->SubPackModeParam);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case PLATFORMPARAMOPT://0x4009:
			ret->addr = (char*)&ControlContext_p->PlatformParamOpt;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
			
		case EXECUTEPARAMOPT://0x4010:
			ret->addr = (char*)ControlContext_p->ExecuteParamOpt;
			ret->length = 5;
			ret->TLVType = TLV_STRING;
			//return ret;		
			break;

		case lengthblank://0x4011:

			break;
			
		case DESSERIESNUM://0x4012:
			ret->addr = (char*)ControlContext_p->DesSeriesNum;
			ret->length = strlen((const char *)ControlContext_p->DesSeriesNum);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case SRCSERIESNUM://0x4013:
			ret->addr = (char*)ControlContext_p->SrcSeriesNum;
			ret->length = strlen((const char *)ControlContext_p->SrcSeriesNum);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case DESECCODE://0x4014:
			ret->addr = (char*)&ControlContext_p->DesECCode;
			ret->length = LENGTH_OF_INT16;
			ret->TLVType = TLV_INT16;
			//return ret;			
			break;	
		case SRCECCODE://0x4015:
			ret->addr = (char*)ControlContext_p->SrcECCode;
			ret->length = strlen((const char *)ControlContext_p->SrcECCode);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case TRANSCTRLCOMMANDSTATICPARAM://0x4016:
			ret->addr = (char*)ControlContext_p->TransCtrlCommandStaticParam;
			ret->length = strlen((const char *)ControlContext_p->TransCtrlCommandStaticParam);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case VERIFYTLVBYCRC32://0x4017:
			ret->addr = (char*)ControlContext_p->VerifyTLVByCRC32;
			ret->length = strlen((const char *)ControlContext_p->VerifyTLVByCRC32);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case EXTERNALDEVCTRL://0x4021:
			ret->addr = (char*)ControlContext_p->ExternalDevCtrl;
			ret->length = strlen((const char *)ControlContext_p->ExternalDevCtrl);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
			
		default:
			break;			
	}
	
	WMMP_LIB_DBG_TEMP("ReadTLVControl ret->TLVType %d",ret->TLVType);
	WMMP_LIB_DBG_TEMP("ReadTLVControl ret->length %d",ret->length);
  #if 0
	for(loopi = 0;loopi<ret->length;loopi++)
	{
		WMMP_LIB_DBG_TEMP("ReadTLVControl ret->addr[%d] 0x%x",loopi,ret->addr[loopi]);
	}
  #endif
}

/******************************************************************************
* Function:   ReadTLVSecurity    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
static void ReadTLVSecurity(u16 Tag_Id,TLVRetVal_t* ret)
{
    u16	loopi = 0;
	WmmpSecurityContext_t  *SecurityContext_p = &(ptrToWmmpContext()->SecurityContext);

//	memcpy(SecurityContext_p,&wmmpSavedSecurityContext,sizeof(WmmpSecurityContext_t));
	ret->addr = PNULL;
	ret->length = 0;
	ret->TLVType = TLV_STRING;
	
	WMMP_LIB_DBG_TEMP("ReadTLVSecurity Tag_Id 0x%04x",Tag_Id);
	switch(Tag_Id)
	{
		case SIMPIN1://0xE001:
			ret->addr = (char*)SecurityContext_p->SIMPin1;
			ret->length = strlen((const char *)SecurityContext_p->SIMPin1);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case SIMPIN1MD5://0xE002:
			ret->addr = (char*)SecurityContext_p->SIMPin1MD5;
			ret->length = 16;
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case SIMPIN1ACTIVE://0xE003:
			ret->addr = (char*)&SecurityContext_p->SIMPin1Active;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case SIMPIN2://0xE004:
			ret->addr = (char*)SecurityContext_p->SIMPin2;
			ret->length = strlen((const char *)SecurityContext_p->SIMPin2);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case SIMPIN2MD5://0xE005:
			ret->addr = (char*)SecurityContext_p->SIMPin2MD5;
			ret->length = strlen((const char *)SecurityContext_p->SIMPin2MD5);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case SIMPIN2ACTIVE://0xE006:
			ret->addr = (char*)&SecurityContext_p->SIMPin2Active;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case SIMPUK1://0xE007:
			ret->addr = (char*)SecurityContext_p->SIMPuk1;
			ret->length = strlen((const char *)SecurityContext_p->SIMPuk1);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case SIMPUK1MD5://0xE008:
			ret->addr = (char*)SecurityContext_p->SIMPuk1MD5;
			ret->length = strlen((const char *)SecurityContext_p->SIMPuk1MD5);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case SIMPUK1ACTIVE://0xE009:
			ret->addr = (char*)&SecurityContext_p->SIMPuk1Active;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case SIMPUK2://0xE00A:
			ret->addr = (char*)SecurityContext_p->SIMPuk2;
			ret->length = strlen((const char *)SecurityContext_p->SIMPuk2);
			ret->TLVType = TLV_STRING;
			//return ret;	
			break;
		case SIMPUK2MD5://0xE00B:
			ret->addr = (char*)SecurityContext_p->SIMPuk2MD5;
			ret->length = strlen((const char *)SecurityContext_p->SIMPuk2MD5);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case SIMPUK2ACTIVE://0xE00C:
			ret->addr = (char*)&SecurityContext_p->SIMPuk2Active;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;	
		case BINDPARAACTIVE://0xE00D:
			ret->addr = (char*)&SecurityContext_p->BindParaActive;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;	
		case BINDIMEI://0xE00E:
			//memcpy(SecurityContext_p->BindIMEI, SecurityContextIMEI_p, sizeof(SecurityContextIMEI_p));
			WPI_GetIMEI(SecurityContext_p->BindIMEI,17);			
			
			ret->addr = (char*)SecurityContext_p->BindIMEI;
			//ret->length = strlen((const char *)SecurityContext_p->BindIMEI);
			ret->length = 16;
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case BINDIMEIMD5://0xE00F:
			ret->addr = (char*)SecurityContext_p->BindIMEIMD5;
			ret->length = strlen((const char *)SecurityContext_p->BindIMEIMD5);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;				
		case BINDIMEIORIG://0xE010:
			ret->addr = (char*)SecurityContext_p->BindIMEIOrig;
			ret->length = strlen((const char *)SecurityContext_p->BindIMEIOrig);
			ret->TLVType = TLV_STRING;
			//return ret;		
			break;
		case BINDIMEIORIGMD5://0xE011:
			ret->addr = (char*)SecurityContext_p->BindIMEIOrigMD5;
			ret->length = strlen((const char *)SecurityContext_p->BindIMEIOrigMD5);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case BINDIMSI://0xE012:

			WPI_GetIMSI(SecurityContext_p->BindIMSI,17);
			WMMP_LIB_DBG_TEMP("BINDIMSI  BindIMSI  %s   ",SecurityContext_p->BindIMSI);
				//memcpy(SecurityContext_p->BindIMSI,WPI_GetIMSI,sizeof(SecurityContextIMSI_p));
				//WMMP_LIB_DBG_TEMP("SecurityContext_p->BindIMSI:%s",SecurityContext_p->BindIMSI);
			ret->addr = (char*)SecurityContext_p->BindIMSI;
			//ret->length = strlen((const char *)SecurityContext_p->BindIMSI);
			ret->length = 15;
			ret->TLVType = TLV_STRING;
			//return ret;	

			break;	
		case BINDIMSIMD5://0xE013:
			ret->addr = (char*)SecurityContext_p->BindIMSIMD5;
			ret->length = strlen((const char *)SecurityContext_p->BindIMSIMD5);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case BINDIMSIORIG://0xE014:
		{
			ret->addr = (char*)SecurityContext_p->BindIMSIOrig;
			ret->length = 15;
			ret->TLVType = TLV_STRING;
			//return ret;			
			WMMP_LIB_DBG_TEMP("E014 IMSIOrig %s",ret->addr);
			break;	
		}
		case BINDIMSIORIGMD5://0xE015:
			ret->addr = (char*)SecurityContext_p->BindIMSIOrigMD5;
			ret->length = strlen((const char *)SecurityContext_p->BindIMSIOrigMD5);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case BINDCURRENTMD5://0xE016:
			ret->addr = (char*)SecurityContext_p->BindCurrentMD5;
			ret->length = strlen((const char *)SecurityContext_p->BindCurrentMD5);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case BINDORIGINALMD5://0xE017:
			ret->addr = (char*)SecurityContext_p->BindOriginalMD5;
			ret->length = strlen((const char *)SecurityContext_p->BindOriginalMD5);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case PIN1ERRORALLOW://0xE018:
			ret->addr = (char*)&SecurityContext_p->Pin1ErrorAllow;
			ret->length = LENGTH_OF_INT16;
			ret->TLVType = TLV_INT16;
			//return ret;			
			break;	
		case PIN2ERRORALLOW://0xE019:
			ret->addr = (char*)&SecurityContext_p->Pin2ErrorAllow;
			ret->length = LENGTH_OF_INT16;
			ret->TLVType = TLV_INT16;
			//return ret;			
			break;		
		case PUK1ERRORALLOW://0xE01A:
			ret->addr = (char*)&SecurityContext_p->Puk1ErrorAllow;
			ret->length = LENGTH_OF_INT16;
			ret->TLVType = TLV_INT16;
			//return ret;	
			break;
		case PUK2ERRORALLOW://0xE01B:
			ret->addr = (char*)&SecurityContext_p->Puk2ErrorAllow;
			ret->length = LENGTH_OF_INT16;
			ret->TLVType = TLV_INT16;
			//return ret;			
			break;	
		case M2MPASSCLEARPARA://0xE020:
			ret->addr = (char*)&SecurityContext_p->M2MPassClearPara;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;	
		case INITPARA://0xE021:
			ret->addr = (char*)SecurityContext_p->InitPara;
			ret->length = strlen((const char *)SecurityContext_p->InitPara);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case INITPARAUSED://0xE022:
			ret->addr = (char*)&SecurityContext_p->InitParaUsed;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;	
		case INITCOMMAND://0xE023:
			ret->addr = (char*)SecurityContext_p->InitCommand;
			ret->length = strlen((const char *)SecurityContext_p->InitCommand);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;				
		case INITTLVACTTAG://0xE024:
			ret->addr = (char*)SecurityContext_p->InitTLVactTag;
			ret->length = strlen((const char *)SecurityContext_p->InitTLVactTag);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case UPLINKPASSWORD://0xE025:
			ret->addr = (char*)SecurityContext_p->uplinkPassword;
			ret->length = strlen((const char *)SecurityContext_p->uplinkPassword);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case UPLINKPASSWORDMD5://0xE026:
			ret->addr = (char*)SecurityContext_p->uplinkPasswordMD5;
			//ret->length = strlen((const char *)SecurityContext_p->uplinkPasswordMD5);
			ret->length = 16;
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case UPLINKPASSWORDEXPDATE://0xE027:
			ret->addr = (char*)&SecurityContext_p->uplinkPasswordExpDate;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
		case DOWNLINKPASSWORD://0xE028:
			ret->addr = (char*)SecurityContext_p->downlinkPassword;
			ret->length = strlen((const char *)SecurityContext_p->downlinkPassword);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case DOWNLINKPASSWORDMD5://0xE029:
			ret->addr = (char*)SecurityContext_p->downlinkPasswordMD5;
			ret->length = 16;
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case DOWNLINKPASSWORDEXPDATE://0xE02A:
			ret->addr = (char*)&SecurityContext_p->downlinkPasswordExpDate;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;
			
		case ENCRYPTIONSUPPORT://0xE031:
			ret->addr = (char*)&SecurityContext_p->encryptionSupport;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case ENCRYPTIONUSED://0xE032:
			ret->addr = (char*)&SecurityContext_p->encryptionUsed;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case ENCRYPTIONCOMMAND://0xE033:
			ret->addr = (char*)SecurityContext_p->encryptionCommand;
			ret->length = strlen((const char *)SecurityContext_p->encryptionCommand);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;	
		case ENCRTYPTIONTAG://0xE034:
			ret->addr = (char*)SecurityContext_p->encrtyptionTag;
			ret->length = strlen((const char *)SecurityContext_p->encrtyptionTag);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case ENCRYPTIONALGORITHM://0xE035:
			ret->addr = (char*)&SecurityContext_p->encryptionAlgorithm;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case PUBLICKEY://0xE036:
			ret->addr = (char*)SecurityContext_p->publickey;
			ret->length = strlen((const char *)SecurityContext_p->publickey);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case PRIVATEKEY://0xE037:
			ret->addr = (char*)SecurityContext_p->privatekey;
			ret->length = strlen((const char *)SecurityContext_p->privatekey);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case PUBLICKEYEXPDATE://0xE038:
			ret->addr = (char*)&SecurityContext_p->publicKeyExpDate;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;			
			//MCOM_WMMP_LIB_DBG_TEMP("WriteTLVSecurity:publicKeyExpDate =%d\n",SecurityContext_p->publicKeyExpDate);
			//return ret;			
			break;
		case ENCRYPTIONKEYALGORITHM://0xE039:
			ret->addr = (char*)&SecurityContext_p->encryptionKeyAlgorithm;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
			
		case ENCRYPTIONKEYMD5://0xE03A:
			ret->addr = (char*)SecurityContext_p->encryptionKeyMD5;
			ret->length = 16;
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case CONVPUBLICKEY://0xE03B:
			ret->addr = (char*)SecurityContext_p->convPublicKey;
			ret->length = strlen((const char *)SecurityContext_p->convPublicKey);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case CONVPRIVATEKEY://0xE03C:
			ret->addr = (char*)SecurityContext_p->convPrivateKey;
			ret->length = strlen((const char *)SecurityContext_p->convPrivateKey);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case CONVKEYEXPDATE://0xE03D:
			ret->addr = (char*)&SecurityContext_p->convKeyExpDate;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			//return ret;			
			break;	
		case CONVENCRYPTIONALOG://0xE03E:
			ret->addr = (char*)&SecurityContext_p->convEncryptionAlog;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case CONVENCRYPTIONMD5://0xE03F:
			ret->addr = (char*)SecurityContext_p->convEncryptionMD5;
			ret->length = strlen((const char *)SecurityContext_p->convEncryptionMD5);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case CONVENCRYPTIONMSGBODY://0xE040:
			ret->addr = (char*)SecurityContext_p->convEncryptionMsgBody;
			ret->length = strlen((const char *)SecurityContext_p->convEncryptionMsgBody);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case CONVAUTHFAILREPORT://0xE041:
			ret->addr = (char*)&SecurityContext_p->convAuthFailReport;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
		case CONVSINGLEENCRYPTBODY://0xE042:
			ret->addr = (char*)SecurityContext_p->convSingleEncryptBody;
			ret->length = strlen((const char *)SecurityContext_p->convSingleEncryptBody);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case CONVMULTIENCRYPTBODY://0xE043:
			ret->addr = (char*)SecurityContext_p->convMultiEncryptBody;
			ret->length = strlen((const char *)SecurityContext_p->convMultiEncryptBody);
			ret->TLVType = TLV_STRING;
			//return ret;			
			break;
		case CONVPACKEDENCRYPTBODY://0xE044:
			ret->addr = (char*)&SecurityContext_p->convPackedEncryptBody;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			//return ret;			
			break;
			
		default:
			break;			
	}
	
	WMMP_LIB_DBG_TEMP("ReadTLVSecurity ret->TLVType %d",ret->TLVType);
	WMMP_LIB_DBG_TEMP("ReadTLVSecurity ret->length %d",ret->length);
  #if 0
	for(loopi = 0;loopi<ret->length;loopi++)
	{
		WMMP_LIB_DBG_TEMP("ReadTLVSecurity ret->addr[%d] 0x%x",loopi,ret->addr[loopi]);
	}
  #endif
}

static void ReadTLVCustom(u16 Tag_Id,TLVRetVal_t* ret)
{
    u16	loopi = 0;
	WmmpCustomContext_t  *CustomContext_p = &(ptrToWmmpContext()->CustomContext);
	ret->addr = PNULL;
	ret->length = 0;
	ret->TLVType = TLV_STRING;	
	
	WMMP_LIB_DBG_TEMP("ReadTLVCustom Tag_Id 0x%04x",Tag_Id);
	switch(Tag_Id)
	{
		case TLVSYNCTAGNUM://0x8001:
			ret->addr = (char*)&CustomContext_p->TLVSyncTagNum;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			WMMP_LIB_DBG_TEMP("ReadTLVCustom TLVSyncTagNum %d",*(ret->addr));
			break;
		case MAXPDULENGHT://0x8002:
			ret->addr = (char*)&CustomContext_p->MaxPDULength;
			ret->length = LENGTH_OF_INT16;
			ret->TLVType = TLV_INT16;
			break;	
		case INITTERMINALNUM:
			ret->addr = (char*)CustomContext_p->UnregTerminalNum;
			ret->length = strlen((const char *)CustomContext_p->UnregTerminalNum);
			ret->TLVType = TLV_STRING;
			break;			
		case RMCTRLTRACEENABLE: /*0x81f0*/
			ret->addr = (char*)&CustomContext_p->remoteCtrlTraceEnabel;
			ret->length = LENGTH_OF_INT8;
			ret->TLVType = TLV_INT8;
			WMMP_LIB_DBG_TEMP("ReadTLVCustom RMCTRLTRACEENABLE %d",*(ret->addr));
			break;		
		default:
			break;	
	}

	WMMP_LIB_DBG_TEMP("ReadTLVCustom ret->TLVType %d",ret->TLVType);
	WMMP_LIB_DBG_TEMP("ReadTLVCustom ret->length %d",ret->length);
  #if 0
	for(loopi = 0;loopi<ret->length;loopi++)
	{
		WMMP_LIB_DBG_TEMP("ReadTLVCustom ret->addr[%d] 0x%x",loopi,ret->addr[loopi]);
	}
  #endif
}

static void ReadTLVCustom2(u16 Tag_Id,TLVRetVal_t* ret)
{
    u16	loopi = 0;
	WmmpCustomContext_t  *CustomContext_p = &(ptrToWmmpContext()->CustomContext);
	WmmpCustom2Context_t  *Custom2Context_p = &(ptrToWmmpContext()->Custom2Context);
	ret->addr = PNULL;
	ret->length = 0;
	ret->TLVType = TLV_STRING;
	
	WMMP_LIB_DBG_TEMP("ReadTLVCustom2 Tag_Id 0x%04x",Tag_Id);
	switch(Tag_Id)
	{
		case APPSERVER0IPPORT:
		ret->addr = (char*)Custom2Context_p->AppServerString[0];
		ret->length = strlen((const char *)Custom2Context_p->AppServerString[0]);
		ret->TLVType = TLV_STRING;
		WMMP_LIB_DBG_TEMP("ReadTLV SERVER0 addr %s",ret->addr);
		break;	
		case APPSERVER1IPPORT:
		ret->addr = (char*)Custom2Context_p->AppServerString[1];
		ret->length = strlen((const char *)Custom2Context_p->AppServerString[1]);
		ret->TLVType = TLV_STRING;
		WMMP_LIB_DBG_TEMP("ReadTLV SERVER1 addr %s",ret->addr);
		break;	
		case APPSERVER2IPPORT:
		ret->addr = (char*)Custom2Context_p->AppServerString[2];
		ret->length = strlen((const char *)Custom2Context_p->AppServerString[2]);
		ret->TLVType = TLV_STRING;
		WMMP_LIB_DBG_TEMP("ReadTLV SERVER2 addr %s",ret->addr);
		break;	
		case APPSERVER3IPPORT:
		ret->addr = (char*)Custom2Context_p->AppServerString[3];
		ret->length = strlen((const char *)Custom2Context_p->AppServerString[3]);
		ret->TLVType = TLV_STRING;
		WMMP_LIB_DBG_TEMP("ReadTLV SERVER3 addr %s",ret->addr);
		break;	
		case APPSERVER4IPPORT:
		ret->addr = (char*)Custom2Context_p->AppServerString[4];
		ret->length = strlen((const char *)Custom2Context_p->AppServerString[4]);
		ret->TLVType = TLV_STRING;
		WMMP_LIB_DBG_TEMP("ReadTLV SERVER4 addr %s",ret->addr);
		break;	
		case APPSERVER5IPPORT:
		ret->addr = (char*)Custom2Context_p->AppServerString[5];
		ret->length = strlen((const char *)Custom2Context_p->AppServerString[5]);
		ret->TLVType = TLV_STRING;
		WMMP_LIB_DBG_TEMP("ReadTLV SERVER5 addr %s",ret->addr);
		break;	
		case APPSERVER6IPPORT:
		ret->addr = (char*)Custom2Context_p->AppServerString[6];
		ret->length = strlen((const char *)Custom2Context_p->AppServerString[6]);
		ret->TLVType = TLV_STRING;
		WMMP_LIB_DBG_TEMP("ReadTLV SERVER6 addr %s",ret->addr);
		break;	
		case APPSERVER7IPPORT:
		ret->addr = (char*)Custom2Context_p->AppServerString[7];
		ret->length = strlen((const char *)Custom2Context_p->AppServerString[7]);
		ret->TLVType = TLV_STRING;
		WMMP_LIB_DBG_TEMP("ReadTLV SERVER7 addr %s",ret->addr);
		break;		
		default:
			break;	
	}
	
	WMMP_LIB_DBG_TEMP("ReadTLVCustom2 ret->TLVType %d",ret->TLVType);
	WMMP_LIB_DBG_TEMP("ReadTLVCustom2 ret->length %d",ret->length);
   
  #if 0
	for(loopi = 0;loopi<ret->length;loopi++)
	{
		WMMP_LIB_DBG_TEMP("ReadTLVSecurity ret->addr[%d] 0x%x",loopi,ret->addr[loopi]);
	}
  #endif
}

static void ReadTLVCustom3(u16 Tag_Id,TLVRetVal_t* ret)
{
    u16	loopi = 0;
	WmmpCustom3Context_t  *Custom3Context_p = &(ptrToWmmpContext()->Custom3Context);
	ret->addr = PNULL;
	ret->length = 0;
	ret->TLVType = TLV_STRING;
	
	WMMP_LIB_DBG_TEMP("ReadTLVCustom Tag_Id %x",Tag_Id);
	WMMP_LIB_DBG_TEMP("ReadTLVCustom Tag_Id %d",Tag_Id);
	switch(Tag_Id)
	{		
		case RMCTRLTRACEATNAME:	/*0x81f1*/
			ret->addr = (char*)Custom3Context_p->remoteCtrlTraceATNamParm;
			ret->length = strlen((const char *)Custom3Context_p->remoteCtrlTraceATNamParm);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATRET:	/*0x81f2*/
			//ret->addr = (char*)Custom3Context_p->remoteCtrlTraceATRet;
			//ret->length = strlen((const char *)Custom3Context_p->remoteCtrlTraceATRet);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATACK:	/*0x81f3*/
			ret->addr = (char*)Custom3Context_p->remoteCtrlTraceATAck;
			ret->length = strlen((const char *)Custom3Context_p->remoteCtrlTraceATAck);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATMSG:	/*0x81f4*/	
			ret->addr = (char*)Custom3Context_p->remoteCtrlTraceATMsg;
			ret->length = strlen((const char *)Custom3Context_p->remoteCtrlTraceATMsg);
			ret->TLVType = TLV_STRING;
			break;
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_ADC_GPIO__
		case CUSTOMADC:	/*0x81f5*/	
			ret->addr = (char*)&Custom3Context_p->wmmp_adc_value;
			ret->length = LENGTH_OF_INT16;
			ret->TLVType = TLV_INT16;
			break;
		case CUSTOMGPIOINPUT:	/*0x81f6*/	
			break;
		case CUSTOMGPIOOUTPUT:	/*0x81f7*/	
			ret->addr = (char*)&Custom3Context_p->wmmp_gpio_status;
			ret->length = LENGTH_OF_INT32;
			ret->TLVType = TLV_INT32;
			break;
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/
		case RMCTRLTRACEATCFGLISTN: /*0x82f0*/
			ret->addr = (char*)Custom3Context_p->rmCtrlTraceATNParmCfglist;
			ret->length = strlen((const char *)Custom3Context_p->rmCtrlTraceATNParmCfglist);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATCFGN:
			ret->addr = (char*)Custom3Context_p->rmCtrlTraceATNParmCfg;
			ret->length = strlen((const char *)Custom3Context_p->rmCtrlTraceATNParmCfg);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATM2MSTAN:
			ret->addr = (char*)Custom3Context_p->rmCtrlTraceATNParmStat;
			ret->length = strlen((const char *)Custom3Context_p->rmCtrlTraceATNParmStat);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATTRAPN:
			ret->addr = (char*)Custom3Context_p->rmCtrlTraceATNParmTrap;
			ret->length = strlen((const char *)Custom3Context_p->rmCtrlTraceATNParmTrap);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATAPPOPENN:
			ret->addr = (char*)Custom3Context_p->rmCtrlTraceATNParmAppopen;
			ret->length = strlen((const char *)Custom3Context_p->rmCtrlTraceATNParmAppopen);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATAPPCLOSEN:	
			ret->addr = (char*)Custom3Context_p->rmCtrlTraceATNParmAppclose;
			ret->length = strlen((const char *)Custom3Context_p->rmCtrlTraceATNParmAppclose);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATBHVNAME:
			ret->addr = (char*)Custom3Context_p->remoteCtrlTraceATNamParmBhv;
			ret->length = strlen((const char *)Custom3Context_p->remoteCtrlTraceATNamParmBhv);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATBROPENNAME:
			ret->addr = (char*)Custom3Context_p->rmCtrlTraceATNamParmbrOpen;
			ret->length = strlen((const char *)Custom3Context_p->rmCtrlTraceATNamParmbrOpen);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATBRSTAN:
			ret->addr = (char*)Custom3Context_p->rmCtrlTraceATNamParmbrSta;
			ret->length = strlen((const char *)Custom3Context_p->rmCtrlTraceATNamParmbrSta);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATCGPADDRN:
			ret->addr = (char*)Custom3Context_p->rmCtrlTraceATNamParmcgpAddr;
			ret->length = strlen((const char *)Custom3Context_p->rmCtrlTraceATNamParmcgpAddr);
			ret->TLVType = TLV_STRING;
			break;	
	case RMCTRLTRACEATAPPCLOSENALREADY:
			ret->addr = (char*)Custom3Context_p->rmCtrlTraceATNParmAppcloseAlready;
			ret->length = strlen((const char *)Custom3Context_p->rmCtrlTraceATNParmAppcloseAlready);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATCFGLISTRET:	/* 0x83f0*/
			//ret->addr = (char*)Custom3Context_p->remoteCtrlTraceATRetCfglist;
			//ret->length = strlen((const char *)Custom3Context_p->remoteCtrlTraceATRetCfglist);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATCFGRET:
			//ret->addr = (char*)Custom3Context_p->remoteCtrlTraceATRetCfg;
			//ret->length = strlen((const char *)Custom3Context_p->remoteCtrlTraceATRetCfg);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATM2MSTARET:
			//ret->addr = (char*)Custom3Context_p->remoteCtrlTraceATRetSta;
			//ret->length = strlen((const char *)Custom3Context_p->remoteCtrlTraceATRetSta);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATTRAPRET:
			//ret->addr = (char*)Custom3Context_p->remoteCtrlTraceATRetTrap;
			//ret->length = strlen((const char *)Custom3Context_p->remoteCtrlTraceATRetTrap);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATAPPOPENRET:
			//ret->addr = (char*)Custom3Context_p->remoteCtrlTraceATRetAppopen;
			//ret->length = strlen((const char *)Custom3Context_p->remoteCtrlTraceATRetAppopen);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATAPPCLOSERET:
			//ret->addr = (char*)Custom3Context_p->remoteCtrlTraceATRetAppclose;
			//ret->length = strlen((const char *)Custom3Context_p->remoteCtrlTraceATRetAppclose);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATBHVRET:
			//ret->addr = (char*)Custom3Context_p->remoteCtrlTraceATRetBhv;
			//ret->length = strlen((const char *)Custom3Context_p->remoteCtrlTraceATRetBhv);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATBROPENRET:
			//ret->addr = (char*)Custom3Context_p->rmCtrlTraceATRetbrOpen;
			//ret->length = strlen((const char *)Custom3Context_p->rmCtrlTraceATRetbrOpen);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATBRSTARET:
			//ret->addr = (char*)Custom3Context_p->rmCtrlTraceATRetbrSta;
			//ret->length = strlen((const char *)Custom3Context_p->rmCtrlTraceATRetbrSta);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATCGPADDRRET:
			//ret->addr = (char*)Custom3Context_p->rmCtrlTraceATRetcgpAddr;
			//ret->length = strlen((const char *)Custom3Context_p->rmCtrlTraceATRetcgpAddr);
			ret->TLVType = TLV_STRING;
			break;

		case RMCTRLTRACEATACKAPPOPEN: /* 0x84f0*/
			ret->addr = (char*)Custom3Context_p->rmCtrlTraceATAckAppopen;
			ret->length = strlen((const char *)Custom3Context_p->rmCtrlTraceATAckAppopen);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATACKACK:
			ret->addr = (char*)Custom3Context_p->remoteCtrlTraceATAckack;
			ret->length = strlen((const char *)Custom3Context_p->remoteCtrlTraceATAckack);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATMSGRECVDATA:  /*0x85f0*/
			ret->addr = (char*)Custom3Context_p->remoteCtrlTraceATRecvData;
			ret->length = strlen((const char *)Custom3Context_p->remoteCtrlTraceATRecvData);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATMSGSOCDESTR:
			ret->addr = (char*)Custom3Context_p->remoteCtrlTraceATSocDestroy;
			ret->length = strlen((const char *)Custom3Context_p->remoteCtrlTraceATSocDestroy);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATMSGCLOSEDIND:
			ret->addr = (char*)Custom3Context_p->remoteCtrlTraceATMsgClosedInd;
			ret->length = strlen((const char *)Custom3Context_p->remoteCtrlTraceATMsgClosedInd);
			ret->TLVType = TLV_STRING;
			break;
		case RMCTRLTRACEATMSGCLOSEDINDTOUT:
			ret->addr = (char*)Custom3Context_p->rmtTraceATMsgClosedIndTimeOut;
			ret->length = strlen((const char *)Custom3Context_p->rmtTraceATMsgClosedIndTimeOut);
			ret->TLVType = TLV_STRING;
			break;

			
		default:
			break;	
}

	WMMP_LIB_DBG_TEMP("ReadTLVCustom3 ret->TLVType %d",ret->TLVType);
	WMMP_LIB_DBG_TEMP("ReadTLVCustom3 ret->length %d",ret->length);
  #if 0
	for(loopi = 0;loopi<ret->length;loopi++)
	{
		WMMP_LIB_DBG_TEMP("ReadTLVCustom3 ret->addr[%d] 0x%x",loopi,ret->addr[loopi]);
	}
  #endif
}


/******************************************************************************
* Function:   ReadTLV    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
void ReadTLV(u16 Tag_Id ,TLVRetVal_t* ret)
{

	ret->addr=NULL;
	ret->length=0;
	ret->TLVType=TLV_DEFAULT;

	WMMP_LIB_DBG_TEMP("ReadTLV Tag_Id %x",Tag_Id);	
	WMMP_LIB_DBG_TEMP("ReadTLV Tag_Id %d",Tag_Id);	

	if((Tag_Id>=GPRSDAILNUM)&&(Tag_Id<=GATELIMIT))
	{
		ReadTLVcfg(Tag_Id,ret);
	}
	else if((Tag_Id>=UPDATEMODE)&&(Tag_Id<=CURRENTFILEREV))
	{
		ReadTLVUpdate(Tag_Id,ret);
	}
	else if((Tag_Id>=STATINFOREPORT)&&(Tag_Id<=OPERATIONMUTUALUSSDSTAT))
	{
		ReadTLVStatistic(Tag_Id,ret);
	}
	else if((Tag_Id>=PROTOCOLREV)&&(Tag_Id<=FAILWARNING))
	{
		ReadTLVStatus(Tag_Id,ret);
	}
	else if((Tag_Id>=SETCOMMAND)&&(Tag_Id<=EXTERNALDEVCTRL))
	{
		ReadTLVControl(Tag_Id,ret);
	}
	else if((Tag_Id>=SIMPIN1)&&(Tag_Id<=CONVPACKEDENCRYPTBODY))
	{
		ReadTLVSecurity(Tag_Id,ret);
	}
	else if(((Tag_Id>=TLVSYNCTAGNUM)&&(Tag_Id<=INITTERMINALNUM)) || (Tag_Id == RMCTRLTRACEENABLE)) 
	{
		ReadTLVCustom(Tag_Id,ret);
	}
	else if((Tag_Id >= RMCTRLTRACEATNAME) && (Tag_Id < RMCTRLTRACEATMAX))
	{
		ReadTLVCustom3(Tag_Id,ret);
	}
   	else if((Tag_Id>=APPSERVER0IPPORT)&&(Tag_Id<=APPSERVER7IPPORT))
	{
		ReadTLVCustom2(Tag_Id,ret);
	}
	
}


/******************************************************************************
* Function:   WriteTLVcfg    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
static void WriteTLVcfg(u16 Tag_Id, u16 length, char* value)
{	
	WmmpContext_t  *mainContext_p = ptrToWmmpContext();
	WmmpCfgContext_t  *CfgContext_p = &(mainContext_p->ConfigContext);
	WmmpCustomContext_t  *CustomContext_p = &(ptrToWmmpContext()->CustomContext);
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_PRIVATE_APN__
	AppWmmpContext_t     *appWmmpContext_p = (AppWmmpContext_t*)ptrToAppWmmpContext();
	WmmpNetWorkContext_t    *networkContext_p;
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/
	u8	ret8=0;
	u16	ret16=0;
	u32	ret32=0;
	/*
	if((length == 0)||(value == NULL)	)
	{
		return;
	}*/
	switch(Tag_Id)
	{
		case GPRSDAILNUM://0x0001:
			memset(CfgContext_p->GPRSDailNum,0x00,sizeof(CfgContext_p->GPRSDailNum));
			memcpy(CfgContext_p->GPRSDailNum,value,length);
			break;
		case M2MAPN://0x0002:
			memset(CfgContext_p->M2MAPN,0x00,sizeof(CfgContext_p->M2MAPN));
			memcpy(CfgContext_p->M2MAPN,value,length);
			break;
		case GPRSUSERID://0x0003:
			memset(CfgContext_p->GPRSUserId,0x00,sizeof(CfgContext_p->GPRSUserId));
			memcpy(CfgContext_p->GPRSUserId,value,length);
			break;
		case GPRSPASSWORD://0x0004:
			memset(CfgContext_p->GPRSPassword,0x00,sizeof(CfgContext_p->GPRSPassword));
			memcpy(CfgContext_p->GPRSPassword,value,length);	
			break;
		case SMSCENTRENUM://0x0005:
			memset(CfgContext_p->SMSCentreNum,0x00,sizeof(CfgContext_p->SMSCentreNum));
			memcpy(CfgContext_p->SMSCentreNum,value,length);
			break;
		case APPSRVTRANSKEY://0x0006:
			memset(CfgContext_p->AppSrvTransKey,0x00,sizeof(CfgContext_p->AppSrvTransKey));
			memcpy(CfgContext_p->AppSrvTransKey,value,length);
			break;
		case M2MSMSPECIALNUM://0x0007:
			memset(CfgContext_p->M2MSMSpecialNum,0x00,sizeof(CfgContext_p->M2MSMSpecialNum));
			memcpy(CfgContext_p->M2MSMSpecialNum,value,length);
			break;
		case M2MURLADDR://0x0008:
			memset(CfgContext_p->M2MURLAddr,0x00,sizeof(CfgContext_p->M2MURLAddr));
			memcpy(CfgContext_p->M2MURLAddr,value,length);
			break;
		case M2MIPADDR://0x0009:
			//back up first 
			mainContext_p->M2MIPAddr_b=CfgContext_p->M2MIPAddr;/* cbc@20100420: IP 地址先备份，用于参数回滚 */
			ret32 = Readu32(value);
			memcpy(&CfgContext_p->M2MIPAddr,&ret32,length);/* cbc@20100420: 更新IP 地址，用于参数回滚 */
			break;
		case M2MPORT://0x000A:
			//back up first 
			mainContext_p->M2MPort_b = CfgContext_p->M2MPort;
			
			ret32 = Readu32(value);
			memcpy(&CfgContext_p->M2MPort,&ret32,length);
			break;
		case APPSMSCENTRENUM://0x000B:
			//back up first 
			memcpy(mainContext_p->AppSMSCentreNum_b,CfgContext_p->AppSMSCentreNum,sizeof(CfgContext_p->AppSMSCentreNum));
			
			memset(CfgContext_p->AppSMSCentreNum,0x00,sizeof(CfgContext_p->AppSMSCentreNum));
			memcpy(CfgContext_p->AppSMSCentreNum,value,length);
			break;
		case APPSRVURLADDR://0x000C:
			memset(CfgContext_p->AppSrvURLAddr,0x00,sizeof(CfgContext_p->AppSrvURLAddr));
			memcpy(CfgContext_p->AppSrvURLAddr,value,length);
			break;
		case APPSRVIPADDR://0x000D:
			ret32 = Readu32(value);
			memcpy(&CfgContext_p->AppSrvIPAddr,&ret32,length);
			break;
		case APPSRVPORT://0x000E:
			ret32 = Readu32(value);
			memcpy(&CfgContext_p->AppSrvPort,&ret32,length);
			break;
			
		case reserve1://0x000F:
			//reserve
			break;	

		case DNSADDR://0x0010:
			memset(CfgContext_p->DNSAddr,0x00,sizeof(CfgContext_p->DNSAddr));
			memcpy(CfgContext_p->DNSAddr,value,length);
			break;
		case PERIOD_CFG://0x0011:
			ret32 = Readu32(value);
			//MCOM_WMMP_LIB_DBG_TEMP("write heart beat %d\n",ret32);
			memcpy(&CfgContext_p->Period,&ret32,length);
			break;
		case APPUSSDSMSSPECIALNUM://0x0012:
			memset(CfgContext_p->AppUSSDSMSSpecialNum,0x00,sizeof(CfgContext_p->AppUSSDSMSSpecialNum));
			memcpy(CfgContext_p->AppUSSDSMSSpecialNum,value,length);
			break;
		case M2MAPPAPN://0x0013:
			memset(CfgContext_p->M2MAppAPN,0x00,sizeof(CfgContext_p->M2MAppAPN));
			memcpy(CfgContext_p->M2MAppAPN,value,length);
			break;
		case BACKUPDNSADDR://0x0014:
			memset(CfgContext_p->BackupDNSAddr,0x00,sizeof(CfgContext_p->BackupDNSAddr));
			memcpy(CfgContext_p->BackupDNSAddr,value,length);
			break;

		case TLVSYNCTAGVALUE://0x0025:
			memset(CfgContext_p->TLVSyncTagValue,0x00,sizeof(CfgContext_p->TLVSyncTagValue));
			memcpy(CfgContext_p->TLVSyncTagValue,value,length);
			CustomContext_p->TLVSyncTagNum = length/2;
			WMMP_LIB_DBG_TEMP("writeTLVCFG 0025 TLVSyncTagNum %d",CustomContext_p->TLVSyncTagNum);
			break;
		case SMSRECVOVERTIME://0x0026:
			ret16 = Readu16(value);
			memcpy(&CfgContext_p->SMSRecvOvertime,&ret16,length);
			break;
		case MAXLOGINRETRYTIMES://0x0027:
			ret8 = *value;
			memcpy(&CfgContext_p->MaxLoginRetryTimes,&ret8,length);
			break;
		case DEFAULTECCODE://0x0028:
			memset(CfgContext_p->DefaultECCode,0x00,sizeof(CfgContext_p->DefaultECCode));
			memcpy(CfgContext_p->DefaultECCode,value,length);
			break;

		case LOCALPTRVALUE://0x0101:
			ret16 = Readu16(value);
			memcpy(&CfgContext_p->LocalPTRValue,&ret16,length);
			break;
		case LOCALPORTMODE://0x0102:
			ret8 = *value;
			memcpy(&CfgContext_p->LocalPortMode,&ret8,length);
			break;
		case LOCALPORTHALT://0x0103:
			ret8 = *value;
			memcpy(&CfgContext_p->LocalPorthalt,&ret8,length);
			break;
		case LOCALPORTVERIFY://0x0104:
			ret8 = *value;
			memcpy(&CfgContext_p->LocalPortVerify,&ret8,length);
			break;
		case LOCALPORTSCANPERIOD://0x0105:
			ret16 = Readu16(value);
			memcpy(&CfgContext_p->LocalPortScanPeriod,&ret16,length);
			break;
			
		case reserve2://0x0106:
			//reserve
			break;
			
		case DATAPACKRESPOVERTIME://0x0107:
			ret32 = Readu32(value);
			memcpy(&CfgContext_p->DataPackRespOvertime,&ret32,length);
			break;
		case TRANSFAILRETRYTIMES://0x0108:
			ret8 = *value;
			memcpy(&CfgContext_p->TransFailRetryTimes,&ret8,length);
			break;
		case GATELIMIT://0x0109:
			ret8 = *value;
			memcpy(&CfgContext_p->GateLimit,&ret8,length);
			break;	
			
		default:
			break;
	}
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_PRIVATE_APN__
	if(Tag_Id==M2MAPN)
	{
		if(wmmp_get_apn_type()==WMMP_APN_PRIVATE)
		{
            
            networkContext_p = &(appWmmpContext_p->networkContext[2]);
            memcpy((void*)(networkContext_p->p_account->ps_account_info.apn),
					 (void*)mainContext_p->ConfigContext.M2MAPN, MAX_GPRS_APN_LEN);
            networkContext_p->p_account->ps_account_info.apn_length=strlen((char*)(networkContext_p->p_account->ps_account_info.apn));

			wmmpProcessSave(NVRAM_EF_WMMP_ACCOUNT_LID,(void *)networkContext_p->p_account,sizeof(Wmmp_mmi_abm_account_info_struct));
		}
	
	}
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/
//	memcpy(&wmmpSavedCfgContext,CfgContext_p,sizeof(WmmpCfgContext_t));
}


/******************************************************************************
* Function:   WriteTLVUpdate    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
static void WriteTLVUpdate(u16 Tag_Id, u16 length, char* value)
{
	WmmpUpdateContext_t  *UpdateContext_p = &(ptrToWmmpContext()->UpdateContext);
	u8		ret8=0;
	u16	ret16=0;
	u32	ret32=0;
	/*
	if((length == 0)||(value == NULL)	)
	{
		return;
	}	*/
	switch(Tag_Id)
	{
		case UPDATEMODE://0x1000:
			ret8 = *value;
			memcpy(&UpdateContext_p->UpdateMode,&ret8,length);
			break;
		case UPDATESRVURLADDR://0x1001:
			memset(UpdateContext_p->UpdateSrvURLAddr,0x00,sizeof(UpdateContext_p->UpdateSrvURLAddr));
			memcpy(UpdateContext_p->UpdateSrvURLAddr,value,length);
			break;
		case UPDATESRVIPADDR://0x1002:
			memset(UpdateContext_p->UpdateSrvIPAddr,0x00,sizeof(UpdateContext_p->UpdateSrvIPAddr));
			memcpy(UpdateContext_p->UpdateSrvIPAddr,value,length);
			break;
		case UPDATESRVPORT://0x1003:
			ret16 = Readu16(value);
			memcpy(&UpdateContext_p->UpdateSrvPort,&ret16,length);
			break;
		case UPDATESRVLOGINPASSWORD://0x1004:
			memset(UpdateContext_p->UpdateSrvLoginPassword,0x00,sizeof(UpdateContext_p->UpdateSrvLoginPassword));
			memcpy(UpdateContext_p->UpdateSrvLoginPassword,value,length);
			break;
		case UPDATEFILEREV://0x1005:
			memset(UpdateContext_p->UpdateFileRev,0x00,sizeof(UpdateContext_p->UpdateFileRev));
			memcpy(UpdateContext_p->UpdateFileRev,value,length);
			break;
		case UPDATEFILELENGTH://0x1006:
			ret32 = Readu32(value);
			memcpy(&UpdateContext_p->UpdateFileLength,&ret32,length);
			break;
		case CRC16CHECK://0x1007:
			ret16 = Readu16(value);
			memcpy(&UpdateContext_p->CRC16Check,&ret16,length);
			break;
		case CRC32CHECK://0x1008:
			ret32 = Readu32(value);
			memcpy(&UpdateContext_p->CRC32Check,&ret32,length);
			break;
		case UPDATEAFFAIRTRANSID://0x1009:
			ret16 = Readu32(value);
			memcpy(&UpdateContext_p->UpdateAffairTransId,&ret32,length);
			break;
		case FILERELEASEDATA://0x100A:
			memset(UpdateContext_p->FileReleaseData,0x00,sizeof(UpdateContext_p->FileReleaseData));
			memcpy(UpdateContext_p->FileReleaseData,value,length);
			break;
		case FORCETOUPDATEPARAM://0x100B:
			ret8 = *value;
			memcpy(&UpdateContext_p->ForcetoUpdateParam,&ret8,length);
			break;	
		case CURRENTFILEREV://0x100C
			memset(UpdateContext_p->CurrentFileRev,0x00,sizeof(UpdateContext_p->CurrentFileRev));
			memcpy(UpdateContext_p->CurrentFileRev,value,length);
			break;	
		default:
			break;			
	}
//	memcpy(&wmmpSavedUpdateContext,UpdateContext_p,sizeof(WmmpUpdateContext_t));
}


/******************************************************************************
* Function:   WriteTLVStatistic    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
static void WriteTLVStatistic(u16 Tag_Id, u16 length, char* value)
{
	WmmpStatisticContext_t  *StatisticContext_p = &(ptrToWmmpContext()->StatisticContext);
	u8		ret8=0;
	u16	ret16=0;
	u32	ret32=0;
	/*
	if((length == 0)||(value == NULL)	)
	{
		return;
	}	*/
	switch(Tag_Id)
	{
		case STATINFOREPORT://0x2001:
			ret8 = *value;
			memcpy(&StatisticContext_p->StatInfoReport,&ret8,length);
			break;
		case REPORTTIME://0x2002:
			ret32 = Readu32(value);
			memcpy(&StatisticContext_p->ReportTime,&ret32,length);
			break;
		case PERIODREPORT://0x2003:
			ret32 = Readu32(value);
			memcpy(&StatisticContext_p->PeriodReport,&ret32,length);
			break;
		case MONTHLYOPERATIONSIGN://0x2004:
			ret8 = *value;
			memcpy(&StatisticContext_p->MonthlyOperationSign,&ret8,length);
			break;
		case CLEARSTATDATA://0x2005:
			ret8 = *value;
			memcpy(&StatisticContext_p->ClearStatData,&ret8,length);
			break;
		case STATSTARTTIME://0x2006:
			ret32 = Readu32(value);
			memcpy(&StatisticContext_p->StatStartTime,&ret32,length);
			break;
		case STATFINISHTIME://0x2007:
			ret32 = Readu32(value);
			memcpy(&StatisticContext_p->statFinishTime,&ret32,length);
			break;
		case SMSDELIVERYSUCC://0x2008:
			ret32 = Readu32(value);
			memcpy(&StatisticContext_p->SMSDeliverySucc,&ret32,length);
			break;
		case SMSDELIVERUFAIL://0x2009:
			ret32 = Readu32(value);
			memcpy(&StatisticContext_p->SMSDeliveruFail,&ret32,length);
			break;
		case SMSRECEIVEDNUM://0x200A:
			ret32 = Readu32(value);
			memcpy(&StatisticContext_p->SMSReceivedNum,&ret32,length);
			break;
		case GRPSDATASIZE://0x200B:
			ret32 = Readu32(value);
			memcpy(&StatisticContext_p->GRPSDataSize,&ret32,length);
			break;	
		case USSDSUCCNUM://0x200C:
			ret32 = Readu32(value);
			memcpy(&StatisticContext_p->USSDSuccNum,&ret32,length);
			break;	
		case USSDFAILNUM://0x200D:
			ret32 = Readu32(value);
			memcpy(&StatisticContext_p->USSDFailNum,&ret32,length);
			break;	
		case USSDRECEIVEDNUM://0x200E:
			ret32 = Readu32(value);
			memcpy(&StatisticContext_p->USSDReceivedNum,&ret32,length);
			break;	
			
		case SMSSTATPARAM://0x2011:
			memset(StatisticContext_p->SMSStatParam,0x00,sizeof(StatisticContext_p->SMSStatParam));
			memcpy(StatisticContext_p->SMSStatParam,value,length);
			break;	
		case SMSSTATAUTOREPORTPARAM://0x2012:
			memset(StatisticContext_p->SMSStatAutoReportParam,0x00,sizeof(StatisticContext_p->SMSStatAutoReportParam));
			memcpy(StatisticContext_p->SMSStatAutoReportParam,value,length);
			break;	
		case M2MMUTUALSMSSTAT://0x2013:
			memset(StatisticContext_p->M2MMutualSMSStat,0x00,sizeof(StatisticContext_p->M2MMutualSMSStat));
			memcpy(StatisticContext_p->M2MMutualSMSStat,value,length);
			break;	
		case OPERATIONMUTUALSMSSTAT://0x2014:
			memset(StatisticContext_p->OperationMutualSMSStat,0x00,sizeof(StatisticContext_p->OperationMutualSMSStat));
			memcpy(StatisticContext_p->OperationMutualSMSStat,value,length);
			break;	
		case GPRSOPERATIONSTATPARAM://0x2015:
			memset(StatisticContext_p->GPRSOperationStatParam,0x00,sizeof(StatisticContext_p->GPRSOperationStatParam));
			memcpy(StatisticContext_p->GPRSOperationStatParam,value,length);
			break;	
		case GPRSOPERATIONSTATREPORTPARAM://0x2016:
			memset(StatisticContext_p->GPRSOperationStatReportParam,0x00,sizeof(StatisticContext_p->GPRSOperationStatReportParam));
			memcpy(StatisticContext_p->GPRSOperationStatReportParam,value,length);
			break;	
		case M2MMUTUALGPRSDATASIZE://0x2017:
			memset(StatisticContext_p->M2MMutualGPRSDataSize,0x00,sizeof(StatisticContext_p->M2MMutualGPRSDataSize));
			memcpy(StatisticContext_p->M2MMutualGPRSDataSize,value,length);
			break;
		case OPERATIONMUTUALGPRSDATASIZE://0x2018:
			memset(StatisticContext_p->OperationMutualGPRSDataSize,0x00,sizeof(StatisticContext_p->OperationMutualGPRSDataSize));
			memcpy(StatisticContext_p->OperationMutualGPRSDataSize,value,length);
			break;	
		case USSDSTATPARAM://0x2019:
			memset(StatisticContext_p->USSDStatParam,0x00,sizeof(StatisticContext_p->USSDStatParam));
			memcpy(StatisticContext_p->USSDStatParam,value,length);
			break;
		case USSDSTATREPORTPARAM://0x201A:
			memset(StatisticContext_p->USSDStatReportParam,0x00,sizeof(StatisticContext_p->USSDStatReportParam));
			memcpy(StatisticContext_p->USSDStatReportParam,value,length);
			break;
		case M2MMUTUALUSSDSTAT://0x201B:
			memset(StatisticContext_p->M2MMutualUSSDStat,0x00,sizeof(StatisticContext_p->M2MMutualUSSDStat));
			memcpy(StatisticContext_p->M2MMutualUSSDStat,value,length);
			break;
		case OPERATIONMUTUALUSSDSTAT://0x201C:
			memset(StatisticContext_p->OperationMutualUSSDStat,0x00,sizeof(StatisticContext_p->OperationMutualUSSDStat));
			memcpy(StatisticContext_p->OperationMutualUSSDStat,value,length);
			break;	
			
		default:
			break;			
	}
//	memcpy(&wmmpSavedStatisticContext,StatisticContext_p,sizeof(WmmpStatisticContext_t));
}	

/******************************************************************************
* Function:   WriteTLVStatus    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
static void WriteTLVStatus(u16 Tag_Id, u16 length, char* value)
{
	WmmpTermStatusContext_t  *StatusContext_p = &(ptrToWmmpContext()->StatusContext);
	u8		ret8=0;
	u16	ret16=0;
	u32	ret32=0;
	/*
	if((length == 0)||(value == NULL)	)
	{
		return;
	}	*/
	switch(Tag_Id)
	{
		case PROTOCOLREV://0x3001:
			memset(StatusContext_p->ProtocolRev,0x00,sizeof(StatusContext_p->ProtocolRev));
			memcpy(StatusContext_p->ProtocolRev,value,length);
			break;
		case TERMINALSERIESNUM://0x3002:
			memset(StatusContext_p->TerminalSeriesNum,0x00,sizeof(StatusContext_p->TerminalSeriesNum));
			memcpy(StatusContext_p->TerminalSeriesNum,value,length);
			break;
		case IMSINUM://0x3003:
			memset(StatusContext_p->IMSINum,0x00,sizeof(StatusContext_p->IMSINum));
			memcpy(StatusContext_p->IMSINum,value,length);
			break;
		case FIRMWAREREV://0x3004:
			memset(StatusContext_p->FirmwareRev,0x00,sizeof(StatusContext_p->FirmwareRev));
			memcpy(StatusContext_p->FirmwareRev,value,length);
			break;
		case PREVPERIOD://0x3005:
			ret32 = Readu32(value);
			memcpy(&StatusContext_p->PrevPeriod,&ret32,length);
			break;
		case CELLULARID://0x3006:
			ret32 = Readu32(value);
			memcpy(&StatusContext_p->CellularId,&ret32,length);
			break;
		case SIGNALLEVEL://0x3007:
			ret8 = *value;
			memcpy(&StatusContext_p->SignalLevel,&ret8,length);
			break;
		case INITPARAFLAG://0x3008:
			ret8 = *value;
			memcpy(&StatusContext_p->InitParaFlag,&ret8,length);
			break;
		case SYSTEMTIME://0x3009:
			ret32 = Readu32(value);
			memcpy(&StatusContext_p->SystemTime,&ret32,length);
			break;
			
		case reserve3://0x300A:
			//reserve		
			break;
			
		case ALARMSTATUS://0x300B:
			ret8 = *value;
			memcpy(&StatusContext_p->AlarmStatus,&ret8,length);
			break;	
		case ALARMTYPE://0x300C:
			ret8 = *value;
			memcpy(&StatusContext_p->AlarmType,&ret8,length);
			break;	
		case ALARMCODE://0x300D:
			ret16 = Readu16(value);
			WMMP_LIB_DBG_TEMP("WriteTLVStatus TLV 0x300D INT16 ORG value 0x%04x %d",StatusContext_p->AlarmCode,StatusContext_p->AlarmCode);
			WMMP_LIB_DBG_TEMP("WriteTLVStatus TLV 0x300D INT16 value 0x%04x %d",ret16,ret16);
			memcpy(&StatusContext_p->AlarmCode,&ret16,length);
			WMMP_LIB_DBG_TEMP("WriteTLVStatus TLV 0x300D INT16 NEW AlarmCode 0x%04x %d",StatusContext_p->AlarmCode,StatusContext_p->AlarmCode);
			break;	
		case RESTOREALARM://0x300E:
			memset(StatusContext_p->RestoreAlarm,0x00,sizeof(StatusContext_p->RestoreAlarm));
			memcpy(StatusContext_p->RestoreAlarm,value,length);
			break;	
		/*bob modify 20101127*/	//case DEVSTATUS://0x300F:DEVSTATUS			 
		case M2MDEVSTATUS:
			memset(StatusContext_p->DEVStatus,0x00,sizeof(StatusContext_p->DEVStatus));
			memcpy(StatusContext_p->DEVStatus,value,length);
			break;	
		case SUPPORTEDDEV://0x3010:
			memset(StatusContext_p->SupportedDev,0x00,sizeof(StatusContext_p->SupportedDev));
			memcpy(StatusContext_p->SupportedDev,value,length);
			break;	
		case PACKLOSTNUM://0x3011:
			ret32 = Readu32(value);
			memcpy(&StatusContext_p->PackLostNum,&ret32,length);
			break;	
		case FAILWARNING://0x3012:
			ret32 = Readu32(value);
			memcpy(&StatusContext_p->FailWarning,&ret32,length);
			break;
			
		default:
			break;			
	}
//	memcpy(&wmmpSavedStatusContext,StatusContext_p,sizeof(WmmpTermStatusContext_t));
}

/******************************************************************************
* Function:   WriteTLVControl    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
static void WriteTLVControl(u16 Tag_Id, u16 length, char* value)
{
	WmmpControlContext_t  *ControlContext_p = &(ptrToWmmpContext()->ControlContext);
	u8		ret8=0;
	u16	ret16=0;
	u32	ret32=0;
	/*
	if((length == 0)||(value == NULL)	)
	{
		return;
	}	*/
	switch(Tag_Id)
	{
		case SETCOMMAND://0x4001:
			ret8 = *value;
			memcpy(&ControlContext_p->SetCommand,&ret8,length);
			break;
		case CONNECTMODE://0x4002:
			ret8 = *value;
			memcpy(&ControlContext_p->ConnectMode,&ret8,length);
			break;
		case DATATRANSMODE://0x4003:
			ret8 = *value;
			memcpy(&ControlContext_p->DataTransMode,&ret8,length);
			break;
		case PERIOD_CTRL://0x4004:
			ret16 = Readu16(value);
			memcpy(&ControlContext_p->Period,&ret16,length);
			break;
		case COLLECTDATATRANSMODE://0x4005:
			ret8 = *value;
			memcpy(&ControlContext_p->CollectDataTransMode,&ret8,length);
			break;
		case M2MCONNECTMODEONIP://0x4006:
			memset(ControlContext_p->M2MConnectModeOnIP,0x00,sizeof(ControlContext_p->M2MConnectModeOnIP));
			memcpy(ControlContext_p->M2MConnectModeOnIP,value,length);
			break;
		case USERDATAID://0x4007:
			memset(ControlContext_p->UserDataID,0x00,sizeof(ControlContext_p->UserDataID));
			memcpy(ControlContext_p->UserDataID,value,length);
			break;
		case SUBPACKMODEPARAM://0x4008:
			memset(ControlContext_p->SubPackModeParam,0x00,sizeof(ControlContext_p->SubPackModeParam));
			memcpy(ControlContext_p->SubPackModeParam,value,length);
			break;
		case PLATFORMPARAMOPT://0x4009:
			ret8 = *value;
			memcpy(&ControlContext_p->PlatformParamOpt,&ret8,length);
			break;
			
		case EXECUTEPARAMOPT://0x4010:
			memset(ControlContext_p->ExecuteParamOpt,0x00,sizeof(ControlContext_p->ExecuteParamOpt));
			memcpy(ControlContext_p->ExecuteParamOpt,value,length);
			break;

		case lengthblank://0x4011:
			//length=0
			break;
			
		case DESSERIESNUM://0x4012:
			memset(ControlContext_p->DesSeriesNum,0x00,sizeof(ControlContext_p->DesSeriesNum));
			memcpy(ControlContext_p->DesSeriesNum,value,length);
			break;	
		case SRCSERIESNUM://0x4013:
			memset(ControlContext_p->SrcSeriesNum,0x00,sizeof(ControlContext_p->SrcSeriesNum));
			memcpy(ControlContext_p->SrcSeriesNum,value,length);
			break;	
		case DESECCODE://0x4014:
			memset(ControlContext_p->DesECCode,0x00,sizeof(ControlContext_p->DesECCode));
			memcpy(ControlContext_p->DesECCode,value,length);
			break;	
		case SRCECCODE://0x4015:
			memset(ControlContext_p->SrcECCode,0x00,sizeof(ControlContext_p->SrcECCode));
			memcpy(ControlContext_p->SrcECCode,value,length);
			break;	
		case TRANSCTRLCOMMANDSTATICPARAM://0x4016:
			memset(ControlContext_p->TransCtrlCommandStaticParam,0x00,sizeof(ControlContext_p->TransCtrlCommandStaticParam));
			memcpy(ControlContext_p->TransCtrlCommandStaticParam,value,length);
			break;	
		case VERIFYTLVBYCRC32://0x4017:
			memset(ControlContext_p->VerifyTLVByCRC32,0x00,sizeof(ControlContext_p->VerifyTLVByCRC32));
			memcpy(ControlContext_p->VerifyTLVByCRC32,value,length);
			break;	
		case EXTERNALDEVCTRL://0x4021:
			memset(ControlContext_p->ExternalDevCtrl,0x00,sizeof(ControlContext_p->ExternalDevCtrl));
			memcpy(ControlContext_p->ExternalDevCtrl,value,length);
			break;	
			
		default:
			break;			
	}
//	memcpy(&wmmpSavedControlContext,ControlContext_p,sizeof(WmmpControlContext_t));
}

/******************************************************************************
* Function:   WriteTLVSecurity    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
static void WriteTLVSecurity(u16 Tag_Id, u16 length, char* value)
{
	WmmpContext_t  *mainContext_p = ptrToWmmpContext();
	WmmpSecurityContext_t  *SecurityContext_p = &(ptrToWmmpContext()->SecurityContext);
	u8		ret8=0;
	u16	ret16=0;
	u32	ret32=0;
	/*
	if((length == 0)||(value == NULL)	)
	{
		return;
	}	*/
	//MCOM_WMMP_LIB_DBG_TEMP("WriteTLVSecurity 1 TLV %x %d\n",Tag_Id,length);
	switch(Tag_Id)
	{
		case SIMPIN1://0xE001:
			memset(SecurityContext_p->SIMPin1,0x00,sizeof(SecurityContext_p->SIMPin1));
			memcpy(SecurityContext_p->SIMPin1,value,length);
			break;
		case SIMPIN1MD5://0xE002:
			memset(SecurityContext_p->SIMPin1MD5,0x00,sizeof(SecurityContext_p->SIMPin1MD5));
			memcpy(SecurityContext_p->SIMPin1MD5,value,length);
			break;
		case SIMPIN1ACTIVE://0xE003:
			ret8 = *value;
			memcpy(&SecurityContext_p->SIMPin1Active,&ret8,length);
			break;
		case SIMPIN2://0xE004:
			memset(SecurityContext_p->SIMPin2,0x00,sizeof(SecurityContext_p->SIMPin2));
			memcpy(SecurityContext_p->SIMPin2,value,length);
			break;
		case SIMPIN2MD5://0xE005:
			memset(SecurityContext_p->SIMPin2MD5,0x00,sizeof(SecurityContext_p->SIMPin2MD5));
			memcpy(SecurityContext_p->SIMPin2MD5,value,length);
			break;
		case SIMPIN2ACTIVE://0xE006:
			ret8 = *value;
			memcpy(&SecurityContext_p->SIMPin2Active,&ret8,length);
			break;
		case SIMPUK1://0xE007:
			memset(SecurityContext_p->SIMPuk1,0x00,sizeof(SecurityContext_p->SIMPuk1));
			memcpy(SecurityContext_p->SIMPuk1,value,length);
			break;
		case SIMPUK1MD5://0xE008:
			memset(SecurityContext_p->SIMPuk1MD5,0x00,sizeof(SecurityContext_p->SIMPuk1MD5));
			memcpy(SecurityContext_p->SIMPuk1MD5,value,length);
			break;
		case SIMPUK1ACTIVE://0xE009:
			ret8 = *value;
			memcpy(&SecurityContext_p->SIMPuk1Active,&ret8,length);
			break;
		case SIMPUK2://0xE00A:
			memset(SecurityContext_p->SIMPuk2,0x00,sizeof(SecurityContext_p->SIMPuk2));
			memcpy(SecurityContext_p->SIMPuk2,value,length);
			break;
		case SIMPUK2MD5://0xE00B:
			memset(SecurityContext_p->SIMPuk2MD5,0x00,sizeof(SecurityContext_p->SIMPuk2MD5));
			memcpy(SecurityContext_p->SIMPuk2MD5,value,length);
			break;	
		case SIMPUK2ACTIVE://0xE00C:
			ret8 = *value;
			memcpy(&SecurityContext_p->SIMPuk2Active,&ret8,length);
			break;	
		case BINDPARAACTIVE://0xE00D:
			ret8 = *value;
			memcpy(&SecurityContext_p->BindParaActive,&ret8,length);
			break;	
		case BINDIMEI://0xE00E:
			//already get it
			break;	
		case BINDIMEIMD5://0xE00F:
			memset(SecurityContext_p->BindIMEIMD5,0x00,sizeof(SecurityContext_p->BindIMEIMD5));
			memcpy(SecurityContext_p->BindIMEIMD5,value,length);
			break;				
		case BINDIMEIORIG://0xE010:
			memset(SecurityContext_p->BindIMEIOrig,0x00,sizeof(SecurityContext_p->BindIMEIOrig));
			memcpy(SecurityContext_p->BindIMEIOrig,value,length);
			break;
		case BINDIMEIORIGMD5://0xE011:
			memset(SecurityContext_p->BindIMEIOrigMD5,0x00,sizeof(SecurityContext_p->BindIMEIOrigMD5));
			memcpy(SecurityContext_p->BindIMEIOrigMD5,value,length);
			break;	
		case BINDIMSI://0xE012:
			//already get it
			break;	
		case BINDIMSIMD5://0xE013:
			memset(SecurityContext_p->BindIMSIMD5,0x00,sizeof(SecurityContext_p->BindIMSIMD5));
			memcpy(SecurityContext_p->BindIMSIMD5,value,length);
			break;	
		case BINDIMSIORIG://0xE014:
			memset(SecurityContext_p->BindIMSIOrig,0x00,sizeof(SecurityContext_p->BindIMSIOrig));
			memcpy(SecurityContext_p->BindIMSIOrig,value,length);
			break;	
		case BINDIMSIORIGMD5://0xE015:
			memset(SecurityContext_p->BindIMSIOrigMD5,0x00,sizeof(SecurityContext_p->BindIMSIOrigMD5));
			memcpy(SecurityContext_p->BindIMSIOrigMD5,value,length);
			break;	
		case BINDCURRENTMD5://0xE016:
			memset(SecurityContext_p->BindCurrentMD5,0x00,sizeof(SecurityContext_p->BindCurrentMD5));
			memcpy(SecurityContext_p->BindCurrentMD5,value,length);
			break;	
		case BINDORIGINALMD5://0xE017:
			memset(SecurityContext_p->BindOriginalMD5,0x00,sizeof(SecurityContext_p->BindOriginalMD5));
			memcpy(SecurityContext_p->BindOriginalMD5,value,length);
			break;	
		case PIN1ERRORALLOW://0xE018:
			ret16 = Readu16(value);
			memcpy(&SecurityContext_p->Pin1ErrorAllow,&ret16,length);
			break;	
		case PIN2ERRORALLOW://0xE019:
			ret16 = Readu16(value);
			memcpy(&SecurityContext_p->Pin2ErrorAllow,&ret16,length);
			break;		
		case PUK1ERRORALLOW://0xE01A:
			ret16 = Readu16(value);
			memcpy(&SecurityContext_p->Puk1ErrorAllow,&ret16,length);
			break;
		case PUK2ERRORALLOW://0xE01B:
			ret16 = Readu16(value);
			memcpy(&SecurityContext_p->Puk2ErrorAllow,&ret16,length);
			break;	
		case M2MPASSCLEARPARA://0xE020:
			ret8 = *value;
			memcpy(&SecurityContext_p->M2MPassClearPara,&ret8,length);
			break;	
		case INITPARA://0xE021:
			memset(SecurityContext_p->InitPara,0x00,sizeof(SecurityContext_p->InitPara));
			memcpy(SecurityContext_p->InitPara,value,length);
			break;	
		case INITPARAUSED://0xE022:
			ret8 = *value;
			memcpy(&SecurityContext_p->InitParaUsed,&ret8,length);
			break;	
		case INITCOMMAND://0xE023:
			memset(SecurityContext_p->InitCommand,0x00,sizeof(SecurityContext_p->InitCommand));
			memcpy(SecurityContext_p->InitCommand,value,length);
			break;				
		case INITTLVACTTAG://0xE024:
			memset(SecurityContext_p->InitTLVactTag,0x00,sizeof(SecurityContext_p->InitTLVactTag));
			memcpy(SecurityContext_p->InitTLVactTag,value,length);
			break;
		case UPLINKPASSWORD://0xE025:
			memset(SecurityContext_p->uplinkPassword,0x00,sizeof(SecurityContext_p->uplinkPassword));
			memcpy(SecurityContext_p->uplinkPassword,value,length);

			memcpy(mainContext_p->uplinkPassword_b,SecurityContext_p->uplinkPassword,sizeof(SecurityContext_p->uplinkPassword));		/*cbc@20100518 for 安全参数回滚 */	
			break;
		case UPLINKPASSWORDMD5://0xE026:
			memset(SecurityContext_p->uplinkPasswordMD5,0x00,sizeof(SecurityContext_p->uplinkPasswordMD5));
			memcpy(SecurityContext_p->uplinkPasswordMD5,value,length);
			break;
		case UPLINKPASSWORDEXPDATE://0xE027:
			ret32 = Readu32(value);
			memcpy(&SecurityContext_p->uplinkPasswordExpDate,&ret32,length);
			break;
		case DOWNLINKPASSWORD://0xE028:
			memset(SecurityContext_p->downlinkPassword,0x00,sizeof(SecurityContext_p->downlinkPassword));
			memcpy(SecurityContext_p->downlinkPassword,value,length);

			memcpy(mainContext_p->downlinkPassword_b,SecurityContext_p->downlinkPassword,sizeof(SecurityContext_p->downlinkPassword));	/*cbc@20100518 for 安全参数回滚 */
			break;	
		case DOWNLINKPASSWORDMD5://0xE029:
			memset(SecurityContext_p->downlinkPasswordMD5,0x00,sizeof(SecurityContext_p->downlinkPasswordMD5));
			memcpy(SecurityContext_p->downlinkPasswordMD5,value,length);
			break;
		case DOWNLINKPASSWORDEXPDATE://0xE02A:
			ret32 = Readu32(value);
			memcpy(&SecurityContext_p->downlinkPasswordExpDate,&ret32,length);
			break;
			
		case ENCRYPTIONSUPPORT://0xE031:
			ret8 = *value;
			memcpy(&SecurityContext_p->encryptionSupport,&ret8,length);
			break;
		case ENCRYPTIONUSED://0xE032:
			ret8 = *value;
			memcpy(&SecurityContext_p->encryptionUsed,&ret8,length);
			break;
		case ENCRYPTIONCOMMAND://0xE033:
			memset(SecurityContext_p->encryptionCommand,0x00,sizeof(SecurityContext_p->encryptionCommand));
			memcpy(SecurityContext_p->encryptionCommand,value,length);
			break;	
		case ENCRTYPTIONTAG://0xE034:
			memset(SecurityContext_p->encrtyptionTag,0x00,sizeof(SecurityContext_p->encrtyptionTag));
			memcpy(SecurityContext_p->encrtyptionTag,value,length);
			break;
		case ENCRYPTIONALGORITHM://0xE035:
			ret8 = *value;
			memcpy(&SecurityContext_p->encryptionAlgorithm,&ret8,length);
			break;
		case PUBLICKEY://0xE036:
			memset(SecurityContext_p->publickey,0x00,sizeof(SecurityContext_p->publickey));
			memcpy(SecurityContext_p->publickey,value,length);

			memcpy(mainContext_p->publickey_b,SecurityContext_p->publickey,sizeof(SecurityContext_p->publickey));	/*cbc@20100518 for 安全参数回滚 */
			break;
		case PRIVATEKEY://0xE037:
			memset(SecurityContext_p->privatekey,0x00,sizeof(SecurityContext_p->privatekey));
			memcpy(SecurityContext_p->privatekey,value,length);
			break;
		case PUBLICKEYEXPDATE://0xE038:
			ret32 = Readu32(value);
			memcpy(&SecurityContext_p->publicKeyExpDate,&ret32,length);		
			//MCOM_WMMP_LIB_DBG_TEMP("WriteTLVSecurity:publicKeyExpDate =0x%x\n",SecurityContext_p->publicKeyExpDate);
			break;
		case ENCRYPTIONKEYALGORITHM://0xE039:
			ret8 = *value;
			memcpy(&SecurityContext_p->encryptionKeyAlgorithm,&ret8,length);
			break;
			
		case ENCRYPTIONKEYMD5://0xE03A:
			memset(SecurityContext_p->encryptionKeyMD5,0x00,sizeof(SecurityContext_p->encryptionKeyMD5));
			memcpy(SecurityContext_p->encryptionKeyMD5,value,length);
			break;
		case CONVPUBLICKEY://0xE03B:
			memset(SecurityContext_p->convPublicKey,0x00,sizeof(SecurityContext_p->convPublicKey));
			memcpy(SecurityContext_p->convPublicKey,value,length);
			break;
		case CONVPRIVATEKEY://0xE03C:
			memset(SecurityContext_p->convPrivateKey,0x00,sizeof(SecurityContext_p->convPrivateKey));
			memcpy(SecurityContext_p->convPrivateKey,value,length);
			break;
		case CONVKEYEXPDATE://0xE03D:
			ret32 = Readu32(value);
			memcpy(&SecurityContext_p->convKeyExpDate,&ret32,length);
			break;	
		case CONVENCRYPTIONALOG://0xE03E:
			ret8 = *value;
			memcpy(&SecurityContext_p->convEncryptionAlog,&ret8,length);
			break;
		case CONVENCRYPTIONMD5://0xE03F:
			memset(SecurityContext_p->convEncryptionMD5,0x00,sizeof(SecurityContext_p->convEncryptionMD5));
			memcpy(SecurityContext_p->convEncryptionMD5,value,length);
			break;
		case CONVENCRYPTIONMSGBODY://0xE040:
			memset(SecurityContext_p->convEncryptionMsgBody,0x00,sizeof(SecurityContext_p->convEncryptionMsgBody));
			memcpy(SecurityContext_p->convEncryptionMsgBody,value,length);
			break;
		case CONVAUTHFAILREPORT://0xE041:
			ret8 = *value;
			memcpy(&SecurityContext_p->convAuthFailReport,&ret8,length);
			break;
		case CONVSINGLEENCRYPTBODY://0xE042:
			memset(SecurityContext_p->convSingleEncryptBody,0x00,sizeof(SecurityContext_p->convSingleEncryptBody));
			memcpy(SecurityContext_p->convSingleEncryptBody,value,length);
			break;
		case CONVMULTIENCRYPTBODY://0xE043:
			memset(SecurityContext_p->convMultiEncryptBody,0x00,sizeof(SecurityContext_p->convMultiEncryptBody));
			memcpy(SecurityContext_p->convMultiEncryptBody,value,length);
			break;
		case CONVPACKEDENCRYPTBODY://0xE044:
			memset(SecurityContext_p->convPackedEncryptBody,0x00,sizeof(SecurityContext_p->convPackedEncryptBody));
			memcpy(SecurityContext_p->convPackedEncryptBody,value,length);
			break;
			
		default:
			break;			
	}
//	memcpy(&wmmpSavedSecurityContext,SecurityContext_p,sizeof(WmmpSecurityContext_t));
}

static void WriteTLVCustom(u16 Tag_Id, u16 length, char* value)
{
	WmmpCustomContext_t  *CustomContext_p = &(ptrToWmmpContext()->CustomContext);
	u8		ret8=0;
	u16	ret16=0;
	u32	ret32=0;
	/*
	if((length == 0)||(value == NULL))
	{
		return;
	}	*/
	switch(Tag_Id)
	{
		case TLVSYNCTAGNUM://0x8001:
			//ret8 = *value;
			//memcpy(&CustomContext_p->TLVSyncTagNum,&ret8,length);
			break;	
		case MAXPDULENGHT://0x8002:
			ret16 = Readu16(value);
			memcpy(&CustomContext_p->MaxPDULength,&ret16,length);
			break;				
		case INITTERMINALNUM:
			memset(CustomContext_p->UnregTerminalNum,0x00,sizeof(CustomContext_p->UnregTerminalNum));
			memcpy(CustomContext_p->UnregTerminalNum,value,length);
			break;
		case RMCTRLTRACEENABLE:
			ret8 = *value;
			memcpy(&CustomContext_p->remoteCtrlTraceEnabel,&ret8,length);		
		default:
			break;			
	}
}

static void WriteTLVCustom2(u16 Tag_Id, u16 length, char* value)
{
	WmmpCustomContext_t  *CustomContext_p = &(ptrToWmmpContext()->CustomContext);
	WmmpCustom2Context_t  *Custom2Context_p = &(ptrToWmmpContext()->Custom2Context);
	u8		ret8=0;
	u16	ret16=0;
	u32	ret32=0;
   	u16   loopi = 0;
	/*
	if((length == 0)||(value == NULL))
	{
		return;
	}	*/
	switch(Tag_Id)
	{

		case APPSERVER0IPPORT:
		{
			memset(Custom2Context_p->AppServerString[0],0x00,MAX_SERVER_ADDR);
			memcpy(Custom2Context_p->AppServerString[0],value,length);
#if 0
			for(loopi = 0;loopi<length;loopi++)
			{
				WMMP_LIB_DBG_TEMP("WriteTLVCustom2 SERVER0 AppServerString[%d] 0x%x",loopi,*(value+loopi));
			}          
#endif
		break;	
		}
		case APPSERVER1IPPORT:
		{
			memset(Custom2Context_p->AppServerString[1],0x00,MAX_SERVER_ADDR);
			memcpy(Custom2Context_p->AppServerString[1],value,length);              
		break;	
		}
		case APPSERVER2IPPORT:
		{
			memset(Custom2Context_p->AppServerString[2],0x00,MAX_SERVER_ADDR);
			memcpy(Custom2Context_p->AppServerString[2],value,length);              
		break;	
		}
		case APPSERVER3IPPORT:
		{
			memset(Custom2Context_p->AppServerString[3],0x00,MAX_SERVER_ADDR);
			memcpy(Custom2Context_p->AppServerString[3],value,length);              
		break;	
		}
		case APPSERVER4IPPORT:
		{
			memset(Custom2Context_p->AppServerString[4],0x00,MAX_SERVER_ADDR);
			memcpy(Custom2Context_p->AppServerString[4],value,length);              
		break;	
		}
		case APPSERVER5IPPORT:
		{
			memset(Custom2Context_p->AppServerString[5],0x00,MAX_SERVER_ADDR);
			memcpy(Custom2Context_p->AppServerString[5],value,length);              
		break;	
		}
		case APPSERVER6IPPORT:
		{
			memset(Custom2Context_p->AppServerString[6],0x00,MAX_SERVER_ADDR);
			memcpy(Custom2Context_p->AppServerString[6],value,length);              
		break;	
		}
		case APPSERVER7IPPORT:
		{
			memset(Custom2Context_p->AppServerString[7],0x00,MAX_SERVER_ADDR);
			memcpy(Custom2Context_p->AppServerString[7],value,length);              
		break;	
		}
		default:
		break;			
	}
}

static void WriteTLVCustom3(u16 Tag_Id, u16 length, char* value)
{
	WmmpCustom3Context_t  *Custom3Context_p = &(ptrToWmmpContext()->Custom3Context);
	u8		ret8=0;
	u16	ret16=0;
	u32	ret32=0;
	/*
	if((length == 0)||(value == NULL))
	{
		return;
	}	*/
	switch(Tag_Id)
	{		
		case RMCTRLTRACEATNAME:	/*0x81f1*/
			memset(Custom3Context_p->remoteCtrlTraceATNamParm,0x00,sizeof(Custom3Context_p->remoteCtrlTraceATNamParm));
			memcpy(Custom3Context_p->remoteCtrlTraceATNamParm,value,length);			
			break;
		case RMCTRLTRACEATRET:	/*0x81f2*/
			//memset(Custom3Context_p->remoteCtrlTraceATRet,0x00,sizeof(Custom3Context_p->remoteCtrlTraceATRet));
			//memcpy(Custom3Context_p->remoteCtrlTraceATRet,value,length);				
			break;
		case RMCTRLTRACEATACK:	/*0x81f3*/
			memset(Custom3Context_p->remoteCtrlTraceATAck,0x00,sizeof(Custom3Context_p->remoteCtrlTraceATAck));
			memcpy(Custom3Context_p->remoteCtrlTraceATAck,value,length);			
			break;
		case RMCTRLTRACEATMSG:	/*0x81f4*/
			memset(Custom3Context_p->remoteCtrlTraceATMsg,0x00,sizeof(Custom3Context_p->remoteCtrlTraceATMsg));
			memcpy(Custom3Context_p->remoteCtrlTraceATMsg,value,length);			
			break;		
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_ADC_GPIO__
		case CUSTOMADC:	/*0x81f5*/	
			break;
		case CUSTOMGPIOINPUT:	/*0x81f6*/
			break;		
		case CUSTOMGPIOOUTPUT:	/*0x81f7*/
			//ret32 = Readu32(value);
			//memcpy(&Custom3Context_p->wmmp_gpio_status,&ret32,length);
			break;		
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/
		case RMCTRLTRACEATCFGLISTN: /*0x82f0*/
			memset(Custom3Context_p->rmCtrlTraceATNParmCfglist,0x00,sizeof(Custom3Context_p->rmCtrlTraceATNParmCfglist));
			memcpy(Custom3Context_p->rmCtrlTraceATNParmCfglist,value,length);			
			break;
		case RMCTRLTRACEATCFGN:
			memset(Custom3Context_p->rmCtrlTraceATNParmCfg,0x00,sizeof(Custom3Context_p->rmCtrlTraceATNParmCfg));
			memcpy(Custom3Context_p->rmCtrlTraceATNParmCfg,value,length);			
			break;
		case RMCTRLTRACEATM2MSTAN:
			memset(Custom3Context_p->rmCtrlTraceATNParmStat,0x00,sizeof(Custom3Context_p->rmCtrlTraceATNParmStat));
			memcpy(Custom3Context_p->rmCtrlTraceATNParmStat,value,length);			
			break;
		case RMCTRLTRACEATTRAPN:
			memset(Custom3Context_p->rmCtrlTraceATNParmTrap,0x00,sizeof(Custom3Context_p->rmCtrlTraceATNParmTrap));
			memcpy(Custom3Context_p->rmCtrlTraceATNParmTrap,value,length);			
			break;
		case RMCTRLTRACEATAPPOPENN:
			memset(Custom3Context_p->rmCtrlTraceATNParmAppopen,0x00,sizeof(Custom3Context_p->rmCtrlTraceATNParmAppopen));
			memcpy(Custom3Context_p->rmCtrlTraceATNParmAppopen,value,length);			
			break;
		case RMCTRLTRACEATAPPCLOSEN:	
			memset(Custom3Context_p->rmCtrlTraceATNParmAppclose,0x00,sizeof(Custom3Context_p->rmCtrlTraceATNParmAppclose));
			memcpy(Custom3Context_p->rmCtrlTraceATNParmAppclose,value,length);			
			break;
		case RMCTRLTRACEATBHVNAME:
			memset(Custom3Context_p->remoteCtrlTraceATNamParmBhv,0x00,sizeof(Custom3Context_p->remoteCtrlTraceATNamParmBhv));
			memcpy(Custom3Context_p->remoteCtrlTraceATNamParmBhv,value,length);			
			break;
		case RMCTRLTRACEATBROPENNAME:
			memset(Custom3Context_p->rmCtrlTraceATNamParmbrOpen,0x00,sizeof(Custom3Context_p->rmCtrlTraceATNamParmbrOpen));
			memcpy(Custom3Context_p->rmCtrlTraceATNamParmbrOpen,value,length);			
			break;
		case RMCTRLTRACEATBRSTAN:
			memset(Custom3Context_p->rmCtrlTraceATNamParmbrSta,0x00,sizeof(Custom3Context_p->rmCtrlTraceATNamParmbrSta));
			memcpy(Custom3Context_p->rmCtrlTraceATNamParmbrSta,value,length);			
			break;
		case RMCTRLTRACEATCGPADDRN:
			memset(Custom3Context_p->rmCtrlTraceATNamParmcgpAddr,0x00,sizeof(Custom3Context_p->rmCtrlTraceATNamParmcgpAddr));
			memcpy(Custom3Context_p->rmCtrlTraceATNamParmcgpAddr,value,length);			
			break;
		case RMCTRLTRACEATAPPCLOSENALREADY:
			memset(Custom3Context_p->rmCtrlTraceATNParmAppcloseAlready,0x00,sizeof(Custom3Context_p->rmCtrlTraceATNParmAppcloseAlready));
			memcpy(Custom3Context_p->rmCtrlTraceATNParmAppcloseAlready,value,length);			
			break;

		case RMCTRLTRACEATCFGLISTRET:	/* 0x83f0*/
			//memset(Custom3Context_p->remoteCtrlTraceATRetCfglist,0x00,sizeof(Custom3Context_p->remoteCtrlTraceATRetCfglist));
			//memcpy(Custom3Context_p->remoteCtrlTraceATRetCfglist,value,length);			
			break;
		case RMCTRLTRACEATCFGRET:
			//memset(Custom3Context_p->remoteCtrlTraceATRetCfg,0x00,sizeof(Custom3Context_p->remoteCtrlTraceATRetCfg));
			//memcpy(Custom3Context_p->remoteCtrlTraceATRetCfg,value,length);			
			break;
		case RMCTRLTRACEATM2MSTARET:
			//memset(Custom3Context_p->remoteCtrlTraceATRetSta,0x00,sizeof(Custom3Context_p->remoteCtrlTraceATRetSta));
			//memcpy(Custom3Context_p->remoteCtrlTraceATRetSta,value,length);			
			break;
		case RMCTRLTRACEATTRAPRET:
			//memset(Custom3Context_p->remoteCtrlTraceATRetTrap,0x00,sizeof(Custom3Context_p->remoteCtrlTraceATRetTrap));
			//memcpy(Custom3Context_p->remoteCtrlTraceATRetTrap,value,length);			
			break;
		case RMCTRLTRACEATAPPOPENRET:
			//memset(Custom3Context_p->remoteCtrlTraceATRetAppopen,0x00,sizeof(Custom3Context_p->remoteCtrlTraceATRetAppopen));
			//memcpy(Custom3Context_p->remoteCtrlTraceATRetAppopen,value,length);			
			break;
		case RMCTRLTRACEATAPPCLOSERET:
			//memset(Custom3Context_p->remoteCtrlTraceATRetAppclose,0x00,sizeof(Custom3Context_p->remoteCtrlTraceATRetAppclose));
			//memcpy(Custom3Context_p->remoteCtrlTraceATRetAppclose,value,length);			
			break;
		case RMCTRLTRACEATBHVRET:	
			//memset(Custom3Context_p->remoteCtrlTraceATRetBhv,0x00,sizeof(Custom3Context_p->remoteCtrlTraceATRetBhv));
			//memcpy(Custom3Context_p->remoteCtrlTraceATRetBhv,value,length);			
			break;
		case RMCTRLTRACEATBROPENRET:
			//memset(Custom3Context_p->rmCtrlTraceATRetbrOpen,0x00,sizeof(Custom3Context_p->rmCtrlTraceATRetbrOpen));
			//memcpy(Custom3Context_p->rmCtrlTraceATRetbrOpen,value,length);			
			break;
		case RMCTRLTRACEATBRSTARET:
			//memset(Custom3Context_p->rmCtrlTraceATRetbrSta,0x00,sizeof(Custom3Context_p->rmCtrlTraceATRetbrSta));
			//memcpy(Custom3Context_p->rmCtrlTraceATRetbrSta,value,length);			
			break;
		case RMCTRLTRACEATCGPADDRRET:
			//memset(Custom3Context_p->rmCtrlTraceATRetcgpAddr,0x00,sizeof(Custom3Context_p->rmCtrlTraceATRetcgpAddr));
			//memcpy(Custom3Context_p->rmCtrlTraceATRetcgpAddr,value,length);			
			break;

		case RMCTRLTRACEATACKAPPOPEN: /* 0x84f0*/
			memset(Custom3Context_p->rmCtrlTraceATAckAppopen,0x00,sizeof(Custom3Context_p->rmCtrlTraceATAckAppopen));
			memcpy(Custom3Context_p->rmCtrlTraceATAckAppopen,value,length);			
			break;
		case RMCTRLTRACEATACKACK:
			memset(Custom3Context_p->remoteCtrlTraceATAckack,0x00,sizeof(Custom3Context_p->remoteCtrlTraceATAckack));
			memcpy(Custom3Context_p->remoteCtrlTraceATAckack,value,length);			
			break;
		case RMCTRLTRACEATMSGRECVDATA:  /*0x85f0*/
			memset(Custom3Context_p->remoteCtrlTraceATRecvData,0x00,sizeof(Custom3Context_p->remoteCtrlTraceATRecvData));
			memcpy(Custom3Context_p->remoteCtrlTraceATRecvData,value,length);			
			break;
		case RMCTRLTRACEATMSGSOCDESTR:
			memset(Custom3Context_p->remoteCtrlTraceATSocDestroy,0x00,sizeof(Custom3Context_p->remoteCtrlTraceATSocDestroy));
			memcpy(Custom3Context_p->remoteCtrlTraceATSocDestroy,value,length);			
			break;
		case RMCTRLTRACEATMSGCLOSEDIND:
			memset(Custom3Context_p->remoteCtrlTraceATMsgClosedInd,0x00,sizeof(Custom3Context_p->remoteCtrlTraceATMsgClosedInd));
			memcpy(Custom3Context_p->remoteCtrlTraceATMsgClosedInd,value,length);			
			break;
		case RMCTRLTRACEATMSGCLOSEDINDTOUT:
			memset(Custom3Context_p->rmtTraceATMsgClosedIndTimeOut,0x00,sizeof(Custom3Context_p->rmtTraceATMsgClosedIndTimeOut));
			memcpy(Custom3Context_p->rmtTraceATMsgClosedIndTimeOut,value,length);			
			break;
		default:
			break;			
	}
}



/******************************************************************************
* Function:   WriteTLV    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
void WriteTLV(u16 Tag_Id, u16 length, char* value) 
{
	u16	loopi = 0;
/*
	if((length == 0)||(value == PNULL)	)
	{
		return;
	}	
	if(value == NULL)
	{
		return;
	}	*/
	//MCOM_WMMP_LIB_DBG_TEMP("WriteTLV write TLV %x %d\n",Tag_Id,length);
	WMMP_LIB_DBG_TEMP("WriteTLV Tag_Id 0x%04x",Tag_Id);
	WMMP_LIB_DBG_TEMP("WriteTLV length %d",length);
#if 0
	for(loopi = 0;loopi<length;loopi++)
	{
		WMMP_LIB_DBG_TEMP("WriteTLV value[%d] 0x%02x",loopi,value[loopi]);
	}
#endif

	if((Tag_Id>=GPRSDAILNUM)&&(Tag_Id<=GATELIMIT))
	{
		WriteTLVcfg(Tag_Id, length, value);
	}
	else if((Tag_Id>=UPDATEMODE)&&(Tag_Id<=CURRENTFILEREV))
	{
		WriteTLVUpdate(Tag_Id, length, value);
	}
	else if((Tag_Id>=STATINFOREPORT)&&(Tag_Id<=OPERATIONMUTUALUSSDSTAT))
	{
		WriteTLVStatistic(Tag_Id, length, value);
	}
	else if((Tag_Id>=PROTOCOLREV)&&(Tag_Id<=FAILWARNING))
	{
		WriteTLVStatus(Tag_Id, length, value);
	}
	else if((Tag_Id>=SETCOMMAND)&&(Tag_Id<=EXTERNALDEVCTRL))
	{
		WriteTLVControl(Tag_Id, length, value);
	}
	else if((Tag_Id>=SIMPIN1)&&(Tag_Id<=CONVPACKEDENCRYPTBODY))
	{
		//MCOM_WMMP_LIB_DBG_TEMP("WriteTLV 1 TLV %x %d\n",Tag_Id,length);
		WriteTLVSecurity(Tag_Id, length, value);
	}
	else if(((Tag_Id>=TLVSYNCTAGNUM)&&(Tag_Id<=INITTERMINALNUM)) || (Tag_Id == RMCTRLTRACEENABLE))
	{
		WriteTLVCustom(Tag_Id, length, value);
	}
	else if((Tag_Id >= RMCTRLTRACEATNAME) && (Tag_Id < RMCTRLTRACEATMAX))
	{
		WriteTLVCustom3(Tag_Id, length, value);
	}
   	else if((Tag_Id>=APPSERVER0IPPORT)&&(Tag_Id<=APPSERVER7IPPORT))
	{
		WriteTLVCustom2(Tag_Id, length, value);       
	}
}

/******************************************************************************
* Function:   CheckTLVcfg    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
static bool CheckTLVcfg(u16 Tag_Id, u16 length, char* value)
{
	WmmpCfgContext_t  *CfgContext_p = &(ptrToWmmpContext()->ConfigContext);
	bool ret = false;

	
	switch(Tag_Id)
	{
		case GPRSDAILNUM://0x0001:
			WMMP_LIB_DBG_TEMP("CheckTLVcfg sizeof GPRSDailNum %d",sizeof(CfgContext_p->GPRSDailNum));
			if(length > sizeof(CfgContext_p->GPRSDailNum))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}
			break;
		case M2MAPN://0x0002:
			WMMP_LIB_DBG_TEMP("CheckTLVcfg sizeof M2MAPN %d",sizeof(CfgContext_p->M2MAPN));
			if(length > sizeof(CfgContext_p->M2MAPN))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case GPRSUSERID://0x0003:
			WMMP_LIB_DBG_TEMP("CheckTLVcfg sizeof GPRSUserId %d",sizeof(CfgContext_p->GPRSUserId));
			if(length > sizeof(CfgContext_p->GPRSUserId))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case GPRSPASSWORD://0x0004:
			WMMP_LIB_DBG_TEMP("CheckTLVcfg sizeof GPRSPassword %d",sizeof(CfgContext_p->GPRSPassword));
			if(length > sizeof(CfgContext_p->GPRSPassword))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SMSCENTRENUM://0x0005:
			if(length > sizeof(CfgContext_p->SMSCentreNum))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case APPSRVTRANSKEY://0x0006:
			if(length > sizeof(CfgContext_p->AppSrvTransKey))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case M2MSMSPECIALNUM://0x0007:
			if(length > sizeof(CfgContext_p->M2MSMSpecialNum))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case M2MURLADDR://0x0008:
			if(length > sizeof(CfgContext_p->M2MURLAddr))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case M2MIPADDR://0x0009:
			if(length > sizeof(CfgContext_p->M2MIPAddr))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case M2MPORT://0x000A:
			if(length > sizeof(CfgContext_p->M2MPort))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case APPSMSCENTRENUM://0x000B:
			if(length > sizeof(CfgContext_p->AppSMSCentreNum))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}		
			break;
		case APPSRVURLADDR://0x000C:
			if(length > sizeof(CfgContext_p->AppSrvURLAddr))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}		
			break;
		case APPSRVIPADDR://0x000D:
			if(length > sizeof(CfgContext_p->AppSrvIPAddr))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case APPSRVPORT://0x000E:
			if(length > sizeof(CfgContext_p->AppSrvPort))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
			
		case reserve1://0x000F:
			//reserve
			ret = false;
			break;	

		case DNSADDR://0x0010:
			if(length > sizeof(CfgContext_p->DNSAddr))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case PERIOD_CFG://0x0011:
 			if(length > sizeof(CfgContext_p->Period))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case APPUSSDSMSSPECIALNUM://0x0012:
			if(length > sizeof(CfgContext_p->AppUSSDSMSSpecialNum))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}
			break;
		case M2MAPPAPN://0x0013:
 			if(length > sizeof(CfgContext_p->M2MAppAPN))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case BACKUPDNSADDR://0x0014:
 			if(length > sizeof(CfgContext_p->BackupDNSAddr))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;

		case TLVSYNCTAGVALUE://0x0025:
 			if(length > sizeof(CfgContext_p->TLVSyncTagValue))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SMSRECVOVERTIME://0x0026:
 			if(length != 2)// > sizeof(CfgContext_p->SMSRecvOvertime))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case MAXLOGINRETRYTIMES://0x0027:
 			if(length > sizeof(CfgContext_p->MaxLoginRetryTimes))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case DEFAULTECCODE://0x0028:
 			if(length > sizeof(CfgContext_p->DefaultECCode))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;

		case LOCALPTRVALUE://0x0101:
 			if(length > sizeof(CfgContext_p->LocalPTRValue))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case LOCALPORTMODE://0x0102:
 			if(length > sizeof(CfgContext_p->LocalPortMode))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case LOCALPORTHALT://0x0103:
 			if(length > sizeof(CfgContext_p->LocalPorthalt))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case LOCALPORTVERIFY://0x0104:
 			if(length > sizeof(CfgContext_p->LocalPortVerify))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case LOCALPORTSCANPERIOD://0x0105:
 			if(length > sizeof(CfgContext_p->LocalPortScanPeriod))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
			
		case reserve2://0x0106:
			//reserve
			ret = false;
			break;
			
		case DATAPACKRESPOVERTIME://0x0107:
 			if(length > sizeof(CfgContext_p->DataPackRespOvertime))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case TRANSFAILRETRYTIMES://0x0108:
 			if(length > sizeof(CfgContext_p->TransFailRetryTimes))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case GATELIMIT://0x0109:
 			if(length > sizeof(CfgContext_p->GateLimit))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;	
			
		default:
			ret = false;

	}
	return ret;
}


/******************************************************************************
* Function:   Checkpdate    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
static bool CheckTLVUpdate(u16 Tag_Id, u16 length, char* value)
{
	WmmpUpdateContext_t  *UpdateContext_p = &(ptrToWmmpContext()->UpdateContext);
	bool ret = false;
	/*if((length == 0)||(value == NULL)	)
	{
		return false;
	}*/	
	switch(Tag_Id)
	{
		case UPDATEMODE://0x1000:
 			if(length > sizeof(UpdateContext_p->UpdateMode))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case UPDATESRVURLADDR://0x1001:
 			if(length > sizeof(UpdateContext_p->UpdateSrvURLAddr))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case UPDATESRVIPADDR://0x1002:
 			if(length > sizeof(UpdateContext_p->UpdateSrvIPAddr))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case UPDATESRVPORT://0x1003:
 			if(length > sizeof(UpdateContext_p->UpdateSrvPort))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case UPDATESRVLOGINPASSWORD://0x1004:
 			if(length > sizeof(UpdateContext_p->UpdateSrvLoginPassword))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case UPDATEFILEREV://0x1005:
 			if(length > sizeof(UpdateContext_p->UpdateFileRev))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case UPDATEFILELENGTH://0x1006:
 			if(length > sizeof(UpdateContext_p->UpdateFileLength))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case CRC16CHECK://0x1007:
 			if(length > sizeof(UpdateContext_p->CRC16Check))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case CRC32CHECK://0x1008:
 			if(length > sizeof(UpdateContext_p->CRC32Check))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case UPDATEAFFAIRTRANSID://0x1009:
 			if(length > sizeof(UpdateContext_p->UpdateAffairTransId))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case FILERELEASEDATA://0x100A:
 			if(length > sizeof(UpdateContext_p->FileReleaseData))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case FORCETOUPDATEPARAM://0x100B:
 			if(length > sizeof(UpdateContext_p->ForcetoUpdateParam))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;	
		case CURRENTFILEREV://0x100C
			if(length > sizeof(UpdateContext_p->CurrentFileRev))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}							
			break;
		default:
			ret = false;
		
	}
	return ret;
}

/******************************************************************************
* Function:   CheckTLVStatistic    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
static bool CheckTLVStatistic(u16 Tag_Id, u16 length, char* value)
{
	WmmpStatisticContext_t  *StatisticContext_p = &(ptrToWmmpContext()->StatisticContext);
	bool ret = false;
	/*if((length == 0)||(value == NULL)	)
	{
		return false;
	}*/
	switch(Tag_Id)
	{
		case STATINFOREPORT://0x2001:
 			if(length > sizeof(StatisticContext_p->StatInfoReport))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case REPORTTIME://0x2002:
 			if(length > sizeof(StatisticContext_p->ReportTime))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case PERIODREPORT://0x2003:
 			if(length > sizeof(StatisticContext_p->PeriodReport))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case MONTHLYOPERATIONSIGN://0x2004:
 			if(length > sizeof(StatisticContext_p->MonthlyOperationSign))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case CLEARSTATDATA://0x2005:
 			if(length > sizeof(StatisticContext_p->ClearStatData))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case STATSTARTTIME://0x2006:
 			if(length > sizeof(StatisticContext_p->StatStartTime))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case STATFINISHTIME://0x2007:
 			if(length > sizeof(StatisticContext_p->statFinishTime))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case SMSDELIVERYSUCC://0x2008:
 			if(length > sizeof(StatisticContext_p->SMSDeliverySucc))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case SMSDELIVERUFAIL://0x2009:
 			if(length > sizeof(StatisticContext_p->SMSDeliveruFail))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case SMSRECEIVEDNUM://0x200A:
 			if(length > sizeof(StatisticContext_p->SMSReceivedNum))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case GRPSDATASIZE://0x200B:
 			if(length > sizeof(StatisticContext_p->GRPSDataSize))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;	
		case USSDSUCCNUM://0x200C:
 			if(length > sizeof(StatisticContext_p->USSDSuccNum))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;	
		case USSDFAILNUM://0x200D:
 			if(length > sizeof(StatisticContext_p->USSDFailNum))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;	
		case USSDRECEIVEDNUM://0x200E:
 			if(length > sizeof(StatisticContext_p->USSDReceivedNum))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;	
			
		case SMSSTATPARAM://0x2011:
 			if(length > sizeof(StatisticContext_p->SMSStatParam))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;	
		case SMSSTATAUTOREPORTPARAM://0x2012:
 			if(length > sizeof(StatisticContext_p->SMSStatAutoReportParam))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;	
		case M2MMUTUALSMSSTAT://0x2013:
 			if(length > sizeof(StatisticContext_p->M2MMutualSMSStat))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;	
		case OPERATIONMUTUALSMSSTAT://0x2014:
 			if(length > sizeof(StatisticContext_p->OperationMutualSMSStat))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;	
		case GPRSOPERATIONSTATPARAM://0x2015:
 			if(length > sizeof(StatisticContext_p->GPRSOperationStatParam))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;	
		case GPRSOPERATIONSTATREPORTPARAM://0x2016:
 			if(length > sizeof(StatisticContext_p->GPRSOperationStatReportParam))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;	
		case M2MMUTUALGPRSDATASIZE://0x2017:
 			if(length > sizeof(StatisticContext_p->M2MMutualGPRSDataSize))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case OPERATIONMUTUALGPRSDATASIZE://0x2018:
 			if(length > sizeof(StatisticContext_p->OperationMutualGPRSDataSize))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;	
		case USSDSTATPARAM://0x2019:
 			if(length > sizeof(StatisticContext_p->USSDStatParam))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case USSDSTATREPORTPARAM://0x201A:
 			if(length > sizeof(StatisticContext_p->USSDStatReportParam))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case M2MMUTUALUSSDSTAT://0x201B:
 			if(length > sizeof(StatisticContext_p->M2MMutualUSSDStat))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;
		case OPERATIONMUTUALUSSDSTAT://0x201C:
 			if(length > sizeof(StatisticContext_p->OperationMutualUSSDStat))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}				
			break;	
			
		default:
			ret = false;			
	}
	return ret;
}	

/******************************************************************************
* Function:   CheckTLVStatus    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
static bool CheckTLVStatus(u16 Tag_Id, u16 length, char* value)
{
	WmmpTermStatusContext_t  *StatusContext_p = &(ptrToWmmpContext()->StatusContext);
	bool ret = false;
	/*if((length == 0)||(value == NULL)	)
	{
		return false;
	}*/	
	switch(Tag_Id)
	{
		case PROTOCOLREV://0x3001:

				ret = false;
		
			break;
		case TERMINALSERIESNUM://0x3002:

				ret = false;
		
			break;
		case IMSINUM://0x3003:
 			if(length > sizeof(StatusContext_p->IMSINum))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case FIRMWAREREV://0x3004:
 			if(length > sizeof(StatusContext_p->FirmwareRev))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case PREVPERIOD://0x3005:
 			if(length > sizeof(StatusContext_p->PrevPeriod))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case CELLULARID://0x3006:
 			if(length > sizeof(StatusContext_p->CellularId))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SIGNALLEVEL://0x3007:
 			if(length > sizeof(StatusContext_p->SignalLevel))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case INITPARAFLAG://0x3008:
 			if(length > sizeof(StatusContext_p->InitParaFlag))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SYSTEMTIME://0x3009:
 			if(length > sizeof(StatusContext_p->SystemTime))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
			
		case reserve3://0x300A:
			//reserve		
			ret = false;
			break;
			
		case ALARMSTATUS://0x300B:
 			if(length > sizeof(StatusContext_p->AlarmStatus))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case ALARMTYPE://0x300C:
 			if(length > sizeof(StatusContext_p->AlarmType))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case ALARMCODE://0x300D:
			WMMP_LIB_DBG_TEMP("CheckTLVStatus 0x300D sizeof(StatusContext_p->AlarmCode) %d",sizeof(StatusContext_p->AlarmCode));
 			if(length > sizeof(StatusContext_p->AlarmCode))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case RESTOREALARM://0x300E:
 			if(length > sizeof(StatusContext_p->RestoreAlarm))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;	
		/*bob modify 20101127*/		//case DEVSTATUS://0x300F: 
		case M2MDEVSTATUS:	
 			if(length > sizeof(StatusContext_p->DEVStatus))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SUPPORTEDDEV://0x3010:
 			if(length > sizeof(StatusContext_p->SupportedDev))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case PACKLOSTNUM://0x3011:
 			if(length > sizeof(StatusContext_p->PackLostNum))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case FAILWARNING://0x3012:
 			if(length > sizeof(StatusContext_p->FailWarning))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
			
		default:
			ret = false;		
	}
	return ret;
}

/******************************************************************************
* Function:   CheckTLVControl    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
static bool CheckTLVControl(u16 Tag_Id, u16 length, char* value)
{
	WmmpControlContext_t  *ControlContext_p = &(ptrToWmmpContext()->ControlContext);
	bool ret = false;
	/*if((length == 0)||(value == NULL)	)
	{
		return false;
	}*/	
	switch(Tag_Id)
	{
		case SETCOMMAND://0x4001:
 			if(length > sizeof(ControlContext_p->SetCommand))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case CONNECTMODE://0x4002:
 			if(length > sizeof(ControlContext_p->ConnectMode))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case DATATRANSMODE://0x4003:
 			if(length > sizeof(ControlContext_p->DataTransMode))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case PERIOD_CTRL://0x4004:
 			if(length > sizeof(ControlContext_p->Period))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case COLLECTDATATRANSMODE://0x4005:
 			if(length > sizeof(ControlContext_p->CollectDataTransMode))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case M2MCONNECTMODEONIP://0x4006:
 			if(length > sizeof(ControlContext_p->M2MConnectModeOnIP))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case USERDATAID://0x4007:
 			if(length > sizeof(ControlContext_p->UserDataID))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SUBPACKMODEPARAM://0x4008:
 			if(length > sizeof(ControlContext_p->SubPackModeParam))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case PLATFORMPARAMOPT://0x4009:
 			if(length > sizeof(ControlContext_p->PlatformParamOpt))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
			
		case EXECUTEPARAMOPT://0x4010:
 			if(length > sizeof(ControlContext_p->ExecuteParamOpt))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;

		case lengthblank://0x4011:
			//length=0
			ret = false;
			break;
			
		case DESSERIESNUM://0x4012:
 			if(length > sizeof(ControlContext_p->DesSeriesNum))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SRCSERIESNUM://0x4013:
 			if(length > sizeof(ControlContext_p->SrcSeriesNum))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case DESECCODE://0x4014:
 			if(length > sizeof(ControlContext_p->DesECCode))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SRCECCODE://0x4015:
 			if(length > sizeof(ControlContext_p->SrcECCode))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case TRANSCTRLCOMMANDSTATICPARAM://0x4016:
 			if(length > sizeof(ControlContext_p->TransCtrlCommandStaticParam))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;	
		case VERIFYTLVBYCRC32://0x4017:
 			if(length > sizeof(ControlContext_p->VerifyTLVByCRC32))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case EXTERNALDEVCTRL://0x4021:
 			if(length > sizeof(ControlContext_p->ExternalDevCtrl))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
			
		default:
			ret = false;		
	}
	return ret;
}

/******************************************************************************
* Function:   CheckTLVSecurity    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
static bool CheckTLVSecurity(u16 Tag_Id, u16 length, char* value)
{
	WmmpSecurityContext_t  *SecurityContext_p = &(ptrToWmmpContext()->SecurityContext);
	bool ret = false;
	/*if((length == 0)||(value == NULL)	)
	{
		return false;
	}*/
	switch(Tag_Id)
	{
		case SIMPIN1://0xE001:
 			if(length > sizeof(SecurityContext_p->SIMPin1))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SIMPIN1MD5://0xE002:
 			if(length > sizeof(SecurityContext_p->SIMPin1MD5))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SIMPIN1ACTIVE://0xE003:
 			if(length > sizeof(SecurityContext_p->SIMPin1Active))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SIMPIN2://0xE004:
 			if(length > sizeof(SecurityContext_p->SIMPin2))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SIMPIN2MD5://0xE005:
 			if(length > sizeof(SecurityContext_p->SIMPin2MD5))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SIMPIN2ACTIVE://0xE006:
 			if(length > sizeof(SecurityContext_p->SIMPin2Active))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SIMPUK1://0xE007:
 			if(length > sizeof(SecurityContext_p->SIMPuk1))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SIMPUK1MD5://0xE008:
 			if(length > sizeof(SecurityContext_p->SIMPuk1MD5))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SIMPUK1ACTIVE://0xE009:
 			if(length > sizeof(SecurityContext_p->SIMPuk1Active))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SIMPUK2://0xE00A:
 			if(length > sizeof(SecurityContext_p->SIMPuk2))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SIMPUK2MD5://0xE00B:
 			if(length > sizeof(SecurityContext_p->SIMPuk2MD5))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case SIMPUK2ACTIVE://0xE00C:
 			if(length > sizeof(SecurityContext_p->SIMPuk2Active))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case BINDPARAACTIVE://0xE00D:
 			if(length > sizeof(SecurityContext_p->BindParaActive))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case BINDIMEI://0xE00E:
			//already get it
			ret = false;
			break;	
		case BINDIMEIMD5://0xE00F:

				ret = false;
		
			break;			
		case BINDIMEIORIG://0xE010:

				ret = false;
	
			break;
		case BINDIMEIORIGMD5://0xE011:

				ret = false;
	
			break;
		case BINDIMSI://0xE012:
			//already get it
			ret = false;
			break;	
		case BINDIMSIMD5://0xE013:
 			if(length > sizeof(SecurityContext_p->BindIMSIMD5))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;	
		case BINDIMSIORIG://0xE014:
 			if(length > sizeof(SecurityContext_p->BindIMSIOrig))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case BINDIMSIORIGMD5://0xE015:
 			if(length > sizeof(SecurityContext_p->BindIMSIOrigMD5))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case BINDCURRENTMD5://0xE016:
 			if(length > sizeof(SecurityContext_p->BindCurrentMD5))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case BINDORIGINALMD5://0xE017:
 			if(length > sizeof(SecurityContext_p->BindOriginalMD5))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case PIN1ERRORALLOW://0xE018:
 			if(length > sizeof(SecurityContext_p->Pin1ErrorAllow))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case PIN2ERRORALLOW://0xE019:
 			if(length > sizeof(SecurityContext_p->Pin2ErrorAllow))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case PUK1ERRORALLOW://0xE01A:
 			if(length > sizeof(SecurityContext_p->Puk1ErrorAllow))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case PUK2ERRORALLOW://0xE01B:
 			if(length > sizeof(SecurityContext_p->Puk2ErrorAllow))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case M2MPASSCLEARPARA://0xE020:
 			if(length > sizeof(SecurityContext_p->M2MPassClearPara))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case INITPARA://0xE021:
 			if(length > sizeof(SecurityContext_p->InitPara))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case INITPARAUSED://0xE022:
 			if(length > sizeof(SecurityContext_p->InitParaUsed))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case INITCOMMAND://0xE023:
 			if(length > sizeof(SecurityContext_p->InitCommand))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;			
		case INITTLVACTTAG://0xE024:
 			if(length > sizeof(SecurityContext_p->InitTLVactTag))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case UPLINKPASSWORD://0xE025:
 			if(length > sizeof(SecurityContext_p->uplinkPassword))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case UPLINKPASSWORDMD5://0xE026:
 			if(length > sizeof(SecurityContext_p->uplinkPasswordMD5))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case UPLINKPASSWORDEXPDATE://0xE027:
 			if(length > sizeof(SecurityContext_p->uplinkPasswordExpDate))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case DOWNLINKPASSWORD://0xE028:
 			if(length > sizeof(SecurityContext_p->downlinkPassword))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case DOWNLINKPASSWORDMD5://0xE029:
 			if(length > sizeof(SecurityContext_p->downlinkPasswordMD5))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case DOWNLINKPASSWORDEXPDATE://0xE02A:
 			if(length > sizeof(SecurityContext_p->downlinkPasswordExpDate))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
			
		case ENCRYPTIONSUPPORT://0xE031:
 			if(length > sizeof(SecurityContext_p->encryptionSupport))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case ENCRYPTIONUSED://0xE032:
 			if(length > sizeof(SecurityContext_p->encryptionUsed))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case ENCRYPTIONCOMMAND://0xE033:
 			if(length > sizeof(SecurityContext_p->encryptionCommand))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;	
		case ENCRTYPTIONTAG://0xE034:
 			if(length > sizeof(SecurityContext_p->encrtyptionTag))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case ENCRYPTIONALGORITHM://0xE035:
 			if(length > sizeof(SecurityContext_p->encryptionAlgorithm))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case PUBLICKEY://0xE036:
 			if(length > sizeof(SecurityContext_p->publickey))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case PRIVATEKEY://0xE037:
 			if(length > sizeof(SecurityContext_p->privatekey))
			{
				ret = false;
			}
			else
			{
				return true;
			}			
			break;
		case PUBLICKEYEXPDATE://0xE038:
 			if(length > sizeof(SecurityContext_p->publicKeyExpDate))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case ENCRYPTIONKEYALGORITHM://0xE039:
 			if(length > sizeof(SecurityContext_p->encryptionKeyAlgorithm))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
			
		case ENCRYPTIONKEYMD5://0xE03A:
 			if(length > sizeof(SecurityContext_p->encryptionKeyMD5))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case CONVPUBLICKEY://0xE03B:
 			if(length > sizeof(SecurityContext_p->convPublicKey))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case CONVPRIVATEKEY://0xE03C:
 			if(length > sizeof(SecurityContext_p->convPrivateKey))
			{
				ret = false;
			}
			else
			{
				return true;
			}			
			break;
		case CONVKEYEXPDATE://0xE03D:
 			if(length > sizeof(SecurityContext_p->convKeyExpDate))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;	
		case CONVENCRYPTIONALOG://0xE03E:
 			if(length > sizeof(SecurityContext_p->convEncryptionAlog))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case CONVENCRYPTIONMD5://0xE03F:
 			if(length > sizeof(SecurityContext_p->convEncryptionMD5))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case CONVENCRYPTIONMSGBODY://0xE040:
 			if(length > sizeof(SecurityContext_p->convEncryptionMsgBody))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case CONVAUTHFAILREPORT://0xE041:
 			if(length > sizeof(SecurityContext_p->convAuthFailReport))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case CONVSINGLEENCRYPTBODY://0xE042:
 			if(length > sizeof(SecurityContext_p->convSingleEncryptBody))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case CONVMULTIENCRYPTBODY://0xE043:
 			if(length > sizeof(SecurityContext_p->convMultiEncryptBody))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case CONVPACKEDENCRYPTBODY://0xE044:
 			if(length > sizeof(SecurityContext_p->convPackedEncryptBody))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
			
		default:
			ret = false;		
	}
	return ret;
}

static bool CheckTLVCustom2(u16 Tag_Id, u16 length, char* value)
{
	WmmpCustomContext_t  *CustomContext_p = &(ptrToWmmpContext()->CustomContext);
	bool ret = false;
   u8   appserCount = 0;
	/*if((length == 0)||(value == NULL)	)
	{
		return false;
	}*/
	switch(Tag_Id)
	{
      case APPSERVER0IPPORT:
      case APPSERVER1IPPORT:
      case APPSERVER2IPPORT:
      case APPSERVER3IPPORT:
      case APPSERVER4IPPORT:
      case APPSERVER5IPPORT:
      case APPSERVER6IPPORT:
      case APPSERVER7IPPORT:
      {
          appserCount = Tag_Id - APPSERVER0IPPORT;
          WMMP_LIB_DBG_TEMP("CheckTLVCustom2 appserCount %d ",appserCount);
          if(length > MAX_SERVER_ADDR)
			{
				ret = false;
			}
			else
			{
				ret = true;
			}
       }
		break;	
       
		default:
			ret = false;		
	}
	return ret;
}


/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_ADC_GPIO__
static bool CheckTLVCustom3(u16 Tag_Id, u16 length, char* value)
{
	WmmpCustom3Context_t  *Custom3Context_p = &(ptrToWmmpContext()->Custom3Context);
	bool ret = false;
	switch(Tag_Id)
	{
		case CUSTOMADC:
			if(length > sizeof(Custom3Context_p->wmmp_adc_value))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case CUSTOMGPIOINPUT:
			if(length > sizeof(Custom3Context_p->wmmp_gpio_input))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;
		case CUSTOMGPIOOUTPUT:
			if(length > sizeof(Custom3Context_p->wmmp_gpio_status))
			{
				ret = false;
			}
			else
			{
				ret = true;
			}			
			break;

		default:
			ret = false;		
	}
	return ret;
}
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/

/******************************************************************************
* Function:   CheckTLV    
* 
* Author:     wangzhengning
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
bool CheckTLV(u16 Tag_Id, u16 length, char* value)
{
	u16 	loopi = 0;

	WMMP_LIB_DBG_TEMP("CheckTLV Tag_Id 0x%04x",Tag_Id);
	WMMP_LIB_DBG_TEMP("CheckTLV length %d",length);
	
	if((Tag_Id>=GPRSDAILNUM)&&(Tag_Id<=GATELIMIT))
	{
		return CheckTLVcfg(Tag_Id, length, value);
	}
	else if((Tag_Id>=UPDATEMODE)&&(Tag_Id<=CURRENTFILEREV))
	{
		return CheckTLVUpdate(Tag_Id, length, value);
	}
	else if((Tag_Id>=STATINFOREPORT)&&(Tag_Id<=OPERATIONMUTUALUSSDSTAT))
	{
		return CheckTLVStatistic(Tag_Id, length, value);
	}
	else if((Tag_Id>=PROTOCOLREV)&&(Tag_Id<=FAILWARNING))
	{
		return CheckTLVStatus(Tag_Id, length, value);
	}
	else if((Tag_Id>=SETCOMMAND)&&(Tag_Id<=EXTERNALDEVCTRL))
	{
		return CheckTLVControl(Tag_Id, length, value);
	}
	else if((Tag_Id>=SIMPIN1)&&(Tag_Id<=CONVPACKEDENCRYPTBODY))
	{
		return CheckTLVSecurity(Tag_Id, length, value);
	}
   	else if((Tag_Id>=APPSERVER0IPPORT)&&(Tag_Id<=APPSERVER7IPPORT))
	{
		return CheckTLVCustom2(Tag_Id, length, value);
	}
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_ADC_GPIO__
   	else if((Tag_Id>=CUSTOMADC)&&(Tag_Id<=CUSTOMGPIOOUTPUT))
	{
		return CheckTLVCustom3(Tag_Id, length, value);
	}
#endif	
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/
	else if(Tag_Id==0)//加密报文时，最后会产生多余的00 字段
	{
		WMMP_LIB_DBG_TEMP("CheckTLV ret true");
		return true;
	}else
	{
		WMMP_LIB_DBG_TEMP("CheckTLV ret false");
		return false;
}
}

bool CheckTLVTag(u16 Tag_Id)
{
	if(((Tag_Id>=GPRSDAILNUM)&&(Tag_Id<=GATELIMIT))||
	((Tag_Id>=UPDATEMODE)&&(Tag_Id<=FORCETOUPDATEPARAM))||
	((Tag_Id>=STATINFOREPORT)&&(Tag_Id<=OPERATIONMUTUALUSSDSTAT))||
	((Tag_Id>=PROTOCOLREV)&&(Tag_Id<=FAILWARNING))||
	((Tag_Id>=SETCOMMAND)&&(Tag_Id<=EXTERNALDEVCTRL))||
	((Tag_Id>=SIMPIN1)&&(Tag_Id<=CONVPACKEDENCRYPTBODY))||
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_ADC_GPIO__
	((Tag_Id>=CUSTOMADC)&&(Tag_Id<=CUSTOMGPIOOUTPUT))||
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/
	(Tag_Id==0))
	{
		return true;
	}
	else
	{
		return false;
	}
}

void WriteAllTLVParam2Flash(void)
{
	wmmpProcessSave(NVRAM_EF_WMMP_TLV_CFG_LID,(void *)WPA_ptrToConfigTLV(),sizeof(WmmpCfgContext_t));
	wmmpProcessSave(NVRAM_EF_WMMP_TLV_UPDATE_LID,(void *)WPA_ptrToUpdateTLV(),sizeof(WmmpUpdateContext_t));
	wmmpProcessSave(NVRAM_EF_WMMP_TLV_STAT_LID,(void *)WPA_ptrToStatisticTLV(),sizeof(WmmpStatisticContext_t));
	wmmpProcessSave(NVRAM_EF_WMMP_TLV_STATUS_LID,(void *)WPA_ptrToStatusTLV(),sizeof(WmmpTermStatusContext_t));
	wmmpProcessSave(NVRAM_EF_WMMP_TLV_CTRL_LID,(void *)WPA_ptrToControlTLV(),sizeof(WmmpControlContext_t));
	wmmpProcessSave(NVRAM_EF_WMMP_TLV_SECURITY_LID,(void *)WPA_ptrToSecurityTLV(),sizeof(WmmpSecurityContext_t));
	wmmpProcessSave(NVRAM_EF_WMMP_TLV_CUSTOM_LID,(void *)WPA_ptrToCustomTLV(),sizeof(WmmpCustomContext_t));
	wmmpProcessSave(NVRAM_EF_WMMP_TLV_CUSTOM2_LID,(void *)WPA_ptrToCustom2TLV(),sizeof(WmmpCustom2Context_t));
}

#endif
/***********End of File******************************************/
