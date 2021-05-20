/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                               *
 *---------------------------------------------------------------------
 * FileName      :   simcom_wmmp_hdlr_ex.c
 * version       :   0.10
 * Description   :   wmmp AT handler
 * Authors       :   Maobin  
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2013-01-11, maobin, create originally.
 *
 ********************************************************************/
#ifdef __SIMCOM_WMMP__
/********************************************************************
 * Include Files
 ********************************************************************/
//#include "simcom_at_config_implement.h"
#include "simcom_at_parser.h"
//#include "simcom_wmmp_at_cmd.def"

#include "simcom_wmmp_utility.h"
#include "simcom_wmmp_nvram_define.h"
#include "simcom_m2m_at_fun.h"

#include "wmmp_interface.h"
//#include "simcom_wmmp_trc.h"
#include "wmmp_api.h"
#include "wmmp_lib.h" //WmmpDataInterface_t
#include "eat_at.h"
//#include "nvram_enums.h"
//#include "nvram_struct.h"
//#include "nvram_user_defs.h"
//#include "nvram_data_items.h"
//#include "custom_nvram_editor_data_item.h"



/********************************************************************
 * Macros
 ********************************************************************/
#define WMMP_DBG_E(x,...)
#define WMMP_DBG(x,...)

//该宏可以简单的检查当前参数是否为最后一个参数，如果不是则打印ERROR并返回
#define RETURN_IF_PARAM_IS_NOT_END() \
    do\
{\
    if(!simcom_at_parser_is_end())\
    {\
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_PARAM_COUNT_BEYOND);\
        simcom_at_output_default_end();\
        return;\
    }\
}while(0)

/********************************************************************
 * Types
 ********************************************************************/
#define MAX_UART_DATA_LEN	256  
#define ULONG_MAX       0xFFFFFFFFL

/********************************************************************
 * Extern Variables (Extern /Global)
 ********************************************************************/

extern u8 currAppNum; //当前appopen 的序号，范围0～MAX_WMMP_APP_NUM-1
extern u32 appTransaction_Id[MAX_WMMP_APP_NUM]; //appopen 链接标识

extern u8 gwopenValue;
extern char g_commom_imsi_str[];
extern char g_commom_imei_str[];

extern u8 bhvMode;
extern u8  bhvValue[MAX_BHV_MODE];

extern Wmmp_atc_value_struct atcValueNv;

extern u32 Transaction_Id;

/********************************************************************
 * Local Variables:  STATIC
 ********************************************************************/

/********************************************************************
 * External Functions declaration
 ********************************************************************/

/********************************************************************
 * Local Function declaration
 ********************************************************************/

bool app_open(u32 index);

void simcom_output_symbol(void);
/********************************************************************
 * Local Function
 ********************************************************************/

bool simcom_m2m_at_send = false;/* 当前AT 命令格式是否处于用户透传状态*/
char  g_simcom_m2m_at_buff[300*1024] ;/* 当前AT 命令格式处于用户透传状态时的数据缓冲区*/
u16 simcom_m2m_data_len = 0;/* 当前AT 命令格式处于用户透传状态时的数据长度*/


bool simcom_m2m_bs_flag = false;
bool simcom_m2m_echo_flag = true;/*SIMCOM luyong 2012-07-20 Fix MKBug00013366 add */

SIMCOM_WMMP_AT_ENUM simcom_m2m_at_type = VG_AT_M2MC_BEGIN;//m2m AT type

char Data_type[2];//目的地址类型       T 表示终端序列号，E表示EC 业务码
char Data_destination[41];//目的地址

u16 simcom_m2m_at_len = 0;//TLV num
extern u8 m2mStatusParam;
extern ProtocolStatus_e gM2MStatus;
static u16 g_src_id; /*记录AT src id*/

/*****************************************************************************
 * 函数名: eat_at_mode_ok
 * 作  者:  
 * 功能描述: 打出默认格式的<CR><LF>OK<CR><LF>
 * 输入参数说明: 需要符合回调函数类型，所以加入此参数，但无需使用
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void eat_at_mode_ok(void* pPFAddData)
{
    simcom_at_output_default_end();
}

/*****************************************************************************
 * 函数名: eat_at_mode_err
 * 作  者:  DongBingquan
 * 功能描述: 打出默认格式的<CR><LF>ERROR<CR><LF>
 * 输入参数说明: 需要符合回调函数类型，所以加入此参数，但无需使用
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void eat_at_mode_err(void* pPFAddData)
{
    simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
    simcom_at_output_default_end();
}

bool sincom_wmmp_wopen(void)
{
    return gwopenValue;
}

/******** APPOPEN APPCLOSE APPSEND function begin ********/

/*-----------------------------------------------------
 * AT$M2MAPPOPEN=?
 *-----------------------------------------------------*/
void m2mappopen_test_hdlr(void *data)
{
    simcom_at_output("$M2MAPPOPEN: (0-%d)", MAX_WMMP_APP_NUM-1);
    simcom_at_output_default_end();
}

/*-----------------------------------------------------
 * AT$M2MAPPOPEN=n
 *-----------------------------------------------------*/
void m2mappopen_set_hdlr(void *data)
{
    SIMCOM_AT_PARSER_RET result = EAT_APR_OK;
    u32	app_open_index;
    bool wait=false;  /*是否需要等待异步返回的结果*/
    result = simcom_at_parser_get_u32( 0, MAX_WMMP_APP_NUM-1,&app_open_index );

    g_src_id=simcom_at_parser_get_src_id();

    if(result == EAT_APR_OK)
    {
        RETURN_IF_PARAM_IS_NOT_END();
        if( !app_open(app_open_index))
        {
            simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_UNKNOWN );
        }
        else
        {
            wait=true;
        }
    }else
    {
        WMMP_DBG_E(WMMP_APPOPEN_PARSING_ERROR);
    }

    if(!wait )   /*只有出错时才直接输出结果*/
    {
        simcom_at_output_default_end();
    }

}
/*****************************************************************************
 *	函数名	      : wmmpAppCreateConnectCallback
 * 作  者        : 
 *	功能描述      : APP建立连接的回调函数
 *	输入参数说明  : 无
 * 输出参数说明  : 无
 *	返回值说明    : 无
 *****************************************************************************/
void wmmpAppCreateConnectCallback(bool success,Simcom_wmmp_connRef connRef)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    WmmpDataInterface_t* wmmpDataResult_t = ptrToWmmpDataInterface();
    char  temp[256]={0};
    bool result=true;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    WMMP_DBG_TEMP("wmmpAppCreateConnectCallback,%d,%d",success,connRef);

    if(success) /*成功*/
    {	
        if(wmmpDataResult_t->dataBuffer_p==NULL)
        {
            //malloc mem for wmmp use
            wmmpDataResult_t->dataBuffer_p = (char*)WPI_Malloc(WMMP_MAX_NET_BUFFER_SIZE );
            if(wmmpDataResult_t->dataBuffer_p==NULL)
            {										
                WMMP_DBG_E(M2MAPPOPEN_MALLOC_FAIL);
                result=false;
            }else
            {
                currAppNum = (u8)connRef-1;
                appTransaction_Id[connRef-1] = WPI_rand();
                sprintf( temp,"$M2MAPPOPEN: %d\r\n\r\nOK",appTransaction_Id[connRef-1]);

                simcom_at_output_async(g_src_id,temp,strlen(temp),true);
            }
        }
    }
    else
    {
        result=false;
        WMMP_DBG_E(M2MAPPOPEN_WPA_APPOPENREQ_FAIL);
    }

    if(!result)
    {
        sprintf( temp,"\r\nERROR");  
        simcom_at_output_async(g_src_id,temp,strlen(temp),true);
    }

}
bool app_open(u32 index)
{
    bool result = true;
    StatusNumber_t *wmmpStatus_p = (StatusNumber_t *)WPA_GetWmmpStatus();
    WmmpDataInterface_t* wmmpDataResult_t = ptrToWmmpDataInterface();

    if((index >= MAX_WMMP_APP_NUM))
    {
        return false;
    }

    if(wmmpStatus_p->app_status[index]==APPLINK_OK)
    {        					
        simcom_at_output("$M2MAPPOPEN: %d Already Open",index);
    }
    else
    {
        if(wmmpStatus_p->net_status == LINK_FAILED)
        {
            simcom_at_output("$M2MAPPOPEN: NET STATUS %d", wmmpStatus_p->net_status);
            result = false;
        }else
        {
            WPA_AppOpenReq(index,wmmpAppCreateConnectCallback);
            result=true;/*异步，返回true,表示要等待异步消息*/
        }
    }
    return result;
}
/*-----------------------------------------------------
 * AT$M2MAPPOPEN=?
 *-----------------------------------------------------*/
void m2mappclose_test_hdlr(void *data)
{
    simcom_at_output("$M2MAPPCLOSE: (0-%d)", MAX_WMMP_APP_NUM-1);
    simcom_at_output_default_end();
}

bool app_close(u32 index)
{
    s32 loopi;
    WmmpContext_t  *mainContext_p = ptrToWmmpContext();
    StatusNumber_t *wmmpStatus_p = WPA_GetWmmpStatus();
    WmmpDataInterface_t* wmmpDataResult_t = ptrToWmmpDataInterface();
    bool result = true;

    if((index >= MAX_WMMP_APP_NUM))
    {
        return false;
    }

    if(wmmpStatus_p->net_status==LINK_FAILED)
    {
        simcom_at_output("$M2MAPPCLOSE: NET STATUS %d", wmmpStatus_p->net_status);
        result = false;
    }
    else
    {
        if(wmmpStatus_p->app_status[index]==APPLINK_FAILED)
        {
            simcom_at_output("$M2MAPPCLOSE: %d  Already Closed",index);
        }
        else 
        {
            result = WPA_AppCloseReq(index);									
            if (result)
            {				                   		
                mainContext_p->wmmpOpenSkt[index] = (0xFFFFFFFFL);
                for(loopi = 0;loopi < MAX_WMMP_APP_NUM;loopi++)
                {
                    if (mainContext_p->wmmpOpenSkt[loopi] !=(0xFFFFFFFFL))
                    {
                        mainContext_p->dataBufferFlag = 1;
                        break;
                    }
                }

                if((wmmpDataResult_t->dataBuffer_p!=NULL) && (mainContext_p->dataBufferFlag == 0))
                {
                    mainContext_p->dataBufferFlag = 0;
                    WPI_Free(wmmpDataResult_t->dataBuffer_p);
                    wmmpDataResult_t->dataBuffer_p = NULL;
                }
                simcom_at_output("OK");
                simcom_at_output("%s8,%d,0,\"%s\"",WMMP_AT_EVENT_STR, index,"ok");
            }else
            {
                WMMP_DBG_E(M2MAPPOPEN_WPA_APPCLOSEREQ_FAIL);
                result = false;
            }

        }
    }

    return result;
} 
void m2mappclose_set_hdlr(void *data)
{
    SIMCOM_AT_PARSER_RET result = EAT_APR_OK;
    u32 app_close_index;
    result = simcom_at_parser_get_u32( 0, MAX_WMMP_APP_NUM-1,&app_close_index );

    if( result == EAT_APR_OK)
    {
        RETURN_IF_PARAM_IS_NOT_END();
        if( !app_close(app_close_index))
        {
            simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_UNKNOWN );
        }
    }

    simcom_at_output_default_end();
}

void m2mappsend_test_hdlr(void* data)
{
    simcom_at_output("$M2MAPPSEND: (0-%d)", MAX_WMMP_APP_NUM-1);
    simcom_at_output_default_end();
}
bool app_send(u32 index)
{
    s32 loopi;
    StatusNumber_t *wmmpStatus_p = WPA_GetWmmpStatus();
    bool result = true;

    if((index >= MAX_WMMP_APP_NUM))
    {
        return false;
    }

    if(wmmpStatus_p->app_status==APPLINK_FAILED)
    {
        simcom_at_output("$M2MAPPSEND: NET STATUS %d", wmmpStatus_p->app_status);
        result = false;
    }
    else if(wmmpStatus_p->app_status[index]==APPLINK_FAILED)
    {								
        simcom_at_output("$M2MAPPSEND: %d not Open",index);
        result = false;
    }
    else
    {
        simcom_m2m_at_type = VG_AT_M2MC_MAPPSEND;
        simcom_output_symbol();
        currAppNum = index;//当前是哪个一个链路
        eat_ds_enter_data_mode();

        result = true;
    }

    return result;
}

void m2mappsend_set_hdlr(void* data)
{
    SIMCOM_AT_PARSER_RET result = EAT_APR_OK;
    u32 app_send_index;
    result = simcom_at_parser_get_u32( 0, MAX_WMMP_APP_NUM-1,&app_send_index );

    if( result == EAT_APR_OK)
    {
        RETURN_IF_PARAM_IS_NOT_END();
        if(app_send(app_send_index))
        {
            /*Entry data mode,so don't output any result words*/
        }else
        {
            simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_UNKNOWN );
            simcom_at_output_default_end();
        }
    }else
    {
        simcom_at_output_default_end();
    }
}

void m2mappstatus_test_hdlr(void* data)
{
    simcom_at_output("$M2MAPPSTATUS: (0-%d)", MAX_WMMP_APP_NUM-1);
    simcom_at_output_default_end();
}

void app_status(u32 index)
{
    char str[32] = {0};
    StatusNumber_t* wmmpStatus_p = (StatusNumber_t *) WPA_GetWmmpStatus();
    PrintCurrentAppStatus(wmmpStatus_p->app_status[index],str);
    simcom_at_output("$M2MAPPSTATUS: %d,%04X,%s", index, wmmpStatus_p->app_status[index],str); 
}

void m2mstatus_set_hdlr(void* data)
{
    SIMCOM_AT_PARSER_RET result = EAT_APR_OK;
    u32 app_status_index;
    result = simcom_at_parser_get_u32( 0, MAX_WMMP_APP_NUM-1,&app_status_index );

    if( result == EAT_APR_OK)
    {
        app_status(app_status_index);
    }
    simcom_at_output_default_end();
}

/*M2MCGPADDR 查询IP地址*/
void  m2mcgpaddr_exe_hdlr(void* data)
{
    WmmpNetWorkContext_t *networkContext_p;
    AppWmmpContext_t *appWmmpContext_p = ptrToAppWmmpContext();   
    u32 i;

    for(i=0; i< MAX_SIMCOM_WMMP_CONTEXT_NUM; i++)
    {
        networkContext_p  = &appWmmpContext_p->networkContext[i];
        simcom_at_output( "IP:%d.%d.%d.%d",networkContext_p->ip_addr[0]  ,
                networkContext_p->ip_addr[1],
                networkContext_p->ip_addr[2],
                networkContext_p->ip_addr[3]);
        simcom_at_output_cr_lf();
    }

    simcom_at_output_default_end();
}

/*AT$M2MAPPBROPEN=?*/
void m2mappbropen_test_hdlr(void* data)
{
    simcom_at_output("$M2MAPPBROPEN: (0-%d)", MAX_WMMP_APP_NUM-1);
    simcom_at_output_default_end();
}

bool app_bropen(u32 index)
{
    bool result = WPA_AppBrOpenReq();
    if (result)
    {	
        simcom_at_output( "$M2MAPPBROPEN: %d",Transaction_Id);
    }
    return result;
}

/* AT$M2MAPPBROPEN=x BROPEN 激活PDP上下文，参数没有使用，
 * 只要是0～MAX_WMMP_APP_NUM-1就可以
 * M2MAPPOPEN 使用之前需要使用BROPEN激活PDP上下文*/
void m2mappbropen_set_hdlr(void* data)
{
    SIMCOM_AT_PARSER_RET result = EAT_APR_OK;
    u32  app_bropen_index;
    result = simcom_at_parser_get_u32( 0, MAX_WMMP_APP_NUM-1,&  app_bropen_index);
    if(result == EAT_APR_OK)
    {
        RETURN_IF_PARAM_IS_NOT_END();
        if( !app_bropen(app_bropen_index))
        {
            simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_UNKNOWN );
        }
    }else
    {
        WMMP_DBG_E(WMMP_APPBROPEN_PARSING_ERROR);
    }

    simcom_at_output_default_end();
}

/*AT$M2MAPPBRCLOSE=?*/
void m2mappbrclose_test_hdlr(void* data)
{
    simcom_at_output("$M2MAPPBRCLOSE: (0-%d)", MAX_WMMP_APP_NUM-1);
    simcom_at_output_default_end();
}

bool app_brclose(u32 index)
{
    bool result = WPA_AppBrCloseReq();
    if (result)
    {   
        simcom_at_output( "$M2MAPPBRCLOSE: %d",Transaction_Id);
    }
    return result;
}
/*AT$M2MAPPBRCLOSE=x BRCLOSE 去激活PDP上下文，参数没有使用*/
void m2mappbrclose_set_hdlr(void* data)
{
    SIMCOM_AT_PARSER_RET result = EAT_APR_OK;
    u32  app_brclose_index;
    result = simcom_at_parser_get_u32( 0, MAX_WMMP_APP_NUM-1,&  app_brclose_index);
    if(result == EAT_APR_OK)
    {
        RETURN_IF_PARAM_IS_NOT_END();
        if( !app_brclose(app_brclose_index))
        {
            simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_UNKNOWN );
        }
    }

    simcom_at_output_default_end();
}

void m2mappbrstatus_test_hdlr(void* data)
{
    simcom_at_output("$M2MAPPBRSTATUS: (0-%d)", MAX_WMMP_APP_NUM-1);
    simcom_at_output_default_end();
}

void app_brstatus(u32 index)
{
    char str[32] = {0};
    StatusNumber_t* wmmpStatus_p = (StatusNumber_t *) WPA_GetWmmpStatus();
    PrintCurrentNetStatus(wmmpStatus_p->net_status,str);
    simcom_at_output("$M2MAPPBRSTATUS: %d,4,%d,%s",index, wmmpStatus_p->net_status,str); 
}

/*AT$M2MAPPBRSTATUS=x*/
void m2mappbrstatus_set_hdlr(void* data)
{
    SIMCOM_AT_PARSER_RET result = EAT_APR_OK;
    u32  app_br_index;
    result = simcom_at_parser_get_u32( 0, MAX_WMMP_APP_NUM-1,&  app_br_index);
    if(result == EAT_APR_OK)
    {
        RETURN_IF_PARAM_IS_NOT_END();
        app_brstatus(app_br_index);
    }
    simcom_at_output_default_end();
}

/******** APPOPEN APPCLOSE APPSEND function end********/


/*SIMCOM Laiwenjie 2013-01-25 Fix MKBug00015013 for at command BEGIN*/

/*****************************************************************************
 * 函数名: WmmpSend_Echo_Bs_flag
 * 作  者:  Laiwenjie
 * 功能描述: 根据bhv的值设置标志位，该接口中设置的标志位好像没有使用到
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
bool WmmpSend_Echo_Bs_flag(void)                                
{

    if((bhvValue[0]==1)||(bhvValue[3]==1))
    {
        //RMMI_PTR->rsp_mode.echo_flag=0xff;
        simcom_m2m_bs_flag=true;
        simcom_m2m_echo_flag=true;/*SIMCOM luyong 2012-07-20 Fix MKBug00013366 add */
        return true;
    }
    else
    {
        //RMMI_PTR->rsp_mode.echo_flag=0;
        simcom_m2m_bs_flag=false;
        simcom_m2m_echo_flag=false;/*SIMCOM luyong 2012-07-20 Fix MKBug00013366 add */
        return false;
    }

}

/*****************************************************************************
 * 函数名: wmmpProcessTestCaseReq
 * 作  者:  Laiwenjie
 * 功能描述: 根据不同的status进行对应的设置，用于测试
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void wmmpProcessTestCaseReq(u8 Status)
{
    CommandInfo_t CommandInfo;
    u16 *tags=NULL;
    u16 *tags2 = NULL;
    u8 i;	

    switch (Status)
    {
        case 0:
            {

                /***************************************************************************************
                 *			test ID 396  ---多包处理_MIMO_终端分包请求CONFIG_REQ_UDP
                 *
                 *  中移动研究院m2m platform: 13917999485 -- imsi = 460007914127292, 
                 *    imei = 351692032182973,

                 *   cbc@20100426:终端多包发起请求,平台多包应答

                 **************************************************************************************/	
                CommandInfo.CommandID = CONFIG_REQ_COMMAND;
                CommandInfo.SecuritySet = 0;
                //KiAllocZeroMemory(15*sizeof(u16), (void**) &tags);
                tags = (u16*)WPI_Malloc(15*sizeof(u16));
                memset(tags,0,15*sizeof(u16));

                for(i=0;i<15;i++)
                {
                    tags[i]=0x0027;
                }
                CommandInfo.CommandExInfo.ConfigReqInfo.TLVTags=tags;
                CommandInfo.CommandExInfo.ConfigReqInfo.TagNum=15;
                CommandInfo.TransType = WMMP_TRANS_DEFAULT;	
                SaveToCommandBuffer(&CommandInfo);		


            }
            break;

        case 1:
            {

                /***************************************************************************************
                 *			test ID 388 :  多包处理_MIMO_终端发起CONFIG_TRAP_UDP
                 *
                 *  中移动研究院m2m platform: 13917999485 -- imsi = 460007914127292, 
                 *    imei = 351692032182973,
                 *                  cbc@20100526:    终端多个请求，平台单个应答

                 **************************************************************************************/	
                CommandInfo.CommandID = CONFIG_TRAP_COMMAND;
                CommandInfo.SecuritySet = 0;
                //KiAllocZeroMemory(10*sizeof(u16), (void**) &tags);
                tags = (u16*)WPI_Malloc(10*sizeof(u16));
                memset(tags,0,10*sizeof(u16));
                for(i=0;i<10;i++)
                {
                    tags[i]=0x3002;
                }
                CommandInfo.CommandExInfo.ConfigTrapInfo.TLVTags=tags;
                CommandInfo.CommandExInfo.ConfigTrapInfo.TagNum=10;
                CommandInfo.TransType = WMMP_TRANS_DEFAULT;	
                SaveToCommandBuffer(&CommandInfo);						
            }
            break;

        case 2:
            {

                /***************************************************************************************
                 *			test ID 544  :安全_变更密码密钥_UDP(密文)
                 *
                 *  中移动研究院m2m platform: 13917999485 -- imsi = 460007914127292, 
                 *    imei = 351692032182973,
                 *
                 *  CBC@20100427:M2M终端请求变更接入密码和基础密钥_密文模式
                 *  

                 **************************************************************************************/	
                CommandInfo.CommandID = SECURITY_CONFIG_COMMAND;
                CommandInfo.SecuritySet = 0;	
                CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */			
                CommandInfo.TransType = WMMP_TRANS_DEFAULT;	
                //KiAllocZeroMemory(6*sizeof(u16), (void**) &tags);
                tags = (u16*)WPI_Malloc(6*sizeof(u16));
                memset(tags,0,6*sizeof(u16));
                tags[0]=0xE038;
                tags[1]=0xE036;
                tags[2]=0xE02A;
                tags[3]=0xE028;
                tags[4]=0xE027;
                tags[5]=0xE025;
                CommandInfo.CommandExInfo.SecurityInfo.operation=6;/*请求变更安全参数密文操作数*/
                CommandInfo.CommandExInfo.SecurityInfo.tags=tags;
                CommandInfo.CommandExInfo.SecurityInfo.tagnum=6;//10;
                WMMP_DBG_TEMP("at_handle_m2m_test:SECURITY_CONFIG_COMMAND\n");
                SaveToCommandBuffer(&CommandInfo);				

            }
            break;

        case 3:
            {

                /***************************************************************************************
                 *			test ID 544  :安全_变更密码密钥_UDP(明文)
                 *
                 *  中移动研究院m2m platform: 13917999485 -- imsi = 460007914127292, 
                 *    imei = 351692032182973,
                 *
                 *  CBC@20100427:M2M终端请求变更接入密码和基础密钥_明文模式
                 *  

                 **************************************************************************************/	
                CommandInfo.CommandID = SECURITY_CONFIG_COMMAND;
                CommandInfo.SecuritySet = 0;	
                CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */			
                CommandInfo.TransType = WMMP_TRANS_DEFAULT;	
                //KiAllocZeroMemory(6*sizeof(u16), (void**) &tags);
                tags = (u16*)WPI_Malloc(6*sizeof(u16));
                memset(tags,0,6*sizeof(u16));
                tags[0]=0xE038;
                tags[1]=0xE036;
                tags[2]=0xE02A;
                tags[3]=0xE028;
                tags[4]=0xE027;
                tags[5]=0xE025;
                CommandInfo.CommandExInfo.SecurityInfo.operation=5;/* 请求变更安全参数明文操作数*/
                CommandInfo.CommandExInfo.SecurityInfo.tags=tags;
                CommandInfo.CommandExInfo.SecurityInfo.tagnum=6;//10;
                WMMP_DBG_TEMP("at_handle_m2m_test:SECURITY_CONFIG_COMMAND\n");
                SaveToCommandBuffer(&CommandInfo);				

            }
            break;

        case 4:
            {

                /***************************************************************************************
                 *			终端上报安全参数_接入密码和基础密钥_密文模式_UDP长连接
                 *
                 *     CBC@20100427             
                 *

                 **************************************************************************************/	
                CommandInfo.CommandID = SECURITY_CONFIG_COMMAND;
                CommandInfo.SecuritySet = 0;	
                CommandInfo.TransType = WMMP_TRANS_DEFAULT;	
                //KiAllocZeroMemory(6*sizeof(u16), (void**) &tags);
                tags = (u16*)WPI_Malloc(6*sizeof(u16));
                memset(tags,0,6*sizeof(u16));

                tags[5]=0xE038;
                tags[4]=0xE036;
                tags[3]=0xE02A;
                tags[2]=0xE028;
                tags[1]=0xE027;
                tags[0]=0xE025;

                CommandInfo.CommandExInfo.SecurityInfo.operation=4;/* 终端上报安全参数*/
                CommandInfo.CommandExInfo.SecurityInfo.tags=tags;
                CommandInfo.CommandExInfo.SecurityInfo.tagnum=6;//10;
                WMMP_DBG_TEMP("at_handle_m2m_test:SECURITY_CONFIG_COMMAND\n");
                SaveToCommandBuffer(&CommandInfo);					
            }
            break;

        case 7:
            {

                /***************************************************************************************
                 *			终端上报安全参数_接入密码和基础密钥_明文模式_UDP长连接
                 *
                 *   需要上报 0xe026 = MD5(0XE025+0XE027),0XE029=MD5(0XE028+0XE02A),0XE03A=
                 MD5(0XE036+0X38)
                 *   参考协议 P92, P119,  SECURITY SET -- OPERATION= 04
                 *
                 *     CBC@20100519            
                 *

                 **************************************************************************************/	
                CommandInfo.CommandID = SECURITY_CONFIG_COMMAND;
                CommandInfo.SecuritySet = 0;	
                CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */			
                CommandInfo.TransType = WMMP_TRANS_DEFAULT;	
                //KiAllocZeroMemory(6*sizeof(u16), (void**) &tags);
                tags = (u16*)WPI_Malloc(3*sizeof(u16));
                memset(tags,0,3*sizeof(u16));			

                tags[2]=0xE03a;
                tags[1]=0xE029;
                tags[0]=0xE026;

                CommandInfo.CommandExInfo.SecurityInfo.operation=4;/* 终端明文上报上报安全参数*/
                CommandInfo.CommandExInfo.SecurityInfo.tags=tags;
                CommandInfo.CommandExInfo.SecurityInfo.tagnum=3;
                WMMP_DBG_TEMP("at_handle_m2m_test:SECURITY_CONFIG_COMMAND\n");
                SaveToCommandBuffer(&CommandInfo);					
            }
            break;

        case 8:
            {

                /***************************************************************************************
                 *			终端上报SIM 卡PIN1 安全参数_明文模式_UDP长连接
                 *
                 *   需要上报   0XE002(MD5(终端序列好+IMEI+IMSI+PIN1)),OPERATION = 4
                 *   参考协议 P92, P116,  SECURITY SET -- OPERATION= 04
                 *
                 *     CBC@20100519            
                 *

                 **************************************************************************************/		
                CommandInfo.CommandID = SECURITY_CONFIG_COMMAND;
                CommandInfo.SecuritySet = 0;	
                CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */			
                CommandInfo.TransType = WMMP_TRANS_DEFAULT;	
                //KiAllocZeroMemory(6*sizeof(u16), (void**) &tags);
                tags = (u16*)WPI_Malloc(1*sizeof(u16));
                memset(tags,0,1*sizeof(u16));				

                tags[0]=0XE002;

                CommandInfo.CommandExInfo.SecurityInfo.operation=4;/*终端明文上报上报安全参数*/
                CommandInfo.CommandExInfo.SecurityInfo.tags=tags;
                CommandInfo.CommandExInfo.SecurityInfo.tagnum=1;
                WMMP_DBG_TEMP("at_handle_m2m_test:SECURITY_CONFIG_COMMAND\n");
                SaveToCommandBuffer(&CommandInfo);		
            }
            break;

        case 9:
            {

                /***************************************************************************************
                 *			终端上报SIM 卡PIN1 安全参数_密文模式_UDP长连接
                 *
                 *   需要上报   PIN1  : 0XE001,pin 有效期,TION = 4
                 *   参考协议 P92, P116,  SECURITY SET -- OPERATION= 04
                 *
                 *     CBC@20100519            
                 *

                 **************************************************************************************/
                CommandInfo.CommandID = SECURITY_CONFIG_COMMAND;
                CommandInfo.SecuritySet = 0;	
                CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */				
                CommandInfo.TransType = WMMP_TRANS_DEFAULT;	
                //KiAllocZeroMemory(6*sizeof(u16), (void**) &tags);
                tags = (u16*)WPI_Malloc(2*sizeof(u16));
                memset(tags,0,2*sizeof(u16));				

                tags[0]=0XE001;
                tags[1]=0XE003;

                CommandInfo.CommandExInfo.SecurityInfo.operation=4;
                CommandInfo.CommandExInfo.SecurityInfo.tags=tags;
                CommandInfo.CommandExInfo.SecurityInfo.tagnum=2;
                WMMP_DBG_TEMP("at_handle_m2m_test:SECURITY_CONFIG_COMMAND\n");
                SaveToCommandBuffer(&CommandInfo);		
            }
            break;

        case 5:
            {

                /***************************************************************************************
                 *  终端请求启用各类安全参数_接入密码和基础密钥_密文模式_UDP长连接
                 *
                 *     CBC@20100427             
                 *

                 **************************************************************************************/
                CommandInfo.CommandID = SECURITY_CONFIG_COMMAND;
                CommandInfo.SecuritySet = 0;	
                CommandInfo.SecuritySet |= (1<<5);/* 需要摘要 */	
                CommandInfo.TransType = WMMP_TRANS_DEFAULT;	

                //tags = (u16*)WPI_Malloc(6*sizeof(u16));
                //memset(tags,0,6*sizeof(u16));			

                CommandInfo.CommandExInfo.SecurityInfo.operation=7;/* 终端上报安全参数*/
                CommandInfo.CommandExInfo.SecurityInfo.tags=tags;
                CommandInfo.CommandExInfo.SecurityInfo.tagnum=0;//10;
                WMMP_DBG_TEMP("at_handle_m2m_test:SECURITY_CONFIG_COMMAND\n");
                SaveToCommandBuffer(&CommandInfo);					
            }
            break;

        case 6:
            {
                //wmmpSecurityEnableCpin("1234");
            }
            break;



        case 15:
            {
                /************************************************************************
                 *      cbc@20100424:    本地参数配置
                 *
                 *      终端登录,本地配置参数生效后用 用config TRAP 和0X4011 上报通知,
                 *      该命令和 AT?M2MCFG=N ， 配合使用,先更改参数后上报通知
                 *************************************************************************/

                /* add by cbc@20100424: 本地参数配置上报通知标识,M2M终端向平台上报begin*/
                WPA_LocalCfgReq(NULL,0);
                /* add by cbc@20100424: 本地参数配置上报通知标识,M2M终端向平台上报end*/

            }
            break;

        case 16:
            {
                /************************************************************************			*      cbc@20100424:    本地参数配置
                 *
                 *   终端在未登录平台前先修改配置的参数, 之后一登录就上报本地参数
                 *  修改更新通知给服务器,向M2M平台登录后，用CONFIG_TRAP和TLV 0x4011
                 *	上报其配置参数
                 *		该命令和 AT%M2MCFG=N ， 配合使用,先更改参数后后重起

                 ******************************************************************************************/

                /* add by cbc@20100424: 本地参数配置上报通知标识,M2M终端向平台上报begin*/
                wmmpLocalCfgNotify();
                //WPA_LocalCfgReq(NULL,0);
                /* add by cbc@20100424: 本地参数配置上报通知标识,M2M终端向平台上报end*/

            }
            break;

        case 17:
            {
                /************************************************************************
                 *      cbc@20100424:    本地参数配置
                 *
                 *      登录后,如果参数生效需要重启终端，则先用LOGOUT.LOGOUTREASON=3通知M2M
                 *	平台需要应用本地配置参数退出登录；
                 *	待新参数生效，重新向M2M平台登录后，用CONFIG_TRAP和TLV 0x4011
                 *	上报其配置参数，平台记录终端参数，流程结束。
                 *
                 *		该命令和 AT%M2MCFG=N ， 配合使用,先更改参数后后重起
                 *************************************************************************/

                /* add by cbc@20100424: 本地参数配置上报通知标识,M2M终端向平台上报begin*/
                wmmpLocalCfgReboot(3);
                /* add by cbc@20100424: 本地参数配置上报通知标识,M2M终端向平台上报end*/

            }
            break;

        case 18:
            {
                /* *******************************************************************
                 *        cbc@20100424:  终端异常告警TRAP 上报
                 *
                 *       0x300b:   0-- 正常， 1 -- 告警
                 *       0x300c:  1--通讯告警,  2 -- 硬件告警,3 -- 软件告警
                 *       0x300d:  1,2,3,
                 ************************************************************************/
                //void wmmpTrapAbortWarn(u8 AlarmStatus,u8	AlarmType,u16	AlarmCode)
                wmmpTrapAbortWarn(1,2,3);
            }
            break;

        case 19:
            {
                /* *******************************************************************
                 *        cbc@20100424:  终端异常告警TRAP 上报
                 *
                 *       0x300b:   0-- 正常， 1 -- 告警
                 *       0x300c:  1--通讯告警,  2 -- 硬件告警,3 -- 软件告警
                 *       0x300d:  1,2,3,
                 ************************************************************************/
                //void wmmpTrapAbortWarn(u8 AlarmStatus,u8	AlarmType,u16	AlarmCode)
                wmmpTrapAbortWarn(0,2,3);
            }
            break;


        default:
            break;
    }
}

/*****************************************************************************
 * 函数名: wmmp_M2MTLV_set_hdlr
 * 作  者:  Laiwenjie
 * 功能描述: AT$M2MTLV=* 
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2mtlv_set_hdlr(void *pPfData)
{
    //WMMP_DBG_TEMP("vgWOPEN gwopenValue %d ",gwopenValue);
    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }
}

/*****************************************************************************
 * 函数名: wmmp_M2MTEST_set_hdlr
 * 作  者:  Laiwenjie
 * 功能描述: AT$M2MTEST=*
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2mtest_set_hdlr(void *pPfData)
{
    u32              Status;

    char buffer[MAX_UART_DATA_LEN];
    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    if(EAT_APR_OK == simcom_at_parser_get_u32(0, ULONG_MAX, &Status)) 
    {
        WMMP_DBG(WMMP_M2MTEST_STATUS,Status);
        wmmpProcessTestCaseReq((u8)Status);
    }

    simcom_at_output_cr_lf();
    simcom_at_output("+M2MTEST:");
    simcom_at_output_default_end();

}

/*****************************************************************************
 * 函数名: wmmp_M2MPARACFG_test_hdlr
 * 作  者:  Laiwenjie
 * 功能描述: AT$M2MPARACFG=?
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2mparacfg_test_hdlr(void *pPfData)
{

    WMMP_DBG(WMMP_WOPEN_VALUE,gwopenValue);
    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    simcom_at_output_cr_lf();
    simcom_at_output("+M2MPARACFG:(1,M2MAPN)");

    simcom_at_output_cr_lf();
    simcom_at_output("+M2MPARACFG:(2,M2MSMSpecialNum)");

    simcom_at_output_cr_lf();
    simcom_at_output("+M2MPARACFG:(3,TerminalSeriesNum)");

    simcom_at_output_cr_lf();
    simcom_at_output("+M2MPARACFG:(4,uplinkPassword)");

    simcom_at_output_cr_lf();
    simcom_at_output("+M2MPARACFG:(5,downlinkPassword)");

    simcom_at_output_cr_lf();
    simcom_at_output("+M2MPARACFG:(6,publickey)");

    simcom_at_output_cr_lf();
    simcom_at_output("+M2MPARACFG:(7,BindIMSIOrig)");

    simcom_at_output_cr_lf();
    simcom_at_output("+M2MPARACFG:(8,IP addr,IP port)");

    simcom_at_output_cr_lf();
    simcom_at_output("+M2MPARACFG:(10,imei,imsi)");

    simcom_at_output_cr_lf();
    simcom_at_output("+M2MPARACFG:(13,save all para)");

    simcom_at_output_cr_lf();
    simcom_at_output("+M2MPARACFG:(15,close M2M)");

    simcom_at_output_cr_lf();
    simcom_at_output("+M2MPARACFG:(16,save all para)");

    simcom_at_output_cr_lf();
    simcom_at_output("+M2MPARACFG:(17,save all para)");

    simcom_at_output_cr_lf();
    simcom_at_output("+M2MPARACFG:(18,UnregTerminalNum)");

    simcom_at_output_cr_lf();
    simcom_at_output("+M2MPARACFG:(19,publicKeyExpDate)");

    simcom_at_output_default_end();

}

/*****************************************************************************
 * 函数名: wmmp_M2MPARACFG_read_hdlr
 * 作  者:  Laiwenjie
 * 功能描述: AT$M2MPARACFG?
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2mparacfg_read_hdlr(void *pPfData)
{

    WmmpContext_t    *mainContext_p =  ptrToWmmpContext();
    WMMP_DBG(WMMP_WOPEN_VALUE,gwopenValue);
    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }
    simcom_at_output_cr_lf();
    simcom_at_output("+M2MPARACFG:");
    simcom_at_output_cr_lf();
    simcom_at_output("APN:%s",(char *)mainContext_p->ConfigContext.M2MAPN);

    simcom_at_output_cr_lf();
    simcom_at_output("SMS center:%s",(char *)mainContext_p->ConfigContext.M2MSMSpecialNum);

    simcom_at_output_cr_lf();
    simcom_at_output("register terminal no:%s",(char *)mainContext_p->StatusContext.TerminalSeriesNum);

    simcom_at_output_cr_lf();
    simcom_at_output("register imsi:%s",(char *)mainContext_p->SecurityContext.BindIMSIOrig);


    simcom_at_output_cr_lf();
    simcom_at_output("IP port:%x %x",mainContext_p->ConfigContext.M2MIPAddr,mainContext_p->ConfigContext.M2MPort);

    simcom_at_output_cr_lf();
    simcom_at_output("uplink pwd:%s",(char *)mainContext_p->SecurityContext.uplinkPassword);


    simcom_at_output_cr_lf();
    simcom_at_output("down pwd:%s", (char *)mainContext_p->SecurityContext.downlinkPassword);

    simcom_at_output_cr_lf();
    simcom_at_output("publi key:%s", (char *)mainContext_p->SecurityContext.publickey);

    simcom_at_output_cr_lf();
    simcom_at_output("IMEI:%s", (char *)g_commom_imei_str);

    simcom_at_output_cr_lf();
    simcom_at_output("IMSI:%s", (char *)g_commom_imsi_str);

    simcom_at_output_default_end();

}

/*****************************************************************************
 * 函数名: wmmp_M2MPARACFG_set_hdlr
 * 作  者:  Laiwenjie
 * 功能描述: AT$M2MPARACFG=*
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/

/**********************************************************************************
 *                       测试使用,强制修改模块M2M 登录的一些初始化参数值
 *
 *    1.     APN
 *    2.     m2m 短信中
 *    3.     终端序列号
 *    4.     上行密码
 *    5.     下行密码
 *    6.    基础密钥
 *    7.    m2m IMSI
 *    8.    m2m IP add,ip port
 *    9.    WMMP3.0 存储参数文件 "/flash/dm/wmmp.dat" 清空为初始化值
 *   
 *********************************************************************************/
void m2mparacfg_set_hdlr(void *pPfData)
{
    char	IDParam[100]={0};
    char strtemp[30]={0};
    char ipadd[100]={0};
    char ipport[100]={0};

    u32	Status;

    WmmpContext_t    *mainContext_p =  ptrToWmmpContext();
    /*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_PRIVATE_APN__
    AppWmmpContext_t     *appWmmpContext_p = (AppWmmpContext_t*)ptrToAppWmmpContext();
    WmmpNetWorkContext_t    *networkContext_p;
#endif
    /*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/

    WMMP_DBG(WMMP_WOPEN_VALUE,gwopenValue);
    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    memset(IDParam,0,sizeof(IDParam));

    if(EAT_APR_OK == simcom_at_parser_get_u32(0, ULONG_MAX, &Status)) 
    {
        WMMP_DBG(WMMP_M2MPARACFG_SET_POS, Status);

        switch(Status)                  
        {
            case 1:
                {       /*   1.     APN   */
                    if (EAT_APR_OK == simcom_at_parser_get_str(0, 19, IDParam ))
                    {
                        /*wmmpPreCreateSocketConnect will set apn to account id*/
                        strcpy((char *)mainContext_p->ConfigContext.M2MAPN , (char *)IDParam);
                        /*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_PRIVATE_APN__
                        if(wmmp_get_apn_type()==WMMP_APN_PRIVATE)
                        {
                            networkContext_p = &(appWmmpContext_p->networkContext[2]);
                            memcpy((void*)(networkContext_p->p_account->ps_account_info.apn),
                            (void*)mainContext_p->ConfigContext.M2MAPN, MAX_GPRS_APN_LEN);
                            networkContext_p->p_account->ps_account_info.apn_length=strlen((char*)(networkContext_p->p_account->ps_account_info.apn));

                            wmmpProcessSave(NVRAM_EF_WMMP_ACCOUNT_LID,(void *)networkContext_p->p_account,sizeof(Wmmp_mmi_abm_account_info_struct));
                        }

#endif
                        /*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/
                    }
                    else
                    {
                        WMMP_DBG_E(WMMP_SET_APN_ERROR);
                    }
                    break;
                }

            case 2:
                {   /*   2.     m2m 短信中心 */
                    if (EAT_APR_OK == simcom_at_parser_get_str(0, 19, IDParam ))
                    {
                        strcpy((char *)mainContext_p->ConfigContext.M2MSMSpecialNum , IDParam);
                    }
                    else
                    {
                        WMMP_DBG_E(WMMP_SET_SMS_SPECIALNUM_ERROR);
                    }
                    break;                      
                }

            case 3:
                {
                    /*   3.     已注册终端序列号 */
                    if (EAT_APR_OK == simcom_at_parser_get_str(0, 16, IDParam ))
                    {
                        strcpy((char *)mainContext_p->StatusContext.TerminalSeriesNum , IDParam);
                    }
                    else
                    {
                        WMMP_DBG_E(WMMP_SET_REGISTERED_TERMINAL_SERIES_NUM_ERROR);
                    }
                    break;
                }

            case 4:
                {   /*   4.     上行密码  */                        
                    if (EAT_APR_OK == simcom_at_parser_get_str(0, 8, IDParam ))
                    {
                        strcpy((char *)mainContext_p->SecurityContext.uplinkPassword , IDParam);
                    }
                    else
                    {
                        WMMP_DBG_E(WMMP_SET_UPLINK_PASSWORD_ERROR);
                    }
                    break;
                }

            case 5:
                {   /* *    5.     下行密码  */ 
                    if (EAT_APR_OK == simcom_at_parser_get_str(0, 8, IDParam ))
                    {
                        strcpy((char *)mainContext_p->SecurityContext.downlinkPassword, IDParam);
                    }
                    else
                    {
                        WMMP_DBG_E(WMMP_SET_DOWNLINK_PASSWORD_ERROR);
                    }
                    break;
                }

            case 6:
                {  /* *    6.    基础密钥 */
                    if (EAT_APR_OK == simcom_at_parser_get_str(0, 31, IDParam ))
                    {
                        strcpy((char *)mainContext_p->SecurityContext.publickey , IDParam);
                    }
                    else
                    {
                        WMMP_DBG_E(WMMP_SET_PUBLICKEY_ERROR);
                    }
                    break;
                }

            case 19:
                {  /* *    19.    基础密钥 */
                    if (EAT_APR_OK == simcom_at_parser_get_str(0, 100, IDParam ))
                    {
                        //u32 tmpVall = atoi((char*)&IDParam);
                        u32 tmpVall = strtoul((char*)&IDParam,NULL,0);
                        mainContext_p->SecurityContext.publicKeyExpDate=tmpVall;
                        WMMP_DBG(WMMP_SET_PUBLICKEY_EXPDATE,mainContext_p->SecurityContext.publicKeyExpDate);
                    }
                    else
                    {
                        WMMP_DBG_E(WMMP_SET_PUBLICKEY_EXPDATE_ERROR);
                    }
                    break;
                }

            case 7:
                {   /* 已注册的 M2M IMSI*/  
                    if (EAT_APR_OK == simcom_at_parser_get_str(0, 15, IDParam ))
                    {
                        strcpy((char *)mainContext_p->SecurityContext.BindIMSIOrig , IDParam);
                    }
                    else
                    {
                        WMMP_DBG_E(WMMP_SET_BIND_IMSI_ERROR);
                    }
                    break;
                }                   

            case 8:
                {   /*  M2M ip add   at+M2MPARACFG=8,C0A80A5E,27072707*/   
                    /*该接口获取hex值，需要更改*/
                    if ((EAT_APR_OK == simcom_at_parser_get_str(0, ULONG_MAX, ipadd ))
                            &&(EAT_APR_OK == simcom_at_parser_get_str(0, ULONG_MAX, ipport )))
                    {
                        u32 ip_add =strtoul((char*)&ipadd,NULL,16);
                        u32 ip_port =strtoul((char*)&ipport,NULL,16);
                        WMMP_DBG(WMMP_SET_IP_ADDR_AND_PORT,ip_add,ip_port);
                        mainContext_p->ConfigContext.M2MIPAddr =ip_add;
                        mainContext_p->ConfigContext.M2MPort =ip_port;                           
                    }
                    else
                    {
                        WMMP_DBG_E(WMMP_SET_IP_ADDR_AND_PORT_ERROR);
                    }                       
                    break;
                }

            case 10:
                {   /*  修改终端自己的IMEI 号和IMSI 号*/    
                    if ((EAT_APR_OK == simcom_at_parser_get_str(0, 15, IDParam ))
                            &&(EAT_APR_OK == simcom_at_parser_get_str(0, 16, strtemp )))
                    {
                        WMMP_DBG(WMMP_SET_OLD_IMSI_AND_IMEI, g_commom_imei_str,g_commom_imsi_str);

                        WMMP_DBG(WMMP_SET_IMSI_AND_IMEI, IDParam,strtemp);
                        WPI_MODIFY_IMEI_IMSI(IDParam,strtemp);
                    }                       
                    else
                    {
                        WMMP_DBG(WMMP_SET_OLD_IMSI_AND_IMEI, g_commom_imei_str,g_commom_imsi_str);
                        WMMP_DBG_E(WMMP_SET_IMSI_AND_IMEI_ERROR);
                    }
                    break;
                }

            case 9:
                {
                    //                      simcom_wmmp_file_data_init();
                    break;
                }

            case 11:
                {   
                    /*  查询终端当前的命令运行状态*/
                    break;
                }

            case 12:
                {   
                    /*  查询终端当前的命令运行状态*/
                    break;
                }

            case 13:
                {
                    //simcom_wmmp_file_data_write();
                    WriteAllTLVParam2Flash();
                    break;
                }

            case 15:
                {
                    /* cbc@20100412:释放 WMMP 协议流程*/
                    WPA_Term();
                    mainContext_p->terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
                    mainContext_p->last_terminal_run_status = M2M_TERMINAL_RUN_STATUS_INIT;
                    wmmpFreeTest();
                    WPA_Term();
                    break;
                }

            case 14:
                {   
                    break;
                }       

            case 16:
                {
                    /* cbc@20100412: SIM 更换测试保证IMSI 号正常*/      

                    /*****************************************************************************
                      add by cbc@20100412:SIM 卡更换,重新注册,
                      测试时还用原来SIM卡,此时修改绑定IMSI号,算摘要时还用原IMSI号   ,

                     *******************************************************************************
                     *****/
                    sim_card_changed_test(1);
                    break;
                }

            case 17:
                {
                    /* cbc@20100412: SIM 更换测试保证IMSI 号正常*/  
                    sim_card_changed_test(0);
                    break;
                }

            case 18:
                {
                    /*   18.     未注册时预置的终端序列号 */
                    if (EAT_APR_OK == simcom_at_parser_get_str(0, 16, IDParam ))
                    {
                        strcpy((char *)mainContext_p->CustomContext.UnregTerminalNum, IDParam)
                            ;   
                    }
                    else
                    {
                        WMMP_DBG_E(WMMP_SET_UNREG_TERMINAL_SERIES_NUM_ERROR);
                    }
                    break;
                }


            default:
                {
                    WMMP_DBG_E(WMMP_SET_PARAMETER_ERROR);

                    break;
                }

        }

    }
    simcom_at_output_default_end();

}



/*****************************************************************************
 * 函数名: m2mdatatest_set_hdlr
 * 作  者:  Laiwenjie
 * 功能描述: AT$M2MDATATEST=*
 * 输入参数说明: 
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
/**************************************************************************************
 *
 *                                       M2M wmmp 端到端业务数据发送
 *   模拟测试AT$M2MDATA ，因为没有at 透传
 *    AT$M2MDATA=T(E),12345678
 *    >ab234567
 *
 *AT$M2MDATA=<TYPE>,<DESTINATION>[,<TRANS_ID>,<TOTAL>,<SEQ>]<CR><LF>等待”>” 
 （0x3E）后输<DATA><CTRL-Z>
 *
 *<TYPE>：目的地址类型，T表示目的地址为终端序列号，E表示目的地址为EC业务码；
 *<DESTINATION>：目的地址（终端序列号或EC业务码）；
 *<TRANS_ID>、<TOTAL>、<SEQ> ： 分包事务的TRANS_ID
 、子包总数、子包序号，用于分包处理，参见0x4008的TLV说明。
 *<DATA>：透传数据。采用HEX编码。
 *
 *************************************************************************************/

void m2mdatatest_set_hdlr(void *pPfData)
{
    char type[5];
    char destination[30];
    char data[128];

    s16 char_len;
    s16 i;

    char buffer[MAX_UART_DATA_LEN];

    StatusNumber_t *wmmpStatus_p = (StatusNumber_t *)WPA_GetWmmpStatus();   

    WMMP_DBG(WMMP_WOPEN_VALUE,gwopenValue);

    if((WPA_isWmmpRun()== false)||(wmmpStatus_p->protocol_status!=LOGINOK))
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_UNLOGIN);
    }
    else if (EAT_APR_OK == simcom_at_parser_get_str(0, 5, type ))   
    {
        WMMP_DBG(WMMP_M2MDATATEST_TYPE,type);

        if (EAT_APR_OK == simcom_at_parser_get_str(0, 30, destination))
        {
            WMMP_DBG(WMMP_M2MDATATEST_DEST,destination);

            if(((tolower(type[0])=='t')||(tolower(type[0])=='e'))&&(strlen((char*)destination)!=0))
            {
                if (EAT_APR_OK == simcom_at_parser_get_str(0, 128, data ))
                {
                    if ( 0 != strlen((char*)data) )
                    {
                        char *sendata;
                        sendata = (char*)WPI_Malloc(char_len/2);              
                        memset(sendata,0,char_len/2);

                        for(i=0;i<char_len/2;i++)
                        {
                            sendata[i]=OChar2Bin8(data+i*2);
                        }       

                        if (!WPA_DataReq(type[0], sendata, char_len/2, destination))
                        {
                            WPI_Free(sendata);
                            sendata = NULL;
                        }
                    }
                }
            }

        }


    }

    sprintf(buffer, "\r\n+M2MTEST:\r\n");
    simcom_at_output("%s",(u8*)buffer);

    simcom_at_output_default_end();

}

/*****************************************************************************
 * 函数名: wmmp_M2MVER_active_hdlr
 * 作  者:  Laiwenjie
 * 功能描述: AT$M2MVER
 * 输入参数说明: 
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2mver_active_hdlr(void *pPfData)
{
    char build_date_time_str[100] = {0};
    WmmpContext_t *mainContext_p = ptrToWmmpContext();
    bool      ret_val = false;
    u8        eq_id[100]={0};
    u8        buffer[RMMI_SHORT_RSP_LEN+RMMI_SHORT_RSP_LEN];
    u8        build_time[20];
    u8        build_user[20];

    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    l4cuem_get_equip_id_req(EAT_EQ_ME_REVISION_ID, (u8 *)eq_id);
    WMMP_DBG(WMMP_M2MVER_EQ_ID,ret_val,eq_id);

    memcpy(build_time, (u8*) build_date_time(), 20);

    sprintf(build_date_time_str,"BUILD @: %s by %s",build_time,build_user);
    WMMP_DBG(WMMP_M2MVER_BUILD_TIME,build_date_time_str);            

    simcom_at_output_cr_lf();
    simcom_at_output("$M2MVER:");

    simcom_at_output_cr_lf();
    simcom_at_output("APP:%d.%02d",mainContext_p->StatusContext.Majorver,mainContext_p->StatusContext.MinorVer);

    simcom_at_output_cr_lf();
    simcom_at_output("WMMP:3.00");

    simcom_at_output_cr_lf();
    simcom_at_output("Fireware:%s SubRevision %s",eq_id,(u8*)release_build());

    simcom_at_output_cr_lf();
    simcom_at_output("%s", build_date_time_str);

    simcom_at_output_cr_lf();

    simcom_at_output_default_end();
}

/*****************************************************************************
 * 函数名: wmmp_M2MALMRST_test_hdlr
 * 作  者:  Laiwenjie
 * 功能描述: AT$M2MALMRST=?
 * 输入参数说明: 
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2malmrst_test_hdlr(void *pPfData)
{
    char        passCode[20] = "\0";
    WmmpContext_t *mainContext_p = ptrToWmmpContext();
    char *pl_StrPtr = NULL;

    WMMP_DBG(WMMP_WOPEN_VALUE,gwopenValue);
    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    simcom_at_output_cr_lf();
    simcom_at_output("$M2MALMRST: <PASSCODE>");
    simcom_at_output_cr_lf();
    simcom_at_output_default_end();

}

/*****************************************************************************
 * 函数名: wmmp_M2MALMRST_set_hdlr
 * 作  者:  Laiwenjie
 * 功能描述: AT$M2MALMRST=*
 * 输入参数说明: 
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2malmrst_set_hdlr(void *pPfData)
{
    char        passCode[20] = "\0";
    WmmpContext_t *mainContext_p = ptrToWmmpContext();
    u16  passCodeLen = 0;

    WMMP_DBG(WMMP_WOPEN_VALUE,gwopenValue);
    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    memset(passCode,0,sizeof(passCode));             
    if (EAT_APR_OK == simcom_at_parser_get_str(0, 4, passCode ))
    {
        WMMP_DBG(WMMP_M2MALMRST_PASSCODE,passCode,passCodeLen);                
        if (!strcmp( "1234",passCode))
        {              
            mainContext_p->m2mswitch = true;
            mainContext_p->Wmmp_status.protocol_status = LOGINOK; 
        }
        else
        {
            simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        }
    }
    else
    {
    }
    simcom_at_output_default_end();

}

/*****************************************************************************
 * 函数名: wmmp_M2MBHV_test_hdlr
 * 作  者:  Laiwenjie
 * 功能描述: AT$M2MBHV=?
 * 输入参数说明: 
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2mbhv_test_hdlr(void *pPfData)
{
    char*           	pl_StrPtr = NULL;       

    WMMP_DBG(WMMP_WOPEN_VALUE,gwopenValue);
    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    simcom_at_output_cr_lf();
    simcom_at_output("$M2MBHV: <MODE>");
    simcom_at_output_cr_lf();

    simcom_at_output("$M2MBHV: 0,\"Use TLV interactive edit: <0/1>\"");
    simcom_at_output_cr_lf();

    simcom_at_output("$M2MBHV: 1,\"Use TLV printing: <0/1>\"");
    simcom_at_output_cr_lf();

    simcom_at_output("$M2MBHV: 2,\"Use TLV parsing: <0/1>\"");
    simcom_at_output_cr_lf();

    simcom_at_output("$M2MBHV: 3,\"Use DATA interactive edit: <0/1>\"");
    simcom_at_output_cr_lf();

    simcom_at_output("$M2MBHV: 4,\"Use DATA is RAW: <0/1>\"");
    simcom_at_output_cr_lf();

    simcom_at_output("$M2MBHV: 5,\"Use Received DATA printing: <0/1>\"");
    simcom_at_output_cr_lf();

    simcom_at_output_default_end();

    if((bhvMode==0)||(bhvMode==3))
    {
        WmmpSend_Echo_Bs_flag();
    }
}

/*****************************************************************************
 * 函数名: wmmp_M2MBHV_read_hdlr
 * 作  者:  Laiwenjie
 * 功能描述: AT$M2MBHV?
 * 输入参数说明: 
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2mbhv_read_hdlr(void *pPfData)
{
    u16			loopi = 0;	

    WMMP_DBG(WMMP_WOPEN_VALUE,gwopenValue);
    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }


    simcom_at_output_cr_lf();
    for(loopi = 0;loopi < MAX_BHV_MODE;loopi++)
    {
        simcom_at_output("$M2MBHV: %d , %d",loopi,bhvValue[loopi]);

        simcom_at_output_cr_lf();
    }		        

    simcom_at_output_default_end();


    if((bhvMode==0)||(bhvMode==3))
    {
        WmmpSend_Echo_Bs_flag();
    }
}

/*****************************************************************************
 * 函数名: wmmp_M2MBHV_set_hdlr
 * 作  者:  Laiwenjie
 * 功能描述: AT$M2MBHV=*
 * 输入参数说明: 
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2mbhv_set_hdlr(void *pPfData)
{
    u32			Param1 = 0;
    u32    		Param2 = 0;

    WMMP_DBG(WMMP_WOPEN_VALUE,gwopenValue);
    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    if((EAT_APR_OK == simcom_at_parser_get_u32(0, ULONG_MAX, &Param1)))
    {
        WMMP_DBG(WMMP_M2MBHV_PARAM_1,Param1); 
        if((Param1>=0) && (Param1<MAX_BHV_MODE))
        {
            if (EAT_APR_OK == simcom_at_parser_get_u32(0, ULONG_MAX, &Param2))
            {
                WMMP_DBG(WMMP_M2MBHV_PARAM_2,Param2);
                if((Param2==0) || (Param2==1))
                {
                    bhvMode = Param1;
                    bhvValue[bhvMode] = Param2;
                    WMMP_DBG(WMMP_M2MBHV_VALUE,bhvMode,bhvValue[bhvMode]);
                }
                else
                {
                    WMMP_DBG_E(WMMP_M2MBHV_PARAM_2_ERROR,Param2); 
                    simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_PARAMETER_WRONG);
                }                            
            }
            else
            {
                WMMP_DBG_E(WMMP_M2MBHV_GET_PARAM_2_ERROR,Param2);                            
                bhvMode = Param1;
                WMMP_DBG(WMMP_M2MBHV_MODE,bhvMode);
                WMMP_DBG(WMMP_M2MBHV_LAST_VALUE,bhvMode,bhvValue[bhvMode]);
            }
        }
        else 
        {
            WMMP_DBG_E(WMMP_M2MBHV_PARAM_1_ERROR,Param1); 
            simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_PARAMETER_WRONG);
        }
    }
    else
    {
        WMMP_DBG_E(WMMP_M2MBHV_GET_PARAM_1_ERROR,Param1); 
    }

    simcom_at_output_default_end();
    if((bhvMode==0)||(bhvMode==3))
    {
        WmmpSend_Echo_Bs_flag();
    }
}

/*****************************************************************************
 * 函数名: wmmp_WOPEN_set_hdlr
 * 作  者:  Laiwenjie
 * 功能描述: AT+WOPEN=*
 * 输入参数说明: 
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void wopen_set_hdlr(void *pPfData)
{
    u32               wopenParam = 0;
    WmmpContext_t    *mainContext_p = ptrToWmmpContext();


    WMMP_DBG(WMMP_WOPEN_VALUE,gwopenValue);

    if(EAT_APR_OK == simcom_at_parser_get_u32(0, ULONG_MAX, &wopenParam))
    {
        WMMP_DBG(WMMP_SET_WOPEN_PARAM,wopenParam); 

        if(wopenParam == 0)
        {  
            gwopenValue = wopenParam;
            atcValueNv.wopenValueNv = gwopenValue;
            //vgWMMPWriteNvram (NVRAM_EF_WMMP_WOPEN_LID,0);
            wmmpProcessSave(NVRAM_EF_WMMP_WOPEN_LID,&atcValueNv,sizeof(Wmmp_atc_value_struct));

            WPA_LogoutReq(0);

        }
        else if(wopenParam == 1)
        {
            WMMP_DBG(WMMP_WOPEN_VALUE,gwopenValue);

            if(1 == gwopenValue)
            {
                WMMP_DBG(WMMP_WOPEN_ALREADY_OPEN);						
            }
            else
            {
                bool rett = false;

                gwopenValue = wopenParam;
                atcValueNv.wopenValueNv = gwopenValue;
                wmmpProcessSave(NVRAM_EF_WMMP_WOPEN_LID,&atcValueNv,sizeof(Wmmp_atc_value_struct));

                rett = WPA_Init();

                if (rett)
                {
                    WPA_LoginReq();
                }
                else
                {
                    WMMP_DBG_E(WMMP_WPA_INIT_FAILED,rett); 

                    WPA_Term();
                    simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
                }
            }
        }              		
        else
        {                  			
            WMMP_DBG_E(WMMP_SET_WOPEN_PARAM_ERROR,wopenParam); 
            simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_PARAMETER_WRONG);
        }       
    }
    else
    {
        WMMP_DBG_E(WMMP_SET_WOPEN_GET_PARAM_ERROR,wopenParam); 
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
    }

    simcom_at_output_default_end();
}
/*****************************************************************************
 * 函数名: wmmp_WOPEN_test_hdlr
 * 作  者:  Laiwenjie
 * 功能描述: AT+WOPEN=?
 * 输入参数说明: 
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void wopen_test_hdlr(void *pPfData)
{
    WMMP_DBG(WMMP_WOPEN_VALUE,gwopenValue);

    simcom_at_output_cr_lf();
    simcom_at_output("+WOPEN: (0,1)");
    simcom_at_output_cr_lf();
    simcom_at_output_default_end();
}

/*****************************************************************************
 * 函数名: wmmp_WOPEN_read_hdlr
 * 作  者:  Laiwenjie
 * 功能描述: AT+WOPEN?
 * 输入参数说明: 
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void wopen_read_hdlr(void *pPfData)
{
    WMMP_DBG(WMMP_WOPEN_VALUE,gwopenValue);

    simcom_at_output_cr_lf();
    simcom_at_output("+WOPEN: %d",gwopenValue);
    simcom_at_output_cr_lf();
    simcom_at_output_default_end();
}


/*****************************************************************************
 * 函数名: wmmp_WREBOOT_active_hdlr
 * 作  者:  Laiwenjie
 * 功能描述: AT+WREBOOT
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void wreboot_active_hdlr(void *pPfData)
{
    WMMP_DBG(WMMP_WOPEN_VALUE,gwopenValue);

    simcom_at_output_default_end();
    WPI_SystemReboot();
}
/*SIMCOM Laiwenjie 2013-01-25 Fix MKBug00015013 for at command END*/


void simcom_output_symbol(void)
{
    u8  buffer[10] = {0};
    buffer[0]='>';
    simcom_at_output_async(simcom_at_parser_get_src_id(), buffer, strlen(buffer), true);

}

/*************************************************************************
 *                                              TAG  配置
 *   at$m2mcfg=8                ---- 对8个TAG 进行参数配置,小于8个按实际操作,
 大于8个取前8个
 *   >0002,1100
 *   >0002:111111
 *   >0007:31303635373137333431
 *   >0009:DAC92D63
 *   >000A:26FD26FD
 *   >0011:0000001E
 **************************************************************************/
void m2mcfg_test_hdlr(void *data)
{
    simcom_at_output("$M2MCFG: (1-32)");
    simcom_at_output_default_end();
}
void m2mcfg_read_hdlr(void *data)
{
    TLVRetVal_t tlv,value;  
    u16 i,Tag;
    char *p=NULL;
    char *temp=NULL;

    WMMP_DBG_TEMP("m2mcfg_read_hdlr");
    WPA_ReadTLV(TLVSYNCTAGVALUE,&tlv);
    p= tlv.addr;	
    WMMP_DBG_TEMP("m2mcfg_read_hdlr:bhvValue[1]=%d,tlv.length=%d",bhvValue[1],tlv.length);

    if(bhvValue[1] == 1)
    {
        for(i=0;i<tlv.length/2;i++)
        {

            Tag = Readu16(p);
            WPA_ReadTLV(Tag,&value);	
            p+=2;				

            switch (value.TLVType)
            {
                case TLV_INT16:
                    {
                        u16 tmpVal =0;
                        memcpy(&tmpVal,value.addr,2);
                        WMMP_DBG_TEMP("INT6 tmpVal is %02X\n",tmpVal);
                        if(value.length)
                        {
                            simcom_at_output("%04X:0x%02x",Tag,tmpVal);
                        }
                        else
                        {
                            simcom_at_output("%04X:0",Tag);
                        }
                        simcom_at_output_cr_lf();
                        break;
                    }										
                case TLV_INT32:
                    {
                        u32 tmpVal = 0;
                        memcpy(&tmpVal,value.addr,4);
                        WMMP_DBG_TEMP("INT32 tmpVal is %04X\n",tmpVal);
                        if(value.length)
                        {
                            simcom_at_output("%04X:0x%04x",Tag,tmpVal);
                        }
                        else
                        {
                            simcom_at_output("%04X:0",Tag);
                        }
                        simcom_at_output_cr_lf();
                        break;
                    }										
                case  TLV_STRING:
                    {
                        WMMP_DBG_TEMP("m2mCfg TLV_STRING wcom ");

                        if(value.length)
                        {
                            simcom_at_output("%04X:\"%s\"",Tag,(char*)value.addr);
                        }
                        else
                        {
                            simcom_at_output("%04X:\"\"",Tag);
                        }
                        simcom_at_output_cr_lf();
                        break;
                    }
                case TLV_INT8:
                    {
                        u8 tmpVal = 0;
                        memcpy(&tmpVal,value.addr,1);
                        WMMP_DBG_TEMP("INT8 tmpVal is %x",tmpVal);
                        if(value.length)
                        {
                            simcom_at_output("%04X:%d",Tag,tmpVal);
                        }
                        else
                        {
                            simcom_at_output("%04X:0",Tag);
                        }
                        simcom_at_output_cr_lf();
                        break;	
                    }						

                default:
                    {
                        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_UNKNOWN);
                        break;
                    }

            }				
        }
    }
    else
    {
        for(i=0;i<tlv.length/2;i++)
        {

            Tag = Readu16(p);
            WPA_ReadTLV(Tag,&value);	
            p+=2;				
            temp = (char*)WPI_Malloc(value.length*2+1);
            memset(temp,0,value.length*2+1);
            WMMP_DBG_TEMP("i=%d,Tag=%04X,value.TLVType=%d\n",i,Tag,value.TLVType);	

            switch (value.TLVType)
            {
                case  TLV_INT16:
                    {
                        u16 tmpVal =0;
                        memcpy(&tmpVal,value.addr,2);
                        tmpVal = soc_htons(tmpVal); 											
                        WMMP_DBG_TEMP("tmpVal is %02X\n",tmpVal);	
                        if(value.length)
                        {
                            StringToHexCode((char*)&tmpVal, temp, value.length);
                            simcom_at_output("%04X:\"%s\"",Tag,temp);
                        }
                        else
                        {
                            simcom_at_output("%04X:\"0000\"",Tag);
                        }
                        simcom_at_output_cr_lf();
                        break;
                    }
                case TLV_INT32:
                    {
                        u32 tmpVal = 0;
                        memcpy(&tmpVal,value.addr,4);
                        tmpVal = soc_htonl(tmpVal); 											
                        WMMP_DBG_TEMP("tmpVal is %04X\n",tmpVal);								
                        if(value.length)
                        {
                            StringToHexCode((char*)&tmpVal, temp, value.length);
                            simcom_at_output("%04X:\"%s\"",Tag,temp);
                        }
                        else
                        {
                            simcom_at_output("%04X:\"00000000\"",Tag);
                        }
                        simcom_at_output_cr_lf();
                        break;
                    }
                case  TLV_STRING:
                    {
                        WMMP_DBG_TEMP("TLV m2mCfg TLV_STRING");								
                        if(value.length)
                        {
                            StringToHexCode((char*)value.addr, temp, value.length);
                            simcom_at_output("%04X:\"%s\"",Tag,temp);
                        }
                        else
                        {
                            simcom_at_output("%04X:\"0000\"",Tag);
                        }
                        simcom_at_output_cr_lf();
                        break;
                    }
                case TLV_INT8:
                    {
                        WMMP_DBG_TEMP("TLV m2mCfg TLV_INT8");
                        if(value.length)
                        {
                            StringToHexCode((char*)value.addr, temp, value.length);
                            simcom_at_output("%04X:\"%s\"",Tag,temp);
                        }
                        else
                        {
                            simcom_at_output("%04X:\"00\"",Tag);
                        }
                        simcom_at_output_cr_lf();
                        break;
                    }						

                default:
                    {
                        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_UNKNOWN);
                        break;
                    }
            }
            WPI_Free(temp); 								
        }
    }			
    simcom_at_output_default_end();
}
void m2mcfg_set_hdlr(void *data)
{
    s32 cfgnum;
    u8  buffer[10] = {0};
    if((WPA_isWmmpRun()== false)
            ||(EAT_APR_OK != simcom_at_parser_get_u32(1, 32, &cfgnum)))
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_PARAMETER_WRONG);
        simcom_at_output_default_end();
        return;
    }
    g_src_id=simcom_at_parser_get_src_id();

    WMMP_DBG_TEMP("m2mcfg_set_hdlr cfgnum %d",cfgnum);
    simcom_m2m_at_len = cfgnum; 	   
    simcom_m2m_at_type = VG_AT_M2MC_CFG;
    simcom_output_symbol();
    eat_ds_enter_data_mode();
}
/**************************************************************************************
 *
 *        M2M WMMP 参数查询,每次查询不能超过 8  个
 *   例子: AT$M2MCFG=0002,0007,0009,000A,0011    ---------  查询 TAG 为0002，0007，
 *                                  0009，000A,0011对应的TLV值
 *************************************************************************************/
void m2mcfglist_test_hdlr(void *data)
{
    simcom_at_output("$M2MCFGLIST: [<TAG>,<VALUE>]*");
    simcom_at_output_default_end();
}
void m2mcfglist_set_hdlr(void *data)
{
    char TagIdParam[5]={0};
    s32 TagId;
    char *temp = NULL;
    TLVRetVal_t value; 
    u8 i;	
    u16 loopi;
    WmmpContext_t  *mainContext_p = ptrToWmmpContext(); 	 
    WmmpCustom2Context_t  *Custom2Context_p = &(mainContext_p->Custom2Context);

    if(WPA_isWmmpRun()== false)
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_PARAMETER_WRONG);
        simcom_at_output_default_end();
        return;
    }

    simcom_at_output("$M2MCFGLIST:");
    simcom_at_output_cr_lf();
    for(i=0;i<8;i++)
    {
        memset(TagIdParam,0x0,sizeof(TagIdParam));
        if(EAT_APR_OK == simcom_at_parser_get_str(1, 4, TagIdParam))
        {
            TagId=strtoul(TagIdParam, 0, 16);
            WMMP_DBG_TEMP("m2mcfglist_set_hdlr TagId %4x",TagId);
            WPA_ReadTLV((u16)TagId,&value); 
            temp = (char*)WPI_Malloc(value.length*2+1);
            memset((char *)temp,0x00,(value.length*2+1));
            WMMP_DBG_TEMP("m2mcfglist_set_hdlr value.TLVType=%d,bhvValue[1]=%d",value.TLVType,bhvValue[1]);

            if(bhvValue[1] ==1)
            {
                switch (value.TLVType)
                {
                    case  TLV_INT16:
                        {
                            u16 tmpVal =0;
                            memcpy(&tmpVal,value.addr,2);																										
                            WMMP_DBG_TEMP("m2mcfglist_set_hdlr: INT6 tmpVal is %02X\n ",tmpVal);
                            tmpVal=soc_htons(tmpVal);

                            if(value.length)
                            {
                                simcom_at_output("%04X:0x%02x",TagId,tmpVal);
                            }
                            else
                            {
                                simcom_at_output("%04X:0",TagId);
                            }			
                            simcom_at_output_cr_lf();
                            break;
                        }										
                    case TLV_INT32:
                        {
                            u32 tmpVal = 0;
                            memcpy(&tmpVal,value.addr,4);
                            WMMP_DBG_TEMP("m2mcfglist_set_hdlr INT32 tmpVal is %04x \n ",tmpVal);
                            if(value.length)
                            {
                                simcom_at_output("%04X:0x%04x",TagId,tmpVal);
                            }
                            else
                            {
                                simcom_at_output("%04X:0",TagId);
                            }
                            simcom_at_output_cr_lf();
                            break;
                        }										
                    case  TLV_STRING:
                        {
                            switch (TagId)
                            {
                                case 0x1005:
                                    {
                                        simcom_at_output("%04X:DL_VER-VERSION",TagId);
                                        if(value.length)
                                        {
                                            simcom_at_output("version(16)\"%s\"",(char*)value.addr);

                                        }
                                        else
                                        {
                                            simcom_at_output("version(16)\"\"");
                                        }
                                        simcom_at_output_cr_lf();
                                    }
                                    break;			
                                case 0x100C:
                                    {														
                                        simcom_at_output("%04X:DL_VER-VERSION",TagId);
                                        if(value.length)
                                        {
                                            simcom_at_output("verstr(8)\"%s\"",(char*)value.addr);

                                        }
                                        else
                                        {
                                            simcom_at_output("verstr(8)\"\"");
                                        }
                                    }												
                                    simcom_at_output_cr_lf();
                                    break;

                                case 0x300f:
                                    {										
                                        char mDevNum[5] = "\0";
                                        u16 mdestDevNum[10];
                                        char mDevStatus[2] = "\0";
                                        u8 mdestDevStatus[10];
                                        simcom_at_output("%04X ",TagId);

                                        if(value.length)
                                        {
                                            for(loopi = 0;loopi < value.length/6;loopi++)
                                            {
                                                memset(mDevNum,0,sizeof(mDevNum));
                                                memcpy(mDevNum,&value.addr[6*loopi],4); 															
                                                mdestDevNum[loopi] = vgAsciiToDecu16(mDevNum);
                                                memset(mDevStatus,0,sizeof(mDevStatus));
                                                memcpy(mDevStatus,(&value.addr[6*loopi])+4,2);
                                                mdestDevStatus[loopi] = vgAsciiToDecu16(mDevStatus);
                                                simcom_at_output(",%02d,%d",mdestDevNum[loopi],mdestDevStatus[loopi]);
                                            }
                                        }
                                        else
                                        {
                                            simcom_at_output("\"\"");
                                        }
                                        simcom_at_output_cr_lf();
                                    }
                                    break;												
                                case 0x3010:
                                    {

                                        char mSupportDev[5] = "\0";
                                        u16 mdestSupDev[10];														
                                        simcom_at_output("%04X ",TagId);

                                        if(value.length)
                                        {
                                            for(loopi = 0;loopi < value.length/4;loopi++)
                                            {
                                                memset(mSupportDev,0,sizeof(mSupportDev));
                                                memcpy(mSupportDev,&value.addr[4*loopi],4); 															
                                                mdestSupDev[loopi] = vgAsciiToDecu16(mSupportDev);
                                                simcom_at_output(",%02d",mdestSupDev[loopi]);
                                            }
                                        }
                                        else
                                        {
                                            simcom_at_output("\"\"");
                                        }
                                        simcom_at_output_cr_lf();
                                    }												
                                    break;
                                case 0xd001:
                                case 0xd002:
                                case 0xd003:
                                case 0xd004:
                                case 0xd005:
                                case 0xd006:
                                case 0xd007:
                                case 0xd008:
                                    {
                                        u8 serNumCount = TagId-0xd001;
                                        WMMP_DBG_TEMP("m2mcfglist_set_hdlr TLV_STRING serNumCount is %d\n ",serNumCount);
                                        simcom_at_output("%04X:PRM_FLOW%02d-URL",TagId,serNumCount+1);
                                        simcom_at_output("brtype[%d]addr(%d)\"%s\"",Custom2Context_p->AppSerType[serNumCount],strlen(Custom2Context_p->AppServerString[serNumCount]),&Custom2Context_p->AppServerString[serNumCount][1]);
                                    }
                                    simcom_at_output_cr_lf();
                                    break;
                                default:
                                    {														
                                        if(value.length)
                                        {
                                            simcom_at_output("%04X,\"%s\"",TagId,(char*)value.addr);
                                        }
                                        else
                                        {
                                            simcom_at_output("%04X,\"\"",TagId);
                                        }
                                        simcom_at_output_cr_lf();
                                        break;
                                    }
                            }
                            break;
                        }
                    case TLV_INT8:
                        {
                            u8 tmpVal = 0;
                            memcpy(&tmpVal,value.addr,1);
                            if(value.length)
                            {
                                simcom_at_output("%04X:%d",TagId,tmpVal);
                            }
                            else
                            {
                                simcom_at_output("%04X:0",TagId);
                            }
                            break;
                            simcom_at_output_cr_lf();
                        }						
                    default:
                        {
                            simcom_at_output("%04X:%d unknown",TagId);
                            simcom_at_output_cr_lf();
                            break;
                        }
                }					
            }
            else
            {
                switch (value.TLVType)
                {
                    case  TLV_INT16:
                        {
                            u16 tmpVal =0;
                            memcpy(&tmpVal,value.addr,2);
                            tmpVal = soc_htons(tmpVal);									
                            WMMP_DBG_TEMP("m2mcfglist_set_hdlr:TLV_INT16 tmpVal is %02X\n",tmpVal);	
                            if(value.length)
                            {
                                StringToHexCode((char*)&tmpVal, temp, value.length);
                                simcom_at_output("%04X,\"%s\"",TagId,temp);
                            }
                            else
                            {
                                simcom_at_output("%04X,\"0000\"",TagId);
                            }
                            simcom_at_output_cr_lf();
                            break;
                        }
                    case TLV_INT32:
                        {
                            u32 tmpVal = 0;
                            memcpy(&tmpVal,value.addr,4);
                            WMMP_DBG_TEMP("m2mcfglist_set_hdlr INT32 tmpVal is %04X\n ",tmpVal);
                            tmpVal = soc_htonl(tmpVal); 										
                            if(value.length)
                            {
                                StringToHexCode((char*)&tmpVal, temp, value.length);
                                simcom_at_output("%04X,\"%s\"",TagId,temp);
                            }
                            else
                            {
                                simcom_at_output("%04X,\"00000000\"",TagId);
                            }
                            simcom_at_output_cr_lf();
                            break;
                        }
                    case  TLV_STRING:
                        {
                            if(value.length)
                            {
                                switch (TagId)
                                {
                                    case 0xd001:
                                    case 0xd002:
                                    case 0xd003:
                                    case 0xd004:
                                    case 0xd005:
                                    case 0xd006:
                                    case 0xd007:
                                    case 0xd008:
                                        {
                                            if(isdigit(value.addr[0]))	
                                            {
                                                StringToHexCode((char*)(&value.addr[1]), temp, value.length-1);
                                                simcom_at_output("%04X,\"%02d%s\"",TagId,value.addr[0]-0x30,temp);
                                            }
                                            else
                                            {
                                                StringToHexCode((char*)(&value.addr[1]), temp, value.length-1);
                                                simcom_at_output("%04X,\"%02d%s\"",TagId,value.addr[0],temp);
                                            }
                                        }
                                        simcom_at_output_cr_lf();
                                        break;
                                    default:
                                        {
                                            StringToHexCode((char*)value.addr, temp, value.length);
                                            simcom_at_output("%04X,\"%s\"",TagId,temp);
                                            simcom_at_output_cr_lf();
                                            break;
                                        }
                                }		
                            }
                            else
                            {
                                simcom_at_output("%04X,\"0000\"",TagId);
                                simcom_at_output_cr_lf();
                            }
                            break;
                        }
                    case TLV_INT8:
                        {
                            if(value.length)
                            {
                                StringToHexCode((char*)value.addr, temp, value.length);
                                simcom_at_output("%04X,\"%s\"",TagId,temp);
                            }
                            else
                            {
                                simcom_at_output("%04X,\"00\"",TagId);
                            }
                            simcom_at_output_cr_lf();
                            break;
                        }						
                    default:
                        {
                            simcom_at_output("%04X,unknown",TagId);
                            simcom_at_output_cr_lf();
                            break;
                        }
                }					
            }					   
            WPI_Free(temp); 
            temp=NULL;
        }
        else
        {
            simcom_at_output_set_result(EAT_AER_OK,0);
            simcom_at_output_default_end();
            break;			
        }	
    }
    if(i>=8)
    {
        simcom_at_output_default_end();
    }
}

/******************************************************************************
 * Function:   vgM2MCTRAP    
 * 
 * Author:    
 * 
 * Parameters:    
 * 
 * Return:          
 * 
 * Description:      
 ******************************************************************************/
void m2mtrap_test_hdlr(void *data)
{
    simcom_at_output("$M2MTRAP: (1-8)");
    simcom_at_output_default_end();
}
void m2mtrap_set_hdlr(void *data)
{
    u32	TrapNum;
    StatusNumber_t *WmmpStatus_p=WPA_GetWmmpStatus();  
    if((WPA_isWmmpRun()==	false)
            ||(WmmpStatus_p->protocol_status!=LOGINOK)
            ||(EAT_APR_OK != simcom_at_parser_get_u32(0, 32, &TrapNum)))
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_PARAMETER_WRONG);
        simcom_at_output_default_end();
        return;
    }			
    g_src_id=simcom_at_parser_get_src_id();
    WMMP_DBG_TEMP("m2mtrap_set_hdlr TrapNum %d",TrapNum);
    simcom_m2m_at_len = TrapNum; 	   
    simcom_m2m_at_type = VG_AT_M2MC_TRAP;
    simcom_output_symbol();
    eat_ds_enter_data_mode();

}
/**************************************************************************************
 *
 *                                  M2M WMMP 主动配置请求
 *
 *    AT+M2MREQ=0002,0011              --------请求M2M平台对终端进行网络接入点和心跳间隔的参数配置
 **********************************************************************************/
void m2mreq_test_hdlr(void *data)
{
    simcom_at_output("$M2MREQ:<TAG>[,…]");
    simcom_at_output_default_end();
}

void m2mreq_set_hdlr(void *data)
{
    u16 *TLV=NULL;
    u8 i = 0;
    u16 tagnum=0;
    char TagIdParam[5]={0};
    s32 TagId;
    bool m2m_req = false;
    StatusNumber_t *WmmpStatus_p=WPA_GetWmmpStatus();  
    if((WPA_isWmmpRun()== false)||(WmmpStatus_p->protocol_status!=LOGINOK))
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_PARAMETER_WRONG);
        simcom_at_output_default_end();
        return;
    }
    TLV = (u16*)WPI_Malloc(16);
    memset(TLV,0,16);

    for (i=0;i<8;i++)
    {	
        if(EAT_APR_OK == simcom_at_parser_get_str(1, 4, TagIdParam))
        {
            TagId=strtoul(TagIdParam, 0, 16);
            WMMP_DBG_TEMP("m2mcfglist_set_hdlr TagId %4x",TagId);

            m2m_req = true;/*  M2MREQ 需要正常操作	*/	
            TLV[i] = TagId;
            tagnum++;
        }
        else
        {
            break;
        }
    }

    /* 输入的参数超过8个直接抛掉继续操作 */
    if (m2m_req)
    {		
        m2m_req= false; 
        WPA_ReqReq(TLV,tagnum);
        simcom_at_output("$M2MREQ:ACK:%d",Transaction_Id);
        simcom_at_output_set_result(EAT_AER_OK,0);
        simcom_at_output_default_end();
    }
}
/**************************************************************************************
 *
 *                                       M2M wmmp 断到端业务数据发送
 *
 *    AT$M2MDATA=T(E),12345678
 *    >ab234567
 *
 *AT$M2MDATA=<TYPE>,<DESTINATION>[,<TRANS_ID>,<TOTAL>,<SEQ>]<CR><LF>等待”>” 
 （0x3E）后输<DATA><CTRL-Z>
 *
 *<TYPE>：目的地址类型，T表示目的地址为终端序列号，E表示目的地址为EC业务码；
 *<DESTINATION>：目的地址（终端序列号或EC业务码）；
 *<TRANS_ID>、<TOTAL>、<SEQ> ： 分包事务的TRANS_ID
 、子包总数、子包序号，用于分包处理，参见0x4008的TLV说明。
 *<DATA>：透传数据。采用HEX编码。
 *
 *************************************************************************************/
void m2mdata_test_hdlr(void *data)
{
    simcom_at_output("$M2MDATA: <TYPE>,<DESTINATION>[,<TRANS_ID>,<TOTAL>,<SEQ>] ");
    simcom_at_output_default_end();
}
void m2mdata_set_hdlr(void *data)
{
    char	type[1]={0};
    char	destination[30]={0};
    StatusNumber_t *WmmpStatus_p=WPA_GetWmmpStatus();  
    if((WPA_isWmmpRun()== false)
            ||(WmmpStatus_p->protocol_status!=LOGINOK)
            ||(EAT_APR_OK != simcom_at_parser_get_str(0, 1, type))
            ||(EAT_APR_OK == simcom_at_parser_get_str(0, 30, destination)))
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_PARAMETER_WRONG);
        simcom_at_output_default_end();
        return;
    }
    WMMP_DBG_TEMP("m2mdata_set_hdlr type=%s,destination=%s",type,destination);
    if(((tolower(type[0])=='t')||(tolower(type[0])=='e'))&&(strlen((char*)destination)!=0))
    {

        g_src_id=simcom_at_parser_get_src_id();
        simcom_m2m_at_type = VG_AT_M2MC_DATA;
        memset(Data_type,0x00,sizeof(Data_type));
        memset(Data_destination,0x00,sizeof(Data_destination));
        memcpy(Data_type,type,1);
        memcpy(Data_destination,destination,30);
        simcom_output_symbol();
        eat_ds_enter_data_mode();
    }
    else
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_PARAMETER_WRONG);
        simcom_at_output_default_end();
    }				
}
/**************************************************************************************
 *
 *                                                 M2M WMMP 文件下载
 *
 * 1. 收到服务器下载更新的通知的例子:
 *		$M2MMSG:6,WMMP://61.135.68.199:80/wmmp_1.dat?TRANS_ID=00000295&FILE_SIZE=10908&CRC16=d4da&VER_DSC
 * 2. 向服务器下载数据:
 *	 AT$M2MFILE="00000295",0,0,300            ---- 开始从 0  位置开始下载300 字节
 *     AT$M2MFILE="00000295",1,300,400         ---- 继续从300位置下载400 字节
 *     AT$M2MFILE="00000295",2,65535,0         ---- 下载完毕,65535随便给一个数字都可以
 *************************************************************************************/
void m2mfile_test_hdlr(void *data)
{
    simcom_at_output("$M2MFILE: <TRANS_ID>,(0-3),<FILE_READ_POINT>,<FILE_BLOCK_SIZE>");
    simcom_at_output_default_end();
}
void m2mfile_set_hdlr(void *data)
{
    s8 tmpVal = 0;
    char	IDParam_t[8],IDParam[4];
    u16 IDParam_len,i;
    s32	Status,FileReadPoint,FileBlockSize;
    StatusNumber_t *WmmpStatus_p=WPA_GetWmmpStatus();  
    if((WPA_isWmmpRun()== false)||(WmmpStatus_p->protocol_status!=LOGINOK)||(EAT_APR_OK != simcom_at_parser_get_str(0, 8, IDParam_t)))
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_PARAMETER_WRONG);
        simcom_at_output_default_end();
        return;
    }

    IDParam_len=strlen(IDParam_t);
    memset(IDParam,0,4);
    WMMP_DBG_TEMP("IDParam_t =%s,IDParam_len=%d",IDParam_t,IDParam_len);
    for(i=0;i<IDParam_len/2;i++)
    {
        getHexaValue(IDParam_t[i*2],&tmpVal);
        IDParam[i]=tmpVal<<4;
        getHexaValue(IDParam_t[i*2+1],&tmpVal);
        IDParam[i]+=tmpVal;
    }
    if((EAT_APR_OK == simcom_at_parser_get_u32(0, 3, &Status))&&
            (EAT_APR_OK == simcom_at_parser_get_u32(0, 0xffffffff, &FileReadPoint))&&
            (EAT_APR_OK == simcom_at_parser_get_u32(0, 0xffffffff, &FileBlockSize)))
    {
        WMMP_DBG_TEMP("Status =%s,FileReadPoint=%d,FileBlockSize=%d",Status,FileReadPoint,FileBlockSize);
        WMMP_DBG_TEMP("m2mfile_set_hdlr:%x,%x,%x,%x\n",IDParam[0],IDParam[1],IDParam[2],IDParam[3]);
        if (Status == 2)
        {
            FileReadPoint = 0xffffffff;
        }
        if (!WPA_FileReq(IDParam, (u8)Status, (u32)FileReadPoint,(u16)FileBlockSize))
        {
            WMMP_DBG_TEMP("m2mfile_set_hdlr WPA_FileReq fail"); 			
            simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_PARAMETER_WRONG);
            simcom_at_output_default_end();
        }
    }
    else
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_PARAMETER_WRONG);
    }
    simcom_at_output_default_end();
}
/*断点续传方式接收新版本程序  ，远程升级版本，需支持 fota功能，目前暂不支持*/
void m2madfile_set_hdlr(void *data)
{
    bool result=true,rett;
    u16 TagIdParam = 0x1009;
    TLVRetVal_t value;
    s32 adFileParam = 0;
    char *temp=NULL;    
    u8 IDParam_len,i;
    char IDParam_t[9];
    char IDParam[4];
    s32 Status;
    u32 FileReadPoint;
    s32 FileBlockSize;
    WmmpContext_t *mainContext_p = ptrToWmmpContext();
    WmmpUpdateContext_t  *UpdateContext_p = &(mainContext_p->UpdateContext);
    StatusNumber_t *WmmpStatus_p = WPA_GetWmmpStatus();

    if(EAT_APR_OK != simcom_at_parser_get_u32(0, 0, &adFileParam))
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_PARAMETER_WRONG);
        simcom_at_output_default_end();
        return;
    }
    WMMP_DBG_TEMP("vgM2MADFILE adFileParam %d ",adFileParam); 					
    WMMP_DBG_TEMP("m2madFile UpdateAffairTransId 0x%08x",mainContext_p->UpdateContext.UpdateAffairTransId);

    WPA_ReadTLV(TagIdParam,&value);
    WMMP_DBG_TEMP("m2madFile value.TLVType=%d,value.length=%d, value.addr=%s",value.TLVType,value.length, value.addr);

    if(TLV_INT32 == value.TLVType)
    {
        u32 tmpVal = 0;
        u8 tmpVal2 = 0;

        temp = (char*)WPI_Malloc(value.length*2+1);
        memset(temp,0,value.length*2+1);							
        memcpy(&tmpVal,value.addr,4);
        tmpVal = soc_htonl(tmpVal);              								
        WMMP_DBG_TEMP("tmpVal is %04X",tmpVal);								
        if(value.length)
        {
            StringToHexCode((char*)&tmpVal, temp, value.length);
            IDParam_len = strlen(temp);
            memset(IDParam_t,0,sizeof(IDParam_t));
            memcpy(IDParam_t,temp,8);
            WPI_Free(temp);
            memset(IDParam,0,4);
            memset(UpdateContext_p->TransId,0,4);
            for(i=0;i<IDParam_len/2;i++)
            {
                getHexaValue(IDParam_t[i*2],&tmpVal2);
                IDParam[i]=tmpVal2<<4;
                UpdateContext_p->TransId[i] = tmpVal2<<4;
                getHexaValue(IDParam_t[i*2+1],&tmpVal2);
                IDParam[i]+=tmpVal2;
                UpdateContext_p->TransId[i] += tmpVal2;
                WMMP_DBG_TEMP("m2madFile IDParam[%d] 0x%x TransId[%d] 0x%x",i,IDParam[i],i,UpdateContext_p->TransId[i]);
            }
        }
        else
        {
            WPI_Free(temp);
            result = false;
        }
        WMMP_DBG_TEMP("afFile FileReq TransId:%x,%x,%x,%x",UpdateContext_p->TransId[0],UpdateContext_p->TransId[1],UpdateContext_p->TransId[2],UpdateContext_p->TransId[3]);
        if(!UpdateContext_p->adFileFileReadPoint)
        {
            WMMP_DBG_TEMP("afFile start Erase Flash");
            //rett = fotaSysFlashErase();
            WMMP_DBG_TEMP("afFile end Erase Flash rett %d",rett);
            if(!rett)
            {
                WMMP_DBG_TEMP("m2madFile fotaSysFlashErase failed 00000");
                result = false;
            }
            else
            {
                UpdateContext_p->adFileFlag = 1;	
                simcom_at_output("$M2MMSG:40,5829,%d,0/%d,10",UpdateContext_p->adFileStatus,UpdateContext_p->UpdateFileLength);
            }
        }

        WMMP_DBG_TEMP("adFile FileReq:%x,%x,%x,%x,adFileStatus %d,adFileFileReadPoint %d,adFileFlag %d",IDParam[0],IDParam[1],IDParam[2],IDParam[3],(u8)UpdateContext_p->adFileStatus, UpdateContext_p->adFileFileReadPoint,UpdateContext_p->adFileFlag);
        if(UpdateContext_p->UpdateFileLength > UpdateContext_p->adFileFileReadPoint)
        {
            FileBlockSize = UpdateContext_p->UpdateFileLength - UpdateContext_p->adFileFileReadPoint;
            WMMP_DBG_TEMP("m2madFile FileBlockSize %d",FileBlockSize);
            if(FileBlockSize > MAX_BLOCK_SIZE_WMMP_FILEDL)
            {
                FileBlockSize = MAX_BLOCK_SIZE_WMMP_FILEDL;
            }
            //wmmp2_20WriteUpdateFile();
            rett = WPA_adFileReq(UpdateContext_p->TransId, (u8)UpdateContext_p->adFileStatus, (u32)UpdateContext_p->adFileFileReadPoint,(u16)FileBlockSize);
            if(!rett)
            {
                WMMP_DBG_TEMP("m2madFile failed 1111");
                result = false;
            }
        }
        else
        {
            UpdateContext_p->adFileStatus = 2;
            UpdateContext_p->adFileFileReadPoint = 0xFFFFFFFF;
            //UpdateContext_p->UpdateFileLength = 0;
            FileBlockSize = 0;

            rett = WPA_adFileReq(UpdateContext_p->TransId, UpdateContext_p->adFileStatus, (u32)UpdateContext_p->adFileFileReadPoint,(u16)FileBlockSize);
            if(!rett)
            {
                WMMP_DBG_TEMP("m2madFile failed 22222");
                result = false;
            }
            else
            {	
                WMMP_DBG_TEMP("vgM2MADFILE getExtendedParameter adFileParam %d OK",adFileParam); 
            }
        }						

    }
    else
    {
        result = false;
    }							
    if(result==false)
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_PARAMETER_WRONG);
        simcom_at_output_default_end();
        return;
    }
}

/*****************************************************************************
 * 函数名: m2minit_active_hdlr
 * 作  者:  zhengwei
 * 功能描述: M2MINIT ACTION处理
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2minit_active_hdlr(void *pPfData)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bool rett=false;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    WMMP_DBG_TEMP("m2minit_active_hdlr gwopenValue: %d",gwopenValue);

    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    rett = WPA_Init();
    if(rett)
    {
        WMMP_DBG_TEMP("OPERATION_EXECUTE_TYPE WPA_Init() ok\n");
    }
    else
    {
        WMMP_DBG_TEMP("OPERATION_EXECUTE_TYPE WPA_Init() err\n");			
        simcom_at_output_cr_lf();
        simcom_at_output("$M2MINIT:ALREADY DONE");
        simcom_at_output_cr_lf();
    }	


    simcom_at_output_default_end();
}
/*****************************************************************************
 * 函数名: m2mterm_active_hdlr
 * 作  者:  zhengwei
 * 功能描述: M2MATERM ACTION处理
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2mterm_active_hdlr(void *pPfData)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bool rett=false;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    WMMP_DBG_TEMP("m2minit_active_hdlr gwopenValue: %d",gwopenValue);

    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    if(rett)
    {
        WMMP_DBG_TEMP("OPERATION_EXECUTE_TYPE WPA_Term() ok\n");
    }
    else
    {

        WMMP_DBG_TEMP("OPERATION_EXECUTE_TYPE WPA_Term() err\n");			
        simcom_at_output_cr_lf();
        simcom_at_output( "$M2MTERM:ALREADY DONE");
        simcom_at_output_cr_lf();		
    }

    simcom_at_output_default_end();	
}
/*****************************************************************************
 * 函数名: m2mlogin_active_hdlr
 * 作  者:  zhengwei
 * 功能描述: M2MLOGIN ACTION处理
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2mlogin_active_hdlr(void *pPfData)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bool rett=false;
    StatusNumber_t *WmmpStatus_p;
    WmmpStatus_p = (StatusNumber_t *)WPA_GetWmmpStatus(); 

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    WMMP_DBG_TEMP("m2minit_active_hdlr gwopenValue: %d",gwopenValue);

    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    WmmpStatus_p = (StatusNumber_t *)WPA_GetWmmpStatus();   	

    simcom_at_output_cr_lf();
    if(LOGINOK==WmmpStatus_p->protocol_status)
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_REPEAT);
    }
    else if(WMMP_STATE_NOTINIT==WmmpStatus_p->protocol_status  )
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_UNINIT_WMMP);
    }else
    {
        simcom_at_output( "$M2MMSG:START TO LOGIN ...");
        rett = WPA_LoginReq();
        if(rett)
        {
            WMMP_DBG_TEMP("OPERATION_EXECUTE_TYPE WPA_LoginReq ok\n");	
        }
        else
        {
            WMMP_DBG_TEMP("OPERATION_EXECUTE_TYPE WPA_LoginReq err\n");
            simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        }
    }

    simcom_at_output_default_end();
}
/*****************************************************************************
 * 函数名: m2mlogout_range_hdlr
 * 作  者:  zhengwei
 * 功能描述: M2MLOGOUT RANGE处理
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2mlogout_range_hdlr(void *pPfData)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    simcom_at_output_cr_lf();
    simcom_at_output("$M2MLOGOUT: (0-255)");	
    simcom_at_output_cr_lf();
    simcom_at_output_default_end();
}
/*****************************************************************************
 * 函数名: m2mlogout_assgin_hdlr
 * 作  者:  zhengwei
 * 功能描述: M2MLOGOUT ASSIGN处理
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2mlogout_assgin_hdlr(void *pPfData)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bool rett=false;
    StatusNumber_t *WmmpStatus_p;
    u32 logoutPara;
    u8 reason;
    WmmpContext_t *mainContext_p = ptrToWmmpContext();
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    WmmpStatus_p = (StatusNumber_t *)WPA_GetWmmpStatus(); 

    WMMP_DBG_TEMP("vgM2MLOGOUT EXTENDED_ASSIGN ");
    if(WmmpStatus_p->protocol_status != LOGINOK)
    {
        simcom_at_output_cr_lf();                   
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_UNLOGIN);
    }
    else
    {
        if(EAT_APR_OK==simcom_at_parser_get_u32(0, ULONG_MAX, &logoutPara))
        {
            WMMP_DBG_TEMP("vgM2MLOGOUT LogoutParam %d",logoutPara);
            if(logoutPara == 1)	/*0x100c CurrentFileRev*/
            {
                WMMP_DBG_TEMP("vgM2MLOGOUT s StatusContext.Majorver %d StatusContext.MinorVer %d UpdateContext.UpdateFileMajorRev %d UpdateContext.UpdateFileMinorRev %d",mainContext_p->StatusContext.Majorver,mainContext_p->StatusContext.MinorVer,mainContext_p->UpdateContext.UpdateFileMajorRev,mainContext_p->UpdateContext.UpdateFileMinorRev);
                WMMP_DBG_TEMP("vgM2MLOGOUT be UpdateContext.CurrentFileRev %s",mainContext_p->UpdateContext.CurrentFileRev);
                WMMP_DBG_TEMP("vgM2MLOGOUT be StatusContext.FirmwareRev %s",mainContext_p->StatusContext.FirmwareRev);
                WMMP_DBG_TEMP("vgM2MLOGOUT be UpdateContext.UpdateFileRev %s",mainContext_p->UpdateContext.UpdateFileRev);

                //if((2 == mainContext_p->UpdateContext.adFileStatus) && ((mainContext_p->UpdateContext.UpdateFileMajorRev > mainContext_p->StatusContext.Majorver) || ((mainContext_p->UpdateContext.UpdateFileMajorRev >= mainContext_p->StatusContext.Majorver) && (mainContext_p->UpdateContext.UpdateFileMinorRev > mainContext_p->StatusContext.MinorVer))))
                if(2 == mainContext_p->UpdateContext.adFileStatus)
                {						
                    mainContext_p->UpdateContext.adFileStatus = 0;						
                    mainContext_p->StatusContext.Majorver = mainContext_p->UpdateContext.UpdateFileMajorRev;
                    mainContext_p->StatusContext.MinorVer = mainContext_p->UpdateContext.UpdateFileMinorRev;						
                    memset(mainContext_p->StatusContext.FirmwareRev,0x00,sizeof(mainContext_p->StatusContext.FirmwareRev));
                    sprintf(mainContext_p->StatusContext.FirmwareRev,"%d.%02d",mainContext_p->UpdateContext.UpdateFileMajorRev,mainContext_p->UpdateContext.UpdateFileMinorRev);						
                    memset(mainContext_p->UpdateContext.CurrentFileRev,0x00,sizeof(mainContext_p->UpdateContext.CurrentFileRev));/*tlv 0x100c*/
                    sprintf(mainContext_p->UpdateContext.CurrentFileRev,"%d.%02d",mainContext_p->UpdateContext.UpdateFileMajorRev,mainContext_p->UpdateContext.UpdateFileMinorRev);
                    WMMP_DBG_TEMP("vgM2MLOGOUT af UpdateContext.CurrentFileRev %s",mainContext_p->UpdateContext.CurrentFileRev);
                    WMMP_DBG_TEMP("vgM2MLOGOUT af StatusContext.FirmwareRev %s",mainContext_p->StatusContext.FirmwareRev);						
                    WMMP_DBG_TEMP("vgM2MLOGOUT s fotaSysSetHandoff");	
                    WMMP_DBG_TEMP("vgM2MLOGOUT e fotaSysSetHandoff");
                }
                WPA_update_app(1);
            }
            else if((logoutPara >= 0)&&(logoutPara <= 255))
            {
                reason = (u8)logoutPara;                    

                rett = WPA_LogoutReq(reason);
                if(rett)
                {
                    simcom_at_output_cr_lf();
                    simcom_at_output( "$M2MLOGOUT: START TO LOGOUT ...reason =%d",reason);
                }
                else
                {
                    simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
                }
            }
            else
            {
                WMMP_DBG_TEMP("vgM2MLOGOUT 2 LogoutParam %d",logoutPara);
                simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
            }

        }
        else
        {
            WMMP_DBG_TEMP("vgM2MLOGOUT 3 LogoutParam %d",logoutPara);
            simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        }	
    }

    simcom_at_output_default_end();
}
/*****************************************************************************
 * 函数名: m2mterm_active_hdlr
 * 作  者:  zhengwei
 * 功能描述: M2MLOGOUT CTION处理
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2mlogout_active_hdlr(void *pPfData)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bool rett=false;
    StatusNumber_t *WmmpStatus_p;
    WmmpStatus_p = (StatusNumber_t *)WPA_GetWmmpStatus(); 

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    if(WmmpStatus_p->protocol_status != LOGINOK)
    {
        simcom_at_output_cr_lf();
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_UNLOGIN);
    }
    else
    {
        rett = WPA_LogoutReq(0);
        if(rett)
        {
            simcom_at_output_cr_lf();
            simcom_at_output( "$M2MLOGOUT: START TO LOGOUT ...reason =%d",0);
        }
        else
        {
            simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        }
    }

    simcom_at_output_default_end();
}
/*****************************************************************************
 * 函数名: m2msecurity_rang_hdlr
 * 作  者:  zhengwei
 * 功能描述: M2MSECURITY RANG处理
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2msecurity_rang_hdlr(void *pPfData)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    simcom_at_output_cr_lf();			
    simcom_at_output( "$M2MSECURITY: (0-255)");
    simcom_at_output_cr_lf();			
    simcom_at_output_default_end();
}
/*****************************************************************************
 * 函数名: m2msecurity_assign_hdlr
 * 作  者:  zhengwei
 * 功能描述: M2MSECURITY ASSIGN处理
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2msecurity_assign_hdlr(void *pPfData)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    u32     SecurityParam = 0;
    StatusNumber_t *WmmpStatus_p;
    WmmpStatus_p = (StatusNumber_t *)WPA_GetWmmpStatus();  

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    if(EAT_APR_OK==simcom_at_parser_get_u32(0, ULONG_MAX,&SecurityParam))
    {
        WMMP_DBG_TEMP("vgM2MCSECURITY WmmpStatus_p->protocol_status %x ",WmmpStatus_p->protocol_status);
        if(WmmpStatus_p->protocol_status!=LOGINOK)
        {	
            if((SecurityParam >= 0)&&(SecurityParam <= 255))
            {	
                if(WPA_ClearSecurity((u8)SecurityParam) == true)						
                {
                    WMMP_DBG_TEMP(" vgM2MCSECURITY OPERATION_EXECUTE_TYPE WPA_ClearSecurity() ok\n");
                }
                else
                {
                    WMMP_DBG_TEMP("vgM2MCSECURITY OPERATION_EXECUTE_TYPE WPA_ClearSecurity() err\n");							
                }				
            }
        }
        else
        {
            simcom_at_output_cr_lf();	                   
            simcom_at_output_set_result(EAT_AER_ERROR, EAT_WMMP_LOGIN);
        }

    }
    else
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
    }

    simcom_at_output_default_end();
}
/*****************************************************************************
 * 函数名: m2mstatus_rang_hdlr
 * 作  者:  zhengwei
 * 功能描述: M2MSTATUS RANGE处理
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2mstatus_rang_hdlr(void *pPfData)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    simcom_at_output_cr_lf();		
    simcom_at_output("$M2MSTATUS: (0-3)");
    simcom_at_output_cr_lf();		
    simcom_at_output_default_end();

}
/*****************************************************************************
 * 函数名: m2mstatus_query_hdlr
 * 作  者:  zhengwei
 * 功能描述: M2MSTATUS QUERY处理
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2mstatus_query_hdlr(void *pPfData)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    simcom_at_output_cr_lf();		

    switch(m2mStatusParam)
    {
        case PROTOCOL_STATUS:
            {
                simcom_at_output("WMMP PROTOCOL STATUS");
            }
            break;

        case NET_STATUS:
            {
                simcom_at_output("GPRS NET STATUS");
            }
            break;

        case PARAM_STATUS:
            {
                simcom_at_output("PARAM STATUS");
            }
            break;

        case APP_STATUS:
            {
                simcom_at_output("APP SERVER STATUS");
            }
            break;

        default:
            {
            }
            break;
    }

    simcom_at_output_default_end();
}
/*****************************************************************************
 * 函数名: m2mstatus_assign_hdlr
 * 作  者:  zhengwei
 * 功能描述: M2MSTATUS ASSIGN处理
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2mstatus_assign_hdlr(void *pPfData)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    u32 StatusParam;
    StatusNumber_t *WmmpStatus_p = NULL;
    char str[250];

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    WmmpStatus_p = (StatusNumber_t *)WPA_GetWmmpStatus();   

    if(EAT_APR_OK==simcom_at_parser_get_u32(0,ULONG_MAX,&StatusParam) )
    {
        if((StatusParam >= 0)&&(StatusParam <= 3))
        {                        
            switch(StatusParam)
            {
                case NET_STATUS:
                    {
                        simcom_at_output_cr_lf();	
                        PrintCurrentNetStatus(WmmpStatus_p->net_status, str);
                        simcom_at_output("$M2MSTATUS: %d,%d,%s",StatusParam,WmmpStatus_p->net_status,str);
                    }
                    break;

                case PROTOCOL_STATUS:
                    {
                        simcom_at_output_cr_lf();	
                        PrintCurrentProtocolStatus(WmmpStatus_p->protocol_status, str);
                        simcom_at_output("$M2MSTATUS: %d,%04X,%s",StatusParam,WmmpStatus_p->protocol_status,str);
                    }							
                    break;

                case PARAM_STATUS:
                    {
                        simcom_at_output_cr_lf();	
                        PrintCurrentParamStatus(WmmpStatus_p->param_status, str);
                        simcom_at_output("$M2MSTATUS: %d,%d,%s",StatusParam,WmmpStatus_p->param_status,str);
                    }							
                    break;

                case APP_STATUS:
                    {
                        simcom_at_output_cr_lf();	
                        PrintCurrentAppStatus(WmmpStatus_p->app_status[currAppNum], str);/*SIMCOM luyong 2012-07-13 Fix MKBug00013342  modify */
                        simcom_at_output("$M2MSTATUS: %d,%d,%s",StatusParam,WmmpStatus_p->app_status[currAppNum],str);/*SIMCOM luyong 2012-07-13 Fix MKBug00013342  modify */
                    }							
                    break;

                default:
                    break;
            }
        }
        else
        {
            WMMP_DBG_TEMP("vgM2MCSTATUS StatusParam %d ",StatusParam);
            simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        }
    }
    else
    {
        WMMP_DBG_TEMP("vgM2MCSTATUS 2 StatusParam %d ",StatusParam);
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
    }

    simcom_at_output_default_end();			
}
/*****************************************************************************
 * 函数名: m2mstatus_active_hdlr
 * 作  者:  zhengwei
 * 功能描述: M2MSTATUS ACTION处理
 * 输入参数说明: -
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void m2mstatus_active_hdlr(void *pPfData)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    StatusNumber_t *WmmpStatus_p = NULL;
    char str[250];
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    WmmpStatus_p = (StatusNumber_t *)WPA_GetWmmpStatus();

    if(!sincom_wmmp_wopen())
    {
        simcom_at_output_set_result(EAT_AER_ERROR, EAT_CME_OPERATION_NOT_ALLOWED_ERR);
        simcom_at_output_default_end();
        return;
    }

    WMMP_DBG_TEMP("vgM2MCSTATUS m2mStatusParam %d",m2mStatusParam);

    simcom_at_output_cr_lf();	

    WMMP_DBG_TEMP("vgM2MCSTATUS net_status %d gM2MStatus %d",WmmpStatus_p->net_status,gM2MStatus);

    if(LINK_OK == WmmpStatus_p->net_status)
    {
        PrintCurrentProtocolStatus(WmmpStatus_p->protocol_status,str);										
        simcom_at_output("$M2MSTATUS: %04X,%s",WmmpStatus_p->protocol_status,str);	  
    }
    else
    {
        PrintCurrentProtocolStatus(gM2MStatus,str);										
        simcom_at_output("$M2MSTATUS: %04X,%s",gM2MStatus,str);	  
    }

    simcom_at_output_cr_lf();	

    simcom_at_output_default_end();			
}
void WmmpCustomer_AddCRLF(char **pp_PointerAdress)
{  
    (*pp_PointerAdress) [0]= 0x0D;
    (*pp_PointerAdress) [1]= 0x0A;

    (*pp_PointerAdress) [2]=0;  
    (*pp_PointerAdress)+=2;
}           
void WmmpAppSendDataCallback(u32 sizeProcessed)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    u8  buffer[256] = {0};
    u8 *pl_StrPtr = NULL;
    u16 src_id;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    pl_StrPtr=buffer;
    src_id=simcom_at_parser_get_src_id();

    if(sizeProcessed==0xFFFFFFFF)/*失败*/
    {
        pl_StrPtr += sprintf(pl_StrPtr, "ERROR");			
        WmmpCustomer_AddCRLF(&pl_StrPtr);
    }
    else
    {
        pl_StrPtr += sprintf(pl_StrPtr, "OK");
        WmmpCustomer_AddCRLF(&pl_StrPtr);
        pl_StrPtr += sprintf(pl_StrPtr, WMMP_AT_EVENT_ACK_STR"%d\r\n",appTransaction_Id[currAppNum]);
        pl_StrPtr += sprintf(pl_StrPtr, "%d,1,\"ok\",%d,%d\r\nOK\r\n",currAppNum,WMMP_MAX_NET_DATA_SIZE ,MAX_SEND_BSD_LEN);
    }

    simcom_at_output_async(src_id, buffer, strlen(buffer), true);   
}
void WmmpProcessAtSendData(char *atData,u16 atLen)
{
    char *decodeString_p = NULL;  
    u16 *TLVtag = NULL;
    u8 TLVnum = 0;	 
    bool rett = true;
    bool result = true;
    char *TLVData = NULL; 
    u8  buffer[256] = {0};
    u8 *pl_StrPtr = NULL;

    pl_StrPtr=buffer;
    decodeString_p = atData;

    WMMP_DBG_TEMP( "WmmpProcessAtSendData:  simcom_m2m_at_type=%d,g_src_id=%d",simcom_m2m_at_type,g_src_id);
    WMMP_DBG_TEMP( "WmmpProcessAtSendData:  bhvValue[2]=%d,bhvValue[4]=%d",bhvValue[2],bhvValue[4]);

    if(simcom_m2m_at_type == VG_AT_M2MC_CFG)	 
    {		 
        TLVtag = (u16*)WPI_Malloc(simcom_m2m_at_len*2);
        memset(TLVtag,0,simcom_m2m_at_len*2);

        if(bhvValue[2] == 1)  
        {
            rett = WPA_WcomParseCfgData(decodeString_p,atLen,(u16 *)(&TLVnum),TLVtag);
            if(!rett)
            {
                WPI_Free(TLVtag);				
                pl_StrPtr += sprintf(pl_StrPtr, "ERROR");
                simcom_at_output_async(g_src_id, buffer, strlen(buffer), true);
                simcom_m2m_data_len=0;
                //memset(g_simcom_m2m_at_buff, 0, 300*1024);
                return;
            }
            else
            {
                WPI_Free(TLVtag);				
                WriteAllTLVParam2Flash();
                pl_StrPtr += sprintf(pl_StrPtr, "OK");
                simcom_at_output_async(g_src_id, buffer, strlen(buffer), true); 
            }
        }
        else
        {
            rett = WPA_ParseTrapDataUpdate(decodeString_p,atLen,(u16 *)(&TLVnum),TLVtag);
            WMMP_DBG_TEMP( "WmmpProcessAtSendData:M2MC_CFG-->rett=%d",rett);
            if(!rett)
            {
                WPI_Free(TLVtag);				
                pl_StrPtr += sprintf(pl_StrPtr, "ERROR");
                simcom_at_output_async(g_src_id, buffer, strlen(buffer), true); 
                simcom_m2m_data_len=0;
                //memset(g_simcom_m2m_at_buff, 0, 300*1024);
                return;
            }
            else
            {
                WPI_Free(TLVtag);				
                WriteAllTLVParam2Flash();
                pl_StrPtr += sprintf(pl_StrPtr, "OK");
                simcom_at_output_async(g_src_id, buffer, strlen(buffer), true); 
            }
        }
    }
    else if(simcom_m2m_at_type == VG_AT_M2MC_TRAP)
    {
        TLVtag = (u16*)WPI_Malloc(simcom_m2m_at_len*2);
        memset(TLVtag,0,simcom_m2m_at_len*2);

        if(bhvValue[2] == 1)	
        {			  
            rett = WPA_WcomParseCfgData(decodeString_p,atLen,(u16 *)(&TLVnum),TLVtag);
            WMMP_DBG_TEMP( "WmmpProcessAtSendData:M2MC_TRAP1 rett=%d ,TLVnum=%02x,TLVtag=%s",rett,TLVnum,TLVtag);

            if(!rett)
            {
                WPI_Free(TLVtag);					 
                pl_StrPtr += sprintf(pl_StrPtr, "ERROR");
                simcom_at_output_async(g_src_id, buffer, strlen(buffer), true); 
                simcom_m2m_data_len=0;
                //memset(g_simcom_m2m_at_buff, 0, 300*1024);
                return;
            }
            else
            {
                rett = WPA_TrapReq(TLVtag, (u16 )TLVnum);							 
                WMMP_DBG_TEMP( "WmmpProcessAtSendData:WPA_TrapReq1->rett=%d ",rett);

                if (rett)
                {	  
                    WriteAllTLVParam2Flash();
                    Transaction_Id++;
                    pl_StrPtr += sprintf(pl_StrPtr, "$M2MTRAP:ACK:%d",Transaction_Id);
                    WmmpCustomer_AddCRLF(&pl_StrPtr);
                    pl_StrPtr += sprintf(pl_StrPtr, "OK");
                }
                else
                {
                    WPI_Free(TLVtag);
                    pl_StrPtr += sprintf(pl_StrPtr, "ERROR");
                }
                simcom_at_output_async(g_src_id, buffer, strlen(buffer), true); 
            }
        }
        else
        {
            rett= WPA_ParseTrapDataUpdate(decodeString_p,atLen,(u16 *)(&TLVnum),TLVtag);
            WMMP_DBG_TEMP( "WmmpProcessAtSendData:M2MC_TRAP2 rett=%d ,TLVnum=%02x,TLVtag=%s",rett,TLVnum,TLVtag);
            if(!rett)
            {
                WPI_Free(TLVtag);					 
                pl_StrPtr += sprintf(pl_StrPtr, "ERROR");
                simcom_at_output_async(g_src_id, buffer, strlen(buffer), true); 
                simcom_m2m_data_len=0;
                //memset(g_simcom_m2m_at_buff, 0, 300*1024);
                return;
            }
            else
            {
                rett = WPA_TrapReq(TLVtag, (u16 )TLVnum);							 
                WMMP_DBG_TEMP( "WmmpProcessAtSendData:WPA_TrapReq2->rett=%d ",rett);
                if (rett)
                {	  
                    WriteAllTLVParam2Flash();
                    pl_StrPtr += sprintf(pl_StrPtr, "$M2MTRAP: %d",Transaction_Id);
                    WmmpCustomer_AddCRLF(&pl_StrPtr);
                    pl_StrPtr += sprintf(pl_StrPtr, "OK"); 				 
                }
                else
                {
                    WPI_Free(TLVtag);
                    pl_StrPtr += sprintf(pl_StrPtr, "ERROR");
                }
                simcom_at_output_async(g_src_id, buffer, strlen(buffer), true); 
            }
        }
    }
    else if(simcom_m2m_at_type == VG_AT_M2MC_DATA)
    {
        u32 i = 0;
        char *sendata = NULL;

        if(bhvValue[4] == 1)
        {					
            rett = WPA_DataReq(Data_type[0], decodeString_p, atLen, Data_destination);
            WMMP_DBG_TEMP( "WmmpProcessAtSendData:M2MC_DATA->rett=%d ",rett);
            if (rett)
            {			   
                pl_StrPtr += sprintf(pl_StrPtr, "$M2MTDATA: %d",Transaction_Id);
                WmmpCustomer_AddCRLF(&pl_StrPtr);
                pl_StrPtr += sprintf(pl_StrPtr, "OK");
                WmmpCustomer_AddCRLF(&pl_StrPtr);
                pl_StrPtr += sprintf(pl_StrPtr, "$M2MACK: %d",Transaction_Id);
                WmmpCustomer_AddCRLF(&pl_StrPtr);
                pl_StrPtr += sprintf(pl_StrPtr, "OK");
            }
            else
            {
                pl_StrPtr += sprintf(pl_StrPtr, "ERROR");
            }
            simcom_at_output_async(g_src_id, buffer, strlen(buffer), true); 
        }
        else
        {
            sendata = (char*)WPI_Malloc(atLen/2); 				
            memset(sendata,0,atLen/2);

            for(i=0;i<atLen/2;i++)
            {							 
                result=Char2Bin8((decodeString_p+i*2),&sendata[i]);
                if(!result)			 
                {	 
                    WMMP_DBG_TEMP("WmmpProcessAtSendData result=%d ",result);  
                    WPI_Free(sendata);
                    pl_StrPtr += sprintf(pl_StrPtr, "ERROR");			 
                    simcom_at_output_async(g_src_id, buffer, strlen(buffer), true); 
                    simcom_m2m_data_len=0;
                    //memset(g_simcom_m2m_at_buff, 0, 300*1024);
                    return;
                }
            }		 

            rett = WPA_DataReq(Data_type[0], sendata, atLen/2, Data_destination);
            WMMP_DBG_TEMP("WmmpProcessAtSendData rett=%d ",rett);  
            if (rett)
            {							 
                pl_StrPtr += sprintf(pl_StrPtr, "$M2MTDATA: %d",Transaction_Id);
                WmmpCustomer_AddCRLF(&pl_StrPtr);
                pl_StrPtr += sprintf(pl_StrPtr, "OK");
                WmmpCustomer_AddCRLF(&pl_StrPtr);
                pl_StrPtr += sprintf(pl_StrPtr, "$M2MACK: %d",Transaction_Id);
                WmmpCustomer_AddCRLF(&pl_StrPtr);
                pl_StrPtr += sprintf(pl_StrPtr, "OK");
            }
            else
            {
                WPI_Free(sendata);
                pl_StrPtr += sprintf(pl_StrPtr, "ERROR");
            }
            simcom_at_output_async(g_src_id, buffer, strlen(buffer), true); 
        }
    }	 
    else if(simcom_m2m_at_type == VG_AT_M2MC_MAPPSEND)
    {
        u32 i = 0;
        char *sendata = NULL;
        WMMP_DBG_TEMP("processWMMPAtSendData bhvValue[4]=%d ",bhvValue[4]);  
        if(bhvValue[4] == 0)
        {
            //HEX发送,需要把得到的ascii数据转换为hex，例如13转换为0x13
            sendata = (char*)WPI_Malloc(atLen/2);
            if( sendata == NULL)
            {
                WMMP_DBG_TEMP("(u16*)WPI_Malloc size %d return NULL",atLen/2);  
                pl_StrPtr += sprintf(pl_StrPtr, "ERROR");			
                WmmpCustomer_AddCRLF(&pl_StrPtr);
                simcom_at_output_async(g_src_id, buffer, strlen(buffer), true);
                simcom_m2m_data_len=0;
                return ;
            }
            memset(sendata,0,atLen/2);

            for(i=0;i<atLen/2;i++)
            {
                result=Char2Bin8((decodeString_p+i*2),&sendata[i]);
                if(!result)				
                {	
                    WMMP_DBG_TEMP("Char2Bin8 %d return error",i);  
                    WPI_Free(sendata);
                    pl_StrPtr += sprintf(pl_StrPtr, "ERROR");			
                    WmmpCustomer_AddCRLF(&pl_StrPtr);
                    simcom_at_output_async(g_src_id, buffer, strlen(buffer), true);
                    simcom_m2m_data_len=0;
                    //kal_mem_set(g_simcom_m2m_at_buff, 0, 300*1024);
                    return;
                }
            }
            decodeString_p = sendata;
            atLen = atLen/2;
        }

        appTransaction_Id[currAppNum] = WPI_rand();
        pl_StrPtr += sprintf(pl_StrPtr, "$M2MAPPSEND: %d",appTransaction_Id[currAppNum]);
        WmmpCustomer_AddCRLF(&pl_StrPtr);
        rett = WPA_AppSendReq(currAppNum, decodeString_p, atLen,WmmpAppSendDataCallback);
        if(bhvValue[4] == 0)
        {
            WPI_Free(sendata);
        }
    }
    else
    {
        pl_StrPtr += sprintf(pl_StrPtr, "ERROR");			 
        simcom_at_output_async(g_src_id, buffer, strlen(buffer), true); 
    }

    simcom_m2m_data_len=0;
    //memset(g_simcom_m2m_at_buff, 0, 300*1024);
    return;
}

/*SIMCOM Laiwenjie 2013-01-29 Fix MKBug00015062 BEGIN*/
/*****************************************************************************
 * 函数名: simcom_wmmp_init_open_status
 * 作  者:  Laiwenjie
 * 功能描述: 读取WMMP 开关状态的nvram value
 * 输入参数说明: 
 * 输出参数说明: -
 * 返回值说明: -
 *****************************************************************************/
void simcom_wmmp_init_open_status()
{
    Wmmp_atc_value_struct wmmpopenInfo = {0};
    if(simcom_read_nvram_data(NVRAM_EF_WMMP_WOPEN_LID, &wmmpopenInfo, sizeof(Wmmp_atc_value_struct), 1, 1))
    {
        gwopenValue = wmmpopenInfo.wopenValueNv;

        WMMP_DBG_TEMP("simcom_wmmp_init_open_status gwopenValue = %d\n",gwopenValue);
    }
}

/*SIMCOM Laiwenjie 2013-01-29 Fix MKBug00015062 END*/

static EAT_AT_REG_INFO g_atRegInfo[] = 
{
    /*at_name, pfTest =?, pfRead ?, pfSet =x , pfExe ,...*/
    {"AT$M2MAPPOPEN", m2mappopen_test_hdlr,  eat_at_mode_ok, m2mappopen_set_hdlr, eat_at_mode_err},
    {"AT$M2MAPPCLOSE", m2mappclose_test_hdlr, eat_at_mode_ok, m2mappclose_set_hdlr, eat_at_mode_err},
    {"AT$M2MAPPSEND", m2mappsend_test_hdlr,  eat_at_mode_ok, m2mappsend_set_hdlr, eat_at_mode_err},
    {"AT$M2MAPPSTATUS", m2mappstatus_test_hdlr, eat_at_mode_ok,  m2mstatus_set_hdlr, eat_at_mode_err},

    {"AT$M2MCGPADDR", eat_at_mode_err, eat_at_mode_err, eat_at_mode_err,  m2mcgpaddr_exe_hdlr},

    {"AT$M2MAPPBROPEN", m2mappbropen_test_hdlr,  eat_at_mode_err, m2mappbropen_set_hdlr, eat_at_mode_err},
    {"AT$M2MAPPBRCLOSE", m2mappbrclose_test_hdlr,  eat_at_mode_err, m2mappbrclose_set_hdlr, eat_at_mode_err},
    {"AT$M2MAPPBRSTATUS", m2mappbrstatus_test_hdlr, eat_at_mode_err, m2mappbrstatus_set_hdlr, eat_at_mode_err},

    {"AT$M2MTLV", eat_at_mode_err, eat_at_mode_err, eat_at_mode_err, eat_at_mode_err},
    {"AT$M2MTEST", eat_at_mode_err, eat_at_mode_err, m2mtest_set_hdlr, eat_at_mode_err},
    {"AT$M2MPARACFG", eat_at_mode_err, m2mparacfg_read_hdlr, m2mparacfg_set_hdlr, eat_at_mode_err},
    {"AT$M2MDATATEST", eat_at_mode_err, eat_at_mode_err, m2mdatatest_set_hdlr, eat_at_mode_err},
    {"AT$M2MVER", eat_at_mode_err, eat_at_mode_err, eat_at_mode_err,m2mver_active_hdlr},
    {"AT$M2MALMRST", m2malmrst_test_hdlr, eat_at_mode_err, m2malmrst_set_hdlr, eat_at_mode_err},
    {"AT$M2MBHV", m2mbhv_test_hdlr, m2mbhv_read_hdlr, m2mbhv_set_hdlr, eat_at_mode_err},

    {"AT+WOPEN", wopen_test_hdlr, wopen_read_hdlr, wopen_set_hdlr, eat_at_mode_err},
    {"AT+WREBOOT", eat_at_mode_err, eat_at_mode_err, eat_at_mode_err, wreboot_active_hdlr},

    {"AT$M2MCFG", m2mcfg_test_hdlr,  m2mcfg_read_hdlr, m2mcfg_set_hdlr, eat_at_mode_err},
    {"AT$M2MCFGLIST", m2mcfglist_test_hdlr, eat_at_mode_err, m2mcfglist_set_hdlr, eat_at_mode_err},
    {"AT$M2MTRAP", m2mtrap_test_hdlr,  eat_at_mode_err, m2mtrap_set_hdlr, eat_at_mode_err},
    {"AT$M2MREQ", m2mreq_test_hdlr, eat_at_mode_err, m2mreq_set_hdlr, eat_at_mode_err},
    {"AT$M2MDATA", m2mdata_test_hdlr, eat_at_mode_err, m2mdata_set_hdlr, eat_at_mode_err},
    {"AT$M2MFILE", m2mfile_test_hdlr, eat_at_mode_err, m2mfile_set_hdlr, eat_at_mode_err},
    {"AT$M2MADFILE", eat_at_mode_err, eat_at_mode_err, m2madfile_set_hdlr, eat_at_mode_err},

    {"AT$M2MINIT", eat_at_mode_err, eat_at_mode_err, eat_at_mode_err, m2minit_active_hdlr},
    {"AT$M2MTERM", eat_at_mode_err, eat_at_mode_err, eat_at_mode_err, m2mterm_active_hdlr},
    {"AT$M2MLOGIN", eat_at_mode_err, eat_at_mode_err, eat_at_mode_err, m2mlogin_active_hdlr},
    {"AT$M2MLOGOUT", m2mlogout_range_hdlr, eat_at_mode_err, m2mlogout_assgin_hdlr, m2mlogout_active_hdlr},
    {"AT$M2MSECURITY", m2msecurity_rang_hdlr, eat_at_mode_ok, m2msecurity_assign_hdlr, eat_at_mode_ok},
    {"AT$M2MSTATUS", m2mstatus_rang_hdlr, m2mstatus_query_hdlr, m2mstatus_assign_hdlr, m2mstatus_active_hdlr},
};

u32 wmmp_at_init(void)
{
    u32 uRegCount = eat_customer_at_reg(g_atRegInfo, sizeof(g_atRegInfo)/sizeof(EAT_AT_REG_INFO));
    return uRegCount;

}

#endif
