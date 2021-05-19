
#if defined (__SIMCOM_WMMP__)

#ifndef _SIMCOM_WMMP_SMS_H_
#define  _SIMCOM_WMMP_SMS_H_

/* MAX data buffer length */
#define SMS_MAX_NUMBER_LEN          20
#define SMS_MAX_USER_DATA_LEN       919
#define SMS_MAX_TPDU_LEN            176/*164*/
#define SMS_MAX_CONCATENATED_SMS_NUMBER 6

typedef struct simcom_wmmp_send_para_tag
{
	wmmp_data_send_enum sel;/*  选择发送数据的类型*/
	
	s32 socketId;			/*  SOCKEET 目标ID  */
	char smsDes[20];			/* 短信目标号码 */
	
	char data[1024];			/* 发送的数据*/
	u32 len;				/* 发送数据的长度  */
	
}simcom_wmmp_send_para;

typedef struct
{
    u8    year[3];
    u8    month[3];
    u8    day[3];
    u8    hour[3];
    u8    minute[3];
    u8    second[3];
    u8    time_zone[3];
    u8    diff_sign[2];
} SMS_TIME_STAMP;

typedef struct
{
    u8               type;   
    u8               number[SMS_MAX_NUMBER_LEN+1];
    u8               text[SMS_MAX_USER_DATA_LEN]; 
    u8               status_report;   
    SMS_TIME_STAMP      time;
} SMS_TEXT_DATA;

typedef  u8 SMS_TPDU_DATA;

typedef union
{
    SMS_TEXT_DATA        text_data;    
    SMS_TPDU_DATA        tpdu_data[SMS_MAX_TPDU_LEN];    
} SMS_DATA_U;

/* PDU 短信发送相关参数结构定义 */
typedef struct tagSMSType
{ // 3gpp 23040-490 9.2.2.2 SMS SUBMIT type
	unsigned char TP_MTI:2;
	unsigned char TP_RD:1;
	unsigned char TP_VPF:2;
	unsigned char TP_RP:1;
	unsigned char TP_UDHI:1;
	unsigned char TP_SRR:1;
}SMSType;

typedef struct tagSmsAddress
{ // 3gpp 23040-490 9.1.2.5 Address fields
	unsigned char ucAddrLen;
	unsigned char ucAddrType;
	unsigned char semiNum_1:4;
	unsigned char semiNum_2:4;
	unsigned char semiNum_3:4;
	unsigned char semiNum_4:4;
	unsigned char semiNum_5:4;
	unsigned char semiNum_6:4;
	unsigned char semiNum_7:4;
	unsigned char semiNum_8:4;
	unsigned char semiNum_9:4;
	unsigned char semiNum_10:4;
	unsigned char semiNum_11:4;
	unsigned char semiNum_12:4;
	unsigned char semiNum_13:4;
	unsigned char semiNum_14:4;
	unsigned char semiNum_15:4;
	unsigned char semiNum_16:4;
	unsigned char semiNum_17:4;
	unsigned char semiNum_18:4;
	unsigned char semiNum_19:4;
	unsigned char semiNum_20:4;
}SmsAddress;

typedef struct tagSmsSubmit
{ // 3gpp 23040-490 9.2.2.2 SMS SUBMIT type
	SMSType stSmsType;
	unsigned char ucTP_MR;
	SmsAddress stTP_DA;
	unsigned char ucTP_PID;
	unsigned char ucTP_DCS;
	unsigned char ucTP_VP;
	unsigned char ucTP_UDL;
}SmsSubmit;

/*无线机器通信协议(WMMP) M2M 平台与终端接口分册(WMMP-T)
 * 附录G  PDU短信格式说明
 * User-Data-Header*/
typedef struct tagUDH
{
    unsigned char ucBuff;//未使用,用来填充，字节对齐
    unsigned char ucHeaderlen;
    unsigned char ucIEType;
    unsigned char ucIElen;
    unsigned short usSrcPort;
    unsigned short usDstPort;

    unsigned char ucIESAR;
    unsigned char ucIESARlen;
    unsigned char refNo;
    unsigned char segNum;
    unsigned char segCnt;
	unsigned char isnull;  //未使用，用来字节对齐
}UDH;

typedef struct _sms_wmmp_struct_
{
    u16 len;
    u8 data[170];
}sms_wmmp_struct;

/* ****************************************************
*                PUSH 短信解析相关结构定义
*
*
********************************************************/
typedef struct
{
    union
    {
	struct
	{
	    unsigned char tp_mti:2; //TP-Message-Type-Indeicator  M 2b 
	    unsigned char tp_mms:1; //TP-More-Messages-to-Send M b
	    unsigned char tp_vpf:2; //TP-Validity-Period-Format
	    unsigned char tp_sri:1; //TP-Status-Report-Indication
	    unsigned char tp_udhi:1; //TP-User-Data-Header-Indication O b
	    unsigned char tp_rp:1; //TP-Reply-Path O b
	}tp_bitOfOctet;
	unsigned char tp_ByteOfOctet;
    }tp_firstOctet;

    unsigned char tp_oa[12];//TP-Originating-Address
    unsigned char tp_pid; //TP-Protocol-Identifier
    unsigned char tp_dcs; //TP-Data-Coding-Scheme
    unsigned char tp_scts[7]; //TP-Service-Centre-Time-stamp
    unsigned char tp_udl; //TP-User-Data-Length
    unsigned char tp_ud[140];//TP-User-Data
}SMS_PUD;

#define NF_SMS_ALPHABET_7_BIT          0        /* 0 */
#define NF_SMS_ALPHABET_8_BIT_DATA     1        /* 1 - Used only for SMS */
#define NF_SMS_ALPHABET_8_BIT_TEXT     4        /* 4 - Used only for SMSP */
#define NF_SMS_ALPHABET_16_BIT         2        /* 2 */
#define SMS_MAX_DECODED_USER_DATA       161

void simcom_wmmp_push_sms_send_pdu(char *sms_data,short sms_len,char *center_num);

#endif // _SIMCOM_WMMP_SMS_H_
#endif //__SIMCOM_WMMP__
