#ifndef _WMMP_INTERFACE_H_
#define _WMMP_INTERFACE_H_

/******************************************************************************
* Include Statements
******************************************************************************/
#include "wmmp_basic_type.h"
#include "eat_wmmp.h"
#include "eat_nvram.h"
#include "eat_interface.h"
#include "eat_timer.h"
/******************************************************************************
* Macros
******************************************************************************/
/*DEBUG接口*/
#ifndef __SIMCOM_WMMP_DEBUG__
#define __SIMCOM_WMMP_DEBUG__
#endif 

#ifdef __SIMCOM_WMMP_DEBUG__
extern  void wmmp_trace(u8 level,const char* fun, const char* file, u32 line,const char *fmt,...);
#endif 
#if 0
#ifdef __SIMCOM_WMMP_DEBUG__
#define WMMP_DBG_TEMP(...)        wmmp_trace(1,__func__ ,__FILE__,__LINE__,__VA_ARGS__)
#define WMMP_DBG_E_TEMP(...)	        wmmp_trace(0,__func__,__FILE__,__LINE__,__VA_ARGS__)
#define WMMP_LIB_DBG_TEMP(...)     wmmp_trace(1,__func__,__FILE__,__LINE__,__VA_ARGS__)
#define WMMP_LIB_DBG_E_TEMP(...)     wmmp_trace(0, __func__,__FILE__,__LINE__,__VA_ARGS__)

#define WMMP_DBG(...)    kal_trace(TRACE_INFO, __VA_ARGS__)
#define WMMP_DBG_E(...)    kal_trace(TRACE_ERROR, __VA_ARGS__)
#define WMMP_LIB_DBG(...)    kal_trace(TRACE_INFO, __VA_ARGS__)
#define WMMP_LIB_DBG_E(...)    kal_trace(TRACE_ERROR, __VA_ARGS__)

#else
#define WMMP_DBG_TEMP(...) 
#define WMMP_DBG_E_TEMP(...)

#define WMMP_LIB_DBG_TEMP(...)
#define WMMP_LIB_DBG_E_TEMP(...)

#define WMMP_DBG(...) 
#define WMMP_DBG_E(...)

#define WMMP_LIB_DBG(...)
#define WMMP_LIB_DBG_E(...)
#endif
#else
#define WMMP_DBG_TEMP       eat_trace 
#define WMMP_DBG_E_TEMP	 eat_trace 
#define WMMP_LIB_DBG_TEMP     eat_trace
#define WMMP_LIB_DBG_E_TEMP     eat_trace
#endif

#ifndef MAX
#define MAX(a,b) ((a)>(b))?(a):(b)
#endif 

#ifndef MIN
#define MIN(a,b) ((a)<(b))?(a):(b)
#endif 

//void* WPI_get_net_data_buf(u32 size);
//void WPI_free_net_data_buf(void* p);
#define WPI_Malloc eat_mem_alloc
#define WPI_Free eat_mem_free
/*时间接口*/
#define WPI_GetCurrentTime eat_WPI_GetCurrentTime

/*网络接口*/
#define WPI_CreateConnect  eat_WPI_CreateConnect
#define WPI_Send eat_WPI_Send 
  
#define WPI_CloseSocket eat_WPI_CloseSocket
#define WPI_ActiveGPRS eat_WPI_ActiveGPRS
#define WPI_DeactiveGPRS eat_WPI_DeactiveGPRS
#define WPI_SendSMS eat_WPI_SendSMS
#define WPI_sleep eat_sleep

#define WPI_GetSimState eat_WPI_GetSimState
#define WPI_GetIMEI eat_WPI_GetIMEI
#define WPI_GetIMSI eat_WPI_GetIMSI
#define WPI_GetCellID eat_WPI_GetCellID
#define WPI_GetLAC eat_WPI_GetLAC
#define WPI_GetCSQLevel eat_WPI_GetCSQLevel

#define WPI_SecurityAutoCpin eat_WPI_SecurityAutoCpin
#define WPI_SecurityChangeCpin eat_WPI_SecurityChangeCpin
#define WPI_SecurityEnableCpin eat_WPI_SecurityEnableCpin
#define WPI_SecurityDisableCPIN eat_WPI_SecurityDisableCPIN
// wmmpProcessWPIEvent(WPIEventType Event, WPIEventData_t * EventData);
#define WPI_sendRxDataCnf  eat_WPI_sendRxDataCnf

//nvram interface
#define WPI_WriteNvram eat_nvram_write
#define WPI_ReadNvram eat_nvram_read

EatTimer_enum wmmp_get_lib_timer_id(void);
void wmmp_set_lib_timer_id(EatTimer_enum id);
void WPI_StartTimer(u32 timeoutPeriod);
void WPI_StopTimer(void);

extern void wmmp_SendSMS(char *message, char *num, u8 messagelen);

#endif // _WMMP_INTERFACE_H_
