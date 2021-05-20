/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :   app_demo_least.c
 * version       :   0.20
 * Description   :   This is the smallest sample code,only include initialized
 *                   code.
 * Authors       :   maobin
 * Notes:   
 *       1.All the codes are necessary for customer.
 *                
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2012-09-24, maobin, create originally.
 *0.20  2013-02-10, maobin, add c library initialize code.
 *--------------------------------------------------------------------
 ********************************************************************/

/********************************************************************
 * Include Files
 ********************************************************************/
#include <rt_misc.h>
#include "eat_interface.h"
#include "eat_wmmp.h"
#include "eat_nvram.h"
#include "wmmp_api.h"
#include "simcom_wmmp_utility.h"
#include "wmmp_typ.h"
#include "wmmp_interface.h"
#include "simcom_wmmp_nvram_define.h"
#include "wmmp_lib.h"
/********************************************************************
 * Macros
 ********************************************************************/
#define	MAX_SOC_RECV_LENGTH		WMMP_MAX_NET_DATA_SIZE 
#define WMMP_CORE_MEM_POOL_LEN  300*1024
#undef EAT_CORE_APP_INDEP
/********************************************************************
 * Types
 ********************************************************************/
typedef void (*app_user_func)(void*);
/********************************************************************
 * Extern Variables (Extern /Global)
 ********************************************************************/
extern char  g_simcom_m2m_at_buff[300*1024] ;
extern u16 simcom_m2m_data_len;

/********************************************************************
 * Local Variables:  STATIC
 ********************************************************************/
static AppWmmpContext_t	s_appWmmpContext;
static Wmmp_mmi_abm_account_info_struct g_wmmp_account[MAX_SIMCOM_WMMP_CONTEXT_NUM]; 
u8    gLoginStart = 0;

s8 wmmprecvbuff[MAX_SOC_RECV_LENGTH];/*保存接收的数据*/

extern u8 gwopenValue;
Wmmp_atc_value_struct atcValueNv={0};
char g_commom_imei_str[16]={0};//存储IMEI号
char g_commom_imsi_str[17]={0};//存储IMSI

static char wmmp_core_mem_pool[WMMP_CORE_MEM_POOL_LEN];
static EatUart_enum eat_at_port = EAT_UART_1;
/********************************************************************
 * External Functions declaration
 ********************************************************************/
extern void APP_InitRegions(void);

/********************************************************************
 * Local Function declaration
 ********************************************************************/
void app_main(void *data);
void app_func_ext1(void *data);
/********************************************************************
 * Local Function
 ********************************************************************/
#pragma arm section rodata="APPENTRY"
const EatEntry_st AppEntry = 
{
    app_main,
    app_func_ext1,
    (app_user_func)EAT_NULL,//app_user1,
    (app_user_func)EAT_NULL,//app_user2,
    (app_user_func)EAT_NULL,//app_user3,
    (app_user_func)EAT_NULL,//app_user4,
    (app_user_func)EAT_NULL,//app_user5,
    (app_user_func)EAT_NULL,//app_user6,
    (app_user_func)EAT_NULL,//app_user7,
    (app_user_func)EAT_NULL,//app_user8,
    EAT_NULL,
    EAT_NULL,
    EAT_NULL,
    EAT_NULL,
    EAT_NULL,
    EAT_NULL
};
#pragma arm section rodata
#ifdef EAT_CORE_APP_INDEP
/* C lib function begin*/
__value_in_regs struct __initial_stackheap $Sub$$__user_initial_stackheap(unsigned R0, unsigned SP, unsigned R2, unsigned SL)
{
    struct __initial_stackheap config;

    config.heap_base = R0;
    config.heap_limit =  R2;
    config.stack_base =  SP; 
    config.stack_limit =   SL;
    return config;
}

void $Sub$$_fp_init(void)
{
}
void $Sub$$_initio(void)
{
}
__value_in_regs struct __argc_argv $Sub$$__ARM_get_argv(void *a )
{
    /*
       int argc;
       char **argv;
       int r2, r3;
       */
    struct __argc_argv arg;
    arg.argc = 0;
    arg.r2 = 0;
    arg.r3 = 0;
    return arg;
}
/* C lib function end*/
#endif /*EAT_CORE_APP_INDEP*/

void app_func_ext1(void *data)
{
    /*This function can be called before Task running ,configure the GPIO,uart and etc.
      Only these api can be used:
      eat_uart_set_debug: set debug port
      eat_pin_set_mode: set GPIO mode
      eat_uart_set_at_port: set AT port
    */
    EatUartConfig_st cfg =
    {
        EAT_UART_BAUD_115200,
        EAT_UART_DATA_BITS_8,
        EAT_UART_STOP_BITS_1,
        EAT_UART_PARITY_NONE
    };
    eat_uart_set_debug(EAT_UART_2);
	//set debug string info to UART_2 
    eat_uart_set_debug_config(EAT_UART_DEBUG_MODE_UART, &cfg);
    eat_uart_set_at_port(EAT_UART_1);
}

/*****************************************************************************
 * 函数名        : ptrToAppWmmpContext
 * 作  者        : 
 * 功能描述      :获取WMMP APP相关上下文
 * 输入参数说明  : 无
 * 输出参数说明  : 无
 * 返回值说明    : 无
 *****************************************************************************/
AppWmmpContext_t* ptrToAppWmmpContext (void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    return &s_appWmmpContext;
}
/*****************************************************************************
 * 函数名        : simcom_wmmp_timer_init
 * 作  者        : 
 * 功能描述      wmmp stack timer初始化
 * 输入参数说明  : 无
 * 输出参数说明  : 无
 * 返回值说明    : 无
 *****************************************************************************/
static void wmmpTimerInit(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

}

/*****************************************************************************
 *	函数名	      : appWmmpInitialiseContext
 * 作  者        : 
 *	功能描述      :上下文相关初始化
 *	输入参数说明  : 无
 * 输出参数说明  : 无
 *	返回值说明    : 无
 *****************************************************************************/
static void appWmmpInitContext(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    u8       i;
    AppWmmpContext_t *appWmmpContext_p = ptrToAppWmmpContext();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    memset((void*)appWmmpContext_p, 0x00, sizeof(AppWmmpContext_t));
    memset((void*)&g_wmmp_account, 0x00, MAX_SIMCOM_WMMP_CONTEXT_NUM*sizeof(Wmmp_mmi_abm_account_info_struct));

    for(i = 0; i < MAX_SIMCOM_WMMP_CONTEXT_NUM; i++)
    {
        appWmmpContext_p->networkContext[i].bearerType     = WMMP_IP_GPRS_CONFIG;
        appWmmpContext_p->networkContext[i].tcpPort        = 6060	;	
        appWmmpContext_p->networkContext[i].udpPort        = 7070;	
        appWmmpContext_p->networkContext[i].p_account =&g_wmmp_account[i];
    }

    /*SIMCOM maobin@2013-03-14 modify for laiwenjie begin*/
 
   wmmp_nvram_read_req(NVRAM_EF_MIN_LID, NVRAM_EF_MAX_LID);
    /*SIMCOM maobin@2013-03-14 modify for laiwenjie end*/
    WPI_GetIMEI(g_commom_imei_str,16);
    WPI_GetIMSI(g_commom_imsi_str,17);

}

void wmmpProcessWPIEvent(WPIEventType Event, WPIEventData_t * EventData)
{
    char buffer[128]={0};
    u8		ret = 0;
    u16	tag = 0;

    WMMP_DBG_TEMP("wmmpProcessWPIEvent   Event  %d ",Event);
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
                WMMP_DBG_TEMP("wmmpProcessWPIEvent  WPI_SET_REQ_EVENT free  Event  %d ",Event);
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
                WMMP_DBG_TEMP("wmmpProcessWPIEvent  WPI_SOCKET_CREATE_EVENT  ");
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
/*****************************************************************************
 * 函数名	    : wmmp_init
 * 作  者        : 
 * 功能描述      : 初始化
 * 输入参数说明  : 无
 * 输出参数说明  : 无
 * 返回值说明    : 无
 *****************************************************************************/
static void  wmmp_init()
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    eat_trace("  wmmp_init 1");
    WMMP_DBG_TEMP("wmmp_init");

    eat_trace("  wmmp_init 2");
    WPA_StackInit();
    eat_trace("  wmmp_init 3");
    appWmmpInitContext();
    eat_trace("  wmmp_init 4");
    wmmpTimerInit();
    eat_trace("  wmmp_init 5");
    WPA_SetWPIEventFucPtr(wmmpProcessWPIEvent);
    s_appWmmpContext.power_on_step=WMMP_PON_INIT;

    eat_trace("  wmmp_init 6");
    if (1 == gwopenValue)
    {
        WPI_send_urc(WMMP_AT_EVENT_STR"%d,%s %s", WMMP_AET_LIB_STATE, "0010", "\"Initialization started\"");
    }
}
/*****************************************************************************
 * 函数名	    : wmmp_tcpip_start_cnf_proc
 * 作  者        : 
 * 功能描述      : 处理EVENT EAT_EVENT_TCPIP_STARTUP_CNF
 * 输入参数说明  : 无
 * 输出参数说明  : 无
 * 返回值说明    : 无
 *****************************************************************************/
static void  wmmp_tcpip_start_cnf_proc(EatEvent_st* event)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    WmmpNetWorkContext_t    *networkContext_p = NULL;
    AppWmmpContext_t *appWmmpContext_p = ptrToAppWmmpContext();   
    WPAEventData_t WPAEventData;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    WMMP_DBG_TEMP("wmmp_tcpip_start_cnf_proc,result %d",event->data.tcpip_start_Cnf.UpResult);

    if((event->data.tcpip_start_Cnf.UpResult != IP_UP_OK) && (event->data.tcpip_start_Cnf.UpResult != IP_UP_OK_BY_OTHER))
    {
        WMMP_DBG_TEMP("simcom_wmmp_mtcp_ip_start_cnf_handle, open bear error ");
        return;
    }

    networkContext_p=&(appWmmpContext_p->networkContext[event->data.tcpip_start_Cnf.BearRef]);

    WPI_send_urc(WMMP_AT_EVENT_STR"%d,%d,%d %s", WMMP_AET_BEARER_TYPE, 4,2, "\"bearer connected\"");

    networkContext_p->addrLen   = IP_ADDR_LEN;
    memset(&WPAEventData,0,sizeof(WPAEventData_t));
    WPAEventData.WPAGPRSActiveEvent.ret= true;
    WPA_ProcessEvent(WPA_GPRS_ACTIVE_EVENT, &WPAEventData);	    

}
/*****************************************************************************
 * 函数名	      : wmmp_tcpip_shutdown_cnf_proc
 * 作  者        : 
 * 功能描述      : 处理EVENT EAT_EVENT_TCPIP_SHUTDOWN_CNF
 * 输入参数说明  : 无
 * 输出参数说明  : 无
 * 返回值说明    : 无
 *****************************************************************************/
static void  wmmp_tcpip_shutdown_cnf_proc(EatEvent_st* event)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    WPAEventData_t WPAEventData;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    WMMP_DBG_TEMP("wmmp_tcpip_shutdown_cnf_proc,result %d",event->data.tcpip_shutdown_cnf.DownResult);

    if (event->data.tcpip_shutdown_cnf.DownResult==IP_DOWN_OK) 
    {
        WPAEventData.WPASocketEvent.SocketID = event->data.tcpip_shutdown_cnf.BearRef;  
        WPAEventData.WPASocketEvent.error = 0;
    }
    else
    {
        WPAEventData.WPASocketEvent.SocketID = event->data.tcpip_shutdown_cnf.BearRef;
        WPAEventData.WPASocketEvent.error = -1;
    }	   

    WPA_ProcessEvent(WPA_GPRS_DEACTIVE_EVENT, &WPAEventData);
}
/*****************************************************************************
 * 函数名	      : wmmp_tcpip_shutdown_ind_proc
 * 作  者        : 
 * 功能描述      : 处理EVENT EAT_EVENT_TCPIP_SHUTDOWN_IND
 * 输入参数说明  : 无
 * 输出参数说明  : 无
 * 返回值说明    : 无
 *****************************************************************************/
static void  wmmp_tcpip_shutdown_ind_proc(EatEvent_st* event)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    WPAEventData_t WPAEventData;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    WMMP_DBG_TEMP("wmmp_tcpip_shutdown_ind_proc, connect error ");

    WPAEventData.WPASocketEvent.SocketID = event->data.tcpip_shutdown_ind.BearRef;  
    WPAEventData.WPASocketEvent.error = 0;

    WPA_ProcessEvent(WPA_GPRS_DEACTIVE_EVENT, &WPAEventData);
}
/*****************************************************************************
 * 函数名	      : wmmp_tcpip_close_cnf_proc
 * 作  者        : 
 * 功能描述      : 处理EVENT EAT_EVENT_TCPIP_CLOSE_CNF
 * 输入参数说明  : 无
 * 输出参数说明  : 无
 * 返回值说明    : 无
 *****************************************************************************/
static void  wmmp_tcpip_close_cnf_proc(EatEvent_st* event)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    WPAEventData_t WPAEventData;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    WMMP_DBG_TEMP("wmmp_tcpip_close_cnf_proc");

    WPAEventData.WPASocketEvent.SocketID = event->data.tcpip_close_cnf.ConnRef;  
    WPAEventData.WPASocketEvent.error = 0;

    WPA_ProcessEvent(WPA_SOCKET_DESTROY_EVENT, &WPAEventData);
}
/*****************************************************************************
 * 函数名	      : wmmp_tcpip_tx_data_cnf_proc
 * 作  者        : 
 * 功能描述      : 处理EVENT EAT_EVENT_TCPIP_ACTIVE_OPEN_CNF
 * 输入参数说明  : 无
 * 输出参数说明  : 无
 * 返回值说明    : 无
 *****************************************************************************/
static void  wmmp_tcpip_active_open_cnf_proc(EatEvent_st* event)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    WPAEventData_t WPAEventData;
    WmmpContext_t  *mainContext_p = ptrToWmmpContext();
    u8 connRef=event->data.tcpip_activeopen_cnf.ConnRef;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    WMMP_DBG_TEMP("wmmp_tcpip_active_open_cnf_proc,result %d",event->data.tcpip_activeopen_cnf.OpenResult);

    if(event->data.tcpip_activeopen_cnf.OpenResult != SOCKET_OPEN_OK_CONNECT) /*WMMP连接*/
    {
        if(WMMP_CONNREF_WMMP==connRef)
        {
            mainContext_p->wmmpSocket=CONNREF_INVALID;
        }
        else  /*应用连接*/
        {
            mainContext_p->wmmpOpenSkt[connRef-1]=CONNREF_INVALID;
            if(mainContext_p->createConnectCallBack)
            {
                mainContext_p->createConnectCallBack(false,connRef);
            }
        }
        WPAEventData.WPASocketEvent.SocketID =connRef ;
        WPAEventData.WPASocketEvent.error = 1;
    }
    else /*连接成功*/
    {
        if(WMMP_CONNREF_WMMP==connRef)
        {     
            mainContext_p->wmmpSocket=WMMP_CONNREF_WMMP;
        }
        else  /*应用连接*/
        {
            mainContext_p->wmmpOpenSkt[connRef-1]=connRef;
            if(mainContext_p->createConnectCallBack)
            {
                mainContext_p->createConnectCallBack(true,connRef);
            }
        }  

        WPAEventData.WPASocketEvent.SocketID = connRef;
        WPAEventData.WPASocketEvent.error =0;
    }

    WPA_ProcessEvent(WPA_SOCKET_CONNECT_EVENT, &WPAEventData);		
}
/*****************************************************************************
 * 函数名	      :wmmp_tcpip_tx_data_cnf_proc
 * 作  者        : 
 * 功能描述      : 处理EVENT EAT_EVENT_TCPIP_TX_DATA_CNF
 * 输入参数说明  : 无
 * 输出参数说明  : 无
 * 返回值说明    : 无
 *****************************************************************************/
static void  wmmp_tcpip_tx_data_cnf_proc(EatEvent_st* event)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    WmmpDataInterface_t* wmmpDataResult_t = ptrToWmmpDataInterface();
    WmmpContext_t    *mainContext_p = ptrToWmmpContext();
    s32 bytesSent,alreadySent;
    char* buf_p = wmmpDataResult_t->dataBuffer_p;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    WMMP_DBG_TEMP("wmmp_tcpip_tx_data_cnf_proc,sizeProcessed %d",event->data.tcpip_tx_data_cnf.sizeProcessed);



    if(0xFFFFFFFF==event->data.tcpip_tx_data_cnf.sizeProcessed ) /*发送出错，返回*/
    {
        WMMP_DBG_TEMP("wmmp_tcpip_tx_data_cnf_proc,send error");
        wmmpDataResult_t->sendLen=0; /*表示没有在发送*/
        return;
    } 


    wmmpDataResult_t->remLen-=wmmpDataResult_t->sendLen;

    /*分包发送处理*/
    if(mainContext_p->dataBufferFlag)
    {
        if(0==wmmpDataResult_t->remLen)
        {
            WPI_send_urc("\r\n"WMMP_APP_EVENT_MSG_STR"7,%d,%d",event->data.tcpip_tx_data_cnf.ConnRef, wmmpDataResult_t->totalLen);
            mainContext_p->dataBufferFlag = 0;
            wmmpDataResult_t->totalLen = 0;
            wmmpDataResult_t->sendLen=0;
        }
        else /*延时再发下一个数据包*/
        {
            simcom_task_sleep(50);   

            bytesSent = wmmpDataResult_t->remLen;

            if( 1 == mainContext_p->dataBufferFlag && bytesSent > 0)
            {
                if(bytesSent>MAX_SEND_BSD_LEN)
                {    
                    bytesSent = MAX_SEND_BSD_LEN;
                }
                alreadySent = wmmpDataResult_t->totalLen - wmmpDataResult_t->remLen;
            }

            WPI_send_net_data(event->data.tcpip_tx_data_cnf.BearRef+1,event->data.tcpip_tx_data_cnf.ConnRef,bytesSent,buf_p+alreadySent);
        }
    }
    else /*只有一包*/
    {
        if( event->data.tcpip_tx_data_cnf.ConnRef!=WMMP_CONNREF_WMMP ) /*WMMP协议数据不在串口输出*/
        {
            WPI_send_urc("\r\n"WMMP_APP_EVENT_MSG_STR"7,%d,%d",event->data.tcpip_tx_data_cnf.ConnRef,event->data.tcpip_tx_data_cnf.sizeProcessed);
        }		
    }	

    if(mainContext_p->gprsSendCallBack&&0==wmmpDataResult_t->remLen)  /*发送结束，回调处理*/
    {
        mainContext_p->gprsSendCallBack(event->data.tcpip_tx_data_cnf.sizeProcessed);
    }
}
/*****************************************************************************
 * 函数名	      : wmmp_tcpip_tx_data_enabled_ind_proc
 * 作  者        : 
 * 功能描述      : 处理EVENT EAT_EVENT_TCPIP_TX_DATA_ENABLE_IND
 * 输入参数说明  : 无
 * 输出参数说明  : 无
 * 返回值说明    : 无
 *****************************************************************************/
static void  wmmp_tcpip_tx_data_enabled_ind_proc(EatEvent_st* event)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    WmmpContext_t    *mainContext_p = ptrToWmmpContext();
    WmmpDataInterface_t* wmmpDataResult_t = ptrToWmmpDataInterface();
    s32 bytesSent,alreadySent;
    char* buf_p = wmmpDataResult_t->dataBuffer_p;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    WMMP_DBG_TEMP("wmmp_tcpip_tx_data_enabled_ind_proc");

    /*可以发送了，继续发送*/
    simcom_task_sleep(50);   
    if( 1 == mainContext_p->dataBufferFlag && bytesSent > 0)
    {
        if(bytesSent>MAX_SEND_BSD_LEN)
        {    
            bytesSent = MAX_SEND_BSD_LEN;
        }
        alreadySent = wmmpDataResult_t->totalLen - wmmpDataResult_t->remLen;
    }

    WPI_send_net_data(event->data.tcpip_tx_data_enabled_ind.BearRef+1,event->data.tcpip_tx_data_enabled_ind.ConnRef,bytesSent,buf_p+alreadySent);
}
/*****************************************************************************
 * 函数名	      : wmmp_tcpip_rx_data_req_proc
 * 作  者        : 
 * 功能描述      : 处理EVENT EAT_EVENT_TCPIP_RX_DATA_REQ
 * 输入参数说明  : 无
 * 输出参数说明  : 无
 * 返回值说明    : 无
 *****************************************************************************/
static void  wmmp_tcpip_rx_data_req_proc(EatEvent_st* event)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    WPAEventData_t WPAEventData;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    WMMP_DBG_TEMP( "wmmp_tcpip_rx_data_req_proc,len %d",event->data.tcpip_rx_data_req.Size);

    memset(wmmprecvbuff,0x00,sizeof(wmmprecvbuff));

    memcpy(wmmprecvbuff,event->data.tcpip_rx_data_req.Data,event->data.tcpip_rx_data_req.Size);

    WMMP_DBG_TEMP("WmmpProcessSocNotifyInd  readLen %d ",event->data.tcpip_rx_data_req.Size);
    WPAEventData.WPASocketRecvEvent.SocketID=event->data.tcpip_rx_data_req.ConnRef;
    WPAEventData.WPASocketRecvEvent.data=wmmprecvbuff;	
    WPAEventData.WPASocketRecvEvent.size=event->data.tcpip_rx_data_req.Size;	

    WPA_ProcessEvent(WPA_SOCKET_RECV_EVENT, &WPAEventData);

    WPI_sendRxDataCnf(event->data.tcpip_rx_data_req.BearRef,event->data.tcpip_rx_data_req.ConnRef,event->data.tcpip_rx_data_req.Size);
}
/*****************************************************************************
 * 函数名	      : wmmp_login_for_wopen
 * 作  者        : 
 * 功能描述      : 当WOPEN=1时，调用此函数登录平台
 * 输入参数说明  : 无
 * 输出参数说明  : 无
 * 返回值说明    : 无
 *****************************************************************************/
void wmmp_login_for_wopen(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bool  rett = false;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    WMMP_DBG_TEMP("wmmp_login_for_wopen ");

    rett = WPA_Init();

    if(rett)
    {        
        WMMP_DBG_TEMP(" WPA_Init() ok");
        rett = WPA_LoginReq();
        if(rett)
        {       
            WMMP_DBG_TEMP(" WPA_LoginReq() ok");
        }
        else
        {
            WMMP_DBG_TEMP(" WPA_LoginReq() err");	

        }
    }
    else
    {
        WMMP_DBG_TEMP(" WPA_Init() err");		
    }

}
/*****************************************************************************
 * 函数名	      : wmmp_nw_attach_ind_proc
 * 作  者        : 
 * 功能描述      : 处理EVENT EAT_EVENT_TCPIP_RX_DATA_REQ
 * 输入参数说明  : 无
 * 输出参数说明  : 无
 * 返回值说明    : 无
 *****************************************************************************/
static void  wmmp_nw_attach_ind_proc(EatEvent_st* event)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    WPAEventData_t WPAEventData;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    WMMP_DBG_TEMP("wmmp_nw_attach_ind_proc,state %d",event->data.nwAttachInd.gprs_state);

    if (REG_STATE_SEARCHING == event->data.nwAttachInd.gprs_state  )
    {   
        if (WMMP_PON_REG_SEARCHING > WPA_Get_Power_On_Step())
        {
            if (1 == gwopenValue)
            {
                WPI_send_urc(WMMP_AT_EVENT_STR"%d,%s %s", WMMP_AET_LIB_STATE, "0013", "\"Waiting for network registration\"");
            }
        }
    }

    WMMP_DBG_TEMP("gLoginStart=%d ", gLoginStart);

    if(REG_STATE_REGISTERED == event->data.nwAttachInd.gprs_state)
    {
        gLoginStart=1;
    }
    /*gprs registed and not in login*/
    if ( REG_STATE_REGISTERED == event->data.nwAttachInd.gprs_state&&(WPA_Get_Power_On_Step()==WMMP_PON_SMS_BEGIN))
    {
        /*need auto login*/
        if (1 == gwopenValue)
        {
            wmmp_login_for_wopen(); /*暂关闭，以后根据情况打开*/
        }
    }

}
/*****************************************************************************
*	函数名	      : wmmp_uart_rtr_ind_hdlr
* 作  者        : 
*	功能描述      : 消息EAT_MSG_ID_UART_READY_TO_READ_IND处理
*	输入参数说明  : 无
* 输出参数说明  : 无
*	返回值说明    : 无
*****************************************************************************/
void wmmp_uart_rtr_ind_hdlr(void)
{
    simcom_m2m_data_len=eat_data_mode_read(g_simcom_m2m_at_buff,300*1024);
    
    WmmpProcessAtSendData(g_simcom_m2m_at_buff,simcom_m2m_data_len);
}


void app_main(void *data)
{
    EatEvent_st event;

    APP_InitRegions();//Init app RAM
#ifdef EAT_CORE_APP_INDEP
    __rt_lib_init(0xF0380000,0xF0400000); //Init C lib
#endif
    if(EAT_TRUE != eat_mem_init((void*)wmmp_core_mem_pool,WMMP_CORE_MEM_POOL_LEN))
    {
        eat_trace(" eat_mem_init Fail ! app exit!");
        return ;
    }
    eat_trace("app main entry1");
    wmmp_at_init();
    eat_trace("app main entry2");
    wmmp_init(); /*初始化*/

    eat_trace("app main entry3");
    while(EAT_TRUE)
    {
        eat_get_event(&event);
        switch(event.event)
        {
            case EAT_EVENT_TCPIP_STARTUP_CNF:
                {
                    wmmp_tcpip_start_cnf_proc(&event);
                }
                break;

            case EAT_EVENT_TCPIP_SHUTDOWN_CNF:
                {
                    wmmp_tcpip_shutdown_cnf_proc(&event);
                }
                break;

            case EAT_EVENT_TCPIP_SHUTDOWN_IND:
                {
                    wmmp_tcpip_shutdown_ind_proc(&event);
                }
                break;

            case EAT_EVENT_TCPIP_CLOSE_CNF:
                {
                    wmmp_tcpip_close_cnf_proc(&event);
                }
                break;

            case EAT_EVENT_TCPIP_ACTIVE_OPEN_CNF:
                {
                    wmmp_tcpip_active_open_cnf_proc(&event);
                }
                break;

            case EAT_EVENT_TCPIP_TX_DATA_CNF:
                {
                    wmmp_tcpip_tx_data_cnf_proc(&event);
                }
                break;

            case EAT_EVENT_TCPIP_TX_DATA_ENABLE_IND:
                {
                    wmmp_tcpip_tx_data_enabled_ind_proc(&event);
                }
                break;

            case EAT_EVENT_TCPIP_RX_DATA_REQ:
                {
                    wmmp_tcpip_rx_data_req_proc(&event);
                }
                break;

            case EAT_EVENT_NW_ATTACH_IND:
                {
                    wmmp_nw_attach_ind_proc(&event);
                }
                break;
            case EAT_EVENT_NEW_SMS_IND:
                {
                    EatSms_st* sms = &event.data.sms;
                    EatSms_st get_sms_body;
                    if(simcom_wmmp_check_sms(sms, &get_sms_body))
                    {
                        WPA_Decode(WMMP_TRANS_SMS,(char*)get_sms_body.data, get_sms_body.len);
                    }
                }
                break;
            case EAT_EVENT_TIMER:
                {
                    if(event.data.timer.timer_id == wmmp_get_lib_timer_id())
                    {
                        //wmmp core timer
                        WPA_ProcessEvent(WPA_TIMER_EVENT, NULL);
                    }
                }
                break;
            case EAT_EVENT_UART_READY_RD:
            {
                    eat_trace( "WMMP:MSG_ID_UART_READY_TO_READ_IND");
                    if( event.data.uart.uart == eat_at_port)
                    {
                        wmmp_uart_rtr_ind_hdlr();
                    }
                    break;			
            }

            default:
                break;
        }
    }

}


