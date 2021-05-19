#if defined(__MODEM_WMMP30_SIMCOM__)
/*****************************************************************
* Include Files
*****************************************************************/
#include <stdio.h>
#include <string.h>

#include <stdarg.h>

#include "wmmp_typ.h"
#include "wmmp_api.h"
#include "wmmp_tlv.h"
#include "wmmp_lib.h"
#include "wmmp_interface.h"
#include "wmmp_encode.h"
#include "eat_nvram.h"

#include "simcom_wmmp_utility.h"

WmmpDecodeRecvResult_e  WPA_Decode(WmmpTransType_e type, char* data ,u16 length);
bool ParserIPAddrPortStr(char *srcString ,u16 strLength,u8 serverNum);

WmmpContext_t  WmmpContext;   
static u8 g_localCfgFlag = 0; 

WmmpDataInterface_t wmmpDataResult;

ProtocolStatus_e gM2MStatus;

u8    gLoginStatus = 0;

//bool m2m_sms_engine_init_ok = false; /* 短信功能是否启动 */


u8 currAppNum;//当前正在使用的app 连接 index
u8 m2mStatusParam;
u8 m2mAppStatusParam;
u8 m2mAppBrStatusParam;
u8 m2mHasAppDataFlag[MAX_WMMP_APP_NUM];
u8 m2mHasAppDataType[WMMP_HAS_APP_DATA_TYPE_END];
u32 Transaction_Id;
u32 appTransaction_Id[MAX_WMMP_APP_NUM];
u8  bhvValue[MAX_BHV_MODE];
u8 gwopenValue;
u8 bhvMode;
extern u16 simcom_m2m_at_len;//TLV num

//bool Loginagain;  //登出后更新参数后,是否再次登陆
/******************************************************************************
* Function:   ptrToWmmpMainContext    
* 
* Author:     zhangyuechuan
* 
* Parameters:    
* 
* Return:          
* 
* Description:      
******************************************************************************/
WmmpContext_t*  ptrToWmmpContext(void)
{
  return (&WmmpContext);
}

WmmpDataInterface_t* ptrToWmmpDataInterface(void)
{
    return &wmmpDataResult;
}
//eg, *p= 0x10,0x00  return 0x1000
u16 Readu16(char *p)
{
	return ((*p)<<8)|(*(p+1));
}

u32 Readu32(char *p)
{
	return ((*p)<<24)|(*(p+1)<<16)|(*(p+2)<<8)|(*(p+3));
}
/*cbc220100402:   i --0x1011 -->p-- 0x10,0x11,p=p+2 */
char * Writeu16(char *p,u16 i)
{
	*p=i>>8;
	*(p+1)=i&0xFF;
	return p+2;
}

char *Writeu32(char *p,u32 i)
{
	*p=i>>24;
	*(p+1)=i>>16&0xFF;
	*(p+2)=i>>8&0xFF;
	*(p+3)=i&0xFF;
	return p+4;
}




///* cbc@20100522 注册后未收到来自平台 下发的密码的短信,需要告警,*/
void startRegisterForKeyTimer(void)
{
	WmmpCfgContext_t  *CfgContext_p = &(ptrToWmmpContext()->ConfigContext);
	u16 TLVtag;
	TLVRetVal_t TLVret;	
	
	 ReadTLV(SMSRECVOVERTIME,&TLVret);
	 WMMP_LIB_DBG_TEMP("startRegisterForKeyTimer:tag=0x%x,val=%d,%d,len=%d",SMSRECVOVERTIME,*(TLVret.addr),CfgContext_p->SMSRecvOvertime,TLVret.length);	

        /*ReadTLV will set the SMSRecvOvertime*/
//	 CfgContext_p->SMSRecvOvertime=90;	

#if 0  /*bob remove 20101123*/
	 ReadTLV(SMSRECVOVERTIME,&TLVret);
	 WMMP_LIB_DBG_TEMP("startRegisterForKeyTimer:tag=0x%x,val=%d,%d,len=%d\n",SMSRECVOVERTIME,*(TLVret.addr),CfgContext_p->SMSRecvOvertime,TLVret.length);	
#endif	
	
	wmmpStartInterTimer(RESIGTER_KEY_DOWN_TIMEOUT_TIMER, CfgContext_p->SMSRecvOvertime);
}

void stopRegisterForKeyTimer(void)/* cbc@20100525:停止注册密码下发超时操作 */
{	
	WMMP_LIB_DBG_TEMP("stopRegisterForKeyTimer\n");
	wmmpStopInterTimer(RESIGTER_KEY_DOWN_TIMEOUT_TIMER);
}

void SaveToCriticalBuffer(char * data, u16 len,WmmpTransType_e type)
{
	WmmpContext_t    *wmmpContext_p = ptrToWmmpContext();
	wmmpContext_p->isCriticalUsed = true;
	memcpy(wmmpContext_p->CriticalBuffer,data,len);
	wmmpContext_p->CriticalBufferLen = len;
	wmmpContext_p->CriticalTransType = type;
	//WMMP_LIB_DBG_TEMP("WMMP stack---save to critical buffer");
	WMMP_LIB_DBG_TEMP("WMMP stack---save to critical buffer\n");
}

void DecodeCriticalBuffer(void)
{
	WmmpContext_t    *wmmpContext_p = ptrToWmmpContext();
	WPA_Decode(wmmpContext_p->CriticalTransType, wmmpContext_p->CriticalBuffer, wmmpContext_p->CriticalBufferLen);
	wmmpContext_p->isCriticalUsed = false;
}

void SaveToHighCommandBuffer(CommandInfo_t *CommandInfo)
{

	WmmpContext_t    *wmmpContext_p = ptrToWmmpContext();
    
        WMMP_LIB_DBG_TEMP("SaveToHighCommandBuffer,%d,%d",wmmpContext_p->CommandBufferCount_High,wmmpContext_p->StackState==WMMP_STACK_INIT);
	
	if(wmmpContext_p->m2mswitch == false)
		return ;

	if (wmmpContext_p->CommandBufferCount_High== MAX_COMMANDINFO_NUM)
	{
	    WMMP_LIB_DBG_TEMP("command buffer high is full!\n");
		return ;
	}
	
	if((wmmpContext_p->CommandBufferCount_High==0)&&(wmmpContext_p->StackState==WMMP_STACK_INIT))
	{
	     WMMP_LIB_DBG_TEMP("Send Command_High at once\n");
	     SendCommand(CommandInfo);
	}	
	else
	{
		memcpy(&wmmpContext_p->CommandInfoBuffer_High[wmmpContext_p->CommandBufferRear_High],CommandInfo,sizeof(CommandInfo_t));
		
		wmmpContext_p->CommandBufferRear_High = (wmmpContext_p->CommandBufferRear_High + 1)%MAX_COMMANDINFO_NUM;

		wmmpContext_p->CommandBufferCount_High++;
		WMMP_LIB_DBG_TEMP("store commandbuffer_High:  Rear %d,Count %d\n",wmmpContext_p->CommandBufferRear_High,wmmpContext_p->CommandBufferCount_High);	
	}	

	
}

/*****************************************************************************
*	函数名	      : wmmpLibActiveGprs
* 作  者        : 
*	功能描述      : 活活GPRS
*	输入参数说明  : 无
* 输出参数说明  : 无
*	返回值说明    : 无
*****************************************************************************/
void wmmpLibActiveGprs()
{
     /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
     WMMPAPNType apn_type = wmmp_get_apn_type();
     WmmpCfgContext_t           *pt1 = NULL; 

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    pt1 = (WmmpCfgContext_t *)WPA_ptrToConfigTLV();
    
    WPI_ActiveGPRS(apn_type,pt1->M2MAPN,pt1->GPRSPassword,pt1->GPRSUserId);
}

/*****************************************************************************
*	函数名	      : wmmpLibCloseConnect
* 作  者        : 
*	功能描述      : 去活活GPRS
*	输入参数说明  : 无
* 输出参数说明  : 无
*	返回值说明    : 无
*****************************************************************************/
void wmmpLibCloseConnect(u8 connRef)
{
     /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
     WMMPAPNType apn_type = wmmp_get_apn_type();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    WPI_CloseSocket(apn_type,connRef);
}
/*****************************************************************************
*	函数名	      : wmmpLibDectiveGprs
* 作  者        : 
*	功能描述      : 关闭联接
*	输入参数说明  : 无
* 输出参数说明  : 无
*	返回值说明    : 无
*****************************************************************************/
void wmmpLibDectiveGprs(void)
{
     /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
     WMMPAPNType apn_type = wmmp_get_apn_type();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    
    WPI_DeactiveGPRS(apn_type);
}
/*****************************************************************************
*	函数名	      : SaveToCommandBuffer
* 作  者        : 
*	功能描述      : 命令处理
*	输入参数说明  : 无
* 输出参数说明  : 无
*	返回值说明    : 无
*****************************************************************************/
void SaveToCommandBuffer(CommandInfo_t *CommandInfo)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	WmmpContext_t    *wmmpContext_p = ptrToWmmpContext();
	CommandInfo_t Command_login; 
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	WMMP_LIB_DBG_TEMP("SaveToCommandBuffer CommandBufferCount %d CommandID 0x%04x",wmmpContext_p->CommandBufferCount,CommandInfo->CommandID);
	memset(&Command_login,0,sizeof(CommandInfo_t));
	if(wmmpContext_p->m2mswitch == false)
	{
		WMMP_LIB_DBG_TEMP("wmmpContext_p->m2mswitch == false, exit!");
		return ;
	}

	if (wmmpContext_p->CommandBufferCount == MAX_COMMANDINFO_NUM)
	{
	    WMMP_LIB_DBG_TEMP("command buffer is full!");
		return ;
	}
	/* CBC@20100402:如果当前是心跳报文,而且报文缓冲区中存在未发送报文,扔掉当前心跳包 */
	if((wmmpContext_p->CommandBufferCount != 0)&&(CommandInfo->CommandID==HEART_BEAT_COMMAND))
	{
	    WMMP_LIB_DBG_TEMP("Skip heart beat !");
		return ;	
	}
	
	//处理短连接方式，插入登录报文
	if((wmmpContext_p->ControlContext.ConnectMode==0||wmmpContext_p->ControlContext.ConnectMode==2)&&
		(wmmpContext_p->Wmmp_status.protocol_status==PIDLE)&&(CommandInfo->CommandID!=LOGIN_COMMAND))
	{
			Command_login.CommandID = LOGIN_COMMAND;
			//Command_login.SecuritySet = 224;//11100000
			Command_login.SecuritySet = 0;
			Command_login.TransType = WMMP_TRANS_DEFAULT;
			Command_login.PDUUsage = WMMP_SHORT_CONNECT;
			WMMP_LIB_DBG_TEMP("SaveToHighCommandBuffer");
			SaveToHighCommandBuffer(&Command_login);/*cbc@短连接每次报文需要登录 ,长连接时只有TRAP 命令	*/
	}

	/* 当前报文缓冲区为空,立即发送当前报文*/
	if((wmmpContext_p->CommandBufferCount==0)&&(wmmpContext_p->CommandBufferCount_High==0)
		&&(wmmpContext_p->StackState==WMMP_STACK_INIT)/*&&(wmmpContext_p->enableSend==true)*/)/*如果有命令要发送，马上发送*/
	{
		WMMP_LIB_DBG_TEMP("Send Command at once");
	         SendCommand(CommandInfo);
	}	
	else
	{     /* 当前报文缓冲去不为空,保存当前的 报文至缓冲区队列*/
		memcpy(&wmmpContext_p->CommandInfoBuffer[wmmpContext_p->CommandBufferRear],CommandInfo,sizeof(CommandInfo_t));
		
		wmmpContext_p->CommandBufferRear = (wmmpContext_p->CommandBufferRear + 1)%MAX_COMMANDINFO_NUM;

		wmmpContext_p->CommandBufferCount++;
		WMMP_LIB_DBG_TEMP("STORE command:front=%d,rear=%d,count=%d",wmmpContext_p->CommandBufferFront,
																		wmmpContext_p->CommandBufferRear,
																			wmmpContext_p->CommandBufferCount);	
		WMMP_LIB_DBG_TEMP("STORE:Count_High=%d,StackState=%d,enableSend=%d",wmmpContext_p->CommandBufferCount_High,
																					wmmpContext_p->StackState,
																						wmmpContext_p->enableSend);	
	}	

	
}

void ClearCommandBuffer(void)
{
	u8 i;
	WmmpContext_t    *wmmpContext_p = ptrToWmmpContext();

        WMMP_LIB_DBG_TEMP("ClearCommandBuffer");

	//防止内存泄露
	for(i=0;i<MAX_COMMANDINFO_NUM;i++)
	{
		switch(wmmpContext_p->CommandInfoBuffer[i].CommandID)
		{
			case TRANSPARENT_DATA_COMMAND:
			{
				WMMP_LIB_DBG_TEMP("ClearCommandBufferl TRANSPARENT_DATA_COMMAND!\n");
				
				if(wmmpContext_p->CommandInfoBuffer[i].CommandExInfo.TransparentDataInfo.Data!=PNULL)
				{
					WPI_Free(wmmpContext_p->CommandInfoBuffer[i].CommandExInfo.TransparentDataInfo.Data);
				}
			}
			break;
			case CONFIG_TRAP_COMMAND:
			{
				WMMP_LIB_DBG_TEMP("ClearCommandBufferl CONFIG_TRAP_COMMAND!\n");
				
				if(wmmpContext_p->CommandInfoBuffer[i].CommandExInfo.ConfigTrapInfo.TLVTags !=PNULL)
				{
					WPI_Free(wmmpContext_p->CommandInfoBuffer[i].CommandExInfo.ConfigTrapInfo.TLVTags);
				}
			}
			break;
			case CONFIG_REQ_COMMAND:
			{
				WMMP_LIB_DBG_TEMP("ClearCommandBufferl CONFIG_REQ_COMMAND!\n");
				
				if(wmmpContext_p->CommandInfoBuffer[i].CommandExInfo.ConfigReqInfo.TLVTags !=PNULL)
				{
					WPI_Free(wmmpContext_p->CommandInfoBuffer[i].CommandExInfo.ConfigReqInfo.TLVTags);
				}
			}
			break;		

			default:
				WMMP_LIB_DBG_TEMP("ClearCommandBufferl default!\n");
				break;
		}

		switch(wmmpContext_p->CommandInfoBuffer_High[i].CommandID)
		{
			case TRANSPARENT_DATA_COMMAND:
			{
				WMMP_LIB_DBG_TEMP("ClearCommandBufferl High TRANSPARENT_DATA_COMMAND!\n");
				
				if(wmmpContext_p->CommandInfoBuffer_High[i].CommandExInfo.TransparentDataInfo.Data!=PNULL)
				{
					WPI_Free(wmmpContext_p->CommandInfoBuffer_High[i].CommandExInfo.TransparentDataInfo.Data);
				}
			}
			break;
			case CONFIG_TRAP_COMMAND:
			{
				WMMP_LIB_DBG_TEMP("ClearCommandBufferl High CONFIG_TRAP_COMMAND!\n");
				
				if(wmmpContext_p->CommandInfoBuffer_High[i].CommandExInfo.ConfigTrapInfo.TLVTags !=PNULL)
				{
					WPI_Free(wmmpContext_p->CommandInfoBuffer_High[i].CommandExInfo.ConfigTrapInfo.TLVTags);
				}
			}
			break;
			case CONFIG_REQ_COMMAND:
			{
				WMMP_LIB_DBG_TEMP("ClearCommandBufferl High CONFIG_REQ_COMMAND!\n");
				
				if(wmmpContext_p->CommandInfoBuffer_High[i].CommandExInfo.ConfigReqInfo.TLVTags !=PNULL)
				{
					WPI_Free(wmmpContext_p->CommandInfoBuffer_High[i].CommandExInfo.ConfigReqInfo.TLVTags);
				}
			}
			break;	

			default:
				WMMP_LIB_DBG_TEMP("ClearCommandBufferl High default!\n");
				break;
		}		
	}

 	wmmpContext_p->CommandBufferFront = 0;    
	wmmpContext_p->CommandBufferRear = 0;   
 	wmmpContext_p->CommandBufferCount = 0;   

 	wmmpContext_p->CommandBufferFront_High= 0;    
	wmmpContext_p->CommandBufferRear_High= 0;   
 	wmmpContext_p->CommandBufferCount_High= 0;   
}



void RollClearCommandBuffer(void)
{
	u8 i;
	WmmpContext_t    *wmmpContext_p = ptrToWmmpContext();	

        WMMP_LIB_DBG_TEMP("RollClearCommandBuffer");

 	wmmpContext_p->CommandBufferFront = 0;    
	wmmpContext_p->CommandBufferRear = 0;   
 	wmmpContext_p->CommandBufferCount = 0;   

 	wmmpContext_p->CommandBufferFront_High= 0;    
	wmmpContext_p->CommandBufferRear_High= 0;   
 	wmmpContext_p->CommandBufferCount_High= 0;   
}



/*
bool InTimePeriod(u32 time1,u32 time2)
{

	u32 i;
	 
	i = aclkGetCurrentUnixTime();
	WMMP_LIB_DBG_TEMP("compare time %d,%d,%d", i, time1,time2);
	if( i!=0)
	{
		if ((i>=time1) &&(i<=time2))
			return true;
	}

	return false;
}
*/
void FreeWMMPPDUContext(void)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	PDUContext_t  *PDUContext = &mainContext_p->PDUContext;
	PDUContext_t  *PDUContext_receive = &mainContext_p->PDUContext_receive;	
	memset(mainContext_p->ReceiveBuffer,0,MAX_RECEIVE_BUFFER);
	memset(mainContext_p->SendBuffer,0,MAX_SEND_BUFFER);
	memset(mainContext_p->IOBuffer,0,MAX_IO_BUFFER);
	memset(PDUContext,0,sizeof(PDUContext_t));

	memset(PDUContext_receive,0,sizeof(PDUContext_t));

	mainContext_p->StackState = WMMP_STACK_INIT;
	mainContext_p->TransType = WMMP_TRANS_DEFAULT;  //default transtype;
	mainContext_p->PDUUsage = WMMP_PDU_UNKNOWN;
	//mainContext_p->TLVtemp= PNULL;
	//mainContext_p->TLVtempLen = 0;

	WMMP_LIB_DBG_TEMP("FreeWMMPPDUContext!");

}

u32 CalcStatPeriod(u16 basevalue,u8 unit)
{
	switch(unit)
	{
		case 0:
		{
			return 0;
		}
		//break;
		case 1:
		{
			return basevalue*60;
		}
		//break;
		case 2:
		{
			return basevalue*60*60;
		}
		//break;
		case 3:
		{
			return basevalue*60*60*24;
		}
		//break;
		case 4:
		{
			return basevalue*60*60*24*7;
		}
		//break;
		case 5:
		{
			return basevalue*60*60*24*30;
		}
		//break;
		case 6:
		{
			return basevalue*60*60*24*365;
		}
		//break;
		default:
			return 0;
	}
}

void SigmaStatTrap(WmmpPDUUsage_e Usage)
{
	u16  *TLV;
	CommandInfo_t CommandInfo;
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();	
	TLV = (u16*)WPI_Malloc( 16);

	WMMP_LIB_DBG_TEMP("SigmaStatTrap!");

	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	//KiAllocZeroMemory(16, (void**) &TLV);
	TLV[0]=0x2004;
	TLV[1]=0x2008;
	TLV[2]=0x2009;	
	TLV[3]=0x200A;	
	TLV[4]=0x200B;	
	TLV[5]=0x200C;		
	TLV[6]=0x200D;		
	TLV[7]=0x200E;		
	CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
	CommandInfo.SecuritySet = 0;
	CommandInfo.TransType = WMMP_TRANS_DEFAULT;
	CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=TLV;
	CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=8;
	CommandInfo.PDUUsage=Usage;
	SaveToCommandBuffer(&CommandInfo);

}

//目前统计功能作用于长连接,对于短连接,需要分辨登录登出是由用户输入的还是功能流程自动进行的
void CheckStatTime(void)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	u32 starttime;
	u32 endtime;
	u32 currenttime;
	u16 basevalue;
	u8 unit ;
	u16  *TLV;
	CommandInfo_t CommandInfo;

	WPIRtcDateAndTime dateAndTime;
	WPIRtcStatus_e      rtcStatus;

        WMMP_LIB_DBG_TEMP("CheckStatTime");

	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	currenttime = WPI_GetCurrentTime(&dateAndTime, &rtcStatus);	
		
	//currenttime = aclkGetCurrentUnixTime();
	//check statc eabled, TLV0x2006 and TLV2007
	if(((currenttime>=mainContext_p->StatisticContext.StatStartTime)&&(mainContext_p->StatisticContext.statFinishTime>=currenttime))
		||((mainContext_p->StatisticContext.StatInfoReport==0)&&(mainContext_p->StatisticContext.ReportTime!=0)))
	{
		mainContext_p->isM2MStatTime = true;
	}
	else
	{
		mainContext_p->isM2MStatTime = false;
	}

	//检查0x2001, 定时立即上报或者每月首次上报
	switch(mainContext_p->StatisticContext.StatInfoReport)
	{
		case 0:
		{
			if((mainContext_p->StatisticContext.ReportTime!=0)&&(currenttime>=mainContext_p->StatisticContext.ReportTime))
			{
				SigmaStatTrap(WMMP_PDU_TRAP_SIGMA_TIME_STAT);
			}
		}
		break;
		
		case 2:
		{
			if((mainContext_p->CustomContext.LastTrappedMonth!=dateAndTime.month)&&(mainContext_p->CustomContext.isCurrentMonthTrapped==false))
			{
				mainContext_p->CustomContext.LastTrappedMonth=dateAndTime.month;
			 	mainContext_p->CustomContext.isCurrentMonthTrapped=true;
				SigmaStatTrap(WMMP_PDU_TRAP_SIGMA_STAT);
			}
		}
		break;	

		default:
			break;
	}
	
	//使用M2MStatRecFirstTime来模拟定时器,因为定时器的时间无法非常长
	//check SMS static 
	if(mainContext_p->StatisticContext.SMSStatParam[0]==1)
	{
		if(mainContext_p->CustomContext.SMSM2MStatRecFirstTime==0)
		{
			starttime=Readu32(&mainContext_p->StatisticContext.SMSStatParam[4]);
			endtime = Readu32(&mainContext_p->StatisticContext.SMSStatParam[8]);		
			
			if((currenttime>=starttime)&&(currenttime<=endtime))
			{
				mainContext_p->CustomContext.SMSM2MStatRecFirstTime = currenttime;
				mainContext_p->CustomContext.SMSM2MStatRecID = 0;
			}
			else
			{
				mainContext_p->CustomContext.SMSM2MStatRecFirstTime = 0;
				mainContext_p->CustomContext.SMSM2MStatRecID = 0;
			}
		}
		else
		{
			basevalue  = Readu16(&mainContext_p->StatisticContext.SMSStatParam[1]);
			unit = mainContext_p->StatisticContext.SMSStatParam[3];
			if((currenttime-mainContext_p->CustomContext.SMSM2MStatRecFirstTime>=CalcStatPeriod(basevalue,unit))&&(mainContext_p->CustomContext.SMSM2MStatRecID<=MAX_STAT_NUM))
			{
				//save record
				WMMP_LIB_DBG_TEMP("save SMS record,%d,%d",mainContext_p->CustomContext.SMSM2MSendStat,mainContext_p->CustomContext.SMSM2MRecvStat);
				Writeu32(&mainContext_p->StatisticContext.M2MMutualSMSStat[mainContext_p->CustomContext.SMSM2MStatRecID][0], currenttime);
				Writeu16(&mainContext_p->StatisticContext.M2MMutualSMSStat[mainContext_p->CustomContext.SMSM2MStatRecID][4], mainContext_p->CustomContext.SMSM2MSendStat);
				Writeu16(&mainContext_p->StatisticContext.M2MMutualSMSStat[mainContext_p->CustomContext.SMSM2MStatRecID][6], mainContext_p->CustomContext.SMSM2MRecvStat);
				mainContext_p->CustomContext.SMSM2MStatRecID++;
				mainContext_p->CustomContext.SMSM2MStatRecFirstTime = currenttime;
				mainContext_p->CustomContext.SMSM2MSendStat = 0;
				mainContext_p->CustomContext.SMSM2MRecvStat = 0;
			}
		}
	}
	//check GPRS static
	if(mainContext_p->StatisticContext.GPRSOperationStatParam[0]==1)
	{
		if(mainContext_p->CustomContext.GPRSM2MStatRecFirstTime==0)
		{
			starttime=Readu32(&mainContext_p->StatisticContext.GPRSOperationStatParam[4]);
			endtime = Readu32(&mainContext_p->StatisticContext.GPRSOperationStatParam[8]);		

			if((currenttime>=starttime)&&(currenttime<=endtime))
			{
				mainContext_p->CustomContext.GPRSM2MStatRecFirstTime = currenttime;
				mainContext_p->CustomContext.GPRSM2MStatRecID = 0;
			}
			else
			{
				mainContext_p->CustomContext.GPRSM2MStatRecFirstTime = 0;
				mainContext_p->CustomContext.GPRSM2MStatRecID = 0;
			}
		}
		else
		{
			basevalue  = Readu16(&mainContext_p->StatisticContext.GPRSOperationStatParam[1]);
			unit = mainContext_p->StatisticContext.GPRSOperationStatParam[3];

			if((currenttime-mainContext_p->CustomContext.GPRSM2MStatRecFirstTime>=CalcStatPeriod(basevalue,unit))&&(mainContext_p->CustomContext.GPRSM2MStatRecID<=MAX_STAT_NUM))
			{
				//save record
				WMMP_LIB_DBG_TEMP("save GPRS record,%d,%d",mainContext_p->CustomContext.GPRSM2MSendStat,mainContext_p->CustomContext.GPRSM2MRecvStat);
				Writeu32(&mainContext_p->StatisticContext.M2MMutualGPRSDataSize[mainContext_p->CustomContext.GPRSM2MStatRecID][0], currenttime);
				Writeu32(&mainContext_p->StatisticContext.M2MMutualGPRSDataSize[mainContext_p->CustomContext.GPRSM2MStatRecID][4], mainContext_p->CustomContext.GPRSM2MSendStat);
				Writeu32(&mainContext_p->StatisticContext.M2MMutualGPRSDataSize[mainContext_p->CustomContext.GPRSM2MStatRecID][8], mainContext_p->CustomContext.GPRSM2MRecvStat);
				mainContext_p->CustomContext.GPRSM2MStatRecID++;
				mainContext_p->CustomContext.GPRSM2MStatRecFirstTime = currenttime;
				mainContext_p->CustomContext.GPRSM2MSendStat = 0;
				mainContext_p->CustomContext.GPRSM2MRecvStat = 0;
			}
		}
	}	
	
	//check SMS auto trap
	if(mainContext_p->StatisticContext.SMSStatAutoReportParam[0]==1)
	{
		if(mainContext_p->CustomContext.SMSM2MStatTrapFirstTime==0)
		{
			starttime=Readu32(&mainContext_p->StatisticContext.SMSStatAutoReportParam[4]);
			endtime = Readu32(&mainContext_p->StatisticContext.SMSStatAutoReportParam[8]);		
			
			if((currenttime>=starttime)&&(currenttime<=endtime))
			{
				mainContext_p->CustomContext.SMSM2MStatTrapFirstTime = currenttime;
			}
			else
			{
				mainContext_p->CustomContext.SMSM2MStatRecFirstTime = 0;
			}
		}
		else
		{
			basevalue  = Readu16(&mainContext_p->StatisticContext.SMSStatAutoReportParam[1]);
			unit = mainContext_p->StatisticContext.SMSStatAutoReportParam[3];
			if(currenttime-mainContext_p->CustomContext.SMSM2MStatTrapFirstTime>=CalcStatPeriod(basevalue,unit))
			{
				//trap record
				TLV = (u16*)WPI_Malloc(4);
				//KiAllocZeroMemory(4, (void**) &TLV);
				TLV[0]=0x2013;
				TLV[1]=0x2014;
				CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
				CommandInfo.SecuritySet = 0;
				CommandInfo.TransType = WMMP_TRANS_DEFAULT;
				CommandInfo.PDUUsage = WMMP_PDU_TRAP_SMS_STAT;

				CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=TLV;
				CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=2;
				SaveToCommandBuffer(&CommandInfo);

				//mainContext_p->SMSM2MStatRecID=0
				//mainContext_p->SMSStatTrapped = true;
				mainContext_p->CustomContext.SMSM2MStatTrapFirstTime = currenttime;

			}
		}
	}

	//check GPRS auto trap
	if(mainContext_p->StatisticContext.GPRSOperationStatReportParam[0]==1)
	{
		if(mainContext_p->CustomContext.GPRSM2MStatTrapFirstTime==0)
		{
			starttime=Readu32(&mainContext_p->StatisticContext.GPRSOperationStatReportParam[4]);
			endtime = Readu32(&mainContext_p->StatisticContext.GPRSOperationStatReportParam[8]);		
			
			if((currenttime>=starttime)&&(currenttime<=endtime))
			{
				mainContext_p->CustomContext.GPRSM2MStatTrapFirstTime = currenttime;
			}
			else
			{
				mainContext_p->CustomContext.GPRSM2MStatTrapFirstTime = 0;
			}
		}
		else
		{
			basevalue  = Readu16(&mainContext_p->StatisticContext.GPRSOperationStatReportParam[1]);
			unit = mainContext_p->StatisticContext.GPRSOperationStatReportParam[3];
			if(currenttime-mainContext_p->CustomContext.GPRSM2MStatTrapFirstTime>=CalcStatPeriod(basevalue,unit))
			{
				WMMP_LIB_DBG_TEMP("auto trap!");
				//trap record
				TLV = (u16*)WPI_Malloc( 4);
				//KiAllocZeroMemory(4, (void**) &TLV);
				TLV[0]=0x2017;
				TLV[1]=0x2018;
				CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
				CommandInfo.SecuritySet = 0;
				CommandInfo.TransType = WMMP_TRANS_DEFAULT;
				CommandInfo.PDUUsage = WMMP_PDU_TRAP_GPRS_STAT;

				CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=TLV;
				CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=2;
				SaveToCommandBuffer(&CommandInfo);

				//mainContext_p->SMSM2MStatRecID=0
				//mainContext_p->GPRSStatTrapped = true;
				mainContext_p->CustomContext.GPRSM2MStatTrapFirstTime = currenttime;

			}
		}
	}


	//检查会话密钥有效期
	if((mainContext_p->SecurityContext.convKeyExpDate<currenttime)&&(mainContext_p->SecurityContext.convKeyExpDate!=0)
		&&(mainContext_p->SecurityContext.encryptionUsed==1))
	{
		WMMP_LIB_DBG_TEMP("convKeyExpDate is out of data!,logout");
		CommandInfo.CommandID = LOGOUT_COMMAND;
		CommandInfo.SecuritySet = 0;
		CommandInfo.TransType = WMMP_TRANS_DEFAULT;
		CommandInfo.PDUUsage = WMMP_KEY_OUT_OF_DATE;
		CommandInfo.CommandExInfo.LoginOutInfo.reason=8;

		SaveToCommandBuffer(&CommandInfo);

		CommandInfo.CommandID = LOGIN_COMMAND;
		CommandInfo.SecuritySet = 0;	
		CommandInfo.TransType = WMMP_TRANS_DEFAULT;
		CommandInfo.PDUUsage = WMMP_KEY_OUT_OF_DATE;
		SaveToCommandBuffer(&CommandInfo);		
	}

	//检查基础密钥有效期
	if(((mainContext_p->SecurityContext.publicKeyExpDate<currenttime)||(mainContext_p->SecurityContext.uplinkPasswordExpDate<currenttime)||(mainContext_p->SecurityContext.downlinkPasswordExpDate<currenttime))
		&&(mainContext_p->SecurityContext.encryptionUsed==1))	
	{
		u16 *tags;
		WMMP_LIB_DBG_TEMP("WMMP stack---publicKeyExpDate is out of data!,security cfg");
		CommandInfo.CommandID = SECURITY_CONFIG_COMMAND;
		CommandInfo.SecuritySet = 0;	
		CommandInfo.TransType = WMMP_TRANS_DEFAULT;	
		CommandInfo.PDUUsage =WMMP_KEY_OUT_OF_DATE;
		tags = (u16*)WPI_Malloc(6*sizeof(u16));
		tags[0]=0xE038;
		tags[1]=0xE036;
		tags[2]=0xE02A;
		tags[3]=0xE028;
		tags[4]=0xE027;
		tags[5]=0xE025;
		CommandInfo.CommandExInfo.SecurityInfo.operation=6;
		CommandInfo.CommandExInfo.SecurityInfo.tags=tags;
		CommandInfo.CommandExInfo.SecurityInfo.tagnum=6;
		SaveToCommandBuffer(&CommandInfo);			
	}
}

void wmmpWriteTLVs(char *head,u16 length)
{
	char *p=head;
	u16 tag_Id,tag_length;
	// u16 *tag;
	while (p-head< length)
	{
		tag_Id = Readu16(p);
		p+=2;
		tag_length = Readu16(p);
		p+=2;
		WriteTLV(tag_Id, tag_length, p);
		WMMP_LIB_DBG_TEMP("write tlv %04X,%d",tag_Id,tag_length);
		p+=tag_length; 
		//memcpy(tag+2*tagnum,tag_Id,2);
		//tag[tagnum]=tag_Id;
		//tagnum++;
	}
}



bool checkTLVtemp(u16 Tag,char* head, u16 templength, TLVRetVal_t* tlvret)
{
	char *p=head;
	u16 tag_Id,tag_length;
	//u16 *tag;	

        WMMP_LIB_DBG_TEMP("checkTLVtemp Tag=%d",Tag);

	if (head==PNULL)
	return false;

	while (p-head< templength)
	{
		tag_Id = Readu16(p);
		p+=2;
		tag_length = Readu16(p);
		p+=2;
		
		if(tag_Id==Tag)	
		{
			//find the tag
			tlvret->addr = p;
			tlvret->length = tag_length;
			return true;
		}				
		p+=tag_length;     	
	}	
	return false;
}

//value is frome tlvtemp ,not the actual value
//若应用成功，则返回true, 
bool wmmpAppTLV(u16 Tag_Id, u16 length, char* value) 
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	//CommandInfo_t CommandInfo; 
	//u16 *TLV;	
	char simpin[9];	
	TLVRetVal_t TLVret; 
	bool ret=true;
	WPAEventData_t WPAEvent;
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_ADC_GPIO__
	u8 mode=0,status=0;
	u16 temp_value1=0;
	s16 i;/*SIMCOM luyong 2012-09-25 Fix MKBug00013842 add */
	u32 index,temp_value=0,temp_index=0;
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/
	WMMP_LIB_DBG_TEMP("wmmpAppTLV -; %04X,%d",Tag_Id,length);
	//WPI_hex_printf((char*)value,(u16)(length));
	switch(Tag_Id)
	{
		case 0xE00D:
		{			
			if(*value==2)
			{
				if((strlen((char *)mainContext_p->SecurityContext.SIMPin1)!=0)&&(mainContext_p->SecurityContext.SIMPin1Active==1))
				{
					//disable cpin
					if (WPI_SecurityDisableCPIN(mainContext_p->SecurityContext.SIMPin1) == true)
					{
						WPAEvent.WPACPinEvent.error = WMMP_SECURITY_OPER_ERROR_OK;
					}
					else
					{
						WPAEvent.WPACPinEvent.error = WMMP_SECURITY_OPER_ERROR_DISABLE_FAIL;
					}
					 
					WPAEvent.WPACPinEvent.operation = WMMP_SECURITY_OPER_DISABLE;
					WPA_ProcessEvent(WPA_CPIN_EVENT, &WPAEvent);
				}

				ret = false;
				
				//mainContext_p->SecurityContext.BindParaActive = 0;
				//Loginagain = false;
			}
			
			
		}
		break;
		case 0xE001:
		{
			memset(simpin,0,9);
			memcpy(simpin,value,length);			
			if(checkTLVtemp(0xE003,mainContext_p->TLVtemp,mainContext_p->TLVtempLen,&TLVret)==true)
			{
				if(*TLVret.addr==1)
				{
					//change cpin
					if(strcmp((char *)simpin,(char *)mainContext_p->SecurityContext.SIMPin1)!=0)
					{
						if (WPI_SecurityChangeCpin(mainContext_p->SecurityContext.SIMPin1, simpin) == true)
						{
							WPAEvent.WPACPinEvent.error = WMMP_SECURITY_OPER_ERROR_OK;
						}
						else
						{
							WPAEvent.WPACPinEvent.error = WMMP_SECURITY_OPER_ERROR_OTHER;
						}
						WPAEvent.WPACPinEvent.operation = WMMP_SECURITY_OPER_AUTO;
						WPA_ProcessEvent(WPA_CPIN_EVENT, &WPAEvent);
						WMMP_LIB_DBG_TEMP("change cpin %s",simpin);
					}	
					else
					{
						 if (WPI_SecurityEnableCpin(mainContext_p->SecurityContext.SIMPin1) == true)
						 {
							WPAEvent.WPACPinEvent.error = WMMP_SECURITY_OPER_ERROR_OK;
						 }
						 else
						 {
							WPAEvent.WPACPinEvent.error = WMMP_SECURITY_OPER_ERROR_ENABLE_FAIL;
						 }
						WPAEvent.WPACPinEvent.operation = WMMP_SECURITY_OPER_ENABLE;	
						WPA_ProcessEvent(WPA_CPIN_EVENT, &WPAEvent);
						WMMP_LIB_DBG_TEMP("enabled cpin %s",mainContext_p->SecurityContext.SIMPin1);
					}
				}
				else
				{
					//i think it's invalid if 0xE003=0 and 0xe001 also has value
					ret = false;
				}
			}
			else
			{
				if(mainContext_p->SecurityContext.SIMPin1Active==1)
				{
					if(strcmp((char *)simpin,(char *)mainContext_p->SecurityContext.SIMPin1)!=0)
					{
						if (WPI_SecurityChangeCpin(mainContext_p->SecurityContext.SIMPin1, simpin) == true)
						{
							WPAEvent.WPACPinEvent.error = WMMP_SECURITY_OPER_ERROR_OK;
						}
						else
						{
							WPAEvent.WPACPinEvent.error = WMMP_SECURITY_OPER_ERROR_OTHER;
						}
						WPAEvent.WPACPinEvent.operation = WMMP_SECURITY_OPER_AUTO;
						WPA_ProcessEvent(WPA_CPIN_EVENT, &WPAEvent);
						WMMP_LIB_DBG_TEMP("change cpin %s",simpin);
					}						
					else
					{
						ret = false;
					}
				}
				else
				{
					ret = false;
				}
			}
			
		}
		break;
		case 0xE003:
		{
			if(*value!=mainContext_p->SecurityContext.SIMPin1Active)
			{
				//check  modify TLV0xE001 together or not
				if(checkTLVtemp(0xE001,mainContext_p->TLVtemp,mainContext_p->TLVtempLen,&TLVret)==false)
				{
					//enable cpin			
					if(*value==1)
					{
						 if (WPI_SecurityEnableCpin(mainContext_p->SecurityContext.SIMPin1) == true)
						 {
							WPAEvent.WPACPinEvent.error = WMMP_SECURITY_OPER_ERROR_OK;
						 }
						 else
						 {
							WPAEvent.WPACPinEvent.error = WMMP_SECURITY_OPER_ERROR_ENABLE_FAIL;
						 }
						WPAEvent.WPACPinEvent.operation = WMMP_SECURITY_OPER_ENABLE;	
						WPA_ProcessEvent(WPA_CPIN_EVENT, &WPAEvent);
						WMMP_LIB_DBG_TEMP("enabled cpin %s",mainContext_p->SecurityContext.SIMPin1);
					}
					else if(*value==0)
					{
						//disable cpin
						if (WPI_SecurityDisableCPIN(mainContext_p->SecurityContext.SIMPin1) == true)
						{
							WPAEvent.WPACPinEvent.error = WMMP_SECURITY_OPER_ERROR_OK;
						}
						else
						{
							WPAEvent.WPACPinEvent.error = WMMP_SECURITY_OPER_ERROR_DISABLE_FAIL;
						}
						 
						WPAEvent.WPACPinEvent.operation = WMMP_SECURITY_OPER_DISABLE;
						WPA_ProcessEvent(WPA_CPIN_EVENT, &WPAEvent);
						WMMP_LIB_DBG_TEMP("disabled the cpin %s",mainContext_p->SecurityContext.SIMPin1);
					}
				}
			}
		}
		break;
		case 0x4002:  //change connect mode
		{

			if((* value!=1)&&(* value!=3))
			{
			   wmmpStopInterTimer(HEART_BEAT_TIMER);
				//KiStopTimer(&mainContext_p->HeartBeatTimer);
				WMMP_LIB_DBG_TEMP("stop heartbeat");
			}
			else
			{
				wmmpStartInterTimer(HEART_BEAT_TIMER, mainContext_p->ConfigContext.Period);
				WMMP_LIB_DBG_TEMP("start heartbeat");				
			}

			if((* value!=0)&&(* value!=2))
			{
				wmmpStopInterTimer(SHORT_CONNECT_MODE_TIMER);
				//KiStopTimer(&mainContext_p->ShortConnectModeTimer);
				WMMP_LIB_DBG_TEMP("stop ShortConnectModeTimer");
			}
			
			wmmpLibDectiveGprs();
			//wmmpSendIpShutdownReq(SIMCOM_WMMP_TASK_ID);
		}
		break;
		//累加信息统计上报
		case 0x2001:
		{
			switch(*value)
			{
				case 0:
				{
					mainContext_p->StatisticContext.MonthlyOperationSign=0;

					mainContext_p->CustomContext.isCurrentMonthTrapped=false;
					wmmpStopInterTimer(SIGMA_STAT_TIMER);
					//KiStopTimer(&mainContext_p->SigmaStatTimer);
				}
				break;
				
				case 1:
				{
					u32 PeriodTime;
					if(checkTLVtemp(0x2003,mainContext_p->TLVtemp,mainContext_p->TLVtempLen,&TLVret)==true)
					{
						PeriodTime=Readu32(TLVret.addr);
					}
					else
					{
						PeriodTime= mainContext_p->StatisticContext.PeriodReport;
					}
					
					mainContext_p->StatisticContext.MonthlyOperationSign=0;
		
					mainContext_p->CustomContext.isCurrentMonthTrapped=false;
					
			       	// KiStopTimer(&mainContext_p->SigmaStatTimer);
			       	wmmpStopInterTimer(SIGMA_STAT_TIMER);
							
					if(PeriodTime!=0)
					{
						wmmpStartInterTimer(SIGMA_STAT_TIMER, PeriodTime);					
				 		// mainContext_p->SigmaStatTimer.timeoutPeriod = SECONDS_TO_TICKS(PeriodTime); 
						 WMMP_LIB_DBG_TEMP("StatisticContext.PeriodReport is ,%d",PeriodTime);
				       // KiStartTimer(&mainContext_p->SigmaStatTimer);
					}
				}
				break;	
				
				case 2:
				{	
					mainContext_p->StatisticContext.MonthlyOperationSign=1;
				
					mainContext_p->CustomContext.isCurrentMonthTrapped=false;
					//KiStopTimer(&mainContext_p->SigmaStatTimer);
					wmmpStopInterTimer(SIGMA_STAT_TIMER);
				}
				break;

				default:
					break;
			}
			

		}
		break;


		case 0x0026:
		{
			if((*value==0)&&(*(value+1)==0))
			{
				WMMP_LIB_DBG_TEMP("wmmpAppTLV: OX0026 IS 0");
				ret = false;				
			}
		}
		break;
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_ADC_GPIO__
		case 0x81F6:
		{
			temp_value1=Readu16(value);
			WMMP_LIB_DBG_TEMP("wmmpAppTLV: 0x81F6 :value=%2x ",temp_value1);
			//WMMP_LIB_DBG_TEMP("wmmpAppTLV: 0x81F6 :value=%d ",temp_value1);
			temp_value1=(~temp_value1);
			WMMP_LIB_DBG_TEMP("wmmpAppTLV: 0x81F6 :~value=%2x ",temp_value1);
			//WMMP_LIB_DBG_TEMP("wmmpAppTLV: 0x81F6 :~value=%d ",temp_value1);
			for(i=15;i>=0;i--)/*SIMCOM luyong 2012-09-25 Fix MKBug00013842 add */
			{
				if((temp_value1>>i)==1)
				{
					index=i+1;
					//WMMP_LIB_DBG_TEMP("wmmpAppTLV: 0x81F6 :index=%d,i=%d ",index,i);
					ret=WPI_Config_Gpio(index,mode,status);
					if(ret==true)
					{
						WPI_Set_Gpio_Status( index,mode, status);
					}
					//WMMP_LIB_DBG_TEMP("wmmpAppTLV: 0x81F6 :index=%d ",index);
					temp_value1=temp_value1&(~(1<<i));/*SIMCOM luyong 2012-09-25 Fix MKBug00013842 add */
					WMMP_LIB_DBG_TEMP("wmmpAppTLV: 0x81F6 :temp_value1=%d ,i=%d",temp_value1,i);
				}
			}
		}
		break;
		case 0x81F7:
		{
			mode=1;
			temp_value=Readu32(value);
			WMMP_LIB_DBG_TEMP("wmmpAppTLV: 0x81F7 :value=%x ",temp_value);
			temp_index=(temp_value>>16);
			WMMP_LIB_DBG_TEMP("wmmpAppTLV: 0x81F7 :temp_index=%x ",temp_index);
			for(i=15;i>=0;i--)/*SIMCOM luyong 2012-09-25 Fix MKBug00013842 add */
			{
				if((temp_index>>i)==1)
				{
					index=i+1;
					//WMMP_LIB_DBG_TEMP("wmmpAppTLV: 0x81F7 :index=%d,temp_value=%x ",index,temp_value);
					if(((1<<(index-1))&temp_value)!=0)
					{
						status=1;
					}
					else
					{
						status=0;
					}
					//WMMP_LIB_DBG_TEMP("wmmpAppTLV: 0x81F7:status=%d ",status);
					ret=WPI_Config_Gpio(index,mode,status);
					if(ret==true)
					{
						WPI_Set_Gpio_Status( index,mode, status);
					}
					temp_index=temp_index&(~(1<<i));/*SIMCOM luyong 2012-09-25 Fix MKBug00013842 add */
					WMMP_LIB_DBG_TEMP("wmmpAppTLV: 0x81F7 :temp_index=%d ,i=%d",temp_index,i);
				}
			}
			
		}
		break;
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/

		default:
			break;
	}
	return ret;
}



void wmmpAppTLVs(char *head,u16 length)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
    char *p=head;
    u16 tag_Id,tag_length;

    char *q_tlv= PNULL;
    char TLV_buffer[1024]; //temp for calc TLV CRC
    u32 CRC_value=0;
   TLVRetVal_t TLVret;
   bool tlvflag = false;

    WMMP_LIB_DBG_TEMP("wmmpAppTLVs ");

    memset(TLV_buffer,0,sizeof(TLV_buffer));
    q_tlv = TLV_buffer;
	
   // u16 *tag;	
    while (p-head< length)
    {
        tag_Id = Readu16(p);
        p+=2;
        tag_length = Readu16(p);
        p+=2;
		
        if (wmmpAppTLV(tag_Id, tag_length, p)==true)
	{
		WriteTLV(tag_Id, tag_length, p);
        	WMMP_LIB_DBG_TEMP("WMMP stack---write tlv %04X,%d",tag_Id,tag_length);

               /*add cbc@20100526 :用来计算0X4017的值begin*/
		if (tag_Id != 0x4009)
		{			
		       q_tlv=Writeu16(q_tlv,tag_Id);	
			ReadTLV(tag_Id,&TLVret);
			q_tlv=Writeu16(q_tlv,tag_length);
			memcpyTLV(TLVret,q_tlv);
			q_tlv+=TLVret.length;
		}
		   /*add cbc@20100526 :用来计算0X4017的值end*/	      
       }      
	else
	{
	       tlvflag = true;
		/* cbc@20100527:平台下发配置参数格式正常,应用失败,无需重启, */
		//WMMP_LIB_DBG_TEMP("wmmpAppTLVs---write tlv 0x4010 = 2 \n");
		//mainContext_p->ControlContext.ExecuteParamOpt[0]=2;
	}
	
        p+=tag_length;     	
    }

	if (tlvflag)
	{
			/* cbc@20100527:平台下发配置参数格式正常,应用失败,无需重启, */
		WMMP_LIB_DBG_TEMP("wmmpAppTLVs---write tlv 0x4010 = 2 ");
		mainContext_p->ControlContext.ExecuteParamOpt[0]=2;
	}
	else
	{
		/* cbc@20100527:平台下发配置参数格式正常,应用成功, */
		WMMP_LIB_DBG_TEMP("wmmpAppTLVs---write tlv 0x4010 = 0 ");
		mainContext_p->ControlContext.ExecuteParamOpt[0]=0;
	}

	   /*add cbc@20100526 :用来计算0X4017的值begin*/
	WMMP_LIB_DBG_TEMP("wmmpAppTLVs: 0x4017 crcinput len=%d",q_tlv -TLV_buffer );
	//WPI_hex_printf((char*)TLV_buffer,(u16)(q_tlv -TLV_buffer));
	CRC_value = CreateCRC32(TLV_buffer,q_tlv-TLV_buffer);
	WMMP_LIB_DBG_TEMP("wmmpAppTLVs: 0x4017 =0x%x",CRC_value );
	Writeu32(TLV_buffer,CRC_value);	
	WriteTLV(0x4017, 4, TLV_buffer);
	WMMP_LIB_DBG_TEMP("wmmpAppTLVs: 0x4017 CRC_value" );
	//WPI_hex_printf((char*)TLV_buffer,4);	
	   /*add cbc@20100526 :用来计算0X4017的值end*/
}


//关闭平台连接
void wmmpCloseSocket(void)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
    
        WMMP_LIB_DBG_TEMP("wmmpCloseSocket ");
    
	wmmpStartInterTimer(SOCKET_CLOSE_TIMEOUT_TIMER, 10);
	mainContext_p->enableSend=false;
	wmmpLibCloseConnect(mainContext_p->wmmpSocket);
}

void wmmpSendNextCommand(void)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	PDUContext_t   *pduContext = &mainContext_p->PDUContext_receive;  
		
      /*modified by cbc@20100416  begiin           */
      /**********************************************************************
      *                              cbc  20100416
      *存在这种情况， 例如终端发送SECURITY CONFIG 上报后,未收到 ACK 
      *，此时该定时器来临，把mainContext->StackState==WMMP_STACK_SERVER_WAITING 
      *的状态给修改了.
      ********************************************************************************/
      WMMP_LIB_DBG_TEMP("wmmpSendNextCommand():StackState =%d,dispartFlag=%d\n",mainContext_p->StackState,pduContext->dispartFlag);		
      if (mainContext_p->StackState==WMMP_STACK_SERVER_WAITING)
      	{
      		WMMP_LIB_DBG_TEMP("StackState==WMMP_STACK_SERVER_WAITING,waiting for the ack");
      		/* cbc@20100401:  2 秒后检查是否还有数据未发送 */
		wmmpStartInterTimer(PDU_INTERVAL_TIMER, 2);
      	}
	  /*CBC@20100419  :多包处理 */
	 else if((( mainContext_p->StackState ==WMMP_STACK_SERVER_CONTINUE)||
	 	( mainContext_p->StackState ==WMMP_STACK_CLIENT_CONTINUE))&&(pduContext->dispartFlag))
	 {
		WMMP_LIB_DBG_TEMP("wmmpSendNextCommand():dispart process1\n");		
	 
	 }
	  /*CBC@20100419  :多包处理 */
	 else
	{
		if (wmmpisInterTimerRun(SEND_PDU_TIMEOUT_TIMER)==true)
		{		
			wmmpStopInterTimer(SEND_PDU_TIMEOUT_TIMER);
			
			mainContext_p->maxPDUTimeOutCount=0;
			mainContext_p->ReSendTime = 0;
		}
		
		mainContext_p->FrameNo++;    
		 
	      	FreeWMMPPDUContext();/* cbc@20100402:   相当于协议层初始化或复位 */

		/* cbc@20100402: 临界区报文体处理,如果终端和平台同时主动发送报文,终端	先保存平台报文，然乎等待收到平台的回复后再处理由平台主动发送的报文*/
		if(mainContext_p->isCriticalUsed==true)
		{
			WMMP_LIB_DBG_TEMP("WMMP stack---send critical bufferr");
			DecodeCriticalBuffer();
		}
		else
		if(mainContext_p->CommandBufferCount_High!= 0)
		{
			/*两种情况: 1.短连接时插入的登录报文.  	 2. 登录以后的TRAP 报文*/

			WMMP_LIB_DBG_TEMP("send next Count_High command buffer cmdId=%d,Front %d,Count %d",mainContext_p->CommandInfoBuffer_High[mainContext_p->CommandBufferFront_High].CommandID,mainContext_p->CommandBufferFront_High,mainContext_p->CommandBufferCount_High);
                     WMMP_LIB_DBG_TEMP("wmmpSendNextCommand:Count_High protocol_status=0x%x",mainContext_p->Wmmp_status.protocol_status);
				/* modified by cbc@20100513:已登录就不要发送登录包, begin */
			if((mainContext_p->Wmmp_status.protocol_status == LOGINOK)&&(mainContext_p->CommandInfoBuffer_High[mainContext_p->CommandBufferFront_High].CommandID== LOGIN_COMMAND))
			{
				/* cbc@20100518:回滚重登录 */
				mainContext_p->PDUUsage = mainContext_p->CommandInfoBuffer_High[mainContext_p->CommandBufferFront_High].PDUUsage;
				WMMP_LIB_DBG_TEMP("wmmpSendNextCommand:Count_High PDUUsage=%d",mainContext_p->PDUUsage );
				//if ((mainContext_p->PDUUsage==WMMP_TLV_REBOOT) ||(mainContext_p->PDUUsage==WMMP_LOCAL_CONFIG_REBOOT)||(mainContext_p->PDUUsage==WMMP_SECURITY_REBOOT))
				/*bob modify 20101122 for remote_control_reboot*/
				if ((mainContext_p->PDUUsage==WMMP_TLV_REBOOT) ||(mainContext_p->PDUUsage==WMMP_LOCAL_CONFIG_REBOOT)||(mainContext_p->PDUUsage==WMMP_SECURITY_REBOOT) || (mainContext_p->PDUUsage==WMMP_REMOTE_CONTROL_REBOOT) || (mainContext_p->PDUUsage==WMMP_CONFIG_SET_APN_REBOOT) ||(mainContext_p->PDUUsage==WMMP_RMCON_REFACTORY_REBOOT))
				{
					WMMP_LIB_DBG_TEMP("wmmpSendNextCommand:Count_High loginok ,roll  login again");
					SendCommand(&mainContext_p->CommandInfoBuffer_High[mainContext_p->CommandBufferFront_High]);		
					memset(&mainContext_p->CommandInfoBuffer_High[mainContext_p->CommandBufferFront_High],0,sizeof(CommandInfo_t));		
					mainContext_p->CommandBufferFront_High = (mainContext_p->CommandBufferFront_High + 1)%MAX_COMMANDINFO_NUM;
					mainContext_p->CommandBufferCount_High--;		
				}
				else
				{
					WMMP_LIB_DBG_TEMP("wmmpSendNextCommand:Count_High loginok ,no send login again");
					mainContext_p->CommandBufferFront_High = (mainContext_p->CommandBufferFront_High + 1)%MAX_COMMANDINFO_NUM;
					mainContext_p->CommandBufferCount_High--;	
					wmmpStartInterTimer(PDU_INTERVAL_TIMER, 2);
				}
			}
			else
			{		
				SendCommand(&mainContext_p->CommandInfoBuffer_High[mainContext_p->CommandBufferFront_High]);		
				memset(&mainContext_p->CommandInfoBuffer_High[mainContext_p->CommandBufferFront_High],0,sizeof(CommandInfo_t));		
				mainContext_p->CommandBufferFront_High = (mainContext_p->CommandBufferFront_High + 1)%MAX_COMMANDINFO_NUM;
				mainContext_p->CommandBufferCount_High--;		
				WMMP_LIB_DBG_TEMP("send command buffer_High front %d,count %d",mainContext_p->CommandBufferFront_High,mainContext_p->CommandBufferCount_High);
			}
		}
		else
		if(mainContext_p->CommandBufferCount != 0)
		{		
			WMMP_LIB_DBG_TEMP("send next command buffer cmdId=%d,front %d,count %d",mainContext_p->CommandInfoBuffer[mainContext_p->CommandBufferFront].CommandID,mainContext_p->CommandBufferFront,mainContext_p->CommandBufferCount);
			WMMP_LIB_DBG_TEMP("wmmpSendNextCommand:protocol_status=0x%x",mainContext_p->Wmmp_status.protocol_status);
			/* modified by cbc@20100421:已登录就不要发送登录包, begin */
			if((mainContext_p->Wmmp_status.protocol_status == LOGINOK)&&(mainContext_p->CommandInfoBuffer[mainContext_p->CommandBufferFront].CommandID== LOGIN_COMMAND))
			{
				/* cbc@20100518:回滚重登录 */
				mainContext_p->PDUUsage = mainContext_p->CommandInfoBuffer[mainContext_p->CommandBufferFront].PDUUsage;
				WMMP_LIB_DBG_TEMP("wmmpSendNextCommand:PDUUsage=%d",mainContext_p->PDUUsage) ;
				//if ((mainContext_p->PDUUsage==WMMP_TLV_REBOOT) ||(mainContext_p->PDUUsage==WMMP_LOCAL_CONFIG_REBOOT)||(mainContext_p->PDUUsage==WMMP_SECURITY_REBOOT))
				/*bob modify 20101122 for remote_control_reboot*/
				if ((mainContext_p->PDUUsage==WMMP_TLV_REBOOT) ||(mainContext_p->PDUUsage==WMMP_LOCAL_CONFIG_REBOOT)||(mainContext_p->PDUUsage==WMMP_SECURITY_REBOOT) || (mainContext_p->PDUUsage==WMMP_REMOTE_CONTROL_REBOOT) || (mainContext_p->PDUUsage==WMMP_CONFIG_SET_APN_REBOOT) || (mainContext_p->PDUUsage==WMMP_RMCON_REFACTORY_REBOOT))
				{
					WMMP_LIB_DBG_TEMP("wmmpSendNextCommand:loginok ,roll  login again");
					SendCommand(&mainContext_p->CommandInfoBuffer[mainContext_p->CommandBufferFront]);		
					memset(&mainContext_p->CommandInfoBuffer[mainContext_p->CommandBufferFront],0,sizeof(CommandInfo_t));		
					mainContext_p->CommandBufferFront = (mainContext_p->CommandBufferFront + 1)%MAX_COMMANDINFO_NUM;
					mainContext_p->CommandBufferCount--;		
				}
				else
				{
					WMMP_LIB_DBG_TEMP("wmmpSendNextCommand:loginok ,no send login again");
					mainContext_p->CommandBufferFront = (mainContext_p->CommandBufferFront + 1)%MAX_COMMANDINFO_NUM;
					mainContext_p->CommandBufferCount--;	
					wmmpStartInterTimer(PDU_INTERVAL_TIMER, 2);
				}
			}
			else
			{				
				SendCommand(&mainContext_p->CommandInfoBuffer[mainContext_p->CommandBufferFront]);		
				memset(&mainContext_p->CommandInfoBuffer[mainContext_p->CommandBufferFront],0,sizeof(CommandInfo_t));		
				mainContext_p->CommandBufferFront = (mainContext_p->CommandBufferFront + 1)%MAX_COMMANDINFO_NUM;
				mainContext_p->CommandBufferCount--;		
				WMMP_LIB_DBG_TEMP("send command buffer Front %d,count %d",mainContext_p->CommandBufferFront,mainContext_p->CommandBufferCount);
			}
			/* modified by cbc@20100421:已登录就不要发送登录包, end */
		}
		else
		{
			WMMP_LIB_DBG_TEMP("WMMP stack---command buffer is empty");
		}
	}
          /*modified by cbc@20100416  end    */

}
void wmmpCreateSocketConnect(void)
{
	WmmpContext_t  *mainContext_p = ptrToWmmpContext();	
	WmmpSocketType_e type;
	u16               sockPort;
	WPIEventData_t WPIEventData;

	WMMP_LIB_DBG_TEMP("wmmpCreateSocketConnect SendLength %d",mainContext_p->PDUContext.SendLength);
	if(mainContext_p->PDUContext.SendLength!=0)
	{
		if((mainContext_p->ControlContext.ConnectMode==0)||(mainContext_p->ControlContext.ConnectMode==1))
		{
			sockPort = (u16)(mainContext_p->ConfigContext.M2MPort>>16);  
			type=WMMP_UDP_CONNECT;
		}
		else
		if((mainContext_p->ControlContext.ConnectMode==2)||(mainContext_p->ControlContext.ConnectMode==3))	
		{
			sockPort = (u16)(mainContext_p->ConfigContext.M2MPort&0xFFFF);
			type=WMMP_TCP_CONNECT;
		}		

		WMMP_LIB_DBG_TEMP("wmmpCreateSocketConnect type %d M2MIPAddr 0x%08x sockPort 0x%04x",type,mainContext_p->ConfigContext.M2MIPAddr,sockPort);
		WMMP_LIB_DBG_TEMP("wmmpCreateSocketConnect old wmmpSocket 0x%x",mainContext_p->wmmpSocket);
        /*bob add 20120209 for login idle -s */
        if(mainContext_p->ConfigContext.M2MIPAddr == 0)
        {
        }
        /*bob add 20120209 for login idle -e */
		if (mainContext_p->wmmpSocket ==CONNREF_INVALID)
		{
		mainContext_p->createConnectCallBack=NULL;/*保证回调为空*/
        //TODO:modigy for apn_type temp 130314
		WPI_CreateConnect(1,type,WMMP_CONNREF_WMMP, mainContext_p->ConfigContext.M2MIPAddr, sockPort);
		}
		WMMP_LIB_DBG_TEMP("wmmpCreateSocketConnect new wmmpSocket 0x%x ",mainContext_p->wmmpSocket);
		if(mainContext_p->WPIEventFucPtr!=PNULL)
		{
			WPIEventData.WPISocketCreateEvent.type= type;
			WPIEventData.WPISocketCreateEvent.socket = mainContext_p->wmmpSocket;
			(mainContext_p->WPIEventFucPtr)(WPI_SOCKET_CREATE_EVENT,&WPIEventData);

		}	
	}
}

void wmmpOthCreateSocketConnect(void)
{
	WmmpContext_t  *mainContext_p = ptrToWmmpContext();	
	WmmpSocketType_e type;
	u16               sockPort;
	WPIEventData_t WPIEventData;

	WMMP_LIB_DBG_TEMP("wmmpOthCreateSocketConnect ");
	
	if((mainContext_p->ControlContext.ConnectMode==0)||(mainContext_p->ControlContext.ConnectMode==1))
	{
		sockPort = (u16)(mainContext_p->ConfigContext.M2MPort>>16);  
		type=WMMP_UDP_CONNECT;
	}
	else
	if((mainContext_p->ControlContext.ConnectMode==2)||(mainContext_p->ControlContext.ConnectMode==3))	
	{
		sockPort = (u16)(mainContext_p->ConfigContext.M2MPort&0xFFFF);
		type=WMMP_TCP_CONNECT;
	}
	WMMP_LIB_DBG_TEMP("wmmpOthCreateSocketConnect type %d M2MIPAddr 0x%08x sockPort 0x%04x",type,mainContext_p->ConfigContext.M2MIPAddr,sockPort);
	WMMP_LIB_DBG_TEMP("wmmpOthCreateSocketConnect old wmmpSocket 0x%x ",mainContext_p->wmmpSocket);
	if (mainContext_p->wmmpSocket ==CONNREF_INVALID)
	{
		 mainContext_p->createConnectCallBack=NULL;/*保证回调为空*/
        //TODO:modigy for apn_type temp 130314
		WPI_CreateConnect(1, type, WMMP_CONNREF_WMMP,mainContext_p->ConfigContext.M2MIPAddr, sockPort);
	}
	WMMP_LIB_DBG_TEMP("wmmpOthCreateSocketConnect new wmmpSocket 0x%x ",mainContext_p->wmmpSocket);
	if(mainContext_p->WPIEventFucPtr!=PNULL)
	{
		WPIEventData.WPISocketCreateEvent.type= type;
		WPIEventData.WPISocketCreateEvent.socket = mainContext_p->wmmpSocket;
		(mainContext_p->WPIEventFucPtr)(WPI_SOCKET_CREATE_EVENT,&WPIEventData);
	}	
	
}


void wmmpProcessPDUIntervalTimer(void)
{
    WMMP_LIB_DBG_TEMP("wmmpProcessPDUIntervalTimer ");

	wmmpSendNextCommand();
	
}

void wmmpProcessSocketCloseTimer(void)
{
	
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	mainContext_p->wmmpSocket= CONNREF_INVALID;

	WMMP_LIB_DBG_TEMP("wmmpProcessSocketCloseTimer");

	wmmpLibDectiveGprs();	
}



void  registerTrapAbortWarn(u8 AlarmStatus,u8	AlarmType,u16	AlarmCode)
{
	WmmpTermStatusContext_t  *StatusContext_p = &(ptrToWmmpContext()->StatusContext);
	
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();	
	PDUContext_t   *pduContext = &mainContext_p->PDUContext;  
	CommandInfo_t CommandInfo;
	u16* tags;
	WPIEventData_t WPIEventData;
	bool TransBySMS = false;	

	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	WMMP_LIB_DBG_TEMP("registerTrapAbortWarn");
	wmmpStopInterTimer(PDU_INTERVAL_TIMER);
	RollClearCommandBuffer();
	FreeWMMPPDUContext();
	
	mainContext_p->StatusContext.AlarmStatus =AlarmStatus;// 1
	mainContext_p->StatusContext.AlarmType=AlarmType;  // 1
	mainContext_p->StatusContext.AlarmCode=AlarmCode;  // 3
	CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
	CommandInfo.SecuritySet = 0;
	
	tags = (u16*)WPI_Malloc( sizeof(u16)*4);
	tags[0]=0x300B;
	tags[1]=0x300C;
	tags[2]=0x300D;

	CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=3;
	if (AlarmStatus == 0)
	{
		tags[3]=0x300E;
		CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=4;
	}
	else
	{	/*记住异常类型 */
	      memset((char*)StatusContext_p->RestoreAlarm,0,20);
		//strcpy(StatusContext_p->RestoreAlarm,(char*)("0003"));/*bob modify 20111220 from 0004 to 0003*/
        strcpy(StatusContext_p->RestoreAlarm,(char*)("0004"));
	}
	
	CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=tags;	
#if defined(GUIZHOU_WMMP)
	CommandInfo.TransType = WMMP_TRANS_GPRS;
#else
	CommandInfo.TransType = WMMP_TRANS_DEFAULT;//WMMP_TRANS_SMS;	 //use SMS !
	CommandInfo.TransType = WMMP_TRANS_SMS;	 //use SMS !
#endif	
	SaveToCommandBuffer(&CommandInfo);

}

void  ActiveGprsTrapAbortWarn(u8 AlarmStatus,u8	AlarmType,u16	AlarmCode)
{
	WmmpTermStatusContext_t  *StatusContext_p = &(ptrToWmmpContext()->StatusContext);
	
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();	
	PDUContext_t   *pduContext = &mainContext_p->PDUContext;  
	CommandInfo_t CommandInfo;
	u16* tags;
	WPIEventData_t WPIEventData;
	bool TransBySMS = false;	
	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	
	WMMP_LIB_DBG_TEMP("ActiveGprsTrapAbortWarn");
	//wmmpStopInterTimer(PDU_INTERVAL_TIMER);
	RollClearCommandBuffer();
	FreeWMMPPDUContext();
	
	mainContext_p->StatusContext.AlarmStatus =AlarmStatus;// 1
	mainContext_p->StatusContext.AlarmType=AlarmType;  // 1
	mainContext_p->StatusContext.AlarmCode=AlarmCode;  // 3
	CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
	CommandInfo.SecuritySet = 0;
	
	tags = (u16*)WPI_Malloc( sizeof(u16)*4);
	tags[0]=0x300B;
	tags[1]=0x300C;
	tags[2]=0x300D;

	CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=3;
	if (AlarmStatus == 0)
	{
		tags[3]=0x300E;
		CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=4;
	}
	else
	{	/*记住异常类型 */
	      memset((char*)StatusContext_p->RestoreAlarm,0,20);
		//strcpy(StatusContext_p->RestoreAlarm,(char*)("0003"));/*bob modify 20111220 from 0004 to 0003*/
		strcpy(StatusContext_p->RestoreAlarm,(char*)("0004"));
	}
	
	CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=tags;	
#if defined(GUIZHOU_WMMP)
	CommandInfo.TransType = WMMP_TRANS_GPRS;
#else	
	CommandInfo.TransType = WMMP_TRANS_DEFAULT;//WMMP_TRANS_SMS;	 //use SMS !
	CommandInfo.TransType = WMMP_TRANS_SMS;	 //use SMS !
#endif	
	SaveToCommandBuffer(&CommandInfo);

}

void wmmpProcessRegisterTimer(void)///* add by cbc@20100522 for 注册等待平台下发密码超时 */
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();	
    
//	WPIRtcDateAndTime dateAndTime;
//	WPIRtcStatus_e      rtcStatus;
//	u32	curuTime = 0;

	WMMP_LIB_DBG_TEMP("wmmpProcessRegisterTimer maxRegSecuSmsTimeOutCount %d",mainContext_p->maxRegSecuSmsTimeOutCount);	

	wmmpStopInterTimer(SEND_PDU_TIMEOUT_TIMER);
	wmmpStopInterTimer(TIMING_REGISTER_TIMER);
	wmmpStopInterTimer(TIMING_LOGIN_TIMER);
	wmmpStopInterTimer(TRAPABORTWARN_TIIMER);

        /*chb: if not stop the timer it will result that the modem run registerTrapAbortWarn for ever*/
        stopRegisterForKeyTimer();
    
	mainContext_p->maxRegSecuSmsTimeOutCount++;
    
	//registerTrapAbortWarn(1,2,0x1c);
	registerTrapAbortWarn(1,3,0x1c);   

/*chb:  it will result that the modem run registerTrapAbortWarn for ever*/
#if 0    
	if(mainContext_p->maxRegSecuSmsTimeOutCount >= 3)
	{    
		mainContext_p->maxRegSecuSmsTimeOutCount = 0;	
            /*register again*/
		wmmpStartInterTimer(TIMING_REGISTER_TIMER, 90);			
	}
	else
	{    
		startRegisterForKeyTimer();
		wmmpStartInterTimer(TIMING_LOGIN_TIMER, 90);	
	}
#else 
	if(mainContext_p->maxRegSecuSmsTimeOutCount >= 2)
	{    
		mainContext_p->maxRegSecuSmsTimeOutCount = 0;	

            /*local warning*/
            WPI_send_urc(WMMP_AT_EVENT_STR"%s", "\"register failed: not received security sms\"");

            /*one hour later, tyr to register again*/
            wmmpStartInterTimer(TIMING_REGISTER_TIMER, 1*60*60);			
	}
	else
	{    
            /*register again*/
		wmmpStartInterTimer(TIMING_REGISTER_TIMER, 90);			
	}
#endif //0        
		
}

void wmmpProcessConnectTimer(void)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();

	
	WMMP_LIB_DBG_TEMP("wmmpProcessConnectTimer");
	if(CONNREF_INVALID == mainContext_p->wmmpSocket)
	{
	   // wmmpSendIpShutdownReq(SIMCOM_WMMP_TASK_ID);
	   
		wmmpLibDectiveGprs();	
	}
	else
	{
		wmmpCloseSocket();
	    //closesocket(mainContext_p->wmmpSocket);
	}
	wmmpProcessError();	
	//wmmpSendNextCommand();
}

/* *******************************************************************
*        cbc@20100424:  终端异常告警TRAP 上报
*
*       0x300b:   0-- 正常， 1 -- 告警
*       0x300c:  1--通讯告警,  2 -- 硬件告警,3 -- 软件告警
*       0x300d:  1,2,3,
************************************************************************/
void wmmpTrapAbortWarn(u8 AlarmStatus,u8	AlarmType,u16	AlarmCode)
{
	WmmpTermStatusContext_t  *StatusContext_p = &(ptrToWmmpContext()->StatusContext);
	
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();	
	PDUContext_t   *pduContext = &mainContext_p->PDUContext;  
	CommandInfo_t CommandInfo;
	u16* tags;
	WPIEventData_t WPIEventData;
	bool TransBySMS = false;	
	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	WMMP_LIB_DBG_TEMP("wmmpTrapAbortWarn");
	mainContext_p->StatusContext.AlarmStatus =AlarmStatus;// 1
	mainContext_p->StatusContext.AlarmType=AlarmType;  // 1
	mainContext_p->StatusContext.AlarmCode=AlarmCode;  // 3
	CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
	CommandInfo.SecuritySet = 0;
	
	tags = (u16*)WPI_Malloc( sizeof(u16)*4);
	tags[0]=0x300B;
	tags[1]=0x300C;
	tags[2]=0x300D;

	CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=3;
	if (AlarmStatus == 0)
	{
		tags[3]=0x300E;
		CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=4;
	}
	else
	{	/*记住异常类型 */
	      memset((char*)StatusContext_p->RestoreAlarm,0,20);
		//strcpy(StatusContext_p->RestoreAlarm,(char*)("0003"));/*bob modify 20111220 from 0004 to 0003*/
		strcpy(StatusContext_p->RestoreAlarm,(char*)("0004"));
	}
	
	CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=tags;	
#if defined(GUIZHOU_WMMP)
	CommandInfo.TransType = WMMP_TRANS_GPRS;
#else	
	CommandInfo.TransType = WMMP_TRANS_DEFAULT;//WMMP_TRANS_SMS;	 //use SMS !
	CommandInfo.TransType = WMMP_TRANS_SMS;	 //use SMS !
#endif	
	SaveToCommandBuffer(&CommandInfo);	
}

void wmmpProcessPDUTimer(void)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();	
	PDUContext_t   *pduContext = &mainContext_p->PDUContext;  
	CommandInfo_t CommandInfo;
	u16* tags;
	WPIEventData_t WPIEventData;
	bool TransBySMS = false;

	WMMP_LIB_DBG_TEMP("wmmpProcessPDUTimer,ReSendTime =%d StackState %d",mainContext_p->ReSendTime,mainContext_p->StackState);

	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	wmmpStopInterTimer(SEND_PDU_TIMEOUT_TIMER);
	wmmpStopInterTimer(TIMING_REGISTER_TIMER);	
	wmmpStopInterTimer(TIMING_LOGIN_TIMER);	
	wmmpStopInterTimer(TRAPABORTWARN_TIIMER);	
	stopRegisterForKeyTimer();

	/* cbc@20100518: 重发3次,   短信不需要重发机制*/
	if((mainContext_p->ReSendTime==2)||(TransBySMS==true))
	{
		WMMP_LIB_DBG_TEMP("wmmpProcessPDUTimer 1 clear ReSendTime 0 ");
		mainContext_p->ReSendTime=0;
	} 
	else
	{
		mainContext_p->ReSendTime++;
		mainContext_p->FrameNo++;    
		WMMP_LIB_DBG_TEMP("wmmpProcessPDUTimer resend ReSendTime:%d",mainContext_p->ReSendTime);
		ResendCommand();
		return;
	}

	if(TransBySMS==false)
    {		
        WMMP_LIB_DBG_TEMP("wmmpProcessPDUTimer :mainContext_p->PDUUsage=%d",mainContext_p->PDUUsage);
        if(mainContext_p->isCriticalUsed)
        {
            WMMP_LIB_DBG_TEMP("wmmpProcessPDUTimer isCriticalUsed true");		 
            FreeWMMPPDUContext();		
            wmmpSendNextCommand();
            return;
        }
        else
        {		
            PDUContext_t   *pduContext = &mainContext_p->PDUContext;  
            WPI_send_urc(WMMP_AT_EVENT_STR"CommandID %d %s", pduContext->CommandID, " not receive ack 3 times");

            wmmpProcessErrorRoll();
        }
    }
	else
	{
		WMMP_LIB_DBG_TEMP("wmmpProcessPDUTimer 	5");
		wmmpSendNextCommand();
	}


	WMMP_LIB_DBG_TEMP("wmmpProcessPDUTimer:protocol_status = 0x%x",mainContext_p->Wmmp_status.protocol_status);


	/*cbc@20100518:  回滚IP地址端口短信中心等重要参数begin  */
	if(mainContext_p->M2MIPAddr_b!=0)
	{
		WMMP_LIB_DBG_TEMP("wmmpProcessPDUTimer 	6\n");
		mainContext_p->ConfigContext.M2MIPAddr=mainContext_p->M2MIPAddr_b;
		mainContext_p->M2MIPAddr_b=0;
	}
	if(mainContext_p->M2MPort_b!=0)
	{
		WMMP_LIB_DBG_TEMP("wmmpProcessPDUTimer 7\n");
		mainContext_p->ConfigContext.M2MPort = mainContext_p->M2MPort_b;
		mainContext_p->M2MPort_b=0;
	}
	if(mainContext_p->AppSMSCentreNum_b[0]!=0)
	{
		WMMP_LIB_DBG_TEMP("wmmpProcessPDUTimer 	8\n");
		memcpy(mainContext_p->ConfigContext.AppSMSCentreNum,mainContext_p->AppSMSCentreNum_b,sizeof(mainContext_p->AppSMSCentreNum_b));
		memset(mainContext_p->AppSMSCentreNum_b,0,sizeof(mainContext_p->AppSMSCentreNum_b));	
	}       

	if(mainContext_p->uplinkPassword_b[0]!=0)
	{		
		memcpy(mainContext_p->SecurityContext.uplinkPassword,mainContext_p->uplinkPassword_b,sizeof(mainContext_p->uplinkPassword_b));
		WMMP_LIB_DBG_TEMP("wmmpProcessPDUTimer:uplinkPassword=%s,uplinkPassword_b=%s\n",mainContext_p->SecurityContext.uplinkPassword,mainContext_p->uplinkPassword_b);
		memset(mainContext_p->uplinkPassword_b,0,sizeof(mainContext_p->uplinkPassword_b));	
	}
	if(mainContext_p->downlinkPassword_b[0]!=0)
	{		
		memcpy(mainContext_p->SecurityContext.downlinkPassword,mainContext_p->downlinkPassword_b,sizeof(mainContext_p->downlinkPassword_b));
		WMMP_LIB_DBG_TEMP("wmmpProcessPDUTimer:uplinkPassword=%s,uplinkPassword_b=%s\n",mainContext_p->SecurityContext.downlinkPassword,mainContext_p->downlinkPassword_b);
		memset(mainContext_p->downlinkPassword_b,0,sizeof(mainContext_p->downlinkPassword_b));	
	}
	if(mainContext_p->publickey_b[0]!=0)
	{		
		memcpy(mainContext_p->SecurityContext.publickey,mainContext_p->publickey_b,sizeof(mainContext_p->publickey_b));
		WMMP_LIB_DBG_TEMP("wmmpProcessPDUTimer:uplinkPassword=%s,uplinkPassword_b=%s\n",mainContext_p->SecurityContext.publickey,mainContext_p->publickey_b);
		memset(mainContext_p->publickey_b,0,sizeof(mainContext_p->publickey_b));	
	}       
	/*cbc@20100518:  回滚IP地址端口短信中心等重要参数end  */

	/* cbc@20100421: 恢复最新 可用 IP 等接入参数后重新登录 ,begin*/
	WriteAllTLVParam2Flash();//save all TLV

		/*cbc@20100518:继续发送下一个命令 */
		
	WMMP_LIB_DBG_TEMP("wmmpProcessPDUTimer E PDUUsage=%d\n",mainContext_p->PDUUsage);
	
}

void wmmpProcessHeartTimer(void)
{
	CommandInfo_t CommandInfo;
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();	

	WMMP_LIB_DBG_TEMP("wmmpProcessHeartTimer heart beat,%d\n",mainContext_p->ConfigContext.Period); 
	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	CommandInfo.CommandID = HEART_BEAT_COMMAND;
	CommandInfo.SecuritySet = 0;
	CommandInfo.TransType = WMMP_TRANS_DEFAULT;
	SaveToCommandBuffer(&CommandInfo);
	wmmpStartInterTimer(HEART_BEAT_TIMER, mainContext_p->ConfigContext.Period);

}

void wmmpProcessShortConnectTimer(void)
{
	
	//short connect, auto login out
	CommandInfo_t CommandInfo;
    
	WMMP_LIB_DBG_TEMP("wmmpProcessShortConnectTimer");
    
	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	CommandInfo.CommandID = LOGOUT_COMMAND;
	CommandInfo.SecuritySet = 0;
	CommandInfo.TransType = WMMP_TRANS_DEFAULT;
	CommandInfo.CommandExInfo.LoginOutInfo.reason=0;
	CommandInfo.PDUUsage = WMMP_SHORT_CONNECT;
	SaveToCommandBuffer(&CommandInfo);	
	//KiStopTimer(&mainContext_p->ShortConnectModeTimer);
	//wmmpStopInterTimer(SHORT_CONNECT_MODE_TIMER);
	WMMP_LIB_DBG_TEMP("LOGOUT_COMMAND\n");
}

void wmmpProcessSigmaStatTimer(void)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();	
	WMMP_LIB_DBG_TEMP("wmmpProcessSigmaStatTimer");
    
	SigmaStatTrap(WMMP_PDU_TRAP_SIGMA_STAT);
	//get TLV2003
	wmmpStartInterTimer(SIGMA_STAT_TIMER, mainContext_p->StatisticContext.PeriodReport);

}

void wmmpProcessStatTimer(void)
{
	WMMP_LIB_DBG_TEMP("wmmpProcessStatTimer");

	 //检查统计时间
	 CheckStatTime();	
	 wmmpStartInterTimer(STAT_TIMER, 120);		 
}


void wmmpProcessTrapAbortWarnTimer(void)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	WPIRtcDateAndTime dateAndTime;
	WPIRtcStatus_e      rtcStatus;
	u32	curuTime = 0;
	u16	loopi = 0;
	WMMP_LIB_DBG_TEMP("wmmpProcessTrapAbortWarnTimer maxTrapAbortWarn %d",mainContext_p->maxTrapAbortWarn);
	WMMP_LIB_DBG_TEMP("  StackState %d ",mainContext_p->StackState);
	WMMP_LIB_DBG_TEMP("  protocol_status %d ",mainContext_p->Wmmp_status.protocol_status);


	mainContext_p->Wmmp_status.protocol_status =  PIDLE;
	wmmpStopInterTimer(SEND_PDU_TIMEOUT_TIMER);
	wmmpStopInterTimer(TRAPABORTWARN_TIIMER);
	stopRegisterForKeyTimer();
	if(mainContext_p->isCriticalUsed)
	{
		WMMP_LIB_DBG_TEMP("wmmpProcessTrapAbortWarnTimer isCriticalUsed true");		 
		mainContext_p->maxTrapAbortWarn = 0;
		FreeWMMPPDUContext();		
		//DecodeCriticalBuffer();
		wmmpSendNextCommand();
		return;
	}
	
	//wmmpTrapAbortWarn(1,3,0x1c);   /*bob remove 20120206*/
	
	mainContext_p->maxTrapAbortWarn++;
	if(mainContext_p->maxTrapAbortWarn >= 3)  
	{
		WMMP_LIB_DBG_TEMP("wmmpProcess be ReLogin TrapAbortWarnTimer  StackState %d ",mainContext_p->StackState);
		RollClearCommandBuffer();  
		FreeWMMPPDUContext();
		WMMP_LIB_DBG_TEMP("wmmpProcessTrapAbortWarnTimer 2  protocol_status %d ",mainContext_p->Wmmp_status.protocol_status);

		//mainContext_p->maxTrapAbortWarn=0;  /*bob remove 20120206*/
		curuTime = WPI_GetCurrentTime(&dateAndTime, &rtcStatus);	
		WMMP_LIB_DBG_TEMP("wmmpProcessRegisterTimer be ReLogin curuTime %d autoLoginuTime %d",curuTime,mainContext_p->autoLoginuTime);
		mainContext_p->autoLoginuTime = curuTime + 24*60*60;
		WMMP_LIB_DBG_TEMP("wmmpProcessRegisterTimer be ReLogin autoLoginuTime %d",mainContext_p->autoLoginuTime);
		//WPI_StopTimer();  /*bob remove 20101207*/
		/*bob add 20110215 for debug -s*/		
		for(loopi = 0;loopi < MAX_WMMP_APP_NUM;loopi++)
		{
			WMMP_LIB_DBG_TEMP("wmmpProcessTrapAbortWarnTimer be enter autoLogin appnum %d wmmpOpenSkt 0x%x  m2mHasAppDataFlag %d app_status %d" ,loopi,mainContext_p->wmmpOpenSkt[loopi],m2mHasAppDataFlag[loopi],mainContext_p->Wmmp_status.app_status[loopi]);
		}
		/*bob add 20110215 for debug -e*/
		
	}
	else
	{		
	    wmmpTrapAbortWarn(1,3,0x1c); /*bob add 20120206*/
		wmmpStartInterTimer(TRAPABORTWARN_TIIMER, 90);	
		mainContext_p->StackState = WMMP_STACK_INIT; /*bob add 20101229*/
	}  
	

}

void wmmpProcessTimingRegisterTimer(void)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	WMMP_LIB_DBG_TEMP("wmmpProcessTimingRegisterTimer maxRegisterTimeOutCount %d",mainContext_p->maxRegisterTimeOutCount);
	WMMP_LIB_DBG_TEMP("  StackState %d ",mainContext_p->StackState);
	WMMP_LIB_DBG_TEMP("  protocol_status %d ",mainContext_p->Wmmp_status.protocol_status);


	mainContext_p->Wmmp_status.protocol_status =  PIDLE;
	wmmpStopInterTimer(SEND_PDU_TIMEOUT_TIMER);
	wmmpStopInterTimer(TIMING_REGISTER_TIMER);
	//RollClearCommandBuffer();
	//FreeWMMPPDUContext();
	if(mainContext_p->isCriticalUsed)
	{
		WMMP_LIB_DBG_TEMP("wmmpProcessTimingRegisterTimer isCriticalUsed true");		 
		mainContext_p->maxRegisterTimeOutCount=0;
		FreeWMMPPDUContext();		
		//DecodeCriticalBuffer();
		wmmpSendNextCommand();
		return;
	}	
	wmmpRegisterReq(WMMP_TRANS_DEFAULT);	
	mainContext_p->maxRegisterTimeOutCount++;
	if(mainContext_p->maxRegisterTimeOutCount >= 3)  
	{
		WMMP_LIB_DBG_TEMP("wmmpProcessTimingRegisterTimer  2 StackState %d ",mainContext_p->StackState);
		WMMP_LIB_DBG_TEMP("wmmpProcessTimingRegisterTimer  2 protocol_status %d ",mainContext_p->Wmmp_status.protocol_status);
		mainContext_p->maxRegisterTimeOutCount=0;
		wmmpStartInterTimer(TRAPABORTWARN_TIIMER, 90);
	}
	else
	{			
		wmmpStartInterTimer(TIMING_REGISTER_TIMER, 90);					
	} 
	 
}


void wmmpProcessTimingLoginTimer(void)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	WMMP_LIB_DBG_TEMP("wmmpProcessTimingLoginTimer LoginFaildCount %d MaxLoginRetryTimes %d",mainContext_p->LoginFaildCount,mainContext_p->ConfigContext.MaxLoginRetryTimes);
	WMMP_LIB_DBG_TEMP("  StackState %d ",mainContext_p->StackState);
	WMMP_LIB_DBG_TEMP("  protocol_status %d ",mainContext_p->Wmmp_status.protocol_status);

	mainContext_p->Wmmp_status.protocol_status =  REGISTERSUCCUSSFUL;
	wmmpStopInterTimer(SEND_PDU_TIMEOUT_TIMER);
	wmmpStopInterTimer(TIMING_LOGIN_TIMER);
	//RollClearCommandBuffer();
	//FreeWMMPPDUContext();
	if(mainContext_p->isCriticalUsed)
	{
		WMMP_LIB_DBG_TEMP("wmmpProcessTimingRegisterTimer isCriticalUsed true");		 
		mainContext_p->LoginFaildCount = 0;
		FreeWMMPPDUContext();		
		//DecodeCriticalBuffer();
		wmmpSendNextCommand();
		return;
	}
	wmmpLoginReq(WMMP_TRANS_DEFAULT);
	
	mainContext_p->LoginFaildCount++;
	if(mainContext_p->LoginFaildCount >= mainContext_p->ConfigContext.MaxLoginRetryTimes)
	{	
		
		WMMP_LIB_DBG_TEMP("wmmpProcessTimingLoginTimer 2 StackState %d ",mainContext_p->StackState);
		WMMP_LIB_DBG_TEMP("wmmpProcessTimingLoginTimer  2 protocol_status %d ",mainContext_p->Wmmp_status.protocol_status);
		mainContext_p->LoginFaildCount = 0;
        
            /*chongqing demand local warning and to register after an hour*/  
//		wmmpStartInterTimer(TIMING_REGISTER_TIMER, 90);	
            /*local warning*/
            WPI_send_urc(WMMP_AT_EVENT_STR"%s", "\"LOGIN failed 3 times\"");
		wmmpStartInterTimer(TIMING_REGISTER_TIMER, 1*60*60);	
	}
	else
	{				
		wmmpStartInterTimer(TIMING_LOGIN_TIMER, 90);		
	} 
	 
}

void wmmpProcessServerLogoutReq(u8 reason)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	CommandInfo_t CommandInfo;  
	WPIEventData_t WPIEventData;

	WMMP_LIB_DBG_TEMP("wmmpProcessServerLogoutReq");
    
	//由服务器发起的登出，目前的发起的唯一原因为密码有效期过期
	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	CommandInfo.CommandID = LOGOUT_ACK_COMMAND;
	CommandInfo.SecuritySet = 0;
	CommandInfo.TransType = WMMP_TRANS_DEFAULT;
	SendCommand(&CommandInfo);

	if(mainContext_p->WPIEventFucPtr!=PNULL)
	{
		WPIEventData.WPIStatusIndEvent.type = 1;
		WPIEventData.WPIStatusIndEvent.value= PIDLE;	
		WPIEventData.WPIStatusIndEvent.reason = WMMP_KEY_OUT_OF_DATE;/* 	CBC@密码过期,跟流程无关*/
		(mainContext_p->WPIEventFucPtr)(WPI_STATUS_IND_EVENT,&WPIEventData);	
	}			
	//login again
	if(reason==8)
	{
		WMMP_LIB_DBG_TEMP("logout reason=%d\n",reason);
		
		CommandInfo.CommandID = LOGIN_COMMAND;
		CommandInfo.SecuritySet = 0;
		CommandInfo.TransType = WMMP_TRANS_DEFAULT;
		CommandInfo.PDUUsage = WMMP_KEY_OUT_OF_DATE;/*  不会有状态上报	CBC@这个有协议栈操作，不会与MMI 交互，*/
		SaveToCommandBuffer(&CommandInfo);
	}

}
u8 urcSecurityFlag = 0;
void wmmpProcessRegAck(u8 result)
{
	CommandInfo_t CommandInfo;  
	TLVRetVal_t TLVret;
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	u16 *TLV;	

	WMMP_LIB_DBG_TEMP("wmmpProcessRegAck result =%d", result);

	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	//REGISTER ACK
	switch(result)
	{
	   	//case 0x07:   /*bob remove 20101201 */
      		//case 0x08:  /*bob remove 20101201 */
		case 0x00:
		{
			WMMP_LIB_DBG_TEMP("Deal with Register ack result =%d\n",result);
			wmmpStopInterTimer(TIMING_REGISTER_TIMER);
			ReadTLV(0xE012,&TLVret);
			WriteTLV(0xE014,TLVret.length,TLVret.addr);
          		WriteTLV(0x3003,TLVret.length,TLVret.addr);          
			mainContext_p->Wmmp_status.protocol_status = REGISTERSUCCUSSFUL;
			//WPI_WriteNvram(WMMP_TLV_SECURITY);
			break;
		}

		case 0x09:
		case 0x01:
		{
			WMMP_LIB_DBG_TEMP("Deal with Register ack result =%d\n",result);
			wmmpStopInterTimer(TIMING_REGISTER_TIMER);
			ReadTLV(0xE012,&TLVret);
			WriteTLV(0xE014,TLVret.length,TLVret.addr);				
          		WriteTLV(0x3003,TLVret.length,TLVret.addr);
			mainContext_p->Wmmp_status.protocol_status = REGISTERSUCCUSSFUL;
			CommandInfo.CommandID = LOGIN_COMMAND;
			//CommandInfo.SecuritySet = 224;	
			CommandInfo.SecuritySet = 0;
			CommandInfo.TransType = WMMP_TRANS_DEFAULT;
			CommandInfo.PDUUsage = WMMP_PDU_ATCOMMAND;
			SaveToCommandBuffer(&CommandInfo);
			//WPI_WriteNvram(WMMP_TLV_SECURITY);			
			break;
		}

		/*  addd by cbc@20100413: 注册映射关系失败,改为短信重新注册   begin  */
		case 0x02:  /* 注册失败要求改为短信重新注册 */
		{
			WmmpContext_t *mainContext_p = ptrToWmmpContext();
			//mainContext_p->ControlContext.ConnectMode = 4; /*bob remove 20101201*/ /* 短信注册 模式*/
			
			WMMP_LIB_DBG_TEMP("Deal with Register ack result =%d\n",result);
			if(mainContext_p->maxRegisterTimeOutCount >= 3)  /*bob add 20101201*/
			{		
#if defined(GUIZHOU_WMMP)
				wmmpRegisterReq(WMMP_TRANS_GPRS);
#else
				wmmpRegisterReq(WMMP_TRANS_SMS);		/* 重新用短信注册 */   /*bob remove 20111125 for guizhou*/
#endif
			}			
			break;
		}
		/*  addd by cbc@20100413: 注册映射关系失败,改为短信重新注册   end  */

		default:
		{
			//wmmpProcessTimingRegisterTimer();	/*bob add 20101228*/
			WMMP_LIB_DBG_TEMP("Deal none with deault Register ack result =%d",result);
			break;
		}
	}
}

/************************************************************************
*      cbc@20100424:
*
*   终端在未登录平台前先修改配置的参数, 之后一登录就上报本地参数
*  修改更新通知给服务器,向M2M平台登录后，用CONFIG_TRAP和TLV 0x4011
*	上报其配置参数
*		该命令和 AT%M2MCFG=N ， 配合使用,先更改参数后后重起
******************************************************************************************/
void wmmpLocalCfgNotify(void)
{
	g_localCfgFlag = 1;
}
u8 wmmpGetLocalCfgFlag(void)
{
return g_localCfgFlag;
}
void wmmpClearLocalCfgFlag(void)
{
    g_localCfgFlag = 0;
}



void wmmpProcessLoginAck(u8 result)
{
	CommandInfo_t CommandInfo;  
	TLVRetVal_t TLVret;
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	u16 *TLV;
	WPIEventData_t WPIEventData;
   	u16  loopi = 0;

	
	WMMP_LIB_DBG_TEMP("wmmpProcessLoginAck mainContext->PDUUsage %d CommandInfo->PDUUsage %d",mainContext_p->PDUUsage,CommandInfo.PDUUsage);
	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	WMMP_LIB_DBG_TEMP("wmmpProcessLoginAck result = %d",result);
		//login ACK
        if (1==result || 0==result || 8==result)
        {
                WPI_send_urc(WMMP_AT_EVENT_STR"%d,%s %s", WMMP_AET_LIB_STATE, "0401", "\"Logged-in:login-trap with TLVs synchronization\"");

                if (mainContext_p->Wmmp_status.register_status == WMMP_RECV_SECU_SMS)
                {
                    mainContext_p->Wmmp_status.register_status = WMMP_REGISTER_COMPLETE;
                    WPI_WriteNvram(NVRAM_EF_WMMP_TLV_SECURITY_LID,(WmmpSecurityContext_t*)WPA_ptrToSecurityTLV(), sizeof(WmmpSecurityContext_t));
                    WPI_WriteNvram((u16)NVRAM_EF_WMMP_TLV_STATUS_LID,(WmmpTermStatusContext_t *)WPA_ptrToStatusTLV,sizeof(WmmpTermStatusContext_t));
                }
        }
        else
        {
                WPI_send_urc(WMMP_AT_EVENT_STR"%s, %d", "Login failed: result is", result);
        }
        
    switch(result)
    {
		case 1:
      		{        
			u8 i;
			char *TLVaddr=PNULL;
			mainContext_p->Wmmp_status.protocol_status = LOGINOK;
			
			ReadTLV(0x0025,&TLVret);
          		TLVret.length = TLVret.length+2;
			TLV = (u16*)WPI_Malloc(TLVret.length);
			//KiAllocZeroMemory(TLVret.length, (void**) &TLV);
			TLVaddr = TLVret.addr;
			TLV[0] = 0x0025;
			WMMP_LIB_DBG_TEMP("1 ProcessLoginAck:,TLVSyncTagNum=%d\n",mainContext_p->CustomContext.TLVSyncTagNum);
			             
			for(i=1;i<TLVret.length/2;i++)
			{
				TLV[i]= Readu16(TLVaddr);
				TLVaddr+=2;
			}

			mainContext_p->LoginFaildCount=0;

			CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
			CommandInfo.SecuritySet = 0;
			CommandInfo.TransType = WMMP_TRANS_DEFAULT;
			CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=TLV;
			CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=mainContext_p->CustomContext.TLVSyncTagNum+1;   
			SaveToHighCommandBuffer(&CommandInfo);  //use high buffer!


			/* add by cbc@20100424: 本地参数配置上报通知标识,M2M终端向平台上报begin*/
			WMMP_LIB_DBG_TEMP("1 login OK!PDUUsage=%d,localcfg=%d\n",mainContext_p->PDUUsage,wmmpGetLocalCfgFlag());
			if ((mainContext_p->PDUUsage == WMMP_LOCAL_CONFIG_REBOOT)||(wmmpGetLocalCfgFlag()))
			{				
				wmmpClearLocalCfgFlag();
				WPA_LocalCfgReq(NULL,0);				
			}
			/* add by cbc@20100424: 本地参数配置上报通知标识,M2M终端向平台上报end*/
			
			
			//long connect need heart beat
			if((mainContext_p->ControlContext.ConnectMode==3)||(mainContext_p->ControlContext.ConnectMode==1))
			{			
				WMMP_LIB_DBG_TEMP("1 login OK! start heart beat timer Period %d",mainContext_p->ConfigContext.Period);
				wmmpStopInterTimer(HEART_BEAT_TIMER);
				wmmpStartInterTimer(HEART_BEAT_TIMER, mainContext_p->ConfigContext.Period);
			}
			wmmpStartInterTimer(STAT_TIMER, 120);			

			//login ok ,so clear the back up param
			mainContext_p->M2MIPAddr_b=0;
			mainContext_p->M2MPort_b=0;
			memset(mainContext_p->AppSMSCentreNum_b,0,sizeof(mainContext_p->AppSMSCentreNum_b));

			/*cbc@20100518 安全参数 */
			memset(mainContext_p->uplinkPassword_b,0,sizeof(mainContext_p->uplinkPassword_b));
			memset(mainContext_p->downlinkPassword_b,0,sizeof(mainContext_p->downlinkPassword_b));
			memset(mainContext_p->publickey_b,0,sizeof(mainContext_p->publickey_b));

			//login ok , no alarm now
			mainContext_p->StatusContext.AlarmStatus =0;
			mainContext_p->StatusContext.AlarmType=0;
			mainContext_p->StatusContext.AlarmCode=0;

			//login ok, clear para is 0
			mainContext_p->SecurityContext.M2MPassClearPara=0;

			//清除锁机标志
			//不能清除，锁机流程中就包含登录步骤
			//mainContext_p->SecurityContext.BindParaActive=0;
          
			//status_ind should always send to ci
			if(mainContext_p->WPIEventFucPtr!=PNULL)
			{
				WPIEventData.WPIStatusIndEvent.type = 1;
				WPIEventData.WPIStatusIndEvent.value= LOGINOK;	
				WPIEventData.WPIStatusIndEvent.reason = mainContext_p->PDUUsage;/* CBC@登录成功 */
				(mainContext_p->WPIEventFucPtr)(WPI_STATUS_IND_EVENT,&WPIEventData);					
			}		
		
      }
      break;
      case 0:
      {
			u8 i;
			char *TLVaddr=PNULL;
			mainContext_p->Wmmp_status.protocol_status = LOGINOK;
			
			mainContext_p->LoginFaildCount=0;

			CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
			CommandInfo.SecuritySet = 0;
			CommandInfo.TransType = WMMP_TRANS_DEFAULT;
			CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=NULL;
			CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=0;   
			SaveToHighCommandBuffer(&CommandInfo);  //use high buffer!


			/* add by cbc@20100424: 本地参数配置上报通知标识,M2M终端向平台上报begin*/
			WMMP_LIB_DBG_TEMP("0 login OK !PDUUsage=%d,localcfg=%d\n",mainContext_p->PDUUsage,wmmpGetLocalCfgFlag());
			if ((mainContext_p->PDUUsage == WMMP_LOCAL_CONFIG_REBOOT)||(wmmpGetLocalCfgFlag()))
			{				
				wmmpClearLocalCfgFlag();
				WPA_LocalCfgReq(NULL,0);				
			}
			/* add by cbc@20100424: 本地参数配置上报通知标识,M2M终端向平台上报end*/
			
			
			//long connect need heart beat
			if((mainContext_p->ControlContext.ConnectMode==3)||(mainContext_p->ControlContext.ConnectMode==1))
			{			
				WMMP_LIB_DBG_TEMP("login OK! start heart beat timer\n");
				wmmpStopInterTimer(HEART_BEAT_TIMER);
				wmmpStartInterTimer(HEART_BEAT_TIMER, mainContext_p->ConfigContext.Period);
			}
			wmmpStartInterTimer(STAT_TIMER, 120);			

			//login ok ,so clear the back up param
			mainContext_p->M2MIPAddr_b=0;
			mainContext_p->M2MPort_b=0;
			memset(mainContext_p->AppSMSCentreNum_b,0,sizeof(mainContext_p->AppSMSCentreNum_b));

			/*cbc@20100518 安全参数 */
			memset(mainContext_p->uplinkPassword_b,0,sizeof(mainContext_p->uplinkPassword_b));
			memset(mainContext_p->downlinkPassword_b,0,sizeof(mainContext_p->downlinkPassword_b));
			memset(mainContext_p->publickey_b,0,sizeof(mainContext_p->publickey_b));

			//login ok , no alarm now
			mainContext_p->StatusContext.AlarmStatus =0;
			mainContext_p->StatusContext.AlarmType=0;
			mainContext_p->StatusContext.AlarmCode=0;

			//login ok, clear para is 0
			mainContext_p->SecurityContext.M2MPassClearPara=0;

			//清除锁机标志
			//不能清除，锁机流程中就包含登录步骤
			//mainContext_p->SecurityContext.BindParaActive=0;
          
			//status_ind should always send to ci
			if(mainContext_p->WPIEventFucPtr!=PNULL)
			{
				WPIEventData.WPIStatusIndEvent.type = 1;
				WPIEventData.WPIStatusIndEvent.value= LOGINOK;	
				WPIEventData.WPIStatusIndEvent.reason = mainContext_p->PDUUsage;/* CBC@登录成功 */
				(mainContext_p->WPIEventFucPtr)(WPI_STATUS_IND_EVENT,&WPIEventData);					
			}		
		}
     		break;
		case 8:
		{
			u8 i;
			char *TLVaddr=PNULL;
			mainContext_p->Wmmp_status.protocol_status = LOGINOK;
			
			ReadTLV(0x0025,&TLVret);
			TLV = (u16*)WPI_Malloc(TLVret.length);
			//KiAllocZeroMemory(TLVret.length, (void**) &TLV);
			TLVaddr = TLVret.addr;
			
			WMMP_LIB_DBG_TEMP("wmmpProcessLoginAck:,TLVSyncTagNum=%d\n",mainContext_p->CustomContext.TLVSyncTagNum);
		
			for(i=0;i<TLVret.length/2;i++)
			{
				TLV[i]= Readu16(TLVaddr);
				TLVaddr+=2;
			}

			mainContext_p->LoginFaildCount=0;

			CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
			CommandInfo.SecuritySet = 0;
			CommandInfo.TransType = WMMP_TRANS_DEFAULT;
			CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=TLV;
			CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=mainContext_p->CustomContext.TLVSyncTagNum;   
			SaveToHighCommandBuffer(&CommandInfo);  //use high buffer!


			/* add by cbc@20100424: 本地参数配置上报通知标识,M2M终端向平台上报begin*/
			WMMP_LIB_DBG_TEMP("8 login OK!PDUUsage=%d,localcfg=%d\n",mainContext_p->PDUUsage,wmmpGetLocalCfgFlag());
			if ((mainContext_p->PDUUsage == WMMP_LOCAL_CONFIG_REBOOT)||(wmmpGetLocalCfgFlag()))
			{				
				wmmpClearLocalCfgFlag();
				WPA_LocalCfgReq(NULL,0);				
			}
			/* add by cbc@20100424: 本地参数配置上报通知标识,M2M终端向平台上报end*/
			
			
			//long connect need heart beat
			if((mainContext_p->ControlContext.ConnectMode==3)||(mainContext_p->ControlContext.ConnectMode==1))
			{			
				WMMP_LIB_DBG_TEMP("8 login OK! start heart beat timer\n");
				wmmpStopInterTimer(HEART_BEAT_TIMER);
				wmmpStartInterTimer(HEART_BEAT_TIMER, mainContext_p->ConfigContext.Period);
			}
			wmmpStartInterTimer(STAT_TIMER, 120);			

			//login ok ,so clear the back up param
			mainContext_p->M2MIPAddr_b=0;
			mainContext_p->M2MPort_b=0;
			memset(mainContext_p->AppSMSCentreNum_b,0,sizeof(mainContext_p->AppSMSCentreNum_b));

			/*cbc@20100518 安全参数 */
			memset(mainContext_p->uplinkPassword_b,0,sizeof(mainContext_p->uplinkPassword_b));
			memset(mainContext_p->downlinkPassword_b,0,sizeof(mainContext_p->downlinkPassword_b));
			memset(mainContext_p->publickey_b,0,sizeof(mainContext_p->publickey_b));

			//login ok , no alarm now
			mainContext_p->StatusContext.AlarmStatus =0;
			mainContext_p->StatusContext.AlarmType=0;
			mainContext_p->StatusContext.AlarmCode=0;

			//login ok, clear para is 0
			mainContext_p->SecurityContext.M2MPassClearPara=0;

			//清除锁机标志
			//不能清除，锁机流程中就包含登录步骤
			//mainContext_p->SecurityContext.BindParaActive=0;

			//status_ind should always send to ci
			if(mainContext_p->WPIEventFucPtr!=PNULL)
			{
				WPIEventData.WPIStatusIndEvent.type = 1;
				WPIEventData.WPIStatusIndEvent.value= LOGINOK;	
				WPIEventData.WPIStatusIndEvent.reason = mainContext_p->PDUUsage;/* CBC@登录成功 */
				(mainContext_p->WPIEventFucPtr)(WPI_STATUS_IND_EVENT,&WPIEventData);					
			}		
		}	
		break;
		/*bob add 20101227 -s*/
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:	
		{	
                    /*chongqin demand if result is 6, need register again*/
                    if (6 == result)
                    {
                        /*close login timer, or it will login failed 3 times */
                        wmmpStopInterTimer(TIMING_LOGIN_TIMER);
                        
                        mainContext_p->maxRegisterTimeOutCount = 0;
                        wmmpStartInterTimer(TIMING_REGISTER_TIMER, 90);
                    }

			//wmmpProcessTimingLoginTimer();
			if(mainContext_p->WPIEventFucPtr!=PNULL)
			{	
				WPIEventData.WPIStatusIndEvent.type = 1;
				WPIEventData.WPIStatusIndEvent.value= ALARMLOGINFAILED;	/*  	CBC@登录失败 */
				WPIEventData.WPIStatusIndEvent.reason = mainContext_p->PDUUsage;
			 	(mainContext_p->WPIEventFucPtr)(WPI_STATUS_IND_EVENT,&WPIEventData);					
			}	
		}
		break;
#if 0		
		case 7:
		{
			u8 i;
			char *TLVaddr=PNULL;
			mainContext_p->Wmmp_status.protocol_status = LOGINOK;
			
			mainContext_p->LoginFaildCount=0;
		
			//long connect need heart beat
			if((mainContext_p->ControlContext.ConnectMode==3)||(mainContext_p->ControlContext.ConnectMode==1))
			{			
				WMMP_LIB_DBG_TEMP("7 login OK! start heart beat timer\n");
				wmmpStopInterTimer(HEART_BEAT_TIMER);
				wmmpStartInterTimer(HEART_BEAT_TIMER, mainContext_p->ConfigContext.Period);
			}
			wmmpStartInterTimer(STAT_TIMER, 120);
			
			//status_ind should always send to ci
			if(mainContext_p->WPIEventFucPtr!=PNULL)
			{
				WPIEventData.WPIStatusIndEvent.type = 1;
				WPIEventData.WPIStatusIndEvent.value= LOGINOK;	
				WPIEventData.WPIStatusIndEvent.reason = mainContext_p->PDUUsage;/* CBC@登录成功 */
				(mainContext_p->WPIEventFucPtr)(WPI_STATUS_IND_EVENT,&WPIEventData);					
			}		
		}
		break;
#endif		
		/*bob add 20101227 -e*/
		default:
		{
			//wmmpProcessTimingLoginTimer();	/*bob add 20101228*/
			//status_ind should always send to ci
			if(mainContext_p->WPIEventFucPtr!=PNULL)
			{	
				WPIEventData.WPIStatusIndEvent.type = 1;
				WPIEventData.WPIStatusIndEvent.value= ALARMLOGINFAILED;	/*  	CBC@登录失败 */
				WPIEventData.WPIStatusIndEvent.reason = mainContext_p->PDUUsage;
			 	(mainContext_p->WPIEventFucPtr)(WPI_STATUS_IND_EVENT,&WPIEventData);					
			}	
				
			break;
		}
    }
					

}



//static void  wmmpProcessTrapCnf(const SignalBuffer *signal_p)
void  wmmpProcessTrapCnf()
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();

	WMMP_LIB_DBG_TEMP("wmmpProcessTrapCnf");
    
	switch(mainContext_p->PDUUsage)
	{
		case WMMP_PDU_TRAP_SIGMA_TIME_STAT:
		{
			mainContext_p->StatisticContext.ReportTime=0;
		}/*cbc@20100447:这里没有break; */
		//no break here		
		case WMMP_PDU_TRAP_SIGMA_STAT:
		{
			if(mainContext_p->StatisticContext.ClearStatData==1)
			{
				mainContext_p->StatisticContext.SMSDeliverySucc = 0;
				mainContext_p->StatisticContext.SMSDeliveruFail= 0;
				mainContext_p->StatisticContext.SMSReceivedNum= 0;
				mainContext_p->StatisticContext.GRPSDataSize= 0;
				mainContext_p->StatisticContext.USSDSuccNum= 0;
				mainContext_p->StatisticContext.USSDFailNum= 0;
				mainContext_p->StatisticContext.USSDReceivedNum= 0;
			}
		}
		break;
		
		case WMMP_PDU_TRAP_SMS_STAT:
		{
			mainContext_p->CustomContext.SMSM2MStatRecID=0;
		}
		break;
		
		case WMMP_PDU_TRAP_GPRS_STAT:
		{
			mainContext_p->CustomContext.GPRSM2MStatRecID=0;
		}
		break;

		default:
			break;
	}
		

}

void  wmmpProcessReqCnf(u8 ret,u16 tag)
{

	CommandInfo_t CommandInfo;  
	u16 *tags;
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
    
	WMMP_LIB_DBG_TEMP("wmmpProcessReqCnf");

	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	if(ret==0)
	{
	    //after req, need trap 4010
	   CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
	   CommandInfo.SecuritySet = 0;
		 tags = (u16*)WPI_Malloc(sizeof(u16));
	    //KiAllocZeroMemory(sizeof(u16), (void**) &tags);
	    tags[0]=0x4010;
	    mainContext_p->ControlContext.ExecuteParamOpt[0]=0;
	    memcpy(&mainContext_p->ControlContext.ExecuteParamOpt[1],mainContext_p->FisrtFrameNo,4);
	    CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=tags;
	    CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=1;
	    CommandInfo.TransType = WMMP_TRANS_DEFAULT;	
	    SaveToCommandBuffer(&CommandInfo);			
	}
	
}

void wmmpProcessSecurtiyCfgAck(u8 result)
{
    WmmpContext_t *mainContext_p = ptrToWmmpContext();
    
    CommandInfo_t CommandInfo;

	WMMP_LIB_DBG_TEMP("wmmpProcessSecurtiyCfgAck result =%d", result);

	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	switch(result)
	{
		case 0:
		{
			//终端发起变更密码密钥
			if(mainContext_p->PDUUsage == WMMP_KEY_OUT_OF_DATE)
			{
				CommandInfo.CommandID = LOGOUT_COMMAND;
				CommandInfo.SecuritySet = 0;
				CommandInfo.CommandExInfo.LoginOutInfo.reason=0x09;
				CommandInfo.TransType = WMMP_TRANS_DEFAULT;
				CommandInfo.PDUUsage = WMMP_KEY_OUT_OF_DATE;
				SaveToCommandBuffer(&CommandInfo);	

				CommandInfo.CommandID = LOGIN_COMMAND;
				CommandInfo.SecuritySet = 0;
				CommandInfo.TransType = WMMP_TRANS_DEFAULT;
				CommandInfo.PDUUsage = WMMP_KEY_OUT_OF_DATE;
				SaveToCommandBuffer(&CommandInfo);							
			}							
		}
		break;		

		default: 
		{
			break;
		}		
		
	}
}

void wmmpProcessSecurtiyCfgReq(u8 operation,u8 retcode,u16 *tags,u8 tagnum)
{
    WmmpContext_t *mainContext_p = ptrToWmmpContext();
    
    CommandInfo_t CommandInfo;
    
    WMMP_LIB_DBG_TEMP("wmmpProcessSecurtiyCfgReq retcode =%d, operation =%d", retcode, operation);

	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	switch(retcode)
	{
		case 0:
		{
			switch(operation)
			{
				case 0:
				{
										
					if(mainContext_p->Wmmp_status.protocol_status==REGISTERSUCCUSSFUL)
					{
						//注册流程，收到安全设置短信后自动登录
                                          	WMMP_LIB_DBG_TEMP("wmmpProcessSecurtiyCfgReq 1");
										//SecuritySet ACK
							CommandInfo.CommandID = SECURITY_CONFIG_ACK_COMMAND;
#if defined(GUIZHOU_WMMP)
							CommandInfo.TransType = WMMP_TRANS_GPRS; 
#else
							CommandInfo.TransType = WMMP_TRANS_SMS; /* cbc@20100618:security sms ack  */
#endif
							CommandInfo.SecuritySet = 0;
							CommandInfo.CommandExInfo.SecurityAckInfo.operation=operation;
							CommandInfo.CommandExInfo.SecurityAckInfo.retcode=retcode;
									
							SendCommand(&CommandInfo);

							CommandInfo.CommandID = LOGIN_COMMAND;
							//CommandInfo.SecuritySet = 224;	
							CommandInfo.SecuritySet = 0;
							CommandInfo.TransType = WMMP_TRANS_DEFAULT;
							CommandInfo.PDUUsage = WMMP_PDU_ATCOMMAND;
							SaveToCommandBuffer(&CommandInfo);	


/* 重庆要求在登录成功后保存安全短信的信息，
*  以防登录不成功时，在收到安全短信就保存到NV中，导致再次注册的operator =3
*/
//                                            WPI_WriteNvram(WMMP_TLV_SECURITY);
                                                //save terminalservies num
//                                                WPI_WriteNvram(WMMP_TLV_STATUS);/*bob add 20101210 */
                                                if (mainContext_p->Wmmp_status.register_status == WMMP_REGISTER)
                                                {
                                                    mainContext_p->Wmmp_status.register_status = WMMP_RECV_SECU_SMS;
                                                }
                            

					}
					else
					if(mainContext_p->Wmmp_status.protocol_status==LOGINOK)
					{
						//锁机锁卡流程,或者 密钥变更流程
						if((mainContext_p->PDUUsage == WMMP_PDU_SECURITY_LOGOUT) /*20100521@cbc;平台设置终端安全参数,设置后终端登出并启用新安全参数登录 */
							||(mainContext_p->PDUUsage ==  WMMP_SIM_SECURITY_LOGOUT)/* cbc@20100521: 平台清除SIM卡安全参数 */
								||(mainContext_p->PDUUsage ==  WMMP_SECURITY_REBOOT))
						{
                                             WMMP_LIB_DBG_TEMP("wmmpProcessSecurtiyCfgReq  2");
							if((mainContext_p->TLVtemp!=PNULL)
								&&(mainContext_p->PDUUsage !=  WMMP_SECURITY_REBOOT)) /* cbc@20100518: WMMP_SECURITY_REBOOT 和WMMP_KEY_OUT_OF_DATE 分开*/
							{
								//Loginagain=true;
					   		 	//app first
					   		 	TLVRetVal_t tlvret;
								WMMP_LIB_DBG_TEMP("wmmpProcessSecurtiyCfgReq 4\n");	
								if(checkTLVtemp(0xE036,mainContext_p->TLVtemp,mainContext_p->TLVtempLen,&tlvret)==true)
								{   
									WMMP_LIB_DBG_TEMP("wmmpProcessSecurtiyCfgReq  3\n");	
									mainContext_p->PDUUsage = WMMP_KEY_OUT_OF_DATE;
								}
							}



							CommandInfo.CommandID = SECURITY_CONFIG_ACK_COMMAND;
							CommandInfo.TransType= WMMP_TRANS_DEFAULT; 
							CommandInfo.SecuritySet = 0;
							CommandInfo.CommandExInfo.SecurityAckInfo.operation=operation;
							CommandInfo.CommandExInfo.SecurityAckInfo.retcode=retcode;
									
							SendCommand(&CommandInfo);

							if((mainContext_p->PDUUsage == WMMP_PDU_SECURITY_LOGOUT)
								||(mainContext_p->PDUUsage ==  WMMP_SIM_SECURITY_LOGOUT))
							{
								WMMP_LIB_DBG_TEMP("wmmpProcessSecurtiyCfgReq ,PDUUsage=%d\n",mainContext_p->PDUUsage);	
							    CommandInfo.CommandID = LOGOUT_COMMAND;
							    CommandInfo.SecuritySet = 0;
							    CommandInfo.CommandExInfo.LoginOutInfo.reason=0x0A;/* 更新SIM 卡和对应安全参数 */
							    CommandInfo.TransType = WMMP_TRANS_DEFAULT;
							     CommandInfo.PDUUsage = mainContext_p->PDUUsage;
							    SaveToCommandBuffer(&CommandInfo);	
						      }					
							else if(mainContext_p->PDUUsage == WMMP_SECURITY_REBOOT)
							{
								WMMP_LIB_DBG_TEMP("wmmpProcessSecurtiyCfgReq  6");	
								CommandInfo.CommandID = LOGOUT_COMMAND;
								CommandInfo.SecuritySet = 0;
								CommandInfo.CommandExInfo.LoginOutInfo.reason=0x09; /* CBC@20100518:上下行密码基础密钥修改需要重登陆 */
								CommandInfo.TransType = WMMP_TRANS_DEFAULT;
								CommandInfo.PDUUsage = WMMP_SECURITY_REBOOT;
								SaveToCommandBuffer(&CommandInfo);		

								CommandInfo.CommandID = LOGIN_COMMAND;
								CommandInfo.SecuritySet = 0;	
								CommandInfo.TransType = WMMP_TRANS_DEFAULT;								
								CommandInfo.PDUUsage = WMMP_SECURITY_REBOOT;
								SaveToCommandBuffer(&CommandInfo);	
							}
						}					
					}
					break;
				}
				case 1:
				{
					WMMP_LIB_DBG_TEMP("wmmpProcessSecurtiyCfgReq 7\n");	
					CommandInfo.CommandID = SECURITY_CONFIG_ACK_COMMAND;
					// CommandInfo.SecuritySet = 0xF0;
					CommandInfo.SecuritySet = 0x10;
					CommandInfo.CommandExInfo.SecurityAckInfo.operation=operation;
					CommandInfo.CommandExInfo.SecurityAckInfo.retcode=retcode;
					CommandInfo.CommandExInfo.SecurityAckInfo.tagnum =tagnum;
					CommandInfo.CommandExInfo.SecurityAckInfo.tags = tags;
					CommandInfo.TransType = mainContext_p->TransType;
					SendCommand(&CommandInfo);	
					break;
				}
				case 2:
				case 3:
				{
					WMMP_LIB_DBG_TEMP("wmmpProcessSecurtiyCfgReq  	8\n");	
					CommandInfo.CommandID = SECURITY_CONFIG_ACK_COMMAND;
					CommandInfo.SecuritySet = 0;
					CommandInfo.TransType = mainContext_p->TransType;
					CommandInfo.CommandExInfo.SecurityAckInfo.operation=operation;
					CommandInfo.CommandExInfo.SecurityAckInfo.retcode=retcode;
					CommandInfo.CommandExInfo.SecurityAckInfo.tagnum = tagnum;
					CommandInfo.CommandExInfo.SecurityAckInfo.tags = tags;			
					SendCommand(&CommandInfo);
					break;
				}

				default:
					WMMP_LIB_DBG_TEMP("wmmpProcessSecurtiyCfgReq 9\n");
					break;
			}	
		}
		break;
		
		case 2:
		{
				WMMP_LIB_DBG_TEMP("wmmpProcessSecurtiyCfgReq a\n");
				CommandInfo.CommandID = SECURITY_CONFIG_ACK_COMMAND;
				CommandInfo.SecuritySet = 0;
				CommandInfo.CommandExInfo.SecurityAckInfo.operation=operation;
				CommandInfo.CommandExInfo.SecurityAckInfo.retcode=retcode;
				CommandInfo.CommandExInfo.SecurityAckInfo.tagnum = tagnum;
				CommandInfo.CommandExInfo.SecurityAckInfo.tags = tags;
				CommandInfo.TransType = mainContext_p->TransType;		 /* cbc@20100618:security sms ack  */	
           			//CommandInfo.TransType = WMMP_TRANS_DEFAULT; //mainContext_p->TransType;		 /* cbc@20100618:security sms ack  */		
				SendCommand(&CommandInfo);
		}
		break;

		default:
			WMMP_LIB_DBG_TEMP("wmmpProcessSecurtiyCfgReq 11\n");
			break;
	}

	
	
}

void wmmpProcessConfigGetReq(u16 *TLVTag, u8 Tagnum,u8 ret)
{
	WmmpContext_t *mainContext_p = ptrToWmmpContext();
	CommandInfo_t CommandInfo;
	memset(&CommandInfo,0,sizeof(CommandInfo_t));

    WMMP_LIB_DBG_TEMP("wmmpProcessConfigGetReq");

	CommandInfo.CommandID = CONFIG_GET_ACK_COMMAND;
	CommandInfo.SecuritySet = 0;
	CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
	CommandInfo.TransType = mainContext_p->TransType;
	CommandInfo.CommandExInfo.ConfigGetAckInfo.TLVTags =TLVTag;
	CommandInfo.CommandExInfo.ConfigGetAckInfo.TagNum = Tagnum;
	CommandInfo.CommandExInfo.ConfigGetAckInfo.ret = ret;

	// SaveToCommandBuffer(&CommandInfo);
	SendCommand(&CommandInfo);
}


/************************************************************************
*      cbc@20100424:
*
*      如果参数生效需要重启终端，则先用LOGOUT.LOGOUTREASON=3通知M2M
*	平台需要应用本地配置参数退出登录；
*	待新参数生效，重新向M2M平台登录后，用CONFIG_TRAP和TLV 0x4011
*	上报其配置参数，平台记录终端参数，流程结束。
*
*		该命令和 AT%M2MCFG=N ， 配合使用,先更改参数后后重起
******************************************************************************************/
void wmmpLocalCfgReboot(u8 reason)
{
	WmmpContext_t *mainContext_p = ptrToWmmpContext();
	CommandInfo_t CommandInfo;

	WMMP_LIB_DBG_TEMP("wmmpLocalCfgReboot\n");
    
	memset(&CommandInfo,0,sizeof(CommandInfo_t));
    
	CommandInfo.CommandID = LOGOUT_COMMAND;
	CommandInfo.SecuritySet = 0;
	CommandInfo.TransType = WMMP_TRANS_DEFAULT;
	CommandInfo.PDUUsage = WMMP_LOCAL_CONFIG_REBOOT;
	CommandInfo.CommandExInfo.LoginOutInfo.reason=reason;/* 	cbc@20100419平台下发终端配置本地参数,重新会话 */

	SaveToCommandBuffer(&CommandInfo);	

	//login again
	CommandInfo.CommandID = LOGIN_COMMAND;
	CommandInfo.SecuritySet = 0;	
	CommandInfo.TransType = WMMP_TRANS_DEFAULT;
	CommandInfo.PDUUsage = WMMP_LOCAL_CONFIG_REBOOT;
	SaveToCommandBuffer(&CommandInfo);	
	
}


void wmmpProcessConfigSetReq(u8 ret,u16 *tags, u8 tagnum,char *PDU_TLV, u16 TLVLen)
{

	WmmpContext_t *mainContext_p = ptrToWmmpContext();
	CommandInfo_t CommandInfo;
	u8 i;
	bool  bHasAppData = false;
	bool needreboot=false;
	TLVRetVal_t tlvret;
    WmmpPDUUsage_e reboot_PDUUsage;

    WMMP_LIB_DBG_TEMP("wmmpProcessConfigSetReq ret =%d", ret);

	memset(&tlvret,0,sizeof(tlvret));
	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	mainContext_p->ControlContext.ExecuteParamOpt[0]=0;// 0x4010[0]= 0;
	
	if((PDU_TLV!=PNULL)&&(ret==0)) /*CBC@20100512:只有RET=0表示报文正常的情况下才可以操作 */
	{		
		wmmpAppTLVs(PDU_TLV,TLVLen);		
	}

	CommandInfo.CommandID = CONFIG_SET_ACK_COMMAND;
	CommandInfo.SecuritySet = 0;	
	CommandInfo.SecuritySet |= (1<<5); /* cbc@20100512:CONFIG_SET_ACK_COMMAND 报文必须要摘要*/
	CommandInfo.TransType = mainContext_p->TransType;
	CommandInfo.CommandExInfo.ConfigSetAckInfo.ret = ret;
	
	if(ret==1)
	{
		WMMP_LIB_DBG_TEMP("wmmpProcessConfigSetReq ret==1\n");	
		
		CommandInfo.CommandExInfo.ConfigSetAckInfo.errortag = tags;
		CommandInfo.CommandExInfo.ConfigSetAckInfo.errorTagNum= tagnum;
	}
    //SaveToCommandBuffer(&CommandInfo);    
	SendCommand(&CommandInfo);
	
	if(ret==0)
	{
		WMMP_LIB_DBG_TEMP("wmmpProcessConfigSetReq ret==0\n");	
		
		//check is need reboot
		for(i=0;i<tagnum;i++)
		{
			if(tags[i]==M2MAPN)
			{
				needreboot = true;
                reboot_PDUUsage = WMMP_CONFIG_SET_APN_REBOOT;
                WMMP_LIB_DBG_TEMP("ConfigSetReq M2MAPN reboot tag is %d\n",tags[i]);
				break;
			}
			if((tags[i]==M2MIPADDR)||(tags[i]==M2MPORT))
			{
				needreboot = true;
                reboot_PDUUsage = WMMP_TLV_REBOOT;
                WMMP_LIB_DBG_TEMP("ConfigSetReq reboot tag is %d\n",tags[i]);
				break;
			}
		}
		
		WMMP_LIB_DBG_TEMP("ConfigSet Reqreboot:PDUUsage %d", reboot_PDUUsage);
		//trapp 0x4010
		if(needreboot==true)
		{
			bHasAppData= wmmpCheckHasAppData(MAX_WMMP_APP_NUM);
			if(!bHasAppData)
			{
				WMMP_LIB_DBG_TEMP("ConfigSet Reqreboot:LOGOUT_COMMAND\n");
			CommandInfo.CommandID = LOGOUT_COMMAND;
			CommandInfo.SecuritySet = 0;
			CommandInfo.TransType = WMMP_TRANS_DEFAULT;
            CommandInfo.PDUUsage =  reboot_PDUUsage;
            CommandInfo.CommandExInfo.LoginOutInfo.reason=7;/* 	cbc@20100419平台下发终端配置本地参数,重新会话 */
			    
			SaveToCommandBuffer(&CommandInfo);	
			
			//login again
			CommandInfo.CommandID = LOGIN_COMMAND;
			CommandInfo.SecuritySet = 0;	
			CommandInfo.TransType = WMMP_TRANS_DEFAULT;
            CommandInfo.PDUUsage = reboot_PDUUsage;
            SaveToCommandBuffer(&CommandInfo);	
			}
		}
		else
		{
			u16 *TLV=NULL;	
			u16 *TLV1;	
			//mainContext_p->ControlContext.ExecuteParamOpt[0]=0;
			memcpy(&mainContext_p->ControlContext.ExecuteParamOpt[1],mainContext_p->ReceiveFrameNo,4);

			#if 0
			TLV = WPI_Malloc(2);			
			TLV[0]=0x4010;
			CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
			CommandInfo.SecuritySet = 0;
			CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
			CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=TLV;
			CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=1;
			CommandInfo.TransType = WMMP_TRANS_DEFAULT;
			SaveToCommandBuffer(&CommandInfo);	
			WMMP_LIB_DBG_TEMP("wmmpProcessConfigSetReq trap 0x4010 \n");
			#endif

			if (mainContext_p->ControlContext.ExecuteParamOpt[0]==0)
			{
				  /*add cbc@20100526 : 协议P64  begin*/
				TLV1 = (u16*)WPI_Malloc(4);			 
				TLV1[0]=0x4010;
				TLV1[1]=0x4017;
				CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
				CommandInfo.SecuritySet = 0;
				CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
				CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=TLV1;
				CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=2;
				CommandInfo.TransType = WMMP_TRANS_DEFAULT;
				SaveToCommandBuffer(&CommandInfo);	
				WMMP_LIB_DBG_TEMP("wmmpProcessConfigSetReq trap 0x4010- 0x4017 \n");
				 /*add cbc@20100526 : 协议P64  end*/
			}
			else
			{
				/*add cbc@20100526 : 协议P64  begin*/
				TLV1 = (u16*)WPI_Malloc(2);			 
				TLV1[0]=0x4010;				
				CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
				CommandInfo.SecuritySet = 0;
				CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
				CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=TLV1;
				CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=1;
				CommandInfo.TransType = WMMP_TRANS_DEFAULT;
				SaveToCommandBuffer(&CommandInfo);	
				WMMP_LIB_DBG_TEMP("wmmpProcessConfigSetReq trap 0x4010 \n");
				 /*add cbc@20100526 : 协议P64  end*/
			}
			
		}

		WriteAllTLVParam2Flash();

	}

}

void wmmpProcessRemotCtrlTraceReq(u8 ret,u8 paramId, u16 *errortag, u8 errortagnum)
{
	TLVRetVal_t TLVret;
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	u16 *TLV;
	CommandInfo_t CommandInfo;
	bool		bHasAppData = false;
	char			IDParam[2] = "";
	u16			IDParam_len;  

  	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	WMMP_LIB_DBG_TEMP("wmmpProcessRemotCtrlTraceReq:ret=%d,paramId=%d, protocol_status=0x%x errortag[0] 0x%x",ret, paramId, mainContext_p->Wmmp_status.protocol_status,errortag[0]);
	/*bob add 20101207*/
	if(ret == false)
	{
		if((paramId == 0) || (paramId == 1))
		{
			if(mainContext_p->Wmmp_status.protocol_status==LOGINOK)
			{
				CommandInfo.CommandID = REMOTE_CTRL_ACK_COMMAND;
				CommandInfo.SecuritySet = 0;
				CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
				CommandInfo.CommandExInfo.RemoteCtrlAckInfo.ret = ret;
				CommandInfo.CommandExInfo.RemoteCtrlAckInfo.errortag = errortag;
				CommandInfo.CommandExInfo.RemoteCtrlAckInfo.errorTagNum = errortagnum;
				CommandInfo.TransType = mainContext_p->TransType;
				SendCommand(&CommandInfo);  
			}
		}
		else
		{
			if(mainContext_p->Wmmp_status.protocol_status==LOGINOK)
			{
				CommandInfo.CommandID = REMOTE_CTRL_ACK_COMMAND;
				CommandInfo.SecuritySet = 0;
				CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
				CommandInfo.CommandExInfo.RemoteCtrlAckInfo.ret = 1;
				CommandInfo.CommandExInfo.RemoteCtrlAckInfo.errortag = errortag;
				CommandInfo.CommandExInfo.RemoteCtrlAckInfo.errorTagNum = 1;
				CommandInfo.TransType = mainContext_p->TransType;
				SendCommand(&CommandInfo);  
			}

		}
	}
	else
	{
			if(mainContext_p->Wmmp_status.protocol_status==LOGINOK)
			{
				CommandInfo.CommandID = REMOTE_CTRL_ACK_COMMAND;
				CommandInfo.SecuritySet = 0;
				CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
				CommandInfo.CommandExInfo.RemoteCtrlAckInfo.ret = ret;
				CommandInfo.CommandExInfo.RemoteCtrlAckInfo.errortag = errortag;
				CommandInfo.CommandExInfo.RemoteCtrlAckInfo.errorTagNum = errortagnum;
				CommandInfo.TransType = mainContext_p->TransType;
				SendCommand(&CommandInfo);  
			}

	}

	if((paramId == 0) || (paramId == 1))
	{
		memset(IDParam,0,sizeof(IDParam));
		IDParam[0] = paramId;
		IDParam_len =1;
		WMMP_LIB_DBG_TEMP("write int8 tlv 0x%04x,%d,%d",errortag[0], IDParam_len, IDParam[0]);
		WriteTLV((u16)errortag[0], IDParam_len, IDParam);
	}

}


void wmmpProcessRemotCtrlReq(u8 ret,u8 paramId, u16 *errortag, u8 errortagnum)
{
	TLVRetVal_t TLVret;
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	u16 *TLV;
	CommandInfo_t CommandInfo;
	bool		bHasAppData = false;

  	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	WMMP_LIB_DBG_TEMP("RemotCtrlReq:protocol_status=0x%x",mainContext_p->Wmmp_status.protocol_status);
	/*bob add 20101207*/
	if(ret == false)
	{
		if(paramId == 0x02)
		{
			wmmpStopInterTimer(PDU_INTERVAL_TIMER);
			wmmpStopInterTimer(SEND_PDU_TIMEOUT_TIMER);
			wmmpStopInterTimer(HEART_BEAT_TIMER);
			wmmpStopInterTimer(TIMING_REGISTER_TIMER);
			wmmpStopInterTimer(TIMING_LOGIN_TIMER);
			wmmpStopInterTimer(TRAPABORTWARN_TIIMER);
			stopRegisterForKeyTimer();			
			//RollClearCommandBuffer();   /*bob remove 20101229*/
			//FreeWMMPPDUContext();	/*bob remove 20101229*/
			CommandInfo.CommandID = REMOTE_CTRL_ACK_COMMAND;
			CommandInfo.SecuritySet = 0;
			CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
			CommandInfo.CommandExInfo.RemoteCtrlAckInfo.ret = ret;
			CommandInfo.CommandExInfo.RemoteCtrlAckInfo.errortag = errortag;
			CommandInfo.CommandExInfo.RemoteCtrlAckInfo.errorTagNum = errortagnum;
			CommandInfo.TransType = mainContext_p->TransType;
			SendCommand(&CommandInfo);  
		}
		else if((paramId == 0x03) || (paramId == 0x04) || (paramId == 0x05) || (paramId == 0x0A) || (paramId == 0x0B))
		{
			if(mainContext_p->Wmmp_status.protocol_status==LOGINOK)
			{
				CommandInfo.CommandID = REMOTE_CTRL_ACK_COMMAND;
				CommandInfo.SecuritySet = 0;
				CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
				CommandInfo.CommandExInfo.RemoteCtrlAckInfo.ret = ret;
				CommandInfo.CommandExInfo.RemoteCtrlAckInfo.errortag = errortag;
				CommandInfo.CommandExInfo.RemoteCtrlAckInfo.errorTagNum = errortagnum;
				CommandInfo.TransType = mainContext_p->TransType;
				SendCommand(&CommandInfo);  
			}
		}
		else
		{
			if(mainContext_p->Wmmp_status.protocol_status==LOGINOK)
			{
				CommandInfo.CommandID = REMOTE_CTRL_ACK_COMMAND;
				CommandInfo.SecuritySet = 0;
				CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
				CommandInfo.CommandExInfo.RemoteCtrlAckInfo.ret = 1;
				CommandInfo.CommandExInfo.RemoteCtrlAckInfo.errortag = errortag;
				CommandInfo.CommandExInfo.RemoteCtrlAckInfo.errorTagNum = 1;
				CommandInfo.TransType = mainContext_p->TransType;
				SendCommand(&CommandInfo);  
			}
	
		}
	}
	else
	{
	if(mainContext_p->Wmmp_status.protocol_status==LOGINOK)
	{
		CommandInfo.CommandID = REMOTE_CTRL_ACK_COMMAND;
		CommandInfo.SecuritySet = 0;
		CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
		CommandInfo.CommandExInfo.RemoteCtrlAckInfo.ret = ret;
		CommandInfo.CommandExInfo.RemoteCtrlAckInfo.errortag = errortag;
		CommandInfo.CommandExInfo.RemoteCtrlAckInfo.errorTagNum = errortagnum;
		CommandInfo.TransType = mainContext_p->TransType;
		SendCommand(&CommandInfo);  
	}

	}
	WMMP_LIB_DBG_TEMP("wmmpProcessRemotCtrlReq:paramId=%d",paramId);
	
	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	switch(paramId)
	{		
	   case 0x02: //重启
      {   
		bHasAppData= wmmpCheckHasAppData(MAX_WMMP_APP_NUM);
		if(!bHasAppData)
		{
			
			WMMP_LIB_DBG_TEMP("RemotCtrlReq Reqreboot:LOGIN");
#if 1		/*bob modify 20110120*/
			if(strcmp(mainContext_p->StatusContext.TerminalSeriesNum,(char*)UNREGISTER_TERMINAL_NUM) == 0)
			{
				WPI_sleep(800000);
				WPI_SystemReboot();
			}
			else
			{
				WMMP_LIB_DBG_TEMP("RemotCtrlReq Reqreboot:LOGOUT_COMMAND");
				CommandInfo.CommandID = LOGOUT_COMMAND;
				CommandInfo.SecuritySet = 0;
				CommandInfo.TransType = WMMP_TRANS_DEFAULT;
				CommandInfo.PDUUsage = WMMP_REMOTE_CONTROL_REBOOT;
//				CommandInfo.CommandExInfo.LoginOutInfo.reason=7;				    
				CommandInfo.CommandExInfo.LoginOutInfo.reason=0; //chongqin demand
				SaveToCommandBuffer(&CommandInfo);					
			}
			WMMP_LIB_DBG_TEMP("RemotCtrlReq Reqreboot:LOGIN debugdebug    ");	
#else			
			/*bob add 20101209*/		
			if(strcmp(mainContext_p->StatusContext.TerminalSeriesNum,(char*)UNREGISTER_TERMINAL_NUM) == 0)
			{
				wmmpRegisterReq(WMMP_TRANS_DEFAULT);	
			}
			else
			{
				WMMP_LIB_DBG_TEMP("RemotCtrlReq Reqreboot:LOGOUT_COMMAND");
				CommandInfo.CommandID = LOGOUT_COMMAND;
				CommandInfo.SecuritySet = 0;
				CommandInfo.TransType = WMMP_TRANS_DEFAULT;
				CommandInfo.PDUUsage = WMMP_REMOTE_CONTROL_REBOOT;
				CommandInfo.CommandExInfo.LoginOutInfo.reason=7;
				    
				SaveToCommandBuffer(&CommandInfo);	
				//login again
				CommandInfo.CommandID = LOGIN_COMMAND;
				CommandInfo.SecuritySet = 0;	
				CommandInfo.TransType = WMMP_TRANS_DEFAULT;
				CommandInfo.PDUUsage = WMMP_REMOTE_CONTROL_REBOOT;
				//SaveToHighCommandBuffer(&CommandInfo);	  /*bob remove 20101230*/
				SaveToCommandBuffer(&CommandInfo);	
			}
#endif			
		}
			
       }
       break;
		case 0x03:
		{
			WMMP_LIB_DBG_TEMP("RemotCtrlReq:paramId=%d,protocol_status=%d\n",paramId,mainContext_p->Wmmp_status.protocol_status);
			
			if(mainContext_p->Wmmp_status.protocol_status==PIDLE)
			{
				WMMP_LIB_DBG_TEMP("mainContext_p->Wmmp_status.protocol_status==PIDLE\n");
					
				//wmmpSendNextCommand();
				wmmpStartInterTimer(PDU_INTERVAL_TIMER, 2); /*20100408@cbc*/

				/* add by cbc@20100520: 远程唤醒肯定只能是 短信唤醒需要一个短信唤醒回复 begin*/
				CommandInfo.CommandID = REMOTE_CTRL_ACK_COMMAND;
				CommandInfo.SecuritySet = 0;				
				CommandInfo.CommandExInfo.RemoteCtrlAckInfo.ret = ret;
				CommandInfo.CommandExInfo.RemoteCtrlAckInfo.errortag = errortag;
				CommandInfo.CommandExInfo.RemoteCtrlAckInfo.errorTagNum = errortagnum;
#if defined(GUIZHOU_WMMP)				
				CommandInfo.TransType = WMMP_TRANS_GPRS;
#else
				CommandInfo.TransType = WMMP_TRANS_SMS;  /*bob remove 20111125 for guizhou*/
#endif
				SendCommand(&CommandInfo); 
				/* add by cbc@20100520: 远程唤醒肯定只能是 短信唤醒需要一个短信唤醒回复 end*/

				CommandInfo.CommandID = LOGIN_COMMAND;
				//CommandInfo.SecuritySet = 224;//11100000
				CommandInfo.SecuritySet = 0;
				CommandInfo.TransType = WMMP_TRANS_DEFAULT;
				CommandInfo.PDUUsage = WMMP_REMOTE_CONTROL;
				SaveToCommandBuffer(&CommandInfo);
			}
		}
		break;

		case 0x04:
		{
			WMMP_LIB_DBG_TEMP("paramId ==4");
			SigmaStatTrap(WMMP_PDU_TRAP_SIGMA_STAT);
		}
		break;

		case 0x05:
		{
			WPIRtcDateAndTime dateAndTime;
			WPIRtcStatus_e status;
			WMMP_LIB_DBG_TEMP("paramId ==5\n");
			if(mainContext_p->StatisticContext.StatStartTime==0)
			{
				mainContext_p->StatisticContext.StatStartTime	= WPI_GetCurrentTime(&dateAndTime,&status);
			}
			mainContext_p->StatisticContext.SMSDeliverySucc = 0;
			mainContext_p->StatisticContext.SMSDeliveruFail= 0;
			mainContext_p->StatisticContext.SMSReceivedNum= 0;
			mainContext_p->StatisticContext.GRPSDataSize= 0;
			//trap 0x4010
			mainContext_p->ControlContext.ExecuteParamOpt[0]=0;
			memcpy(&mainContext_p->ControlContext.ExecuteParamOpt[1],mainContext_p->ReceiveFrameNo,4);
			TLV = (u16*)WPI_Malloc(2);
			//KiAllocZeroMemory(2, (void**) &TLV);
			TLV[0]=0x4010;

			CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
			CommandInfo.SecuritySet = 0;
			CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=TLV;
			CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=1;
			CommandInfo.TransType = WMMP_TRANS_DEFAULT;
			SaveToCommandBuffer(&CommandInfo);					
		}
		break;
      case 0x0A:/*restore factory configure*/
      {
           	bHasAppData= wmmpCheckHasAppData(MAX_WMMP_APP_NUM);
		if(!bHasAppData)
		{
			WMMP_LIB_DBG_TEMP("RemotCtrlReq restore factory:LOGOUT_COMMAND\n");
			CommandInfo.CommandID = LOGOUT_COMMAND;
			CommandInfo.SecuritySet = 0;
			CommandInfo.TransType = WMMP_TRANS_DEFAULT;
			CommandInfo.PDUUsage = WMMP_RMCON_REFACTORY_REBOOT;
			CommandInfo.CommandExInfo.LoginOutInfo.reason=7;

			SaveToCommandBuffer(&CommandInfo);	

			WMMP_LIB_DBG_TEMP("RemotCtrlReq restore factory :LOGIN");
			//login again
			CommandInfo.CommandID = LOGIN_COMMAND;
			CommandInfo.SecuritySet = 0;	
			CommandInfo.TransType = WMMP_TRANS_DEFAULT;
			CommandInfo.PDUUsage = WMMP_RMCON_REFACTORY_REBOOT;
			SaveToCommandBuffer(&CommandInfo);   
		}
      }
      break;

		case 0x0B:
		{
			u8 i;
			char *TLVaddr=PNULL;
			WMMP_LIB_DBG_TEMP("paramId ==0x0b\n");
			ReadTLV(0x0025,&TLVret);
			TLV = (u16*)WPI_Malloc(TLVret.length);
			// KiAllocZeroMemory(TLVret.length, (void**) &TLV);
			TLVaddr = TLVret.addr;
			for(i=0;i<TLVret.length/2;i++)
			{
				TLV[i]= Readu16(TLVaddr);
				TLVaddr+=2;
			}
          		WMMP_LIB_DBG_TEMP("RemotCtrlReq TLVSyncTagNum %d",mainContext_p->CustomContext.TLVSyncTagNum);
			CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
			CommandInfo.SecuritySet = 0;
			CommandInfo.TransType = WMMP_TRANS_DEFAULT;
			CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=TLV;
			CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=mainContext_p->CustomContext.TLVSyncTagNum;   
			SaveToCommandBuffer(&CommandInfo);
		}
		break;

		default:
			break;
	}
}

void wmmpProcessDownloadReq(char* url,u8 ret)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	CommandInfo_t CommandInfo;

	WMMP_LIB_DBG_TEMP("wmmpProcessDownloadReq:ret=%d",ret);

	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	CommandInfo.CommandID = DOWNLOAD_INFO_ACK_COMMAND;
	CommandInfo.SecuritySet = 0;
	CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
	CommandInfo.TransType = mainContext_p->TransType;
	CommandInfo.CommandExInfo.DownloadAckInfo.ret = ret;
	//SaveToCommandBuffer(&CommandInfo);  
	SendCommand(&CommandInfo);
	
}

void wmmpProcessTransDataReq(char type, char* des, char *data, u16 datalen)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	CommandInfo_t CommandInfo;

	WMMP_LIB_DBG_TEMP("wmmpProcessTransDataReq:type=%d",type);

	memset(&CommandInfo,0,sizeof(CommandInfo_t));

	CommandInfo.CommandID = TRANSPARENT_DATA_ACK_COMMAND;
	CommandInfo.SecuritySet = 0;	
	CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */		
	CommandInfo.TransType = mainContext_p->TransType;
	SendCommand(&CommandInfo);
	//SaveToCommandBuffer(&CommandInfo); 

}

/* cbc@20100412:释放 WMMP 协议流程*/
void wmmpFreeTest(void)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	CommandInfo_t CommandInfo;
	char *p;
	u16 TLVtemplength;
      u16 tag;
      u16 length;
      u16 loopi = 0;

	WMMP_LIB_DBG_TEMP("wmmpFreeTest:");
        
	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	ClearCommandBuffer();
	memset(mainContext_p->TimeStamp,0,4);
	wmmpStopInterTimer(HEART_BEAT_TIMER);
	wmmpStopInterTimer(SHORT_CONNECT_MODE_TIMER);
	wmmpStopInterTimer(SIGMA_STAT_TIMER);
	wmmpStopInterTimer(STAT_TIMER);	
	
	mainContext_p->Wmmp_status.protocol_status =  PIDLE;

	WPI_StopTimer();
	
	mainContext_p->m2mswitch = false;
	//close all socket;
	if(mainContext_p->wmmpSocket!=CONNREF_INVALID)
	{
		WMMP_LIB_DBG_TEMP("mainContext_p->PDUUsage==WMMP_PDU_LOGOUT_TERM 1\n");	
		wmmpCloseSocket();
	}	
   for(loopi = 0;loopi < MAX_WMMP_APP_NUM;loopi++)
	{
    	if(mainContext_p->wmmpOpenSkt[loopi]!=CONNREF_INVALID)
    	{
    		WMMP_LIB_DBG_TEMP("mainContext_p->PDUUsage==WMMP_PDU_LOGOUT_TERM 2\n");	
    		wmmpLibCloseConnect(mainContext_p->wmmpOpenSkt[loopi]);

}
  }
}

void wmmpProcessLogoutCnf(void)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	CommandInfo_t CommandInfo;
	char *p;
	u16 TLVtemplength;
      u16 tag;
      u16 length;

	u16 *tags=NULL;
	u16 *tags2 = NULL;
	u8 i;	

	WMMP_LIB_DBG_TEMP("wmmpProcessLogoutCnf");
		
	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	ClearCommandBuffer();
	memset(mainContext_p->TimeStamp,0,4);
	wmmpStopInterTimer(HEART_BEAT_TIMER);
	wmmpStopInterTimer(SHORT_CONNECT_MODE_TIMER);
	wmmpStopInterTimer(SIGMA_STAT_TIMER);
	wmmpStopInterTimer(STAT_TIMER);
	
	mainContext_p->Wmmp_status.protocol_status =  PIDLE;

	//gwopenValue = MC_AKI_READ_EEPROM_VALUE(AKI_HEE_WOPEN_PARAM);   /*20120508 for compile*/
	WMMP_LIB_DBG_TEMP("wmmpProcessLogoutCnf gwopenValue %d PDUUsage = %d",gwopenValue, mainContext_p->PDUUsage);	

	//auto login out, doesn't need close socket	
	if((mainContext_p->PDUUsage == WMMP_PDU_SECURITY_LOGOUT)||(mainContext_p->PDUUsage == WMMP_KEY_OUT_OF_DATE))
	{
	
		if(mainContext_p->TLVtemp!=PNULL)
		{
					
   		 	//app first
	    		wmmpAppTLVs(mainContext_p->TLVtemp,mainContext_p->TLVtempLen);
	  		 //write second
					//wmmpWriteTLVs(mainContext_p->TLVtemp,mainContext_p->TLVtempLen);
				 
			WPI_Free(mainContext_p->TLVtemp);
			mainContext_p->TLVtemp=PNULL;
			mainContext_p->TLVtempLen = 0;
									
			WMMP_LIB_DBG_TEMP("WMMP stack---updata key\n");					
				
		}				
				
		CommandInfo.CommandID = LOGIN_COMMAND;
		CommandInfo.SecuritySet = 0;	
		CommandInfo.TransType = WMMP_TRANS_DEFAULT;
		CommandInfo.PDUUsage = WMMP_PDU_SECURITY_LOGOUT;
		SaveToCommandBuffer(&CommandInfo);	

		/* add by cbc@20100521 : SIM卡PIN1 更改后重启需要上报 SECURITY TRAP 命令, OPERATION = 4, BEGIN */
		if(mainContext_p->PDUUsage == WMMP_PDU_SECURITY_LOGOUT)
		{
			CommandInfo.CommandID = SECURITY_CONFIG_COMMAND;
			CommandInfo.SecuritySet = 0;	
			CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */			
			CommandInfo.TransType = WMMP_TRANS_DEFAULT;	

			/* 加密情况下PIN1 的上报需要摘要值 */
			if (mainContext_p->SecurityContext.encryptionUsed==1)
			{
				tags = (u16*)WPI_Malloc(1*sizeof(u16));
				memset(tags,0,1*sizeof(u16));					
				tags[0]=0XE002;
				CommandInfo.CommandExInfo.SecurityInfo.tagnum=1;
			}
			else
			{
				tags = (u16*)WPI_Malloc(2*sizeof(u16));
				memset(tags,0,2*sizeof(u16));					
				tags[0]=0XE001;
				tags[1]=0XE003;
				CommandInfo.CommandExInfo.SecurityInfo.tagnum=2;
			}
			/* add by cbc@20100521 : SIM卡PIN1 更改后重启需要上报 SECURITY TRAP 命令, OPERATION = 4, end */
		
			CommandInfo.CommandExInfo.SecurityInfo.operation=4;/* 终端明文上报上报安全参数*/
			CommandInfo.CommandExInfo.SecurityInfo.tags=tags;
			
//			SIMCOM_DBG(SIMCOM_MODULE_M2MAT,DEBUG_LEVEL_NORMAL,"at_handle_m2m_test:SECURITY_CONFIG_COMMAND\n");
			SaveToCommandBuffer(&CommandInfo);		


			
		}
		/* add by cbc@20100521 : SIM卡PIN1 更改后重启需要上报 SECURITY TRAP 命令, OPERATION = 4, END */
		
		WMMP_LIB_DBG_TEMP("WMMP stack---updata key 1\n");			

	}
	else
	if(mainContext_p->PDUUsage==WMMP_PDU_LOGOUT_TERM)    //when login out by AT command , then close socket
	{
		WPI_StopTimer();
		mainContext_p->m2mswitch = false;
		//close all socket;
		if(mainContext_p->wmmpSocket!=CONNREF_INVALID)
		{
			WMMP_LIB_DBG_TEMP("mainContext_p->PDUUsage==WMMP_PDU_LOGOUT_TERM 1\n");	
			wmmpCloseSocket();
		}	
		WMMP_LIB_DBG_TEMP("mainContext_p->PDUUsage==WMMP_PDU_LOGOUT_TERM 2\n");	
	}
	/* add by cbc@20100409  更新 SIM 卡安全参数设置 begin */
	else
		 if(mainContext_p->PDUUsage==WMMP_SIM_SECURITY_LOGOUT) /* cbc@20100521: 平台清除SIM卡安全参数 */
	{
		WPI_StopTimer();
		mainContext_p->m2mswitch = false;		
		
		WMMP_LIB_DBG_TEMP("mainContext_p->PDUUsage==WMMP_SIM_SECURITY_LOGOUT 1\n");	
		//close all socket;
		if(mainContext_p->wmmpSocket!=CONNREF_INVALID)
		{
			WMMP_LIB_DBG_TEMP("mainContext_p->PDUUsage==WMMP_SIM_SECURITY_LOGOUT 1\n");	
			wmmpCloseSocket();
		}	
		WMMP_LIB_DBG_TEMP("mainContext_p->PDUUsage==WMMP_PDU_LOGOUT_TERM 3\n");	
	}
	/* add by cbc@20100409  更新 SIM 卡安全参数设置 end */
	else if(!gwopenValue)
	{
			
		//MC_AKI_SAVE_EEPROM_VALUE(AKI_HEE_WOPEN_PARAM,gwopenValue);
		WMMP_LIB_DBG_TEMP("wmmpProcessLogoutCnf gwopenValue %d",gwopenValue);	
		WPI_StopTimer();
		mainContext_p->m2mswitch = false;
		//close all socket;
		if(mainContext_p->wmmpSocket!=CONNREF_INVALID)
		{			
			wmmpCloseSocket();			
		}
		
		WPA_Term();
		gLoginStatus = 0;
	}
	else
	{
		//close wmmp socket
		if(mainContext_p->wmmpSocket!=CONNREF_INVALID)
		{
			WMMP_LIB_DBG_TEMP("mainContext_p->PDUUsage==WMMP_PDU_LOGOUT_TERM 3\n");	
			wmmpCloseSocket();			
		}
		WMMP_LIB_DBG_TEMP("mainContext_p->PDUUsage==WMMP_PDU_LOGOUT_TERM 4\n");	
	}	
	WriteAllTLVParam2Flash();//save all TLV

}

void wmmpServerContinueSend(u16 CommandID, char* transID,u16 partnum,u16 partallnum)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	CommandInfo_t CommandInfo;
    
	WMMP_LIB_DBG_TEMP("wmmpServerContinueSend CommandID=%d", CommandID);

	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	CommandInfo.CommandID = CONTINUE_SEND_COMMAND;
	CommandInfo.SecuritySet = 0;
	CommandInfo.TransType = mainContext_p->TransType;
	CommandInfo.CommandExInfo.ContinueSendInfo.CommandID = CommandID;
	CommandInfo.CommandExInfo.ContinueSendInfo.partallnum = partallnum;
	CommandInfo.CommandExInfo.ContinueSendInfo.partnum= partnum;
	memcpy(CommandInfo.CommandExInfo.ContinueSendInfo.transID,transID,2);
	//SaveToCommandBuffer(&CommandInfo);  

	SendCommand(&CommandInfo);
}


void wmmpSendbySMS(PDUContext_t   *pduContext)
{
	WmmpContext_t *mainContext = ptrToWmmpContext(); 

	WMMP_LIB_DBG_TEMP("wmmpSendbySMS StackState %d",mainContext->StackState);
	if(mainContext->ConfigContext.M2MSMSpecialNum[0]!=0)
	{	
		/* simcom cbc@20091221:这里的判断条件和GPRS 的条件不一致,等待ZYC确认???? */
		if((mainContext->StackState== WMMP_STACK_SERVER_WAITING)||(mainContext->StackState==WMMP_STACK_CLIENT_WAITING))
		{	
			wmmp_SendSMS(mainContext->SendBuffer,mainContext->ConfigContext.M2MSMSpecialNum, pduContext->SendLength);	
		}
		else
		{	
			wmmp_SendSMS(mainContext->IOBuffer,mainContext->ConfigContext.M2MSMSpecialNum,pduContext->SendLength);	
		}

		if(mainContext->isM2MStatTime ==true)
		{
			mainContext->StatisticContext.SMSDeliverySucc++;
		}

		if(mainContext->CustomContext.SMSM2MStatRecFirstTime!=0)
		{
			mainContext->CustomContext.SMSM2MSendStat++;
		}				 
	}
	else
	{
		//若没有设置短信号码,则1秒后继续处理下一个报文
		 if((mainContext->StackState== WMMP_STACK_SERVER_WAITING)||(mainContext->StackState==WMMP_STACK_SERVER_CONTINUE))
		 { 
		 	 if(wmmpisInterTimerRun(SEND_PDU_TIMEOUT_TIMER)==false)
		 	 {		
		 	 	WMMP_LIB_DBG_TEMP("wmmpSendbySMS 2 resent timer start\n");
		 	 	wmmpStartInterTimer(SEND_PDU_TIMEOUT_TIMER, 1);
		 	 }
			else
			{
				WMMP_LIB_DBG_TEMP("wmmpSendbySMS 2 didn't start send timeout timer,%d\n",mainContext->ReSendTime);
			}		
		}
	}
}


void  adFileEndFileReqHandle(void)
{
	WmmpContext_t *mainContext = ptrToWmmpContext();
	PDUContext_t   *pduContext = &mainContext->PDUContext_receive;	
	//WmmStackState_e RecvState=pduContext->StackState;
	WmmpDecodeRecvResult_e ret=WMMP_DECODE_NOPDU;
	//char * pl_StrPtr;

	WMMP_LIB_DBG_TEMP("adFileEndFileReqHandle StackState=%d",mainContext->StackState);
	WMMP_LIB_DBG_TEMP(" m2mswitch %d",mainContext->m2mswitch );
	WMMP_LIB_DBG_TEMP(" mainContext->PDUUsage %d ",mainContext->PDUUsage);
	if(mainContext->m2mswitch == false)
	{
		//return ret;   /*20120508 for compile*/
	}    


	ret = WMMP_DECODE_OK;


	WMMP_LIB_DBG_TEMP(" mainContext->StackState=%d",mainContext->StackState);


	WMMP_LIB_DBG_TEMP("  ret %d  mainContext->TransType %d PDUUsage %d",ret,mainContext->TransType,mainContext->PDUUsage);
   
	if(mainContext->StackState==WMMP_STACK_SERVER_WAITING)
	{
		WMMP_LIB_DBG_TEMP(" mainContext->StackState==WMMP_STACK_SERVER_WAITING\n");

		mainContext->StackState=WMMP_STACK_INIT;/* add by    cbc@20100408 for 在重查发送定时器未来前如果平台马上主动发起报文会丢包 */

		//if((mainContext->PDUUsage==WMMP_TLV_REBOOT)||(mainContext->PDUUsage==WMMP_LOCAL_CONFIG_REBOOT)) 
		/*bob modify 20101122*/
		if((mainContext->PDUUsage==WMMP_TLV_REBOOT)||(mainContext->PDUUsage==WMMP_LOCAL_CONFIG_REBOOT) || (mainContext->PDUUsage==WMMP_CONFIG_SET_APN_REBOOT) || (mainContext->PDUUsage==WMMP_REMOTE_CONTROL_REBOOT) ||(mainContext->PDUUsage==WMMP_RMCON_REFACTORY_REBOOT)) 
		{
			if(mainContext->PDUUsage==WMMP_REMOTE_CONTROL_REBOOT)  /*bob add 20110122*/
			{
				WMMP_LIB_DBG_TEMP("  before WPI_SystemReboot ret %d  mainContext->TransType %d",ret,mainContext->TransType);
				WPI_SystemReboot();	
			}
			else
			{
				WMMP_LIB_DBG_TEMP(" mainContext->PDUUsage==WMMP_TLV_REBOOT and other ");
				wmmpStartInterTimer(PDU_INTERVAL_TIMER, 2); /*cbc@20100406:检查发送缓冲区*/	
			}
		}
		else
		{
			WMMP_LIB_DBG_TEMP(" StackState==WMMP_STACK_SERVER_WAITING:PDU_INTERVAL_TIMER");
			wmmpStartInterTimer(PDU_INTERVAL_TIMER, 1); /*cbc@20100406:检查发送缓冲区*/	
			}		
		}
	else
	{
		WMMP_LIB_DBG_TEMP(" mainContext->StackState!=WMMP_STACK_SERVER_WAITING");
	}
	
  	
	
	//return ret;  /*20120508 for compile */
}


/*****************************************************************************
*	函数名	      : wmmpGprsSendCallback
* 作  者        : 
*	功能描述      : WMMP调用WPI_Send的回调函数
*	输入参数说明  : 无
* 输出参数说明  : 无
*	返回值说明    : 无
*****************************************************************************/
void wmmpGprsSendCallback(u32 sizeProcessed)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	WmmpContext_t *mainContext = ptrToWmmpContext();
	WmmpUpdateContext_t  *UpdateContext_p = &(mainContext->UpdateContext);	
	PDUContext_t *pduContext=&(mainContext->PDUContext);
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	          WMMP_LIB_DBG_TEMP("wmmpSendCallback");
	
                  if(pduContext->NeedDispart ==false)	
                  	{
                  	
			if(sizeProcessed==0xFFFFFFFF)
			{
				if(pduContext->adFileEndFileReqFlag)
				{
					 if(wmmpisInterTimerRun(SEND_PDU_TIMEOUT_TIMER)==false)
		 			{
		 				WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS 1 for the end file_req start SEND_PDU_TIMEOUT_TIMER ");
						wmmpStartInterTimer(SEND_PDU_TIMEOUT_TIMER, 90);
					 }
				}					
			}
			else  /*bob add 20110217 for the end FILE_REQ no FILE_ACK*/
			{
				WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS 1 ok adFileEndFileReqFlag %d",pduContext->adFileEndFileReqFlag);
				if(pduContext->adFileEndFileReqFlag)
				{
					WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS 1 clear s adFileEndFileReqFlag");
					UpdateContext_p->adFileFlag = 0;				
					UpdateContext_p->adFileFileReadPoint = 0;	
					WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS 1 clear e adFileEndFileReqFlag");					
				}
			}
                  	}
		 else
		 	{
		 	if(sizeProcessed==0xFFFFFFFF)
			{
				if(pduContext->adFileEndFileReqFlag)
				{
					 if(wmmpisInterTimerRun(SEND_PDU_TIMEOUT_TIMER)==false)
		 			{
		 				WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS 2 for the end file_req start SEND_PDU_TIMEOUT_TIMER ");
						wmmpStartInterTimer(SEND_PDU_TIMEOUT_TIMER, 90);
					 }
				}				
			}
			else  /*bob add 20110217 for the end FILE_REQ no FILE_ACK*/
			{
				WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS 2 ok adFileEndFileReqFlag %d",pduContext->adFileEndFileReqFlag);
				if(pduContext->adFileEndFileReqFlag)
				{
					WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS 2 clear s adFileEndFileReqFlag");
					UpdateContext_p->adFileFlag = 0;				
					UpdateContext_p->adFileFileReadPoint = 0;
					mainContext->StatusContext.Majorver = mainContext->UpdateContext.UpdateFileMajorRev;
					mainContext->StatusContext.MinorVer = mainContext->UpdateContext.UpdateFileMinorRev;					
					WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS 2 clear e adFileEndFileReqFlag");
				}
			}	
		 	}

		 WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS 1 ReSendTime %d",mainContext->ReSendTime);
		 
		if(wmmpisInterTimerRun(SEND_PDU_TIMEOUT_TIMER)==false)
		{
			
			WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS 1 ");
			if(pduContext->adFileEndFileReqFlag)
			{
				pduContext->adFileEndFileReqFlag = 0;
			/* cbc@2010: PDU 发送超时重新发送20秒定时 */
				WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS 1  adFileEndFileReqFlag 1 not start resent timer start");				
			}
			else
			{
				WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS 1  adFileEndFileReqFlag 0 start resent timer start");
				wmmpStartInterTimer(SEND_PDU_TIMEOUT_TIMER, 90);
			}
		}
		else
		{
		     	WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS 1 didn't start send timeout timer,%d\n",mainContext->ReSendTime);
		}	
	
}
/*****************************************************************************
*	函数名	      : wmmpSendbyGPRS
* 作  者        : 
*	功能描述      : 通过GPRS发送WMMP数据
*	输入参数说明  : 无
* 输出参数说明  : 无
*	返回值说明    : 无
*****************************************************************************/
void wmmpSendbyGPRS(PDUContext_t   *pduContext)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	WmmpContext_t *mainContext = ptrToWmmpContext();
	WmmpUpdateContext_t  *UpdateContext_p = &(mainContext->UpdateContext);
	WMMPAPNType apn_type = wmmp_get_apn_type();

	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS WPI_hex_printf() adFileEndFileReqFlag %d",pduContext->adFileEndFileReqFlag);			
	WPI_hex_printf((char*)(mainContext->SendBuffer),(u16)(pduContext->SendLength));//      // 20100315@CBC DEBUG 
		
	if (mainContext->wmmpSocket != CONNREF_INVALID)/*有效连接*/
	{
		if(pduContext->NeedDispart ==false)			
		{		    	
			 WPI_Send(apn_type,mainContext->wmmpSocket,mainContext->SendBuffer,pduContext->SendLength);
			  mainContext->gprsSendCallBack=wmmpGprsSendCallback;                         
		}
		else
		{	    		
			 WPI_Send(apn_type,mainContext->wmmpSocket,mainContext->IOBuffer,pduContext->SendLength);/*     cbc@20100401: 分包是 	mainContext->SendBuffer 分到	 mainContext->IOBuffer ，然后发送出去	 */
			  mainContext->gprsSendCallBack=wmmpGprsSendCallback;
		}

		//统计
		if(mainContext->isM2MStatTime ==true)
		{
			mainContext->StatisticContext.GRPSDataSize+=pduContext->SendLength;
		}	
		if(mainContext->CustomContext.GPRSM2MStatRecFirstTime!=0)
		{
			mainContext->CustomContext.GPRSM2MSendStat+=pduContext->SendLength;
		}				 
	}
	else
	{
    		WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS 2 ReSendTime %d",mainContext->ReSendTime);

		/* cbc@20100401: 40秒激活场景的时间,b */
		if(wmmpisInterTimerRun(SEND_PDU_TIMEOUT_TIMER)==false)
		{
  			WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS 2 resent timer start");
  			//wmmpStartInterTimer(SEND_PDU_TIMEOUT_TIMER, 60);			/*bob 20101106 remove */
  			wmmpStartInterTimer(SEND_PDU_TIMEOUT_TIMER, 90);
		}				
		else
		{
  			WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS 2 didn't start send timeout timer,%d\n",mainContext->ReSendTime);
		}	

		if(mainContext->Wmmp_status.net_status == LINK_OK)
		{
			wmmpCreateSocketConnect();
			WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS 2 :wmmpCreateSocketConnect");
		}
		else
		{
			wmmpLibActiveGprs();
			WMMP_LIB_DBG_TEMP("wmmpSendbyGPRS 2 :wmmpActiveGprs");
		}
    
		
	}

	//if short connect, connect time out timer start
	if((mainContext->ControlContext.ConnectMode==0)||(mainContext->ControlContext.ConnectMode==2))
	{   /* cbc@20100401: 短连接情况下,如果60秒内没有报文就自动登出 */
		wmmpStopInterTimer(SHORT_CONNECT_MODE_TIMER);
		wmmpStartInterTimer(SHORT_CONNECT_MODE_TIMER, 60);	

		WMMP_LIB_DBG_TEMP("short connect start send timeout timer");
	}
}


void wmmpSendPDU(PDUContext_t   *pduContext)
{
	WmmpContext_t *mainContext = ptrToWmmpContext();
	//PDUContext_t   *pduContext = &mainContext->PDUContext;	
	
	WMMP_LIB_DBG_TEMP("wmmpSendPDU TransType %d",mainContext->TransType);
	switch(mainContext->TransType)
	{
		case WMMP_TRANS_GPRS:
		{
			wmmpSendbyGPRS(pduContext);
		}
		break;
		
		case WMMP_TRANS_SMS:
		{
			wmmpSendbySMS(pduContext);
		}
		break;
		
		case WMMP_TRANS_DEFAULT:
		{
			switch(mainContext->ControlContext.ConnectMode)
			{
				case 0:
				case 1:
				case 2:
				case 3:
				{
					wmmpSendbyGPRS(pduContext);
				}
				break;
				
				case 4:
				{
					wmmpSendbySMS(pduContext);
				}
				break;
				
				default:
				{
					wmmpSendbyGPRS(pduContext);
				}
					break;
			}
		}
		
		break;
		
		default:
		{
			wmmpSendbyGPRS(pduContext);
		}
			break;
	}

	

}


bool  wmmpRegisterReq(WmmpTransType_e TransTypeValue)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	CommandInfo_t CommandInfo; 
	TLVRetVal_t TLVret;
	bool rett = false;
	WmmpTermStatusContext_t  *StatusContext_p = &(mainContext_p->StatusContext);
	WmmpSecurityContext_t  *SecurityContext_p = &(mainContext_p->SecurityContext);	

    WMMP_LIB_DBG_TEMP("wmmpRegisterReq");
	
	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	strcpy(StatusContext_p->TerminalSeriesNum,(char*)UNREGISTER_TERMINAL_NUM); 
	memset(SecurityContext_p->BindIMSIOrig,0,sizeof(SecurityContext_p->BindIMSIOrig));
	ReadTLV(0xE014,&TLVret);/* CBC@20100402:0xE014 -- 原IMSI  号码,如果不为空 表示该卡已注册*/

        mainContext_p->Wmmp_status.register_status = WMMP_REGISTER;
            
	if (*(TLVret.addr) == 0)
	{
		WMMP_LIB_DBG_TEMP("REGISTER:init IMsI is not exit\n");
		
		CommandInfo.CommandID = REGISTER_COMMAND;
		CommandInfo.SecuritySet =0x80;
		CommandInfo.TransType = WMMP_TRANS_DEFAULT;
		CommandInfo.PDUUsage = WMMP_PDU_ATCOMMAND;
		CommandInfo.TransType = TransTypeValue;
		
		if(strcmp((char *)mainContext_p->StatusContext.TerminalSeriesNum,mainContext_p->CustomContext.UnregTerminalNum)==0)
		{	
			/* 申请平台分配 终端序列号*/
			WMMP_LIB_DBG_TEMP("ask for 	the equitment sequence from m2m platform\n");
			CommandInfo.CommandExInfo.RegisterInfo.operation=0;		
			startRegisterForKeyTimer();/* cbc@20100525 密码下发超时定时器 */
		}
		else	
		{
			ReadTLV(0xE025,&TLVret); /*/should be edit,0xE025 -- 上行介入密码*/
			
			if (TLVret.length==0)
			{
				/* 未预置上行接入密码  */
				 CommandInfo.SecuritySet =0xc0; /*0; --  11000000 */ /* 20100408@cbc 注册时请求下发介入密码*/
				WMMP_LIB_DBG_TEMP("not 	being set uplink pin\n");
				CommandInfo.CommandExInfo.RegisterInfo.operation=1;
				startRegisterForKeyTimer();//* cbc@20100525 密码下发超时定时器 */
			}
			else
			{
				/*已预置上行接入密码 */
				WMMP_LIB_DBG_TEMP("have being set uplink pin\n");
				CommandInfo.CommandExInfo.RegisterInfo.operation=3;
			}			
		}		

		rett = true;
		
	}
	else
	{
		char Imsi_bind[17];
		char Imsi[17];
		memset(Imsi_bind,0,sizeof(Imsi_bind));
		memset(Imsi,0,sizeof(Imsi));

		memcpy(Imsi_bind,TLVret.addr,TLVret.length);

		//read current IMSI
		ReadTLV(0xE012,&TLVret);/* 0xE012 -- 当前SIM卡的IMSI  号码 */
		memcpy(Imsi,TLVret.addr,TLVret.length);

		WMMP_LIB_DBG_TEMP("init IMsI is exit\n");

		if(strcmp((char *)Imsi_bind,(char *)Imsi)!=0)
		{
			if((mainContext_p->SecurityContext.BindParaActive==0)||(mainContext_p->SecurityContext.BindParaActive==2))
			{
				/* imei imsi 终端序列号申请变更，属于注册的一种 */
		    		CommandInfo.CommandID = REGISTER_COMMAND;
		    		CommandInfo.SecuritySet =0x80;
				CommandInfo.TransType = WMMP_TRANS_DEFAULT;
				CommandInfo.PDUUsage = WMMP_PDU_ATCOMMAND;
				CommandInfo.CommandExInfo.RegisterInfo.operation=2;/**/
				CommandInfo.TransType = TransTypeValue;
				rett = true;
				
				WMMP_LIB_DBG_TEMP("imei-imsi-equitment changed\n");
			}
			else
			{
			    WMMP_LIB_DBG_TEMP("Register  command ERR1\n");

				rett = false;
			}
		}
		else
		{
			/*本地人工清除上行接入密码,本地人工清除下行接入密码,本地人工清除基础密钥 */
				if(mainContext_p->SecurityContext.M2MPassClearPara==0x0E)
				{
					WMMP_LIB_DBG_TEMP("login:SecurityContext.M2MPassClearPara==0x0E\n");
			    		CommandInfo.CommandID = REGISTER_COMMAND;
			    		CommandInfo.SecuritySet = 0xc0;  //0; --  11000000 /* 20100408@cbc 注册时请求下发介入密码*/
					CommandInfo.TransType = WMMP_TRANS_DEFAULT;
					CommandInfo.PDUUsage = WMMP_PDU_ATCOMMAND;
					CommandInfo.CommandExInfo.RegisterInfo.operation=4;			
					CommandInfo.TransType = TransTypeValue;
					rett = true;
				}
				else
				{
					WMMP_LIB_DBG_TEMP("Register  command ERR2\n");

					rett = false;
				}
		}
	}


	if (rett == true)
	{
		WMMP_LIB_DBG_TEMP("Register  command SaveToCommandBuffer\n");
		SaveToCommandBuffer(&CommandInfo);
	}
	
	return rett;
	
}

bool  wmmpLoginReq(WmmpTransType_e TransTypeValue)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	CommandInfo_t CommandInfo; 
	TLVRetVal_t TLVret;
	bool rett = true;

	
	WMMP_LIB_DBG_TEMP("wmmpLoginReq LoginFaildCount %d TransTypeValue %d PDUUsage %d",mainContext_p->LoginFaildCount,TransTypeValue,mainContext_p->PDUUsage);
	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	CommandInfo.CommandID = LOGIN_COMMAND;
	//CommandInfo.SecuritySet = 224;//11100000
	CommandInfo.SecuritySet = 0x80;
	CommandInfo.PDUUsage = WMMP_PDU_ATCOMMAND;
	CommandInfo.TransType = WMMP_TRANS_DEFAULT;
	CommandInfo.TransType = TransTypeValue;
	SaveToHighCommandBuffer(&CommandInfo);		
	

	return rett;	
}

void wmmpProcessErrorRoll(void)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	WmmpTermStatusContext_t  *StatusContext_p = &(mainContext_p->StatusContext);
	WmmpSecurityContext_t  *SecurityContext_p = &(ptrToWmmpContext()->SecurityContext);
	WPIEventData_t WPIEventData;
	PDUContext_t   *pduContext = &mainContext_p->PDUContext;  
	CommandInfo_t CommandInfo;
	WPIRtcDateAndTime dateAndTime;
	WPIRtcStatus_e      rtcStatus;
	u32			curuTime = 0;
	bool				rett	=	true;
	WmmpUpdateContext_t  *UpdateContext_p = &(mainContext_p->UpdateContext);

	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	WMMP_LIB_DBG_TEMP("wmmpProcessErrorRoll CommandID %d PDUUsage %d",pduContext->CommandID,mainContext_p->PDUUsage);
	
	switch (pduContext->CommandID)
	{
		case LOGOUT_COMMAND:    /*bob add 20110106*/
		{
			WMMP_LIB_DBG_TEMP("LOGOUT 	fail");						
			RollClearCommandBuffer();
			FreeWMMPPDUContext();
			mainContext_p->Wmmp_status.protocol_status =  PIDLE;			
			memset(mainContext_p->TimeStamp,0,4);
			wmmpStopInterTimer(HEART_BEAT_TIMER);
			wmmpStopInterTimer(SHORT_CONNECT_MODE_TIMER);
			wmmpStopInterTimer(SIGMA_STAT_TIMER);
			wmmpStopInterTimer(STAT_TIMER);
			//登出返回永远是idle
			  WPIEventData.WPIStatusIndEvent.type = 1;
			WPIEventData.WPIStatusIndEvent.value= PIDLE;	
			WPIEventData.WPIStatusIndEvent.reason = mainContext_p->PDUUsage;/*  	CBC@超时强制登出*/
			 (mainContext_p->WPIEventFucPtr)(WPI_STATUS_IND_EVENT,&WPIEventData);		
						
		}		
		break;	
		case REGISTER_COMMAND:			
		{		
			wmmpStopInterTimer(PDU_INTERVAL_TIMER);
			wmmpStopInterTimer(HEART_BEAT_TIMER);
			wmmpStopInterTimer(SHORT_CONNECT_MODE_TIMER);
			wmmpStopInterTimer(SIGMA_STAT_TIMER);
			wmmpStopInterTimer(STAT_TIMER);	
			RollClearCommandBuffer();
			FreeWMMPPDUContext();
			wmmpProcessTrapAbortWarnTimer();			
			mainContext_p->maxTrapAbortWarn=0;
#if 0   /*bob remove 20101230*/			
			curuTime = WPI_GetCurrentTime(&dateAndTime, &rtcStatus);	
			WMMP_LIB_DBG_TEMP("wmmpProcessErrorRoll be ReLogin curuTime %d autoLoginuTime %d",curuTime,mainContext_p->autoLoginuTime);
			mainContext_p->autoLoginuTime = curuTime + 24*60*60;
			WMMP_LIB_DBG_TEMP("wmmpProcessErrorRoll be ReLogin autoLoginuTime %d",mainContext_p->autoLoginuTime);
#endif			
			//WPI_StopTimer();  /*bob remove 20101207*/
		}
		break;

		case LOGIN_COMMAND:			
		{			
			wmmpStopInterTimer(PDU_INTERVAL_TIMER);
			wmmpStopInterTimer(HEART_BEAT_TIMER);
			wmmpStopInterTimer(SHORT_CONNECT_MODE_TIMER);
			wmmpStopInterTimer(SIGMA_STAT_TIMER);
			wmmpStopInterTimer(STAT_TIMER);	
			RollClearCommandBuffer();
			FreeWMMPPDUContext();
			wmmpProcessTimingRegisterTimer();								
		}											
		break;
		case HEART_BEAT_COMMAND:			
		{		
			wmmpStopInterTimer(PDU_INTERVAL_TIMER);
			RollClearCommandBuffer();
			FreeWMMPPDUContext();
			wmmpStopInterTimer(HEART_BEAT_TIMER);
			wmmpProcessTimingLoginTimer();
		}											
		break;
		case FILE_REQ_COMMAND:
		{	
			char disbuf[250];
			rett = WPA_adFileReq((char *)&UpdateContext_p->TransId, 3, 0xFFFFFFFF,(u16)0);
			if(!rett)
			{
				WMMP_LIB_DBG_TEMP("wmmpProcessErrorRoll FILE_REQ_COMMAND failed 3333333");
				memset(disbuf,0,sizeof(disbuf));
				//snprintf(disbuf,sizeof(disbuf),"\r\n abort");
				snprintf(disbuf,sizeof(disbuf),"$M2MMSG:6,2,\"aborted\"");
				WPI_send_urc((char *)disbuf);	
			}
		}
		break;
		default:
		#if 0
		RollClearCommandBuffer();
		FreeWMMPPDUContext();
		#else
		//RollClearCommandBuffer();
		//FreeWMMPPDUContext();
		wmmpSendNextCommand();
		#endif
		break;
	}  
	if(strcmp(StatusContext_p->TerminalSeriesNum,(char*)UNREGISTER_TERMINAL_NUM) == 0)
	{		
		memset(SecurityContext_p->BindIMSIOrig,0,sizeof(SecurityContext_p->BindIMSIOrig));
	}
	
}


void wmmpProcessError(void)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	WPIEventData_t WPIEventData;
	PDUContext_t   *pduContext = &mainContext_p->PDUContext;  
	CommandInfo_t CommandInfo;  
	
	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	WMMP_LIB_DBG_TEMP("wmmpProcessError CommandID %d PDUUsage %d",pduContext->CommandID,mainContext_p->PDUUsage);

	//wmmpProcessErrorRoll();
	
	if(mainContext_p->WPIEventFucPtr!=PNULL)
	{
		if(mainContext_p->PDUUsage==WMMP_PDU_ATCOMMAND)
		{
        	  switch (pduContext->CommandID)
	        {
	            case REGISTER_COMMAND:
			{
			    	WPIEventData.WPIStatusIndEvent.type = 1;
				WPIEventData.WPIStatusIndEvent.value= ALARMLOGINFAILED;	/*  	CBC@登录失败 */
				WPIEventData.WPIStatusIndEvent.reason = mainContext_p->PDUUsage;
				 (mainContext_p->WPIEventFucPtr)(WPI_STATUS_IND_EVENT,&WPIEventData);	
			}
			break;
			
	            case LOGIN_COMMAND:
			{
				WPIEventData.WPIStatusIndEvent.type = 1;
				WPIEventData.WPIStatusIndEvent.value= ALARMLOGINFAILED;	/*  	CBC@登录失败 */
				WPIEventData.WPIStatusIndEvent.reason = mainContext_p->PDUUsage;
				 (mainContext_p->WPIEventFucPtr)(WPI_STATUS_IND_EVENT,&WPIEventData);					
			}											
			break;
	            case TRANSPARENT_DATA_COMMAND:
			{
				WPIEventData.WPIDataAckEvent.ret=1;
				(mainContext_p->WPIEventFucPtr)(WPI_DATA_ACK_EVENT,&WPIEventData);	
	            	}
					
			break;
					
	            case CONFIG_TRAP_COMMAND:
			{
				WPIEventData.WPITrapAckEvent.ret=2;
				WPIEventData.WPITrapAckEvent.tag = 0;
				(mainContext_p->WPIEventFucPtr)(WPI_TRAP_ACK_EVENT,&WPIEventData);	
	            	}
			break;
					
	            case CONFIG_REQ_COMMAND:
			{
				WPIEventData.WPIReqAckEvent.ret=2;
				WPIEventData.WPIReqAckEvent.tag=0;
				(mainContext_p->WPIEventFucPtr)(WPI_REQ_ACK_EVENT,&WPIEventData);	
	            	}
			break;
					
	            case FILE_REQ_COMMAND:
			{
				WPIEventData.WPIFileAckEvent.data=PNULL;
				WPIEventData.WPIFileAckEvent.CRC16=PNULL;
				WPIEventData.WPIFileAckEvent.readpoint=0;
				WPIEventData.WPIFileAckEvent.blocksize=0;
				memset(WPIEventData.WPIFileAckEvent.transID,0,4);
				WPIEventData.WPIFileAckEvent.status=0;
				(mainContext_p->WPIEventFucPtr)(WPI_FILE_ACK_EVENT,&WPIEventData);	
	            	}
	            	break;

			default:
				break;
					
	        }   
		}
		else if(pduContext->CommandID==LOGOUT_COMMAND)
		{
			WMMP_LIB_DBG_TEMP("LOGIN_COMMAND:login 	fail, maxPDUTimeOutCount=%d\n",mainContext_p->maxPDUTimeOutCount);			
						
			ClearCommandBuffer();
			//FreeWMMPPDUContext();
			mainContext_p->Wmmp_status.protocol_status =  PIDLE;
			mainContext_p->maxPDUTimeOutCount=0;
			memset(mainContext_p->TimeStamp,0,4);
			wmmpStopInterTimer(HEART_BEAT_TIMER);
			wmmpStopInterTimer(SHORT_CONNECT_MODE_TIMER);
			wmmpStopInterTimer(SIGMA_STAT_TIMER);
			wmmpStopInterTimer(STAT_TIMER);
			//登出返回永远是idle
			  WPIEventData.WPIStatusIndEvent.type = 1;
			WPIEventData.WPIStatusIndEvent.value= PIDLE;	
			WPIEventData.WPIStatusIndEvent.reason = mainContext_p->PDUUsage;/*  	CBC@超时强制登出*/
			 (mainContext_p->WPIEventFucPtr)(WPI_STATUS_IND_EVENT,&WPIEventData);		
			//wmmpSendLogoutCnf(VG_CI_TASK_ID, 1);			
		}
	}
	 WMMP_LIB_DBG_TEMP("wmmpProcessError");  
}

void wmmp_status_ack_conform(u8 type,	u16 value,	u16 reason)
{
    WmmpContext_t *mainContext_p = ptrToWmmpContext();
    char buffer[256];

    WMMP_LIB_DBG_TEMP("wmmp_status_ack_conform type = %d value=%d reason=%d", type, value, reason);

    switch(value)
    {
        case REGISTERSUCCUSSFUL:
            {		
                mainContext_p->terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
                mainContext_p->last_terminal_run_status = STATUS_REGISTER_ACK; 			
                break;
            }
        case PIDLE:
        case REGISTERFAIL:
            {			
                mainContext_p->terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
                mainContext_p->last_terminal_run_status = STATUS_LOGOUT_ACK_COMMAND; 			
                break;
            }
        case LOGINOK:
            {			
                mainContext_p->terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
                mainContext_p->last_terminal_run_status = STATUS_LOGIN_ACK_COMMAND; 			
                break;
            }
        case ALARMLOGINFAILED:
            {			
                mainContext_p->terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
                mainContext_p->last_terminal_run_status = STATUS_LOGIN_ACK_COMMAND; 			
                break;
            }
        default:
            {
                break;
            }
    }

    if (type == 1)
    {				
        memset(buffer,0,sizeof(buffer));
        PrintCurrentProtocolStatus(value,buffer);
        WPI_send_urc((u8 *)buffer);
    }    
}

void wmmp_req_ack_conform(u8 ret,u16 tag)
{

	char disbuf[100];	
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();	

	mainContext_p->terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
	mainContext_p->last_terminal_run_status = STATUS_CONFIG_REQ_ACK_COMMAND; 

	WMMP_LIB_DBG_TEMP( "wmmp_req_ack_conform ret =%d tag =%d\n", ret, tag);

	memset(disbuf,0,sizeof(disbuf));
	snprintf(disbuf,sizeof(disbuf),"\r\n$M2MACK:%d",Transaction_Id++);
	WPI_send_urc((u8 *)disbuf);

	switch(ret)
	{
		case 0:
		{
			memset(disbuf,0,sizeof(disbuf));
			snprintf(disbuf,sizeof(disbuf),"OK");
			WPI_send_urc((u8 *)disbuf);
			break;
		}
		case 1:
		{
			memset(disbuf,0,sizeof(disbuf));
			snprintf(disbuf,sizeof(disbuf),"1,%04X\r\nERROR",tag);
			WPI_send_urc((u8 *)disbuf);
			break;
			
		}	
		default:
		{
			memset(disbuf,0,sizeof(disbuf));
			snprintf(disbuf,sizeof(disbuf),"ERROR");
			WPI_send_urc((u8 *)disbuf);
			break;
		}
	}   
}

void wmmp_trap_ack_conform(u8 ret,u16 tag)
{
	char disbuf[100];
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();	

	mainContext_p->terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
	mainContext_p->last_terminal_run_status = STATUS_CONFIG_TRAP_ACK_COMMAND; 

	WMMP_LIB_DBG_TEMP("wmmp_trap_ack_conform start\n" );

	memset(disbuf,0,sizeof(disbuf));
	snprintf(disbuf,sizeof(disbuf),"\r\n$M2MACK:%d",Transaction_Id++);
	WPI_send_urc((u8 *)disbuf);

	switch(ret)
	{
		case 0:
		{
			memset(disbuf,0,sizeof(disbuf));
			snprintf(disbuf,sizeof(disbuf),"OK");
			WPI_send_urc((u8 *)disbuf);
			break;
		}
		case 1:
		{
			memset(disbuf,0,sizeof(disbuf));
			snprintf(disbuf,sizeof(disbuf),"1,%04X\r\nERROR",tag);
			WPI_send_urc((u8 *)disbuf);
			break;			
		}
		case 2:
		default:
		{
			memset(disbuf,0,sizeof(disbuf));
			snprintf(disbuf,sizeof(disbuf),"ERROR");
			WPI_send_urc((u8 *)disbuf);
			break;
		}
	}
}


void wmmp_data_ack_conform(u8 ret)
{
	char disbuf[100];

	WMMP_LIB_DBG_TEMP( "wmmp_data_ack_conform start\n" );

	memset(disbuf,0,sizeof(disbuf));
	snprintf(disbuf,sizeof(disbuf),"\r\n$M2MACK:%d",Transaction_Id++);
	WPI_send_urc((u8 *)disbuf);

	switch(ret)
	{
		case 0:
		{
			memset(disbuf,0,sizeof(disbuf));
			snprintf(disbuf,sizeof(disbuf),"OK");
			WPI_send_urc((u8 *)disbuf);
			break;
		}		
		default:
		{
			memset(disbuf,0,sizeof(disbuf));
			snprintf(disbuf,sizeof(disbuf),"%d\r\nERROR",ret);
			WPI_send_urc((u8 *)disbuf);
			break;
		}
	}
}

void wmmp_file_ack_conform(char* data,	char* CRC16,	u32 readpoint,u16 blocksize,char* transID,u8 status)
{
	char disbuf[100];
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();	

	mainContext_p->terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
	mainContext_p->last_terminal_run_status = STATUS_FILE_REQ_ACK_COMMAND; 
	
	WMMP_LIB_DBG_TEMP( "wmmp_file_ack_conform start\n" );

	memset(disbuf,0,sizeof(disbuf));
	snprintf(disbuf,sizeof(disbuf),"\r\n$M2MACK:%d",Transaction_Id++);
	WPI_send_urc((u8 *)disbuf);	
	
	memset(disbuf,0,sizeof(disbuf));
	snprintf(disbuf,sizeof(disbuf),"%02X%02X%02X%02X,%d,%02X%02X,%d,%d",
            transID[0],transID[1],transID[2],transID[3],status,CRC16[0],CRC16[1],	
            readpoint,blocksize);
	WPI_send_urc((u8 *)disbuf);
	
	if(status==0)
	{
		memset(disbuf,0,sizeof(disbuf));
		snprintf(disbuf,sizeof(disbuf),"\r\nOK");
		WPI_send_urc((u8 *)disbuf);
	}
	else
	{
		memset(disbuf,0,sizeof(disbuf));
		snprintf(disbuf,sizeof(disbuf),"\r\nERROR");
		WPI_send_urc((u8 *)disbuf);
	}
}

void wmmpSetConform(u8 ret,u16 *tags, u8 tagnum)
{
	char str[100];
	char *str_p;
	char temp[5];
	u8 i;

	memset(str,0,sizeof(str));
	memset(temp,0,sizeof(temp));
	str_p=str;	
	//print info to the CI
	if(ret==0)
	{
		strcpy(str_p,(const char*)"\r\n$M2MMSG:2,");
		str_p+=10;

		for(i=0;i<tagnum;i++)
		{

			if(i==tagnum-1)
			{
				sprintf(temp, "%04X", tags[i]);
				memcpy(str_p, temp, 4);
				str_p += 4;
			}
			else
			{
				sprintf(temp, "%04X,", tags[i]);
				memcpy(str_p, temp, 5);
				str_p += 5;
			}
		}	
		WPI_send_urc((u8 *)str);
	}
}

void wmmpRemoteConform(u8 ret,u8 paramId, u16 *errortag, u8 errortagnum)
{
	char disbuf[100];
	
	WMMP_LIB_DBG_TEMP( "wmmpRemoteConform \n" );

	memset(disbuf,0,sizeof(disbuf));
	snprintf(disbuf,sizeof(disbuf),"\r\n$M2MMSG:3,%d",paramId);
	WPI_send_urc((u8 *)disbuf);
}

void wmmpDownConform(char* url,u8 ret)
{
	char disbuf[100];
	
	WMMP_LIB_DBG_TEMP("wmmpDownConform");

	memset(disbuf,0,sizeof(disbuf));
	snprintf(disbuf,sizeof(disbuf),"\r\n$M2MMSG:6,%s",url);
	WPI_send_urc((u8 *)disbuf);
	
}

void wmmpOpenSocketReadEvent(char *data, u16 datalength, u16 index)
{
    char *HexBuf=PNULL;
    char disbuf[100];
    u16 head_length=0;
    u32 i;

    memset(disbuf,0,sizeof(disbuf));
    /*index 从0～MAX_WMMP_APP_NUM-1，APPOPEN 从0～MAX_WMMP_APP_NUM-1*/
    snprintf(disbuf,sizeof(disbuf),"\r\nWPI_APPSOCKET_READ_EVENT\r\n$M2MMSG:7,%d,%d\r\n", index, datalength);
    head_length=strlen(disbuf);

    if(datalength!=0)
    {
        if((bhvValue[5] == 0))
        {
            HexBuf =(char*)WPI_Malloc(head_length+datalength*2+1);		
            memset(HexBuf,0,head_length+datalength*2+1);		
            StringToHexCode(data,&HexBuf[head_length],datalength);
        }
        else
        {
            HexBuf = (char*)WPI_Malloc(head_length+datalength+1);		
            memset(HexBuf,0,head_length+datalength+1);	
            memcpy(&HexBuf[head_length],data,datalength); 
        }
        for(i=0;i<head_length;i++)
        {
            HexBuf[i]=disbuf[i];
        }
        WPI_WriteUartData(HexBuf,strlen(HexBuf));
        WPI_Free(HexBuf);
    }
}

void wmmpSendServerDataConform(char type, char* des, char *data, u16 datalen)
{
	char*	str;
	char disbuf[300];
	char datatmp[210] = {0};
	
	if( (des == NULL)||(data == NULL))
	{
		WPI_send_urc((u8 *)"\r\n$M2MMSG:4, DATA REQ  ERROR");
		return;
	}
	
	if( datalen >=200)
	{
		memcpy(datatmp,data,200);
		datatmp[201] = '.';
		datatmp[202] = '.';
		datatmp[203] = '.';
		datatmp[204] = 0;
	}
	else
	{
		memcpy(datatmp,data,datalen);
		datatmp[datalen] = 0;
	}	
	
	memset(disbuf,0,sizeof(disbuf));
	snprintf(disbuf,sizeof(disbuf),"\r\n$M2MMSG:4,%c,%s,%s,%d",type,des,datatmp,datalen);
	WPI_send_urc((u8 *)disbuf);
	
	
	if(datalen!=0)
	{

		str = (char*)WPI_Malloc(datalen*2+1);		
		memset(str,0,datalen*2+1);	
		
	      StringToHexCode(data,str,datalen);
		WPI_send_urc((u8 *)str);
		
		WPI_Free(str);
	}	
}

#if 0
void wmmpProcessWPIEvent(WPIEventType Event, WPIEventData_t * EventData)
{
	char buffer[128]={0};
	u8		ret = 0;
	u16	tag = 0;

	WMMP_LIB_DBG_TEMP("wmmpProcessWPIEvent   Event  %d ",Event);
	switch(Event)
	{
		case WPI_STATUS_IND_EVENT:
		{
			sprintf(buffer,"$M2MMSG:%d,%04X ",EventData->WPIStatusIndEvent.type,EventData->WPIStatusIndEvent.value);
			WPI_send_urc((u8 *)buffer);
			wmmp_status_ack_conform(EventData->WPIStatusIndEvent.type,EventData->WPIStatusIndEvent.value,EventData->WPIStatusIndEvent.reason);
		}
		break;

		case WPI_REQ_ACK_EVENT:
		{
			WPI_send_urc((u8 *)"WPI_REQ_ACK_EVENT");
			wmmp_req_ack_conform(EventData->WPIReqAckEvent.ret,EventData->WPIReqAckEvent.tag);
		}
		break;	

		case WPI_TRAP_ACK_EVENT:
		{
			ret = EventData->WPITrapAckEvent.ret ;
			tag = EventData->WPITrapAckEvent.tag;

			WPI_send_urc((u8 *)"WPI_TRAP_ACK_EVENT");
			wmmp_trap_ack_conform(EventData->WPITrapAckEvent.ret,EventData->WPITrapAckEvent.tag);
		}
		break;	

		case WPI_DATA_ACK_EVENT:
		{
			WPI_send_urc((u8 *)"WPI_DATA_ACK_EVENT");
			wmmp_data_ack_conform(EventData->WPIDataAckEvent.ret);
		}
		break;	

		case WPI_FILE_ACK_EVENT:
		{
			WPI_send_urc((u8 *)"WPI_FILE_ACK_EVENT");

			wmmp_file_ack_conform(EventData->WPIFileAckEvent.data,
			EventData->WPIFileAckEvent.CRC16,
			EventData->WPIFileAckEvent.readpoint,
			EventData->WPIFileAckEvent.blocksize,
			EventData->WPIFileAckEvent.transID,
			EventData->WPIFileAckEvent.status);			

			if (EventData->WPIFileAckEvent.data)
			{
				WPI_Free(EventData->WPIFileAckEvent.data);
			}
		}
		break;	

		case WPI_SET_REQ_EVENT:
		{
			WMMP_LIB_DBG_TEMP("wmmpProcessWPIEvent  WPI_SET_REQ_EVENT free  Event  %d ",Event);
			WPI_send_urc((u8 *)"WPI_SET_REQ_EVENT");
			wmmpSetConform(EventData->WPISetReqEvent.ret, EventData->WPISetReqEvent.tags, EventData->WPISetReqEvent.tagnum);
			WPI_Free (EventData->WPISetReqEvent.tags);
		}
		break;

		case WPI_REMOTE_REQ_EVENT:
		{
			WPI_send_urc((u8 *)"WPI_REMOTE_REQ_EVENT");

			wmmpRemoteConform(EventData->WPIRemoteReqEvent.ret,
			EventData->WPIRemoteReqEvent.paramId,
			EventData->WPIRemoteReqEvent.errortag,
			EventData->WPIRemoteReqEvent.errortagnum);
		}
		break;

		case WPI_DOWN_REQ_EVENT:
		{
			WPI_send_urc((u8 *)"WPI_DOWN_REQ_EVENT");
			wmmpDownConform(EventData->WPIDownReqEvent.url,EventData->WPIDownReqEvent.ret);
			WPI_Free(EventData->WPIDownReqEvent.url);
		}
		break;

		case WPI_DATA_REQ_EVENT:
		{
			WPI_send_urc((u8 *)"WPI_DATA_REQ_EVENT");

			wmmpSendServerDataConform(EventData->WPIDataReqEvent.type,
			EventData->WPIDataReqEvent.des,
			EventData->WPIDataReqEvent.data,
			EventData->WPIDataReqEvent.datalen);

			WPI_Free(EventData->WPIDataReqEvent.data)	;			
		}
		break;

		case WPI_SOCKET_CREATE_EVENT:
		{
			WMMP_LIB_DBG_TEMP("wmmpProcessWPIEvent  WPI_SOCKET_CREATE_EVENT  ");
		}
		break;

		case WPI_APPSOCKET_READ_EVENT:
		{
			wmmpOpenSocketReadEvent(EventData->WPIAppSockReadEvent.data,EventData->WPIAppSockReadEvent.datalength,EventData->WPIAppSockReadEvent.linkindex );
		}
		break;

		default:
		break;
	}
}
#endif

void WPA_StackInit(void)
{
	u16   loopi;
	WmmpContext_t *mainContext_p = ptrToWmmpContext();
        
	WMMP_LIB_DBG_TEMP("WPA_StackInit");
	memset((void*)mainContext_p, 0x00, sizeof(WmmpContext_t));

	mainContext_p->wmmpSocket = CONNREF_INVALID;

	mainContext_p->Wmmp_status.net_status = LINK_FAILED;
	mainContext_p->Wmmp_status.protocol_status = PIDLE;
	mainContext_p->Wmmp_status.param_status = PARAMSYNC;


	mainContext_p->m2mswitch = false;

	mainContext_p->TransType = WMMP_TRANS_DEFAULT;	

	mainContext_p->enableSend = true;
	

	/* add by cbc@0091216: 记住当前终端和M2M平台各主动发起的命令运行状态:BEGIN*/
	mainContext_p->terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
	mainContext_p->last_terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
	mainContext_p->platform_run_status = M2M_PLATFORM_RUN_STATUS_INIT;
	/* add by cbc@0091216: 记住当前终端和M2M平台各主动发起的命令运行状态:end*/
	mainContext_p->StackState = WMMP_STACK_INIT;   /*bob add 20101123*/
	//v_wmmp_ChannelId = v_acs_ChannelId;   /*20120508 for compile*/
	WPA_AppInit();
}

/*****************************************************************************
*	函数名	      : WPA_Init
* 作  者        : 
*	功能描述      : 开启M2M功能，主要是使SIM卡OK并置位m2mswitch
*	输入参数说明  : 无
* 输出参数说明  : 无
*	返回值说明    : 无
*****************************************************************************/
bool WPA_Init(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	WmmpContext_t *mainContext_p = ptrToWmmpContext();	
	WPAEventData_t WPAEvent;
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	WMMP_LIB_DBG_TEMP("WPA_Init:sim card state is %d",WPI_GetSimState());

	if(mainContext_p->m2mswitch == false)
	{
		switch(WPI_GetSimState())
		{
			case WMMP_SECURITY_SIM_STATE_PIN:
			{
				//有PIN码，自动解PIN
				if((strlen((char *)mainContext_p->SecurityContext.SIMPin1)!=0)&&(mainContext_p->SecurityContext.SIMPin1Active==1))
				{					
					if (WPI_SecurityAutoCpin(mainContext_p->SecurityContext.SIMPin1) == true)
					{
						WPAEvent.WPACPinEvent.error = WMMP_SECURITY_OPER_ERROR_OK;
					}
					else
					{
						WPAEvent.WPACPinEvent.error = WMMP_SECURITY_OPER_ERROR_OTHER;
					}
					WPAEvent.WPACPinEvent.operation = WMMP_SECURITY_OPER_AUTO;	
					WPA_ProcessEvent(WPA_CPIN_EVENT, &WPAEvent);
				}
				
			}
			break;
			
			case WMMP_SECURITY_SIM_STATE_OTHER:
			case WMMP_SECURITY_SIM_STATE_OK:
			{
				mainContext_p->m2mswitch = true; /*打开M2M功能*/
				WPI_StartTimer(1000); /*启动1s一次的定时器，只要M2M在跑，1S触发一次事件*/
			}
			break;
			
			default:
				break;
		}
		return true;
	}
	else
		return false;
	
}
/*****************************************************************************
*	函数名	      : WPA_Term
* 作  者        : 
*	功能描述      : 关闭M2M功能
*	输入参数说明  : 无
* 输出参数说明  : 无
*	返回值说明    : 无
*****************************************************************************/
bool WPA_Term(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	CommandInfo_t CommandInfo;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	WMMP_LIB_DBG_TEMP("WPA_Term:m2mswitch %d",mainContext_p->m2mswitch);
	WMMP_LIB_DBG_TEMP("WPA_Term:protocol_status %d",mainContext_p->Wmmp_status.protocol_status);
	
	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	
	if((WPI_GetSimState()==WMMP_SECURITY_SIM_STATE_OK)&&(mainContext_p->m2mswitch==true))
	{
		if(mainContext_p->Wmmp_status.protocol_status==LOGINOK)
		{
			    /*发送LOGOUT COMMAND*/
			    WMMP_LIB_DBG_TEMP("WPA_Term:LOGOUT_COMMAND");
			    CommandInfo.CommandID = LOGOUT_COMMAND;
			    CommandInfo.SecuritySet = 0;
			    CommandInfo.TransType = WMMP_TRANS_DEFAULT;
			    CommandInfo.CommandExInfo.LoginOutInfo.reason=1;
			    CommandInfo.PDUUsage = WMMP_PDU_LOGOUT_TERM;
			    SaveToCommandBuffer(&CommandInfo);
		}
		else
		{
				WMMP_LIB_DBG_TEMP("WPA_Term:1");
				WPI_StopTimer();
				
				mainContext_p->m2mswitch = false;
				
				wmmpLibDectiveGprs();
		}
	}
	else
	{
		WMMP_LIB_DBG_TEMP("WPA_Term:2");
		WPI_StopTimer();
		mainContext_p->m2mswitch = false;
		wmmpLibDectiveGprs();		/*bob add 20110331*/
	}

	mainContext_p->terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
	mainContext_p->last_terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
	
	return true;
}

/*****************************************************************************
*	函数名	      : WPA_LoginReq
* 作  者        : 
*	功能描述      : 登录
*	输入参数说明  : 无
* 输出参数说明  : 无
*	返回值说明    : 无
*****************************************************************************/
bool WPA_LoginReq(void)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	CommandInfo_t CommandInfo; 
	TLVRetVal_t TLVret;
	WPIEventData_t WPIEventData;

	WMMP_LIB_DBG_TEMP("WPA_LoginReq");

	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	/* simcom cbc@20091222 for :deal with ternimal wmmp run status  begin*/
	if (mainContext_p->terminal_run_status != M2M_TERMINAL_RUN_STATUS_INIT)
	{
		WMMP_LIB_DBG_TEMP("login fail:terminal_run_status=0x%x",mainContext_p->terminal_run_status);
		return false;
	}
	else
	{
		mainContext_p->terminal_run_status = STATUS_LOGIN_COMMAND;		
	}
	/* simcom cbc@20091222 for :deal with ternimal wmmp run status  end*/
	
	ReadTLV(0xE014,&TLVret);/* CBC@20100402:0xE014 -- 原IMSI  号码,如果不为空 表示该卡已注册*/
	
	WMMP_LIB_DBG_TEMP("imei-imsi-equitment changed,BindParaActive =%d",mainContext_p->SecurityContext.BindParaActive);
	
	if((WPI_GetSimState()==WMMP_SECURITY_SIM_STATE_OK)&&(mainContext_p->m2mswitch==true))
	{
		//check if changed the sim card,use 0xE014 to check whether registed
		
		//the length read from 0xe014 is alway 16,so check the first byte
		if (*(TLVret.addr)!=0) //the card has registed
		{
			char Imsi_bind[17];
			char Imsi[17];
			memset(Imsi_bind,0,sizeof(Imsi_bind));
			memset(Imsi,0,sizeof(Imsi));
			
			memcpy(Imsi_bind,TLVret.addr,TLVret.length);

			//read current IMSI
			ReadTLV(0xE012,&TLVret);/* 0xE012 -- 当前SIM卡的IMSI  号码 */
			memcpy(Imsi,TLVret.addr,TLVret.length);
			
			WMMP_LIB_DBG_TEMP("init IMsI is exist:Imsi_bind=%s,Imsi=%s",Imsi_bind,Imsi);
		
			if(strcmp((char *)Imsi_bind,(char *)Imsi)!=0)
			{
				if((mainContext_p->SecurityContext.BindParaActive==0)||(mainContext_p->SecurityContext.BindParaActive==2))
				{
					/* imei imsi 终端序列号申请变更，属于注册的一种 */
			    		CommandInfo.CommandID = REGISTER_COMMAND;
			    		CommandInfo.SecuritySet =0x80;
					CommandInfo.TransType = WMMP_TRANS_DEFAULT;
					CommandInfo.PDUUsage = WMMP_PDU_ATCOMMAND;
					CommandInfo.CommandExInfo.RegisterInfo.operation=2;/**/
					wmmpStartInterTimer(TIMING_REGISTER_TIMER, 90);
					WMMP_LIB_DBG_TEMP("imei-imsi-equitment changed");
				}
				else if(mainContext_p->SecurityContext.BindParaActive==1)
				{
					//若机卡互锁，告警
					
					/*
					WPI_PutNewLine();
					WPI_Printf((const char*)" SIM card error!!");
					WPI_PutNewLine();*/
					WMMP_LIB_DBG_TEMP("sim 	card locked with equitment");
					return false;
				}
			}
			else
			{
				/*本地人工清除上行接入密码,本地人工清除下行接入密码,本地人工清除基础密钥 */
				if(mainContext_p->SecurityContext.M2MPassClearPara==0x0E)    /*    AT$M2MTLV=E020,"14"    */
				{
					WMMP_LIB_DBG_TEMP("login:SecurityContext.M2MPassClearPara==0x0E");
			    		CommandInfo.CommandID = REGISTER_COMMAND;
			    		CommandInfo.SecuritySet = 0xc0;  //0; --  11000000 /* 20100408@cbc 注册时请求下发介入密码*/
					CommandInfo.TransType = WMMP_TRANS_DEFAULT;
					CommandInfo.PDUUsage = WMMP_PDU_ATCOMMAND;
					CommandInfo.CommandExInfo.RegisterInfo.operation=4;			
					wmmpStartInterTimer(TIMING_REGISTER_TIMER, 90);
				}
				else if(mainContext_p->SecurityContext.M2MPassClearPara==0x80)/*本地人工清除PIN1码各AT$M2MTLV=E020,"128"    */
				{
					WMMP_LIB_DBG_TEMP("login:SecurityContext.M2MPassClearPara==0x80");
					CommandInfo.CommandID = LOGIN_COMMAND;
					CommandInfo.SecuritySet = 0x80;
					CommandInfo.TransType = WMMP_TRANS_DEFAULT;
					CommandInfo.PDUUsage = WMMP_PDU_ATCOMMAND;
					CommandInfo.CommandExInfo.LoginInfo.clearSecurity=true;		
					wmmpStartInterTimer(TIMING_LOGIN_TIMER, 90);
				}
				else/* 普通正常登录 */
				{
					WMMP_LIB_DBG_TEMP("normal login:");
                    
                                        WPI_send_urc(WMMP_AT_EVENT_STR"%d,%s %s", WMMP_AET_LIB_STATE, "0302", "\"Logging-in:normal login\"");

					CommandInfo.CommandID = LOGIN_COMMAND;
					CommandInfo.SecuritySet = 0x80;
					CommandInfo.TransType = WMMP_TRANS_DEFAULT;	
					CommandInfo.PDUUsage = WMMP_PDU_ATCOMMAND;
					wmmpStartInterTimer(TIMING_LOGIN_TIMER, 90);
				}
			}
			
		}

		
		//if(wmmpSavedSecurityContext.LoginStatus == true)

		else
		{
			WMMP_LIB_DBG_TEMP("init IMsI is not exit");
		//should be edit 
			CommandInfo.CommandID = REGISTER_COMMAND;
			CommandInfo.SecuritySet =0x80;
			CommandInfo.TransType = WMMP_TRANS_DEFAULT;
			CommandInfo.PDUUsage = WMMP_PDU_ATCOMMAND;
			if(strcmp((char *)mainContext_p->StatusContext.TerminalSeriesNum,mainContext_p->CustomContext.UnregTerminalNum)==0)
			{	
				/* 申请平台分配 终端序列号*/
				WMMP_LIB_DBG_TEMP("ask for 	the equitment sequence from m2m platform");
		    		CommandInfo.CommandExInfo.RegisterInfo.operation=0;
				startRegisterForKeyTimer();//* cbc@20100525 密码下发超时定时器 */
				wmmpStartInterTimer(TIMING_REGISTER_TIMER, 90);
			}
			else	
			{
				ReadTLV(0xE025,&TLVret); /*/should be edit,0xE025 -- 上行介入密码*/
				if (TLVret.length==0)
				{
					/* 未预置上行接入密码  */
					 CommandInfo.SecuritySet =0xc0; /*0; --  11000000 /* 20100408@cbc 注册时请求下发介入密码*/
					WMMP_LIB_DBG_TEMP("not 	being set uplink pin");
					CommandInfo.CommandExInfo.RegisterInfo.operation=1;
					startRegisterForKeyTimer();//* cbc@20100525 密码下发超时定时器 */
					wmmpStartInterTimer(TIMING_REGISTER_TIMER, 90);
				}
				else
				{
					/*已预置上行接入密码 */
					WMMP_LIB_DBG_TEMP("have being set uplink pin");
					CommandInfo.CommandExInfo.RegisterInfo.operation=3;
					wmmpStartInterTimer(TIMING_REGISTER_TIMER, 90);
				}
			}
		}
	    SaveToCommandBuffer(&CommandInfo);
	}
	else
	{
		mainContext_p->terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
		WMMP_LIB_DBG_TEMP("sim card state is=%d,m2mswitch=%d",WPI_GetSimState(),mainContext_p->m2mswitch);
		return false;
	}

        gLoginStatus = 1; 
	return true;
}

bool WPA_LogoutReq(u8 reason)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	CommandInfo_t CommandInfo;  
	WPIEventData_t WPIEventData;

	WMMP_LIB_DBG_TEMP("WPA_LogoutReq reason=%d", reason);
    
	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	/* simcom cbc@20091222 add for :deal with ternimal wmmp run status  begin*/
	if (mainContext_p->terminal_run_status != M2M_TERMINAL_RUN_STATUS_INIT)
	{
		WMMP_LIB_DBG_TEMP("login fail:terminal_run_status=0x%x\n",mainContext_p->terminal_run_status);
		return false;
	}
	else
	{
		mainContext_p->terminal_run_status = STATUS_LOGOUT_COMMAND;		
	}
	/* simcom cbc@20091222 add for :deal with ternimal wmmp run status  end*/
	
	if((WPI_GetSimState()==WMMP_SECURITY_SIM_STATE_OK)&&(mainContext_p->m2mswitch==true))
	{	
		//由模块发起的登出
		CommandInfo.CommandID = LOGOUT_COMMAND;
		CommandInfo.SecuritySet = 0;
		CommandInfo.TransType = WMMP_TRANS_DEFAULT;
		CommandInfo.PDUUsage = WMMP_PDU_ATCOMMAND;
		CommandInfo.CommandExInfo.LoginOutInfo.reason=reason;

		SaveToCommandBuffer(&CommandInfo);
		WMMP_LIB_DBG_TEMP("WPA_LogoutReq:LOGOUT_COMMAND\n" );
	}
	else
	{
		mainContext_p->terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
		return false;	
	}
	return true;
}


bool  RemoteCtrlTraceTrapReq(u16*TLVTags, u16 TLVNum)
{
	CommandInfo_t CommandInfo; 
	WPIEventData_t WPIEventData;
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();

	WMMP_LIB_DBG_TEMP("RemoteCtrlTraceTrapReq ");

	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	
	if((WPI_GetSimState()==WMMP_SECURITY_SIM_STATE_OK)&&(mainContext_p->Wmmp_status.protocol_status==LOGINOK))
	{	
		CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
		CommandInfo.SecuritySet = 0;
		CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
		CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=TLVTags;
		CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=TLVNum;
		CommandInfo.TransType = WMMP_TRANS_DEFAULT;
		CommandInfo.PDUUsage = WMMP_PDU_ATCOMMAND;
		//CommandInfo.TransType = WMMP_TRANS_SMS;	 //use SMS !  /*20110715 for jianghu debug*/
		CommandInfo.PDUUsage = WMMP_RMCTRL_TRAP_TRACE;		/*bob add 20110301*/
		SaveToCommandBuffer(&CommandInfo);
	}
	else
	{
		mainContext_p->terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
		return false;
	}
	return true;
}





bool WPA_TrapReq(u16*TLVTags, u16 TLVNum)
{
	CommandInfo_t CommandInfo; 
	WPIEventData_t WPIEventData;
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();

	WMMP_LIB_DBG_TEMP("WPA_TrapReq ");

	memset(&CommandInfo,0,sizeof(CommandInfo_t));
#if 0
	/* simcom cbc@20091222 add for :deal with ternimal wmmp run status  begin*/
	if (mainContext_p->terminal_run_status != M2M_TERMINAL_RUN_STATUS_INIT)
	{
		WMMP_LIB_DBG_TEMP("TrapReql:terminal_run_status=0x%x",mainContext_p->terminal_run_status);
		return false;
	}
	else
	{
		mainContext_p->terminal_run_status = STATUS_CONFIG_TRAP_COMMAND;		
	}
	/* simcom cbc@20091222 add for :deal with ternimal wmmp run status  end*/
#endif	
	if((WPI_GetSimState()==WMMP_SECURITY_SIM_STATE_OK)&&(mainContext_p->Wmmp_status.protocol_status==LOGINOK))
	{	
		CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
		CommandInfo.SecuritySet = 0;
		CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
		CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=TLVTags;
		CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=TLVNum;
		CommandInfo.TransType = WMMP_TRANS_DEFAULT;
		CommandInfo.PDUUsage = WMMP_PDU_ATCOMMAND;
		SaveToCommandBuffer(&CommandInfo);
	}
	else
	{
		mainContext_p->terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
		return false;
	}
	return true;
}

/*		 
1.重庆移动扩展的TLV 中value 不止一个参数，所以在标准协议基础上扩展处理
   如:300F,0025,0A	 
		  0025:外设25，0A处于0A状态,300F后有两个参数
2.重启移动扩展的AT命令包括 CONFIGSET，CONFIGSETLIST，
TRANSPARENT Data等命令要在文本和HEXA模式中切换，所以有两套处理接口
*/		 
bool WPA_ParseTrapDataUpdate(char *data, u16 length, u16 *TLVnum,u16 *TLVtag)
{
	WmmpContext_t  *mainContext_p = ptrToWmmpContext();
	WmmpTermStatusContext_t  *StatusContext_p = &(mainContext_p->StatusContext);
	WmmpCustomContext_t  *CustomContext_p = &(mainContext_p->CustomContext);
	u8 Line = 0;
	u16 TagNum;/*TagId*/
	u16 *TLV=PNULL;
	TLVRetVal_t TLV_ret;
	char	 *p = NULL;	
	char	 *TlvTmpParam[20] = {NULL};
	u8 TmpTlvLen[20] = {0};
	char	 *TlvParam[32] = {NULL};
	u16 loopi = 0;  
	bool result = true;
	char	 *TlvDataStart[20] = {NULL};
	char	 *TlvDataEnd[20] = {NULL};
	u8  tlvParamCount = 0;
	char	 *ppParam = NULL;
	p=data;

	WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate atData=%s ,length=%d",p,length);
	WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate simcom_m2m_at_len %d",simcom_m2m_at_len);	
	WPI_hex_printf(p, length);	

	//TLV = WPI_Malloc(simcom_m2m_at_len*2);/*	这个数据被函数返回后才被caller 释放*/
	TLV = TLVtag;
	memset(TLV,0,simcom_m2m_at_len*2);	
	while(Line<simcom_m2m_at_len&&p-data<length)
	{	

		tlvParamCount = 0;
		memset(TlvTmpParam,0,20);
		memset(TmpTlvLen,0,sizeof(TmpTlvLen));
		memset(TlvParam,0,sizeof(TlvParam));
		memset(TlvDataStart,0,sizeof(TlvDataStart));
		memset(TlvDataEnd,0,sizeof(TlvDataEnd));

		WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate Line %d",Line);
		WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 1 *p 0x%x",*p);
		WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 1 p-data %d",p-data);
		result = Char2Bin16(p,&TagNum);
		WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate result=%d",result);
		if(result)
		{
			p= p+4;
			WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate ***p=%s",p);
			TLV[Line] = TagNum;
		}
		else
		{
			*TLVnum = 0;
			WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 3 TLVnum 0 ret");
			TLVtag = TLV;		   
			return false;
		}				

		WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate trap TagNum 0x%04x",TagNum);


		WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate TLV[%d]  0x%04x",Line,TLV[Line]);
		WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 2 *p 0x%x",*p);
		WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 2 p-data %d",p-data);

		if(*p!=0x2C)
		{
			*TLVnum = 0;
			WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 4 TLVnum 0 ret");
			TLVtag = TLV;		   
			return false;
		}			
		else
		{
			p++;
			WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 3 *p 0x%x",*p);
			WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 3 p-data %d",p-data);	

			tlvParamCount++;
			TlvDataStart[tlvParamCount] = p;
			WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 1 *TlvDataStart[%d]  0x%02x",tlvParamCount,*TlvDataStart[tlvParamCount]);
			while(*p != 0x0a && p-data<length)
			{					 
				if(*p == 0x2C)
				{								 
					TlvDataEnd[tlvParamCount] = p;
					WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate w2 *TlvDataEnd[%d]	0x%02x",tlvParamCount,*TlvDataEnd[tlvParamCount]);
					p++;
					tlvParamCount++;
					TlvDataStart[tlvParamCount] = p;
					WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate w2 *TlvDataStart[%d]  0x%02x",tlvParamCount,*TlvDataStart[tlvParamCount]);
				}
				else
				{					 
					p++;
					WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate w1 *p 0x%x",*p);
				}
			}

			TlvDataEnd[tlvParamCount] = p;
			WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 1 *TlvDataEnd[%d]  0x%02x",tlvParamCount,*TlvDataEnd[tlvParamCount]);

		}

		WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 8 1  p-data %d",p-data);
		WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 1 tlvParamCount %d",tlvParamCount);
		
		if((p-data<length)||(Line+1==simcom_m2m_at_len))
		{
			u32 i;
			u32   charCount = 0;
			u32    tmpCount = 0;

			for(loopi = 1;loopi<tlvParamCount+1;loopi++)
			{
				WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 2 *TlvDataStart[%d]  0x%02x",loopi,*TlvDataStart[loopi]);
				if((*TlvDataStart[loopi]) == 0x22)	 /*"*/
				{
					tmpCount = (TlvDataEnd[loopi] - TlvDataStart[loopi] - 2)/2;
					TmpTlvLen[loopi] = tmpCount;
					WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 2 tmpCount %d",tmpCount);
					TlvTmpParam[loopi] =(char*)WPI_Malloc((TlvDataEnd[loopi] - TlvDataStart[loopi] - 2)/2+1);
					charCount += tmpCount;
					WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 2 charCount %d",charCount);
					memset(TlvTmpParam[loopi],0,(TlvDataEnd[loopi] - TlvDataStart[loopi] - 2)/2+1);
					for(i=0;i<(TlvDataEnd[loopi] - TlvDataStart[loopi] - 2)/2;i++)
					{								
						result = Char2Bin8((TlvDataStart[loopi]+1+i*2),(TlvTmpParam[loopi] + i));				
						WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 2 TlvTmpParam[%d] 0x%02x",i,*(TlvTmpParam[loopi] + i));
						if(!result) 			
						{
							*TLVnum = 0;
							WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 2 4 TLVnum 0 ret");
							WPI_Free(TlvTmpParam[loopi]);							
							TLVtag = TLV;		   
							return false;									
						}
					}					
				}				 
				else
				{
					tmpCount = (TlvDataEnd[loopi] - TlvDataStart[loopi])/2;
					TmpTlvLen[loopi] = tmpCount;
					WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 1 tmpCount %d",tmpCount);
					TlvTmpParam[loopi] =(char*)WPI_Malloc((TlvDataEnd[loopi] - TlvDataStart[loopi])/2+1);
					charCount += tmpCount;
					WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 1 charCount %d",charCount);
					memset(TlvTmpParam[loopi],0,(TlvDataEnd[loopi] - TlvDataStart[loopi])/2+1);
					for(i=0;i<(TlvDataEnd[loopi] - TlvDataStart[loopi])/2;i++)
					{								
						result = Char2Bin8((TlvDataStart[loopi]+i*2),(TlvTmpParam[loopi] + i)); 			
						WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 1 TlvTmpParam[%d] 0x%02x",i,*(TlvTmpParam[loopi] + i));
						if(!result) 			
						{
							*TLVnum = 0;
							WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate 1 4 TLVnum 0 ret");
							WPI_Free(TlvTmpParam[loopi]);							
							TLVtag = TLV;		   
							return false;									
						}
					}								

				}				  

			}

			WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate end charCount  %d TagNum 0x%04x ",charCount,TagNum);

			TlvParam[Line] =(char*)WPI_Malloc(charCount + 1);
			memset(TlvParam[Line],0,charCount+ 1);
			ppParam = TlvParam[Line];
			for(loopi = 1;loopi < tlvParamCount+1;loopi++)
			{
				WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate inloop1 TlvTmpParam[%d]  0x%02x",loopi,TlvTmpParam[loopi]); 			 
				memcpy(ppParam,TlvTmpParam[loopi],TmpTlvLen[loopi]);
				WPI_Free(TlvTmpParam[loopi]);
				ppParam += TmpTlvLen[loopi];			  
			}

			WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate TlvParam[%d] %s",Line,TlvParam[Line]);
			
			WPA_ReadTLV(TagNum,&TLV_ret);
			WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate charCount TagNum 0x%04x",TagNum);
			switch(TLV_ret.TLVType)
			{
				case TLV_STRING:
					{
						WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate trap tag: TLV_STRING\n");

						if(WPA_CheckTLV(TagNum,charCount,TlvParam[Line]))
						{	

							WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate TLV_STRING trap:tagId=%x,len=%d\n",TagNum,charCount);
							if((TagNum>=APPSERVER0IPPORT)&&(TagNum<=APPSERVER7IPPORT))
							{
								WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate TlvParam[%d]: 0x%x",Line,(*TlvParam[Line]));					  
								if(isdigit(*TlvParam[Line]))  
								{
									WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate the first value isdigit");
								}
								else
								{
									WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate the first value not isdigit");
									(*TlvParam[Line]) += 0x30;
								}
								result = ParserIPAddrPortStr(TlvParam[Line],charCount,TagNum-APPSERVER0IPPORT);
								if(!result)
								{
									*TLVnum = Line;
									WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate parserIPAddrPortStr RET false");
									TLVtag = TLV;		
									WPI_Free(TlvParam[Line]);	
									return false;
								}
								else
								{

								}
							}						
							else if(TagNum == SUPPORTEDDEV)
							{
								StatusContext_p->SupportedDev_Len = charCount;
								WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate TLV_STRING SUPPORTEDDEV SupportedDev_Len %d",charCount);
							}
							else		
							{

							}
							WPA_WriteTLV(TagNum,charCount,TlvParam[Line]);
						}
						else
						{
							*TLVnum = Line;
							WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate TLV_STRING RET false");
							TLVtag = TLV;		
							WPI_Free(TlvParam[Line]);	
							return false;
						}
					}
					break;

				case TLV_INT16:
					{
						u16 tmpVal =0;					  
						u16 datalen = charCount;
						char *tmpVal_p ;

						WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate charCount 16 datalen %d",datalen);
						tmpVal_p = (char *)(&tmpVal);
						memcpy(tmpVal_p+2-datalen,TlvParam[Line],datalen);

						WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate TLV_INT16 tmpVal is %02x",tmpVal);
						if(WPA_CheckTLV(TagNum,2,(char *)&tmpVal))
						{	
							WPA_WriteTLV(TagNum,2,(char *)&tmpVal);
							WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate TLV_INT16 trap:tagId=%x,len=2\n",TagNum);			
						}
						else
						{
							*TLVnum = Line;
							WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate  RET false");
							TLVtag = TLV;		
							WPI_Free(TlvParam[Line]);	
							return false;
						}
					}
					break;

				case TLV_INT32:
					{
						u32 tmpVal =0;
						//u16 datalen = (p-TlvDataHex)/2;
						u16 datalen = charCount;
						char *tmpVal_p;

						WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate charCount 32 datalen %d",datalen);
						tmpVal_p = (char *)(&tmpVal);
						memcpy(tmpVal_p+4-datalen,TlvParam[Line],datalen);

						//tmpVal = atoi(TlvData);					
						//tmpVal = htonl((u32)(*tlvdata_p); 				  

						WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate TLV_INT32 tmpVal is %4X",tmpVal);
						if(WPA_CheckTLV(TagNum,4,(char *)&tmpVal))
						{	
							WPA_WriteTLV(TagNum,4,(char *)&tmpVal);
							WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate TLV_INT32 trap:tagId=%x,len=4",TagNum); 
						}
						else
						{
							*TLVnum = Line;
							WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate TLV_INT32 RET false");
							TLVtag = TLV;		
							WPI_Free(TlvParam[Line]);	
							return false;
						}
					}
					break;

				case TLV_INT8:
					{
						if(WPA_CheckTLV(TagNum,1,TlvParam[Line]))
						{	
							WPA_WriteTLV(TagNum,1,TlvParam[Line]);
							WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate TLV_INT8 trap:tagId=%x,len=1\n",TagNum);
						}
						else
						{
							*TLVnum = Line;
							WMMP_DBG_TEMP("WPA_ParseTrapDataUpdate TLV_INT8 RET false");
							TLVtag = TLV;		
							WPI_Free(TlvParam[Line]);	
							return false;
						}
					}
					break;

				default:
					break;
			}

			WPI_Free(TlvParam[Line]);			

		}

		p++;
		Line++;
	}

	*TLVnum =Line;

	TLVtag = TLV;

	return true;

}
bool WPA_WcomParseCfgData(char *data, u16 length, u16 *TLVnum,u16 *TLVtag)
{
    WmmpContext_t  *mainContext_p = ptrToWmmpContext();
    WmmpTermStatusContext_t  *StatusContext_p = &(mainContext_p->StatusContext);
    WmmpCustomContext_t  *CustomContext_p = &(mainContext_p->CustomContext);
    u8 Line = 0;
    u16 TagNum;/*TagId*/
    u16 *TLV=PNULL;
    TLVRetVal_t  TLV_ret;
    char *p = NULL;  
    char *TlvTmpParam[MAX_PARAM_NUMBER+1] = {NULL};  /*without ,*/
    u8 TmpTlvLen[MAX_PARAM_NUMBER+1] = {0};            /*without ,*/
    char *TlvParam[MAX_PARAM_NUMBER+1] = {NULL};     /*without ,*/
    char *ppParam = NULL;                                /*without ,*/   
    u16 loopi = 0;  
    bool result = true;
    char *TlvDataStart[MAX_PARAM_NUMBER+1] = {NULL};
    char *TlvDataEnd[MAX_PARAM_NUMBER+1] = {NULL};
    u8 tlvParamCount = 0;      
    char IDParam[MAX_SERVER_ADDR] = "\0";    
    u32 i;

    p=data;
    WMMP_DBG_TEMP("WPA_WcomParseCfgData length %d",length);
    WMMP_DBG_TEMP("WPA_WcomParseCfgData sizeof(TmpTlvLen) %d",sizeof(TmpTlvLen));
    WMMP_DBG_TEMP("WPA_WcomParseCfgData sizeof(TlvTmpParam) %d",sizeof(TlvTmpParam));
    WMMP_DBG_TEMP("WPA_WcomParseCfgData sizeof(TlvParam) %d",sizeof(TlvParam));

    WMMP_DBG_TEMP("WPA_WcomParseCfgData simcom_m2m_at_len %d",simcom_m2m_at_len);   
    //TLV = WPI_Malloc(simcom_m2m_at_len*2);/*  这个数据被函数返回后才被caller 释放*/
    TLV = TLVtag;
    memset(TLV,0,simcom_m2m_at_len*2);  
    while(Line<simcom_m2m_at_len&&p-data<length)
    {   

        tlvParamCount = 0;
        memset(TlvTmpParam,0,20);
        memset(TmpTlvLen,0,sizeof(TmpTlvLen));
        memset(TlvParam,0,sizeof(TlvParam));
        memset(TlvDataStart,0,sizeof(TlvDataStart));
        memset(TlvDataEnd,0,sizeof(TlvDataEnd));

        WMMP_DBG_TEMP("WPA_WcomParseCfgData Line %d",Line);
        WMMP_DBG_TEMP("WPA_WcomParseCfgData 1 *p 0x%x",*p);
        WMMP_DBG_TEMP("WPA_WcomParseCfgData 1 p-data %d",p-data);
        result = Char2Bin16(p,&TagNum);//ex:tag "D001"->0xD001
        if(result)
        {
            p= p+4;
            TLV[Line] = TagNum;
        }
        else
        {
            *TLVnum = 0;
            WMMP_DBG_TEMP("WPA_WcomParseCfgData 3 TLVnum 0 ret");
            TLVtag = TLV;          
            return false;
        }               

        WMMP_DBG_TEMP("trap TagNum 0x%04x",TagNum);


        WMMP_DBG_TEMP("WPA_WcomParseCfgData TLV[%d]  0x%04x",Line,TLV[Line]);
        WMMP_DBG_TEMP("WPA_WcomParseCfgData 2 *p 0x%x",*p);
        WMMP_DBG_TEMP("WPA_WcomParseCfgData 2 p-data %d",p-data);

        if(*p!=0x2C)//,
        {
            *TLVnum = 0;
            WMMP_DBG_TEMP("WPA_WcomParseCfgData 4 TLVnum 0 ret");
            TLVtag = TLV;          
            return false;
        }           
        else
        {
            p++;
            WMMP_DBG_TEMP("WPA_WcomParseCfgData 3 *p 0x%x",*p);
            WMMP_DBG_TEMP("WPA_WcomParseCfgData 3 p-data %d",p-data);   

            tlvParamCount++;
            TlvDataStart[tlvParamCount] = p;
            WMMP_DBG_TEMP("WPA_WcomParseCfgData 1 *TlvDataStart[%d]  0x%02x",tlvParamCount,*TlvDataStart[tlvParamCount]);
            while(*p != 0x0a && p-data<length)
            {                    
                if(*p == 0x2C)//,
                {                                
                    TlvDataEnd[tlvParamCount] = p;
                    WMMP_DBG_TEMP("WPA_WcomParseCfgData w2 *TlvDataEnd[%d]  0x%02x",tlvParamCount,*TlvDataEnd[tlvParamCount]);
                    p++;
                    tlvParamCount++;
                    TlvDataStart[tlvParamCount] = p;
                    WMMP_DBG_TEMP("WPA_WcomParseCfgData w2 *TlvDataStart[%d]  0x%02x",tlvParamCount,*TlvDataStart[tlvParamCount]);
                }
                else
                {                    
                    p++;
                    WMMP_DBG_TEMP("WPA_WcomParseCfgData w1 *p 0x%x",*p);
                }
            }

            TlvDataEnd[tlvParamCount] = p;
            WMMP_DBG_TEMP("WPA_WcomParseCfgData 1 *TlvDataEnd[%d]  0x%02x",tlvParamCount,*TlvDataEnd[tlvParamCount]);

        }

        WMMP_DBG_TEMP("WPA_WcomParseCfgData 8 1  p-data %d",p-data);
        WMMP_DBG_TEMP("WPA_WcomParseCfgData 1 tlvParamCount %d",tlvParamCount);
        if((p-data<length)||(Line+1==simcom_m2m_at_len))
        {
            u32       charCount = 0;          /*without ,*/
            u32       tmpCount = 0;       /*without ,*/

            WMMP_DBG_TEMP("WPA_WcomParseCfgData TagNum 0x%04x tlvParamCount %d",TagNum,tlvParamCount);
            WPA_ReadTLV(TagNum,&TLV_ret);   
            {               

                for(loopi = 1;loopi<tlvParamCount+1;loopi++)
                {
                    WMMP_DBG_TEMP("WPA_WcomParseCfgData 2 *TlvDataStart[%d]  0x%02x",loopi,*TlvDataStart[loopi]);
                    if((*TlvDataStart[loopi]) == 0x22)   /*"*/
                    {
                        tmpCount = (TlvDataEnd[loopi] - TlvDataStart[loopi] - 2);
                        TmpTlvLen[loopi] = tmpCount;
                        WMMP_DBG_TEMP("WPA_WcomParseCfgData 2 without ,tmpCount %d",tmpCount);
                        TlvTmpParam[loopi] =(char*)WPI_Malloc((TlvDataEnd[loopi] - TlvDataStart[loopi] - 2)+1);
                        charCount += tmpCount;
                        WMMP_DBG_TEMP("WPA_WcomParseCfgData 2 without ,charCount %d",charCount);
                        memset(TlvTmpParam[loopi],0,(TlvDataEnd[loopi] - TlvDataStart[loopi] - 2)+1);
                        memcpy(TlvTmpParam[loopi],TlvDataStart[loopi]+1,(TlvDataEnd[loopi] - TlvDataStart[loopi] - 2));
                    }                
                    else
                    {                       
                        /*  3131  -s */
                        tmpCount = (TlvDataEnd[loopi] - TlvDataStart[loopi]);
                        TmpTlvLen[loopi] = tmpCount;
                        WMMP_DBG_TEMP("WPA_WcomParseCfgData 1 without ,tmpCount %d",tmpCount);
                        TlvTmpParam[loopi] =(char*)WPI_Malloc((TlvDataEnd[loopi] - TlvDataStart[loopi])+1);
                        charCount += tmpCount;
                        WMMP_DBG_TEMP("WPA_WcomParseCfgData 1 without ,charCount %d",charCount);
                        memset(TlvTmpParam[loopi],0,(TlvDataEnd[loopi] - TlvDataStart[loopi])+1);
                        memcpy(TlvTmpParam[loopi],TlvDataStart[loopi],(TlvDataEnd[loopi] - TlvDataStart[loopi]));
                        /*  3131   -e*/                     
                    }                 

                }               

                WMMP_DBG_TEMP("WPA_WcomParseCfgData end charCount %d ",charCount);              

                TlvParam[Line] =(char*)WPI_Malloc(charCount+1);
                memset(TlvParam[Line],0,charCount);
                ppParam = TlvParam[Line];

                for(loopi = 1;loopi < tlvParamCount+1;loopi++)
                {
                    WMMP_DBG_TEMP("wcomCfgData TlvTmpParam[%d]  %s",loopi,TlvTmpParam[loopi]);
                    memcpy(ppParam,TlvTmpParam[loopi],TmpTlvLen[loopi]);
                    WPI_Free(TlvTmpParam[loopi]);
                    ppParam += TmpTlvLen[loopi];                    
                }

                WMMP_DBG_TEMP("WPA_WcomParseCfgData TlvParam[%d] %s",Line,TlvParam[Line]);

            }           

            switch(TLV_ret.TLVType)
            {
                case TLV_STRING:
                    {
                        WMMP_DBG_TEMP("wcom trap tag: TLV_STRING\n");                   

                        WMMP_DBG_TEMP("wcom TLV_STRING trap:tagId=%x,len=%d\n",TagNum,charCount);
                        if((TagNum>=APPSERVER0IPPORT)&&(TagNum<=APPSERVER7IPPORT))
                        {

                            if(WPA_CheckTLV(TagNum,charCount,IDParam))
                            {                           
                                WMMP_DBG_TEMP("charCount APPSERVER0IPPORT TagNum 0x%04x",TagNum);                         
                                result = ParserIPAddrPortStr(TlvParam[Line],charCount,TagNum-APPSERVER0IPPORT);
                                if(!result)
                                {
                                    *TLVnum = Line;
                                    WMMP_DBG_TEMP("WPA_WcomParseCfgData parserIPAddrPortStr RET false");
                                    TLVtag = TLV;       
                                    WPI_Free(TlvParam[Line]);                               
                                    return false;
                                }
                                WPA_WriteTLV(TagNum,charCount,TlvParam[Line]);

                            }
                            else
                            {
                                *TLVnum = Line;
                                WMMP_DBG_TEMP("WPA_WcomParseCfgData TLV_STRING RET false");
                                TLVtag = TLV;       
                                WPI_Free(TlvParam[Line]);                           
                                return false;
                            }

                        }
                        else if(TagNum == M2MDEVSTATUS)  /*0x300f*/
                        {                       
                            if(WPA_CheckTLV(TagNum,charCount,IDParam))
                            {   
                                WPA_WriteTLV(TagNum,charCount,TlvParam[Line]);
                            }                       
                            else
                            {                               
                                *TLVnum = Line;
                                WMMP_DBG_TEMP("WPA_WcomParseCfgData M2MDEVSTATUS input RET false");
                                TLVtag = TLV;       
                                WPI_Free(TlvParam[Line]);                           
                                return false;
                            }                       

                        }
                        else
                        {
                            if(WPA_CheckTLV(TagNum,charCount,IDParam))
                            {   
                                WPA_WriteTLV(TagNum,charCount,TlvParam[Line]);
                            }                       
                            else
                            {                               
                                *TLVnum = Line;
                                WMMP_DBG_TEMP("WPA_WcomParseCfgData M2MDEVSTATUS input RET false");
                                TLVtag = TLV;       
                                WPI_Free(TlvParam[Line]);                           
                                return false;
                            }                       
                        }                   
                    }
                    break;

                case TLV_INT16:
                    {
                        u16 tmpVal =0;            
                        u16 datalen = charCount;
                        char *tmpVal_p = NULL;                      

                        WMMP_DBG_TEMP("WPA_WcomParseCfgData TLV_INT16 charCount datalen %d",datalen);               

                        if(WPA_CheckTLV(TagNum,2,IDParam))
                        {   

	/*bob modify 20110213*/
                            memset(IDParam,0,sizeof(IDParam));  
                            tmpVal = vgAsciiToInt16(TlvParam[Line]);
                            WMMP_DBG_TEMP("WPA_WcomParseCfgData  TLV_INT16  tmpVal %d",tmpVal);
                            Writeu16(IDParam,tmpVal);
                            WMMP_DBG_TEMP("WPA_WcomParseCfgData  TLV_INT16  IDParam %s",IDParam);
                            WMMP_DBG_TEMP("WPA_WcomParseCfgData  TLV_INT16  IDParam[0] 0x%02x IDParam[1] 0x%02x",IDParam[0],IDParam[1]);
                            WPA_WriteTLV(TagNum,2,IDParam);

                                    WMMP_DBG_TEMP("wcom TLV_INT16 trap:tagId=%x,len=2",TagNum);         
                                    }
                                    else
                                    {
                                    *TLVnum = Line;
                                    WMMP_DBG_TEMP("WPA_WcomParseCfgData  TLV_INT16 RET false");
                                    TLVtag = TLV;       
                                    WPI_Free(TlvParam[Line]);                       
                                    return false;
                                    }
                                    }
                                    break;

                case TLV_INT32:
                                    {
                                        u32 tmpVal =0;                    
                                        u16 datalen = charCount;
                                        char *tmpVal_p;

                                        WMMP_DBG_TEMP("wcomCfgData TLV_INT32 charCount datalen %d",datalen);

                                        if(WPA_CheckTLV(TagNum,4,IDParam))
                                        {   
                                            memset(IDParam,0,sizeof(IDParam));
                                            tmpVal = strtoul(TlvParam[Line],NULL,0);
                                            WMMP_DBG_TEMP("WPA_WcomParseCfgData  TLV_INT32  tmpVal %d",tmpVal);
                                            Writeu32(IDParam,tmpVal);
                                            WMMP_DBG_TEMP("WPA_WcomParseCfgData  TLV_INT32  IDParam %s",IDParam);
                                            WMMP_DBG_TEMP("WPA_WcomParseCfgData  TLV_INT32  IDParam[0] 0x%02x IDParam[1] 0x%02x IDParam[2] 0x%02x IDParam[3] 0x%02x",IDParam[0],IDParam[1],IDParam[2],IDParam[3]);
                                            WPA_WriteTLV(TagNum,4,TlvParam[Line]);
                                            WMMP_DBG_TEMP("WPA_WcomParseCfgData TLV_INT32 trap:tagId=%x,len=4",TagNum); 
                                        }
                                        else
                                        {
                                            *TLVnum = Line;
                                            WMMP_DBG_TEMP("WPA_WcomParseCfgData TLV_INT32 RET false");
                                            TLVtag = TLV;       
                                            WPI_Free(TlvParam[Line]);                       
                                            return false;
                                        }                       
                                    }
                                    break;

                case TLV_INT8:
                                    {
                                        u8    tmpVal = 0;

                                        WMMP_DBG_TEMP("wcomCfgData TLV_INT8 charCount %d",charCount);

                                        if(WPA_CheckTLV(TagNum,1,IDParam))
                                        {   
                                            memset(IDParam,0,sizeof(IDParam));
                                            tmpVal = atoi(TlvParam[Line]);
                                            WMMP_DBG_TEMP("WPA_WcomParseCfgData  TLV_INT8  tmpVal %d",tmpVal);
                                            IDParam[0] = tmpVal;    
                                            WPA_WriteTLV(TagNum,1,IDParam);
                                            WMMP_DBG_TEMP("WPA_WcomParseCfgData TLV_INT8 trap:tagId=%x,len=1,value 0x%02x",TagNum,IDParam[0]);                      
                                        }
                                        else
                                        {
                                            *TLVnum = Line;
                                            WMMP_DBG_TEMP("WPA_WcomParseCfgData TLV_INT8 RET false");
                                            TLVtag = TLV;       
                                            WPI_Free(TlvParam[Line]);                       
                                            return false;
                                        }
                                    }
                                    break;

                default:
                                    break;
            }

            WPI_Free(TlvParam[Line]);


        }
        p++;
        Line++;
    }

    *TLVnum =Line;

    TLVtag = TLV;

    return true;

}

//该函数是根据移动at 命令规范写的,但是其规范个人觉得有问题
//不符号本地修改参数的原意图,不建议使用此函数
bool WPA_LocalCfgReq(u16*TLVTags, u16 TLVNum)
{
	u16 i,j;
	u16 trapNum=0;
	u16* trapTLV;
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();
	CommandInfo_t CommandInfo; 
	WPIEventData_t WPIEventData;

	WMMP_LIB_DBG_TEMP("WPA_LocalCfgReq ");

	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	if((WPI_GetSimState()==WMMP_SECURITY_SIM_STATE_OK)&&(mainContext_p->Wmmp_status.protocol_status==LOGINOK))
	{
		//check the tlv is sync tlv , the sync tlv should be trap to server
		trapTLV = (u16*)WPI_Malloc( mainContext_p->CustomContext.TLVSyncTagNum*2);
		
		for(i=0;i<TLVNum;i++)
		{		
			for(j=0;j<mainContext_p->CustomContext.TLVSyncTagNum;j++)
			{
				if(TLVTags[i]==Readu16(&mainContext_p->ConfigContext.TLVSyncTagValue[j*2]))
				{
					WMMP_LIB_DBG_TEMP("WPA_LocalCfgReq:%d\n", TLVTags[i]);
					trapTLV[trapNum]=TLVTags[i];
					trapNum++;
				}
			}
		}


		if(trapNum==0)
		{
			WPI_Free(trapTLV);
			/*
			if(mainContext_p->Wmmp_status.protocol_status==PIDLE)
			{
				CommandInfo.CommandID = LOGIN_COMMAND;
				CommandInfo.SecuritySet = 0;
				CommandInfo.TransType = WMMP_TRANS_DEFAULT;					
				SaveToCommandBuffer(&CommandInfo);
			}*/
			
			WMMP_LIB_DBG_TEMP("WPA_LocalCfgReq:0x4011 trap\n");
			
			trapTLV = (u16*)WPI_Malloc( 2);
			trapTLV[0]=0x4011;
			CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
			CommandInfo.SecuritySet = 0;
			CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
			CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=trapTLV;
			CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=1;
			CommandInfo.TransType = WMMP_TRANS_DEFAULT;
			SaveToHighCommandBuffer(&CommandInfo);			
			
		}
		else
		{
			CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
			CommandInfo.SecuritySet = 0;
			CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
			CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=trapTLV;
			CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=trapNum;
			CommandInfo.TransType = WMMP_TRANS_DEFAULT;
			SaveToCommandBuffer(&CommandInfo);		
		}

	}
	else
	{
		return false;	
	}
	
	return true;
}

bool WPA_ReqReq(u16*TLVTags, u16 TLVNum)
{
	CommandInfo_t CommandInfo;  
	WPIEventData_t WPIEventData;
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();

	WMMP_LIB_DBG_TEMP("WPA_ReqReq ");

	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	/* simcom cbc@20091222 add for :deal with ternimal wmmp run status  begin*/
	if (mainContext_p->terminal_run_status != M2M_TERMINAL_RUN_STATUS_INIT)
	{
		WMMP_LIB_DBG_TEMP("login fail:terminal_run_status=0x%x\n",mainContext_p->terminal_run_status);
		return false;
	}
	else
	{
		WMMP_LIB_DBG_TEMP("WPA_ReqReq :terminal_run_status=0x%x\n",mainContext_p->terminal_run_status);
		
		mainContext_p->terminal_run_status = STATUS_CONFIG_REQ_COMMAND;		
	}
	/* simcom cbc@20091222 add for :deal with ternimal wmmp run status  end*/
	
	if((WPI_GetSimState()==WMMP_SECURITY_SIM_STATE_OK)&&(mainContext_p->Wmmp_status.protocol_status==LOGINOK))
	{	
		WMMP_LIB_DBG_TEMP("WPA_ReqReq  saveToCommand\n");
		
		//CUSTOMER_PRINT_DATA2(inSignal_p->TLVtags,16);
		CommandInfo.CommandID = CONFIG_REQ_COMMAND;
		CommandInfo.SecuritySet = 0;
		CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
		CommandInfo.CommandExInfo.ConfigReqInfo.TLVTags=TLVTags;
		CommandInfo.CommandExInfo.ConfigReqInfo.TagNum=TLVNum;
		CommandInfo.TransType = WMMP_TRANS_DEFAULT;	
		CommandInfo.PDUUsage = WMMP_PDU_ATCOMMAND;
		SaveToCommandBuffer(&CommandInfo);
	}
	else
	{
		WMMP_LIB_DBG_TEMP("WPA_ReqReq  err\n");
		mainContext_p->terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
		return false;

	}
	return true;
}

 bool WPA_DataReq(	char type,char	*senddata,u16	datalength, char *destination)
{
	WPIEventData_t WPIEventData;
	CommandInfo_t CommandInfo;  
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();	
    
	WMMP_LIB_DBG_TEMP("WPA_DataReq\n");
	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	if((WPI_GetSimState()==WMMP_SECURITY_SIM_STATE_OK)&&(mainContext_p->Wmmp_status.protocol_status==LOGINOK))
	{
		WMMP_LIB_DBG_TEMP("WPA_DataReq 1\n");
		CommandInfo.CommandID = TRANSPARENT_DATA_COMMAND;
		CommandInfo.TransType = WMMP_TRANS_DEFAULT;
		CommandInfo.PDUUsage = WMMP_PDU_ATCOMMAND;
		CommandInfo.SecuritySet = 0;
		CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
		CommandInfo.CommandExInfo.TransparentDataInfo.Type=type;
		CommandInfo.CommandExInfo.TransparentDataInfo.Data=senddata;
		CommandInfo.CommandExInfo.TransparentDataInfo.DateLen=datalength;	
		strcpy((char *)CommandInfo.CommandExInfo.TransparentDataInfo.destination,(char *)destination);	
		SaveToCommandBuffer(&CommandInfo);
	}
	else
	{
		WMMP_LIB_DBG_TEMP("WPA_DataReq ERROR \n");
		return false;		
	}
	return true;
}

bool WPA_FileReq(	char* TransID,u8 Status,u32 FileReadPoint,u16 FileBlockSize)
{
	CommandInfo_t CommandInfo;  
	WPIEventData_t WPIEventData;
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();	

	WMMP_LIB_DBG_TEMP("WPA_FileReq");

	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	/* simcom cbc@20091222 add for :deal with ternimal wmmp run status  begin*/
	if (mainContext_p->terminal_run_status != M2M_TERMINAL_RUN_STATUS_INIT)
	{
		WMMP_LIB_DBG_TEMP("login fail:terminal_run_status=0x%x\n",mainContext_p->terminal_run_status);
		return false;
	}
	else
	{
		WMMP_LIB_DBG_TEMP("login fail:terminal_run_status=0x%x\n",mainContext_p->terminal_run_status);
		mainContext_p->terminal_run_status = STATUS_FILE_REQ_COMMAND;		
	}
	/* simcom cbc@20091222 add for :deal with ternimal wmmp run status  end*/

	WMMP_LIB_DBG_TEMP("WPI_GetSimState()=%x\n",WPI_GetSimState());
	
	if(((WPI_GetSimState())==WMMP_SECURITY_SIM_STATE_OK)&&((mainContext_p->Wmmp_status.protocol_status)==LOGINOK))
	{	
		WMMP_LIB_DBG_TEMP("WPI_GetSimState()=%x\n",WPI_GetSimState());
		
		CommandInfo.CommandID = FILE_REQ_COMMAND;
		CommandInfo.PDUUsage = WMMP_PDU_ATCOMMAND;
		CommandInfo.SecuritySet = 0;
		CommandInfo.SecuritySet |= (1<<5); /*cbc@20100513: 安全摘要 */
		CommandInfo.TransType = WMMP_TRANS_DEFAULT;
		memcpy(CommandInfo.CommandExInfo.FileReqInfo.TransID,TransID,sizeof(CommandInfo.CommandExInfo.FileReqInfo.TransID));
		CommandInfo.CommandExInfo.FileReqInfo.Status=Status;
		CommandInfo.CommandExInfo.FileReqInfo.FileReadPoint=FileReadPoint;
		CommandInfo.CommandExInfo.FileReqInfo.FileBlockSize=FileBlockSize;

		SaveToCommandBuffer(&CommandInfo);
	}
	else  
	{
		WMMP_LIB_DBG_TEMP("WPI_GetSimState()=%x\n",WPI_GetSimState());
		mainContext_p->terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
		return false;	
	}
	return true;
}


bool WPA_adFileReq(	char* TransID,u8 Status,u32 FileReadPoint,u16 FileBlockSize)
{
	CommandInfo_t CommandInfo;  
	WPIEventData_t WPIEventData;
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();	

	WMMP_LIB_DBG_TEMP("WPA_adFileReq");

	memset(&CommandInfo,0,sizeof(CommandInfo_t));
	/* simcom cbc@20091222 add for :deal with ternimal wmmp run status  begin*/
	if (mainContext_p->terminal_run_status != M2M_TERMINAL_RUN_STATUS_INIT)
	{
		WMMP_LIB_DBG_TEMP("login fail:terminal_run_status=0x%x",mainContext_p->terminal_run_status);
		return false;
	}
	else
	{
		WMMP_LIB_DBG_TEMP("login ok:terminal_run_status=0x%x",mainContext_p->terminal_run_status);
		mainContext_p->terminal_run_status = STATUS_FILE_REQ_COMMAND;		
	}
	/* simcom cbc@20091222 add for :deal with ternimal wmmp run status  end*/

	WMMP_LIB_DBG_TEMP("WPA_adFileReq WPI_GetSimState()=%x",WPI_GetSimState());
	
	if(((WPI_GetSimState())==WMMP_SECURITY_SIM_STATE_OK)&&((mainContext_p->Wmmp_status.protocol_status)==LOGINOK))
	{	
		WMMP_LIB_DBG_TEMP("WPA_adFileReq WPI_GetSimState()=%x",WPI_GetSimState());
		
		CommandInfo.CommandID = FILE_REQ_COMMAND;
		CommandInfo.PDUUsage = WMMP_PDU_ATCOMMAND;
		CommandInfo.SecuritySet = 0;
		CommandInfo.SecuritySet |= (1<<5); /*cbc@20100513: 安全摘要 */
		CommandInfo.TransType = WMMP_TRANS_DEFAULT;
		memcpy(CommandInfo.CommandExInfo.FileReqInfo.TransID,TransID,sizeof(CommandInfo.CommandExInfo.FileReqInfo.TransID));
		CommandInfo.CommandExInfo.FileReqInfo.Status=Status;
		CommandInfo.CommandExInfo.FileReqInfo.FileReadPoint=FileReadPoint;
		CommandInfo.CommandExInfo.FileReqInfo.FileBlockSize=FileBlockSize;
		WMMP_LIB_DBG_TEMP("WPA_adFileReq Status %d FileReadPoint %d FileBlockSize %d",Status,FileReadPoint,FileBlockSize);
		SaveToCommandBuffer(&CommandInfo);
	}
	else  
	{
		WMMP_LIB_DBG_TEMP("WPA_adFileReq return false WPI_GetSimState()=%x",WPI_GetSimState());
		mainContext_p->terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
		return false;	
	}
	return true;
}

bool wmmpCheckHasAppData(u8 uAppCount)
{
	u16	loopi;
	bool  rett = false;

	WMMP_LIB_DBG_TEMP("wmmpCheckHasAppData uAppCount %d",uAppCount);
	if(uAppCount >= MAX_WMMP_APP_NUM)
	{
		for(loopi = 0;loopi < MAX_WMMP_APP_NUM;loopi++)
		{
			WMMP_LIB_DBG_TEMP("wmmpCheckHasAppData LOOP m2mHasAppDataFlag[%d] %d",loopi,m2mHasAppDataFlag[loopi]);
			if(m2mHasAppDataFlag[loopi])
			{
				return true;
			}
		}

	}
	else
	{
		WMMP_LIB_DBG_TEMP("wmmpCheckHasAppData  m2mHasAppDataFlag[%d] %d",uAppCount,m2mHasAppDataFlag[uAppCount]);	
		if(m2mHasAppDataFlag[uAppCount])
		{
				return true;
		}
	}
	
	WMMP_LIB_DBG_TEMP("wmmpCheckHasAppData rett %d",rett);
	return rett;	   
}
/* 目前暂没有升级程序功能*/
void WPA_update_app(u8  outreason)
{
    WmmpContext_t    *mainContext_p = ptrToWmmpContext();
    CommandInfo_t CommandInfo;
    bool bHasAppData = true;

    memset(&CommandInfo,0,sizeof(CommandInfo_t));
    WMMP_LIB_DBG_TEMP(" WPA_update_app outreason %d",outreason);      

    bHasAppData=  wmmpCheckHasAppData(MAX_WMMP_APP_NUM);
    if(!bHasAppData)
    {
        WMMP_LIB_DBG_TEMP((" WPA_update_app Reqreboot:LOGOUT_COMMAND"));	
        if(strcmp(mainContext_p->StatusContext.TerminalSeriesNum,(char*)UNREGISTER_TERMINAL_NUM) == 0)
        {
            WPI_sleep(800000);
            WPI_SystemReboot();	
        }
        else
        {
            WMMP_LIB_DBG_TEMP((" WPA_update_app Reqreboot:LOGOUT_COMMAND"));
            CommandInfo.CommandID = LOGOUT_COMMAND;
            CommandInfo.SecuritySet = 0;
            CommandInfo.TransType = WMMP_TRANS_DEFAULT;
            CommandInfo.PDUUsage = WMMP_REMOTE_CONTROL_REBOOT;
            CommandInfo.CommandExInfo.LoginOutInfo.reason=outreason;

            SaveToCommandBuffer(&CommandInfo);					
        }

    }
    return;
}

u16 vgAsciiToDecu16(const u8 *hexString_p)
{
  u16 result = 0, pos = 0;
  u8 value;

  WMMP_LIB_DBG_TEMP("vgAsciiToDecu16 hexString_p[%d] 0x%02x",pos,hexString_p[pos]);
  while ((hexString_p[pos] != '\0') && (pos < 4))
  {

    WMMP_LIB_DBG_TEMP("vgAsciiToDecu16 1 hexString_p[%d] 0x%02x",pos,hexString_p[pos]);
    /* Check whatever's entered is valid (i.e. hex).... */
    if (isxdigit(hexString_p[pos]))
    {
      /* Numerical (0-9).... */
      if (isdigit(hexString_p[pos]))
      {
      	 WMMP_LIB_DBG_TEMP("vgAsciiToDecu16 2 hexString_p[%d] 0x%02x",pos,hexString_p[pos]);
        value = hexString_p[pos] - '0';
	  WMMP_LIB_DBG_TEMP("vgAsciiToDecu16 2 result %d",result);
      }

      /* Or a-f.... */
      else
      {
      	  WMMP_LIB_DBG_TEMP("vgAsciiToDecu16 3 hexString_p[%d] 0x%02x",pos,hexString_p[pos]);
        value = tolower(hexString_p[pos]);
        value -= 'a';
        value += 0x0a;
	 WMMP_LIB_DBG_TEMP("vgAsciiToDecu16 3 result %d",result);
      }
    }
    else
    {
      value = 0;
      WMMP_LIB_DBG_TEMP("vgAsciiToDecu16 4 result %d",result);
    }

    /* Convert to 16-bit int.... */
    result = (result * 10) + value;
    WMMP_LIB_DBG_TEMP("vgAsciiToDecu16 result[%d] 0x%02x",pos,result);
    pos++;
  }

   WMMP_LIB_DBG_TEMP("vgAsciiToDecu16 result %d",result);
  return result;
}


u16 vgAsciiToDec(const u8 *hexString_p,u16 vgstrLen)
{
  u16 result = 0;
  u8 value;
  u16	pos = 0;

  WMMP_LIB_DBG_TEMP("vgAsciiToDec hexString_p[%d] 0x%02x vgstrLen %d",pos,hexString_p[pos],vgstrLen);
  while ((hexString_p[pos] != '\0') && (pos < vgstrLen))
  {

    WMMP_LIB_DBG_TEMP("vgAsciiToDec 1 hexString_p[%d] 0x%02x",pos,hexString_p[pos]);
    /* Check whatever's entered is valid (i.e. hex).... */
    if (isxdigit(hexString_p[pos]))
    {
      /* Numerical (0-9).... */
      if (isdigit(hexString_p[pos]))
      {
      	 WMMP_LIB_DBG_TEMP("vgAsciiToDec 2 hexString_p[%d] 0x%02x",pos,hexString_p[pos]);
        value = hexString_p[pos] - '0';
	  WMMP_LIB_DBG_TEMP("vgAsciiToDec 2 result %d",result);
      }

      /* Or a-f.... */
      else
      {
      	  WMMP_LIB_DBG_TEMP("vgAsciiToDec 3 hexString_p[%d] 0x%02x",pos,hexString_p[pos]);
        value = tolower(hexString_p[pos]);
        value -= 'a';
        value += 0x0a;
	 WMMP_LIB_DBG_TEMP("vgAsciiToDec 3 result %d",result);
      }
    }
    else
    {
      value = 0;
      WMMP_LIB_DBG_TEMP("vgAsciiToDec 4 result %d",result);
    }

    /* Convert to 16-bit int.... */
    result = (result * 10) + value;
    WMMP_LIB_DBG_TEMP("vgAsciiToDec result[%d] 0x%02x",pos,result);
    pos++;
  }

   WMMP_LIB_DBG_TEMP("vgAsciiToDec result %d",result);
  return result;
}



bool ParserIPAddrPortStr(char *srcString ,u16 strLength,u8 serverNum)
{

  WmmpContext_t  *mainContext_p = ptrToWmmpContext();
  WmmpCustomContext_t  *CustomContext_p = &(mainContext_p->CustomContext);
  WmmpCustom2Context_t  *Custom2Context_p = &(mainContext_p->Custom2Context);
  WmmpSocketType_e soctype;
  u8                Transtype = 0;
  u16               sockPort;
  
   char *p = NULL;
   char AppSerString[MAX_SERVER_ADDR] = "\0";
   u16 loopi = 0;     
   char *pStartAddr1=NULL;
   char *pStartAddr2=NULL;
   char *pStartAddr3=NULL;
   char *pStartAddr4=NULL;
   char *pStartPort1=NULL;
   char *pStartPort2=NULL;
   
   u32  IPAddrtmpVal = 0;
   u32  IPAddrVal = 0;
   u16  PorttmpVal = 0;
   u16  PortVal = 0;
   //char   tmpSearchStr = "\0";
   u8    ipAddr1Len = 0;
   u8    ipAddr2Len = 0;
   u8    ipAddr3Len = 0;
   u8    ipAddr4Len = 0;
   u8    PortLen = 0;
   u8    AppServerType = 0;   /*4---GPRS*/
   char    tmpStr[33] = "\0";
   

    WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr len %d",strLength);  

    memset(AppSerString,0,sizeof(AppSerString));
    memset(Custom2Context_p->AppServerString[serverNum],0x00,MAX_SERVER_ADDR);
    WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr 1 AppServerString[%d] %s",serverNum,Custom2Context_p->AppServerString[serverNum]); 
    memcpy(Custom2Context_p->AppServerString[serverNum],srcString,strLength);
    WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr 2 AppServerString[%d] %s",serverNum,Custom2Context_p->AppServerString[serverNum]); 
    memcpy(AppSerString,srcString,min(sizeof(AppSerString),strLength));    
    for(loopi=0;loopi<strLength;loopi++)
    {
        AppSerString[loopi] = toupper(AppSerString[loopi]);
    }
   
    WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr AppSerString %s",AppSerString);    

    if(strstr(AppSerString,"TCP://"))
    {        
        soctype = WMMP_TCP_CONNECT;        
        p = strstr(AppSerString,"TCP://");
    }
    else if(strstr(AppSerString,"UDP://"))
    {
        soctype = WMMP_UDP_CONNECT;        
        p = strstr(AppSerString,"UDP://");
    }
    else
    {
          p = NULL;
          WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr connectModeString NOT SUPPORT");
          return false;
    }
    WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr p %s",p);
    p += 6;
    pStartAddr1 = p;
    
    if(pStartAddr1 == NULL)
    {
        WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr 1 NOT FOUND IP");
        return false;
    }
    WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr 2e 1 *P 0x%02x",*p);
    
    for(loopi = 0;loopi < strLength-6-1;loopi++)
    {
          WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr loopi 1 *P 0x%02x",*p);
          if(*p == 0x2e)
          {   
              ipAddr1Len = p-pStartAddr1;
              WMMP_LIB_DBG_TEMP("ParserIPAddrPort ipAddr1Len %d sizeof(tmpStr) %d",ipAddr1Len,sizeof(tmpStr));
              memset(tmpStr,0,sizeof(tmpStr));
              memcpy(tmpStr,pStartAddr1,min(ipAddr1Len,sizeof(tmpStr)));
              WMMP_LIB_DBG_TEMP("ParserIPAddrPort pStartAddr1 %s",tmpStr);
              IPAddrtmpVal = vgAsciiToDecu16(tmpStr);
              IPAddrVal = IPAddrVal | IPAddrtmpVal << 24;
              p ++;             
              pStartAddr2 = p;
              WMMP_LIB_DBG_TEMP(" 1IPAddrtmpVal 0x%02x IPAddrVal 0x%08x",IPAddrtmpVal,IPAddrVal);
              break;
          }
          else
          {
                p ++;
          }
    }
    if(!pStartAddr2)
    {
         WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr 2 NOT FOUND IP");
        return false;
    }
     for(loopi = 0;loopi < strLength-8-ipAddr1Len;loopi++)
    {
          if(*(p) == 0x2e)
          {   
              ipAddr2Len = p-pStartAddr2;
              WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr ipAddr2Len %d",ipAddr2Len);
              memset(tmpStr,0,sizeof(tmpStr));
              memcpy(tmpStr,pStartAddr2,min(ipAddr2Len,sizeof(tmpStr)));
              IPAddrtmpVal = vgAsciiToDecu16(tmpStr);  
              p ++;                          
              IPAddrVal = IPAddrVal | IPAddrtmpVal << 16;
              pStartAddr3 = p;
              WMMP_LIB_DBG_TEMP(" 2IPAddrtmpVal 0x%02x IPAddrVal 0x%08x",IPAddrtmpVal,IPAddrVal);
              break;
          }
          else
          {
                p ++;
          }
    }
    if(!pStartAddr3)
    {
         WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr 3 NOT FOUND IP");
        return false;
    }
    WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr 2e 3 *P 0x%02x",*p);
    for(loopi = 0;loopi <strLength-9-ipAddr1Len-ipAddr2Len;loopi++)
    {
          if(*(p) == 0x2e)
          {   
              ipAddr3Len = p-pStartAddr3;
              WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr ipAddr3Len %d",ipAddr3Len);
              memset(tmpStr,0,sizeof(tmpStr));
              memcpy(tmpStr,pStartAddr3,min(ipAddr3Len,sizeof(tmpStr)));
              IPAddrtmpVal = vgAsciiToDecu16(tmpStr);    
              p ++;                          
              IPAddrVal = IPAddrVal | IPAddrtmpVal << 8;
              pStartAddr4 = p;
              WMMP_LIB_DBG_TEMP("3IPAddrtmpVal 0x%02x IPAddrVal 0x%08x",IPAddrtmpVal,IPAddrVal);
              break;
          }
          else
          {
                p ++;
          }
    }
     if(!pStartAddr4)
    {
         WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr 4 NOT FOUND IP");
        return false;
    }
   WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr 2e 4 *P 0x%02x",*p);
   for(loopi = 0;loopi < strLength-10-ipAddr1Len-ipAddr2Len-ipAddr3Len;loopi++)
    {
          WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr loopi 4 *P 0x%02x",*p);
          if(*(p) == 0x3a)
          {   
              ipAddr4Len = p-pStartAddr4;
              WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr ipAddr4Len %d",ipAddr4Len);
              memset(tmpStr,0,sizeof(tmpStr));
              memcpy(tmpStr,pStartAddr4,min(ipAddr4Len,sizeof(tmpStr)));
		 WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr ipAddr4Len tmpStr %s",tmpStr);
              IPAddrtmpVal = vgAsciiToDecu16(tmpStr);    
              p ++;                                        
              IPAddrVal = IPAddrVal | IPAddrtmpVal;
              pStartPort1 = p;
              WMMP_LIB_DBG_TEMP("4IPAddrtmpVal 0x%02x IPAddrVal 0x%08x",IPAddrtmpVal,IPAddrVal);
              break;
          }
          else
          {
                p ++;
          }
    }
    if(!pStartPort1)
    {
         WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr 5 NOT FOUND port");
        return false;
    }
    WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr 3a 1 *P 0x%02x",*p);
     for(loopi = 0;loopi < strLength-11-ipAddr1Len-ipAddr2Len-ipAddr3Len-ipAddr4Len;loopi++)
    {
          
          WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr port[%d] %d",loopi,(*p));
          if(isdigit(*p))
          {
              p++;
          }
          else
          {
              WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr NOT FOUND port");
              return false;
          }         
    }
  WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr 3a 2 *P 0x%02x",*p);

   PortLen = p - pStartPort1;
   WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr PortLen %d",PortLen);   
   memset(tmpStr,0,sizeof(tmpStr));
   memcpy(tmpStr,pStartPort1,min(PortLen,sizeof(tmpStr)));
   WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr Port tmpStr %s",tmpStr);
   PorttmpVal = vgAsciiToDec(tmpStr,PortLen);    
   WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr PorttmpVal %d",PorttmpVal);
   PortVal = PorttmpVal;
   sockPort = PortVal;

  Custom2Context_p->AppSerConnMode[serverNum] = soctype;
  WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr AppSerConnMode[%d] %d",serverNum,soctype);
  WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr IPAddrVal 0x%04x",IPAddrVal);
  Custom2Context_p->AppSerAddr[serverNum] = IPAddrVal;
  Custom2Context_p->AppSerPort[serverNum] = sockPort;
  WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr PortVal %d",PortVal);
   if(isdigit(srcString[0]))
   {
        AppServerType = srcString[0] - '0';
    }
   else
    {
        AppServerType = 4;
    }
  Custom2Context_p->AppSerType[serverNum] = AppServerType;
  WMMP_LIB_DBG_TEMP("ParserIPAddrPortStr AppServerType %02d",AppServerType);
  return true;
}

bool ParserCustGenParamStr(char *srcString ,u16 strLength,u16 TlvId)
{

  WmmpContext_t  *mainContext_p = ptrToWmmpContext();  
  WmmpCustomContext_t  *CustomContext_p = &(mainContext_p->CustomContext);
   
    WMMP_LIB_DBG_TEMP("ParserCustGenParamStr");
   

  return true;
}


void WPA_AppOpenReq(u8 appNum,wmmpCreateConnectCallback callback)
{
    WmmpContext_t  *mainContext_p = ptrToWmmpContext();
    WmmpCustomContext_t  *CustomContext_p = &(mainContext_p->CustomContext);
    WmmpCustom2Context_t  *Custom2Context_p = &(mainContext_p->Custom2Context);
    WmmpSocketType_e soctype;

    WMMP_LIB_DBG_TEMP("WPA_AppOpenReq net_status %d",mainContext_p->Wmmp_status.net_status);

    if(mainContext_p->Wmmp_status.net_status == LINK_FAILED)
    {
        wmmpLibActiveGprs();
    }

    mainContext_p->createConnectCallBack=callback;
	
    WMMP_LIB_DBG_TEMP("AppOpenReq soctyp %d serAddr %04x port %d",Custom2Context_p->AppSerConnMode[appNum],
    Custom2Context_p->AppSerAddr[appNum],
    Custom2Context_p->AppSerPort[appNum]);

    soctype = Custom2Context_p->AppSerConnMode[appNum];
    WMMP_LIB_DBG_TEMP("AppOpenReq old wmmpOpenSkt[%d] %x",appNum,mainContext_p->wmmpOpenSkt[appNum]);
    if(mainContext_p->wmmpOpenSkt[appNum] == CONNREF_INVALID)
    {	
        //TODO:modigy for apn_type temp 130314
         WPI_CreateConnect(1,soctype, appNum+1,Custom2Context_p->AppSerAddr[appNum], Custom2Context_p->AppSerPort[appNum]);
    }else
    {
        WPI_send_urc("\r\n$M2MAPPOPEN: %d is Opening",appNum);
    }
    WMMP_LIB_DBG_TEMP("AppOpenReq ok wmmpOpenSkt[%d] %x",appNum,mainContext_p->wmmpOpenSkt[appNum]);

}




bool WPA_AppBrOpenReq(void)
{
	WmmpContext_t  *mainContext_p = ptrToWmmpContext();	
	WmmpCustomContext_t  *CustomContext_p = &(mainContext_p->CustomContext);	
	
	char *p = NULL;  
	u16 loopi = 0;
	u32            socketId = 0;

	WMMP_LIB_DBG_TEMP("WPA_AppBrOpenReq net_status %d",mainContext_p->Wmmp_status.net_status);

	if(mainContext_p->Wmmp_status.net_status == LINK_FAILED)
	{
		mainContext_p->maxOpenGPRSCount = 0;
		wmmpLibActiveGprs();
		return true;
	}
	else
	{
		return true;
	}

}

/*****************************************************************************
*	函数名	      : wmmpSendbyGPRS
* 作  者        : 
*	功能描述      : 通过GPRS发送应用数据
*	输入参数说明  : 无
* 输出参数说明  : 无
*	返回值说明    : 无
*****************************************************************************/
bool WPA_AppSendReq(u8 appNum, char	*senddata,u16 datalength,wmmpSendCallback callBack)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	u32 sktId=0;
	u16 bytesSent=0;
	bool   ret=false;
	WmmpContext_t  *mainContext_p = ptrToWmmpContext();
        WMMPAPNType apn_type = wmmp_get_apn_type();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
	WMMP_LIB_DBG_TEMP("WPA_AppSendReq datalength %d, appNum %d",datalength, appNum);

	sktId=appNum+1;
	
	WMMP_LIB_DBG_TEMP("WPA_AppSendReq sktId %d",sktId);
	if(sktId!=CONNREF_INVALID)
	{
		if(datalength <= MAX_APPSEND_LEN)
		{
			bytesSent=WPI_Send(apn_type,sktId,senddata,datalength);
			mainContext_p->gprsSendCallBack=callBack;
		}
		WMMP_LIB_DBG_TEMP("WPA_AppSendReq bytesSent %d",bytesSent);
		
		if(bytesSent>0)
		{
			ret=true;
		}
	}

	//WPI_Free(senddata);

	return ret;

}
bool WPA_AppCloseReq(u8 appNum)
{
	u32               sktId;
	WmmpContext_t  *mainContext_p = ptrToWmmpContext();
	WMMP_LIB_DBG_TEMP("WPA_AppCloseReq wmmpOpenSkt[%d]",appNum,mainContext_p->wmmpOpenSkt[appNum]);

	sktId=mainContext_p->wmmpOpenSkt[appNum];
	if(sktId != CONNREF_INVALID)
	{
	wmmpLibCloseConnect(sktId);
	}
	return true;
}


bool WPA_AppBrCloseReq(void)
{
	u32               sktId;
	WmmpContext_t  *mainContext_p = ptrToWmmpContext();

	WMMP_LIB_DBG_TEMP("WPA_AppBrCloseReq wmmpOpenSkt[%d]",currAppNum,mainContext_p->wmmpOpenSkt[currAppNum]);
	sktId=mainContext_p->wmmpOpenSkt[currAppNum];
	if(sktId != CONNREF_INVALID)
	{
		wmmpLibCloseConnect(sktId);
	}
	return true;
}

bool WPA_ClearSecurity(u8 SecurityParam)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext(); 
	WPAEventData_t WPAEvent;

	WMMP_LIB_DBG_TEMP("WPA_ClearSecurity SecurityParam=%d",SecurityParam);
				
	mainContext_p->SecurityContext.M2MPassClearPara= SecurityParam;
	WPI_WriteNvram(NVRAM_EF_WMMP_TLV_SECURITY_LID,(WmmpSecurityContext_t*)WPA_ptrToSecurityTLV(), sizeof(WmmpSecurityContext_t));
	if(SecurityParam==128)//0x80
	{
		//clear cpin
		if(mainContext_p->SecurityContext.SIMPin1Active == 1)
		{
			if (WPI_SecurityDisableCPIN(mainContext_p->SecurityContext.SIMPin1) == true)
			{
				WPAEvent.WPACPinEvent.error = WMMP_SECURITY_OPER_ERROR_OK;
			}
			else
			{
				WPAEvent.WPACPinEvent.error = WMMP_SECURITY_OPER_ERROR_DISABLE_FAIL;
			}
			 
			WPAEvent.WPACPinEvent.operation = WMMP_SECURITY_OPER_DISABLE;
			WPA_ProcessEvent(WPA_CPIN_EVENT, &WPAEvent);
		}
			
		//	memset(mainContext_p->SecurityContext.SIMPin1,0,sizeof(mainContext_p->SecurityContext.SIMPin1));				
		return true;
	}
	else if(SecurityParam==14)//0x0e
	{
		//clear password
		memset(mainContext_p->SecurityContext.uplinkPassword,0,sizeof(mainContext_p->SecurityContext.uplinkPassword));
		memset(mainContext_p->SecurityContext.downlinkPassword,0,sizeof(mainContext_p->SecurityContext.downlinkPassword));
		memset(mainContext_p->SecurityContext.publickey,0,sizeof(mainContext_p->SecurityContext.publickey));					
		return true;
	}

	return false;
}

bool WPA_AppInit(void)
{
	u16  loopi=0;
	WmmpContext_t *mainContext_p = ptrToWmmpContext();
	WMMP_LIB_DBG_TEMP("WPA_AppInit %d\n",WPI_GetSimState());

	Transaction_Id = 0;
	bhvMode = 0;
	memset(bhvValue,0,MAX_BHV_MODE);
	bhvValue[3]=1;
	currAppNum = 0;
	m2mStatusParam = 0;
	m2mAppStatusParam = 0;
	m2mAppBrStatusParam = 0;

	for(loopi = 0;loopi < MAX_WMMP_APP_NUM;loopi++)
	{
		mainContext_p->wmmpOpenSkt[loopi]=CONNREF_INVALID;
		mainContext_p->Wmmp_status.app_status[loopi] = APPLINK_FAILED;
		m2mHasAppDataFlag[loopi] = 0;
	}
	for(loopi = 0;loopi < WMMP_HAS_APP_DATA_TYPE_END;loopi++)
	{		
		m2mHasAppDataType[loopi] = 0;
	}
	
	return true;
	
}

void wmmpStartInterTimer(WmmpTimeUserID_e UserValue, u32 timeoutPeriod)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext(); 
	mainContext_p->TimerRun[(u8)UserValue] = true;
	mainContext_p->TimerStopTick[(u8)UserValue]=mainContext_p->TimerTicks+timeoutPeriod;

	WMMP_LIB_DBG_TEMP("wmmpStartInterTimer,timerid=%d,timeoutPeriod=%d\n",UserValue,timeoutPeriod);
	WMMP_LIB_DBG_TEMP("TimerTicks %d\n",mainContext_p->TimerTicks);	
}

void wmmpStopInterTimer(WmmpTimeUserID_e UserValue)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext(); 
	mainContext_p->TimerRun[(u8)UserValue] = false;

	WMMP_LIB_DBG_TEMP("wmmpStopInterTimer,timerid=%d\n",UserValue);
	
}

bool wmmpisInterTimerRun(WmmpTimeUserID_e UserValue)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext(); 
	return mainContext_p->TimerRun[(u8)UserValue];
}

void wmmpProcessTimerTick(WmmpTimeUserID_e TimeUserID)
{
	WMMP_LIB_DBG_TEMP("wmmpProcessTimerTick,TimeUserID=%d\n",TimeUserID);

	switch(TimeUserID)
	{
		case   NETWORK_CONNECT_TIMEOUT_TIMER :  //网络连接超时定时器
		{
			WMMP_LIB_DBG_TEMP("wmmpProcessTimerTick()-NETWORK_CONNECT_TIMEOUT_TIMER");
		}
		break;
		
 	 	case 	SEND_PDU_TIMEOUT_TIMER:               /*  报文发送超时定时器*/
 	 	{
			//WMMP_LIB_DBG_TEMP("wmmpProcessTimerTick()-SEND_PDU_TIMEOUT_TIMER\n");
			wmmpProcessPDUTimer();
 	 	}
		break;
		
 	 	case 	HEART_BEAT_TIMER:                            //心跳定时器
 	 	{
			WMMP_LIB_DBG_TEMP("wmmpProcessTimerTick()-HEART_BEAT_TIMER");
			
			#if 1  /* 20100416@cbc  :测试时先屏蔽掉心跳包,实际应用时应该打开 */
			wmmpProcessHeartTimer();
			#endif
 	 	}
		break;
		
 	 	case 	SHORT_CONNECT_MODE_TIMER:         //短连接超时定时器
 	 	{
			WMMP_LIB_DBG_TEMP("wmmpProcessTimerTick()-SHORT_CONNECT_MODE_TIMER");
			wmmpProcessShortConnectTimer();
 	 	}
		break;
		
 	 	case 	SIGMA_STAT_TIMER:                         //汇总统计定时器
 	 	{
			WMMP_LIB_DBG_TEMP("wmmpProcessTimerTick()-SIGMA_STAT_TIMER");
			wmmpProcessSigmaStatTimer();
 	 	}
		break;
		
 	 	case 	STAT_TIMER:                                      //详细统计定时器
 	 	{
			WMMP_LIB_DBG_TEMP("wmmpProcessTimerTick()-STAT_TIMER");
			wmmpProcessStatTimer();
 	 	}
		break;
		
 	 	case 	PDU_INTERVAL_TIMER:
		{
			WMMP_LIB_DBG_TEMP("wmmpProcessTimerTick()-PDU_INTERVAL_TIMER");
			wmmpProcessPDUIntervalTimer();
 	 	}
		break;
		
		case SOCKET_CLOSE_TIMEOUT_TIMER:
		{
			WMMP_LIB_DBG_TEMP("wmmpProcessTimerTick()-SOCKET_CLOSE_TIMEOUT_TIMER");
			wmmpProcessSocketCloseTimer();
		}
		break;
		
		case RESIGTER_KEY_DOWN_TIMEOUT_TIMER:
		{		//* add by cbc@20100522 for 注册等待平台下发密码超时 */	
			wmmpProcessRegisterTimer();
			WMMP_LIB_DBG_TEMP("wmmpProcessTimerTick()-RESIGTER_KEY_DOWN_TIMEOUT_TIMER");
		}
		break;
		case TIMING_REGISTER_TIMER:
		{		
			wmmpProcessTimingRegisterTimer();
			WMMP_LIB_DBG_TEMP("wmmpProcessTimerTick()-TIMING_REGISTER_TIMER");
		}
		break;
		case TIMING_LOGIN_TIMER:
		{		
			wmmpProcessTimingLoginTimer();
			WMMP_LIB_DBG_TEMP("wmmpProcessTimerTick()-TIMING_LOGIN_TIMER");
		}
		break;		
		case TRAPABORTWARN_TIIMER:
		{		
			wmmpProcessTrapAbortWarnTimer();
			WMMP_LIB_DBG_TEMP("wmmpProcessTimerTick()-TRAPABORTWARN_TIIMER");
		}
		default:
			WMMP_LIB_DBG_TEMP("wmmpProcessTimerTick()-default\n");
			break;
		
	}
}

void WPA_ProcessTimerEvent(void)
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext(); 
	u8 i;
	//WMMP_LIB_DBG_TEMP("WPA_ProcessTimerEvent");
    
	WPI_StartTimer(1000);
	//WMMP_LIB_DBG_TEMP("ProcessTimerEvent TimerTicks %d",mainContext_p->TimerTicks);
	for(i=0;i<NUM_OF_WMMP_TIMER;i++)
	{
		//WMMP_LIB_DBG_TEMP("ProcessTimerEvent TimerRun[%d] %d",i,mainContext_p->TimerRun[i]);
		//WMMP_LIB_DBG_TEMP("ProcessTimerEvent TimerStopTick[%d] %d",i,mainContext_p->TimerStopTick[i]);
		if(mainContext_p->TimerRun[i]==true)
		{
			//if(mainContext_p->TimerStopTick[i]>=mainContext_p->TimerTicks)
			if(mainContext_p->TimerStopTick[i] <= mainContext_p->TimerTicks)/* cbc@20100525:改为大于等于更好 */
			{
				WMMP_LIB_DBG_TEMP("WPA_ProcessTimerEvent wmmpProcessTimerTick id=%d",i);
				mainContext_p->TimerRun[i]=false;
				wmmpProcessTimerTick((WmmpTimeUserID_e)(i));
			}
		}
	}
	mainContext_p->TimerTicks++;
}

void WPA_ProcessCpinEvent(	WmmSecurityOperState_e  operation,WmmSecurityOperError_e   error)
{
   WmmpContext_t    *mainContext_p = ptrToWmmpContext();
   
	WMMP_LIB_DBG_TEMP("WPA_ProcessCpinEvent operation =%d, error=%d", operation, error);
   
	//output
	switch(operation)
	{
		case WMMP_SECURITY_OPER_AUTO:
		{
			switch(error)
			{
			//cpin error
				case WMMP_SECURITY_OPER_ERROR_CPIN_FAIL:
				{
					/*
					WPI_PutNewLine();
					WPI_Printf((const char*)"CPIN ERROR");
					WPI_PutNewLine();*/
		
				}
				break;
				
				//cpin ok
				case WMMP_SECURITY_OPER_ERROR_OK:
				{
					mainContext_p->m2mswitch = true;
					WPI_StartTimer(1000);
				}
				break;
				
				//cpin retry time max
				case WMMP_SECURITY_OPER_ERROR_CPIN_LIMIT:
				{
					/*
					WPI_PutNewLine();
					WPI_Printf((const char*)"CPIN LIMIT");
					WPI_PutNewLine();*/
			
				}
				break;
				
				default:
					break;
			}

		}
		break;
		
		case WMMP_SECURITY_OPER_DISABLE:
		{
			switch(error)
			{
				case WMMP_SECURITY_OPER_ERROR_OK:
				{
					mainContext_p->SecurityContext.SIMPin1Active = false;
				}
				break;
				
				default:
					mainContext_p->SecurityContext.SIMPin1Active = true;
					break;				
			}
		}
		break;
		
		case WMMP_SECURITY_OPER_ENABLE:
		{
			switch(error)
			{
				case WMMP_SECURITY_OPER_ERROR_OK:
				{

					mainContext_p->SecurityContext.SIMPin1Active = true;

				}
				break;
				
				default:
					//下发cpin 流程中，会调用writetlv 直接写值，若调用失败，此处恢复
					mainContext_p->SecurityContext.SIMPin1Active = false;
					break;				
			}
		}
		break;	
		
		case  WMMP_SECURITY_OPER_CHANGE:
		{
			switch(error)
			{
				case WMMP_SECURITY_OPER_ERROR_OK:
				{
					
				}
				break;
				
				default:
					break;				
			}
		}
		break;	
		
		default:
			break;
	}	
}

void WPA_ProcessSockConnect(u32 socket,u16 error)
{
	WmmpContext_t *mainContext_p = ptrToWmmpContext();
	PDUContext_t   *pduContext = &mainContext_p->PDUContext;
	u16	loopi;

	WMMP_LIB_DBG_TEMP("ProcessSockConnect socketId 0x%x  error %d",socket,error);
	if(socket== mainContext_p->wmmpSocket)
	{
	   	WMMP_LIB_DBG_TEMP("ProcessSockConnect wmmpsocket 0x%x connected",socket);
        WPI_send_urc(WMMP_AT_EVENT_STR"%d,%d,%d,%d,%s", WMMP_AET_BEARER_LINK, 0,1,2, "\"link  connected\"");
        WPI_send_urc(WMMP_AT_EVENT_STR"%d,%s,%s", WMMP_AET_LIB_STATE, "0020", "\"WMMP-T link to platform is ready\"");
		if(0 == error)
		{
			wmmpSendPDU(pduContext);
		}
		else
		{
			wmmpLibCloseConnect(socket);
		}		

	}
	else
	{	
		WMMP_LIB_DBG_TEMP("ProcessSockConnect app or other socket wmmpOpenSkt[%d]  0x%x ",currAppNum,mainContext_p->wmmpOpenSkt[currAppNum]);
		if(0 == error)
		{
			WMMP_LIB_DBG_TEMP("ProcessSockConnect app or other socket ok ");

			mainContext_p->Wmmp_status.app_status[socket-1] = APPLINK_OK;
			WMMP_LIB_DBG_TEMP("ProcessSockConnect wmmpOpenSkt[%d] 0x%x connected",loopi,socket);
			WPI_send_urc(WMMP_AT_EVENT_ACK_STR"%d\r\n%d,%d,\"%s\",%d,%d%s", appTransaction_Id[loopi], loopi,1,"ok",WMMP_MAX_NET_DATA_SIZE ,MAX_SEND_BSD_LEN,"\r\nOK");

		}
		else
		{
			WMMP_LIB_DBG_TEMP("ProcessSockConnect app or other socket error");
			WPI_send_urc(WMMP_AT_EVENT_ACK_STR"%d\r\n%d,%d,\"%s\",%d,%d%s", appTransaction_Id[loopi],loopi,2,"error",WMMP_MAX_NET_DATA_SIZE ,MAX_SEND_BSD_LEN,"\r\nOK");
			wmmpLibCloseConnect(socket);
		}

	}
		
}

void WPA_ProcessGPRSActiveEvent(bool ret)
{
	WmmpContext_t  *mainContext_p = ptrToWmmpContext();	
    
        WPIEventData_t WPIEventData;
    
	WMMP_LIB_DBG_TEMP("WPA_ProcessGPRSActiveEvent maxOpenGPRSCount %d",mainContext_p->maxOpenGPRSCount);
	if(ret== true)
	{
            WPI_send_urc(WMMP_AT_EVENT_STR"%d,%d,%d,%d,%s", WMMP_AET_BEARER_LINK, 0,1,1, "\"link  opening\"");
            
		mainContext_p->Wmmp_status.net_status = LINK_OK;		
		mainContext_p->maxOpenGPRSCount = 0;
      		//appCreateSocketConnect(0);  /*bob remove 20110118*/
		wmmpCreateSocketConnect();	
	}
	else
	{
		WMMP_LIB_DBG_TEMP("WPA_ProcessGPRSActiveEvent ret false maxOpenGPRSCount %d",mainContext_p->maxOpenGPRSCount);

		if(mainContext_p->maxOpenGPRSCount == 3)
		{
			mainContext_p->maxOpenGPRSCount = 0;
			gM2MStatus = WMMP_STATE_ABNORMAL_TRAPPING;
			ActiveGprsTrapAbortWarn(1,1,0x20);  
			wmmpProcessErrorRoll();
			gM2MStatus = WMMP_STATE_ALARM_ACTIVE_GPRS_FAILED;

                	//status_ind should always send to ci
                	if(mainContext_p->WPIEventFucPtr!=PNULL)
                	{
                        WPIEventData.WPIStatusIndEvent.type = 1;
                        WPIEventData.WPIStatusIndEvent.value= WMMP_STATE_ALARM_ACTIVE_GPRS_FAILED;	
                        WPIEventData.WPIStatusIndEvent.reason = mainContext_p->PDUUsage;
                        
                        (mainContext_p->WPIEventFucPtr)(WPI_STATUS_IND_EVENT,&WPIEventData);		
                	}		


            
		}		
		mainContext_p->maxOpenGPRSCount++;		
		
		wmmpLibDectiveGprs();		
		//wmmpProcessError();	 /*bob remove 20110126*/
	}
}

void WPA_ProcessGPRSDeActiveEvent(void)
{
	u16  loopi;
	WmmpContext_t *mainContext_p = ptrToWmmpContext();

	
	WMMP_LIB_DBG_TEMP("WMMP stack---WPA_ProcessGPRSDeActiveEven\n");
	gM2MStatus = WMMP_STATE_DETACH_BEARER;
	mainContext_p->enableSend=true;	
	mainContext_p->wmmpSocket= CONNREF_INVALID;
	mainContext_p->Wmmp_status.net_status = LINK_FAILED;
	for(loopi = 0;loopi < MAX_WMMP_APP_NUM;loopi++)
	{
		mainContext_p->wmmpOpenSkt[loopi]=CONNREF_INVALID;
		mainContext_p->Wmmp_status.app_status[loopi] = APPLINK_FAILED;
		m2mHasAppDataFlag[loopi] = 0;
	}	
	wmmpStopInterTimer(SOCKET_CLOSE_TIMEOUT_TIMER);/* add cbc@20100421 */  /*7*/
	 
	wmmpSendNextCommand();
	
}



void WPA_ProcessSockReadEvent(u32 socket,char* data, u32 size)
{
	WmmpContext_t *mainContext_p = ptrToWmmpContext();
	WPIEventData_t WPIEventData;
	char *pl_StrPtr = PNULL;
	char* RecvBuf=PNULL;
	u16			loopi = 0;
	//t_mtcp_IpRxDataCnf  *pl_RxDataCnf = NULL;
	char *pData = NULL;
	u32		tmpSize = 0;
	u16 	*TLVtag = NULL;
	u8 	TLVnum = 0;  
	WmmpCustomContext_t  *CustomContext_p = &(ptrToWmmpContext()->CustomContext);
	bool	 found = false;	
	WmmpCustom3Context_t  *Custom3Context_p = &(ptrToWmmpContext()->Custom3Context);
	char		tmpRet[30] = "";

	WMMP_LIB_DBG_TEMP("WPA_ProcessSockReadEvent mainContext->PDUUsage %d",mainContext_p->PDUUsage);
	WMMP_LIB_DBG_TEMP("socket =0x%x,wmmpSocket=0x%x bhvValue[5] %d",socket , mainContext_p->wmmpSocket,bhvValue[5]);
	WMMP_LIB_DBG_TEMP("wmmpOpenSkt[%d] 0x%x size %d",currAppNum,mainContext_p->wmmpOpenSkt[currAppNum],size);
	if (socket == mainContext_p->wmmpSocket)
	{
		u16 actualLen;// test for temp server=

		//the length of PDU header is 28
		if((size!=SOCKET_ERROR)&&(size>=28))
		{
			//for temp server, the server will add many 0x00 after the PDU
			WMMP_LIB_DBG_TEMP("WMMP stack---it's wmmp PDU");
			actualLen = Readu16(data);			
			WPA_Decode(WMMP_TRANS_GPRS, data,actualLen);
		}
		else
		{			
			WMMP_LIB_DBG_TEMP("WMMP stack---it's not wmmp PDU");
		}		
	}
	else
    {
        for(loopi = 0;loopi<MAX_WMMP_APP_NUM;loopi++)
        {
            if(socket == mainContext_p->wmmpOpenSkt[loopi])
            {
                found = true;
                break;
            }
        }
        if(found)
        {
            WMMP_LIB_DBG_TEMP("socket == mainContext_p->wmmpOpenSkt\n");

            //if (size<=MAX_IO_BUFFER)
            if (size>WMMP_MAX_NET_DATA_SIZE )
            {		
                WMMP_LIB_DBG_TEMP("Recv data len over %d\n",WMMP_MAX_NET_DATA_SIZE);
                //RecvBuf = WPI_Malloc( MAX_IO_BUFFER);
                //RecvBuf = WPI_Malloc(size);
            }

            //if(RecvBuf!=PNULL)
            {
               // memcpy(RecvBuf,data,size); 
                WMMP_LIB_DBG_TEMP("wmmpOpenSkt receive %d\n",size);

                if(size!=0)
                {		
                    if(mainContext_p->WPIEventFucPtr!=PNULL)
                    {
                        //WPIEventData.WPIAppSockReadEvent.data= RecvBuf;
                        WPIEventData.WPIAppSockReadEvent.data= data;
                        WPIEventData.WPIAppSockReadEvent.datalength= size;
                        WPIEventData.WPIAppSockReadEvent.linkindex = loopi;
                        (mainContext_p->WPIEventFucPtr)(WPI_APPSOCKET_READ_EVENT,&WPIEventData);

                    }
                    else
                    {
                        //WPI_Free(RecvBuf);
                    }
                }
            }
//            else
//            {
//                WMMP_LIB_DBG_TEMP("RecvBuf is null\n");
//            }
        }
    }
}

void PrintCurrentStateDescription(u8 state,char* value)
{
	WMMP_LIB_DBG_TEMP("PrintCurrentStateDescription state=%d", state);

	switch(state)
	{
		case 0:
			strcpy((char*)value, (char*) "ok");
			break;
		case 1:
			strcpy((char*)value, (char*) "failed");
			break;
		case 2:
			strcpy((char*)value, (char*) "aborted");
			break;
		case 3:
			strcpy((char*)value, (char*) "done");
			break;
	}

}


void WPA_ProcessSockCloseEvent(u32 socket,u16 error)
{
    u16   loopi ;
    WmmpContext_t *mainContext_p = ptrToWmmpContext();
    WMMP_LIB_DBG_TEMP("WPA_ProcessSockCloseEvent socket 0x%x,error %d",socket,error);
	//SIMCom_UART("WPA_ProcessSockCloseEvent socket %x,error %d",socket,error);
      //APP link close
      if (socket != mainContext_p->wmmpSocket)
      {
          for(loopi = 0;loopi < MAX_WMMP_APP_NUM;loopi++)
          {
              if (socket== mainContext_p->wmmpOpenSkt[loopi])
              {
                  mainContext_p->wmmpOpenSkt[loopi]= CONNREF_INVALID;
                  mainContext_p->Wmmp_status.app_status[loopi] = APPLINK_FAILED;
                  /*服务器断开连接，上报：$M2MMSG: 8,0,3,"done" */
                  WPI_send_urc("\r\n"WMMP_AT_EVENT_STR"8,%d,3,\"%s\"",loopi,"done");
              }
          }
      }
      wmmpLibCloseConnect(socket);
}



void WPA_ProcessSockDestroyEvent(u32 socket,u16 error)
{
		WmmpContext_t *mainContext_p = ptrToWmmpContext();
   		u16   loopi ;
		char strTmp[30] = "\0";
		char * pl_StrPtr;
		u16 	*TLVtag = NULL;
		u8 	TLVnum = 0;  
		WmmpCustomContext_t  *CustomContext_p = &(ptrToWmmpContext()->CustomContext);
		bool		rmCtrlTraceRet = false;	
		WmmpCustom3Context_t  *Custom3Context_p = &(ptrToWmmpContext()->Custom3Context);
		
		wmmpStopInterTimer(SOCKET_CLOSE_TIMEOUT_TIMER);
		
		WMMP_LIB_DBG_TEMP("ProcessSockDestroyEvent socket 0x%x ,error %d",socket,error);
		if (socket== mainContext_p->wmmpSocket)
		{
			mainContext_p->wmmpSocket= CONNREF_INVALID;
			WMMP_LIB_DBG_TEMP("ProcessSockDestroyEvent wmmpSocket  0x%x  closed",socket);
		}
		else
        {
            WMMP_LIB_DBG_TEMP("ProcessSockDestroyEvent wmmpDataResult.dataBuffer_p 0x%x ",wmmpDataResult.dataBuffer_p);
            /*maobin@20120601 del , socket在下面for循环中进行匹配*/
            //if(socket == mainContext_p->wmmpOpenSkt[currAppNum])
            {
                for(loopi = 0;loopi < MAX_WMMP_APP_NUM;loopi++)
                {
                    if (socket== mainContext_p->wmmpOpenSkt[loopi])
                    {
                        mainContext_p->wmmpOpenSkt[loopi]= CONNREF_INVALID;
                        WMMP_LIB_DBG_TEMP("ProcessSockDestroyEvent wmmpOpenSkt[%d] 0x%x  closed" ,loopi,socket);
                        mainContext_p->Wmmp_status.app_status[loopi] = APPLINK_FAILED;
                        //m2mHasAppDataFlag[loopi] = 0;  /*bob remove 20120206*/
                        /*主动关闭连接，上报：$M2MMSG:8,0,0,"ok" */
                        //WPI_send_urc("\r\n"WMMP_AT_EVENT_STR"8,%d,0,\"%s\"",loopi,"ok");
                    }
                }	

                /*bob add 20110128 -s*/
                for(loopi = 0;loopi < MAX_WMMP_APP_NUM;loopi++)
                {
                    if (mainContext_p->wmmpOpenSkt[loopi] !=CONNREF_INVALID)
                    {
                        mainContext_p->dataBufferFlag = 1;
                        break;
                    }
                }
                if((wmmpDataResult.dataBuffer_p!=NULL) && (mainContext_p->dataBufferFlag == 0))
                {
                    mainContext_p->dataBufferFlag = 0;
                    WPI_Free(wmmpDataResult.dataBuffer_p);
                    //WPI_free_net_data_buf(wmmpDataResult.dataBuffer_p);
                    wmmpDataResult.dataBuffer_p = NULL;
                }
                /*bob add 20110128 -e*/
                WMMP_LIB_DBG_TEMP("ProcessSockDestroyEvent af Free wmmpDataResult.dataBuffer_p 0x%x ",wmmpDataResult.dataBuffer_p);

            }		

        }
		//only both two socket has been closed ,then shut down 
		 if(mainContext_p->wmmpSocket==CONNREF_INVALID)  /*bob remove 20101111*/
		 {		 	
		 	//WPI_DeactiveGPRS();        	
		 }
		 else
		 {
		 	mainContext_p->enableSend=true;
		 }
}

void WPA_ProcessSMSSendedEvent(bool SendRet)
{
	
}
void WPA_ProcessEvent(WPAEventType Event, WPAEventData_t * EventData)
{
	//WMMP_LIB_DBG_TEMP("WPA_ProcessEvent Event %d",Event);
	switch(Event)
	{
		case WPA_SOCKET_CONNECT_EVENT:
		{
			WMMP_LIB_DBG_TEMP("WPA_ProcessEvent SOCKET_CONNECT");
			WPA_ProcessSockConnect(EventData->WPASocketEvent.SocketID,EventData->WPASocketEvent.error);
			break;
		}
	
		case WPA_SOCKET_RECV_EVENT :
		{
			//WMMP_LIB_DBG_TEMP("WPA_SOCKET_RECV_EVENT:WPI_hex_printf(),socketid=%x\n",EventData->WPASocketRecvEvent.SocketID);		
	      		//WPI_hex_printf(EventData->WPASocketRecvEvent.data,EventData->WPASocketRecvEvent.size);	
			WPA_ProcessSockReadEvent(EventData->WPASocketRecvEvent.SocketID,EventData->WPASocketRecvEvent.data,EventData->WPASocketRecvEvent.size);
			break;
		}

		
		case WPA_SOCKET_CLOSE_EVENT :  
		{
			WMMP_LIB_DBG_TEMP("WPA_ProcessEvent SOCKET_CLOSE");
			//SIMCom_UART("WPA_ProcessEvent SOCKET_CLOSE");
			WPA_ProcessSockCloseEvent(EventData->WPASocketEvent.SocketID,EventData->WPASocketEvent.error);
			break;
		}
		case WPA_SOCKET_DESTROY_EVENT : 	
		{
			WMMP_LIB_DBG_TEMP("WPA_ProcessEvent SOCKET_DESTROY");
			WPA_ProcessSockDestroyEvent(EventData->WPASocketEvent.SocketID,EventData->WPASocketEvent.error);
			break;
		}
		case WPA_CPIN_EVENT:
		{
			WMMP_LIB_DBG_TEMP("WPA_ProcessEvent CPIN");
			WPA_ProcessCpinEvent(EventData->WPACPinEvent.operation,EventData->WPACPinEvent.error);
			break;
		}
		case WPA_GPRS_ACTIVE_EVENT:
		{
			WMMP_LIB_DBG_TEMP("WPA_ProcessEvent GPRS_ACTIVE");
			WPA_ProcessGPRSActiveEvent(EventData->WPAGPRSActiveEvent.ret);
			break;
		}
		case WPA_GPRS_DEACTIVE_EVENT:
		{
			WMMP_LIB_DBG_TEMP("WPA_ProcessEvent GPRS_DEACTIVE");
			WPA_ProcessGPRSDeActiveEvent();
			break;
		}

		case WPA_SMS_SENDED_EVENT:
		{	
			WMMP_LIB_DBG_TEMP("WPA_ProcessEvent SMS_SENDED");
			WPA_ProcessSMSSendedEvent(EventData->WPASMSEvent.ret);
			break;
		}
		case WPA_TIMER_EVENT:
		{
			WPA_ProcessTimerEvent();
			break;
		}

		default:
			break;
	}
}

void WPA_SetWPIEventFucPtr(void (*FucPtr)(WPIEventType,WPIEventData_t *))
{
	WmmpContext_t    *mainContext_p = ptrToWmmpContext();

	WMMP_LIB_DBG_TEMP("WPA_SetWPIEventFucPtr FucPt =%d",FucPtr);
    
	mainContext_p->WPIEventFucPtr = FucPtr;
}

WmmpCfgContext_t *WPA_ptrToConfigTLV(void)
{
	return &(WmmpContext.ConfigContext) ;
}

WmmpUpdateContext_t *WPA_ptrToUpdateTLV(void)
{
	return &(WmmpContext.UpdateContext) ;
}

WmmpStatisticContext_t *WPA_ptrToStatisticTLV(void)
{
	return &(WmmpContext.StatisticContext) ;
}

WmmpTermStatusContext_t *WPA_ptrToStatusTLV(void)
{
	return &(WmmpContext.StatusContext);
}

WmmpControlContext_t * WPA_ptrToControlTLV(void)
{
	return &(WmmpContext.ControlContext);
}

WmmpSecurityContext_t * WPA_ptrToSecurityTLV(void)
{
	return &(WmmpContext.SecurityContext);
}

WmmpCustomContext_t* WPA_ptrToCustomTLV(void)
{
	return &(WmmpContext.CustomContext);	
}

WmmpCustom2Context_t* WPA_ptrToCustom2TLV(void)
{
	return &(WmmpContext.Custom2Context);	
}
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_ADC_GPIO__
WmmpCustom3Context_t* WPA_ptrToCustom3TLV(void)
{
	return &(WmmpContext.Custom3Context);	
}
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/
void WPA_InitConfigTLV(WmmpCfgContext_t *Params_p)
{
	WMMP_LIB_DBG_TEMP("WPA_InitConfigTLV");

	strcpy((char *)Params_p->M2MAPN , "CMNET");

	/*以下是重庆现网配*/
#if 0
	strcpy((char *)Params_p->M2MSMSpecialNum,"1065717341");
	Params_p->M2MIPAddr = 0xDAC92D63;//218.201.45.99
	//Params_p->M2MIPAddr = 0xDACEB02A;//"218.206.176.42"
	Params_p->M2MPort = 0x26FD0000;  /*UDP:9981 TCP:0*/

 #else  /*重庆正式平台*/   /*终端序列号前8位：A9720300*/
	 
   	strcpy((char *)Params_p->M2MSMSpecialNum,"106571734");
	Params_p->M2MIPAddr = 0xDACE050F;//218.206.5.15
	//Params_p->M2MPort = 0x26FD26FD;   //udp 9981
	Params_p->M2MPort = 0x26FD0000;
 
#endif

	//以下是移动测试平台配置
#if 0
	strcpy(Params_p->M2MSMSpecialNum,"1065840436");
	Params_p->M2MIPAddr =0xDACEB02A  ; //"218.206.176.42"
	Params_p->M2MPort = 0x14731474;   //udp 5235  TCP 5236
#endif


	//以下是公司个人端口
#if 0
	Params_p->M2MIPAddr = 0x74E4DD33;//"116.228.221.51"
	Params_p->M2MPort = 0x17C017C2;   //udp 6080  TCP 6082
#endif

	//以下是中兴临时平台
#if 0
	Params_p->M2MIPAddr = 0xDEB113C4;
	Params_p->M2MPort = 0x14721472; 
#endif


	//应用平台地址暂时填的为公司个人端口	
	//strcpy(Params_p->AppSrvIPAddr,"116.228.221.51");
	Params_p->AppSrvIPAddr = 0x74E4DD33;//"116.228.221.51"
	Params_p->AppSrvPort= 0x17C017C2;   //udp 6080  TCP 6082

//#if defined(GUIZHOU_WMMP)
    Params_p->Period = 3600;
//#else
	//heart beat period
	Params_p->Period = 90;		/*重庆正式平台为90s*/
//#endif

	Params_p->MaxLoginRetryTimes = 3;
	Params_p->SMSRecvOvertime = 90;   //60;  //30; /* add by cbc@20100522 for 注册等待平台下发密码超时 */

	//use another custom TLV to control it's length 
	memcpy(Params_p->TLVSyncTagValue,"\x00\x02\x00\x07\x00\x09\x00\x0A\x00\x11",10);

	WMMP_LIB_DBG_TEMP("WPA_InitConfigTLV,APN=%s,%s,%s\n",Params_p->M2MAPN,Params_p->GPRSUserId,Params_p->GPRSPassword);	

}

void WPA_InitUpdateTLV(WmmpUpdateContext_t *Params_p)
{
	//WmmpUpdateContext_t  *UpdateContext_p = &(ptrToWmmpContext()->UpdateContext);
	char  v_CurFileRev[9] = "\0";
	char  v_UpdateFileRev[19] = "";

	WMMP_LIB_DBG_TEMP("WPA_InitUpdateTLV");

	memset(v_CurFileRev,0x00,sizeof(v_CurFileRev));
	sprintf(v_CurFileRev,"%d.%02d",WMMP_MAJOR_VER,WMMP_MINOR_VER);
	memset(Params_p->CurrentFileRev,0x00,sizeof(Params_p->CurrentFileRev));/*tlv 0x100c*/
	memcpy(Params_p->CurrentFileRev,v_CurFileRev,8);
	//memcpy(Params_p->CurrentFileRev,"R300100A",8);

#if 0
	memset(v_UpdateFileRev,0x00,sizeof(v_UpdateFileRev));
	sprintf(v_UpdateFileRev,"WMMP%d.%02d",WMMP_MAJOR_VER,WMMP_MINOR_VER);
	memset(Params_p->UpdateFileRev,0x00,sizeof(Params_p->UpdateFileRev));/*tlv 0x1005*/
	memcpy(Params_p->UpdateFileRev,v_UpdateFileRev,19);		
#endif	
}

void WPA_InitStatisticTLV(WmmpStatisticContext_t *Params_p)
{
	WMMP_LIB_DBG_TEMP("WPA_InitStatisticTLV");

	//just test
	Params_p->StatInfoReport = 0xFF;
	Params_p->ReportTime = 0xFFFFFFFF;

	Params_p->StatStartTime = 0xFFFFFFFF;
	Params_p->statFinishTime = 0xFFFFFFFF;
	// memcpy(sendParams_p->GPRSOperationStatParam,"\x01\x00\x01\x01\x00\x00\x00\x00\xFF\xFF\xFF\xFF",12);
	// memcpy(sendParams_p->GPRSOperationStatReportParam,"\x01\x00\x02\x01\x00\x00\x00\x00\xFF\xFF\xFF\xFF",12);
}

void WPA_InitStatusTLV(WmmpTermStatusContext_t *Params_p)
{
	char  v_FirmwareRev[9] = "\0"; 

	WMMP_LIB_DBG_TEMP("WPA_InitStatusTLV");

	strcpy(Params_p->ProtocolRev,(char*)"30");

#if 0   
	/* 重庆M2M 平台 ----  cbc@20091012: 强制已经注册成功 ,13917999485 -- imsi=460007914127292--imei:351692032182973  */  
	// strcpy(Params_p->TerminalSeriesNum,(char*)"A301300109000009"); 
	/*  重庆M2M 平台----曜硕: 13911796862 -- imsi=460001824127699 -- imei=351692032182974  */
	//strcpy(Params_p->TerminalSeriesNum,(char*)"A301300109000014"); 

	/* 移动研究院M2M 平台13917999485 -- imsi=460007914127292--imei:351692032182973  */
	//strcpy(Params_p->TerminalSeriesNum,(char*)"ASM7000722222222"); //cmcc lab test   

	/*重庆地税8618725652066 */
	strcpy(Params_p->TerminalSeriesNum,(char*)"A311030110000004"); 
#else
	/* 重庆M2M 平台----   cbc@20091012:  正常使用,出厂未注册过 */
	// strcpy(Params_p->TerminalSeriesNum,(char*)"A301300100000000");  

	/* 移动研究院M2M 平台*/
	//strcpy(Params_p->TerminalSeriesNum,(char*)"ASM7000700000000");   

	/*重庆地税8618725652066 */
	strcpy(Params_p->TerminalSeriesNum,(char*)UNREGISTER_TERMINAL_NUM);   
	strcpy(Params_p->TerminalSeriesNum_Bak,(char*)UNREGISTER_TERMINAL_NUM);
  
	/*重庆地税8618725652069 */
	//strcpy(Params_p->TerminalSeriesNum,(char*)"A311030110000003");	
#endif

	memset(Params_p->DEVStatus,0x00,sizeof(Params_p->DEVStatus));
	strcpy(Params_p->DEVStatus,"000100000300");
	memset(Params_p->wcomDEVStatus,0x00,sizeof(Params_p->wcomDEVStatus));
	strcpy(Params_p->wcomDEVStatus,"01,0,03,0");

  	strcpy(Params_p->SupportedDev,(char*)"000100020003");
	Params_p->SupportedDev_Len = strlen(Params_p->SupportedDev);	
	
	Params_p->Majorver = WMMP_MAJOR_VER;
	Params_p->MinorVer = WMMP_MINOR_VER;
	memset(v_FirmwareRev,0x00,sizeof(v_FirmwareRev));
	sprintf(v_FirmwareRev,"%d.%02d",Params_p->Majorver,Params_p->MinorVer);
	memset(Params_p->FirmwareRev,0x00,sizeof(Params_p->FirmwareRev));
	memcpy(Params_p->FirmwareRev,v_FirmwareRev,8);
	
}

void WPA_InitControlTLV(WmmpControlContext_t *Params_p)
{
	Params_p->ConnectMode=1; //  3---tcp  ,  // 1---udp long connect
}

void WPA_InitSecurityTLV(WmmpSecurityContext_t *Params_p)
{

	WMMP_LIB_DBG_TEMP("WPA_InitSecurityTLV");

	memset(Params_p->uplinkPassword,0x00,sizeof(Params_p->uplinkPassword));
	memset(Params_p->downlinkPassword,0x00,sizeof(Params_p->downlinkPassword));
#if 0 /*69*/
	//strcpy((char *)Params_p->uplinkPassword,(char*)"12345678");
	//strcpy((char *)Params_p->downlinkPassword,(char*)"12345678"); 

#else   /*66*/
	/* 上行密码 */
	strcpy((char *)Params_p->uplinkPassword,(char*)"12345679");


	/*重庆M2M 平台 :     下行密码 FOR  重庆M2M 平台 */
	strcpy((char *)Params_p->downlinkPassword,(char*)"12345679"); //cmcc chongqing
	/*移动研究院M2M 平台:     下行密码 FOR 移动研究院M2M 平台  */
	//strcpy((char *)Params_p->downlinkPassword,(char*)"87654321"); //

	/*移动研究院M2M 平台:     基础密钥*/
	//strcpy(Params_p->publickey,(char*)"112233445566778899001234");  //cmcc lab test	  // 基础密钥
#endif

	Params_p->uplinkPasswordExpDate=0xFFFFFFFF;
	Params_p->downlinkPasswordExpDate=0xFFFFFFFF;
	Params_p->publicKeyExpDate=0xFFFFFFFF;

#if 0  /* cbc@20091012: 调试使用，强制已经注册成功 */
	//strcpy(Params_p->BindIMSIOrig,(char*)"460007914127292");/* IMSI --> 13917999485 */
	//strcpy(Params_p->BindIMSIOrig,(char*)"460001824127699");/* IMSI --> 13911796862 */
	 /*重庆地税8618725652066 */
	strcpy(Params_p->BindIMSIOrig,(char*)"460027256500485");   
	
	/*重庆地税8618725652069 */
	//strcpy(Params_p->BindIMSIOrig,(char*)"460027256500488");   
#else
	/* cbc@20091012:  正常使用,出厂未注册过 */
	memset(Params_p->BindIMSIOrig,0,sizeof(Params_p->BindIMSIOrig));
	
#endif

	Params_p->SIMPin1Active = 0;
	strcpy((char *)Params_p->SIMPin1,(char*)"12345678");
}

void WPA_InitCustomTLV(WmmpCustomContext_t *Params_p)
{
	WMMP_LIB_DBG_TEMP("WPA_InitCustomTLV");

	Params_p->TLVSyncTagNum = 5;
  	Params_p->MaxPDULength=900;	
	Params_p->remoteCtrlTraceEnabel = 0;

	  /* 重庆M2M 平台----   cbc@20091012:  正常使用,出厂未注册过 */
	//strcpy((char *)Params_p->UnregTerminalNum,(char *)"A301300100000000");
	  /* 移动研究院M2M 平台*/
      //strcpy((char *)Params_p->UnregTerminalNum,(char*)"ASM7000700000000");  

	 /*重庆地税8618725652066 */
	strcpy(Params_p->UnregTerminalNum,(char*)UNREGISTER_TERMINAL_NUM);   
	
	/*重庆地税8618725652069 */
	//strcpy(Params_p->UnregTerminalNum,(char*)"A001000100000000");
	WMMP_LIB_DBG_TEMP("WPA_InitCustomTLV  TLVSyncTagNum %d",Params_p->TLVSyncTagNum);
  
}


void WPA_InitCustom2TLV(WmmpCustom2Context_t *Params_p)
{
	WMMP_LIB_DBG_TEMP("WPA_InitCustom2TLV");

#if 1 
  #if 1
	/*重庆移动*/
	Params_p->AppSerConnMode[0] = WMMP_TCP_CONNECT;  
	Params_p->AppSerAddr[0] = 0xDACE18C3;
	Params_p->AppSerPort[0] = 0x139D;        /*5021*/  
	Params_p->AppSerType[0] = 4;
	strcpy(Params_p->AppServerString[0],(char *)"4tcp://218.206.24.195:5021");
#else	/*宝信平台*/
	Params_p->AppSerConnMode[0] = WMMP_TCP_CONNECT;  
	Params_p->AppSerAddr[0] = 0xDB860AC7;
	Params_p->AppSerPort[0] = 0x304e;        /*12366*/  
	Params_p->AppSerType[0] = 4;
	strcpy(Params_p->AppServerString[0],(char *)"4tcp://218.201.10.199:12366");
	#endif
#else
	Params_p->AppSerConnMode[0] = WMMP_TCP_CONNECT;   /*simcom testing*/
	Params_p->AppSerAddr[0] = 0x74E4DD33;
	//Params_p->AppSerPort[0] = 0x15B3;        /*hl 5555*/
	Params_p->AppSerPort[0] = 0x1A6E;			/*hjy 6766*/
	Params_p->AppSerType[0] = 4;
	//strcpy(Params_p->AppServerString[0],(char *)"4tcp://116.228.221.51:5555");  /*hl*/
	strcpy(Params_p->AppServerString[0],(char *)"4tcp://116.228.221.51:6766");  /*hjy*/

	/*hjy IP:     116.228.221.51 TCP端口号6766 UDP端口号6767 */

#endif

	Params_p->AppSerConnMode[1] = WMMP_TCP_CONNECT;  
	Params_p->AppSerAddr[1] = 0xDB860AC7;
	Params_p->AppSerPort[1] = 0xEB14;        /*60180*/  
	Params_p->AppSerType[1] = 4;
	strcpy(Params_p->AppServerString[1],(char *)"4tcp://219.134.185.69:60180");


	Params_p->AppSerConnMode[2] = WMMP_TCP_CONNECT;  
	Params_p->AppSerAddr[2] = 0xD2B040CC;
	Params_p->AppSerPort[2] = 0x13B4;        /*5044*/  
	Params_p->AppSerType[2] = 4;
	strcpy(Params_p->AppServerString[2],(char *)"4tcp://210.176.64.204:5044");

	Params_p->AppSerConnMode[3] = WMMP_TCP_CONNECT;   /*simcom testing*/
	Params_p->AppSerAddr[3] = 0x74E4DD33;
	Params_p->AppSerPort[3] = 0x15B3;        /*5555*/  
	Params_p->AppSerType[3] = 4;
	strcpy(Params_p->AppServerString[3],(char *)"4tcp://116.228.221.51:5555");

#if 1
	Params_p->AppSerConnMode[4] = WMMP_TCP_CONNECT;  
	Params_p->AppSerAddr[4] = 0xDAC90AC7;
	Params_p->AppSerPort[4] = 0x304e;        /*12366*/  
	Params_p->AppSerType[4] = 4;
	strcpy(Params_p->AppServerString[4],(char *)"4tcp://218.201.10.199:12366");
#else
	Params_p->AppSerConnMode[4] = WMMP_UDP_CONNECT;   /*simcom testing*/
	Params_p->AppSerAddr[4] = 0x74E4DD33;
	Params_p->AppSerPort[4] = 0x1A73;        /*udp:6771*/   
	Params_p->AppSerType[4] = 4;
	strcpy(Params_p->AppServerString[4],(char *)"4udp://116.228.221.51:6771");
#endif

#if 0
	Params_p->AppSerConnMode[5] = WMMP_TCP_CONNECT;   /*simcom testing*/
	Params_p->AppSerAddr[5] = 0x74E4DD33;
	Params_p->AppSerPort[5] = 0x1A72;        /*tcp:6770*/  
	Params_p->AppSerType[5] = 4;
	strcpy(Params_p->AppServerString[5],(char *)"4tcp://116.228.221.51:6770");
	
	Params_p->AppSerConnMode[6] = WMMP_TCP_CONNECT;   /*simcom testing*/
	Params_p->AppSerAddr[6] = 0x74E4DD33;
	//Params_p->AppSerPort[0] = 0x15B3;        /*hl 5555*/
	Params_p->AppSerPort[6] = 0x1A6E;			/*hjy 6766*/
	Params_p->AppSerType[6] = 4;
	//strcpy(Params_p->AppServerString[0],(char *)"4tcp://116.228.221.51:5555");  /*hl*/
	strcpy(Params_p->AppServerString[6],(char *)"4tcp://116.228.221.51:6766");  /*hjy*/
	/*hjy IP:     116.228.221.51 TCP端口号6766 UDP端口号6767 */
#else
	Params_p->AppSerConnMode[5] = WMMP_TCP_CONNECT;   /*simcom new testing*/
	Params_p->AppSerAddr[5] =  0x74ECDD4B;  /*116.236.221.75*/       /*0x74E7DD4B;116.231.221.75*/
	Params_p->AppSerPort[5] = 0x1A72;        /*tcp:6770*/  
	Params_p->AppSerType[5] = 4;
	strcpy(Params_p->AppServerString[5],(char *)"4tcp://116.231.221.75:6770");
	
	Params_p->AppSerConnMode[6] = WMMP_TCP_CONNECT;   /*simcom testing*/
	Params_p->AppSerAddr[6] =   0x74ECDD4B;        /*0x74E4DD33;116.228.221.51*/
	//Params_p->AppSerPort[0] = 0x15B3;        /*hl 5555*/
	Params_p->AppSerPort[6] = 0x1A6E;			/*hjy 6766*/
	Params_p->AppSerType[6] = 4;
	//strcpy(Params_p->AppServerString[0],(char *)"4tcp://116.228.221.51:5555");  /*hl*/
	strcpy(Params_p->AppServerString[6],(char *)"4tcp://116.228.221.51:6766");  /*hjy*/
	/*hjy IP:     116.228.221.51 TCP端口号6766 UDP端口号6767 */

#endif
	

}

WmmpDecodeRecvResult_e  WPA_Decode(WmmpTransType_e type, char* data ,u16 length)
{
    u16 loopi = 0;
    WmmpContext_t *mainContext_p = (WmmpContext_t *)ptrToWmmpContext();


    WMMP_LIB_DBG_TEMP("WPA_Decode type %d length %d ",type, length);

#if 0	
    for(loopi = 0;loopi<length;loopi++)
    {
    WMMP_LIB_DBG_TEMP("WPA_Decode data[%d] 0x%02x",loopi,data[loopi]);
    }
#endif   

    memcpy(mainContext_p->IOBuffer,data,length);
    return RecvCommand(type, length);
}

StatusNumber_t * WPA_GetWmmpStatus(void)
{
	WmmpContext_t *mainContext_p = ptrToWmmpContext();
	return &(mainContext_p->Wmmp_status);
}

bool WPA_isWmmpRun(void)
{
	WmmpContext_t *mainContext_p = ptrToWmmpContext();
	WMMP_LIB_DBG_TEMP("WPA_isWmmpRun m2mswitch %d",mainContext_p->m2mswitch);
	return mainContext_p->m2mswitch;	
}

void  WPA_ReadTLV(u16 Tag_Id,TLVRetVal_t* ret)
{
	 ReadTLV(Tag_Id,ret);
}

void WPA_WriteTLV(u16 Tag_Id, u16 length, char* value) 
{
        if(length)
        {
        WriteTLV(Tag_Id, length, value) ;
        }
}

bool WPA_CheckTLV(u16 Tag_Id, u16 length, char* value)
{
	return CheckTLV(Tag_Id, length, value);
}

bool WPA_CheckTLVTag(u16 Tag_Id)
{
	return CheckTLVTag(Tag_Id);
}

void WPA_Set_Power_On_Step(AppPONStep flag)
{
	AppWmmpContext_t *appWmmpContext_p = ptrToAppWmmpContext();
	WMMP_DBG_TEMP("WPA_Set_Power_On_Step flag =%d", flag);

	if (flag < WMMP_PON_MAX)
	{
		appWmmpContext_p->power_on_step = flag;
	}
}

AppPONStep WPA_Get_Power_On_Step(void)
{
	AppWmmpContext_t *appWmmpContext_p = ptrToAppWmmpContext();
	WMMP_DBG_TEMP("WPA_Get_Power_On_Step  =%d", appWmmpContext_p->power_on_step);

	return appWmmpContext_p->power_on_step;
}


//#undef WMMP_PROTOCAL
#endif
