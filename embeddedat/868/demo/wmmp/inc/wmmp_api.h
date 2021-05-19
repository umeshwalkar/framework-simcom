#if defined(__SIMCOM_WMMP__)
#ifndef _WMMP_API_H_
#define _WMMP_API_H_

/*SIMCOM Laiwenjie 2013-01-17 Fix MKBug00014691 for change nvram interface BEGIN*/
#ifdef __SIMCOM_WMMP__
#include "simcom_wmmp_nvram_define.h"
#endif
/*SIMCOM Laiwenjie 2013-01-17 Fix MKBug00014691 for change nvram interface END*/
#include "simcom_wmmp_utility.h"

void WPA_StackInit(void);
bool WPA_AppInit(void);
bool WPA_Init(void);
bool WPA_Term(void);
bool WPA_LoginReq(void);
bool WPA_LogoutReq(u8 reason);
bool  RemoteCtrlTraceTrapReq(u16*TLVTags, u16 TLVNum);
bool WPA_TrapReq(u16*TLVTags, u16 TLVNum);
bool WPA_LocalCfgReq(u16*TLVTags, u16 TLVNum);
bool WPA_ReqReq(u16*TLVTags, u16 TLVNum);
bool WPA_DataReq(	char type,char	*senddata,u16	datalength, char *destination);
bool WPA_FileReq(	char* TransID,u8 Status, u32 FileReadPoint,u16 FileBlockSize);
bool WPA_adFileReq(	char* TransID,u8 Status,u32 FileReadPoint,u16 FileBlockSize);
bool WPA_AppBrOpenReq(void);
bool WPA_AppBrSendReq( char	*senddata,u16	datalength);
bool WPA_AppBrCloseReq(void);
void  WPA_AppOpenReq(u8 appNum,wmmpCreateConnectCallback callback);
bool  WPA_AppSendReq(u8 appNum, char	*senddata,u16 datalength,wmmpSendCallback callBack);
bool WPA_MaxDataAppSendReq(u8 appNum, char	*senddata,u16	datalength);
bool WPA_AppCloseReq(u8 appNum );
bool WPA_ClearSecurity(u8 SecurityParam);

void WPA_SetWPIEventFucPtr(void (*FucPtr)(WPIEventType,WPIEventData_t *));
WmmpCfgContext_t *WPA_ptrToConfigTLV(void);
WmmpUpdateContext_t *WPA_ptrToUpdateTLV(void);
WmmpStatisticContext_t *WPA_ptrToStatisticTLV(void);
WmmpTermStatusContext_t *WPA_ptrToStatusTLV(void);
WmmpControlContext_t * WPA_ptrToControlTLV(void);
WmmpSecurityContext_t * WPA_ptrToSecurityTLV(void);
WmmpCustomContext_t* WPA_ptrToCustomTLV(void);
void WPA_InitConfigTLV(WmmpCfgContext_t *Params_p);
void WPA_InitUpdateTLV(WmmpUpdateContext_t *Params_p);
void WPA_InitStatisticTLV(WmmpStatisticContext_t *Params_p);
void WPA_InitStatusTLV(WmmpTermStatusContext_t *Params_p);
void WPA_InitControlTLV(WmmpControlContext_t *Params_p);
void WPA_InitSecurityTLV(WmmpSecurityContext_t *Params_p);
void WPA_InitCustomTLV(WmmpCustomContext_t *Params_p);
WmmpDecodeRecvResult_e  WPA_Decode(WmmpTransType_e type, char* data ,u16 length);
StatusNumber_t * WPA_GetWmmpStatus(void);
bool WPA_isWmmpRun(void);
void  WPA_ReadTLV(u16 Tag_Id,TLVRetVal_t* ret);
void WPA_WriteTLV(u16 Tag_Id, u16 length, char* value) ;
bool WPA_CheckTLV(u16 Tag_Id, u16 length, char* value);
bool WPA_CheckTLVTag(u16 Tag_Id);
void WPA_ProcessEvent(WPAEventType Event, WPAEventData_t * EventData);


void WPA_SetStatusIndFucPtr(void (* FucPtr)(u16,u16));
void WPA_SetTrapAckFucPtr(void (*FucPtr)(u8,u16));
void WPA_SetReqAckFucPtr(void (*FucPtr)(u8,u16));
void WPA_SetDataAckFucPtr(void (* FucPtr)(u8));
void WPA_SetDownNotifyFucPtr(void (* FucPtr)(char *));
void WPA_SetFileAckFucPtr(void (* FucPtr)(char *,char *,u32,u16,char *,u8));
void WPA_SetServerDataReqFucPtr(void (* FucPtr)(char ,char *,char *,u16));
void WPA_SetAppRecvFucPtr(void (*FucPtr)(char *,u16));
void WPA_Set_Power_On_Step(AppPONStep flag);
AppPONStep WPA_Get_Power_On_Step(void);
#endif
#endif
