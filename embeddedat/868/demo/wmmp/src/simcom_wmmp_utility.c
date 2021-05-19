/** \file
 *  \brief Library Support
 */
/*
******************************************************************************
*                 |
*  File Name      | simcomwmmp_utility.c
*-----------------|-----------------------------------------------------------
*  Project        | SIMCOM500W
*-----------------|-----------------------------------------------------------
*  Created        | 09 -- 14 , 2009
*-----------------|-----------------------------------------------------------
*  Description    | This file contains all the support functions for library.
*-----------------|-----------------------------------------------------------
*                 | Copyright (c) 2009 SIMCOM Corp.
*                 | All Rights Reserved.
*                 |
******************************************************************************
*/

/*************************************************************************
* Include Statements
 *************************************************************************/
//#include "nvram_user_defs.h"
#include "eat_type.h"
#include "stdarg.h"
#include "wmmp_typ.h"
#include "wmmp_api.h"
#include "wmmp_lib.h"
#include "wmmp_interface.h"
#include "simcom_wmmp_utility.h"
//#include "simcom_cmee_enum.h"
//#include "custom_config.h" //for MOD_SIMCOM_WMMP define

//#include "simcom_adapter_type.h" //for simcom_module_type 

static EatTimer_enum wmmp_core_timer_id = EAT_TIMER_1;

extern u8 simcom_m2m_at_len ;
#ifdef __SIMCOM_WMMP_DEBUG__

#define MAX_WMMP_TRACE_BUF_LEN 512
/**/
typedef enum{
    WMMP_TRACE_MODE_TRACER, //通过tracer 工具输出
    WMMP_TRACE_MODE_UART    //通过UART端口输出
}WMMP_TRACE_MODE;

typedef enum{
    WMMP_TRACE_EN_FUN, //显示函数名
    WMMP_TRACE_EN_FILE, //显示文件名
    WMMP_TRACE_EN_LINE //显示行号
}WMMP_TRACE_EN_EXTINFO;

typedef struct
{
    u32 level; //0:off 1:error 2:info
    u32 mode;  //0:tracer tool 1:uart
    u32 extinfo;  //1:func name 2:file 4:line
}WMMP_TRACE_CFG;

typedef struct 
{
    char* fun;  //函数名字符串
    char* file; //文件名字符串
    u32 line;   //行号
}WMMP_TRACE_PRE;

char wmmp_trace_buf[MAX_WMMP_TRACE_BUF_LEN]; //log buffer

/*trace 打印配置*/
WMMP_TRACE_CFG wmmp_trace_cfg = 
{  
    2,  //info level
    0,  //by tracer tool
    5}; //display func name and line
#endif

const unsigned char DefaultToAsciiArray[128] = 
{
    64, 163, 36, 165, 232, 233, 249, 236, 242, 199,
    10, 216, 248, 13, 197, 229, 16, 95, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 198, 230,
    223, 200, 32, 33, 34, 35, 164, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63, 161, 65, 66, 67, 68, 69,
    70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
    90, 196, 214, 209, 220, 167, 191, 97, 98, 99,
    100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 228, 246, 241, 252, 224
};


const unsigned char AsciiToDefaultArray[] = 
{
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    10, 32, 32, 13, 32, 32, 16, 32, 18, 19,
    20, 21, 22, 23, 24, 25, 26, 27, 32, 32,
    32, 32, 32, 33, 34, 35, 2, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
    50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
    60, 61, 62, 63, 0, 65, 66, 67, 68, 69,
    70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
    80, 81, 82, 83, 84, 85, 86, 87, 88, 89,
    90, 32, 32, 32, 32, 17, 32, 97, 98, 99,
    100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
    120, 121, 122, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 64, 32, 1, 36, 3, 32, 95, 32, 32,
    32, 32, 32, 64, 32, 32, 32, 32, 32, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
    32, 96, 32, 32, 32, 32, 91, 14, 28, 9,
    31, 32, 32, 32, 32, 32, 32, 32, 32, 93,
    32, 32, 32, 32, 92, 32, 11, 32, 32, 32,
    94, 32, 32, 30, 127, 32, 32, 32, 123, 15,
    29, 32, 4, 5, 32, 32, 7, 32, 32, 32,
    32, 125, 8, 32, 32, 32, 124, 32, 12, 6,
    32, 32, 126, 32, 32, 32
};

/*--------------------------------------------------------------------------
 *
 * Function:        vgAsciiToInt16
 *
 * Parameters:      hexString_p (in)   - String containing (hex) number in
 *                                       ascii to convert NULL TERMINATED!
 *
 * Returns:         u16 value of passed string.
 *
 * Description:     Converts acsii text to numerical value.
 *
 * Design spec:
 *
 *-------------------------------------------------------------------------*/
u16 vgAsciiToInt16(u8 *hexString_p)
{
  u16 result = 0, pos = 0;
  u8 value;

  while ((hexString_p[pos] != '\0') && (pos < 4))
  {
    /* Check whatever's entered is valid (i.e. hex).... */
    if (isxdigit(hexString_p[pos]))
    {
      /* Numerical (0-9).... */
      if (isdigit(hexString_p[pos]))
      {
        value = hexString_p[pos] - '0';
      }

      /* Or a-f.... */
      else
      {
        value = tolower(hexString_p[pos]);
        value -= 'a';
        value += 0x0a;
      }
    }
    else
    {
      value = 0;
    }

    /* Convert to 16-bit int.... */
    result = (result << 4) + value;
    pos++;
  }

  return result;
}

/*****************************************************************************
 * FUNCTION
 *  WmmpConvertAsciiEncodingToGSM7BitDefault
 * DESCRIPTION
 *  Converts  8 bit Ascii to 7 bit encoding
 * PARAMETERS
 *  message     [IN]        Output Buffer
 *  msg         [IN]        Input Buffer
 *  length      [IN]        No of bytes to be converted
 *  outLen      [?]         
 * RETURNS
 *  void
 *****************************************************************************/
void WmmpConvertAsciiEncodingToGSM7BitDefault(unsigned char  *message,unsigned char  *msg, unsigned short length, unsigned short  *outLen)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    unsigned short i, j;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0, j = 0; i < length; i++, j++)
    {
        message[j] = AsciiToDefaultArray[msg[i]];
    }
    if (outLen != NULL)
    {
        *outLen = j;
    }
}

WMMPAPNType wmmp_get_apn_type(void)
{
    WmmpCfgContext_t           *pt1 = NULL; 
    WMMPAPNType ret = WMMP_APN_UNKNOWN;

    pt1 = (WmmpCfgContext_t *)WPA_ptrToConfigTLV();

    WMMP_DBG_TEMP("wmmp_get_apn_type apn=%s",pt1->M2MAPN);

    toUpper(pt1->M2MAPN);
    
    if ( 0 == strcmp(pt1->M2MAPN, "CMNET") )
    {
        ret = WMMP_APN_CMNET;
    }
    else if ( 0 == strcmp(pt1->M2MAPN, "CMWAP") )
    {
        ret = WMMP_APN_CMWAP;    
    }
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_PRIVATE_APN__
    else
    {
        ret = WMMP_APN_PRIVATE;    
    }
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/
    return ret;
}      

/******************************************************************************
* Function:   wmmpGetSocketbyId    
* 
* Author:     bob.deng
* 
* Parameters:    
* 
* Return:     true: successful to find the socket
*             false: failed to find the socket
* 
* Description:Get the position of a socket context according to socketId, and
*             the position is defined by networkContext_p and socketIndex_p
******************************************************************************/
bool wmmpCheckSockIdValid(u8 socketId)
{
    u8 i, j;
    bool        bFind = false;

    WmmpContext_t  *mainContext_p = ptrToWmmpContext();

    WMMP_DBG_TEMP( "wmmpGetSocketbyId socketId=%d wmmpSocket =%d", socketId, mainContext_p->wmmpSocket);

    if(socketId == -1)
    {
        return false;
    }

    /*wmmp platform socket id*/
    if (mainContext_p->wmmpSocket == socketId)
    {
        bFind = true;
    }else
    {
        /*application server socket id*/
        for (i = 0; (i < MAX_WMMP_APP_NUM); i++)
        {
            if (mainContext_p->wmmpOpenSkt[i] == socketId)
            {
                bFind = true;
                break;
            }
        }
    }
	
    return bFind;

}

/*denglieman add 20120505  -s*/
void StringToHexCode(char* string, char* HexCode,u16 stringlen)
{

    u32 i;
    u16 loopi = 0;

    for(i=0;i<stringlen;i++)

    {
        //WMMP_DBG_TEMP("StringToHexCode  string[%d] 0x%x",i,*(string+i));
        sprintf((char *)(HexCode+i*2),"%02X",*(string+i));
        //WMMP_DBG_TEMP("StringToHexCode  HexCode[%d] %s",i,(char*)(HexCode+i*2));
    }

    HexCode[stringlen*2]=0;

}    

bool getHexaValue (char c, s8 *value)
{
  bool result = true;

  if ((c >= '0') && (c <= '9'))
  {
    *value = c - '0';
  }
  else
  {
    if ((c >= 'a') && (c <= 'f'))
    {
      *value = c - 'a' + 10;
    }
    else
    {
      if ((c >= 'A') && (c <= 'F'))
      {
        *value = c - 'A' + 10;
      }
      else
      {
        result = false;
      }
    }
  }

  return (result);
}

char OChar2Bin8(char *input)
 {   
     char    i1,i2;
     getHexaValue((char)(*input), (u8 *)&i1);
     i1 = (i1 << 4) & 0xF0;
     getHexaValue((char)(*(input+1)), (u8 *)&i2);
     i2 = i2 & 0x0F;
 
     i1 = i1 | i2;
  
     return(i1);
 }
 
  
 
 u16 OChar2Bin16(char *input)
 {
      u16 i1,i2;
 
      i1 = (u16)(OChar2Bin8(input));
      i2 = (u16)(OChar2Bin8(input+2));
      i1 = (i1 << 8) & 0xFF00;
      i2 = i2 & 0x00FF;
      i1 = i1 | i2;
      return(i1);
 
 }


         
 /* example:  "1F" ==> 0x1f */
 bool Char2Bin8(char *input,char *outCharHex)
{   
    bool ret = true;    
    char    i1,i2;

    //WMMP_DBG_TEMP(("Char2Bin8 Lchar 0x%x",*input));
    ret = getHexaValue((char)(*input),(u8*)( &i1));
    if(ret)
    {
        i1 = (i1 << 4) & 0xF0;
        //WMMP_DBG_TEMP(("Char2Bin8 LcharHex 0x%02x",i1));
    }
    else
    {
        //WMMP_DBG_TEMP(("Char2Bin8 Lchar illeagle input char ret ERROR "));
        i1 = (*input);
        //WMMP_DBG_TEMP(("Char2Bin8 Lchar i1 %d ",i1));
        i1 = (i1 << 4) & 0xF0;
        //WMMP_DBG_TEMP(("Char2Bin8 2 LcharHex 0x%02x",i1));
        outCharHex = NULL;
        return false;
    }
    //WMMP_DBG_TEMP(("Char2Bin8 Hchar 0x%x",*(input+1)));
    ret = getHexaValue((char)(*(input+1)), (u8*)(&i2));
    if(!ret)
    {
        //WMMP_DBG_TEMP(("Char2Bin8 Hchar illeagle input char ret ERROR "));
        i2 = (*(input+1));
        //WMMP_DBG_TEMP(("Char2Bin8 Hchar i2 %d ",i2));
        i2 = i2 & 0x0F;
        //WMMP_DBG_TEMP(("Char2Bin8 2 HcharHex 0x%02x",i2));
        outCharHex = NULL;
        return false;
    }
    else
    {
        i2 = i2 & 0x0F;
        //WMMP_DBG_TEMP(("Char2Bin8 HcharHex 0x%02x",i2));
    }
    i1 = i1 | i2;
    //WMMP_DBG_TEMP(("Char2Bin8 charHex 0x%02x",i1));
    *outCharHex = i1;
    //WMMP_DBG_TEMP(("Char2Bin8 outCharHex 0x%02x",*outCharHex));
    return true;
}
         
          
         /* example:  "1F" ==> 0x001f */
bool Char2Bin16(char *input,u16 *outWordHex)
{
    u16 i1,i2;
    bool ret = true;

    //WMMP_DBG_TEMP(("Char2Bin16 Lword 0x%x 0x%x",*input,*(input+1)));
    ret = Char2Bin8(input,(char *)(&i1));
    //WMMP_DBG_TEMP(("Char2Bin16 1 Lwordhex 0x%02x",i1));
    if(ret)
    {  
        i1 = (i1 << 8) & 0xFF00;
        //WMMP_DBG_TEMP(("Char2Bin16 2 Lwordhex 0x%02x",i1));
    }
    else
    {
        //WMMP_DBG_TEMP(("Char2Bin16 Lword illeagle input ret ERROR "));
        outWordHex = NULL;
        return false;
    }
    //WMMP_DBG_TEMP(("Char2Bin16 Hword 0x%x 0x%x",*(input+2),*(input+3)));
    ret = Char2Bin8((input+2),(char *)(&i2));
    //WMMP_DBG_TEMP(("Char2Bin16 1 Hwordhex 0x%02x",i2));
    if(ret)
    {  
        i2 = i2 & 0x00FF;
        //WMMP_DBG_TEMP(("Char2Bin16 2 Hwordhex 0x%02x",i2));
    }
    else
    {
        //WMMP_DBG_TEMP(("Char2Bin16 Hwordhex illeagle input ret ERROR "));
        outWordHex = NULL;
        return false;
    }

    i1 = i1 | i2;
    //WMMP_DBG_TEMP(("Char2Bin16 wordhex 0x%04x",i1));
    *outWordHex = i1;
    //WMMP_DBG_TEMP(("Char2Bin16 outWordHex 0x%04x",*outWordHex));
    return true;

}
         
         
         
void PrintCurrentAppStatus(AppStatus_e status,char* value)
{
    switch(status)
    {
        case APPLINK_OK:
            strcpy((char*)value, (char*) "\"connected\"");
            break;
        case APPLINK_FAILED:
            strcpy((char*)value, (char*) "\"not used\"");
            break;
    }
}
         
void PrintCurrentParamStatus(ParamStatus_e status,char* value)
{
    switch(status)
    {
        case PARAMSYNC:
            strcpy((char*)value, (char*) "\"Param Sync\"");
            break;
        case PARAMNOTSYNC:
            strcpy((char*)value, (char*) "\"Param Not Sync\"");
            break;
    }
}
         
void PrintCurrentNetStatus(NetStatus_e status,char* value)
{
    switch(status)
    {
        case LINK_OK:
            strcpy((char*)value, (char*) "\"connected\"");
            break;
        case LINK_FAILED:
            strcpy((char*)value, (char*) "\"not used\"");
            break;
        case LINK_IN_PROGRESS:
            strcpy((char*)value, (char*) "\"GPRS Link in progress\"");
            break;
    }

}
         
 /*bob add 20110221 for m2mstatus*/
void PrintCurrentProtocolStatus(ProtocolStatus_e status, char* value)
{
    switch(status)
    {           
        case WMMP_STATE_NOTINIT:/*0x0000*/
            strcpy((char*)value, (char*)"\"WMMP-LIB Not initialized\"");
            break;
        case WMMP_STATE_READY_FOR_SIM_CHANGE:/*0x0001*/
            strcpy((char*)value, (char*) "\"WMMP-LIB Not initialized, ready for SIM change\"");
            break;      
        case WMMP_STATE_AT_READY:/*0x0002*/
            strcpy((char*)value, (char*) "\"WMMP-LIB AT commands ready (interfaces ready)\"");
            break;  

        case WMMP_STATE_INIT_START:/*0x0010*/  
            strcpy((char*)value, (char*)"\"Initialization started\"");
            break;
        case WMMP_STATE_INIT_BEARER:/*0x0011*//* ""  */
            strcpy((char*)value, (char*)"\"Initiating bearer services\"");
            break;
        case WMMP_STATE_DETACH_BEARER:/*0x0012*/        /* ""*/
            strcpy((char*)value, (char*)"\"Detaching bearer services\"");
            break;
        case WMMP_STATE_NEWTORK_WAITING:/*0x0013*//* "" */
            strcpy((char*)value, (char*)"\"Waiting for network registration\"");
            break;
        case WMMP_STATE_LINK_READY:/*0x0020*/
            strcpy((char*)value, (char*)"\"WMMP-T link to platform is ready\"");
            break;

        case WMMP_STATE_REGISTERING_NO_TERMID_BEGIN:/*0x0100*/
            strcpy((char*)value, (char*)"\"Registering: without preset Terminal ID\"");
            break;
        case WMMP_STATE_REGISTERING_TERMID_BEGIN:/*0x0101*/  
            strcpy((char*)value, (char*)"\"Registering: with preset Terminal ID\"");
            break;  
        case WMMP_STATE_REGISTERING_TERMID_ACCESSKEY_BASEKEY_BEGIN:/*0x0201*/    
            strcpy((char*)value, (char*)"\"Registering: with preset Terminal ID, access keys and base key\"");
            break;
        case WMMP_STATE_REGISTERING_WAIT_SECURITY_AUXDATA:/*0x0202*/
            strcpy((char*)value, (char*)"\"Registering: waiting for security configuration (AUX)\"");
            break;
        case WMMP_STATE_REGISTERING_TERMID_SIMBIND_BEGIN:/*0x0203*/
            strcpy((char*)value, (char*)"\"Registering: with preset Terminal ID, bind SIM to platform\"");
            break;

        case REGISTERFAIL:/*0x0204*/
            strcpy((char*)value, (char*)"\"Registration failed\"");
            break;
        case PIDLE:/*0x0205*/
            strcpy((char*)value, (char*)"\"Registration ok, waiting for activation\"");
            break;

        case REGISTERSUCCUSSFUL:/*0x0300*//*WMMP_STATE_REGISTERED_WAIT_LOGIN*/ /*""*/
            strcpy((char*)value, (char*)"\"Registered: ready for login\"");
            break;
        case WMMP_STATE_LOGGING_IN_FIRST_TIME:/*0x0301*/
            strcpy((char*)value, (char*)"\"Logging-in: the first time\"");
            break;
        case WMMP_STATE_LOGGING_IN_NORMAL:/*0x0302*/
            strcpy((char*)value, (char*)"\"Logging-in: normal login\"");
            break;
        case WMMP_STATE_LOGGING_IN_SIMPIN_CLEARED:/*0x0303*/
            strcpy((char*)value, (char*)"\"Logging-in: re-login since SIM parameters were cleared\"");
            break;
        case WMMP_STATE_LOGGEDIN_TRAPPING_CONFIG:/*0x0401*/
            strcpy((char*)value, (char*)"\"Logged-in: login-trap with TLVs synchronization\"");
            break;
        case WMMP_STATE_LOGGEDIN_TRAPPING_EMPTY:/*0x0402*/
            strcpy((char*)value, (char*)"\"Logged-in: login-trap\"");
            break;  

        case LOGINOK:/*0x0500*//*WMMP_STATE_LOGGEDIN*/
            strcpy((char*)value, (char*)"\"Logged-in: ready for operations\"");
            break;
        case WMMP_STATE_LOGGING_OUT_BEGIN:/*0x0501*/
            strcpy((char*)value, (char*)"\"Logging-out\"");
            break;


        case WMMP_STATE_ABNORMAL_TRAPPING:/*0x0600*/
            strcpy((char*)value, (char*)"\"Abnormal: trapping\"");
            break;
        case WMMP_STATE_ABNORMAL_LOGIN_RETRYING:/*0x0601*/
            strcpy((char*)value, (char*)"\"Abnormal: retrying to login\"");
            break;
        case WMMP_STATE_ABNORMAL_LOGGING_OUT:/*0x0602*/
            strcpy((char*)value, (char*)"\"Abnormal: logging-out\"");
            break;
        case WMMP_STATE_ALARM_MSK:/*0x0700*/
            strcpy((char*)value, (char*)"\"alarm state = WMMP_STATE_ALARM_MSK + alarm code\"");
            break;


        case WMMP_STATE_ALARM_ACTIVE_GPRS_FAILED:/*0x0720*/
            strcpy((char*)value, (char*)"\"Alarm: Bearer service, failed to attach\"");
            break;


        case ALARMLOGINFAILED:/*0x08FF*/
            strcpy((char*)value, (char*)"\"Alarm, login failed\"");
            break;

        case WMMP_STATE_REBINDING_KEY_CLEARED:/*0x0900*/
            strcpy((char*)value, (char*)"\"Re-binding: update keys\"");
            break;
        case WMMP_STATE_REBINDING_WAIT_SECURITY_AUXDATA:/*0x0901*/
            strcpy((char*)value, (char*)"\"Re-binding: waiting for security configuration (AUX)\"");
            break;
        case WMMP_STATE_REBINDING_WAIT_SECURITY_ECDATA:/*0x0902*/
            strcpy((char*)value, (char*)"\"Re-binding: waiting for security configuration (encrypted)\"");
            break;
        case WMMP_STATE_REBINDING_CKEY_ONLY:/*0x0903*/
            strcpy((char*)value, (char*)"\"Re-binding: update communication key\"");
            break;
        case WMMP_STATE_REBINDING_WAIT_SIM_READY:/*0x0904*/
            strcpy((char*)value, (char*)"\"Re-binding: waiting for SIM readiness to update PIN configurations\"");
            break;
        case WMMP_STATE_REBINDING_PIN1_APPLY:/*0x0905*/
            strcpy((char*)value, (char*)"\"Re-binding: apply PIN1 configurations\"");
            break;
        case WMMP_STATE_REBINDING_PIN2_APPLY:/*0x0906*/
            strcpy((char*)value, (char*)"\"Re-binding: apply PIN2 configurations\"");
            break;
        case WMMP_STATE_REBINDING_PIN1_CLEAR:/*0x0907*/
            strcpy((char*)value, (char*)"\"Re-binding: reset PIN1 configurations to default\"");
            break;
        case WMMP_STATE_REBINDING_PIN2_CLEAR:/*0x0908*/
            strcpy((char*)value, (char*)"\"Re-binding: reset PIN2 configurations to default\"");
            break;  
        case WMMP_STATE_TERMINATING:/*0x0A02*/
            strcpy((char*)value, (char*)"\"Terminating\"");
            break;  

    }
}
         
void PrintErrorCode(/* maobin@20130306 del  WMMP_ERR_ID_ENUM*/u32 code, char* value)   
{
    //TODO:暂时简单打印错误码，后续需要添加详细错误信息
    sprintf((char*)value, "Error:%d",code);
}
         
         
void PrintRmCtrlCodeDescription(ENUM_WMMP_RMCTRL_CODE_e code,char* description)
{
    switch(code)
    {
        case WMMP_RCTRL_SUSPEND:        /*stop EC application serviece*/
            strcpy((char*)description, (char*) "\"Resume EC application\"");
            break;
        case WMMP_RCTRL_RESUME:     /*restore EC*/
            strcpy((char*)description, (char*) "\"Resume EC application\"");
            break;
        case WMMP_RCTRL_REBOOT:     
            strcpy((char*)description, (char*) "\"Reboot\"");
            break;
        case WMMP_RCTRL_REQUEST_LOGIN:      
            strcpy((char*)description, (char*) "\"Request to login if not logged in\"");
            break;
        case WMMP_RCTRL_REPORT_STATISTICS:      /*SigmaStatTrap*/
            strcpy((char*)description, (char*) "\"Report statistics TLVs 0x2004, 0x2008-0x200E\"");
            break;
        case WMMP_RCTRL_RESET_STATISTICS:       /*clean EC Stat*/
            strcpy((char*)description, (char*) "\"Reset statistics\"");
            break;
        case WMMP_RCTRL_REPORT_MONITOR_DATA:        /*clean EC Stat*/
            strcpy((char*)description, (char*) "\"Report monitor data\"");
            break;
        case WMMP_RCTRL_UPLOAD_APP_DATA:        /*clean EC Stat*/
            strcpy((char*)description, (char*) "\"Upload application data\"");
            break;
        case WMMP_RCTRL_REPORT_DATACOM_CONFIG:      /*clean EC Stat*/
            strcpy((char*)description, (char*) "\"Reset statistics\"");
            break;
        case WMMP_RCTRL_SWITCH_APN_TO_PLATFORM:     /*clean EC Stat*/
            strcpy((char*)description, (char*) "\"Switch APN to the Platform\"");
            break;
        case WMMP_RCTRL_RESET_FACTORY_DEFAULT:      /*Reset configurations to factory default, except Terminal ID and registration configurations*/
            strcpy((char*)description, (char*) "\"Reset configurations to factory default\"");
            break;
        case WMMP_RCTRL_REPORT_SYNC_TLV:        /*TLVSyncTagNum */
            strcpy((char*)description, (char*) "\"Report the sync. TLVs defined by TLV 0x0025\"");
            break;
        case WMMP_RCTRL_SWITCH_APN_TO_APP:      /*TLVSyncTagNum */
            strcpy((char*)description, (char*) "\"Switch APN to the application\"");
            break;
        case WMMP_RCTRL_RELOGIN_DATACOM_ENCRYPT:        /*TLVSyncTagNum */
            strcpy((char*)description, (char*) "\"Re-login with data encryption enabled\"");
            break;
        default:
            strcpy((char*)description, (char*) "not support");
            break;
    }
}
                                         
void simcom_cmwap_apn_set(char* apnname)
{
     //simcom_wmmp_file_data_para simcom_wmmp_file_data;
     WmmpCfgContext_t           *pt1 = NULL; 

     pt1 = (WmmpCfgContext_t *)WPA_ptrToConfigTLV();
     memset(pt1->M2MAPN,0,20);
     strncpy(pt1->M2MAPN,apnname,20);
     //memcpy(&simcom_wmmp_file_data.wmmpCfgContext,pt1,   sizeof(WmmpCfgContext_t);
     WMMP_DBG_TEMP("cmwap apn=%s",pt1->M2MAPN);
     
     //wmmp2_98FileDataWrite();  /*for compile 20120505*/
     WMMP_DBG_TEMP("cmwap apn=%s",pt1->M2MAPN);
}      

/*denglieman add 20120505  -e*/


#ifdef __SIMCOM_WMMP_DEBUG__
/* if allow -- true*/
bool wmmp_check_output_enable(u8 level)
{
    if( level <=  wmmp_trace_cfg.level)
    {
        return true;
    }else
    {
        return false;
    }
}

WMMP_TRACE_MODE wmmp_dbg_get_mode(void)
{
    return wmmp_trace_cfg.mode;
}

/********************************************************************
* Function    :    wmmp_dbg_get_extinfo_en
* Description :    If the extern info is displayed ,return true,
*                  otherwise return false
* Parameters  :    
* Returns     :    true or false 
* <when>     <version>   <who>        <what>
* 2013-01-22    0.1      maobin      created
********************************************************************/
bool wmmp_dbg_get_extinfo_en(WMMP_TRACE_EN_EXTINFO fun)
{
    if(wmmp_trace_cfg.extinfo & (1<<fun) )
    {
        return true;
    }
    return false;
}
/********************************************************************
* Function    :    wmmp_dbg_str_preproc
* Description :    Process the extern info,e.g. function name,file name and line.
* Parameters  :    
* Returns     :    The string len 
* <when>     <version>   <who>        <what>
* 2013-01-22    0.1      maobin      created
********************************************************************/
u16  wmmp_dbg_str_preproc(char* buf, u16 max_len, WMMP_TRACE_PRE* info)
{
    char* str = buf;
    u16 len = 0;

    if( wmmp_dbg_get_extinfo_en(WMMP_TRACE_EN_FILE))
    {
       len = sprintf(str,"[%s]",info->file);
       str += len;
    }
    if( wmmp_dbg_get_extinfo_en(WMMP_TRACE_EN_FUN))
    {
       len = sprintf(str,"[%s]",info->fun);
       str += len;
    }
    if( wmmp_dbg_get_extinfo_en(WMMP_TRACE_EN_LINE))
    {
       len = sprintf(str,"[L%d] ",info->line);
       str += len;
    }
    return (u16)(str-buf);
}
void wmmp_trace(u8 level,const char* fun, const char* file, u32 line,const char*fmt,...)
{
    WMMP_TRACE_PRE trace_pre = {0};
    u16 len = 0;
    va_list args;

    eat_trace("  wmmp_trace 1");
    if(!wmmp_check_output_enable(level))
    {
        return;
    }
    eat_trace("  wmmp_trace 2");
    trace_pre.fun = (char*)fun;
    trace_pre.file = (char*)file;
    trace_pre.line = (u32)line;

    eat_trace("  wmmp_trace 3");
    len = wmmp_dbg_str_preproc(wmmp_trace_buf,MAX_WMMP_TRACE_BUF_LEN, &trace_pre);

    eat_trace("  wmmp_trace 4");
    va_start (args, fmt);
    len += vsnprintf (wmmp_trace_buf + len, MAX_WMMP_TRACE_BUF_LEN - len, fmt, args);
    va_end (args);
    wmmp_trace_buf[len] = '\0';

    eat_trace("  wmmp_trace 5");
    if( WMMP_TRACE_MODE_TRACER == wmmp_dbg_get_mode())
    {
        //kal_prompt_trace(MOD_SIMCOM_WMMP,wmmp_trace_buf);
        eat_trace("%s",wmmp_trace_buf);
    }else if(WMMP_TRACE_MODE_UART == wmmp_dbg_get_mode())
    {
        ;
    }
    eat_trace("  wmmp_trace 6");
}

EatTimer_enum wmmp_get_lib_timer_id(void)
{
    return wmmp_core_timer_id;
}

void wmmp_set_lib_timer_id(EatTimer_enum id)
{
    wmmp_core_timer_id = id;
}
void WPI_StartTimer(u32 timeoutPeriod)
{
    eat_timer_start(wmmp_core_timer_id, timeoutPeriod);
}

void WPI_StopTimer(void)
{
    eat_timer_stop(wmmp_core_timer_id);
}
#endif /*__SIMCOM_WMMP_DEBUG__*/

