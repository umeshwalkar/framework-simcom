/********************************************************************
 *                Copyright Simcom(shanghai)co. Ltd.                   *
 *---------------------------------------------------------------------
 * FileName     :   app_demo_gps.c
 * version       :   0.10
 * Description  :   
 * Authors       :   heweijiang
 * Notes         :
 *---------------------------------------------------------------------
 *
 *    HISTORY OF CHANGES
 *---------------------------------------------------------------------
 *0.10  2014-07-31, heweijiang, create originally.
 *
 *--------------------------------------------------------------------
 * File Description
 * AT+CEAT=parma1,param2
 * param1 param2 
 *   1      1    gps power on
 *   1      2    gps power off
 *   1      3    query gps power status
 *   2      1    query gps fix status
 *   2      2    gps entry sleep mode
 *   2      3    gps exit sleep mode
 *   2      4    query gps sleep status
 *--------------------------------------------------------------------
 ********************************************************************/
/********************************************************************
 * Include Files
 ********************************************************************/
#include <stdio.h>
#include <string.h>
#include "eat_modem.h"
#include "eat_interface.h"
#include "eat_periphery.h"
#include "eat_uart.h"
#include "eat_gps.h"

#include "eat_clib_define.h" //only in main.c
/********************************************************************
 * Macros
 ********************************************************************/
#define NMEA_BUFF_SIZE 1024
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
static char gps_info_buf[NMEA_BUFF_SIZE]="";

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
#pragma arm section rodata = "APP_CFG"
APP_ENTRY_FLAG 
#pragma arm section rodata

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
		 eat_uart_set_at_port: set AT port
	*/
    eat_uart_set_debug(EAT_UART_USB);
    eat_uart_set_at_port(EAT_UART_1);// UART1 is as AT PORT     
}
eat_bool eat_modem_data_parse(u8* buffer, u16 len, u8* param1, u8* param2)
{
    eat_bool ret_val = EAT_FALSE;
    u8* buf_ptr = NULL;
    /*param:%d,extern_param:%d*/
     buf_ptr = (u8*)strstr((const char *)buffer,"param");
    if( buf_ptr != NULL)
    {
        sscanf((const char *)buf_ptr, "param:%d,extern_param:%d",(int*)param1, (int*)param2);
        eat_trace("data parse param1:%d param2:%d",*param1, *param2);
        ret_val = EAT_TRUE;
    }
    return ret_val;
}

eat_bool eat_module_test_gps(u8 param1, u8 param2)
{
    //The unit of latitude and latitude is degree
    double lat0 = 31.1334;
    double lng0 = 121.2126;
    double radius = 1000;
    double lat1 = 31.2657;
    double lng1 = 121.3026;    
    double lat2 = 31.0000;
    double lng2 = 121.0000;    
    double lat3 = 32.0000;
    double lng3 = 121.0000;

	memset(gps_info_buf,NULL,1024);

    eat_trace("gps test param1=%d,param2=%d",param1,param2);
    if( 1 == param1 )
    {
        if( 1 == param2 )
        {
            //power on
            eat_gps_power_req(EAT_TRUE);
        }else if( 2 == param2 )
        {  
            //power off
            eat_gps_power_req(EAT_FALSE);
        }else if ( 3 == param2 )
        {
            //power status
            eat_trace("gps power status %d", eat_gps_power_status());
        }
    }    
    else if ( 2 == param1)
    {        
        if( 1 == param2 )
        {
            //fix status
            eat_trace("gps current fix status %d", eat_gps_status_get());            
        }else if( 2 == param2 )
        {  
            //enter sleep
            eat_gps_sleep_set(EAT_TRUE);
        }else if ( 3 == param2 )
        {
            //wakeup
            eat_gps_sleep_set(EAT_FALSE);
        }else if ( 4 == param2 )
        {        
            //sleep status
            eat_trace("gps current sleep mode %d", eat_gps_sleep_read());
        }
    }
    else if ( 3 == param1)
    {        
        if( 1 == param2 )
        {
            //cold start
            eat_gps_start_mode_set(EAT_GPS_RST_COLD);
        }else if( 2 == param2 )
        {  
            //hot start
            eat_gps_start_mode_set(EAT_GPS_RST_HOT);
        }else if ( 3 == param2 )
        {
            //warm start
            eat_gps_start_mode_set(EAT_GPS_RST_WARM);
        }else if ( 4 == param2)
        {
            //start mode
            eat_trace("gps start mode=%d", eat_gps_start_mode_get());
        }
    }
    else if ( 4 == param1)
    {
        if( 1 == param2 )
        {
            //GPSIM simcom format
            eat_gps_nmea_info_output(EAT_NMEA_OUTPUT_SIMCOM, gps_info_buf,NMEA_BUFF_SIZE);
        }else if( 2 == param2 )
        {  
            //GPGGA
            eat_gps_nmea_info_output(EAT_NMEA_OUTPUT_GPGGA, gps_info_buf,NMEA_BUFF_SIZE);
        }else if ( 3 == param2 )
        {
            //GPGLL
            eat_gps_nmea_info_output(EAT_NMEA_OUTPUT_GPGLL, gps_info_buf,NMEA_BUFF_SIZE);
        }else if ( 4 == param2)
        {
            //GPGSA
            eat_gps_nmea_info_output(EAT_NMEA_OUTPUT_GPGSA, gps_info_buf,NMEA_BUFF_SIZE);
        }else if ( 5 == param2)
        {   
            //GPGSV
            eat_gps_nmea_info_output(EAT_NMEA_OUTPUT_GPGSV, gps_info_buf,NMEA_BUFF_SIZE);
        }else if ( 6 == param2)
        {   
            //GPRMC
            eat_gps_nmea_info_output(EAT_NMEA_OUTPUT_GPRMC, gps_info_buf,NMEA_BUFF_SIZE);
        }else if ( 7 == param2)
        {   
            //GPVTG
            eat_gps_nmea_info_output(EAT_NMEA_OUTPUT_GPVTG, gps_info_buf,NMEA_BUFF_SIZE);
        }else if ( 8 == param2)
        {
            //GPZDA
            eat_gps_nmea_info_output(EAT_NMEA_OUTPUT_GPZDA, gps_info_buf,NMEA_BUFF_SIZE);
        }
        
        eat_trace("gps NMEA info=%s", gps_info_buf);
    }
    else if ( 5 == param1)
    {
        //NOTE:The unit of latitude and latitude is degree
        if (1 == param2)
        {
            eat_gps_geo_fence_set(0,EAT_GEO_FENCE_ENTER,EAT_GEO_FENCE_CIRCLE_WITH_RADIUS,lat0,lng0,radius,0,0,0,0,0);
        }
        else if (2 == param2)
        {
            eat_gps_geo_fence_set(1,EAT_GEO_FENCE_ENTER,EAT_GEO_FENCE_CIRCLE_WITH_POINT,lat0,lng0,lat1,lng1,0,0,0,0);
        }
        else if (3 == param2)
        {
            eat_gps_geo_fence_set(2,EAT_GEO_FENCE_LEAVE,EAT_GEO_FENCE_TRIANGLE,lat0,lng0,lat1,lng1,lat2,lng2,0,0);
        }
        else if (4 == param2)
        {
            eat_gps_geo_fence_set(3,EAT_GEO_FENCE_LEAVE,EAT_GEO_FENCE_QUADRANGLE,lat0,lng0,lat1,lng1,lat2,lng2,lat3,lng3);
        }        
        else if (5 == param2)
        {
            eat_gps_geo_fence_read(0,gps_info_buf,NMEA_BUFF_SIZE);            
            eat_trace("gps GEO-fence info=%s", gps_info_buf);
        }
    }
    
    return EAT_TRUE;
}

/*
+UGEOIND: <geo id 0>,<mode>,<shape>,<status>,
            <GNSS run status>,<Fix status>,
            <UTC date&Time>,<Latitude>,<Longitude>,
            <MSL Altitude>,<Speed Over Ground>,
            <Course Over Ground>,<Fix Mode>,<Reserved1>,
            <HDOP>,<PDOP>,<VDOP>,<Reserved2>,
            <GPS Satellites in View>,<GPS Satellites Used>,
            <GLONASS Satellites Used>
*/
void geo_fence_proc_cb(char *buf, u8 len)
{
    eat_trace("geo_fence_proc_cb() %s",buf);
}

void eat_gps_init()
{
     eat_trace("#### eat_gps_init");
    eat_gps_register_msg_proc_callback(geo_fence_proc_cb);
}

void app_main(void *data)
{
    EatEvent_st event;
    u8 buf[2048];
    u16 len = 0;

    APP_InitRegions();//Init app RAM
    APP_init_clib();

    eat_gps_init();

    eat_trace("app_main ENTRY");
    
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
                            //Entry gps test module
                            eat_module_test_gps(param1, param2);
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


