/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                               *
 *---------------------------------------------------------------------
 * FileName      :   simcom_at_parser.h
 * version       :   0.10
 * Description   :   AT command parser api
 * Authors       :   Maobin  
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2013-01-11, maobin, create originally.
 *
 ********************************************************************/
#ifndef __SIMCOM_AT_PARSER_H_
#define __SIMCOM_AT_PARSER_H_
/********************************************************************
 * Include Files
 ********************************************************************/
#include "eat_type.h"
#include "eat_at.h"
/********************************************************************
 * Macros
 ********************************************************************/
typedef EAT_AT_PARSER_RET SIMCOM_AT_PARSER_RET ;


#define simcom_at_output_set_result         eat_at_output_set_result
#define simcom_at_output                    eat_at_output
#define simcom_at_output_async              eat_at_output_async
#define simcom_at_output_cr_lf              eat_at_output_cr_lf
#define simcom_at_output_default_end        eat_at_output_default_end
#define simcom_at_output_get_cr             eat_at_output_get_cr
#define simcom_at_output_get_lf             eat_at_output_get_lf
#define simcom_at_output_increase_buffer    eat_at_output_increase_buffer
#define simcom_at_parser_get_s32            eat_at_parser_get_s32
#define simcom_at_parser_get_u32            eat_at_parser_get_u32
#define simcom_at_parser_get_str            eat_at_parser_get_str
#define simcom_at_parser_get_str_between_dq eat_at_parser_get_str_between_dq
#define simcom_at_parser_is_end             eat_at_parser_is_end
#define simcom_at_parser_get_src_id         eat_at_parser_get_src_id

#define simcom_customer_at_reg              eat_customer_at_reg

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

#endif // __SIMCOM_AT_PARSER_H_
