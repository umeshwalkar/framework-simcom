/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName      :   app_demo_spi.c
 * version       :   0.10
 * Description   :   
 * Authors       :   fangshengchang
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2012-09-24, fangshengchang, create originally.
 *
 *--------------------------------------------------------------------
 * File Description
 * AT+CEAT=parma1,param2
 * param1 param2 
 *   1      1     lcd display some word and Backlight on
 *   1      2     Backlight off
 *--------------------------------------------------------------------
 ********************************************************************/
/********************************************************************
 * Include Files
 ********************************************************************/
#include <stdio.h>
#include <string.h>
#include "eat_modem.h"
#include "LcdDisplay.h"
#include "eat_interface.h"
#include "eat_uart.h"
/********************************************************************
 * Macros
 ********************************************************************/

/********************************************************************
* Types
 ********************************************************************/
typedef void (*app_user_func)(void*);
/********************************************************************
 * Extern Variables (Extern /Global)
 ********************************************************************/
 
/********************************************************************
 * Local Variables:  STATIC
 ********************************************************************/

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

void app_func_ext1(void *data)
{
	/*This function can be called before Task running ,configure the GPIO,uart and etc.
	   Only these api can be used:
		 eat_uart_set_debug: set debug port
		 eat_pin_set_mode: set GPIO mode
		 eat_uart_set_at: set AT port
	*/
	eat_uart_set_debug(EAT_UART_USB);
    eat_uart_set_at_port(EAT_UART_1);// UART1 is as AT PORT

#if 0
	eat_pin_set_mode(EAT_PIN29_KBC5, EAT_PIN_MODE_GPIO);
	eat_pin_set_mode(EAT_PIN30_KBC4, EAT_PIN_MODE_KEY);
	eat_pin_set_mode(EAT_PIN31_KBC3, EAT_PIN_MODE_KEY);
	eat_pin_set_mode(EAT_PIN32_KBC2, EAT_PIN_MODE_KEY);
	eat_pin_set_mode(EAT_PIN33_KBC1, EAT_PIN_MODE_KEY);
	eat_pin_set_mode(EAT_PIN34_KBC0, EAT_PIN_MODE_KEY);
	eat_pin_set_mode(EAT_PIN35_KBR5, EAT_PIN_MODE_KEY);
	eat_pin_set_mode(EAT_PIN36_KBR4, EAT_PIN_MODE_KEY);
	eat_pin_set_mode(EAT_PIN37_KBR3, EAT_PIN_MODE_KEY);
	eat_pin_set_mode(EAT_PIN38_KBR2, EAT_PIN_MODE_KEY);
	eat_pin_set_mode(EAT_PIN39_KBR1, EAT_PIN_MODE_KEY);
	eat_pin_set_mode(EAT_PIN40_KBR0, EAT_PIN_MODE_KEY);
#endif
}

eat_bool eat_modem_data_parse(u8* buffer, u16 len, u8* param1, u8* param2)
{
    eat_bool ret_val = EAT_FALSE;
    /*param:%d,extern_param:%d*/
    if( strstr((const char *)buffer,"param"))
    {
        sscanf((const char *)buffer, "param:%d,extern_param:%d",(u32*)param1, (u32*)param2);
        eat_trace("data parse param1:%d param2:%d",*param1, *param2);
        ret_val = EAT_TRUE;
    }
    return ret_val;
}

//lcd test
static void lcd_test(eat_bool display)
{    
    if(display)
    {
        eat_trace("LCD test Display on");
        LcdDisplayClear(0, 0, X_PIXELS-1, Y_PIXELS-1, EAT_TRUE);
        eat_sleep(100);
        LcdDisplayStr(0,  0, 127, 15, FONT_TYPE_MAIN, LCD_ALIGN_V_UP | LCD_ALIGN_H_LEFT, "lcd test", EAT_TRUE);
        LcdDisplayStr(0, 16, 127, 31, FONT_TYPE_MAIN, LCD_ALIGN_V_DOWN | LCD_ALIGN_H_RIGHT, "backlight test", EAT_TRUE);
        LcdDisplayStr(0, 32, 127, 47, FONT_TYPE_MAIN, 0, "ABCDEFGH", EAT_TRUE);
        LcdDisplayStr(0, 48, 127, 63, FONT_TYPE_MAIN, 0, "1234567890", EAT_TRUE);  
    }else
    {
        LcdDisplayClear(0, 0, X_PIXELS-1, Y_PIXELS-1, EAT_TRUE);
    }
}

//LCDLED test
void backlight_test(int sw)
{
   if(sw)
   {
      eat_lcd_light_sw(EAT_TRUE,EAT_BL_STEP_20_MA);
   }
   else
   {
      eat_lcd_light_sw(EAT_FALSE,0);
   }    
}

/****************************************************
 * Timer testing module
 *****************************************************/
eat_bool eat_module_test_spi(u8 param1, u8 param2)
{
    /***************************************
     * example 1
     * used eat_gpio_write test EAT_PIN53_PCM_IN,EAT_PIN57_GPIO4
     ***************************************/
    if( 1 == param1 )
    {
        if( 1 == param2 )
        {          
           lcd_test(EAT_TRUE);
           backlight_test(EAT_TRUE);
           eat_trace("LCD test 1,1 start");
        }else if( 2 == param2)
        {                   
           lcd_test(EAT_FALSE);
           backlight_test(EAT_FALSE);
           eat_trace("LCD test 1,2 backlight off");
        }
    }  
    return EAT_TRUE;
}


void app_main(void *data)
{
    EatEvent_st event;
    u8 buf[2048];
    u16 len = 0;

    APP_InitRegions();//Init app RAM
    lcd_init();
    eat_trace(" app_main ENTRY");
    while(EAT_TRUE)
    {
        eat_get_event(&event);
        eat_trace("MSG id%x", event.event);
        switch(event.event)
        {
            case EAT_EVENT_MDM_READY_RD:
                {
                    u8 param1,param2;
                    len = 0;
                    len = eat_modem_read(buf, 2048);
                    if(len > 0)
                    {
                        //Get the testing parameter
                        if(eat_modem_data_parse(buf,len,&param1,&param2))
                        {
                            //Entry SPI test module
                            eat_module_test_spi(param1, param2);
                        }
                        else
                        {
                            eat_trace("From Mdm:%s",buf);
                        }
                    }

                }
                break;
            case EAT_EVENT_MDM_READY_WR:
            case EAT_EVENT_UART_READY_RD:
                break;
            case EAT_EVENT_UART_SEND_COMPLETE :
                break;
            default:
                break;
        }

    }

}


