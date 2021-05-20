/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :  simcom_wmmp_sms.c
 * version       :   0.10
 * Description   :   
 * Authors       :   maobin
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2012-05-23, maobin, create originally.
 *
 ********************************************************************/
 #if defined (__SIMCOM_WMMP__)
/********************************************************************
 * Include Files
 ********************************************************************/

#include "wmmp_typ.h"
#include "wmmp_lib.h"
#include "wmmp_api.h"

#include "wmmp_encode.h"
#include "wmmp_crc.h"
#include "wmmp_md5.h"
#include "wmmp_tlv.h"
#include "wmmp_interface.h"
#include "simcom_wmmp_sms.h"
#include "eat_sms.h"

/********************************************************************
* Macros
 ********************************************************************/
 
#define MODULE_NUMBER				MODULE_WMMP
#define PROCESS_NUMBER				PROCESS_WMMP
#define FILE_NUMBER					3
/********************************************************************
* Types
 ********************************************************************/
 
/********************************************************************
* Extern Variables (Extern /Global)
 ********************************************************************/
 
/********************************************************************
* Local Variables:  STATIC
 ********************************************************************/
 
/********************************************************************
* External Functions declaration
 ********************************************************************/
 
/********************************************************************
* Local Function declaration
 ********************************************************************/
 
/********************************************************************
* Local Function
 ********************************************************************/

/* 短信号码接收PDU  包*/
int simcom_sms_fill_desnum(SmsAddress *pSmsAddr, char* pszNum,int iMaxLen)
{
	int iCurrent = 0;
	int iStrlen = strlen(pszNum);
    int da_min_len = 0;

	WMMP_DBG_TEMP("simcom_sms_fill_desnum,len=%d,%s",iStrlen,pszNum);
    
	pSmsAddr->ucAddrLen = MIN(iStrlen,iMaxLen);
    da_min_len = MIN(iStrlen,iMaxLen);
    // 3gpp 23040-490 9.2.1.5 二进制 1-010-0001
    pSmsAddr->ucAddrType = 0xA1;

	while (iCurrent <= da_min_len)
	{
        //WMMP_DBG_TEMP("%d",iCurrent);
        switch (iCurrent)
		{
			case 0:
				if (iCurrent == da_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_1 = (pszNum[iCurrent] - '0');
                }
				break;
				
			case 1:
				if (iCurrent == da_min_len)
				{
					pSmsAddr->semiNum_2 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_2 = (pszNum[iCurrent] - '0');
                }
				break;

			case 2:
				if (iCurrent == da_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_3 = (pszNum[iCurrent] - '0');
                }
				break;

			case 3:
				if (iCurrent == da_min_len)
				{
					pSmsAddr->semiNum_4 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_4 = (pszNum[iCurrent] - '0');
                }
				break;

			case 4:
				if (iCurrent == da_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_5 = (pszNum[iCurrent] - '0');
                }
				break;

			case 5:
				if (iCurrent == da_min_len)
				{
					pSmsAddr->semiNum_6 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_6 = (pszNum[iCurrent] - '0');
                }
				break;

			case 6:
				if (iCurrent == da_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_7 = (pszNum[iCurrent] - '0');
                }
				break;

			case 7:
				if (iCurrent == da_min_len)
				{
					pSmsAddr->semiNum_8 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_8 = (pszNum[iCurrent] - '0');
                }
				break;

			case 8:
				if (iCurrent == da_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_9 = (pszNum[iCurrent] - '0');
                }
				break;

			case 9:
				if (iCurrent == da_min_len)
				{
					pSmsAddr->semiNum_10 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_10 = (pszNum[iCurrent] - '0');
                }
				break;

			case 10:
				if (iCurrent == da_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_11 = (pszNum[iCurrent] - '0');
                }
				break;

			case 11:
				if (iCurrent == da_min_len)
				{
					pSmsAddr->semiNum_12 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_12 = (pszNum[iCurrent] - '0');
                }
				break;

			case 12:
				if (iCurrent == da_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_13 = (pszNum[iCurrent] - '0');
                }
				break;

			case 13:
				if (iCurrent == da_min_len)
				{
					pSmsAddr->semiNum_14 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_14 = (pszNum[iCurrent] - '0');
                }
				break;

			case 14:
				if (iCurrent == da_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_15 = (pszNum[iCurrent] - '0');
                }
				break;

			case 15:
				if (iCurrent == da_min_len)
				{
					pSmsAddr->semiNum_16 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_16 = (pszNum[iCurrent] - '0');
                }
				break;

			case 16:
				if (iCurrent == da_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_17 = (pszNum[iCurrent] - '0');
                }
				break;

			case 17:
				if (iCurrent == da_min_len)
				{
					pSmsAddr->semiNum_18 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_18 = (pszNum[iCurrent] - '0');
                }
				break;

			case 18:
				if (iCurrent == da_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_19 = (pszNum[iCurrent] - '0');
                }
				break;

			case 19:
				if (iCurrent == da_min_len)
				{
					pSmsAddr->semiNum_20 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_20 = (pszNum[iCurrent] - '0');
                }
				break;
		}

		++iCurrent;
	}

	return 0;
}


/* 短信中心号码PDU  包*/
int simcom_sms_fill_scnum(SmsAddress *pSmsAddr, char* pszNum,int iMaxLen)
{
	int iCurrent = 0;
	int iStrlen = strlen(pszNum);
    int sc_min_len = 0;

	WMMP_DBG_TEMP("simcom_sms_fill_scnum");
	if ('+' == pszNum[0])
	{
		iStrlen--;
		pszNum++;
	}

    sc_min_len = MIN(iStrlen,iMaxLen);
	pSmsAddr->ucAddrLen =1 + iStrlen/2 + ((iStrlen%2)?1:0);
	// 3gpp 23040-490 9.2.1.5 二进制 1-010-0001
	pSmsAddr->ucAddrType = 0x91;

	WMMP_DBG_TEMP("sms center code len=%d,%s",sc_min_len, pszNum);
	while (iCurrent <= sc_min_len)
	{
        //WMMP_DBG_TEMP("%d",iCurrent);
        switch (iCurrent)
		{
			case 0:
				if (iCurrent == sc_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_1 = (pszNum[iCurrent] - '0');
                }
				break;
				
			case 1:
				if (iCurrent == sc_min_len)
				{
					pSmsAddr->semiNum_2 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_2 = (pszNum[iCurrent] - '0');
                }
				break;

			case 2:
				if (iCurrent == sc_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_3 = (pszNum[iCurrent] - '0');
                }
				break;

			case 3:
				if (iCurrent == sc_min_len)
				{
					pSmsAddr->semiNum_4 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_4 = (pszNum[iCurrent] - '0');
                }
				break;

			case 4:
				if (iCurrent == sc_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_5 = (pszNum[iCurrent] - '0');
                }
				break;

			case 5:
				if (iCurrent == sc_min_len)
				{
					pSmsAddr->semiNum_6 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_6 = (pszNum[iCurrent] - '0');
                }
				break;

			case 6:
				if (iCurrent == sc_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_7 = (pszNum[iCurrent] - '0');
                }
				break;

			case 7:
				if (iCurrent == sc_min_len)
				{
					pSmsAddr->semiNum_8 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_8 = (pszNum[iCurrent] - '0');
                }
				break;

			case 8:
				if (iCurrent == sc_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_9 = (pszNum[iCurrent] - '0');
                }
				break;

			case 9:
				if (iCurrent == sc_min_len)
				{
					pSmsAddr->semiNum_10 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_10 = (pszNum[iCurrent] - '0');
                }
				break;

			case 10:
				if (iCurrent == sc_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_11 = (pszNum[iCurrent] - '0');
                }
				break;

			case 11:
				if (iCurrent == sc_min_len)
				{
					pSmsAddr->semiNum_12 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_12 = (pszNum[iCurrent] - '0');
                }
				break;

			case 12:
				if (iCurrent == sc_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_13 = (pszNum[iCurrent] - '0');
                }
				break;

			case 13:
				if (iCurrent == sc_min_len)
				{
					pSmsAddr->semiNum_14 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_14 = (pszNum[iCurrent] - '0');
                }
				break;

			case 14:
				if (iCurrent == sc_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_15 = (pszNum[iCurrent] - '0');
                }
				break;

			case 15:
				if (iCurrent == sc_min_len)
				{
					pSmsAddr->semiNum_16 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_16 = (pszNum[iCurrent] - '0');
                }
				break;

			case 16:
				if (iCurrent == sc_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_17 = (pszNum[iCurrent] - '0');
                }
				break;

			case 17:
				if (iCurrent == sc_min_len)
				{
					pSmsAddr->semiNum_18 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_18 = (pszNum[iCurrent] - '0');
                }
				break;

			case 18:
				if (iCurrent == sc_min_len)
				{
					break;
				}
				else
                {
					pSmsAddr->semiNum_19 = (pszNum[iCurrent] - '0');
                }
				break;

			case 19:
				if (iCurrent == sc_min_len)
				{
					pSmsAddr->semiNum_20 = 0xF;
				}
				else
                {
					pSmsAddr->semiNum_20 = (pszNum[iCurrent] - '0');
                }
				break;
            default:
                break;
		}

		++iCurrent;
	}

	return 0;
}


/* 短信测试例子  */
void simcom_m2m_sms_test(void)
{

#if 0
	int ret_val;
	SMS_STATUS_COUNT smsCount;
	SMS_STORAGE_INFO storage;

	/* 所有未读短信的数量 */
	u32 status = SMS_STATUS_UNREAD;
	ret_val = tp_sms_get_count( &smsCount, status );
	WMMP_DBG_TEMP(SIMCOM_MODULE_WMMP, DEBUG_LEVEL_NORMAL,"simcom_m2m_sms_test 	ret_val=%d,%d,%d,%d\n",smsCount.total_count,smsCount.handset_count,smsCount.usim_count,	ret_val);	

	status = SMS_STATUS_READ;
	ret_val = tp_sms_get_count( &smsCount, status );
	WMMP_DBG_TEMP(SIMCOM_MODULE_WMMP, DEBUG_LEVEL_NORMAL,"simcom_m2m_sms_test 	ret_val=%d,%d,%d,%d\n",smsCount.total_count,smsCount.handset_count,smsCount.usim_count,	ret_val);	

	status = SMS_STATUS_UNSENT;
	ret_val = tp_sms_get_count( &smsCount, status );
	WMMP_DBG_TEMP(SIMCOM_MODULE_WMMP, DEBUG_LEVEL_NORMAL,"simcom_m2m_sms_test 	ret_val=%d,%d,%d,%d\n",smsCount.total_count,smsCount.handset_count,smsCount.usim_count,	ret_val);	

	status = SMS_STATUS_SENT;
	ret_val = tp_sms_get_count( &smsCount, status );
	WMMP_DBG_TEMP(SIMCOM_MODULE_WMMP, DEBUG_LEVEL_NORMAL,"simcom_m2m_sms_test 	ret_val=%d,%d,%d,%d\n",smsCount.total_count,smsCount.handset_count,smsCount.usim_count,	ret_val);	


	/*******************************************************************
	*   USIM 中的最大存储量和已经使用的存储量
	*  以及手机中的最大存储量和已经使用的存储量 
	********************************************************************/
	ret_val = tp_sms_get_storage_info( &storage );
	WMMP_DBG_TEMP(SIMCOM_MODULE_WMMP, DEBUG_LEVEL_NORMAL,"simcom_m2m_sms_test ret_val=%d,%d,%d,%d,%d\n",
							ret_val,
							storage.max_handset_count,
							storage.used_handset_count,
							storage.max_usim_count,
							storage.used_usim_count);	
#endif	

}


void pushSmsDecodeDcsPeer( unsigned char encoded_dcs, unsigned char * alphabet )
{
	WMMP_DBG_TEMP("pushSmsDecodeDcsPeer");

    /* DCS 00xx xxxx */
    if( !(encoded_dcs & 0xC0) )
    {
		/* Alphabet bit 3 and 2 ( if reserved value, alphabet = 7 bits) */
		* alphabet = (( (unsigned char)(encoded_dcs & 0x0C) >> (unsigned char)2 ) != 3) ? ( (unsigned char)(encoded_dcs & 0x0C) >> (unsigned char)2 ) : NF_SMS_ALPHABET_7_BIT;
    }
    else
    {
        /* DCS 11xx xxxx */
        if( (encoded_dcs & 0xC0) == 0xC0 )
        {
            switch( encoded_dcs & 0xF0 )
            {
                /* DCS 1100 xxxx */
                case 0xC0:
                /* DCS 1101 xxxx */
                case 0xD0:
                /* DCS 1110 xxxx */
                case 0xE0:
                    /* Decode such DCS: 11xx xxxx (except DCS 1111 xxx) */
                    //sc_decode_11xx_dsc( encoded_dcs, p_decoded_dcs );
                    {
						switch( encoded_dcs & 0xF0 )
						{
							/* DCS 1100 x0xx */
							case 0xC0:
								/* Initialize the alphabet to 7-bit coding */
								*alphabet = NF_SMS_ALPHABET_7_BIT;

							break;
						
							/* DCS 1101 x0xx */
							case 0xD0:
								/* Initialize the alphabet to 7-bit coding */
								*alphabet = NF_SMS_ALPHABET_7_BIT;
						

							break;
						
							/* DCS 1110 x0xx */
							/* case 0xE0 */
							default:
								/* Initialize the alphabet to 16-bit coding */
								*alphabet = NF_SMS_ALPHABET_16_BIT;
							break;
						}


					}
                break;

                /* DCS 1111 xxxx */
                /* case 0xF0 */
                default:
                    /* Decode such DCS: 1111 xxxx */
                   // sc_decode_1111_dsc( encoded_dcs, p_decoded_dcs );
                {
						/* If bit 2 = 1 */
						if( encoded_dcs & 0x04 )
						{
							/* Alphabet is 8-bit data */
							*alphabet = NF_SMS_ALPHABET_8_BIT_DATA;
						}
						/* Else bit 2 = 0 */
						else
						{
							/* Alphabet is 7-bit data */
							*alphabet = NF_SMS_ALPHABET_7_BIT;
						}

				}
                break;
            }
        }
        /* Reserved values (DCS 01xx xxxx,10xx xxxx), interpreted as 00000000 
*/
        else
        {
            * alphabet = NF_SMS_ALPHABET_7_BIT;
        }
    }


}


/* 短信发送 pdu*/
int  simcom_wmmp_sms_send_pdu(char *sms_data,short sms_len,char *center_num)
{
	SMS_DATA_U stTPDU = {0};
	UDH stUDH = {0};
	unsigned char stSmsData[176] = {0};
	SmsSubmit stSmsSub = {0};
	SmsAddress stSCNum = {0};
	char acContentBuff[60];
	char acTransContentBuff[120];
	char *pszNumbuf;
	int iFillLen = 0;
	int  iNumLen;
	int iCurPos = 0;
	int iUDHLen = sizeof(UDH) - 1;//strlen("06050442664266");	
	int iResult = 0;	
	//SMS_PARAM stSmsparam = {0};
	//SMS_PARAM OldSmsParam = {0};
	unsigned char ucSubmitLen = 0;
	unsigned char ucTPDULen = 0;	
	char temp_center_num[] = "10658254"; //"13996259954";// "15921006788";
	//char sc_num[]= "+8613800210500";
	char sc_num[]= "+8613800230500";
	char scaNumber[50] = "\0";

	WMMP_DBG_TEMP("simcom_wmmp_sms_send_pdu");

	return 0;
	
	stUDH.ucHeaderlen = 6;
	stUDH.ucIEType = 5;
	stUDH.ucIElen = 4;
	//stUDH.usSrcPort = bsd_htons(16998);
	//stUDH.usDstPort = bsd_htons(16998);	
	stUDH.usSrcPort =  soc_htons(2862);//0b2e
	stUDH.usDstPort =  soc_htons(49154);//c002	
	
    /*得到短信中心号码*/

	/*填写短信中心号码*/
	simcom_sms_fill_scnum(&stSCNum, scaNumber, 20);
	//simcom_sms_fill_scnum(&stSCNum, (char *)stSmsparam.sc_number, 20);	
	ucTPDULen = stSCNum.ucAddrLen + 1;
	stTPDU.tpdu_data[0] = ucTPDULen;
	memcpy(&stTPDU.tpdu_data[1],&stSCNum,ucTPDULen);
	
	/*2.填写submit sms type*/
	// 3gpp 23040-490 9.2.3.1 message type = sms-submit
	stSmsSub.stSmsType.TP_MTI = 0x01; 

	// TP Reject Duplicates
	stSmsSub.stSmsType.TP_RD = 0x00;

	// 3gpp 23040-490 9.2.3.3
	stSmsSub.stSmsType.TP_VPF = 0x02;

	// 3gpp 23040-490 9.2.3.17
	stSmsSub.stSmsType.TP_RP = 0x00;

	// 3gpp 23040-490 9.2.3.23 指示消息内容包含用户自定义消息头
	stSmsSub.stSmsType.TP_UDHI = 0x00;//0x01;//wmmp

	// 3gpp 23040-490 9.2.3.5 不需要状态报告
	stSmsSub.stSmsType.TP_SRR = 0x00;

	// 3gpp 23040-490 9.2.3.6 短信序号
	stSmsSub.ucTP_MR = 0x00;

	/*取目的号码*/
	pszNumbuf = center_num; 
	WMMP_DBG_TEMP("WMMP TEST:taget num = %s\n",center_num);
	
	/*填写目的号码*/
	simcom_sms_fill_desnum(&stSmsSub.stTP_DA, pszNumbuf, strlen(center_num));//wmmp
	//AppDmFillSCNum(&stSmsSub.stTP_DA, pszNumbuf, strlen(temp_center_num));//wmmp	
	pszNumbuf = NULL;

	// 3gpp 23040-490 9.2.3.9 协议类型
	stSmsSub.ucTP_PID = 0x00;

	// smpp_v3.4 5.2.19 data coding scheme 8-bit
	stSmsSub.ucTP_DCS = 0x04;

	// 3gpp 23040-490 9.2.3.12 
	stSmsSub.ucTP_VP = 0xFF;
	/*
	stSmsSub.aucTP_VP[1] = 0x00;
	stSmsSub.aucTP_VP[2] = 0x00;
	stSmsSub.aucTP_VP[3] = 0x00;
	stSmsSub.aucTP_VP[4] = 0x00;
	stSmsSub.aucTP_VP[5] = 0x00;
	stSmsSub.aucTP_VP[6] = 0x00;
	*/
	//剔除stSmsSub.stTP_DA的填充位
	memcpy(stSmsData, &stSmsSub, sizeof(stSmsSub));

	iFillLen = 12 - (stSmsSub.stTP_DA.ucAddrLen/2 + stSmsSub.stTP_DA.ucAddrLen%2 + 2);
	if (iFillLen > 0)
	{
		memmove(&stSmsData[14-iFillLen], &stSmsData[14], 10);
	}

	//构造消息内容
	//memcpy(&acTransContentBuff[iUDHLen],sms_data,sms_len);// wmmp
	memcpy(&acTransContentBuff[0],sms_data,sms_len);//wmmp

	// 对内容进行8-bit编码
/*	while (acContentBuff[iCurPos])
	{
		utilIface_xtoa(acContentBuff[iCurPos], &acTransContentBuff[iUDHLen+iCurPos*2]);
		iCurPos++;
	}*/

	//memcpy(acTransContentBuff, "06050442664266", iUDHLen);
	//memcpy(acTransContentBuff, &stUDH.ucHeaderlen, iUDHLen);//wmmp

	//将内容考入短信PDU
	//memcpy(&stSmsData[sizeof(stSmsSub)-iFillLen], acTransContentBuff, iUDHLen + sms_len);//strlen(acTransContentBuff));//wmmp
	memcpy(&stSmsData[sizeof(stSmsSub)-iFillLen], acTransContentBuff, sms_len);//strlen(acTransContentBuff));//wmmp

	//内容长度
	//stSmsData[sizeof(stSmsSub)-1-iFillLen] =iUDHLen + sms_len;// strlen(acTransContentBuff);//wmmp
	stSmsData[sizeof(stSmsSub)-1-iFillLen] = sms_len;// strlen(acTransContentBuff);//wmmp

	//ucSubmitLen = sizeof(stSmsSub)-iFillLen+iUDHLen + sms_len;//strlen(acTransContentBuff);//wmmp
	ucSubmitLen = sizeof(stSmsSub)-iFillLen+sms_len;//strlen(acTransContentBuff);//wmmp

	memcpy(&stTPDU.tpdu_data[1+ucTPDULen],stSmsData,ucSubmitLen);
	stTPDU.tpdu_data[0] = ucTPDULen+ucSubmitLen;

	WMMP_DBG_TEMP("simcom_wmmp_sms_send_pdu ucSubmitLen %d",ucSubmitLen);
	WMMP_DBG_TEMP("simcom_wmmp_sms_send_pdu ucTPDULen %d",ucTPDULen);
	//WMMP_DBG_TEMP(SIMCOM_MODULE_WMMP, 	DEBUG_LEVEL_NORMAL,"WPI_hex_printf()\n");		
	WPI_hex_printf((char*)stTPDU.tpdu_data,(short)(ucTPDULen+ucSubmitLen + 1));		
	WMMP_DBG_TEMP("sms_send_pdu %d\n",iResult);	
	//iResult = tp_sms_send(&stTPDU, false, SMS_STATUS_READ, 0, false);	
	//iResult = asm5_45SendPduSms(stTPDU.tpdu_data[1],(stTPDU.tpdu_data[0])*2);
	//iResult = asm5_45SendPduSms(stTPDU.tpdu_data,(unsigned short)(ucTPDULen+ucSubmitLen + 1));
	
	WMMP_DBG_TEMP("WMMP TEST:iResult = %d\n",iResult);	

	return iResult;
}


int  simcom_push_port_sms_send(char *sms_data,short sms_len,char *center_num,short dstport,short srcport,unsigned char dcs)
{
	SMS_DATA_U stTPDU = {0};
	UDH stUDH = {0};
	unsigned char stSmsData[176] = {0};
	SmsSubmit stSmsSub = {0};
	SmsAddress stSCNum = {0};
	char acContentBuff[60];
	char acTransContentBuff[120];
	char *pszNumbuf;
	int iFillLen = 0;
	int  iNumLen;
	int iCurPos = 0;
	//int iUDHLen = sizeof(UDH) - 1;//strlen("06050442664266");
	int iUDHLen;// = sizeof(UDH) - 2;//strlen("0B05040000C0020003040101");
	int iResult = 0;
	//SMS_PARAM stSmsparam = {0};
	//SMS_PARAM OldSmsParam = {0};
	unsigned char ucSubmitLen = 0;
	unsigned char ucTPDULen = 0;
	char temp_center_num[] = "10658254"; //"13996259954";// "15921006788";
	//char sc_num[]= "+8613800210500";
	char sc_num[]= "+8613800230500";
	short lenn;
	char scaNumber[50] = "\0";

	WMMP_DBG_TEMP("simcom_push_port_sms_send");

	// 0B 05 04 00 00 C0 02 00 03 04 01 01
	stUDH.ucHeaderlen = 6;   //0x0b;
	stUDH.ucIEType = 5;
	stUDH.ucIElen = 4;
	
	stUDH.usSrcPort = soc_htons(dstport);//0b2e
	stUDH.usDstPort = soc_htons(srcport);//c002srcport

	//stUDH.usSrcPort = bsd_htons(2862);//0b2e
	//stUDH.usDstPort = bsd_htons(49154);//c002	


	stUDH.ucIESAR = 0x00;
	stUDH.ucIESARlen= 0x03;
	stUDH.refNo= 0x04;
	stUDH.segNum= 0x01;
	stUDH.segCnt= 0x01;

	 /* 结构体的字节长度是偶数*/
	iUDHLen =7;   // sizeof(UDH)- 2;/* 减去2是因为UDH 有14个字节结构体多了一个 无用的字节*/	
	WMMP_DBG_TEMP("iUDHLen = %d\n",iUDHLen);	
	
	/*填写短信中心号码*/
	//simcom_sms_fill_scnum(&stSCNum, (char *)stSmsparam.sc_number, 20);	
	//asm5_48GetSCANumber(scaNumber);
	simcom_sms_fill_scnum(&stSCNum, scaNumber, 20);
	WMMP_DBG_TEMP("port stSCNum.ucAddrLen %d",stSCNum.ucAddrLen);
	ucTPDULen = stSCNum.ucAddrLen + 1;
	WMMP_DBG_TEMP("PORT send_pdu ucAddrLen ucTPDULen %d",ucTPDULen);
	stTPDU.tpdu_data[0] = ucTPDULen;
	memcpy(&stTPDU.tpdu_data[1],&stSCNum,ucTPDULen);
	
	/*2.填写submit sms type*/
	// 3gpp 23040-490 9.2.3.1 message type = sms-submit
	stSmsSub.stSmsType.TP_MTI = 0x01;

	// TP Reject Duplicates
	stSmsSub.stSmsType.TP_RD = 0x00;

	// 3gpp 23040-490 9.2.3.3
	stSmsSub.stSmsType.TP_VPF = 0x02;

	// 3gpp 23040-490 9.2.3.17
	stSmsSub.stSmsType.TP_RP = 0x00;

	// 3gpp 23040-490 9.2.3.23 指示消息内容包含用户自定义消息头
	stSmsSub.stSmsType.TP_UDHI = 0x01;

	// 3gpp 23040-490 9.2.3.5 不需要状态报告
	stSmsSub.stSmsType.TP_SRR = 0x00;

	// 3gpp 23040-490 9.2.3.6 短信序号
	stSmsSub.ucTP_MR = 0x00;

	/*取目的号码*/
	pszNumbuf = center_num;
	WMMP_DBG_TEMP("PORT TEST:taget num = %s\n",center_num);

	/*填写目的号码*/
	simcom_sms_fill_desnum(&stSmsSub.stTP_DA, pszNumbuf, strlen(center_num));//wmmp
	//AppDmFillSCNum(&stSmsSub.stTP_DA, pszNumbuf, strlen(temp_center_num));//wmmp	
	pszNumbuf = NULL;

	// 3gpp 23040-490 9.2.3.9 协议类型
	stSmsSub.ucTP_PID = 0x00;

	// smpp_v3.4 5.2.19 data coding scheme 8-bit
	stSmsSub.ucTP_DCS = dcs;//0x04;      

	// 3gpp 23040-490 9.2.3.12 
	stSmsSub.ucTP_VP = 0xFF;
	/*
	stSmsSub.aucTP_VP[1] = 0x00;
	stSmsSub.aucTP_VP[2] = 0x00;
	stSmsSub.aucTP_VP[3] = 0x00;
	stSmsSub.aucTP_VP[4] = 0x00;
	stSmsSub.aucTP_VP[5] = 0x00;
	stSmsSub.aucTP_VP[6] = 0x00;
	*/
	//剔除stSmsSub.stTP_DA的填充位
	memcpy(stSmsData, &stSmsSub, sizeof(stSmsSub));
	WMMP_DBG_TEMP("PORT send_pdu stTP_DA.ucAddrLen %d",stSmsSub.stTP_DA.ucAddrLen);
	iFillLen = 12 - (stSmsSub.stTP_DA.ucAddrLen/2 + stSmsSub.stTP_DA.ucAddrLen%2+ 2);
	WMMP_DBG_TEMP("len = %d\n",iFillLen);
	
	if (iFillLen > 0)
	{
		memmove(&stSmsData[14-iFillLen], &stSmsData[14], 10);
	}

	//构造消息内容
	//memcpy(&acTransContentBuff[iUDHLen],sms_data,sms_len);// wmmp
	memcpy(&acTransContentBuff[iUDHLen],sms_data,sms_len);//wmmp

	// 对内容进行8-bit编码
/*	while (acContentBuff[iCurPos])
	{
		utilIface_xtoa(acContentBuff[iCurPos], &acTransContentBuff[iUDHLen+iCurPos*2]);
		iCurPos++;
	}*/

	//memcpy(acTransContentBuff, "06050442664266", iUDHLen);
	memcpy(acTransContentBuff, &stUDH.ucHeaderlen, iUDHLen);

	//将内容考入短信PDU
	lenn = iUDHLen + sms_len;
	WMMP_DBG_TEMP("len = %d,%d,%d\n",iUDHLen, sms_len,lenn);
	memcpy(&stSmsData[sizeof(stSmsSub)-iFillLen], acTransContentBuff, lenn);

	//内容长度
	stSmsData[sizeof(stSmsSub)-1-iFillLen] = lenn;

	ucSubmitLen = sizeof(stSmsSub)-iFillLen + lenn;
	WMMP_DBG_TEMP("PORT send_pdu ucSubmitLen %d",ucSubmitLen);
	WMMP_DBG_TEMP("PORT send_pdu ucTPDULen %d",ucTPDULen);
	memcpy(&stTPDU.tpdu_data[1+ucTPDULen],stSmsData,ucSubmitLen);
	stTPDU.tpdu_data[0] = ucTPDULen+ucSubmitLen;
	WMMP_DBG_TEMP("WPI_hex_printf()\n");		
	//WPI_hex_printf((char*)stTPDU.tpdu_data,(short)(ucTPDULen+ucSubmitLen + 1));		

	//iResult = tp_sms_send(&stTPDU, false, SMS_STATUS_READ, 0, false);	
	//iResult = asm5_45SendPduSms(stTPDU.tpdu_data[1],(stTPDU.tpdu_data[0])*2);
	//iResult = asm5_45SendPduSms((char*)stTPDU.tpdu_data,(unsigned short)(ucTPDULen+ucSubmitLen + 1));
	WMMP_DBG_TEMP("PORT TEST:iResult = %d\n",iResult);
	
	return iResult;
}

/*******************************************************************
 * PDU发送短信格式：
 *|SCA| |PDUType| |MR|  |DA|   |PID| |DCS|   |VP|    |UDL|  |UD|
 * 1-12     1      1    2-12     1     1    0,1,7      1    0-140
 * 
 * 短信号码格式86+num，例如：86136
 ********************************************************************/
void simcom_wmmp_push_sms_send_pdu(char *sms_data,short sms_len,char *center_num)
{
	SMS_DATA_U stTPDU = {0};
	UDH stUDH = {0};
	unsigned char stSmsData[176] = {0};//用于处理短信的临时buffer
	SmsSubmit stSmsSub = {0}; //短信结构体
	SmsAddress stSCNum = {0}; //短信中心号码
	char acTransContentBuff[120];
	char *pszNumbuf;
	int iFillLen = 0;//SmsAddress在DA中未使用的长度
	int iUDHLen;//strlen("0B05040000C0020003040101");
	unsigned char ucSubmitLen = 0;
	unsigned char ucTPDULen = 0; //短信中心号码域长度
	//char sc_num[]= "+8613800210500"; //for testing
	short lenn;
    char sc_addr[20]={0};
    //char pdu_buf[] = {0x00,0x11,0x00,0x0D,0x91,0x68,0x31,0x06,0x71,0x40,0x49,0xF0,0x00,0x1B,0x01,0x0C,0x00,0x41,
    //     0x00,0x61,0x00,0x31,0x00,0x23,0x4F,0x60,0x59,0x7D}; //for testing

	WMMP_DBG_TEMP("simcom_wmmp_push_sms_send_pdu");
    //return 1;
    /******************************************************************
     * 根据协议 无线机器通信协议(WMMP) M2M 平台与终端接口分册(WMMP-T)
     * 附录G  PDU短信格式说明
     *
     * User-Data-Header:
     * UDH  Value  Description 
     * 0B  User-Data-Header (UDHL) Length = 11 
     * 5  UDH IE identifier: Port numbers 
     * 4  UDH port number IE length 
     * 0B 目的端口号(2 byte)
     * 2E 
     * C0 源端口号(2 byte)  
     * 02 
     * 00  UDH IE identifier: SAR 
     * 03  UDH SAR IE length 
     * 04  Datagram ref no. 
     * 01  Total no. of segments in datagram 
     * 01  Segment count 
     * 
     ******************************************************************/
	// 0B 05 04 00 00 C0 02 00 03 04 01 01
	stUDH.ucHeaderlen = 0x0b; //len is 11，不包括UDHL这个标识字节
	stUDH.ucIEType = 5; 
	stUDH.ucIElen = 4;
	
	stUDH.usSrcPort =  soc_htons(2862);//2862=0x0b2e
	stUDH.usDstPort =  soc_htons(49154);//49154=0xc002	

	stUDH.ucIESAR = 0x00;
	stUDH.ucIESARlen= 0x03;
	stUDH.refNo= 0x04;
	stUDH.segNum= 0x01;
	stUDH.segCnt= 0x01;

	 /* 结构体的字节长度是偶数
	  *为了对齐在开始和结尾填充了两个字节，所以需要减去两个字节，len=12*/
	iUDHLen = sizeof(UDH)-2;
	 WMMP_DBG_TEMP("iUDHLen = %d\n",iUDHLen);

    /*得到短信中心号码*/
     eat_get_SMS_sc(sc_addr);
     /*填写短信中心号码*/
	simcom_sms_fill_scnum(&stSCNum, sc_addr, 20);	

	WMMP_DBG_TEMP("wmmp send_pdu sms sc addr:%s",sc_addr);
    //ucAddrLen = 服务中心类型+中心号码长度,不包括SC长度域本身1个字节
    //所以短信中心号码域长度为 ucAddrLen+1
	ucTPDULen = stSCNum.ucAddrLen + 1;
	WMMP_DBG_TEMP("wmmp send_pdu ucAddrLen ucTPDULen %d",ucTPDULen);
	stTPDU.tpdu_data[0] = ucTPDULen;
    //填充pdu 数据的短信中心号码SCA
	memcpy(&stTPDU.tpdu_data[1],&stSCNum,ucTPDULen);
	
	/*2.填写submit sms type*/
	// 3gpp 23040-490 9.2.3.1 message type = sms-submit
    // stSmsType:01010001 = 0x51
	stSmsSub.stSmsType.TP_MTI = 0x01;

	// TP Reject Duplicates
	stSmsSub.stSmsType.TP_RD = 0x00;

	// 3gpp 23040-490 9.2.3.3
	stSmsSub.stSmsType.TP_VPF = 0x02;

	// 3gpp 23040-490 9.2.3.17
	stSmsSub.stSmsType.TP_RP = 0x00;

	// 3gpp 23040-490 9.2.3.23 指示消息内容包含用户自定义消息头
	stSmsSub.stSmsType.TP_UDHI = 0x01;

	// 3gpp 23040-490 9.2.3.5 不需要状态报告
	stSmsSub.stSmsType.TP_SRR = 0x00;

	// 3gpp 23040-490 9.2.3.6 短信序号
	stSmsSub.ucTP_MR = 0x00;

	/*取目的号码*/
	pszNumbuf = center_num;
	WMMP_DBG_TEMP("WMMP TEST:taget num = %s\n",center_num);

	/*填写目的号码*/
	simcom_sms_fill_desnum(&stSmsSub.stTP_DA, pszNumbuf, strlen(center_num));//wmmp
	pszNumbuf = NULL;

	// 3gpp 23040-490 9.2.3.9 协议类型
	stSmsSub.ucTP_PID = 0x00;

	// smpp_v3.4 5.2.19 data coding scheme 8-bit
	stSmsSub.ucTP_DCS = 0x04;

	// 3gpp 23040-490 9.2.3.12 
	stSmsSub.ucTP_VP = 0xFF;
	/*
	stSmsSub.aucTP_VP[1] = 0x00;
	stSmsSub.aucTP_VP[2] = 0x00;
	stSmsSub.aucTP_VP[3] = 0x00;
	stSmsSub.aucTP_VP[4] = 0x00;
	stSmsSub.aucTP_VP[5] = 0x00;
	stSmsSub.aucTP_VP[6] = 0x00;
	*/
	//剔除stSmsSub.stTP_DA的填充位
	memcpy(stSmsData, &stSmsSub, sizeof(stSmsSub));

	WMMP_DBG_TEMP("wmmp send_pdu stTP_DA.ucAddrLen %d",stSmsSub.stTP_DA.ucAddrLen);

    /****************************************************************
     *SmsSubmit结构体 
     *     |SCA| |PDUType| |MR|  |DA|
     *len:   9        1      1
     *短信中心号码(SC)中的SmsAddress.ucAddrLen 为号码及type在短信中占用的实际长度
     *而目的地址(DA)号码中的SmsAddress.ucAddrLen为号码的ascii类型的长度
     ****************************************************************/
    /*
     * 计算短信目的号码在pdu中实际占用的长度 
     * 例如号码为1064899111111,号码长度为13，DA实际占用的
     * 长度为 1+1+7 = 9
     * len  Type   NUM 
     * 0D   A1    014698191111F1
     */
    iFillLen = stSmsSub.stTP_DA.ucAddrLen/2 + stSmsSub.stTP_DA.ucAddrLen%2+ 2;
    //未用到的长度
    iFillLen = sizeof(SmsAddress) - iFillLen ;
	WMMP_DBG_TEMP("len = %d\n",iFillLen);
	
	if (iFillLen > 0)
	{
        /*把DA以后的数据拷到DA中未使用的位置
         * SmsSubmit 结构体中PDU-type,MR,DA 三个总计长度为14
         * 14-iFillLen 表示 SmsSubmit结构体中DA末位往前推的个数
         */
        memmove(&stSmsData[14-iFillLen], &stSmsData[14], 10);
	}

	//构造消息内容
	memcpy(&acTransContentBuff[iUDHLen],sms_data,sms_len);//wmmp

#if 0
	// 对内容进行8-bit编码
    while (acContentBuff[iCurPos])
	{
		utilIface_xtoa(acContentBuff[iCurPos], &acTransContentBuff[iUDHLen+iCurPos*2]);
		iCurPos++;
	}
#endif

    /*传入WMMP UDH值 */
	memcpy(acTransContentBuff, &stUDH.ucHeaderlen, iUDHLen);

	/*将内容拷入短信PDU
     *PDU中的UDL=(WMMP UDH len)+(sms data len)
	 */
	lenn = iUDHLen + sms_len;
	WMMP_DBG_TEMP("len = %d,%d,%d\n",iUDHLen, sms_len,lenn);
    //把DA后面的所有数据都往前移iFillLen个位置
	memcpy(&stSmsData[sizeof(stSmsSub)-iFillLen], acTransContentBuff, lenn);

	//PDU用户数据长度UDL
	stSmsData[sizeof(stSmsSub)-iFillLen-1] = lenn;

	ucSubmitLen = sizeof(stSmsSub)-iFillLen + lenn;
	WMMP_DBG_TEMP("wmmp send_pdu ucSubmitLen %d",ucSubmitLen);
	WMMP_DBG_TEMP("wmmp send_pdu ucTPDULen %d",ucTPDULen);
    //stTPDU.tpdu_data 第一个字节存放总的pdu数据长度值
	memcpy(&stTPDU.tpdu_data[1+ucTPDULen],stSmsData,ucSubmitLen);
	stTPDU.tpdu_data[0] = ucTPDULen+ucSubmitLen;

	WPI_hex_printf((char*)&stTPDU.tpdu_data[1],(short)(ucTPDULen+ucSubmitLen ));		

    eat_WPI_SendSMS(&stTPDU.tpdu_data[1], stTPDU.tpdu_data[0]);
}

/* 普通端口短信的读取 */
bool simcom_push_port_sms_deal(char* sms_data,SMS_PUD *sms_pdu,char* wmmp_port_head,unsigned char  *alphabet)
{
    //SMS_PUD sms_pdu;
    short lenn;
    char* pPudCode;// =(char*)sms_data;	
    //unsigned char  alphabet;
    char  wmmpHeadSms[20] = {0} ;	

    WMMP_DBG_TEMP("simcom_push_port_sms_deal");

    /*************************************************************************** 
      普通端口短信的例子
     * 08 91683108100005F0 44 0D9168 3119716985F1 00 00 01304201941523 0A 06 0504 C350 00 00 E832
     * 08 91683108100005F0 44 0D9168 3119716985F1 00 08 01304201750223 0B 06 0504 C350 00 00 4F60597D
     *
     *          WMMP  exsamp :         例子
     * 640AA10156484063 210490110371318023750B05040000C00200030401010069000E000000010300800041534D3730303037323232323232323200E02500083132333435363738
     * E0270004AAAAAAAAE02800083837363534333231E02A0004AAAAAAAAE0360018313132323333343435353636373738383939303031323334E0380004AAAAAAAA
     *****************************************************************************************************/
    pPudCode = (char*)sms_data;
    //WPI_hex_printf((char *)pPudCode,100);	

    //  get FO (exsamp : 0x64 --->  bit6 = 1 --- >   VHDI = 1)
    sms_pdu->tp_firstOctet.tp_ByteOfOctet = *pPudCode;
    pPudCode ++;	

    // get length os TP-OA   (exsamp : 0x0a/2 + 2 = 7字节)
    lenn = (((*pPudCode)%2)?((*pPudCode+1)/2):((*pPudCode)/2) )+ 2;
    memcpy(sms_pdu->tp_oa,pPudCode,lenn);

    pPudCode += lenn;
    //WPI_hex_printf((char *)sms_pdu->tp_oa,lenn);	

    // get pid  (example : = 0x21)  
    sms_pdu->tp_pid = *(pPudCode++);

    // get dcs  (example : = 0x04)  
    sms_pdu->tp_dcs = *(pPudCode++);

    // get time stamp  (example: = 90110371318023)
    memcpy(sms_pdu->tp_scts,pPudCode,7);
    WMMP_DBG_TEMP("WPI_hex_printf()\n");		
    WPI_hex_printf((char *)sms_pdu->tp_scts,7);
    pPudCode += 7;
    // WPI_hex_printf((char *)sms_pdu->tp_scts,7);

    // get the user data lenth (example := 0x75)
    sms_pdu->tp_udl = *(pPudCode++);	
    memcpy(sms_pdu->tp_ud,pPudCode,sms_pdu->tp_udl);	
    //WPI_hex_printf((char *)sms_pdu->tp_ud,sms_pdu->tp_udl);

    pushSmsDecodeDcsPeer(sms_pdu->tp_dcs,alphabet);

    WMMP_DBG_TEMP("xpid=%x,dcs=%x,code=%x\n", sms_pdu->tp_pid,sms_pdu->tp_dcs,alphabet);

    if(*alphabet == NF_SMS_ALPHABET_7_BIT)
    {
        WMMP_DBG_TEMP("PUSH SMS smCoding err: 7 bit dcs\n");
        //return false;
    }
    else if(*alphabet == NF_SMS_ALPHABET_8_BIT_DATA)
    {
        WMMP_DBG_TEMP("PUSH SMS smCoding err:  8 bit dcs\n");
    }
    else if(*alphabet == NF_SMS_ALPHABET_16_BIT)
    {
        WMMP_DBG_TEMP("PUSH SMS smCoding err:  16 bit dcs\n");
        //return false;
    }	

    if(1 == (sms_pdu->tp_firstOctet.tp_bitOfOctet.tp_udhi&1))
    {
        //0B 05 04 00 00 C0 02 00 03 04 01 01
        //{0x0B,0X05,0X04,0X0B,0X2E,0XC0,0X02,0X00,0X03,0X04,0X01,0X01};/*平台短信WDP Head */
        wmmpHeadSms[0] = *(pPudCode++);
        WMMP_DBG_TEMP("PUSH SMS port UDHL=%d\n",wmmpHeadSms[0]);
#if 0
        if (wmmpHeadSms[0] != 0x0b)
        {
            WMMP_DBG_TEMP(SIMCOM_MODULE_WMMP, DEBUG_LEVEL_NORMAL,"PUSH SMS smCoding err: wmmpHeadSms[0] != 0x0b\n");
            return false;
        }
#endif

        memcpy(&wmmpHeadSms[1],pPudCode,wmmpHeadSms[0]);

        if( (wmmpHeadSms[1]!=5) ||(wmmpHeadSms[2]!=4) )		
        {
            /* 不是端口短信的条件 */
            WMMP_DBG_TEMP("PUSH SMS PORT HEAD != 0X05 AND 0X04\n");
            return false;
        }

        pPudCode += wmmpHeadSms[0];

        sms_pdu->tp_udl = sms_pdu->tp_udl - wmmpHeadSms[0] - 1;/*有用信息数据的长度 */
        memcpy(sms_pdu->tp_ud,pPudCode,sms_pdu->tp_udl);
        memcpy(wmmp_port_head,wmmpHeadSms,wmmpHeadSms[0]+1);

        WMMP_DBG_TEMP("PUSH SMS is from M2M platform:udl=%x\n",sms_pdu->tp_udl );			
        //WPI_hex_printf((char *)&(sms_pdu->tp_ud[0]),sms_pdu->tp_udl);	

        return true;

    }
    else
    {
        WMMP_DBG_TEMP("PUSH SMS smCoding tp_udhi ERR	err:V\n");
        return false;
    }


    return true;				
}

/* push  短信处理:是否符合WMMP 协议头*/
bool simcom_wmmp_push_sms_deal(char* sms_data,SMS_PUD *sms_pdu,char* wmmp_port_head)
{
    //SMS_PUD sms_pdu;
    short lenn;
    char* pPudCode;// =(char*)sms_data;	
    unsigned char  alphabet;
    char  wmmpHeadSms[20] = {0} ;
    unsigned short	loopi = 0;

    WMMP_DBG_TEMP("simcom_wmmp_push_sms_deal");

    /**********************************************************************************************************************************
     *           exsamp :         例子
     *    640AA10156484063210490110371318023750B05040000C00200030401010069000E000000010300800041534D3730303037323232323232323200E02500083132333435363738
     *    E0270004AAAAAAAAE02800083837363534333231E02A0004AAAAAAAAE0360018313132323333343435353636373738383939303031323334E0380004AAAAAAAA
     ***********************************************************************************************************************************/
    pPudCode = (char*)sms_data;
    //WPI_hex_printf((char *)pPudCode,100);	

    //  get FO (exsamp : 0x64 --->  bit6 = 1 --- >   VHDI = 1)
    sms_pdu->tp_firstOctet.tp_ByteOfOctet = *pPudCode;
    pPudCode ++;	

    WMMP_DBG_TEMP("deal oa *pPudCode %02x",*pPudCode);
    // get length os TP-OA   (exsamp : 0x0a/2 + 2 = 7字节)
    lenn = (((*pPudCode)%2)?((*pPudCode+1)/2):((*pPudCode)/2) )+ 2;
    memcpy(sms_pdu->tp_oa,pPudCode,lenn);
    WMMP_DBG_TEMP("deal oa len %d",lenn);
    pPudCode += lenn;
    //WPI_hex_printf((char *)sms_pdu->tp_oa,lenn);	
    WMMP_DBG_TEMP("deal pid *pPudCode %02x",*pPudCode);
    // get pid  (example : = 0x21)  
    sms_pdu->tp_pid = *(pPudCode++);

    // get dcs  (example : = 0x04)  
    sms_pdu->tp_dcs = *(pPudCode++);

    // get time stamp  (example: = 90110371318023)
    memcpy(sms_pdu->tp_scts,pPudCode,7);
    pPudCode += 7;
    // WPI_hex_printf((char *)sms_pdu->tp_scts,7);

    // get the user data lenth (example := 0x75)
    sms_pdu->tp_udl = *(pPudCode++);
    WMMP_DBG_TEMP("deal tp_udl %02x",sms_pdu->tp_udl);
    //memcpy(sms_pdu->tp_ud,pPudCode,sms_pdu->tp_udl);	
    //WPI_hex_printf((char *)sms_pdu->tp_ud,sms_pdu->tp_udl);

    pushSmsDecodeDcsPeer(sms_pdu->tp_dcs,&alphabet);

    WMMP_DBG_TEMP("xpid=%x,dcs=%x,code=%x\n", sms_pdu->tp_pid,sms_pdu->tp_dcs,alphabet);

    if(alphabet == NF_SMS_ALPHABET_7_BIT)
    {
        WMMP_DBG_TEMP("PUSH SMS smCoding err: not 8 bit dcs\n");
        return false;
    }
    else if(alphabet == NF_SMS_ALPHABET_8_BIT_DATA)
    {

    }
    else if(alphabet == NF_SMS_ALPHABET_16_BIT)
    {
        WMMP_DBG_TEMP("PUSH SMS smCoding err: not 8 bit dcs\n");
        return false;
    }	

    /*chongqing send sms without udhi*/
//    if (1 == (sms_pdu->tp_firstOctet.tp_bitOfOctet.tp_udhi&1))
    if (1)
    {
        //0B 05 04 00 00 C0 02 00 03 04 01 01
        //{0x0B,0X05,0X04,0X0B,0X2E,0XC0,0X02,0X00,0X03,0X04,0X01,0X01};/*平台短信WDP Head */
        WMMP_DBG_TEMP("wmmp push deal *pPudCode %x",*pPudCode);
        wmmpHeadSms[0] = *(pPudCode++);
        WMMP_DBG_TEMP("wmmp push wmmpHeadSms[0] %x",wmmpHeadSms[0]);
        if (wmmpHeadSms[0] != 0x0b)
        {
            WMMP_DBG_TEMP("PUSH SMS smCoding err: wmmpHeadSms[0] != 0x0b\n");
            return false;
        }

        memcpy(&wmmpHeadSms[1],pPudCode,0x0b);

        if ((wmmpHeadSms[1]!=5) ||(wmmpHeadSms[2]!=4) 
                ||(wmmpHeadSms[5]!=0xc0) ||(wmmpHeadSms[6]!=0x02))
        {
            WMMP_DBG_TEMP("PUSH SMS smCoding err:wmmpHeadSms[0] port err!= 0x0b\n");
            return false;
        }

        WMMP_DBG_TEMP("wmmp push 2 *pPudCode 0x%x",*pPudCode);
        pPudCode += 0x0b;
        WMMP_DBG_TEMP("wmmp push 3 *pPudCode 0x%x",*pPudCode);
        /*去掉wmmp head*/
        sms_pdu->tp_udl = sms_pdu->tp_udl - 0x0b - 1;/*有用信息数据的长度 */
        memcpy(sms_pdu->tp_ud,pPudCode,sms_pdu->tp_udl);

        memcpy(wmmp_port_head,wmmpHeadSms,0x0b+1);

        WMMP_DBG_TEMP("PUSH SMS is from M2M platform:udl=%x\n",sms_pdu->tp_udl );	
    }
    else
    {
        WMMP_DBG_TEMP("PUSH SMS pdu type:%x udhi is error!",sms_pdu->tp_firstOctet.tp_ByteOfOctet);	
        return false;;
    }
    return true;				
}

/* 取出PUSH SMS 的OA */
bool get_pushsms_oa(char *num_buf,SMS_PUD *sms_pud)
{
	char *oa_num_str = NULL;
	bool oa_flag = false;
	unsigned char lenn;
	char * oa_pt = NULL;	
	unsigned char cntt = 0;

	WMMP_DBG_TEMP("get_pushsms_oa ");			
	//memset(num_buf,0,sizeof(num_buf));
	oa_num_str = (char *)num_buf;				
	oa_pt = sms_pud->tp_oa;
	
	lenn = ((*oa_pt%2)?(*oa_pt+1)/2:*oa_pt/2) + 2;//  0x0a/2 +2 = 7
	oa_pt++;//0xa1
	lenn --;  // 7-1 = 6

	//WMMP_DBG_TEMP("m2m center num=%x,%x\n",*oa_pt,lenn );				
	if (*oa_pt == 0x91)
	{
		memcpy(oa_num_str,(char*)"+",1);
		oa_num_str += 1;
	}
	oa_pt++;  // 
	lenn -- ;  //  6-1 = 5

	//WMMP_DBG_TEMP("oa  org[%d] valu 0x%02x ",lenn,*oa_pt);	
	// 0156484063 => "1065840436"
	for(cntt = 0; cntt < lenn;cntt++)
	{
		*oa_num_str = ((*oa_pt)&0x0f) + '0';					
		//WMMP_DBG_TEMP("m2m 1 center num=%s,%x",num_buf,*oa_num_str);				
		//WMMP_DBG_TEMP("m2m 1 num_buf[%d]=%x\n",cntt*2,num_buf[cntt*2]);				
		oa_num_str++;

		if ((*oa_num_str ) > '9')
		{
			WMMP_DBG_TEMP("m2m center num ERR\n" );		
			oa_flag  = true;
			break;						
		}

		if( (((*oa_pt)>>4)&0x0f) != 0x0f)
		{
			*oa_num_str = (((*oa_pt)>>4)&0x0f) + '0'; 
			//WMMP_DBG_TEMP("m2m 2 center num=%s,%x",num_buf,*oa_num_str);
			//WMMP_DBG_TEMP("m2m 2 num_buf[%d]=%x\n",cntt*2+1,num_buf[cntt*2+1]);		
			oa_num_str ++;

			if ((*oa_num_str ) > '9')
			{
				WMMP_DBG_TEMP("m2m center num ERR\n");		
				oa_flag  = true;
				break;						
			}
		}					
		oa_pt ++ ;					
	}
	
	WMMP_DBG_TEMP("center num %s",num_buf);	


	return oa_flag;
}

/********************************************************************
* Function    :    simcom_wmmp_check_sms
* Description :    
* Autor       :    maobin
* Parameters  :    [IN]   all sms pdu data ,include head,sc ..
*                  [OUT]  only sms content body
* Returns     :    Be wmmp sms  -- true
*                  others   -- false
********************************************************************/
/*是否符合wmmp server sms格式，返回true或false*/
bool simcom_wmmp_check_sms(EatSms_st* in_sms, EatSms_st* out_body)
{
    /*  
     * 内容格式,包含了一个PDU-Type:0x44
     * 44 0d 91 68 51 28 61 06 92 f3 00 04 21 50 82 81 34 34 23 12 0b 05 04 0b 2e c0 02 00 03 04 01 01 31 32 33 34 35 36 
     * 发送内容：
     * 0891683108200105f051000da1685128610692f30004ff120b05040b2ec0020003040101313233343536
     * */
    u8 wmmp_port_head[0x0c]="\0";	
    bool rett = true;	
    char* pPudCode = NULL;// =(char*)sms_data;	
    char num_buf[21] = {0};
    bool oa_flag = false;
    WmmpCfgContext_t   *pt1 = NULL ;
    SMS_PUD sms_pdu = {0};

    WMMP_DBG_TEMP(" simcom_wmmp_check_sms");
    if(in_sms == NULL || out_body == NULL)
    {
        WMMP_DBG_TEMP(" simcom_wmmp_check_sms");
        return false;
    }
    pPudCode = in_sms->data;
    if (NULL != in_sms->data)
    {
        memset((void *)&sms_pdu ,0,sizeof(SMS_PUD));		
        WMMP_DBG_TEMP("CheckNewSms *pPudCode %02x",*pPudCode);

        if (!simcom_wmmp_push_sms_deal(pPudCode,&sms_pdu,wmmp_port_head))
        {
            WMMP_DBG_TEMP(" simcom_wmmp_check_sms() return false\n");
            rett = false;
        }
        else
        {
            /*  获取取OA  地址*/
            memset(num_buf,0,sizeof(num_buf));
            oa_flag = get_pushsms_oa(num_buf,&sms_pdu);			
            WMMP_DBG_TEMP("CheckNewSms num_buf %s ",num_buf);
            if (!oa_flag)/*  OA 地址正确*/
            {
                pt1 = (WmmpCfgContext_t *)WPA_ptrToConfigTLV();			
                WMMP_DBG_TEMP("CheckNewSms SMSpecialNum %s ",pt1->M2MSMSpecialNum);
                /* 是否来自M2M 平台数据 */
                if(0 == strcmp(num_buf,pt1->M2MSMSpecialNum)) 
                {
                    /**********************************************************************************************
                     *			void WPA_Decode(WmmpTransType_e type, Char* data ,u16 length);
                     * 发送解码请求,当收到短信时,必须调用此函数来使WMMP协议栈进行解码操作
                     * 注意当收到短信时,必须先判断短信的userDataHeader标志是否为true,
                     * 短信号码是否为服务器号码,以确定该短信为WMMP报文, 
                     * 且短信收到以后需要直接把它删除以免SIM卡短信满以后无法继续收短信
                     **********************************************************************************************/
                    WMMP_DBG_TEMP("CheckNewSms PUSH SMS:len=%d\n",sms_pdu.tp_udl);
                    
                    out_body->len = sms_pdu.tp_udl>170?170:sms_pdu.tp_udl;
                    memcpy(out_body->data, sms_pdu.tp_ud, sms_pdu.tp_udl);
                    out_body->data[out_body->len] = '\0';
                }
                else
                {
                    WMMP_DBG_TEMP("not from M2M 	platform push sms\n");
                    rett = false;
                }
            }	
            else
            {
                WMMP_DBG_TEMP("M2M platform num err\n");
                rett = false;
            }

        }

    }
    else
    {
        WMMP_DBG_TEMP("CheckNewSms sms_data null err\n");
        rett = false;
    }
    WMMP_DBG_TEMP("CheckNewSms return %d",rett);
    return rett;	   
}

/********************************************************************
* Function    :    wmmp_SendSMS
* Description :    
* Autor       :    maobin
* Parameters  :    message - sms content
*                  num     - destination number
*                  messagelen - len
* Returns     :    
********************************************************************/

void wmmp_SendSMS(char *message, char *num, u8 messagelen)
{
    simcom_wmmp_push_sms_send_pdu(message, messagelen, num);
}

#endif

