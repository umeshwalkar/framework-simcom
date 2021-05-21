/******************************************************************************
 *        
 *     TPL GSM Phase 2 Application Layer    
 *     Copyright (c) 2008 SIMCom Ltd.       
 *        
 *******************************************************************************
 *  file name:          
 *  author:             zhangyuechuan
 *  version:            1.00
 *  file description:   
 *******************************************************************************
 *  revision history:    date               version                  author
 *
 *  change summary:
 *
 ******************************************************************************/
#if defined(__MODEM_WMMP30_SIMCOM__)

#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "simcom_wmmp_utility.h"

#include "wmmp_typ.h"
#include "wmmp_lib.h"
#include "wmmp_decode.h"
#include "wmmp_interface.h"
#include "wmmp_crc.h"
#include "wmmp_md5.h"
#include "wmmp_tlv.h"
//#include "wmmp_util.h"
//#include "wmmp_encode.h"
#include "wmmp_3des.h"

//extern bool ReadyLoginOut;
extern ProtocolStatus_e gM2MStatus;



WmmpDecodeRecvResult_e Decrypt(PDUContext_t   *pduContext)
{
    char *p;
    u16 i,length;
    char *decrypt_ret;
    WmmpContext_t *mainContext = ptrToWmmpContext();
    //if(pduContext->SecuritySet&(1<<6))
    //{

    WMMP_LIB_DBG_TEMP("Decrypt SecuritySet=%d\n",pduContext->SecuritySet);
    //if(pduContext->SecuritySet&(1<<4))
    if(pduContext->SecuritySet&(1<<4))
    {
        WMMP_LIB_DBG_TEMP("Decrypt 	SecuritySet ok\n");

        p=pduContext->BodyConst;
        i=Readu16(p);

        if(i==0xE042)
        {
            WMMP_LIB_DBG_TEMP("Decrypt 	SecuritySet 0xE042\n");

            p+=2; 
            length=Readu16(p);
            p+=2;
            decrypt_ret = (char*)WPI_Malloc(length);
            //KiAllocZeroMemory(length, (void**) &decrypt_ret);
            m2m_3des_set_2key((u8*)mainContext->SecurityContext.convPublicKey);
            m2m_3des_decrypt((u8*)p,(u8*)decrypt_ret,(u32)length);

            p=pduContext->BodyConst;
            memcpy(p,decrypt_ret,length);
            //CUSTOMER_PRINT_DATA2(p,length);
            WMMP_LIB_DBG_TEMP("Decrypt:WPI_hex_printf()\n");		
            // WPI_hex_printf((char * )pduContext->BodyConst, (u16) length);

            p+=length;
            *p=0;  //防止报文检查tag 报错
            pduContext->BodyLength = length;
            WPI_Free(decrypt_ret);
            //KiFreeMemory((void **) &decrypt_ret);


        }
    }
    //}
    return WMMP_DECODE_OK;
}


WmmpDecodeRecvResult_e DecodeAbstract(PDUContext_t   *pduContext)
{
    WmmpContext_t *mainContext = ptrToWmmpContext();
    TLVRetVal_t TLVret;
    char *p;
    char md5[16];
    char md5_1[16];

    WMMP_LIB_DBG_TEMP("DecodeAbstract():SecuritySet=%d,TransType=%d\n",pduContext->SecuritySet,mainContext->TransType);
    if(pduContext->SecuritySet&(1<<5))	
    {
        WMMP_LIB_DBG_TEMP("pduContext->SecuritySet&(1<<5)l\n");

        /* add by CBC@20100516 :    不要MD5 ,begin*/
        if (mainContext->TransType==WMMP_TRANS_SMS)
        {
            pduContext->BodyLength=pduContext->SendLength-PDU_HEADER_LENGTH;
            return WMMP_DECODE_OK;
        }
        else
            /* add by CBC@20100516 :    不要MD5 ,end*/
        {
            memcpy(md5,mainContext->IOBuffer+pduContext->SendLength-16,16);
            pduContext->BodyLength=pduContext->SendLength-PDU_HEADER_LENGTH-PDU_ABSTRACT_LENGTH;

            p = mainContext->IOBuffer;
            p+=pduContext->SendLength-PDU_ABSTRACT_LENGTH;

            if(pduContext->CommandID==LOGIN_ACK_COMMAND)
            {
                memcpy(p,(pduContext->BodyConst)+1,4);

                WMMP_LIB_DBG_TEMP("LOGIN_ACK TimeStamp[0] %x,[1] %x",*p,*(p+1));	
                WMMP_LIB_DBG_TEMP("LOGIN_ACK TimeStamp[2] %x,[3]%x",*(p+2),*(p+3));	

            }
            else
            {
                memcpy(p,mainContext->TimeStamp,4);

                WMMP_LIB_DBG_TEMP("ABSTRACT TimeStamp[0] %x,[1] %x",mainContext->TimeStamp[0],mainContext->TimeStamp[1]);	
                WMMP_LIB_DBG_TEMP("ABSTRACT TimeStamp[2] %x,[3]%x",mainContext->TimeStamp[2],mainContext->TimeStamp[3]);	

            }
            p+=4;

            ReadTLV(0xE00E,&TLVret);
            memcpyTLV(TLVret,p);
            p+=TLVret.length;

            ReadTLV(0xE014,&TLVret);
            if (*TLVret.addr ==0)
            {				
                WMMP_LIB_DBG_TEMP("tlv e014 is null\n");	 
                ReadTLV(0xE012,&TLVret);
            }
            memcpyTLV(TLVret,p);
            p+=TLVret.length;

            ReadTLV(0xE028,&TLVret);
            memcpyTLV(TLVret,p);
            p+=TLVret.length;

            WMMP_LIB_DBG_TEMP("DecodeAbstract: wmmp_md5  WPI_hex_printf()\n");			
            //WPI_hex_printf((u8*)(mainContext->IOBuffer),(u32)(p-mainContext->IOBuffer));//    
            wmmp_md5((u8*)(mainContext->IOBuffer),(u32)(p-mainContext->IOBuffer),(unsigned char*)md5_1);
            WMMP_LIB_DBG_TEMP("DecodeAbstract: wmmp_md5  WPI_hex_printf()\n");			
            //WPI_hex_printf((u8*)(md5),16);//    
            //WPI_hex_printf((u8*)(md5_1),16);//    
            if(memcmp(md5,md5_1,16)==0)
            {
                WMMP_LIB_DBG_TEMP("MD5 right!!!\n");	 
                return WMMP_DECODE_OK;
            }
            else
            {
                WMMP_LIB_DBG_TEMP("MD5 ERR!!!\n");
                //CUSTOMER_PRINT_DATA2(mainContext->IOBuffer,p-mainContext->IOBuffer);
                return WMMP_DECODE_ERROR;
            }
        }
    }
    else
    {
        WMMP_LIB_DBG_TEMP("!pduContext->SecuritySet&(1<<5)l\n");
        pduContext->BodyLength=pduContext->SendLength-PDU_HEADER_LENGTH;
    }
    return WMMP_DECODE_OK;
}


/*******************************************************
  20100418@cbc:  分包的例子
  全报文 :00480006000000030300300041485731303030313038414231323334400800140000000300010000010004000300014009000100e021001069a4a63786074163d25c185d391a076e
  报文体:40080014  0000         0003          0001      00    00010004 00030001  4009000100
  transid     3个包      第1               

ACK :  4008000700000003000201
40080007    0000                 0003                            0002                                      01
transid                                请求第二个包                    

 *************************************************************/
WmmpDecodeRecvResult_e DecodeDispartProcess(PDUContext_t   *pduContext)
{
    char *p = pduContext->BodyConst;
    WmmpContext_t *mainContext = ptrToWmmpContext();
    char *body_tlv;
    u16 length;
    char TransID[2];
    u16 partnum;
    u16 partallnum;
    char oper;

    WMMP_LIB_DBG_TEMP("DecodeDispartProcess in");
    if(pduContext->SecuritySet&(1<<4))
    {
        WMMP_LIB_DBG_TEMP("DecodeDispartProcess ok");
        body_tlv=p;
        if (Readu16(p)==0x4008)/*cbc220100402:  分包标识  */
        {
            WMMP_LIB_DBG_TEMP("DecodeDispartProcess 0x4008");	
            p+=2;
            length = Readu16(p);
            p+=2;
            if (length==7)/*cbc220100402:  表示收到的是来自  平台的应答包,此时终端继续发送下一个分包 */
            {
                //it's a dispart ack
                WMMP_LIB_DBG_TEMP("continuesend");

                memcpy(TransID,p,2);
                memcpy(pduContext->TransID,TransID,2);
                p+=2;
                partallnum=Readu16(p);
                pduContext->partallnum=partallnum;
                WMMP_LIB_DBG_TEMP("partallnum=%x",partallnum);
                p+=2;
                partnum = Readu16(p);
                pduContext->partnum=partnum;
                WMMP_LIB_DBG_TEMP("partnum=%x",partnum);
                p+=2;
                oper=*p;
                pduContext->oper = oper;
                WMMP_LIB_DBG_TEMP("partnum=%x",oper);				

                SendCommand(PNULL);				
                wmmpStopInterTimer(SEND_PDU_TIMEOUT_TIMER);				

            }
            else/*cbc220100402:  非应答包，收集各个分包的数据  */
            {
                memcpy(TransID,p,2);
                WMMP_LIB_DBG_TEMP("DecodeDispartProcess TransID  WPI_hex_printf()");			
                WPI_hex_printf(TransID,2);//    

                p+=2;
                partallnum=Readu16(p);
                WMMP_LIB_DBG_TEMP("partallnum=%x",partallnum);
                p+=2;
                partnum = Readu16(p);
                WMMP_LIB_DBG_TEMP("partnum=%x",partnum);
                p+=2;
                oper=*p;
                pduContext->oper = oper;
                WMMP_LIB_DBG_TEMP("partnum=%x",oper);
                p++;
                //next is data ,save the dispart

                if(partnum==1)/*cbc220100402:  第一包  */
                {
                    //save first frame;
                    memcpy(mainContext->FisrtFrameNo,mainContext->ReceiveFrameNo,4);
                    WMMP_LIB_DBG_TEMP("DecodeDispartProcess FisrtFrameNo  WPI_hex_printf()");			
                    WPI_hex_printf(mainContext->FisrtFrameNo,4);//    

                    pduContext->Totallength= 0;
                    pduContext->partnum=1;/* cbc20100419:第 一包*/
                    memcpy(pduContext->TransID,TransID,2);
                    pduContext->dispartFlag = true;/* 多包处理开始标识 */
                    //memcpy(mainContext->ReceiveBuffer,pduContext->BodyConst,pduContext->Totallength);

                    if(mainContext->StackState ==WMMP_STACK_SERVER_WAITING)
                    {
                        WMMP_LIB_DBG_TEMP("WMMP_STACK_SERVER_CONTINUE");

                        mainContext->StackState =WMMP_STACK_SERVER_CONTINUE; //eg REQ COMMAND
                    }
                    else 
                        if(mainContext->StackState ==WMMP_STACK_CLIENT_WAITING)
                        {
                            WMMP_LIB_DBG_TEMP("WMMP_STACK_CLIENT_WAITING");

                            mainContext->StackState = WMMP_STACK_CLIENT_CONTINUE;  //eg SET COMMAND				
                        }
                    //pduContext->BodyConst = mainContext->ReceiveBuffer;
                }
                else
                {
                    (pduContext->partnum) ++;
                }

                memcpy((mainContext->ReceiveBuffer)+(pduContext->Totallength),p,length-7);
                WMMP_LIB_DBG_TEMP("DecodeDispartProcess FisrtFrameNo WPI_hex_printf(),len=%d,%d",pduContext->Totallength,length-7);			
                //WPI_hex_printf(mainContext->ReceiveBuffer,(pduContext->Totallength)+length-7);//    


                pduContext->Totallength+=(length-7);     
                WMMP_LIB_DBG_TEMP("pduContext->Totallength=%d,pduContext->partnum=%d",pduContext->Totallength,pduContext->partnum);
                //this may have some question

                //if(partnum!=partallnum)/*cbc220100402: 如果不是最后一个分包需要终端向平台发送子包应答包 */
                if ((pduContext->partnum) !=partallnum)					
                {
                    WMMP_LIB_DBG_TEMP("partnum!=partallnum,CommandID=%d,TransID=%d,partnum=%d,partallnum=%d,",pduContext->CommandID,TransID,partnum,partallnum);

                    wmmpServerContinueSend(pduContext->CommandID,TransID,partnum,partallnum);
                    wmmpStopInterTimer(SEND_PDU_TIMEOUT_TIMER);/*cbc220100402:  				  */

                }
                else/*cbc220100402:  最后一个分包,需要发送报文应答包  */
                {
                    pduContext->BodyConst = mainContext->ReceiveBuffer;
                    //解析报文时候都是用BodyLength来判断长度的
                    pduContext->BodyLength = pduContext->Totallength;
                    WMMP_LIB_DBG_TEMP("all data has received,length=%d",pduContext->BodyLength);

                    if(mainContext->StackState ==WMMP_STACK_SERVER_CONTINUE)
                    {
                        WMMP_LIB_DBG_TEMP("WMMP_STACK_SERVER_CONTINUE");

                        mainContext->StackState =WMMP_STACK_SERVER_WAITING; //eg REQ COMMAND
                    }
                    else if(mainContext->StackState ==WMMP_STACK_CLIENT_CONTINUE)
                    {
                        WMMP_LIB_DBG_TEMP("WMMP_STACK_CLIENT_CONTINUE");

                        mainContext->StackState = WMMP_STACK_CLIENT_WAITING;  //eg SET COMMAND						
                        wmmpStopInterTimer(SEND_PDU_TIMEOUT_TIMER);
                    }

                    //has receive all the data
                    return WMMP_DECODE_OK;
                }

            }
            return WMMP_DECODE_CONTINUE;
        }
    }

    return WMMP_DECODE_OK;
}

u8   urcRegTerminalIDFlag = 0;
WmmpDecodeRecvResult_e DecodeHeadData(u16 length)
{
    WmmpContext_t *mainContext = ptrToWmmpContext();
    PDUContext_t   *pduContext = &mainContext->PDUContext_receive;
    char TerminalNum[17];
    char *p=mainContext->IOBuffer;
    u16 CommandID;
    u16		loopi = 0;
    /*
       if(pduContext->PDUState==WMMP_PDU_INIT)//it means it's a new PDU
       {
       InitPDUContext(pduContext);
       }*/
    // check is it a full PDU
    WMMP_LIB_DBG_TEMP("DecodeHeadData length 0x%x",length);
    WMMP_LIB_DBG_TEMP("DecodeHeadData *p 0x%x",*p);

    pduContext->SendLength= Readu16(p); //报文总长度(2byte)
    WMMP_LIB_DBG_TEMP("DecodeHeadData SendLength 0x%x",pduContext->SendLength);
    if(pduContext->SendLength==length)
    {
        p+=2;
        CommandID = Readu16(p);//command id  (2byte)
        WMMP_LIB_DBG_TEMP("Recv CommandID %x",CommandID);

        pduContext->CommandID = CommandID;	

        p+=2;
        //should switch commnd id in the futrue
        memcpy(mainContext->ReceiveFrameNo,p,4);//保文流水号(4byte)

        WMMP_LIB_DBG_TEMP("DecodeHeadData ReceiveFrameNo %d",mainContext->ReceiveFrameNo);
        WMMP_LIB_DBG_TEMP("DecodeHeadData Frame num ReceiveFrameNo %d,FrameNo %d",Readu32(mainContext->ReceiveFrameNo),mainContext->FrameNo);

#if 0	/*bob remove 20110323 for after gprsnotactive resend register error*/
        //check the frameNO
        //if((mainContext->StackState==WMMP_STACK_SERVER_WAITING)||(mainContext->StackState==WMMP_STACK_SERVER_CONTINUE))
        {
            u32 RecvFrameNo;
            RecvFrameNo = Readu32(mainContext->ReceiveFrameNo);
            if(RecvFrameNo != mainContext->FrameNo)
            {
                WMMP_LIB_DBG_TEMP("Frame num error,%d,%d",RecvFrameNo,mainContext->FrameNo);
                return WMMP_DECODE_FRAME_ERROR;
            }
            else			
            {
                WMMP_LIB_DBG_TEMP("RecvFrameNo = mainContext->FrameNo,Recv CommandID %x",CommandID);
            }
        }
        else			
        {
            WMMP_LIB_DBG_TEMP("Recv CommandID %x",CommandID);
        }
#endif

        p+=6;//报文协议版本(2byte)
        pduContext->SecuritySet = *p;//报文安全标识(1byte)
        p+=2;//保留字(1byte)


        memset(TerminalNum,0,17);
        memcpy(TerminalNum,p,16);//终端序列号(16byte)

        /*bob modify 20101209*/
        /*if((strcmp((char*)mainContext->StatusContext.TerminalSeriesNum,(char*)TerminalNum)!=0)&&(mainContext->PDUContext_receive.CommandID!=REGISTER_ACK))*/
        if((strcmp((char*)mainContext->StatusContext.TerminalSeriesNum_Bak,(char*)TerminalNum)!=0) && (strcmp((char*)mainContext->StatusContext.TerminalSeriesNum,(char*)TerminalNum)!=0)&&(mainContext->PDUContext_receive.CommandID!=REGISTER_ACK))
        {
            WMMP_LIB_DBG_TEMP("terminal num error,%s,%s,bak %s",mainContext->StatusContext.TerminalSeriesNum,TerminalNum,mainContext->StatusContext.TerminalSeriesNum_Bak);
            return WMMP_DECODE_NOPDU;
        }
        else
        {
            WMMP_LIB_DBG_TEMP("Recv CommandID %x",CommandID);
        }
        //check the terminalNum

        //判断报文为WMMP 报文后置状态
        if(mainContext->StackState==WMMP_STACK_INIT)
        {
            WMMP_LIB_DBG_TEMP("mainContext->StackState==WMMP_STACK_INIT");
            mainContext->StackState=WMMP_STACK_CLIENT_WAITING;			
        }	

        //pduContext->DispartHead = des;
        pduContext->BodyConst = mainContext->IOBuffer+PDU_HEADER_LENGTH;

    }
    else
    {
        //in future complete this part
        WMMP_LIB_DBG_TEMP("only received partly data!!");
        return WMMP_DECODE_NOPDU;
    }

    return WMMP_DECODE_OK;
}

WmmpDecodeRecvResult_e DecodeRegisterAck(char *des)
{
    char *p=des;
    WmmpContext_t *mainContext_p = ptrToWmmpContext();
    WPIEventData_t WPIEventData;
    u8 RegRet=  *p; //Register result

    WMMP_LIB_DBG_TEMP("DecodeRegisterAck Register ack result =%d\n",RegRet);
    gM2MStatus = WMMP_STATE_REGISTERING_TERMID_BEGIN;
    /*bob remove 20101126*/
    //memcpy(mainContext_p->StatusContext.FirmwareRev,(const char*)TERMINAL_SOFTWARE_VERSION,8);	
    if((RegRet==0)||(RegRet==1))
    {
        p++;
        //directly write , for can't use WriteTLV to write 0x3002
        memcpy(mainContext_p->StatusContext.TerminalSeriesNum,p,16);
        memcpy(mainContext_p->StatusContext.TerminalSeriesNum_Bak,p,16);

        //memcpy(mainContext_p->StatusContext.TerminalSeriesNum,p,16);
        WMMP_LIB_DBG_TEMP("get TerminalSeriesNum ID,%s\n",mainContext_p->StatusContext.TerminalSeriesNum);
        //WriteTLV(0x3002, 16, p);
        //memcpy(mainContext->StatusContext.TerminalSeriesNum,p,16);

    }

    //save terminalservies num
    // here don't save terminal serivies num, after SECURITY_CONFIG_COMMAND,save it 

    //WPI_WriteNvram(WMMP_TLV_STATUS);   /*bob remove 20101201*/	
    wmmpProcessRegAck(RegRet);


    /*bob add 20101201 -s*/
    if((RegRet==0)||(RegRet==1)||(RegRet==9))
    {
        mainContext_p->maxRegisterTimeOutCount = 0;
        wmmpStopInterTimer(TIMING_REGISTER_TIMER);	

        WPI_send_urc(WMMP_AT_EVENT_STR"%d,%s %s", WMMP_AET_LIB_STATE, "0101", "\"Registering:with preset Terminal ID\"");
    }	
    else
    {
        WPI_send_urc(WMMP_AT_EVENT_ACK_STR"%s %d", "Register failed: result is", RegRet);
    }
    /*bob add 20101201 -e*/


    //status_ind should always send to ci
    if(mainContext_p->WPIEventFucPtr!=PNULL)
    {
        WPIEventData.WPIStatusIndEvent.type = 1;

        if((RegRet==0)||(RegRet==1)||(RegRet==9))
        {
            WPIEventData.WPIStatusIndEvent.value= REGISTERSUCCUSSFUL;	
        }
        else
        {
            WPIEventData.WPIStatusIndEvent.value= REGISTERFAIL;	
        }

        WPIEventData.WPIStatusIndEvent.reason = mainContext_p->PDUUsage;
        (mainContext_p->WPIEventFucPtr)(WPI_STATUS_IND_EVENT,&WPIEventData);	 /*  CBC@注册成功 */		
    }		

    return WMMP_DECODE_OK;
}

u8 syncSystemFlag = 0;

u32 AsciiToDecu32(const u8 *hexString_p)
{
    u32 result = 0, pos = 0;
    u8 value;

    WMMP_LIB_DBG_TEMP("AsciiToDecu32 hexString_p[%d] 0x%02x",pos,hexString_p[pos]);
    while ((hexString_p[pos] != '\0') && (pos < 8))
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
        result = (result * 10) + value;
        WMMP_LIB_DBG_TEMP("AsciiToDecu32 result[%d] 0x%02x",pos,result);
        pos++;
    }
    WMMP_LIB_DBG_TEMP("AsciiToDecu32 result %d 0x%4x",result,result);
    return result;
}

u8 AsciiTou8(const u8 *hexString_p)
{
    u8 result = 0, pos = 0;
    u8 value;

    WMMP_LIB_DBG_TEMP("AsciiTou8 hexString_p[%d] 0x%02x",pos,hexString_p[pos]);
    while ((hexString_p[pos] != '\0') && (pos < 2))
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
        result = (result * 16) + value;
        WMMP_LIB_DBG_TEMP("AsciiTou8 result[%d] 0x%02x",pos,result);
        pos++;
    }
    WMMP_LIB_DBG_TEMP("AsciiTou8 result %d 0x%2x",result,result);
    return result;
}


WmmpDecodeRecvResult_e DecodeLoginAck(char *des)
{
    u32         ucurrentTime = 0;
    //t_hclk_Date_Time rtcDateAndTime_n;	
    //RtcDateAndTimeType vl_DateTime;
    u16 		loopi = 0;

    //TODO: 这里需要重新修改，不要使用系统接口
    //applib_time_struct  curr_time, time_restlt, time_restlt_ext;
    u32  local_sec = 0;
    u32  utc_sec = 0, utc_sec_2 = 0;
    u64   utc_sec_ext = 0;

    u16 nYear;   

    WPIRtcDateAndTime dateAndTime;
    WPIRtcStatus_e      rtcStatus;



    char inputCurTime[25] = "\0" ;
    char *p=des;
    WmmpContext_t *mainContext_p = ptrToWmmpContext();

    u8 LogRet=  *p; //Register result
    p++;

    WMMP_LIB_DBG_TEMP("DecodeLoginAck LogRet = %d",LogRet);

    if((LogRet==0)||(LogRet==1)||(LogRet==8))
    {
        memcpy(mainContext_p->TimeStamp,p,4);

        /*不注册登录，放在发送的地方太早可能还没有建立好连接*/
        WPI_send_urc(WMMP_AT_EVENT_STR"%d,%s %s", WMMP_AET_LIB_STATE, "0302", "\"Logging-in:normal login\"");

        gM2MStatus = WMMP_STATE_LOGGING_IN_NORMAL;
        /*!!!!!!!!!!!!!!!! TODO: 这里需要重新修改，不要使用系统接口 20130130 maobin !!!!!!!!!!!!!*/
#if 0
        if(!syncSystemFlag)
        { 
            syncSystemFlag = 1;
            ucurrentTime = Readu32(mainContext_p->TimeStamp);
            WMMP_LIB_DBG_TEMP("DloginACK m2m ucurrentTime org %d",ucurrentTime);
            WMMP_LIB_DBG_TEMP("DloginACK m2m ucurrentTime new 0x%08x",ucurrentTime);
            applib_dt_utc_sec_2_mytime_ext(ucurrentTime, &time_restlt_ext, true);
            WMMP_LIB_DBG_TEMP("DloginACK m2m 1 time_restlt_ext.nYear %d nMonth %d nDay %d nHour %d nMin %d nSec %d DayIndex %d",time_restlt_ext.nYear,time_restlt_ext.nMonth,time_restlt_ext.nDay,time_restlt_ext.nHour,time_restlt_ext.nMin,time_restlt_ext.nSec,time_restlt_ext.DayIndex);
            ucurrentTime = ucurrentTime+8*60*60;
            WMMP_LIB_DBG_TEMP("DloginACK m2m ucurrentTime new %d",ucurrentTime);
            WMMP_LIB_DBG_TEMP("DloginACK m2m ucurrentTime new 0x%08x",ucurrentTime);
            //_DateTime.dateTimeInSec = ucurrentTime;
            //cConvertFromSecondToDateTime(&vl_DateTime);
            /* get current system date and time */
            applib_dt_get_rtc_time (&curr_time);      /* it get accurate date and time */
            WMMP_LIB_DBG_TEMP("DloginACK m2m curr_time.nYear %d nMonth %d nDay %d nHour %d nMin %d nSec %d DayIndex %d",curr_time.nYear,curr_time.nMonth,curr_time.nDay,curr_time.nHour,curr_time.nMin,curr_time.nSec,curr_time.DayIndex);
            /* convert the date time to second number, the  daylight is true */
            utc_sec = applib_dt_mytime_2_utc_sec(&curr_time, true);
            utc_sec_ext = applib_dt_mytime_2_utc_sec_ext(&curr_time, true);
            WMMP_LIB_DBG_TEMP("DloginACK af utc_sec %d utc_sec_ext %d",utc_sec,utc_sec_ext);

            /* convert the second to date time, the  daylight is true */
            utc_sec += 1;       /* current second add 1 */
            utc_sec_ext += 1;   /* current second add 1 */

            applib_dt_utc_sec_2_mytime(utc_sec, &time_restlt, true);
            WMMP_LIB_DBG_TEMP("DloginACK m2m time_restlt.nYear %d nMonth %d nDay %d nHour %d nMin %d nSec %d DayIndex %d",time_restlt.nYear,time_restlt.nMonth,time_restlt.nDay,time_restlt.nHour,time_restlt.nMin,time_restlt.nSec,time_restlt.DayIndex);
            applib_dt_utc_sec_2_mytime_ext(utc_sec_ext, &time_restlt_ext, true);
            WMMP_LIB_DBG_TEMP("DloginACK m2m time_restlt_ext.nYear %d nMonth %d nDay %d nHour %d nMin %d nSec %d DayIndex %d",time_restlt_ext.nYear,time_restlt_ext.nMonth,time_restlt_ext.nDay,time_restlt_ext.nHour,time_restlt_ext.nMin,time_restlt_ext.nSec,time_restlt_ext.DayIndex);
            local_sec = applib_dt_sec_utc_to_local(utc_sec);
            WMMP_LIB_DBG_TEMP("DloginACK af utc_sec %d local_sec %d",utc_sec,local_sec);
            utc_sec_2 = applib_dt_sec_local_to_utc(local_sec);

            WMMP_LIB_DBG_TEMP("DloginACK af local_sec %d utc_sec_2 %d",local_sec,utc_sec_2);
            /*
               WMMP_LIB_DBG_TEMP("DloginACK af dateTimeInSec %d",vl_DateTime.dateTimeInSec);
               WMMP_LIB_DBG_TEMP("DloginACK vl_DateTime.date.year %d",vl_DateTime.date.year);
               WMMP_LIB_DBG_TEMP("DloginACK vl_DateTime.date.month %d",vl_DateTime.date.month);
               WMMP_LIB_DBG_TEMP("DloginACK vl_DateTime.date.day %d",vl_DateTime.date.day);			
               WMMP_LIB_DBG_TEMP("DloginACK vl_DateTime.time.hours %d",vl_DateTime.time.hours);
               WMMP_LIB_DBG_TEMP("DloginACK vl_DateTime.time.minutes %d",vl_DateTime.time.minutes);
               WMMP_LIB_DBG_TEMP("DloginACK vl_DateTime.time.seconds %d",vl_DateTime.time.seconds);
               */
            //RtcSetTime(&vl_DateTime);
            //vl_DateTime.date.year -= 2000;
            //sprintf(inputCurTime,"%02d\/%02d\/%02d,%02d:%02d:%02d+32",vl_DateTime.date.year,vl_DateTime.date.month,vl_DateTime.date.day,vl_DateTime.time.hours,vl_DateTime.time.minutes,vl_DateTime.time.seconds);
            //agn2_50SetSystemTime(inputCurTime);
        }
#endif

    }

    p+=4;
    if(Readu16(p)==0xE040)
    {
        u16 length;
        char *decrypt_ret;
        p+=2;
        length=Readu16(p);
        p+=2;
        decrypt_ret = (char*)WPI_Malloc(length);
        //KiAllocZeroMemory(length, (void**) &decrypt_ret);
        m2m_3des_set_2key((u8*)mainContext_p->SecurityContext.publickey);        
        m2m_3des_decrypt((u8*)p,(u8*)decrypt_ret,(u32)length);

        //CUSTOMER_PRINT_DATA2(decrypt_ret,length);

        p=decrypt_ret;
        if(Readu16(p)==0xE03B)
        {
            p+=2;
            length=Readu16(p);
            p+=2;
            WriteTLV(0xE03B, length, p);
            p+=length;
        }
        if(Readu16(p)==0xE03D)
        {
            p+=2;
            length=Readu16(p);
            p+=2;
            WriteTLV(0xE03D, length, p);
            p+=length;
        }   
        WPI_Free(decrypt_ret);
    }

#if 1
    /*bob add 20101201  -s*/
    if((LogRet==0)||(LogRet==1)||(LogRet==8))
    {
        mainContext_p->LoginFaildCount = 0;
        wmmpStopInterTimer(TIMING_LOGIN_TIMER);		
    }
    /*bob add 20101201  -e*/
#endif

    wmmpProcessLoginAck(LogRet);

    return WMMP_DECODE_OK;    
}

WmmpDecodeRecvResult_e DecodeLogoutAck(void)
{
    WmmpContext_t *mainContext_p = ptrToWmmpContext();
    WmmpSecurityContext_t  *SecurityContext_p = &(ptrToWmmpContext()->SecurityContext);
    WPIEventData_t WPIEventData;
    WMMPAPNType apn_type = wmmp_get_apn_type();

    char		uplinkPassword_b[9];	
    char		downlinkPassword_b[9];
    char		publickey_b[32];
    u32	socketId_b = INVALID_SOCKET;

    WMMP_LIB_DBG_TEMP("DecodeLogoutAck mainContext_p->PDUUsage=%d",mainContext_p->PDUUsage);

#if 1	/*bob modify 20101122 for remote_control_reboot*/	
    if ((mainContext_p->PDUUsage==WMMP_TLV_REBOOT) ||(mainContext_p->PDUUsage==WMMP_LOCAL_CONFIG_REBOOT)||(mainContext_p->PDUUsage==WMMP_SECURITY_REBOOT) || (mainContext_p->PDUUsage==WMMP_REMOTE_CONTROL_REBOOT) || (mainContext_p->PDUUsage==WMMP_CONFIG_SET_APN_REBOOT) || (mainContext_p->PDUUsage==WMMP_RMCON_REFACTORY_REBOOT))
    {
        if(mainContext_p->PDUUsage==WMMP_REMOTE_CONTROL_REBOOT)
        {
            //mainContext_p->v_rmCtl_reboot_flag = 0;
            WMMP_LIB_DBG_TEMP("DecodeLogoutAck:PDUUsage REMOTE_CONTROL_REBOOT");

        }
        else if(mainContext_p->PDUUsage==WMMP_LOCAL_CONFIG_REBOOT)
        {
            WMMP_LIB_DBG_TEMP("DecodeLogoutAck:PDUUsage WMMP_LOCAL_CONFIG_REBOOT");
        }
        else if(mainContext_p->PDUUsage==WMMP_SECURITY_REBOOT)
        {
            WMMP_LIB_DBG_TEMP("DecodeLogoutAck:mainContext_p->PDUUsage=WMMP_SECURITY_REBOOT");	

            /* 重登陆需要新的密钥上下行密码 */
            if(mainContext_p->uplinkPassword_b[0]!=0)
            {
                memcpy(uplinkPassword_b,SecurityContext_p->uplinkPassword,sizeof(SecurityContext_p->uplinkPassword));	
                memcpy(SecurityContext_p->uplinkPassword,mainContext_p->uplinkPassword_b,sizeof(SecurityContext_p->uplinkPassword));	
                memcpy(mainContext_p->uplinkPassword_b,uplinkPassword_b,sizeof(SecurityContext_p->uplinkPassword));	
            }

            if(mainContext_p->downlinkPassword_b[0]!=0)
            {
                memcpy(downlinkPassword_b,SecurityContext_p->downlinkPassword,sizeof(SecurityContext_p->downlinkPassword));	
                memcpy(SecurityContext_p->downlinkPassword,mainContext_p->downlinkPassword_b,sizeof(SecurityContext_p->downlinkPassword));
                memcpy(mainContext_p->downlinkPassword_b,downlinkPassword_b,sizeof(SecurityContext_p->downlinkPassword));	
            }

            if(mainContext_p->publickey_b[0]!=0)
            {
                memcpy(publickey_b,SecurityContext_p->publickey,sizeof(SecurityContext_p->publickey));	
                memcpy(SecurityContext_p->publickey,mainContext_p->publickey_b,sizeof(SecurityContext_p->publickey));	
                memcpy(mainContext_p->publickey_b,publickey_b,sizeof(SecurityContext_p->publickey));	
            }

        }
        else if(mainContext_p->PDUUsage==WMMP_TLV_REBOOT)  /*ipaddr ,port*/
        {
            WMMP_LIB_DBG_TEMP("DecodeLogoutAck:WMMP_TLV_REBOOT wmmpSocket 0x%x",mainContext_p->wmmpSocket);
            if(mainContext_p->wmmpSocket!=INVALID_SOCKET)
            {	
                socketId_b = mainContext_p->wmmpSocket;
                wmmpOthCreateSocketConnect();				
                //TODO:modigy for apn_type temp 130314
                WPI_CloseSocket(apn_type, socketId_b);				
            }
        }
        else if(mainContext_p->PDUUsage==WMMP_CONFIG_SET_APN_REBOOT)  /*apn*/
        {
            WMMP_LIB_DBG_TEMP("DecodeLogoutAck:PDUUsage==CONFIG_SET_APN_REBOOT");
            //TODO:modigy for apn_type temp 130314
            WPI_DeactiveGPRS(apn_type);

        }
        else if(mainContext_p->PDUUsage==WMMP_RMCON_REFACTORY_REBOOT)
        {
            WMMP_LIB_DBG_TEMP("DecodeLogoutAck:PDUUsage==WMMP_RMCON_REFACTORY_REBOOT");
            /* 重登陆需要old 的密钥上下行密码 */
#if 0
            memset(uplinkPassword_b,0x00,sizeof(uplinkPassword_b));
            memcpy(uplinkPassword_b,SecurityContext_p->uplinkPassword,sizeof(SecurityContext_p->uplinkPassword));	
            memset(downlinkPassword_b,0x00,sizeof(downlinkPassword_b));
            memcpy(downlinkPassword_b,SecurityContext_p->downlinkPassword,sizeof(SecurityContext_p->downlinkPassword));	
            memset(publickey_b,0x00,sizeof(publickey_b));
            memcpy(publickey_b,SecurityContext_p->publickey,sizeof(SecurityContext_p->publickey));			
#endif			
            //wmmp2_90RMCONReFactory();  /*20120509 for compile*/
#if 0
            memset(SecurityContext_p->uplinkPassword,0x00,sizeof(SecurityContext_p->uplinkPassword));
            memcpy(SecurityContext_p->uplinkPassword,uplinkPassword_b,sizeof(SecurityContext_p->uplinkPassword));	
            memset(SecurityContext_p->downlinkPassword,0x00,sizeof(SecurityContext_p->downlinkPassword));
            memcpy(SecurityContext_p->downlinkPassword,downlinkPassword_b,sizeof(SecurityContext_p->downlinkPassword));
            memset(SecurityContext_p->publickey,0x00,sizeof(SecurityContext_p->publickey));
            memcpy(SecurityContext_p->publickey,publickey_b,sizeof(SecurityContext_p->publickey));	
#endif			
            //TODO:modigy for apn_type temp 130314
            WPI_DeactiveGPRS(apn_type);					
        }
        else
        {
            WMMP_LIB_DBG_TEMP("DecodeLogoutAck:PDUUsage== other");
        }	

    }
    else
    {		
        wmmpProcessLogoutCnf();	

        /* add by cbc@20100422:  begin */
        //mainContext_p->Wmmp_status.net_status = LINK_FAILED; /*wmmpSendNextCommand() 时判断是否扔掉登录包*/
        /* add by cbc@20100422:  end */
    }
    /* modified by cbc@20100420  end*/	
    //status_ind should always send to ci
    if(mainContext_p->WPIEventFucPtr!=PNULL)
    {
        WPIEventData.WPIStatusIndEvent.type = 1;
        WPIEventData.WPIStatusIndEvent.value= PIDLE;	
        WPIEventData.WPIStatusIndEvent.reason = mainContext_p->PDUUsage;
        (mainContext_p->WPIEventFucPtr)(WPI_STATUS_IND_EVENT,&WPIEventData);	 /* CBC@正常登出 */		
    }	
#else

    /* modified by cbc@20100420  begiin*/
    //if((mainContext_p->PDUUsage==WMMP_TLV_REBOOT) ||(mainContext_p->PDUUsage==WMMP_LOCAL_CONFIG_REBOOT)||(mainContext_p->PDUUsage==WMMP_SECURITY_REBOOT))
    /*bob modify 20101122 for remote_control_reboot*/
    if ((mainContext_p->PDUUsage==WMMP_TLV_REBOOT) ||(mainContext_p->PDUUsage==WMMP_LOCAL_CONFIG_REBOOT)||(mainContext_p->PDUUsage==WMMP_SECURITY_REBOOT) || (mainContext_p->PDUUsage==WMMP_REMOTE_CONTROL_REBOOT))
    {
        if((mainContext_p->wmmpSocket!=INVALID_SOCKET)
                &&(mainContext_p->PDUUsage!=WMMP_SECURITY_REBOOT)) /* cbc@20100518 ,修改安全参数跟IP 无关就不要重启IP  */
        {
            WMMP_LIB_DBG_TEMP("DecodeLogoutAck:PDUUsage==WMMP_TLV_REBOOT\n");	
            //TODO:modigy for apn_type temp 130314
            WPI_CloseSocket(apn_type, mainContext_p->wmmpSocket);
            //TODO:modigy for apn_type temp 130314
            WPI_DeactiveGPRS(apn_type);	/* WMMP_TLV_REBOOT IP  地址或端口改变需要重新连接吗?*/
        }		 
        else/* mainContext_p->PDUUsage==WMMP_SECURITY_REBOOT*/
        {
            WMMP_LIB_DBG_TEMP("DecodeLogoutAck:mainContext_p->PDUUsage=WMMP_SECURITY_REBOOT\n");	

            /* 重登陆需要新的密钥上下行密码 */
            if(mainContext_p->uplinkPassword_b[0]!=0)
            {
                memcpy(uplinkPassword_b,SecurityContext_p->uplinkPassword,sizeof(SecurityContext_p->uplinkPassword));	
                memcpy(SecurityContext_p->uplinkPassword,mainContext_p->uplinkPassword_b,sizeof(SecurityContext_p->uplinkPassword));	
                memcpy(mainContext_p->uplinkPassword_b,uplinkPassword_b,sizeof(SecurityContext_p->uplinkPassword));	
            }

            if(mainContext_p->downlinkPassword_b[0]!=0)
            {
                memcpy(downlinkPassword_b,SecurityContext_p->downlinkPassword,sizeof(SecurityContext_p->downlinkPassword));	
                memcpy(SecurityContext_p->downlinkPassword,mainContext_p->downlinkPassword_b,sizeof(SecurityContext_p->downlinkPassword));
                memcpy(mainContext_p->downlinkPassword_b,downlinkPassword_b,sizeof(SecurityContext_p->downlinkPassword));	
            }

            if(mainContext_p->publickey_b[0]!=0)
            {
                memcpy(publickey_b,SecurityContext_p->publickey,sizeof(SecurityContext_p->publickey));	
                memcpy(SecurityContext_p->publickey,mainContext_p->publickey_b,sizeof(SecurityContext_p->publickey));	
                memcpy(mainContext_p->publickey_b,publickey_b,sizeof(SecurityContext_p->publickey));	
            }
        }
    }
    else
    {		
        wmmpProcessLogoutCnf();	

        /* add by cbc@20100422:  begin */
        //mainContext_p->Wmmp_status.net_status = LINK_FAILED; /*wmmpSendNextCommand() 时判断是否扔掉登录包*/
        /* add by cbc@20100422:  end */
    }
    /* modified by cbc@20100420  end*/

    //wmmpSendLogoutCnf(VG_CI_TASK_ID, 0);
    //status_ind should always send to ci
    if(mainContext_p->WPIEventFucPtr!=PNULL)
    {
        WPIEventData.WPIStatusIndEvent.type = 1;
        WPIEventData.WPIStatusIndEvent.value= PIDLE;	
        WPIEventData.WPIStatusIndEvent.reason = mainContext_p->PDUUsage;
        (mainContext_p->WPIEventFucPtr)(WPI_STATUS_IND_EVENT,&WPIEventData);	 /* CBC@正常登出 */		
    }		
#endif		
    return  WMMP_DECODE_OK;				
}

WmmpDecodeRecvResult_e DecodeSecurityCfg(char *des,PDUContext_t *pduContext,WmmpTransType_e type)
{
    char *p=des;
    char *q;
    u16 tag;
    u8 tagval=0;
    u16 length;
    WmmpContext_t *mainContext = ptrToWmmpContext();

    u16 *tags=PNULL;
    u8 tagnum=0;
    bool tagError=false;
    bool tagFlag = false;/* cbc@20100518:安全参数修改标识*/
    //if(mainContext->CanSecuritySet==true)	
    //{
    u8 operation=  *p; 
    p++;

    WMMP_LIB_DBG_TEMP("DecodeSecurityCfg");
    //check error tag
    tags = (u16*)WPI_Malloc(pduContext->BodyLength);
    //KiAllocZeroMemory(pduContext->BodyLength, (void**)&tags);
    while (p-pduContext->BodyConst < pduContext->BodyLength)
    {
        tag = Readu16(p);
        WMMP_LIB_DBG_TEMP("tags =%x,%x,%x\n",tag,p-pduContext->BodyConst,pduContext->BodyLength);
        p+=2;
        length=Readu16(p);
        WMMP_LIB_DBG_TEMP("tags len %X\n",length);
        p+=2;

        if (tag == 0xe00d)/* cbc@20100518:清除SIM 卡安全参数 ,这个和修改上下行密码安全参数不能同时进行,因为涉及到LOGOUT  REASON 的值*/
        {
            tagval= *p;
            WMMP_LIB_DBG_TEMP("tagsval=%X\n",tagval);
        }

        /* add by cbc@20100518:安全密钥上下星密码修改需要重登录,REASON=9 ,Begin*/
        if ((tag == 0xE038)||(tag == 0xE036)||(tag == 0xE02A)||(tag == 0xE028)||(tag == 0xE027)||(tag == 0xE025))
        {
            /*
               tags[0]=0xE038;
               tags[1]=0xE036;
               tags[2]=0xE02A;
               tags[3]=0xE028;
               tags[4]=0xE027;
               tags[5]=0xE025;
               */

            /* cbc@20100518:清除SIM 卡安全参数 ,这个和修改上下行密码安全参数不能同时进行,因为涉及到LOGOUT  REASON 的值*/
            WMMP_LIB_DBG_TEMP("tagFlag=%X,protocol_status=0x%x\n",tagFlag,mainContext->Wmmp_status.protocol_status);

            if (mainContext->Wmmp_status.protocol_status != REGISTERSUCCUSSFUL)  /* 注册时下发密码不需要重启*/
            {
                tagFlag = true;
            }
        }
        /* add by cbc@20100518:安全密钥上下星密码修改需要重登录,REASON=9 ,end*/


        if(CheckTLV(tag,length, p)==false)
        {
            tags[tagnum]=tag;
            WMMP_LIB_DBG_TEMP("tags error %X\n",tag);
            tagnum++;	
            tagError = true;
        }
        p+=length;

    } 

    if(tagError==true)
    {
        WMMP_LIB_DBG_TEMP("SecurityCfg decode tagError==true");		
        wmmpProcessSecurtiyCfgReq(operation,2,tags,tagnum);
        return WMMP_DECODE_OK; 	
    }
    else
    {
        WMMP_LIB_DBG_TEMP("SecurityCfg decode tagError!=true");	
        WPI_Free(tags);
        //KiFreeMemory((void **) &tags);
    }

    p=des+1;
    pduContext->BodyTLV = p;

    switch(operation)
    {
        case 0:  /*cbc@20100521    :平台设置终端安全参数  */
            {
                // if((ReadyUpdataSecurtiySet==false)&&(type==WMMP_TRANS_SMS))
                if(mainContext->Wmmp_status.protocol_status==REGISTERSUCCUSSFUL)
                {	
                    WMMP_LIB_DBG_TEMP("SecurityCfg decode mainContext->Wmmp_status.protocol_status==REGISTERSUCCUSSFUL");	

                    WPI_send_urc(WMMP_AT_EVENT_STR"%d,%s %s", WMMP_AET_LIB_STATE, "0202", "\"Registering: waiting for security configuration (AUX)\"");
                    //init password
                    while(p-des<pduContext->BodyLength)
                    {
                        tag=Readu16(p);
                        p+=2;

                        length=Readu16(p);
                        p+=2;

                        WriteTLV(tag,length,p);

                        p+=length;
                        WMMP_LIB_DBG_TEMP("DecodeSecurityCfg write TLV %x %d",tag,length);


                        stopRegisterForKeyTimer(); /*cbc220100525:停止注册超时定时器 */
                        mainContext->maxRegSecuSmsTimeOutCount = 0;
                    }
                }
                else
                {					
                    mainContext->TLVtempLen = pduContext->BodyLength-1;
                    if(mainContext->TLVtemp!=PNULL)
                    {
                        WPI_Free(mainContext->TLVtemp);
                        WMMP_LIB_DBG_TEMP("WMMP stack---Error!, mainContext->TLVtemp is not null");
                    }					
                    mainContext->TLVtemp = (char*)WPI_Malloc(mainContext->TLVtempLen);
                    memcpy(mainContext->TLVtemp,p,mainContext->TLVtempLen);

                    /* modified by cbc@20100409: 变更SIM  卡操作由服务器下发:begin*/
                    if (tagval == 0x02) /*  0XE00D = 2, 平台清除SIM卡安全参数,清除后登出*/
                    {    
                        mainContext->PDUUsage =WMMP_SIM_SECURITY_LOGOUT;  /*  0XE00D = 2, 平台清除SIM卡安全参数,清除后登出*/
                    }
                    else
                    {
                        mainContext->PDUUsage = WMMP_PDU_SECURITY_LOGOUT;/*20100521@cbc;平台设置终端安全参数,设置后终端登出并启用新安全参数登录 */
                    }
                    /* modified by cbc@20100409: 变更SIM  卡操作由服务器下发:end*/

                    /* cbc@20100518:清除SIM 卡安全参数 ,这个和修改上下行密码安全参数不能同时进行,因为涉及到LOGOUT  REASON 的值*/
                    if (tagFlag == true)
                    {
                        mainContext->PDUUsage =WMMP_SECURITY_REBOOT;/*cbc@20100521:基础密钥上下行密码的更新需要重新登录并启用新的安全参数 */

                        /* add by cbc@20100518  安全参数更新应该保存begin */
                        while(p-des<pduContext->BodyLength)
                        {
                            //((tag == 0xE038)||(tag == 0xE036)||(tag == 0xE02A)||(tag == 0xE028)||(tag == 0xE027)||(tag == 0xE025))
                            tag=Readu16(p);
                            p+=2;

                            length=Readu16(p);
                            p+=2;

                            //WriteTLV(tag,length,p);

                            /* 这些安全参数只有在重登陆路时使用 */
                            if (tag == 0xe025)
                            {			/*更新安全参数密码密钥,保存备份等重登录后启用 */		            		
                                memset(mainContext->uplinkPassword_b,0,sizeof(mainContext->uplinkPassword_b));		
                                memcpy(mainContext->uplinkPassword_b,p,length);
                                WMMP_LIB_DBG_TEMP("write TLV %x %d,uplinkPassword_b=%s\n",tag,length,mainContext->uplinkPassword_b);
                            }
                            if (tag == 0xe028)
                            {	/*更新安全参数密码密钥,保存备份等重登录后启用 */		 				            		
                                memset(mainContext->downlinkPassword_b,0,sizeof(mainContext->downlinkPassword_b));	
                                memcpy(mainContext->downlinkPassword_b,p,length);
                                WMMP_LIB_DBG_TEMP("write TLV %x %d,downlinkPassword_b=%s\n",tag,length,mainContext->downlinkPassword_b);
                            }
                            if (tag == 0xe036)
                            {			/*更新安全参数密码密钥,保存备份等重登录后启用 */		 		            		
                                memset(mainContext->publickey_b,0,sizeof(mainContext->publickey_b));	
                                memcpy(mainContext->publickey_b,p,length);
                                //								WMMP_LIB_DBG_TEMP("write TLV %x %d,publickey_b=%s\n",tag,length,mainContext->publickey_b);
                            }

                            p+=length;
                            WMMP_LIB_DBG_TEMP("write TLV %x %d\n",tag,length);

                        }
                        /* add by cbc@20100518 end */
                    }

                    //ReadyLoginOut = true; 						
                    WMMP_LIB_DBG_TEMP("SecurityCfg decode mainContext->Wmmp_status.protocol_status==REGISTERSUCCUSSFUL");	

                }

                WMMP_LIB_DBG_TEMP("SecurityCfg decode over");			
                wmmpProcessSecurtiyCfgReq(operation,0,PNULL,0);
            }
            break;

        case 1:
        case 2:
            {

                tags =(u16*)WPI_Malloc( pduContext->BodyLength);
                //KiAllocZeroMemory(pduContext->BodyLength, (void**)&tags);
                while (p-pduContext->BodyConst < pduContext->BodyLength)
                {
                    tag = Readu16(p);
                    p+=2;
                    length=Readu16(p);
                    p+=2;	        

                    p+=length;			      

                    tags[tagnum]=tag;
                    WMMP_LIB_DBG_TEMP("security read  TLV=%x,tagnum=%d\n",tag,tagnum);
                    tagnum++;
                } 

                WMMP_LIB_DBG_TEMP("security read  TLV=%x,tagnum=%d\n",tag,tagnum);
                wmmpProcessSecurtiyCfgReq(operation,0,tags,tagnum);

                break;
            }


        case 3:
            {
                WMMP_LIB_DBG_TEMP("security read  TLV 	%x\n",tag);
                wmmpProcessSecurtiyCfgReq(operation,0,PNULL,0);
                //ReadyUpdataSecurtiySet = true ;
            }
            break;

        default:
            WMMP_LIB_DBG_TEMP("security read operation=%d	\n",operation);
            break;
    }
    //suppose it's always correct
    // wmmpSendSecurityCfgReq(VG_CI_TASK_ID,operation,0);
    //only send when type=SMS

    //}   
    return WMMP_DECODE_OK; 
}

WmmpDecodeRecvResult_e DecodeCfgTrapAck(char *des,PDUContext_t *pduContext)
{
    char *p=des;
    u16 tag=0;
    u16 length;
    u8 ret=  *p; 
    WPIEventData_t WPIEventData;
    WmmpContext_t    *mainContext_p = ptrToWmmpContext();

    p++;
    WMMP_LIB_DBG_TEMP("DecodeCfgTrapAck ret =%d",ret);

    if (ret!=0)
    {
        //now server only return one invalid tag,so this should be edit
        if(p-des<pduContext->BodyLength)
        {
            tag=Readu16(p);
            p+=2;

            //length=Readu16(p);
            // p+=2;

            WMMP_LIB_DBG_TEMP("DecodeCfgTrapAck: TLV tag 0x%04x is invalid",tag);
        }
    }
    //wmmpSendTrapCnf(VG_CI_TASK_ID, ret,tag);
    //wmmpSendTrapCnf(SIMCOM_WMMP_TASK_ID,ret,tag);
    WMMP_LIB_DBG_TEMP("DecodeCfgTrapAck: tag 0x%04x PDUUsage %d",tag,mainContext_p->PDUUsage);
    wmmpProcessTrapCnf(); 
    if((mainContext_p->WPIEventFucPtr!=PNULL)&&(mainContext_p->PDUUsage==WMMP_PDU_ATCOMMAND))
    {
        WPIEventData.WPITrapAckEvent.ret = ret;
        WPIEventData.WPITrapAckEvent.tag= tag;
        (mainContext_p->WPIEventFucPtr)(WPI_TRAP_ACK_EVENT,&WPIEventData);
    }

    return WMMP_DECODE_OK; 
}

WmmpDecodeRecvResult_e DecodeCfgReqAck(PDUContext_t *pduContext)
{
    WPIEventData_t WPIEventData;
    WmmpContext_t    *mainContext_p = ptrToWmmpContext();
    char *p=pduContext->BodyConst;
    u16 tag_Id,tag_length;
    u16 tag;
    u8 ret=  *p; 
    p++;

    WMMP_LIB_DBG_TEMP("DecodeCfgReqAck ret=%d",ret);

    if (ret==0)
    {	

        while (p-pduContext->BodyConst < pduContext->BodyLength)
        {
            tag_Id = Readu16(p);

            p+=2;
            tag_length = Readu16(p);
            p+=2;
            WriteTLV(tag_Id, tag_length, p);
            WMMP_LIB_DBG_TEMP("DecodeCfgReqAck %d,%d\n",tag_Id,tag_length);
            p+=tag_length; 

        }
    }
    else
    {
        tag_Id = Readu16(p);
        tag=tag_Id;
        WMMP_LIB_DBG_TEMP("DecodeCfgReqAck ERROR %d\n",tag_Id);
    }
    //suppose it's always correct
    wmmpProcessReqCnf(ret,tag);
    //wmmpSendReqCnf(VG_CI_TASK_ID,ret,tag);
    //wmmpSendReqCnf(SIMCOM_WMMP_TASK_ID,ret,tag);

    if((mainContext_p->WPIEventFucPtr!=PNULL)&&(mainContext_p->PDUUsage==WMMP_PDU_ATCOMMAND))
    {
        WPIEventData.WPIReqAckEvent.ret = ret;
        WPIEventData.WPIReqAckEvent.tag= tag;
        (mainContext_p->WPIEventFucPtr)(WPI_REQ_ACK_EVENT,&WPIEventData);
    }	
    return WMMP_DECODE_OK; 
}
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_ADC_GPIO__
extern s32 simcom_wmmp_get_adc(void);
extern bool l4cuem_get_wmmp_adc_flag(void);
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/
WmmpDecodeRecvResult_e DecodeCfgGet(PDUContext_t *pduContext)
{
    char *p=pduContext->BodyConst;
    u16 i;
    u16 *tag;
    u16 tag_Id;
    u8 tagnum=0;
    bool tagError=false;

    tag = (u16*)WPI_Malloc(pduContext->BodyLength);
    //KiAllocZeroMemory(pduContext->BodyLength, (void**) &tag);

    WMMP_LIB_DBG_TEMP("DecodeCfgGet");


    //check TLV
    while (p-pduContext->BodyConst < pduContext->BodyLength)
    {
        tag_Id = Readu16(p);
        p+=4;
        /*cbc220100423:这个是什么条件呢? */
        if((CheckTLVTag(tag_Id)==false)||((tag_Id>=SIMPIN1)&&(tag_Id<=CONVPACKEDENCRYPTBODY)))
        {
            WMMP_LIB_DBG_TEMP("config get command  Check TLV error=0x%x\n",tag_Id);
            tag[tagnum]=tag_Id;
            tagnum++;	
            tagError = true;
            //tag[0]=tag_Id;
        }
        /*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_ADC_GPIO__
        if(tag_Id==0x81f5)	
        {
            simcom_wmmp_get_adc();
            while(l4cuem_get_wmmp_adc_flag()==true);
        }
#endif        
        /*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/
        //memcpy(tag+2*tagnum,tag_Id,2);

    }
    if(tagError==true)
    {
        wmmpProcessConfigGetReq(tag,tagnum,1);
        return WMMP_DECODE_OK; 		
    }

    p=pduContext->BodyConst;

    /*
       for(i=0;i<pduContext->BodyLength/4;i++)
       {
       memcpy(tag+i*2,p+i*4,2);
       }*/

    while (p-pduContext->BodyConst < pduContext->BodyLength)
    {
        tag_Id = Readu16(p);
        p+=2;

        p+=2;

        //memcpy(tag+2*tagnum,tag_Id,2);
        tag[tagnum]=tag_Id;
        tagnum++;

    }    

    wmmpProcessConfigGetReq(tag,tagnum,0);
    return WMMP_DECODE_OK; 
}


WmmpDecodeRecvResult_e DecodeCfgSet(PDUContext_t *pduContext)
{
    char *p=pduContext->BodyConst;
    u16 tag_Id,tag_length;
    u16 *tag;
    u8 tagnum=0;
    bool tagError=false;
    char *q;
    WmmpContext_t *mainContext_p = ptrToWmmpContext();
    WPIEventData_t WPIEventData;
    char *TLVtemp = PNULL;  
    u16 TLVtempLen;

    WMMP_LIB_DBG_TEMP("DecodeCfgSet");

    //如果构建一个比较好的堆栈系统，创建一个正确tlv 和错误tlv的堆栈，则只需要一次遍历即可
    //cacl how many tlv in PDU
    while (p-pduContext->BodyConst < pduContext->BodyLength)
    {
        tag_Id = Readu16(p);
        p+=2;
        tag_length = Readu16(p);
        p+=2;       
        p+=tag_length; 
        tagnum++;	

    }
    WMMP_LIB_DBG_TEMP("there are %d tlv in SET PDU\n",tagnum);

    //KiAllocZeroMemory(tagnum*2, (void**) &tag);
    tag = (u16*)WPI_Malloc( tagnum*2);
    if(tag==PNULL)
    {
        WMMP_LIB_DBG_TEMP("KiAllocZeroMemory failed\n");
        return WMMP_DECODE_ERROR;
    }

    //check TLV 
    tagnum = 0;
    p=pduContext->BodyConst;	

    while (p-pduContext->BodyConst < pduContext->BodyLength)
    {
        tag_Id = Readu16(p);
        p+=2;
        tag_length = Readu16(p);
        p+=2;

        WMMP_LIB_DBG_TEMP("config set cmd  Check TLV id %x len %d\n",tag_Id,tag_length);

        if(CheckTLV(tag_Id, tag_length, p)==false)
        {
            WMMP_LIB_DBG_TEMP("config set cmd  Check TLV error=%x,%d\n",tag_Id,tag_length);
            tag[tagnum]=tag_Id;
            tagnum++;
            tagError = true;
        }

        /*add by cbc@20100512: CONFIG SET 无法完成设置安全信息功能BEGIN*/
        if ((0xe025 ==tag_Id)||(0xe027 ==tag_Id)||(0xe028 ==tag_Id)||(0xe02a==tag_Id)||(0xe036 ==tag_Id)||(0xe038 ==tag_Id))
        {
            WMMP_LIB_DBG_TEMP("config set command  Check TLV 1 error%x,%d\n",tag_Id,tag_length);
            tag[tagnum]=tag_Id;
            tagnum++;			
            tagError = true;			
        }

        if ((tag_Id>= 0xe001)&&(tag_Id<= 0xe01f))/* config SET 无法完成sim 卡安全参数 */
        {
            WMMP_LIB_DBG_TEMP("config set command  Check TLV 2  error%x,%d\n",tag_Id,tag_length);
            tag[tagnum]=tag_Id;
            tagnum++;			
            tagError = true;
        }
        /*add by cbc@20100512: CONFIG SET 无法完成设置安全信息功能END*/

        p+=tag_length; 
        //memcpy(tag+2*tagnum,tag_Id,2);

    }

    if(tagError==false)
    {
        WMMP_LIB_DBG_TEMP("config set command  tagError==false\n");	
        tagnum = 0;
        p=pduContext->BodyConst;		
        TLVtemp = p;
        TLVtempLen = pduContext->BodyLength;	

        while (p-pduContext->BodyConst < pduContext->BodyLength)
        {			
            tag_Id = Readu16(p);
            p+=2;
            tag_length = Readu16(p);
            p+=2;
            //WriteTLV(tag_Id, tag_length, p);
            //WPI_LOG("config set command %d,%d",tag_Id,tag_length);
            p+=tag_length; 
            //memcpy(tag+2*tagnum,tag_Id,2);
            tag[tagnum]=tag_Id;
            tagnum++;

            WMMP_LIB_DBG_TEMP("tag_Id=0x%04x,tag_length=%d,tagnum=%d\n",tag_Id,tag_length,tagnum);		
        }

    }


    wmmpProcessConfigSetReq(tagError,tag,tagnum,TLVtemp,TLVtempLen);
    if(mainContext_p->WPIEventFucPtr!=PNULL)
    {
        WPIEventData.WPISetReqEvent.ret= tagError;
        WPIEventData.WPISetReqEvent.tags = tag;
        WPIEventData.WPISetReqEvent.tagnum = tagnum;
        (mainContext_p->WPIEventFucPtr)(WPI_SET_REQ_EVENT,&WPIEventData);

    }	
    else
    {
        WPI_Free(tag);
    }

    return WMMP_DECODE_OK; 
}

WmmpDecodeRecvResult_e DecodeRemoteCtrl(PDUContext_t *pduContext)
{
    char *p=pduContext->BodyConst;
    u16 tag_Id,tag_length;
    u8 paraId;
    u16 *tag;
    u8 tagnum=0;
    bool tagError=false;
    WPIEventData_t WPIEventData;
    WmmpContext_t *mainContext_p = ptrToWmmpContext();
    WmmpCustomContext_t  *CustomContext_p = &(mainContext_p->CustomContext);

    WMMP_LIB_DBG_TEMP("DeRemoteCtrl remoteCtrlTraceEnabel %d",CustomContext_p->remoteCtrlTraceEnabel);
    tag = (u16*)WPI_Malloc( 100);

    //check TLV
    while (p-pduContext->BodyConst < pduContext->BodyLength)
    {
        tag_Id = Readu16(p);
        p+=2;
        tag_length = Readu16(p);
        p+=2;

        WMMP_LIB_DBG_TEMP("1 DeRemoteCtrl,ag_Id=0x%x len %d",tag_Id,tag_length);

        //if(tag_Id!=0x4001)  /*RMCTRLTRACEENABLE = 0x81f0*/
        if((tag_Id!=0x4001)&&(tag_Id!=0)&&(tag_Id!=RMCTRLTRACEENABLE))/* cbc@20100421:有可能加密后解密出来的错误后面带0 */
        {
            WMMP_LIB_DBG_TEMP("2 DeRemoteCtrl,ag_Id=0x%x len %d",tag_Id,tag_length);
            tag[tagnum]=tag_Id;
            tagnum++;
            tagError = true;
        }
        p+=tag_length; 

    }


    if(tagError==false)
    {
        //now remote ctrl is always 0x4001
        p=pduContext->BodyConst;
        tag_Id = Readu16(p);
        if(tag_Id==0x4001)
        {
            p+=2;
            tag_length = Readu16(p);
            p+=2;
            paraId = *p;
            /*bob add 20101207*/
            tag[tagnum]=tag_Id;
        }
        else if(RMCTRLTRACEENABLE == tag_Id)
        {
            p+=2;
            tag_length = Readu16(p);
            p+=2;
            paraId = *p;
            /*bob add 20101207*/
            tag[tagnum]=tag_Id;
        }

    }

    //suppose it's always correct
    WMMP_LIB_DBG_TEMP("RemoteCtrl com ID,tagError=%d,paraId=%d,tagnum=%d",tagError,paraId,tagnum);

    if(RMCTRLTRACEENABLE == tag_Id)
    {
        wmmpProcessRemotCtrlTraceReq(tagError,paraId,tag,tagnum);

    }
    else
    {
        wmmpProcessRemotCtrlReq(tagError,paraId,tag,tagnum);
        if(mainContext_p->WPIEventFucPtr!=PNULL)
        {
            WPIEventData.WPIRemoteReqEvent.ret= tagError;
            WPIEventData.WPIRemoteReqEvent.paramId= paraId;
            WPIEventData.WPIRemoteReqEvent.errortag= tag;
            WPIEventData.WPIRemoteReqEvent.errortagnum= tagnum;
            (mainContext_p->WPIEventFucPtr)(WPI_REMOTE_REQ_EVENT,&WPIEventData);

        }			
    }


    return WMMP_DECODE_OK; 
}

u8 hexToInt(char ch)
{
    ch = toupper(ch);

    if ( isdigit(ch) )
    {
        ch -= '0';
    }
    else if ( isxdigit(ch) )
    {
        ch -= 'A';
        ch += 0x0A;
    }
    else
    {
        WMMP_LIB_DBG_TEMP("Not a hex digit\n");
    }

    return ch;
}

/*****************************************************************************
 * FUNCTION
 *  hex_to_string
 * DESCRIPTION
 *  
 * PARAMETERS
 *  dest        [?]         
 *  src         [?]         
 *  len         [IN]        
 * RETURNS
 *  
 *****************************************************************************/
bool HextoString(char *dest, u8 *src, u8 len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    u16 i, j;

    WMMP_LIB_DBG_TEMP("HextoString len %d",len);
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (len % 2 != 0)
    {
        return false;
    }

    memset(dest, 0, len / 2 + 1);

    for (i = 0, j = 0; i < len; i++)
    {
        j = (i - i % 2) / 2;
        WMMP_LIB_DBG_TEMP("HextoString src[%d] 0x%x",i,src[i]);
        if (src[i] >= '0' && src[i] <= '9')
        {
            dest[j] += (src[i] - '0') << 4 * ((i + 1) % 2);
            WMMP_LIB_DBG_TEMP("HextoString digital dest[%d] 0x%x",j,dest[j]);
        }
        else if (src[i] >= 'A' && src[i] <= 'F')
        {
            dest[j] += (src[i] - 'A' + 0x0A) << 4 * ((i + 1) % 2);
            WMMP_LIB_DBG_TEMP("HextoString big dest[%d] 0x%x",j,dest[j]);
        }
        else if (src[i] >= 'a' && src[i] <= 'f')
        {
            dest[j] += (src[i] - 'a' + 0x0A) << 4 * ((i + 1) % 2);
            WMMP_LIB_DBG_TEMP("HextoString small dest[%d] 0x%x",j,dest[j]);
        }
        else
        {
            WMMP_LIB_DBG_TEMP("HextoString big return false");
            return false;
        }
    }
    WMMP_LIB_DBG_TEMP("HextoString big return true dest ");
    return true;
}


/*****************************************************************************
 * FUNCTION
 *  is_valid_hexstring
 * DESCRIPTION
 *  
 * PARAMETERS
 *  src     [?]         
 *  len     [IN]        
 * RETURNS
 *  
 *****************************************************************************/
bool is_valid_hexstring(u8*src, u8 len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < len; i++)
    {
        if (src[i] >= '0' && src[i] <= '9')
        {
            continue;
        }
        else if (src[i] >= 'A' && src[i] <= 'F')
        {
            continue;
        }
        else if (src[i] >= 'a' && src[i] <= 'f')
        {
            continue;
        }
        else
        {
            return false;
        }
    }
    return true;
}




bool ParseDownloadURL(char *url)
{
    char *p=PNULL;
    char *q=PNULL;
    u8 i;
    u32 trans_id=0;
    WmmpContext_t *mainContext_p = ptrToWmmpContext();
    char temp[20];
    u16  tmpMinPara = 0;	

    WMMP_LIB_DBG_TEMP("ParseDownloadURL  file size is %d adFileFileReadPoint %d adFileStatus %d",mainContext_p->UpdateContext.UpdateFileLength,mainContext_p->UpdateContext.adFileFileReadPoint,mainContext_p->UpdateContext.adFileStatus);

    //find url	
    p=(char *)strstr((char *)url,(const char*)"://");
    if( p==PNULL)
    {
        return false;
    }
    p+=3;
    q=(char *)strstr((char *)p,(const char*)":");
    if(q==PNULL)
    {
        return false;
    }
    memset(mainContext_p->UpdateContext.UpdateSrvURLAddr,0,sizeof(mainContext_p->UpdateContext.UpdateSrvURLAddr));
    memcpy(mainContext_p->UpdateContext.UpdateSrvURLAddr,p,q-p);
    WMMP_LIB_DBG_TEMP("url is %s",mainContext_p->UpdateContext.UpdateSrvURLAddr);

    p=q;
    p++;
    //find port
    q=(char *)strstr((char *)p,(const char*)"/");
    if(q==PNULL)
    {
        return false;
    }
    memset(temp,0,sizeof(temp));
    memcpy(temp,p,q-p);
    mainContext_p->UpdateContext.UpdateSrvPort = strtoul((char*)&temp,NULL,0);//atoi(temp);
    WMMP_LIB_DBG_TEMP("port is %d",mainContext_p->UpdateContext.UpdateSrvPort);
    //find transid;
    p=q;
    q=(char *)strstr((char *)p,(const char*)"TRANS_ID=");
    if(q==PNULL)
    {
        return false;
    }
    p=q+9;
    for(i=0;i<8;i++)
    {
        trans_id=trans_id*16+hexToInt(*p);
        p++;

    }
    mainContext_p->UpdateContext.UpdateAffairTransId= trans_id;/*0x1009*/
    WMMP_LIB_DBG_TEMP("TransId is %4X",trans_id);
    //find file size
    q=(char *)strstr((char *)p,(const char*)"FILE_SIZE=");
    if(q==PNULL)
    {
        return false;
    }
    p=q+10;

    q=(char *)strstr((char *)p,(const char*)"&");
    if(q==PNULL)
    {
        return false;
    }
    memset(temp,0,sizeof(temp));
    memcpy(temp,p,q-p);	
    mainContext_p->UpdateContext.UpdateFileLength= strtoul((char*)&temp,NULL,0);//atoi(temp);/*0x1006*/
    mainContext_p->UpdateContext.adFileFileReadPoint = 0;
    mainContext_p->UpdateContext.adFileStatus = 0;
    WMMP_LIB_DBG_TEMP("ParseDownloadURL e file size is %d adFileFileReadPoint %d adFileStatus %d",mainContext_p->UpdateContext.UpdateFileLength,mainContext_p->UpdateContext.adFileFileReadPoint,mainContext_p->UpdateContext.adFileStatus);
    p=q;

    //find CRC16
    q=(char *)strstr((char *)p,(const char*)"CRC16=");
    if(q!=PNULL)
    {
        u16 crc16=0;
        p=q+6;

        q=(char *)strstr((char *)p,(const char*)"&");
        if(q==PNULL)
        {
            return false;
        }
        for(i=0;i<q-p;i++)
        {
            crc16=crc16*16+hexToInt(*(p+i));
        }


        mainContext_p->UpdateContext.CRC16Check = crc16;
        WMMP_LIB_DBG_TEMP("WMMP stack---crc16 is %4X",mainContext_p->UpdateContext.CRC16Check);
    }

    //find CRC32
    q=(char *)strstr((char *)p,(const char*)"CRC32=");
    if(q!=PNULL)
    {
        u32 crc32=0;
        p=q+6;
        q=(char *)strstr((char *)p,(const char*)"&");
        if(q==PNULL)
        {
            return false;
        }
        for(i=0;i<q-p;i++)
        {
            crc32=crc32*16+hexToInt(*(p+i));

        }

        mainContext_p->UpdateContext.CRC32Check= crc32;
        WMMP_LIB_DBG_TEMP("WMMP stack---crc32 is %8X",mainContext_p->UpdateContext.CRC32Check);
    }	
    /*bob add 20110210 -s*/
    //find VER_DSC
    q=(char *)strstr((char *)p,(const char*)"VER_DSC=");
    if(q!=PNULL)
    {
        u8 	v_MajorVer = 0;
        u8 	v_MinorVer = 0;
        u8 	v_tmpLen = 0;
        p=q+8;

        v_tmpLen = strlen(p);
        WMMP_LIB_DBG_TEMP("ParseDownloadURL 1 v_tmpLen %d",v_tmpLen);
        if(v_tmpLen)
        {
            memset(mainContext_p->UpdateContext.UpdateFileRev,0,sizeof(mainContext_p->UpdateContext.UpdateFileRev));
            memcpy(mainContext_p->UpdateContext.UpdateFileRev,p,min(v_tmpLen,19)); /*0x1005*/			
            WMMP_LIB_DBG_TEMP("ParseDownloadURL  UpdateFileRev is %s",mainContext_p->UpdateContext.UpdateFileRev);
        }
        else
        {
            WMMP_LIB_DBG_TEMP("ParseDownloadURL not major minor af VER_DSC");
            return false;
        }	

        q=(char *)strstr((char *)p,(const char*)"WMMP");
        if(q != PNULL)
        {
            p = q+4;
            v_tmpLen = strlen(p);
            WMMP_LIB_DBG_TEMP("ParseDownloadURL 2 v_tmpLen %d",v_tmpLen);
            if(v_tmpLen)
            {
                q=(char *)strstr((char *)p,(const char*)".");
                if(q==PNULL)
                {
                    memset(temp,0,sizeof(temp));
                    memcpy(temp,p,min(v_tmpLen,19));
                    WMMP_LIB_DBG_TEMP("ParseDownloadURL 1 temp %s",temp);
                    mainContext_p->UpdateContext.UpdateFileMajorRev = atoi(temp);					
                    WMMP_LIB_DBG_TEMP("UpdateFileMajorRev 1 is %d",mainContext_p->UpdateContext.UpdateFileMajorRev);
                    mainContext_p->UpdateContext.UpdateFileMinorRev = 0;					
                }
                else
                {
                    memset(temp,0,sizeof(temp));
                    memcpy(temp,p,min((q-p),19));
                    WMMP_LIB_DBG_TEMP("ParseDownloadURL 2 temp %s",temp);
                    mainContext_p->UpdateContext.UpdateFileMajorRev= atoi(temp);
                    WMMP_LIB_DBG_TEMP("UpdateFileMajorRev 2 is %d",mainContext_p->UpdateContext.UpdateFileMajorRev);
                    memset(temp,0,sizeof(temp));
                    //tmpMinPara = v_tmpLen-q+p-1;
                    tmpMinPara = v_tmpLen-1-(u8)(q-p);
                    WMMP_LIB_DBG_TEMP("ParseDownloadURL 1 tmpMinPara %d",tmpMinPara);
                    memcpy(temp,q+1,min(tmpMinPara,19));
                    WMMP_LIB_DBG_TEMP("ParseDownloadURL 2 temp %s",temp);
                    mainContext_p->UpdateContext.UpdateFileMinorRev= atoi(temp);
                    WMMP_LIB_DBG_TEMP("UpdateFileMinorRev 2 is %d",mainContext_p->UpdateContext.UpdateFileMinorRev);
                }							
            }
            else
            {
                WMMP_LIB_DBG_TEMP("ParseDownloadURL HAVE WMMP BUT not have MAJOR AND MINOR");
                return false;
            }

        }
        else
        {
            p = mainContext_p->UpdateContext.UpdateFileRev;
            WMMP_LIB_DBG_TEMP("ParseDownloadURL p %s",p);
            v_tmpLen = strlen(p);
            WMMP_LIB_DBG_TEMP("ParseDownloadURL 3 v_tmpLen %d",v_tmpLen);
            if(v_tmpLen)
            {
                q=(char *)strstr((char *)p,(const char*)".");
                if(q==PNULL)
                {
                    memset(temp,0,sizeof(temp));
                    memcpy(temp,p,min(v_tmpLen,19));
                    WMMP_LIB_DBG_TEMP("ParseDownloadURL 3 temp %s",temp);
                    mainContext_p->UpdateContext.UpdateFileMajorRev= atoi(temp);
                    WMMP_LIB_DBG_TEMP("UpdateFileMajorRev 3 is %d",mainContext_p->UpdateContext.UpdateFileMajorRev);
                    mainContext_p->UpdateContext.UpdateFileMinorRev = 0;
                }
                else
                {
                    memset(temp,0,sizeof(temp));
                    memcpy(temp,p,min((q-p),19));
                    WMMP_LIB_DBG_TEMP("ParseDownloadURL 4 temp %s",temp);
                    mainContext_p->UpdateContext.UpdateFileMajorRev= atoi(temp);
                    WMMP_LIB_DBG_TEMP("UpdateFileMajorRev 4 is %d",mainContext_p->UpdateContext.UpdateFileMajorRev);
                    memset(temp,0,sizeof(temp));
                    //tmpMinPara = v_tmpLen-q+p-1;
                    tmpMinPara = v_tmpLen-1-(u8)(q-p);
                    WMMP_LIB_DBG_TEMP("ParseDownloadURL 2 tmpMinPara %d",tmpMinPara);
                    memcpy(temp,q+1,min(tmpMinPara,19));
                    WMMP_LIB_DBG_TEMP("ParseDownloadURL 5 temp %s",temp);
                    mainContext_p->UpdateContext.UpdateFileMinorRev= atoi(temp);
                    WMMP_LIB_DBG_TEMP("UpdateFileMinorRev 5 is %d",mainContext_p->UpdateContext.UpdateFileMinorRev);
                }							
            }
            else
            {
                WMMP_LIB_DBG_TEMP("ParseDownloadURL NOT have wmmp and  not have major minor");
                return false;
            }

        }

    }
    else
    {
        WMMP_LIB_DBG_TEMP("ParseDownloadURL not have VER_DSC");
        return false;
    }
    /*bob add 20110210 -e*/

    return true;
}

WmmpDecodeRecvResult_e DecodeDownload(PDUContext_t *pduContext)
{
    WPIEventData_t WPIEventData;
    WmmpContext_t *mainContext_p = ptrToWmmpContext();
    char *p=pduContext->BodyConst;
    char *URL;
    u16 length;
    //wmmp_ascii * pl_StrPtr = NULL;

    WMMP_LIB_DBG_TEMP("DecodeDownload");

    if (Readu16(p)==0x1001)
    {
        p+=2;
        length = Readu16(p);
        p+=2;
        URL = (char*)WPI_Malloc( length+1);
        //KiAllocZeroMemory(length+1, (void**) &URL);
        memcpy(URL,p,length);
        WMMP_LIB_DBG_TEMP("decode url %s\n",URL);
        //parse download url
        ParseDownloadURL(URL);
        /*bob add 20110210*/
        //wmmp2_20WriteUpdateFile();   /*20120508 for compile*/

        //send sig both to wmmp task and ci task;
        wmmpProcessDownloadReq(URL,1);

        if(mainContext_p->WPIEventFucPtr!=PNULL)
        {
            WPIEventData.WPIDownReqEvent.ret= 1;
            WPIEventData.WPIDownReqEvent.url= URL;

            (mainContext_p->WPIEventFucPtr)(WPI_DOWN_REQ_EVENT,&WPIEventData);

        }
        else
        {
            WPI_Free(URL);
        }

        return WMMP_DECODE_OK; 
    }
    else
    {
        return WMMP_DECODE_ERROR; 
    }
}

WmmpDecodeRecvResult_e DecodeFileReqAck(PDUContext_t *pduContext)
{
    char *p=pduContext->BodyConst;
    WmmpContext_t *mainContext_p = ptrToWmmpContext();
    WPIEventData_t WPIEventData;

    char TransID[4+1];
    u16 length;
    u8 status;
    char *data=PNULL;
    char crc16[2];
    u32 readpoint;
    u16   loopi = 0;

    WMMP_LIB_DBG_TEMP("DecodeFileReqAck");

    memcpy(TransID,p,4);
    TransID[4]=0;
    p+=4;
    status = *p;
    p++;
    //skip crc  
    memcpy(crc16,p,2);
    p+=2;
    readpoint = Readu32(p);
    //skip readpoint
    p+=4;

    length = Readu16(p);
    p+=2;
    if (length!=0)
    {
        //KiAllocZeroMemory(length, (void**) &data);
        data = (char*)WPI_Malloc( length);
        memcpy(data,p,length);
    }

#if 0
    for(loopi =0;loopi < 4;loopi++)
    {
        WMMP_LIB_DBG_TEMP("DecodeFileReqAck TransID[%d] 0x%02x",loopi,TransID[loopi]);

    }
#endif

    WMMP_LIB_DBG_TEMP("DecodeFileReqAck status %d  readpoint %d length %d crc16[0] 0x%x crc16[1] 0x%x",status,readpoint,length,crc16[0],crc16[1]);
    if((mainContext_p->WPIEventFucPtr!=PNULL)&&(mainContext_p->PDUUsage==WMMP_PDU_ATCOMMAND))
    {
        WPIEventData.WPIFileAckEvent.data= data;
        WPIEventData.WPIFileAckEvent.CRC16 = crc16;
        WPIEventData.WPIFileAckEvent.readpoint = readpoint;
        WPIEventData.WPIFileAckEvent.blocksize= length;
        memcpy(WPIEventData.WPIFileAckEvent.transID,TransID,4);
        WPIEventData.WPIFileAckEvent.status= status;
        (mainContext_p->WPIEventFucPtr)(WPI_FILE_ACK_EVENT,&WPIEventData);

    }		
    //wmmpSendFileCnf(VG_CI_TASK_ID, data,crc16,readpoint,length,TransID,status);
    //send sig to CI 
    return WMMP_DECODE_OK; 
}

WmmpDecodeRecvResult_e DecodeTransparentDateAck(PDUContext_t *pduContext)
{
    WmmpContext_t *mainContext_p = ptrToWmmpContext();
    WPIEventData_t WPIEventData;
    char *p=pduContext->BodyConst;
    u8 status;
    status = *p;

    WMMP_LIB_DBG_TEMP("DecodeTransparentDateAck ");

    if(mainContext_p->WPIEventFucPtr!=PNULL)
    {
        WPIEventData.WPIDataAckEvent.ret = status;
        (mainContext_p->WPIEventFucPtr)(WPI_DATA_ACK_EVENT,&WPIEventData);
    }

    return WMMP_DECODE_OK; 
}

WmmpDecodeRecvResult_e DecodeSecurityCfgAck(PDUContext_t *pduContext)
{
    char *p=pduContext->BodyConst;
    u8 ret;
    char *q;	
    WmmpContext_t *mainContext = ptrToWmmpContext();
    ret= *p;
    p++;

    WMMP_LIB_DBG_TEMP("DecodeSecurityCfgAck\n");
    if(ret==0)
    {
        mainContext->TLVtempLen = pduContext->BodyLength-1;			
        if(mainContext->TLVtemp!=PNULL)
        {
            WPI_Free(mainContext->TLVtemp);
            WMMP_LIB_DBG_TEMP("WMMP stack---Error!, mainContext->TLVtemp is not null\n");
        }
        mainContext->TLVtemp = (char*)WPI_Malloc(mainContext->TLVtempLen);
        memcpy(mainContext->TLVtemp,p,mainContext->TLVtempLen);	
    }
    wmmpProcessSecurtiyCfgAck(ret);
    return WMMP_DECODE_OK; 
}

WmmpDecodeRecvResult_e DecodeLoginOutReq(PDUContext_t *pduContext)
{
    char *p=pduContext->BodyConst;
    u8 ret;
    ret= *p;

    WMMP_LIB_DBG_TEMP("DecodeLoginOutReq ret=%d\n",ret);
    //  wmmpSendLogoutReq(SIMCOM_WMMP_TASK_ID, ret);  
    wmmpProcessServerLogoutReq(ret);
    return WMMP_DECODE_OK; 
}

WmmpDecodeRecvResult_e DecodeTransparentDateReq(PDUContext_t *pduContext)
{
    char type;
    char source[20];
    u16 len;
    u16 datalen;
    u16 tag;
    char *data;
    u16 CRC;
    char *p=pduContext->BodyConst;
    WmmpContext_t *mainContext_p = ptrToWmmpContext();
    WPIEventData_t WPIEventData;	

    WMMP_LIB_DBG_TEMP("DecodeTransparentDateReq");

    memcpy(&CRC,p,2);
    p+=2;
    tag = Readu16(p);
    //4013 or 4015
    if(tag==0x4013)
        type = 'T';
    else
        type = 'E';

    p+=2;
    len = Readu16(p);
    p+=2;
    memset(source,0,20);
    memcpy(source,p,len);/* cbc @20100422:数据不能超过20字节*/
    p+=len;

    //read tag 0x4007 user data;
    tag = Readu16(p);
    p+=2;
    datalen = Readu16(p);
    p+=2;

    data = (char*)WPI_Malloc(datalen);
    //KiAllocZeroMemory(datalen, (void**) &data);	
    memcpy(data,p,datalen);

    //send to wmmp task;
    wmmpProcessTransDataReq(type, source, data, datalen);

    if(mainContext_p->WPIEventFucPtr!=PNULL)
    {
        WPIEventData.WPIDataReqEvent.type= type;
        memcpy(WPIEventData.WPIDataReqEvent.des,source,20);
        WPIEventData.WPIDataReqEvent.data =data;
        WPIEventData.WPIDataReqEvent.datalen = datalen;
        (mainContext_p->WPIEventFucPtr)(WPI_DATA_REQ_EVENT,&WPIEventData);
    }	
    else
    {
        WPI_Free(data);
    }
    return WMMP_DECODE_OK; 
}

WmmpDecodeRecvResult_e RecvCommand(WmmpTransType_e type, u16 length)
{
    WmmpContext_t *mainContext = ptrToWmmpContext();
    PDUContext_t   *pduContext = &mainContext->PDUContext_receive;	
    //WmmStackState_e RecvState=pduContext->StackState;
    WmmpDecodeRecvResult_e ret=WMMP_DECODE_NOPDU;
    //wmmp_ascii * pl_StrPtr;

    WMMP_LIB_DBG_TEMP("RecvCommand StackState=%d",mainContext->StackState);
    WMMP_LIB_DBG_TEMP("m2mswitch %d",mainContext->m2mswitch );
    WMMP_LIB_DBG_TEMP("mainContext->PDUUsage %d ",mainContext->PDUUsage);
    if(mainContext->m2mswitch == false)
    {
        return ret;
    }    

    ret = DecodeHeadData(length);/*cbc220100402:报文头    */
    if(ret==WMMP_DECODE_OK)
    {
        mainContext->TransType = type;
        ret=DecodeAbstract(pduContext);/*cbc220100402:是否有摘要    */
        WMMP_LIB_DBG_TEMP("DecodeAbstract() =  %d",ret);	
    }
    if(ret==WMMP_DECODE_OK)
    {
        ret=DecodeDispartProcess(pduContext);/*cbc220100402:  检查分包  */
        WMMP_LIB_DBG_TEMP("DecodeDispartProcess() =  %d",ret);	
    }
    if(ret==WMMP_DECODE_OK)
    {
        ret = Decrypt(pduContext);/*cbc220100402:  解密  */
        WMMP_LIB_DBG_TEMP("Decrypt() =  %d",ret);	
    }

    WMMP_LIB_DBG_TEMP("RecvCommand mainContext->StackState=%d ret=%d",mainContext->StackState, ret);

    if(ret==WMMP_DECODE_OK)
    {	    		
        switch(mainContext->StackState)
        {
            /* 终端发起收到服务器的应答 */
            case  WMMP_STACK_SERVER_WAITING:
                {
                    WMMP_LIB_DBG_TEMP("case  WMMP_STACK_SERVER_WAITING");	
                    switch(pduContext->CommandID)
                    {
                        case REGISTER_ACK:
                            {
                                ret = DecodeRegisterAck(pduContext->BodyConst);
                                WMMP_LIB_DBG_TEMP("DecodeRegisterAck() =  %d",ret);
                                break;   
                            }

                        case LOGIN_ACK_COMMAND:
                            {
                                ret = DecodeLoginAck(pduContext->BodyConst);
                                WMMP_LIB_DBG_TEMP("DecodeLoginAck() =  %d",ret);	
                                break;
                            }

                            // these two cammand has no PDU body,so didn't decode
                        case LOGOUT_ACK_COMMAND:
                            {
                                ret = DecodeLogoutAck();
                                WMMP_LIB_DBG_TEMP("DecodeLogoutAck() =  %d",ret);	
                                break;
                            }

                        case HEART_BEAT_ACK_COMMAND:
                            {    
                                WMMP_LIB_DBG_TEMP("HEART_BEAT_ACK_COMMAND");	
                                ret = WMMP_DECODE_OK;
                                break;
                            }

                        case CONFIG_TRAP_ACK_COMMAND:
                            {
                                ret = DecodeCfgTrapAck(pduContext->BodyConst,pduContext);
                                WMMP_LIB_DBG_TEMP("DecodeCfgTrapAck() =  %d",ret);
                                break;
                            }

                        case CONFIG_REQ_ACK_COMMAND:   
                            {
                                ret = DecodeCfgReqAck(pduContext);
                                WMMP_LIB_DBG_TEMP("DecodeCfgReqAck() =  %d",ret);
                                break;
                            }

                        case FILE_REQ_ACK_COMMAND:
                            {
                                ret = DecodeFileReqAck(pduContext);
                                WMMP_LIB_DBG_TEMP("DecodeFileReqAck() =  %d",ret);
                                break;
                            }

                        case TRANSPARENT_DATA_ACK_COMMAND:
                            {
                                ret= DecodeTransparentDateAck(pduContext);
                                WMMP_LIB_DBG_TEMP("DecodeTransparentDateAck() =  %d",ret);
                                break;
                            }

                        case SECURITY_CONFIG_ACK_COMMAND:
                            {
                                ret=DecodeSecurityCfgAck(pduContext);
                                WMMP_LIB_DBG_TEMP("DecodeSecurityCfgAck() =  %d",ret);
                                break;
                            }

                        default:
                            {
                                WMMP_LIB_DBG_TEMP("WMMP_STACK_STATUS_ERROR() ");
                                ret = WMMP_STACK_STATUS_ERROR;
                                break;
                            }
                    }
                }	
                break;

                /* 服务器主动发起收到服务器终端的应答 */
            case WMMP_STACK_CLIENT_WAITING:
                {
                    WMMP_LIB_DBG_TEMP("case  WMMP_STACK_CLIENT_WAITING");	
                    switch(pduContext->CommandID)
                    {			            
                        case SECURITY_CONFIG_COMMAND:
                            {					  
                                ret = DecodeSecurityCfg(pduContext->BodyConst,pduContext,type);
                                WMMP_LIB_DBG_TEMP("DecodeSecurityCfg() =  %d",ret);
                                break;
                            }

                        case CONFIG_GET_COMMAND:
                            {
                                WMMP_LIB_DBG_TEMP("from m2m:CONFIG_GET_COMMAND");
                                ret = DecodeCfgGet(pduContext);
                                break;
                            }

                        case CONFIG_SET_COMMAND:
                            {
                                WMMP_LIB_DBG_TEMP("from m2m:CONFIG_SET_COMMAND");
                                ret = DecodeCfgSet(pduContext);
                                break;
                            }

                        case REMOTE_CTRL_COMMAND:
                            {
                                WMMP_LIB_DBG_TEMP("from m2m:REMOTE_CTRL_COMMAND");
                                ret = DecodeRemoteCtrl(pduContext);
                                break;
                            }

                        case DOWNLOAD_INFO_COMMAND:
                            {
                                WMMP_LIB_DBG_TEMP("from m2m:DOWNLOAD_INFO_COMMAND");
                                ret = DecodeDownload(pduContext);
                                break;
                            }

                        case TRANSPARENT_DATA_COMMAND:
                            {
                                WMMP_LIB_DBG_TEMP("from m2m:TRANSPARENT_DATA_COMMAND");
                                ret= DecodeTransparentDateReq(pduContext);
                                WMMP_LIB_DBG_TEMP("from m2m:TRANSPARENT_DATA_COMMAND 1");

                                break;
                            }

                        case LOGOUT_COMMAND:
                            {	
                                char *p=pduContext->BodyConst;
                                u8 logoutreason;
                                logoutreason= *p;

                                mainContext->PDUUsage=WMMP_PDU_LOGOUT_TERM;

                                WMMP_LIB_DBG_TEMP("from m2m:LOGOUT_COMMAND");
                                ret = DecodeLoginOutReq(pduContext);

                                /* add by cbc@20100408 begin */
                                if (logoutreason == 8)
                                {
                                    //mainContext->Wmmp_status.net_status = LINK_FAILED;
                                    WMMP_LIB_DBG_TEMP("LOGOUT login at once,ret=%d,reason =%d",ret,logoutreason);
                                }
                                else
                                {
                                    WMMP_LIB_DBG_TEMP("LOGOUT login at once,ret=%d,reason =%d",ret,logoutreason);

                                    //							WPI_sleep(1000);
                                    wmmpProcessLogoutCnf();
                                }
                                /* add by cbc@20100408 end */
                                break;
                            }

                        default:
                            {
                                WMMP_LIB_DBG_TEMP("WMMP_STACK_STATUS_ERROR");
                                ret = WMMP_STACK_STATUS_ERROR;
                                break;
                            }
                    }
                }
                break;

            default:
                {
                    WMMP_LIB_DBG_TEMP("WMMP_STACK_STATUS_ERROR default");	   
                    break;
                }
        }

    }
    else
    {
        WMMP_LIB_DBG_TEMP(" not WMMP_DECODE_OK ret %d",ret);
    }

    WMMP_LIB_DBG_TEMP("RecvCommand  ret %d type %d mainContext->TransType %d PDUUsage %d",ret,type,mainContext->TransType,mainContext->PDUUsage);
    switch(ret)
    {
        case WMMP_DECODE_OK:	
            if( (mainContext->StackState==WMMP_STACK_SERVER_WAITING))
            {
                WMMP_LIB_DBG_TEMP("mainContext->StackState==WMMP_STACK_SERVER_WAITING");

                mainContext->StackState=WMMP_STACK_INIT;/* add by    cbc@20100408 for 在重查发送定时器未来前如果平台马上主动发起报文会丢包 */

                //if((mainContext->PDUUsage==WMMP_TLV_REBOOT)||(mainContext->PDUUsage==WMMP_LOCAL_CONFIG_REBOOT)) 
                /*bob modify 20101122*/
                if((mainContext->PDUUsage==WMMP_TLV_REBOOT)||(mainContext->PDUUsage==WMMP_LOCAL_CONFIG_REBOOT) || (mainContext->PDUUsage==WMMP_CONFIG_SET_APN_REBOOT) || (mainContext->PDUUsage==WMMP_REMOTE_CONTROL_REBOOT) ||(mainContext->PDUUsage==WMMP_RMCON_REFACTORY_REBOOT)) 
                {
                    if(mainContext->PDUUsage==WMMP_REMOTE_CONTROL_REBOOT)  /*bob add 20110122*/
                    {
                        WMMP_LIB_DBG_TEMP("RecvCommand  before WPI_SystemReboot ret %d type %d mainContext->TransType %d",ret,type,mainContext->TransType);													

                        if (LOGOUT_ACK_COMMAND ==pduContext->CommandID)
                        {  /*remote_ctrl_ack 后需要执行logout，等平台回复LOGOUT_ACK_COMMAND 再重启*/
                            WPI_SystemReboot();	
                        }
                    }
                    else
                    {
                        WMMP_LIB_DBG_TEMP("mainContext->PDUUsage==WMMP_TLV_REBOOT and other ");
                        wmmpStartInterTimer(PDU_INTERVAL_TIMER, 2);/*cbc@20100406:检查发送缓冲区*/	
                    }
                }
                else
                {
                    WMMP_LIB_DBG_TEMP("StackState==WMMP_STACK_SERVER_WAITING:PDU_INTERVAL_TIMER");
                    wmmpStartInterTimer(PDU_INTERVAL_TIMER, 1);/*cbc@20100406:检查发送缓冲区*/	

#if 0
                    if ( LOGINOK == ptrToWmmpContext()->Wmmp_status.protocol_status  &&
                            LOGIN_ACK_COMMAND == pduContext->CommandID)
                    {
                        /*login successful and receive config trap ack from servicer*/
                        WPI_send_urc(WMMP_AT_EVENT_STR"%d,%s %s", WMMP_AET_LIB_STATE, "0500", "Logged-in:ready for operations");
                    }
#endif //0                            

                }
            }
            else
            {
                WMMP_LIB_DBG_TEMP("mainContext->StackState!=WMMP_STACK_SERVER_WAITING");
            }
            break;

        case WMMP_DECODE_ERROR:
            //若是模块主动发起的报文返回ERROR,则等待超时重发
            //若是服务器主动发起,则继续下一条报文的处理
            if (mainContext->StackState==WMMP_STACK_CLIENT_WAITING)
            {
                WMMP_LIB_DBG_TEMP("WMMP_DECODE_ERROR 1");
                wmmpStartInterTimer(PDU_INTERVAL_TIMER, 2);
            }
            else
            {
                WMMP_LIB_DBG_TEMP("WMMP_DECODE_ERROR 2");
            }
            break;		

        case WMMP_DECODE_FRAME_ERROR:/*cbc@2010:临界区收到的报文保存起来等待下次处理*/
            {
                if(mainContext->isCriticalUsed==false)
                {
                    WMMP_LIB_DBG_TEMP("WMMP_DECODE_FRAME_ERROR 	1");
                    SaveToCriticalBuffer(mainContext->IOBuffer, length, type);
                }
                else
                {
                    WMMP_LIB_DBG_TEMP("WMMP_DECODE_FRAME_ERROR 	2");
                }
            }
            break;		

        case WMMP_STACK_STATUS_ERROR:
            //wmmpSendNextCommand();
            {
                //避免有错误报文进入critical buffer，且发送队列中有报文时，不会继续发送报文的问题
                if(mainContext->StackState==WMMP_STACK_INIT)
                {
                    WMMP_LIB_DBG_TEMP("WMMP_STACK_STATUS_ERROR 	1");
                    wmmpStartInterTimer(PDU_INTERVAL_TIMER, 2);
                }
                else
                {
                    WMMP_LIB_DBG_TEMP("WMMP_STACK_STATUS_ERROR 	2");
                }
            }
            break;

        default:
            WMMP_LIB_DBG_TEMP("default ");

    }



    if (ret!=WMMP_DECODE_NOPDU)
    {
        if(type==WMMP_TRANS_SMS)
        {
            if(mainContext->isM2MStatTime ==true)
            {
                mainContext->StatisticContext.SMSReceivedNum++;
            }
            if(mainContext->CustomContext.SMSM2MStatRecFirstTime!=0)
            {
                mainContext->CustomContext.SMSM2MRecvStat++;
            }
        }
        else
        {
            if(mainContext->isM2MStatTime ==true)
            {
                mainContext->StatisticContext.GRPSDataSize+=pduContext->SendLength;
            }			
            if(mainContext->CustomContext.GPRSM2MStatRecFirstTime!=0)
            {
                mainContext->CustomContext.GPRSM2MRecvStat+=length;
            }			
        }  
    }
    return ret;
}


#endif
