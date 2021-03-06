/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                               *
 *---------------------------------------------------------------------
 * FileName      :   wmmp_basic_type.h
 * version       :   0.10
 * Description   :   
 * Authors       :   Maobin  
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2013-01-11, maobin, create originally.
 *
 ********************************************************************/
#ifndef _WMMP_BASIC_TYPE_
#define  _WMMP_BASIC_TYPE_
/********************************************************************
 * Include Files
 ********************************************************************/

/********************************************************************
 * Macros
 ********************************************************************/

/********************************************************************
 * Types
 ********************************************************************/
#include "eat_type.h"


#ifndef NULL
#define NULL                       (void *)0
#endif 

#ifndef	PNULL
#define    PNULL    0           
#endif

#define bool eat_bool

#ifndef false
#define false EAT_FALSE
#endif

#ifndef true
#define true EAT_TRUE
#endif

#endif // _WMMP_BASIC_TYPE_
