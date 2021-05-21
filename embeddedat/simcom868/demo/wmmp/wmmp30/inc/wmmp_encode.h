#if defined(__MODEM_WMMP30_SIMCOM__)
#ifndef _WMMP_ENCODE_H_
#define _WMMP_ENCODE_H_

WmmpEncodeSendResult_e SendCommand(CommandInfo_t *CommandInfo);
void ResendCommand(void);
#endif
#endif
