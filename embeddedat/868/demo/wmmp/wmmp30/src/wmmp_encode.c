#if defined(__MODEM_WMMP30_SIMCOM__)
/*****************************************************************
* Include Files
*****************************************************************/
#include <stdarg.h>

#include "wmmp_typ.h"
#include "wmmp_lib.h"
#include "string.h"
#include "stdio.h"
#include "wmmp_encode.h"
//#include "wmmp_main.h"
#include "wmmp_crc.h"
#include "wmmp_md5.h"
//#include "wmmp_util.h"
#include "wmmp_tlv.h"
#include "wmmp_interface.h"

#include "Simcom_wmmp_utility.h"

/*****************************************************************
* Manifest Constants
*****************************************************************/

/*****************************************************************
* Types
*****************************************************************/

/*****************************************************************
* Variables (Extern 、Global and Static)
*****************************************************************/
static u8 simcardchangedflagtest = 0;
extern u8   urcRegTerminalIDFlag;
extern u8 urcSecurityFlag;
extern ProtocolStatus_e gM2MStatus;
//extern WmmpSecurityContext_t		wmmpSavedSecurityContext;
extern u8  bhvValue[MAX_BHV_MODE];


void memcpyTLV(TLVRetVal_t TLV, char*des)
{
	switch(TLV.TLVType)
	{
		case   TLV_STRING:
		{
			memcpy(des,TLV.addr,TLV.length);
		}
		break;
  		case TLV_INT8:
		{
			*des = *TLV.addr;
		}
		break;				
		case TLV_INT16:
		{
			*des = *(TLV.addr+1);
			*(des+1) = *TLV.addr;
			
		}
		break;			
		case TLV_INT32:
		{
			*des = *(TLV.addr+3);
			*(des+1) = *(TLV.addr+2);
			*(des+2) = *(TLV.addr+1);
			*(des+3) = *TLV.addr;
		}
		break;			
		default:
			break;
	}
}





void Encrypt(PDUContext_t   *pduContext)
{
	WmmpContext_t *mainContext = ptrToWmmpContext();
	bool EncryptFlag = false;	

	WMMP_LIB_DBG_TEMP("Encrypt:SecuritySet=0X%X,TransType=%d",pduContext->SecuritySet,mainContext->TransType);	
   WMMP_LIB_DBG_TEMP("Encrypt:ConnectMode=%d\n",mainContext->ControlContext.ConnectMode);	

	if (mainContext->ControlContext.ConnectMode == 4)  /* 短信注册 模式*/
	{
#if defined(GUIZHOU_WMMP)
	mainContext->TransType = WMMP_TRANS_GPRS;
#else	
		mainContext->TransType=WMMP_TRANS_SMS;
#endif	
		WMMP_LIB_DBG_TEMP("Encrypt TRANS_SMS:SecuritySet=%d,SendLen=%d,\n",pduContext->SecuritySet,pduContext->SendLength);

		pduContext->SecuritySet = pduContext->SecuritySet&~(1<<4); /* 不要加密 */				
	}/*CBC@20100402:加密条件--SecuritySet-报文头安全标识备用, encryptionUsed: TLV 	-0XE032为1,而且密钥不能为空*/
	
	WMMP_LIB_DBG_TEMP("Encrypt:SecuritySet=0X%X,TransType=0X%X\n",pduContext->SecuritySet,mainContext->TransType);	

	if((pduContext->CommandID==REGISTER_COMMAND)||(pduContext->CommandID==LOGIN_COMMAND))
	return;

	/* cbc@20100418:多包情况下只看加密使能位是否需要加密 */
	if( ((mainContext->StackState) ==WMMP_STACK_SERVER_CONTINUE)||((mainContext->StackState) ==WMMP_STACK_CLIENT_CONTINUE))
	{
		if((mainContext->SecurityContext.encryptionUsed==1)&&(mainContext->SecurityContext.convPublicKey[0]!=0))
		{
			WMMP_LIB_DBG_TEMP("not dispart Encrypt,SecuritySet=%d,encryptionUsed=%d\n",pduContext->SecuritySet,mainContext->SecurityContext.encryptionUsed);	
			EncryptFlag = true;			
		}

		pduContext->SecuritySet = 0xF0; //多包报文必须要摘要
	}
	else   /* cbc@20100418:非多包情况下加密条件*/
	{
		if(((pduContext->SecuritySet&(1<<4))||(mainContext->SecurityContext.encryptionUsed==1))
			&&(mainContext->SecurityContext.convPublicKey[0]!=0))
		{
			WMMP_LIB_DBG_TEMP("not dispart Encrypt,SecuritySet=%d,encryptionUsed=%d\n",pduContext->SecuritySet,mainContext->SecurityContext.encryptionUsed);	
			EncryptFlag = true;
		}
	}
	
	//if(((pduContext->SecuritySet&(1<<4))||(mainContext->SecurityContext.encryptionUsed==1))&&(mainContext->SecurityContext.convPublicKey[0]!=0))
	if (EncryptFlag)
	{
		if(pduContext->BodyLength!=0)
		//if(pduContext->SecuritySet&(1<<5))
		{
			//if(pduContext->SecuritySet&(1<<4))
			//{
			//fully encrypt
			char *encrypt;
			char *p;
			u16 encrypt_length;
			/*
			pduContext->SecuritySet |=(1<<6);			
			//pduContext->SecuritySet |=(1<<5);
			pduContext->SecuritySet |=(1<<4);	*/
			pduContext->SecuritySet = 0xF0; //加密报文必须要摘要

			encrypt = (char*)WPI_Malloc(pduContext->BodyLength+10);

			WMMP_LIB_DBG_TEMP("Encrypt before simcom_wmmp_msg_dbg_printf()\n");			
                   	//simcom_wmmp_msg_dbg_printf((char*)pduContext->BodyConst,(u16)((pduContext->BodyLength)));//      // 20100315@CBC DEBUG 

			// KiAllocZeroMemory(pduContext->BodyLength+10, (void**) &encrypt);
			m2m_3des_set_2key(mainContext->SecurityContext.convPublicKey);
			encrypt_length=m2m_3des_encrypt(pduContext->BodyConst,pduContext->BodyLength,encrypt,pduContext->BodyLength+10);

			WMMP_LIB_DBG_TEMP("Encrypt after simcom_wmmp_msg_dbg_printf()\n");			
                   //simcom_wmmp_msg_dbg_printf(encrypt,(u16)((pduContext->BodyLength+10)));//      // 20100315@CBC DEBUG 

			p=pduContext->BodyConst;
			p=Writeu16(p,0xE042);
			p=Writeu16(p,encrypt_length);
			memcpy(p,encrypt,encrypt_length);
			pduContext->BodyLength=encrypt_length+4;
			WPI_Free(encrypt);
			
		}
	}
}



//#define EACH_PART_BODY_LENGTH 900
//#define EACH_PART_MAX 1024
//#define EACH_PART_BODY_LENGTH 100
//#define EACH_PART_MAX 200
void  DispartProcess(PDUContext_t   *pduContext)
{
	u16 bodyLength ;
	WmmpContext_t *mainContext = ptrToWmmpContext();
	PDUContext_t   *PDUContext_receive = &mainContext->PDUContext_receive;
	
	PDUContext_receive->dispartFlag = true; /* CBC@20100423:这个为false 	时影响多包上报的状态机,非常重要 */
	
	//just think about GPRS,if body TLVs lenth > 900 then dispart，every sub body length is 900	
	bodyLength = pduContext->BodyLength;

	WMMP_LIB_DBG_TEMP("DispartProcess:SecuritySet=0X%X,TransType=%d,ConnectMode=%d",pduContext->SecuritySet,mainContext->TransType,mainContext->ControlContext.ConnectMode);	

	WMMP_LIB_DBG_TEMP("DispartProcess simcom_wmmp_msg_dbg_printf()");			
      	//simcom_wmmp_msg_dbg_printf((char*)pduContext->BodyConst,(u16)((pduContext->BodyLength)));//      // 20100315@CBC DEBUG 

	
	if(bodyLength>mainContext->CustomContext.MaxPDULength)/* cbc@20100402: 	报文体超过最大900字节就需要分包处理*/
	{
		char  *p= mainContext->IOBuffer;

		//WMMP_LIB_DBG_TEMP("DispartProcess simcom_wmmp_msg_dbg_printf()\n");			
      		//simcom_wmmp_msg_dbg_printf((char*)pduContext->BodyConst,(u16)((pduContext->BodyLength)));//      // 20100315@CBC DEBUG 

		if(pduContext->NeedDispart==false) /* cbc@20100402:    子包第一包处理*/
		{
			WMMP_LIB_DBG_TEMP("DispartProcess:pduContext->NeedDispart==false");	
			if(mainContext->StackState== WMMP_STACK_SERVER_WAITING)
			{     /* cbc@20100402: 单包转化为多包*/
				mainContext->StackState=WMMP_STACK_SERVER_CONTINUE;//eg TRAP_COMMAND
				WMMP_LIB_DBG_TEMP("DispartProcess:WMMP_STACK_SERVER_CONTINUE");	
			}
			else if(mainContext->StackState== WMMP_STACK_CLIENT_WAITING)
			{      /* cbc@20100402: 单包转化为多包*/
				mainContext->StackState= WMMP_STACK_CLIENT_CONTINUE;//eg GET_COMMAND
				WMMP_LIB_DBG_TEMP("DispartProcess:WMMP_STACK_CLIENT_WAITING");	
			}

			//first dispart and calc need how many sub 
			pduContext->NeedDispart = true;
			pduContext->SubPartCount = bodyLength/(mainContext->CustomContext.MaxPDULength)+1;
			pduContext->CurrentPartNo =1;
			pduContext->SendLength = mainContext->CustomContext.MaxPDULength;//first part is alway 900

			WMMP_LIB_DBG_TEMP("DispartProcess:SubPartCount=%d,SendLength=%d",pduContext->SubPartCount,pduContext->SendLength);		

			//save first frame
			Writeu32(mainContext->FisrtFrameNo,mainContext->FrameNo);			

		}
		else /* cbc@20100402:    子包第二包开始处理*/
		{
			pduContext->CurrentPartNo++;
			
			WMMP_LIB_DBG_TEMP("DispartProcess NEXT PARA FROM M2M:SubPartCount=%CurrentPartNo=%d,partallnum=%d,partnum=%d,oper=%d,TransID=0x%x%x,",
				            pduContext->SubPartCount,pduContext->CurrentPartNo,PDUContext_receive->partallnum,PDUContext_receive->partnum,PDUContext_receive->oper,PDUContext_receive->TransID[0],PDUContext_receive->TransID[1]);

			/*cbc@20100423: 多包上报后收到平台的异常回复应该退出当前操作 , begin*/
			 if(     ((pduContext->CurrentPartNo) != (PDUContext_receive->partnum))
			 		||((pduContext->SubPartCount)!= (PDUContext_receive->partallnum))  			 	
			 			||(2==(PDUContext_receive->oper)) 
			 				||(3==( PDUContext_receive->oper))) /* 	应该继续检查 TRANSID  的值是否相同 */
			 				
		 	{
		 		WMMP_LIB_DBG_TEMP("DispartProcess:receive err");	

				p+=PDU_HEADER_LENGTH; //prepare for the head info in the future				

				/******************************************************
				* cbc@20100423  异常终止的报文格式:
				* 00278005000000050300100041485731303030313038414231323334   
				* 4008000700000003000302    
				*****************************************************/
				p = Writeu16(p,0x4008); /* cbc@20100402:分包机制TLV */
				pduContext->SendLength = 7 ;
				p = Writeu16(p,pduContext->SendLength);
				p = Writeu16(p,0);
				p = Writeu16(p,pduContext->SubPartCount);
				p = Writeu16(p,pduContext->CurrentPartNo);
				*p=0x02;/* 异常停止分包交互,退出 */
				p++;
                          pduContext->SendLength += 4;
						  
				if(mainContext->StackState== WMMP_STACK_CLIENT_CONTINUE)
				{
					mainContext->StackState= WMMP_STACK_CLIENT_WAITING;//eg GET_COMMAND
					WMMP_LIB_DBG_TEMP("DispartProcess:WMMP_STACK_CLIENT_WAITING");	
				}
				if(mainContext->StackState== WMMP_STACK_SERVER_CONTINUE)
				{
					mainContext->StackState= WMMP_STACK_SERVER_WAITING;//eg TRAP_COMMAND
					WMMP_LIB_DBG_TEMP("DispartProcess:WMMP_STACK_SERVER_CONTINUE");	
				}	

				pduContext->SecuritySet |=(1<<4);
				
				return;/* 直接退出 */
				
				/*cbc@20100423: 多包上报后收到平台的异常回复应该退出当前操作 , end*/
		 	}
			 else
			 {
			
				if(pduContext->CurrentPartNo==pduContext->SubPartCount) /* cbc@20100402: 子包是否是最后一包*/
				{				
					pduContext->SendLength = bodyLength-mainContext->CustomContext.MaxPDULength*(pduContext->CurrentPartNo-1);
					WMMP_LIB_DBG_TEMP("DispartProcess:the last 	package,SendLength=%d",pduContext->SendLength);	
				}
				else
				{
					WMMP_LIB_DBG_TEMP("DispartProcess:the next package");	
					pduContext->SendLength = mainContext->CustomContext.MaxPDULength;
				}
			 }
			//p=pduContext->DispartHead;	
		}

		p+=PDU_HEADER_LENGTH; //prepare for the head info in the future
		//memcpy(p,pduContext->BodyConst,pduContext->BodyTLV-pduContext->BodyConst);
		//p+=(pduContext->BodyTLV-pduContext->BodyConst);

		p = Writeu16(p,0x4008); /* cbc@20100402:分包机制TLV */
		p = Writeu16(p,pduContext->SendLength+7);
		p = Writeu16(p,0);
		p = Writeu16(p,pduContext->SubPartCount);
		p = Writeu16(p,pduContext->CurrentPartNo);
		*p=0x00;
		p++;
		memcpy(p,pduContext->BodyConst+(pduContext->CurrentPartNo-1)*mainContext->CustomContext.MaxPDULength,pduContext->SendLength);

		WMMP_LIB_DBG_TEMP("DispartProcess simcom_wmmp_msg_dbg_printf()");			
      		//simcom_wmmp_msg_dbg_printf((char*)(pduContext->BodyConst+(pduContext->CurrentPartNo-1)*mainContext->CustomContext.MaxPDULength),(u16)((pduContext->SendLength)));//      // 20100315@CBC DEBUG 

		pduContext->SendLength+=(4+7);//the length of 0x4008
		
		if(pduContext->CurrentPartNo==pduContext->SubPartCount)
		{
			if(mainContext->StackState== WMMP_STACK_CLIENT_CONTINUE)
			{
				mainContext->StackState= WMMP_STACK_CLIENT_WAITING;//eg GET_COMMAND
				WMMP_LIB_DBG_TEMP("DispartProcess:WMMP_STACK_CLIENT_WAITING");	
			}
			if(mainContext->StackState== WMMP_STACK_SERVER_CONTINUE)
			{
				mainContext->StackState= WMMP_STACK_SERVER_WAITING;//eg TRAP_COMMAND
				WMMP_LIB_DBG_TEMP("DispartProcess:WMMP_STACK_SERVER_CONTINUE");	
			}			
		}

		//add dispart mark
		pduContext->SecuritySet |=(1<<4);

	}
	else
	{	
		WMMP_LIB_DBG_TEMP("not dispart");	
		pduContext->NeedDispart = false;
		pduContext->SendLength = pduContext->BodyLength;
	}

}

void EncodePDUHeader(PDUContext_t   *pduContext)
{
	char *p;
	WmmpContext_t *mainContext = ptrToWmmpContext();
	TLVRetVal_t TLV;

	WMMP_LIB_DBG_TEMP("EncodePDUHeader:SecuritySet=0X%X,TransType=%d,ConnectMode=%d StackState %d",pduContext->SecuritySet,mainContext->TransType,mainContext->ControlContext.ConnectMode,mainContext->StackState);	

	//短信方式不带摘要
	if(mainContext->TransType==WMMP_TRANS_SMS) 
	{		
		WMMP_LIB_DBG_TEMP("EncodePDUHeader:mainContext->TransType==WMMP_TRANS_SMS");	
		pduContext->SecuritySet = pduContext->SecuritySet&~(1<<5); /* SMS 不要MD5*/		
		WMMP_LIB_DBG_TEMP("EncodePDUHeader:SecuritySet=0x%x",pduContext->SecuritySet );
	}

	if(mainContext->ReSendTime ==0)
	{
		WMMP_LIB_DBG_TEMP("EncodePDUHeader:mainContext->ReSendTime ==0");	
		// the length of head
		pduContext->SendLength +=PDU_HEADER_LENGTH;
		//if need abstract then add 20

		WMMP_LIB_DBG_TEMP("EncodePDUHeader:SendLength =%d,SecuritySet=0x%x",pduContext->SendLength,pduContext->SecuritySet );	
		
		//if((pduContext->SecuritySet)&(1<<5))/*cbc@20100516: & 和&& 的命令结果完全不同 */
		if((pduContext->SecuritySet)&0x20)
		{
			WMMP_LIB_DBG_TEMP("EncodePDUHeader:pduContext->SendLength +=PDU_ABSTRACT_LENGTH");	
			pduContext->SendLength +=PDU_ABSTRACT_LENGTH;
		}
	}


	if(pduContext->NeedDispart==false)
	{
		WMMP_LIB_DBG_TEMP("EncodePDUHeader:NeedDispart==false");	
		p=mainContext->SendBuffer;
	}
	else
	{
		WMMP_LIB_DBG_TEMP("EncodePDUHeader:NeedDispart!=false");
		p=mainContext->IOBuffer;
	}	
	
	
	WMMP_LIB_DBG_TEMP("EncodePDUHeader:SendLength=%d",pduContext->SendLength);
	p = Writeu16(p,pduContext->SendLength);
	p = Writeu16(p,pduContext->CommandID);

	if((mainContext->StackState== WMMP_STACK_CLIENT_CONTINUE)||(mainContext->StackState==WMMP_STACK_CLIENT_WAITING))    
	{
		WMMP_LIB_DBG_TEMP("EncodePDUHeader:1");
		memcpy(p,mainContext->ReceiveFrameNo,4);
		p+=4;
	}
	else
	{
		WMMP_LIB_DBG_TEMP("EncodePDUHeader:2");
		p = Writeu32(p,mainContext->FrameNo);
	//mainContext->FrameNo++;    
	}


	p = Writeu16(p,PROTOCOL_VERSION);
	
	*p = pduContext->SecuritySet;

	
	p++;
	*p = 0;
	p++;
	//should read TLV 0x3002
	WMMP_LIB_DBG_TEMP("EncodePDUHeader:TerminalSeriesNum %s",mainContext->StatusContext.TerminalSeriesNum);
	ReadTLV(0x3002,&TLV);
	memcpyTLV(TLV,p);
	//memcpy(p,mainContext->StatusContext.TerminalSeriesNum,16);
	//WPI_LOG("TerminalSeriesNum is %s",mainContext->StatusContext.TerminalSeriesNum);

}


/*****************************************************************************
add by cbc@20100412:SIM 卡更换,重新注册,
测试时还用原来SIM卡,此时修改绑定IMSI号,算摘要时还用原IMSI号   ,
begin
************************************************************************************/
void sim_card_changed_test(u8 vall)
{
	simcardchangedflagtest = vall;
}
/*****************************************************************************
add by cbc@20100412:SIM 卡更换,重新注册,
测试时还用原来SIM卡,此时修改绑定IMSI号,算摘要时还用原IMSI号   ,
end
************************************************************************************/
void EncodeAbstract(PDUContext_t   *pduContext)
{
	u16 len;
	char *TLV;
	char *head,*p;
	char md5[16];
	WmmpContext_t *mainContext = ptrToWmmpContext();
	TLVRetVal_t TLVret;
	memset(md5,0,16);
	
	WMMP_LIB_DBG_TEMP("EncodeAbstract:SecuritySet=0X%X,TransType=%d,ConnectMode=%d\n",pduContext->SecuritySet,mainContext->TransType,mainContext->ControlContext.ConnectMode);	

	//if(pduContext->SecuritySet&(1<<5))	
	if(pduContext->SecuritySet&0x20)		
	{
		if (mainContext->TransType==WMMP_TRANS_SMS)/* CBC@20100516 :    不要MD5 */
		{
			WMMP_LIB_DBG_TEMP("EncodeAbstract:SendLength =%d\n",pduContext->SendLength);	
			
			/* cbc@20100516:短信不需要摘要,如果有摘要需要去掉摘要长度,begin */
			//pduContext->SendLength = pduContext->SendLength-PDU_ABSTRACT_LENGTH;
			/* cbc@20100516:短信不需要摘要,如果有摘要需要去掉摘要长度,end */
		}
		else
		{
			if(pduContext->NeedDispart==false)
			{			
				head=mainContext->SendBuffer;
				WMMP_LIB_DBG_TEMP("NeedDispart==false:simcom_wmmp_msg_dbg_printf()\n");	
				//simcom_wmmp_msg_dbg_printf((char*)mainContext->SendBuffer,(u16)((pduContext->SendLength)));
			}
			else
			{
				/********************************************************************************************
				*cbc@20100426:终端多包请求情况下，	如果终端第一个包发送没有收到平台 ACK                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     
	                    ，此时终端继续重发时发送的数据会产生错误。
	                    *********************************************************************************************/
				head=mainContext->IOBuffer;				
				WMMP_LIB_DBG_TEMP("EncodeAbstract NeedDispart simcom_wmmp_msg_dbg_printf()\n");			
	      			//simcom_wmmp_msg_dbg_printf((char*)mainContext->IOBuffer,(u16)((pduContext->BodyLength)));//      // 20100315@CBC DEBUG 
			}
			
			p = head;
			p+=pduContext->SendLength-PDU_ABSTRACT_LENGTH;
			
			//p=Writeu32(p, mainContext->TimeStamp);
			if(pduContext->CommandID==LOGIN_COMMAND)
			{
				memcpy(p,"\x00\x00\x00\x00",4);
				WMMP_LIB_DBG_TEMP("MD5 ABSTRACT TimeStamp 	= 0\n");			
			}
			else
			{
				memcpy(p,mainContext->TimeStamp,4);
				WMMP_LIB_DBG_TEMP("MD5 ABSTRACT TimeStamp = %x,%x,%x,%x\n",mainContext->TimeStamp[0],mainContext->TimeStamp[1],mainContext->TimeStamp[2],mainContext->TimeStamp[3]);	
			}
			p+=4;

			ReadTLV(0xE00E,&TLVret);/*E00E */
			memcpyTLV(TLVret,p);
			p+=TLVret.length;
			WMMP_LIB_DBG_TEMP("MD5 IMEI simcom_wmmp_msg_dbg_printf()\n");			
			//simcom_wmmp_msg_dbg_printf((char*)TLVret.addr,(u16)((TLVret.length)));//      // 20100315@CBC DEBUG 

			//when change the sim card ,should use e014 instead of e012
			ReadTLV(0xE014,&TLVret);/* 原IMSI 号  */
			if (simcardchangedflagtest)  /* 测试使用,正常使用不进入 */
			{
				WMMP_LIB_DBG_TEMP("MD5 IMSI simcom_wmmp_msg_dbg_printf()\n");	
				ReadTLV(0xE012,&TLVret);/* imsi */
			}
			if (*TLVret.addr ==0)//the length return is always 15
			{
				WMMP_LIB_DBG_TEMP("MD5 IMSI simcom_wmmp_msg_dbg_printf()\n");	
				ReadTLV(0xE012,&TLVret);/* imsi */
			}

			memcpyTLV(TLVret,p);
			p+=TLVret.length;
			WMMP_LIB_DBG_TEMP("MD5 IMSI simcom_wmmp_msg_dbg_printf()\n");			
			//simcom_wmmp_msg_dbg_printf((char*)TLVret.addr,(u16)((TLVret.length)));//      // 20100315@CBC DEBUG 


			ReadTLV(0xE025,&TLVret);
			memcpyTLV(TLVret,p);
			p+=TLVret.length;
			WMMP_LIB_DBG_TEMP("MD5 UPLINK PIN simcom_wmmp_msg_dbg_printf()\n");			
			//simcom_wmmp_msg_dbg_printf((char*)TLVret.addr,(u16)((TLVret.length)));//      // 20100315@CBC DEBUG 


			//test

			//test
			// memcpy(p,mainContext->uplinkPassword,8);

			//  p+=8;        
			WMMP_LIB_DBG_TEMP("EncodeAbstract simcom_wmmp_msg_dbg_printf()\n");			
			//simcom_wmmp_msg_dbg_printf((char*)head,(u16)((p-head)));//      // 20100315@CBC DEBUG 
			wmmp_md5((u8 *)head,(u32)(p-head),(unsigned char *)md5);
			WMMP_LIB_DBG_TEMP("md5 simcom_wmmp_msg_dbg_printf()\n");			
			//simcom_wmmp_msg_dbg_printf((char*)md5,16);//      // 20100315@CBC DEBUG 

			p=head+pduContext->SendLength-PDU_ABSTRACT_LENGTH;
			p=Writeu16(p,0xE021);
			p=Writeu16(p,0x10);
			memcpy(p,md5,16);

	             WMMP_LIB_DBG_TEMP("EncodeAbstract simcom_wmmp_msg_dbg_printf()\n");			
		      //simcom_wmmp_msg_dbg_printf((char*)(mainContext->SendBuffer),(u16)(pduContext->SendLength));//      // 20100315@CBC DEBUG 
		}
			
	}
}

/* cbc@20100519:计算两个TAG 的TLV 组的摘要数据 */
void encodeSecurityMd5(u16 Tag1,u16 Tag2,char* md5out)
{
	char *head= NULL;
	char md5input[200] ;
	TLVRetVal_t TLVret;
	char md5[16];
	u8 md5InputLen = 0;

	memset(md5,0,16);
	memset(md5input,0,sizeof(md5input));
	head = (char *)&md5input;

	ReadTLV(Tag1,&TLVret);/*  */
	head=Writeu16(head,Tag1);
	head=Writeu16(head,TLVret.length);
	memcpyTLV(TLVret,head);
	head+=TLVret.length;
	md5InputLen +=TLVret.length+2+2;

	WMMP_LIB_DBG_TEMP("encodeSecurityMd5 simcom_wmmp_msg_dbg_printf(),Tag1=0x%x\n",Tag1);			
	//simcom_wmmp_msg_dbg_printf((char*)md5input,md5InputLen);//     

	ReadTLV(Tag2,&TLVret);/*  */
	head=Writeu16(head,Tag2);
	head=Writeu16(head,TLVret.length);
	memcpyTLV(TLVret,head);
	head+=TLVret.length;
	md5InputLen +=TLVret.length+2+2;

	WMMP_LIB_DBG_TEMP("encodeSecurityMd5 :simcom_wmmp_msg_dbg_printf(),Tag1=0x%x,Tag2=0x%x\n",Tag1,Tag2);			
	//simcom_wmmp_msg_dbg_printf((char*)md5input,md5InputLen);//     

	wmmp_md5((u8 *)md5input,(u32)(md5InputLen),(unsigned char *)md5);

	WMMP_LIB_DBG_TEMP("encodeSecurityMd5 :simcom_wmmp_msg_dbg_printf(),Tag1=0x%x,Tag2=0x%x\n",Tag1,Tag2);			
	//simcom_wmmp_msg_dbg_printf((char*)md5,16);//     
	
	memcpy(md5out,md5,16);

	head= NULL;
}

/* cbc@20100519:计算0XE002 的值= MD5(终端序列好+IMEI+IMSI+PIN1)  */
void encodeSecurityPin1Md5(char* md5out)
{
	char *head= NULL;
	char md5input[200] ;
	TLVRetVal_t TLVret;
	char md5[16];
	u8 md5InputLen = 0;

	memset(md5,0,16);
	memset(md5input,0,sizeof(md5input));
	head = (char *)&md5input;

	ReadTLV(0x3002,&TLVret);/* 终端序列号 */
	head=Writeu16(head,0x3002);
	head=Writeu16(head,TLVret.length);
	memcpyTLV(TLVret,head);
	head+=TLVret.length;
	md5InputLen +=TLVret.length+2+2;
	WMMP_LIB_DBG_TEMP("encodeSecurityPin1Md5 0x3002:TERMINALid simcom_wmmp_msg_dbg_printf()\n");			
	//simcom_wmmp_msg_dbg_printf((char*)md5input,md5InputLen);//     

	ReadTLV(0xE00E,&TLVret);/* IMEI */
	head=Writeu16(head,0xE00E);
	head=Writeu16(head,TLVret.length);
	memcpyTLV(TLVret,head);
	head+=TLVret.length;
	md5InputLen +=TLVret.length+2+2;
	WMMP_LIB_DBG_TEMP("encodeSecurityPin1Md5 0x3002+0xE00E:IMEI simcom_wmmp_msg_dbg_printf()\n");			
	//simcom_wmmp_msg_dbg_printf((char*)md5input,md5InputLen);//     

	ReadTLV(0x3014,&TLVret);/*  IMSI  */
	head=Writeu16(head,0x3014);
	head=Writeu16(head,TLVret.length);
	memcpyTLV(TLVret,head);
	head+=TLVret.length;
	md5InputLen +=TLVret.length+2+2;
	WMMP_LIB_DBG_TEMP("encodeSecurityPin1Md5 0x3002+0xE00E+0x3014 :IMSI simcom_wmmp_msg_dbg_printf()\n");			
	//simcom_wmmp_msg_dbg_printf((char*)md5input,md5InputLen);//     

	ReadTLV(0xe001,&TLVret);/*  pin1  */
	head=Writeu16(head,0xe001);
	head=Writeu16(head,TLVret.length);
	memcpyTLV(TLVret,head);
	head+=TLVret.length;
	md5InputLen +=TLVret.length+2+2;
	WMMP_LIB_DBG_TEMP("encodeSecurityPin1Md5 :0x3002+0xE00E+0x3014+0xe001 :pin1 simcom_wmmp_msg_dbg_printf()\n");			
	//simcom_wmmp_msg_dbg_printf((char*)md5input,md5InputLen);//     
 
	wmmp_md5((u8 *)md5input,(u32)(md5InputLen),(unsigned char *)md5);
	WMMP_LIB_DBG_TEMP("encodeSecurityPin1Md5 :simcom_wmmp_msg_dbg_printf()\n");			
	//simcom_wmmp_msg_dbg_printf((char*)md5,16);//     
	
	memcpy(md5out,md5,16);

	head= NULL;
}



void EncodeRegisterCommand(CommandInfo_t *CommandInfo ,PDUContext_t   *pduContext)
{
	WmmpContext_t *mainContext = ptrToWmmpContext();
	//PDUContext_t   *pduContext = mainContext->PDUContext;
	char  *p= mainContext->SendBuffer;
	TLVRetVal_t TLVret;

	WMMP_LIB_DBG_TEMP("EncodeRegisterCommand" );
	
	pduContext->CommandID = REGISTER_COMMAND;
	//pduContext->CommandIDAck = REGISTER_ACK;
	pduContext->SecuritySet = CommandInfo->SecuritySet;
	
	mainContext->StackState = WMMP_STACK_SERVER_WAITING;
	
	//KiAllocZeroMemory(1024, (void**) &p);
	//pduContext->Head = p;
	//skip header first
	
	p+=PDU_HEADER_LENGTH;  //定义上头长度
	pduContext->BodyConst= p;
	*p = CommandInfo->CommandExInfo.RegisterInfo.operation;
	p++;
    
	ReadTLV(0xE00E,&TLVret);
	memcpyTLV(TLVret,p);
	p+=TLVret.length;
    
	ReadTLV(0xE012,&TLVret);
	memcpyTLV(TLVret,p);
	p+=TLVret.length;	
    
	switch(CommandInfo->CommandExInfo.RegisterInfo.operation)
	{
		case 3:
		{
			char temp[50];
			char *q;
			char temp_md5[16];
			p=Writeu16(p,0xE026);
			p=Writeu16(p,0x10);
			//cal 0xE026
			memset(temp,0,50);
			q=temp;
			q=Writeu16(q,0xE025);
			q=Writeu16(q,8);

			ReadTLV(0xE025,&TLVret);
			memcpyTLV(TLVret,q);
			q+=TLVret.length;	 

			// memcpy(q,mainContext->SecurityContext.uplinkPassword,8);
			//WPI_LOG("uplinkPassword %s",mainContext->SecurityContext.uplinkPassword);
			//q+=8;
			q=Writeu16(q,0xE027);
			q=Writeu16(q,4);
			q=Writeu32(q,0xFFFFFFFF);
			//CUSTOMER_PRINT_DATA2(temp,q-temp);

			wmmp_md5((u8 *)temp,(u32)(q-temp),(unsigned char *)temp_md5);
			memcpy(p,temp_md5,16);
			p+=16;

			p=Writeu16(p,0xE029);
			p=Writeu16(p,0x10);	
			//cal 0xE029
			memset(temp,0,50);
			q=temp;
			q=Writeu16(q,0xE028);
			q=Writeu16(q,8);

			ReadTLV(0xE028,&TLVret);
			memcpyTLV(TLVret,q);
			q+=TLVret.length;
			//test
			//memcpy(q,mainContext->SecurityContext.downlinkPassword,8);
			//WPI_LOG("downlinkPassword %s",mainContext->SecurityContext.downlinkPassword);
			//q+=8;   
			q=Writeu16(q,0xE02A);
			q=Writeu16(q,4);
			q=Writeu32(q,0xFFFFFFFF);
			wmmp_md5((u8 *)temp,(u32)(q-temp),(unsigned char *)temp_md5);
			memcpy(p,temp_md5,16);
			p+=16;

			p=Writeu16(p,0xE03A);
			p=Writeu16(p,0x10);	
			//cal 0xE03A
			memset(temp,0,50);
			q=temp;
			q=Writeu16(q,0xE036);
			q=Writeu16(q,24);  
			ReadTLV(0xE036,&TLVret);
			memcpyTLV(TLVret,q);
			q+=TLVret.length;            
			//test
			//memcpy(q,mainContext->SecurityContext.publickey,24);
			//WPI_LOG("publickey %s",mainContext->SecurityContext.publickey);
			//q+=24;
			q=Writeu16(q,0xE038);
			q=Writeu16(q,4);
			q=Writeu32(q,0xFFFFFFFF);			
			wmmp_md5((u8 *)temp,(u32)(q-temp),(unsigned char *)temp_md5);
			memcpy(p,temp_md5,16);
			p+=16;
		}
		break;
		
		case 4:
		{
			p=Writeu16(p,0xE020);
			p=Writeu16(p,0x1);		
			
			ReadTLV(0xE020,&TLVret);
			memcpyTLV(TLVret,p);
			p+=TLVret.length;
		}
		break;	
		
		case 2:
		{
			pduContext->SecuritySet |=(1<<5);
		}
		break;	

		default:
			break;
			
	}
	pduContext->BodyTLV=p;
	pduContext->BodyLength=p-mainContext->SendBuffer-PDU_HEADER_LENGTH;
	WMMP_LIB_DBG_TEMP("new module, register!\n");
}


void EncodeLoginCommand(CommandInfo_t *CommandInfo,PDUContext_t   *pduContext)
{
	WmmpContext_t *mainContext = ptrToWmmpContext();
	char  *q ,*p= PNULL;
	u16 length = 0;
	char *TLVaddr=PNULL;
	u8 i;
	char TLV_buffer[1024]; //temp for calc TLV CRC
	TLVRetVal_t TLVsret,TLVret;
	u32 CRC_value=0;
	u16  loopi=16;
	u8 ber  = 0;
	char  strImsi[16] = "\0";

	WMMP_LIB_DBG_TEMP("EncodeLoginCommand" );

	pduContext->CommandID = LOGIN_COMMAND;
	
	if(mainContext->SecurityContext.encryptionUsed==1)
	{
		pduContext->SecuritySet = 224;//11100000
	}
	else if(mainContext->SecurityContext.encryptionUsed==0)
	{
		pduContext->SecuritySet = 0xA0;//10100000
	}
	/*
	pduContext->SecuritySet = CommandInfo->SecuritySet;
    //login need abstract
	pduContext->SecuritySet |=(1<<5);*/
    mainContext->TransType = CommandInfo->TransType;
		
	//pduContext->CommandIDAck = LOGIN_ACK_COMMAND;

	mainContext->StackState = WMMP_STACK_SERVER_WAITING;    
	//KiAllocZeroMemory(1024, (void**) &q);
	//pduContext->Head = q;
	//skip header first
	q= mainContext->SendBuffer;
	q+=PDU_HEADER_LENGTH;
	pduContext->BodyConst= q;
	//add terminal software version
#if 1 /*bob modify 20101126*/	
	//memcpy(q,mainContext->StatusContext.FirmwareRev,8);
	WMMP_LIB_DBG_TEMP("EncodeLoginCommand FirmwareRev %s",mainContext->StatusContext.FirmwareRev);
	memcpy(q,mainContext->StatusContext.FirmwareRev,4); /*bob modify 20110223*/
#else
	memcpy(q,(const char*)TERMINAL_SOFTWARE_VERSION,8);
#endif
	q+=8;
	//WPI_GetIMSI(strImsi,16);
	//memcpy(q,(const char*)strImsi,15);
	//q+=15;
	//calc CRC32

   	ReadTLV(0x0025,&TLVsret);
	TLVaddr = TLVsret.addr;
	memset(TLV_buffer,0,sizeof(TLV_buffer));
	p = TLV_buffer;
	

   for(i=0;i<TLVsret.length/2;i++)
   	{
   		memcpy(p,TLVaddr,2);
		p+=2;
		ReadTLV(Readu16(TLVaddr),&TLVret);
		TLVaddr+=2;
		p=Writeu16(p, TLVret.length);
		memcpyTLV(TLVret,p);
		p+=TLVret.length;
   	}
   
	 WMMP_LIB_DBG_TEMP("LOGIN CRC_value simcom_wmmp_msg_dbg_printf()\n");			
    	//simcom_wmmp_msg_dbg_printf((char*)TLV_buffer,(u16)((p-TLV_buffer)));//      // 20100315@CBC DEBUG 
	
	CRC_value = CreateCRC32(TLV_buffer,p-TLV_buffer);
	WMMP_LIB_DBG_TEMP("LOGIN CRC_value 0x%x",CRC_value);
   //memcpy(q,&CRC_value,4);
   //q+=4;
   	q=Writeu32(q, CRC_value);

   	pduContext->BodyTLV=q; 
   //calc 0xE03A
	if(mainContext->SecurityContext.encryptionUsed==1)
	{
		q = Writeu16(q,0xE03A);
		q = Writeu16(q,0x10);
        
        memset(TLV_buffer,0,sizeof(TLV_buffer));
        p = TLV_buffer;
        p=Writeu16(p,0xE036);
        p=Writeu16(p,24);  
        
        ReadTLV(0xE036,&TLVret);
        memcpyTLV(TLVret,p);
        p+=TLVret.length;        
		WMMP_LIB_DBG_TEMP("LOGIN 0XE036 simcom_wmmp_msg_dbg_printf()\n");			
		//simcom_wmmp_msg_dbg_printf((char*)TLVret.addr,(u16)((TLVret.length)));//      // 20100315@CBC DEBUG 

        
        p=Writeu16(p,0xE038);
        p=Writeu16(p,4);
	  ReadTLV(0xE038,&TLVret);
		
	if (*(TLVret.addr)==0)
	{
		p=Writeu32(p,0xFFFFFFFF);
	}
        else
        {
        	memcpyTLV(TLVret, p);
		p+=TLVret.length;
        }
        /*
    	TLVret=ReadTLV(0xE038);
    	memcpyTLV(TLVret, p);           
        p+=TLVret.length; */
	        WMMP_LIB_DBG_TEMP("LOGIN 0XE038 simcom_wmmp_msg_dbg_printf()\n");			
	        //simcom_wmmp_msg_dbg_printf((char*)TLVret.addr,(u16)((TLVret.length)));//      // 20100315@CBC DEBUG 


       // CUSTOMER_PRINT_DATA2(TLV_buffer,p-TLV_buffer);
        
    	wmmp_md5((u8*)TLV_buffer,(u32)(p-TLV_buffer),(unsigned char *)q);
        q+=16;

	}

	if(CommandInfo->CommandExInfo.LoginInfo.clearSecurity==true)
	{
		q=Writeu16(q,0xE020);
		q=Writeu16(q,0x1);		
		
		ReadTLV(0xE020,&TLVret);
		memcpyTLV(TLVret,q);
		q+=TLVret.length;
	}
	
  #if 0
	//重庆移动现网平台要求在登录和心跳报文中添加  TLV3006,3007
	p=Writeu16(p,0x3006);
	p=Writeu16(p,4);
	p=Writeu16(p,WPI_GetLAC());
	p=Writeu16(p,WPI_GetCellID());
	p=Writeu16(p,0x3007);
	p=Writeu16(p,1);
	*p=WPI_GetCSQLevel(&ber);
	p++;
	WMMP_LIB_DBG_TEMP("LOGIN ber %d",ber);
	p=Writeu16(p,0x8100);
	p=Writeu16(p,1);
	ber = WPI_GetCSQLevel(p);
	p++;
  #else
	q=Writeu16(q,0x3006);
	q=Writeu16(q,4);
	q=Writeu16(q,WPI_GetLAC());
	q=Writeu16(q,WPI_GetCellID());
	q=Writeu16(q,0x3007);
	q=Writeu16(q,1);
	*q=WPI_GetCSQLevel(&ber);
	q++;
#if 0	
	q=Writeu16(q,0x3010);
	q=Writeu16(q,6);
	q=Writeu16(q,1);
	q=Writeu16(q,2);
	q=Writeu16(q,3);
#else
	#if 0 /*bob remove 20110223*/
	ReadTLV(0x3010,&TLVret);
	q = Writeu16(q,0x3010);
	q=Writeu16(q,TLVret.length);
	memcpyTLV(TLVret,q);
	q+=TLVret.length;	
	#endif
#endif	
 #if 1	/*bob modify 20110223*/ 
	WMMP_LIB_DBG_TEMP("LOGIN ber %d",ber);
	q=Writeu16(q,0x8100);
	q=Writeu16(q,1);
	ber = WPI_GetCSQLevel(q);
	q++;
#endif

#endif
	
	pduContext->BodyLength=q-mainContext->SendBuffer-PDU_HEADER_LENGTH;
	
}

void EncodeLoginOutCommand(CommandInfo_t *CommandInfo,PDUContext_t   *pduContext)
{
	WmmpContext_t *mainContext = ptrToWmmpContext();
	//PDUContext_t   *pduContext = mainContext->PDUContext;
	char  *p= mainContext->SendBuffer;
	pduContext->CommandID = LOGOUT_COMMAND;
	pduContext->SecuritySet = CommandInfo->SecuritySet;
	//logout need abstract
	pduContext->SecuritySet |=(1<<5);
	mainContext->TransType = CommandInfo->TransType;
	//pduContext->CommandIDAck = LOGOUT_ACK_COMMAND;

	WMMP_LIB_DBG_TEMP("EncodeLoginOutCommand" );

	mainContext->StackState = WMMP_STACK_SERVER_WAITING;    

	//KiAllocZeroMemory(1024, (void**) &p);
	//pduContext->Head = p;
	//skip header first
	p+=PDU_HEADER_LENGTH;
	pduContext->BodyConst= p;
	
	 WMMP_LIB_DBG_TEMP("LOGOUT_COMMAND reason=%d\n",CommandInfo->CommandExInfo.LoginOutInfo.reason);	

	*p =CommandInfo->CommandExInfo.LoginOutInfo.reason;
	p++;

	pduContext->BodyTLV=p;
	pduContext->BodyLength=p-mainContext->SendBuffer-PDU_HEADER_LENGTH;	


}


// the tlv tag must be sorted for encrypt
void EncodeConfigTrapCommand(CommandInfo_t *CommandInfo,PDUContext_t   *pduContext)
{
	u8	len = 0;
	u16 loopi = 0;
	u16 loopMax4 = 0;
	u16 loopMax2 = 0;
	WmmpContext_t *mainContext = ptrToWmmpContext();
	//PDUContext_t   *pduContext = mainContext->PDUContext;
	char  *p= mainContext->SendBuffer;
	u16 *q;
	u16 tag,i;
	u16 totollength;
	TLVRetVal_t TLVret;
	u8 allocnum=1;
	u8 tmpStr1[5] = "";
	u8 tmpStr2[3] = "";
	u16	tmpINT16 = 0;
	u16	tmpINT8 = 0;
	u16	tmpTag = 0;


	WMMP_LIB_DBG_TEMP("EncodeConfigTrapCommand" );
    
	pduContext->CommandID = CONFIG_TRAP_COMMAND;
	pduContext->SecuritySet = CommandInfo->SecuritySet;  
	pduContext->SecuritySet |=(1<<5);
	mainContext->TransType = CommandInfo->TransType;

	mainContext->StackState = WMMP_STACK_SERVER_WAITING;  
	
	//pduContext->Head = p;
	//skip header first
	p+=PDU_HEADER_LENGTH;
	pduContext->BodyConst= p;
	pduContext->BodyTLV= p;

	totollength=PDU_HEADER_LENGTH;
	q=CommandInfo->CommandExInfo.ConfigTrapInfo.TLVTags;
	
	for(i=0;i<CommandInfo->CommandExInfo.ConfigTrapInfo.TagNum;i++)
	{
		tag = *q;
		ReadTLV(tag,&TLVret);
		q++;
		
		WMMP_LIB_DBG_TEMP("EncodeConfigTrapCommand tag is 0x%04x",tag);
		
		 if((TLVret.TLVType == TLV_STRING) && (bhvValue[2] == 1))  /*0x300f*/
		   {
			p=Writeu16(p,tag);
			len = (TLVret.length)/2 ;
			loopMax4 = (TLVret.length)/4;
			p=Writeu16(p,len);
			WMMP_LIB_DBG_TEMP("EncodeConfigTrapCommand:tag=%x,len=%d loopMax4 %d",tag,len,loopMax4);	

			for(loopi = 0;loopi < loopMax4;loopi++)
			{
				memset(tmpStr1,0,sizeof(tmpStr1));
				memcpy(tmpStr1,TLVret.addr+4*loopi,4);
				tmpINT16 = vgAsciiToInt16(tmpStr1);
				WMMP_LIB_DBG_TEMP("EncodeConfigTrapCommand tmpINT16[%d] 0x%04x",loopi,tmpINT16);
				p=Writeu16(p,tmpINT16);	
			}
			if(TLVret.length > loopMax4*4)
			{
				loopMax2 = (TLVret.length - loopMax4*4)/2;
				WMMP_LIB_DBG_TEMP("EncodeConfigTrapCommand loopMax2 %d",loopMax2);
				for(loopi = 0;loopi < loopMax2;loopi++)
				{
					memset(tmpStr2,0,sizeof(tmpStr2));
					memcpy(tmpStr2,TLVret.addr+4*loopMax4+2*loopi,2);				
					tmpINT8 = AsciiTou8(tmpStr2);
					WMMP_LIB_DBG_TEMP("EncodeConfigTrapCommand:tmpINT8[%d] 0x%02x",loopi,tmpINT16,tmpINT8);						
					*p = tmpINT8;
					p++;
				}
			}

		}
		else if((TLV_INT16 == TLVret.TLVType) && (bhvValue[2] == 1))		/*0x300d*/
		{

			p=Writeu16(p,tag);
			p=Writeu16(p,TLVret.length);			

			tmpINT16 = 0;
			memcpy(&tmpINT16,TLVret.addr,2);
			WMMP_LIB_DBG_TEMP("EncodeConfigTrapCommand:TLV_INT16 tag=%x,len=%d tmpINT16 0x%04x",tag,TLVret.length,tmpINT16);	
			//tmpINT16 = bsd_htons(tmpINT16);  			
			WMMP_LIB_DBG_TEMP("EncodeConfigTrapCommand:TLV_INT16 2  tmpINT16 0x%04x",tmpINT16);	
			p=Writeu16(p,tmpINT16);	
		}
		else if((TLV_INT32 == TLVret.TLVType) && (bhvValue[2] == 1))		
		{
			u32 tmpVal = 0;

			p=Writeu16(p,tag);
			p=Writeu16(p,TLVret.length);			

			tmpVal = 0;
			memcpy(&tmpVal,TLVret.addr,4);
			WMMP_LIB_DBG_TEMP("EncodeConfigTrapCommand:TLV_INT32 tag=%x,len=%d tmpVal 0x%08x",tag,TLVret.length,tmpVal);	
			//tmpVal = bsd_htonl(tmpVal);  			
			WMMP_LIB_DBG_TEMP("EncodeConfigTrapCommand:2  tmpVal 0x%08x",tmpVal);	
			p=Writeu32(p,tmpVal);	
		}
		else if((TLV_INT8 == TLVret.TLVType) && (bhvValue[2] == 1))		
		{

			p=Writeu16(p,tag);
			p=Writeu16(p,TLVret.length);			

			tmpINT8 = 0;
			memcpy(&tmpINT8,TLVret.addr,1);			
			WMMP_LIB_DBG_TEMP("EncodeConfigTrapCommand:TLV_INT8 tmpINT8 0x%02x",tmpINT8);						
			*p = tmpINT8;
			p++;
		}	
		else
		{
		p=Writeu16(p,tag);
		p=Writeu16(p,TLVret.length);
			WMMP_LIB_DBG_TEMP("EncodeConfigTrapCommand:tag=%x,len=%d",tag,TLVret.length);	

		memcpyTLV(TLVret,p);
		totollength+=TLVret.length;		
		p+=TLVret.length;

		}		
		
	}

	WPI_Free(CommandInfo->CommandExInfo.ConfigTrapInfo.TLVTags);
	
	pduContext->BodyLength=p-mainContext->SendBuffer-PDU_HEADER_LENGTH;	
}

void EncodeConfigReqCommand(CommandInfo_t *CommandInfo,PDUContext_t   *pduContext)
{
	WmmpContext_t *mainContext = ptrToWmmpContext();
	//PDUContext_t   *pduContext = mainContext->PDUContext;	
	char  *p= mainContext->SendBuffer;
    	u16 *q;
	u16 tag,i;	

	WMMP_LIB_DBG_TEMP("EncodeConfigReqCommand" );
	
	pduContext->CommandID = CONFIG_REQ_COMMAND;
	pduContext->SecuritySet = CommandInfo->SecuritySet;  	
	
	//pduContext->CommandIDAck = CONFIG_REQ_ACK_COMMAND;
	mainContext->StackState = WMMP_STACK_SERVER_WAITING;   
	 mainContext->TransType = CommandInfo->TransType;
	//KiAllocZeroMemory(1024, (void**) &p);
	//pduContext->Head = p;
	//skip header first
	p+=PDU_HEADER_LENGTH;
	pduContext->BodyConst= p;
	pduContext->BodyTLV= p;	
	q= CommandInfo->CommandExInfo.ConfigReqInfo.TLVTags;
	for(i=0;i<CommandInfo->CommandExInfo.ConfigReqInfo.TagNum;i++)
	{
		tag = *q;
		q++;
		p=Writeu16(p,tag);
		p=Writeu16(p,0);
	}
	WPI_Free(CommandInfo->CommandExInfo.ConfigReqInfo.TLVTags);
    //KiFreeMemory((void **) & CommandInfo->CommandExInfo.ConfigReqInfo.TLVTags);
	pduContext->BodyLength=p-mainContext->SendBuffer-PDU_HEADER_LENGTH;		
}

void EncodeSecurityCfgAckCommand(CommandInfo_t *CommandInfo ,PDUContext_t   *pduContext)
{
	WmmpContext_t *mainContext = ptrToWmmpContext();
	//PDUContext_t   *pduContext = mainContext->PDUContext;
	char  *p= mainContext->SendBuffer;
	u8 SecuritySet = CommandInfo->SecuritySet;
	u8 retcode =  CommandInfo->CommandExInfo.SecurityAckInfo.retcode;
	u8 oper = CommandInfo->CommandExInfo.SecurityAckInfo.operation;
    	u16 *TLVtag_p= CommandInfo->CommandExInfo.SecurityAckInfo.tags;
    	u8 tagNum=CommandInfo->CommandExInfo.SecurityAckInfo.tagnum;
	u8 i;
	u16 TLVtag;
	TLVRetVal_t TLVret;

	WMMP_LIB_DBG_TEMP("EncodeSecurityCfgAckCommand" );
	
	pduContext->CommandID = SECURITY_CONFIG_ACK_COMMAND;
	pduContext->SecuritySet = SecuritySet;
	mainContext->TransType = CommandInfo->TransType;
	
    //this PDU need abstract
	pduContext->SecuritySet |=(1<<5);

    
	//KiAllocZeroMemory(1024, (void**) &p);
	//pduContext->Head = p;
	//skip header first
	p+=PDU_HEADER_LENGTH;
	pduContext->BodyConst= p;

	
	*p =retcode;
	p++;
	
	pduContext->BodyTLV=p;
    switch(retcode)
    {
        case 0:
        {
            switch(oper)
            {
                //fill it complete in the future
                case 1:
		   {
			    for(i=0;i<tagNum;i++)
			    {
					
					TLVtag=TLVtag_p[i];
					WMMP_LIB_DBG_TEMP("Security Ack TLV %d \n",TLVtag);
					p=Writeu16(p,TLVtag);
					ReadTLV(TLVtag,&TLVret);

					p=Writeu16(p,TLVret.length);  

					memcpyTLV(TLVret,p);
					p+=TLVret.length;
			    } 
				WPI_Free(TLVtag_p);
				//KiFreeMemory((void **) &TLVtag_p);
			}
			break;
			
			case 2:
			{
				char md5[16];
			    for(i=0;i<tagNum;i++)
			    {
					
					TLVtag=TLVtag_p[i];
					switch(TLVtag)
					{
					case 0xE025:
					TLVtag = 0xE026;
					break;
					case 0xE028:
					TLVtag = 0xE029;
					break;	
					case 0xE036:
					TLVtag = 0xE03A;
					break;
					case 0xE001:
					TLVtag = 0xE002;
					
					break;								
				}
					
			       WMMP_LIB_DBG_TEMP("Security Ack TLV %d\n",TLVtag);
				p=Writeu16(p,TLVtag);
			       ReadTLV(TLVtag,&TLVret);
					
				p=Writeu16(p,TLVret.length); 
					
			        switch(TLVtag)
				{
					char temp[100];
					char *q;
					
					case 0xE002:
						memset(temp,0,sizeof(temp));
						q=temp;
						memcpy(q,mainContext->StatusContext.TerminalSeriesNum,16);
						q+=16;
						memcpy(q,mainContext->SecurityContext.BindIMEI,16);
						q+=16;
						memcpy(q,mainContext->SecurityContext.BindIMSIOrig,15);
						q+=15;
						memcpy(q,mainContext->SecurityContext.SIMPin1,8);
						q+=8;							
						wmmp_md5((u8 *)temp,(u32)(q-temp),(unsigned char *)md5);
						memcpy(p,md5,16);
						p+=16;	
					break;
					
					case 0xE026:
						memset(temp,0,sizeof(temp));
						q=temp;
						q=Writeu16(q,0xE025);
						q=Writeu16(q,8);

						ReadTLV(0xE025,&TLVret);
						memcpyTLV(TLVret,q);
						q+=TLVret.length;	 

						q=Writeu16(q,0xE027);
						q=Writeu16(q,4);

						ReadTLV(0xE027,&TLVret);
						memcpyTLV(TLVret,q);
						q+=TLVret.length;
						//CUSTOMER_PRINT_DATA2(temp,q-temp);
						wmmp_md5((u8 *)temp,(u32)(q-temp),(unsigned char *)md5);							
						memcpy(p,md5,16);
						p+=16;								
					break;
					
					case 0xE029:
						memset(temp,0,sizeof(temp));
						q=temp;
						q=Writeu16(q,0xE028);
						q=Writeu16(q,8);

						ReadTLV(0xE028,&TLVret);
						memcpyTLV(TLVret,q);
						q+=TLVret.length;	

						q=Writeu16(q,0xE02A);
						q=Writeu16(q,4);

						ReadTLV(0xE02A,&TLVret);
						memcpyTLV(TLVret,q);
						q+=TLVret.length;
						//CUSTOMER_PRINT_DATA2(temp,q-temp);

						wmmp_md5((u8 *)temp,(u32)(q-temp),(unsigned char *)md5);
						memcpy(p,md5,16);
						p+=16;								
					break;
					
					case 0xE03A:
						memset(temp,0,sizeof(temp));
						q=temp;
						q=Writeu16(q,0xE036);
						q=Writeu16(q,24);

						ReadTLV(0xE036,&TLVret);
						memcpyTLV(TLVret,q);
						q+=24;	

						q=Writeu16(q,0xE038);
						q=Writeu16(q,4);

						ReadTLV(0xE038,&TLVret);
						memcpyTLV(TLVret,q);
						q+=TLVret.length;
						//CUSTOMER_PRINT_DATA2(temp,q-temp);

						wmmp_md5((u8 *)temp,(u32)(q-temp),(unsigned char *)md5);
						memcpy(p,md5,16);
						p+=16;								
					break;
					
					default:
						memcpyTLV(TLVret,p);
						p+=TLVret.length;
					break;
				}
					

			    }
				WPI_Free(TLVtag_p);
				//KiFreeMemory((void **) &TLVtag_p);
			}
			break;
				
			case 3:
			{
				  WMMP_LIB_DBG_TEMP("Security Ack\n");
			}
			break;
            }
        }
        break;
		
	case 2:
	{
		for(i=0;i<tagNum;i++)
		{

			TLVtag=TLVtag_p[i];
			p=Writeu16(p,TLVtag);				
			p=Writeu16(p,0);  

		} 
		WPI_Free(TLVtag_p);
			//KiFreeMemory((void **) &TLVtag_p);		
	}
	break;
    }
	
	pduContext->BodyLength=p-mainContext->SendBuffer-PDU_HEADER_LENGTH;	   

}


void EncodeConfigGetAckCommand(CommandInfo_t *CommandInfo , PDUContext_t   *pduContext)
{
	WmmpContext_t *mainContext = ptrToWmmpContext();
	
	char  *p= mainContext->SendBuffer;
    u16 *TLVtag_p= CommandInfo->CommandExInfo.ConfigGetAckInfo.TLVTags;
    u8 tagNum=CommandInfo->CommandExInfo.ConfigGetAckInfo.TagNum;
    u8 i;
    u16 TLVtag;
    TLVRetVal_t TLVret;
	pduContext->CommandID = CONFIG_GET_ACK_COMMAND;
	pduContext->SecuritySet = CommandInfo->SecuritySet;
   	mainContext->TransType = CommandInfo->TransType;
    //may be 1024 is not enough;
	//KiAllocZeroMemory(1024, (void**) &p);

    	WMMP_LIB_DBG_TEMP("EncodeConfigGetAckCommand");

	//pduContext->Head = p;
	//skip header first
	p+=PDU_HEADER_LENGTH;
	pduContext->BodyConst= p;
	*p =CommandInfo->CommandExInfo.ConfigGetAckInfo.ret;
	p++;
	
	pduContext->BodyTLV=p;   
	if(CommandInfo->CommandExInfo.ConfigGetAckInfo.ret==0)
	{
	    	for(i=0;i<tagNum;i++)
		{
			//memcpy(p,TLVtag_p+i*2,2);
			//p+=2;
			//TLVtag=Readu16(TLVtag_p+i*2);
			TLVtag=TLVtag_p[i];
			p=Writeu16(p,TLVtag);
			WMMP_LIB_DBG_TEMP("EncodeConfigReqAckCommand read TLV 0x%x",TLVtag);

			ReadTLV(TLVtag,&TLVret);

			p=Writeu16(p,TLVret.length);  

			memcpyTLV(TLVret,p);
			p+=TLVret.length;
		}
	}
	else
	{
	    for(i=0;i<tagNum;i++)
	    {

	       TLVtag=TLVtag_p[i];
		p=Writeu16(p,TLVtag);
		p= Writeu16(p,0);
	    }	
		
		
	}
    //free the memory
    WPI_Free(TLVtag_p);
    //KiFreeMemory((void **) &TLVtag_p);
    pduContext->BodyLength=p-mainContext->SendBuffer-PDU_HEADER_LENGTH;	
}

void EncodeConfigSetAckCommand(CommandInfo_t *CommandInfo , PDUContext_t   *pduContext)
{
    	WmmpContext_t *mainContext = ptrToWmmpContext();
        char  *p= mainContext->SendBuffer;
        u16 *TLVtag_p= CommandInfo->CommandExInfo.ConfigSetAckInfo.errortag;
    	u8 tagNum=CommandInfo->CommandExInfo.ConfigSetAckInfo.errorTagNum;
   	 u8 i;
	 u16 TLVtag;
    	pduContext->CommandID = CONFIG_SET_ACK_COMMAND;
    	pduContext->SecuritySet = CommandInfo->SecuritySet;
		 mainContext->TransType = CommandInfo->TransType;
        //may be 1024 is not enough;
    	//KiAllocZeroMemory(1024, (void**) &p);

    	WMMP_LIB_DBG_TEMP("EncodeConfigSetAckCommand");
        
    	//pduContext->Head = p;
    	//skip header first
    	p+=PDU_HEADER_LENGTH;
    	pduContext->BodyConst= p;
    	*p =CommandInfo->CommandExInfo.ConfigSetAckInfo.ret;
    	p++;
	pduContext->BodyTLV=p;  
    	if(CommandInfo->CommandExInfo.ConfigSetAckInfo.ret==1)
    	{
	    for(i=0;i<tagNum;i++)
	    {

	       TLVtag=TLVtag_p[i];
		p=Writeu16(p,TLVtag);
		p= Writeu16(p,0);
	    }	
		/*
    		p=Writeu16(p, CommandInfo->CommandExInfo.ConfigSetAckInfo.errortag);
			p=Writeu16(p,0);*/
    	}
     //free the memory
   //此处先不释放,由回调函数来释放,否则会导致重复释放
    //	WPI_Free(TLVtag_p);
   
    	   
        pduContext->BodyLength=p-mainContext->SendBuffer-PDU_HEADER_LENGTH;	
}

void EncodeRemoteCtrlAckCommand(CommandInfo_t *CommandInfo , PDUContext_t   *pduContext)
{
	WmmpContext_t *mainContext = ptrToWmmpContext();
	char  *p= mainContext->SendBuffer;
	u16 *TLVtag_p= CommandInfo->CommandExInfo.RemoteCtrlAckInfo.errortag;
	u8 tagNum=CommandInfo->CommandExInfo.RemoteCtrlAckInfo.errorTagNum;
	u8 i;
	u16 TLVtag;

	pduContext->CommandID = REMOTE_CTRL_ACK_COMMAND;
	pduContext->SecuritySet = CommandInfo->SecuritySet;
	mainContext->TransType = CommandInfo->TransType;
	
	//skip header first
	p+=PDU_HEADER_LENGTH;
	pduContext->BodyConst= p;
	*p =CommandInfo->CommandExInfo.RemoteCtrlAckInfo.ret;
	p++;

	pduContext->BodyTLV=p;   

	WMMP_LIB_DBG_TEMP("EncodeRemoteCtrlAckCommand ret=%d\n",CommandInfo->CommandExInfo.RemoteCtrlAckInfo.ret);

	if(CommandInfo->CommandExInfo.RemoteCtrlAckInfo.ret==1)
	{
		for(i=0;i<tagNum;i++)
		{
			TLVtag=TLVtag_p[i];
			p=Writeu16(p,TLVtag);
			p= Writeu16(p,0);
			
			WMMP_LIB_DBG_TEMP("EncodeRemoteCtrlAckCommand TLVtag=%x\n",TLVtag);
		}			
	}
	//free the memory
	WPI_Free(TLVtag_p);	

	pduContext->BodyLength=p-mainContext->SendBuffer-PDU_HEADER_LENGTH;	
}

void EncodeDownLoadAckCommand(CommandInfo_t *CommandInfo , PDUContext_t   *pduContext)
{
    	WmmpContext_t *mainContext = ptrToWmmpContext();
        char  *p= mainContext->SendBuffer;
        
    	pduContext->CommandID = DOWNLOAD_INFO_ACK_COMMAND;
    	pduContext->SecuritySet = CommandInfo->SecuritySet;
       mainContext->TransType = CommandInfo->TransType;

    	//KiAllocZeroMemory(1024, (void**) &p);
    	
    	WMMP_LIB_DBG_TEMP("EncodeDownLoadAckCommand");
    	//pduContext->Head = p;
    	//skip header first
    	p+=PDU_HEADER_LENGTH;
    	pduContext->BodyConst= p;
    	*p =CommandInfo->CommandExInfo.DownloadAckInfo.ret;
    	p++;
    	
    	pduContext->BodyTLV=p;     
        pduContext->BodyLength=p-mainContext->SendBuffer-PDU_HEADER_LENGTH;	
}


void EncodeFileReqCommand(CommandInfo_t *CommandInfo , PDUContext_t   *pduContext)
{
	WmmpContext_t *mainContext = ptrToWmmpContext();
	char  *p= mainContext->SendBuffer;
	WmmpUpdateContext_t  *UpdateContext_p = &(mainContext->UpdateContext);
	u16	ComId = CommandInfo->CommandID;
	
    	WMMP_LIB_DBG_TEMP("EncodeFileReqCommand");

	/*bob add 20110217 for the end FILE_REQ no FILE_ACK*/	
	if((FILE_REQ_COMMAND == CommandInfo->CommandID) && (WMMP_PDU_ATCOMMAND == CommandInfo->PDUUsage) && (2 == CommandInfo->CommandExInfo.FileReqInfo.Status) && (0xFFFFFFFF == CommandInfo->CommandExInfo.FileReqInfo.FileReadPoint) && (1 == UpdateContext_p->adFileFlag))
	{
		pduContext->adFileEndFileReqFlag = 1;
		WMMP_LIB_DBG_TEMP("EncodeFileReqCommand adFileEndFileReqFlag set 1");
	}
	else
	{
		pduContext->adFileEndFileReqFlag = 0;
		WMMP_LIB_DBG_TEMP("EncodeFileReqCommand adFileEndFileReqFlag clear 0");
	}

#if 0
	if((FILE_REQ_COMMAND == CommandInfo->CommandID) && (WMMP_PDU_ATCOMMAND == CommandInfo->PDUUsage) && (3 == CommandInfo->CommandExInfo.FileReqInfo.Status) && (0xFFFFFFFF == CommandInfo->CommandExInfo.FileReqInfo.FileReadPoint) && (1 == UpdateContext_p->adFileFlag))
	{
		pduContext->adFileFileReqAbortFlag = 1;
	}
	else
	{
		pduContext->adFileFileReqAbortFlag = 0;	
	}
#endif	

	pduContext->CommandID = FILE_REQ_COMMAND;
	pduContext->SecuritySet = CommandInfo->SecuritySet;

	//pduContext->CommandIDAck = FILE_REQ_ACK_COMMAND;
	mainContext->StackState = WMMP_STACK_SERVER_WAITING;   
	mainContext->TransType = CommandInfo->TransType;
	//KiAllocZeroMemory(1024, (void**) &p);

	//pduContext->Head = p;
	//skip header first
	p+=PDU_HEADER_LENGTH;
	pduContext->BodyConst= p;
	memcpy(p,CommandInfo->CommandExInfo.FileReqInfo.TransID,4);
	p+=4;

	*p = CommandInfo->CommandExInfo.FileReqInfo.Status;
	p++;

	p=Writeu32(p, CommandInfo->CommandExInfo.FileReqInfo.FileReadPoint);
	p=Writeu16(p,CommandInfo->CommandExInfo.FileReqInfo.FileBlockSize);

	pduContext->BodyTLV=p;     
	pduContext->BodyLength=p-mainContext->SendBuffer-PDU_HEADER_LENGTH;	
}

void EncodeHeartBeatCommand(CommandInfo_t *CommandInfo , PDUContext_t   *pduContext)
{
	u8  ber = 0;
	WmmpContext_t *mainContext = ptrToWmmpContext();
	char  *p= mainContext->SendBuffer;
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_ADC_GPIO__
	WmmpCustom3Context_t  *Custom3Context_p= &(ptrToWmmpContext()->Custom3Context);
#endif
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/
    	WMMP_LIB_DBG_TEMP("EncodeHeartBeatCommand");
        
    	pduContext->CommandID = HEART_BEAT_COMMAND;
    	pduContext->SecuritySet = CommandInfo->SecuritySet;
		 mainContext->TransType = CommandInfo->TransType;	
	//pduContext->CommandIDAck = HEART_BEAT_ACK_COMMAND;
	mainContext->StackState = WMMP_STACK_SERVER_WAITING;   
    	//KiAllocZeroMemory(50, (void**) &p);
        
    	//pduContext->Head = p;
    	//skip header first
    	p+=PDU_HEADER_LENGTH;
    	pduContext->BodyConst= p;
    	pduContext->BodyTLV=p;  

	//重庆移动现网平台要求在登录和心跳报文中添加  TLV3006,3007
	p=Writeu16(p,0x3006);
	p=Writeu16(p,4);
	p=Writeu16(p,WPI_GetLAC());
	p=Writeu16(p,WPI_GetCellID());
	p=Writeu16(p,0x3007);
	p=Writeu16(p,1);
	*p=WPI_GetCSQLevel(&ber);
	p++;
	WMMP_LIB_DBG_TEMP("EncodeHeartBeatCommand ber %d",ber);
	p=Writeu16(p,0x8100);
	p=Writeu16(p,1);
	ber = WPI_GetCSQLevel(p);
	p++;
/*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add begin*/
#ifdef __SIMCOM_WMMP_ADC_GPIO__
	simcom_wmmp_get_adc();
	WPI_Get_Gpio_Status();/*SIMCOM luyong 2012-09-25 Fix MKBug00013842 add */
	while(l4cuem_get_wmmp_adc_flag()==true);
	p=Writeu16(p,0x81f5);
	p=Writeu16(p,2);
	p=Writeu16(p,Custom3Context_p->wmmp_adc_value);
	p=Writeu16(p,0x81f7);
	p=Writeu16(p,4);
	p=Writeu32(p,Custom3Context_p->wmmp_gpio_status);
	WMMP_LIB_DBG_TEMP("EncodeHeartBeatCommand Custom3Context_p->wmmp_adc_value= %2x",Custom3Context_p->wmmp_adc_value);
	WMMP_LIB_DBG_TEMP("EncodeHeartBeatCommand Custom3Context_p->wmmp_gpio_status= %4x",Custom3Context_p->wmmp_gpio_status);
#endif	
 /*SIMCOM luyong 2012-09-05 Fix MKBug00013614  add end*/
       pduContext->BodyLength=p-mainContext->SendBuffer-PDU_HEADER_LENGTH;	    
}
void EncodeLoginOutAckCommand(CommandInfo_t *CommandInfo , PDUContext_t   *pduContext)
{
    	WmmpContext_t *mainContext = ptrToWmmpContext();
        char  *p= mainContext->SendBuffer;

    	WMMP_LIB_DBG_TEMP("EncodeLoginOutAckCommand");
        
    	pduContext->CommandID = LOGOUT_ACK_COMMAND;
    	pduContext->SecuritySet = CommandInfo->SecuritySet;
	pduContext->SecuritySet |=(1<<5);
		
	mainContext->TransType = CommandInfo->TransType;	

	mainContext->StackState = WMMP_STACK_SERVER_WAITING;   

    	p+=PDU_HEADER_LENGTH;
    	pduContext->BodyConst= p;
    	pduContext->BodyTLV=p;     
        pduContext->BodyLength=p-mainContext->SendBuffer-PDU_HEADER_LENGTH;	  
}


void EncodeSecurtiyCommand(CommandInfo_t *CommandInfo , PDUContext_t   *pduContext)
{
	WmmpContext_t *mainContext = ptrToWmmpContext();
	char  *p= mainContext->SendBuffer;
	u16 *q;
	u16 tag,i;
	u16 totollength;
	TLVRetVal_t TLVret;
	u16 tlvtag;

    	WMMP_LIB_DBG_TEMP("EncodeSecurtiyCommand");
    
	pduContext->CommandID = SECURITY_CONFIG_COMMAND;
	pduContext->SecuritySet = CommandInfo->SecuritySet;
	mainContext->TransType = CommandInfo->TransType;	

	mainContext->StackState = WMMP_STACK_SERVER_WAITING;   
	p+=PDU_HEADER_LENGTH;
	pduContext->BodyConst= p;
	*p=	CommandInfo->CommandExInfo.SecurityInfo.operation;
	p++;
	pduContext->BodyTLV=p; 

  	WMMP_LIB_DBG_TEMP("SecurityInfo.operation=%d\n",CommandInfo->CommandExInfo.SecurityInfo.operation);

	/* add cbc@20100416: for 终端发起变更密码密钥begin*/
      if ((CommandInfo->CommandExInfo.SecurityInfo.operation) == 6)
      	{
      		WMMP_LIB_DBG_TEMP("SecurityInfo.operation=%d\n",CommandInfo->CommandExInfo.SecurityInfo.operation);
		mainContext->PDUUsage = WMMP_KEY_OUT_OF_DATE;
      	}
	/* add cbc@20100416: for 终端发起变更密码密钥end*/
	

     /*cbc@20100427:终端上报安全参数_接入密码和基础密钥_密文模式_UDP长连接 */
      if  ((CommandInfo->CommandExInfo.SecurityInfo.operation) == 4)
	{
		totollength=PDU_HEADER_LENGTH;
		q=CommandInfo->CommandExInfo.SecurityInfo.tags;
		
		for(i=0;i<CommandInfo->CommandExInfo.SecurityInfo.tagnum;i++)
		{
			tag = *q;
			ReadTLV(tag,&TLVret);
			q++;
			
			p=Writeu16(p,tag);
			p=Writeu16(p,TLVret.length);
			WMMP_LIB_DBG_TEMP("EncodeConfigTrapCommand:tag=%x,len=%d\n",tag,TLVret.length);	

			/* add by cbc@20100519: 0xe026 = MD5(0XE025+0XE027),0XE029=MD5(0XE028+0XE02A),0XE03A=MD5(0XE036+0X38) */
			if( tag == 0xe026)
			{
				encodeSecurityMd5(0xe025,0XE027,TLVret.addr);
			}
			else if( tag == 0XE029)
			{
				encodeSecurityMd5(0XE028,0XE02A,TLVret.addr);
			}
			else if( tag == 0XE03A)
			{
				encodeSecurityMd5(0XE036,0Xe038,TLVret.addr);
			}
			else if (tag == 0XE002)
			{
				encodeSecurityPin1Md5(TLVret.addr);
			}

			memcpyTLV(TLVret,p);
			totollength+=TLVret.length;		
			p+=TLVret.length;
		}
	 }
	  else	
	 {	
		for(i=0;i<CommandInfo->CommandExInfo.SecurityInfo.tagnum;i++)
		{
			tlvtag=CommandInfo->CommandExInfo.SecurityInfo.tags[i];
			p=Writeu16(p,tlvtag);
			p=Writeu16(p,0);          	
		}
	 }	
	
	
	pduContext->BodyLength=p-mainContext->SendBuffer-PDU_HEADER_LENGTH;	    
}

void EncodeTransparentDataCommandAck(CommandInfo_t * CommandInfo, PDUContext_t * pduContext)
{
	WmmpContext_t *mainContext = ptrToWmmpContext();
	char  *p= mainContext->SendBuffer;
    
    	WMMP_LIB_DBG_TEMP("EncodeTransparentDataCommandAck");

	pduContext->CommandID = TRANSPARENT_DATA_ACK_COMMAND;
	pduContext->SecuritySet = CommandInfo->SecuritySet;
	mainContext->TransType = CommandInfo->TransType;
	p+=PDU_HEADER_LENGTH;
	pduContext->BodyConst= p;
	//suppose it's always correct
	*p=5;
	p++;
	pduContext->BodyTLV=p;     
	pduContext->BodyLength=p-mainContext->SendBuffer-PDU_HEADER_LENGTH;			
}


void EncodeTransparentDataCommand(CommandInfo_t *CommandInfo , PDUContext_t   *pduContext)
{
    	WmmpContext_t *mainContext = ptrToWmmpContext();
        char  *p= mainContext->SendBuffer;  
        u16 CRC16=0;
        TLVRetVal_t TLV;

    	WMMP_LIB_DBG_TEMP("EncodeTransparentDataCommand");

    	pduContext->CommandID = TRANSPARENT_DATA_COMMAND;
    	pduContext->SecuritySet = CommandInfo->SecuritySet;  
		 mainContext->TransType = CommandInfo->TransType;
	//pduContext->CommandIDAck = TRANSPARENT_DATA_ACK_COMMAND;
	mainContext->StackState = WMMP_STACK_SERVER_WAITING;   
	
    	p+=PDU_HEADER_LENGTH;
    	pduContext->BodyConst= p;
		
        if(CommandInfo->CommandExInfo.TransparentDataInfo.Data!=PNULL)
        {
			
			
            CRC16 = CreateCRC16(CommandInfo->CommandExInfo.TransparentDataInfo.Data,CommandInfo->CommandExInfo.TransparentDataInfo.DateLen);   
        }
		
		/*
        memcpy(p,&CRC16,2);
        p+=2;*/

		p=Writeu16(p,CRC16);
		/*
        if(CommandInfo->CommandExInfo.TransparentDataInfo.Type=='T')
        {
            //read 0x4012
            TLV=ReadTLV(0x4012);
        }
        else
        {
            TLV=ReadTLV(0x4014);
        }

        memcpyTLV(TLV,p);
        p+=TLV.length;
*/
		if((CommandInfo->CommandExInfo.TransparentDataInfo.Type=='T')||
			(CommandInfo->CommandExInfo.TransparentDataInfo.Type=='t'))
		{
			p=Writeu16(p, 0x4012);
		}
		else
		{
			p=Writeu16(p, 0x4014);
		}
		p=Writeu16(p, strlen((char *)CommandInfo->CommandExInfo.TransparentDataInfo.destination));
		strcpy((char *)p,(char *)CommandInfo->CommandExInfo.TransparentDataInfo.destination);
		p+=strlen((char *)CommandInfo->CommandExInfo.TransparentDataInfo.destination);
        
    	pduContext->BodyTLV=p; 
		p=Writeu16(p, 0x4007);
		p=Writeu16(p, CommandInfo->CommandExInfo.TransparentDataInfo.DateLen);
        if(CommandInfo->CommandExInfo.TransparentDataInfo.Data!=PNULL)
        {
            memcpy(p,CommandInfo->CommandExInfo.TransparentDataInfo.Data,CommandInfo->CommandExInfo.TransparentDataInfo.DateLen);
            p+=CommandInfo->CommandExInfo.TransparentDataInfo.DateLen;
			  WPI_Free(CommandInfo->CommandExInfo.TransparentDataInfo.Data);
            //KiFreeMemory((void **) &CommandInfo->CommandExInfo.TransparentDataInfo.Data);
        }
        
        pduContext->BodyLength=p-mainContext->SendBuffer-PDU_HEADER_LENGTH;        
}

void EncodeConinueSendCommand(CommandInfo_t *CommandInfo , PDUContext_t   *pduContext)
{
    WmmpContext_t *mainContext = ptrToWmmpContext();
    PDUContext_t   *pduContext_receive = &mainContext->PDUContext_receive;	
	
    char  *p= mainContext->SendBuffer;
    
    WMMP_LIB_DBG_TEMP("EncodeConinueSendCommand");

    switch(CommandInfo->CommandExInfo.ContinueSendInfo.CommandID)
    {
    	/* 只有下面四种情况报文有分包的可能 */
        case CONFIG_SET_COMMAND:
		pduContext->CommandID = CONFIG_SET_ACK_COMMAND;
		WMMP_LIB_DBG_TEMP("EncodeConinueSendCommand CONFIG_SET_COMMAND\n");			
     
    //       *p =0;
     //      p++;
        break;
	
	
        case CONFIG_GET_COMMAND:
		pduContext->CommandID = CONFIG_GET_ACK_COMMAND;
		WMMP_LIB_DBG_TEMP("EncodeConinueSendCommand CONFIG_GET_COMMAND\n");
      //     *p =0;
       //    p++;
        break;	

	   /*******************************************************************************************************
	   *  cbc@20100422:模块发起一个config req     (0x000a )     的请求,平台多包下发给模块,这时如果不是最后一包
	   *  ,终端需要继续多包请求
	   ******************************************************************************************************************/
         case CONFIG_REQ_ACK_COMMAND:
            pduContext->CommandID = CONFIG_REQ_COMMAND;
	      WMMP_LIB_DBG_TEMP("EncodeConinueSendCommand CONFIG_REQ_ACK_COMMAND\n");
        break;	
		
		
	  case CONFIG_TRAP_ACK_COMMAND:
	  	WMMP_LIB_DBG_TEMP("EncodeConinueSendCommand CONFIG_TRAP_ACK_COMMAND\n");
	  	return;
         break;

		 

	 default:
	 	WMMP_LIB_DBG_TEMP("EncodeConinueSendCommand default\n");
	 	return ;
    }   


	/* modified by cbc@20100418:这个可能导致成加密报文 begin */	
	pduContext->SecuritySet = 0;	
	pduContext->SecuritySet |=(1<<4);  
	/* modified by cbc@20100418:这个可能导致成加密报文 end */	

	mainContext->TransType = CommandInfo->TransType;
	//may be 1024 is not enough;
	//KiAllocZeroMemory(1024, (void**) &p);   

	//pduContext->Head = p;
	//skip header first
	p+=PDU_HEADER_LENGTH;
	pduContext->BodyConst= p;

	pduContext->BodyTLV=p;     
	p=Writeu16(p,0x4008);
	p=Writeu16(p,7);
	memcpy(p,CommandInfo->CommandExInfo.ContinueSendInfo.transID,2);
	p+=2;
	p=Writeu16(p,CommandInfo->CommandExInfo.ContinueSendInfo.partallnum);

	/* cbc@20100419:主要实现状态pduContext->CommandID = 	CONFIG_SET_ACK_COMMAND  的异常处理 */
	if( ((pduContext_receive->partnum) != 	CommandInfo->CommandExInfo.ContinueSendInfo.partnum)
		||((CommandInfo->CommandExInfo.ContinueSendInfo.transID[0])!= 	(pduContext_receive->TransID[0]))
		||((CommandInfo->CommandExInfo.ContinueSendInfo.transID[1])!= (pduContext_receive->TransID[1]))
		||((pduContext_receive->oper) == 2)
		||((pduContext_receive->oper) == 3))
	{
		WMMP_LIB_DBG_TEMP("EncodeConinueSendCommand partnum=%d,%d,transid=%d,%d,%d,%d,oper=%d\n",
			pduContext_receive->partnum,
			CommandInfo->CommandExInfo.ContinueSendInfo.partnum,
			CommandInfo->CommandExInfo.ContinueSendInfo.transID[0],
			CommandInfo->CommandExInfo.ContinueSendInfo.transID[1],
			pduContext_receive->TransID[0],
			pduContext_receive->TransID[1],
			pduContext_receive->oper);
		
		p=Writeu16(p,CommandInfo->CommandExInfo.ContinueSendInfo.partnum);
		*p=0x03;   /*  operation type ;发起方异常,终止分包交互*/
		pduContext_receive->dispartFlag = false;
	}
      /* cbc@20100422:  正常处理情况 */
	else
	{		
		WMMP_LIB_DBG_TEMP("EncodeConinueSendCommand partnum=%d,%d,transid=%d,%d,%d,%d,oper=%d\n",
			pduContext_receive->partnum,
			CommandInfo->CommandExInfo.ContinueSendInfo.partnum,
			CommandInfo->CommandExInfo.ContinueSendInfo.transID[0],
			CommandInfo->CommandExInfo.ContinueSendInfo.transID[1],
			pduContext_receive->TransID[0],
			pduContext_receive->TransID[1],
			pduContext_receive->oper);
		
		//pduContext->partnum += 1;
		p=Writeu16(p,CommandInfo->CommandExInfo.ContinueSendInfo.partnum+1);
		*p=1;//operation type
	}
	
	p++;
	pduContext->BodyLength=p-mainContext->SendBuffer-PDU_HEADER_LENGTH;	    

/*
	if(CommandInfo->CommandExInfo.ContinueSendInfo.partnum+1==CommandInfo->CommandExInfo.ContinueSendInfo.partallnum)
	{
		mainContext->StackState=WMMP_STACK_CLIENT_WAITING;
	}*/
}
/*****************************************************************************
*	函数名	      : SendCommand
* 作  者        : 
*	功能描述      : 发送命令,编码并根据当前协议栈状态处理数据
*	输入参数说明  : 无
* 输出参数说明  : 无
*	返回值说明    : 无
*****************************************************************************/
WmmpEncodeSendResult_e SendCommand(CommandInfo_t *CommandInfo)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
	WmmpContext_t *mainContext = ptrToWmmpContext();
	WmmpEncodeSendResult_e ret;
	PDUContext_t   *pduContext = &mainContext->PDUContext;	
	PDUContext_t   *PDUContext_receive = &mainContext->PDUContext_receive;	
	
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
   	WMMP_LIB_DBG_TEMP("SendCommand mainContext->PDUUsage %d CommandInfo->PDUUsage %d adFileEndFileReqFlag %d  StackState %d",mainContext->PDUUsage,CommandInfo->PDUUsage,pduContext->adFileEndFileReqFlag,mainContext->StackState);

	if(CommandInfo!=PNULL)
	{
		switch(mainContext->StackState)
		{
			case  WMMP_STACK_INIT:
			{
				mainContext->PDUUsage=CommandInfo->PDUUsage;
				switch (CommandInfo->CommandID)
				{
					case REGISTER_COMMAND:
						WMMP_LIB_DBG_TEMP("REGISTER_COMMAND");
	    					EncodeRegisterCommand(CommandInfo, pduContext);
						urcRegTerminalIDFlag = 0;
						urcSecurityFlag = 0;
		    			break;
						
		    			case LOGIN_COMMAND:
						WMMP_LIB_DBG_TEMP("LOGIN_COMMAND");
		    				EncodeLoginCommand(CommandInfo, pduContext);
		    			break;
						
			    		case LOGOUT_COMMAND:
						WMMP_LIB_DBG_TEMP("LOGOUT_COMMAND");
			    			EncodeLoginOutCommand(CommandInfo,pduContext);
			    		break;	
						
					case CONFIG_TRAP_COMMAND:
						WMMP_LIB_DBG_TEMP("CONFIG_TRAP_COMMAND");
					   	EncodeConfigTrapCommand(CommandInfo, pduContext);
					break;	
					
					case CONFIG_REQ_COMMAND:
						WMMP_LIB_DBG_TEMP("CONFIG_REQ_COMMAND");
					    	EncodeConfigReqCommand(CommandInfo, pduContext);
					break;	
					
					case HEART_BEAT_COMMAND:
						WMMP_LIB_DBG_TEMP("HEART_BEAT_COMMAND");
					    	EncodeHeartBeatCommand(CommandInfo,pduContext);
					break;
					
					case TRANSPARENT_DATA_COMMAND:
						WMMP_LIB_DBG_TEMP("TRANSPARENT_DATA_COMMAND");
					    	EncodeTransparentDataCommand(CommandInfo,pduContext);
					break;
					
					case FILE_REQ_COMMAND:
						WMMP_LIB_DBG_TEMP("FILE_REQ_COMMAND");
						EncodeFileReqCommand(CommandInfo,pduContext);
					break;	
					
					case SECURITY_CONFIG_COMMAND:
						WMMP_LIB_DBG_TEMP("SECURITY_CONFIG_COMMAND");
						EncodeSecurtiyCommand(CommandInfo,pduContext);
					break;

					default:
						WMMP_LIB_DBG_TEMP("default");
						break;
					
				}
			}
			break;
			
			case WMMP_STACK_CLIENT_WAITING:
			{
				switch (CommandInfo->CommandID)
				{
					case LOGOUT_ACK_COMMAND:
						WMMP_LIB_DBG_TEMP("LOGOUT_ACK_COMMAND");
						EncodeLoginOutAckCommand(CommandInfo, pduContext);
					break;

					case SECURITY_CONFIG_ACK_COMMAND:
						WMMP_LIB_DBG_TEMP("SECURITY_CONFIG_ACK_COMMAND");
						EncodeSecurityCfgAckCommand(CommandInfo, pduContext);
					break;

					case CONFIG_GET_ACK_COMMAND:
						WMMP_LIB_DBG_TEMP("CONFIG_GET_ACK_COMMAND");
						EncodeConfigGetAckCommand(CommandInfo,pduContext);
					break;

					case CONFIG_SET_ACK_COMMAND:
						WMMP_LIB_DBG_TEMP("CONFIG_SET_ACK_COMMAND");
						EncodeConfigSetAckCommand(CommandInfo,pduContext);
					break;

					case REMOTE_CTRL_ACK_COMMAND:
						WMMP_LIB_DBG_TEMP("REMOTE_CTRL_ACK_COMMAND");	
						EncodeRemoteCtrlAckCommand(CommandInfo,pduContext);
					break;

					case DOWNLOAD_INFO_ACK_COMMAND:
						WMMP_LIB_DBG_TEMP("DOWNLOAD_INFO_ACK_COMMAND");			
						EncodeDownLoadAckCommand(CommandInfo,pduContext);
					break;	

					case TRANSPARENT_DATA_ACK_COMMAND:
						WMMP_LIB_DBG_TEMP("TRANSPARENT_DATA_ACK_COMMAND");		
						EncodeTransparentDataCommandAck(CommandInfo,pduContext);
					break;	

					default:
						WMMP_LIB_DBG_TEMP("CommandInfo->CommandID default");		
						break;					

				}
			}
			break;
			
			case  WMMP_STACK_SERVER_CONTINUE:/*CBC:zyc@20100402 -- 多包请求出现的第二个子包开始的TLV 4008 长度不对*/
		   	case  WMMP_STACK_CLIENT_CONTINUE:
			{
				WMMP_LIB_DBG_TEMP("EncodeConinueSendCommand");		
				EncodeConinueSendCommand(CommandInfo,pduContext);/*cbc@20100402:子包应答*/
		   	}
			break;
		}
	

		Encrypt(pduContext); /* cbc@20100401:是否加密 */
	    }

	DispartProcess(pduContext); /* cbc@20100401:是否分包*/
	
	EncodePDUHeader(pduContext);        /* cbc@20100401:添加报文头*/
	EncodeAbstract(pduContext);           /* cbc@20100401:是否添加摘要*/
	WMMP_LIB_DBG_TEMP("SendCommand be wmmpSendPDU simcom_wmmp_msg_dbg_printf()");		
	//void simcom_wmmp_msg_dbg_printf(char *input,u16 len);
	
	//send  from GPRS or SMS
	wmmpSendPDU(pduContext);

	WMMP_LIB_DBG_TEMP("SendCommand,StackState=%d,dispartFlag=%d",mainContext->StackState,PDUContext_receive->dispartFlag);
	if((mainContext->StackState==WMMP_STACK_INIT)||(mainContext->StackState==WMMP_STACK_CLIENT_WAITING))
	{    
		WMMP_LIB_DBG_TEMP("EncodeConinueSendCommand 2");
		/* cbc@20100401:  1秒后检查是否还有数据未发送 */
		wmmpStartInterTimer(PDU_INTERVAL_TIMER, 2);
	}

	/* cbc@20100419:平台下发多包错误,结束退出重新发送别的数据 */
	if((( mainContext->StackState ==WMMP_STACK_SERVER_CONTINUE)||
	 	( mainContext->StackState 	==WMMP_STACK_CLIENT_CONTINUE))&&(!(PDUContext_receive->dispartFlag)) )
	{		
		WMMP_LIB_DBG_TEMP("EncodeConinueSendCommand 3,dispartFlag=%d,StackState=%d",PDUContext_receive->dispartFlag,mainContext->StackState);
		wmmpStartInterTimer(PDU_INTERVAL_TIMER, 2);
	}
	
    return WMMP_ENCODE_OK;
}


void ResendCommand(void)
{
	
	WmmpContext_t *mainContext = ptrToWmmpContext();
	PDUContext_t   *pduContext = &mainContext->PDUContext;	

        WMMP_LIB_DBG_TEMP("ResendCommand");
	
	EncodePDUHeader(pduContext);
	EncodeAbstract(pduContext);

	//send  from GPRS or SMS
	
	wmmpSendPDU(pduContext);
/*
	if((mainContext->StackState==WMMP_STACK_INIT)||(mainContext->StackState==WMMP_STACK_CLIENT_WAITING))
	{
		wmmpSendNextCommand();
	}*/
}
#endif
