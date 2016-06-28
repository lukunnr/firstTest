/*************************************************
* Copyright (C), 2014-2018, TCL AV STB R&D CENTER
* File name:     webMain.c
* Author: sunwei    Version: 01    Date: 2015-04-24
* e-mail: hxw@tcl.com
* Description:   
* Others:         
* History:         
* 1. Date:     Author:
* Modification:
*************************************************/

/*****************************************************/
/******************   head file   **********************/
/*****************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <fcntl.h>
#include <assert.h>
#include <stdint.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
//#include "tcl_debug.h"
#include "system_ipc.h"
#include "system_common.h"
#include "tonly_typedef.h"
#include "system_timer.h"
#include "api_webpage.h"


static WEB_NETWORK_STT network_stt={0};
static char ipc_device_name[NAME_LEN_MAX+1]={0};
static WEB_NETWORK_IP_CONFIG ip_config_info={0};
static WEB_AP_INFO ap_info[AP_NUM_MAX]={0};
static WEB_WIFI_SSID_PW ssid_pw={0};
static int upg_result = 0;

#define APP_NAME           		"web_server"

int ipc_web_server_get_network_status(int fd,char *param,int size);
int ipc_web_server_set_device_name(int fd,char *param,int size);
int ipc_web_server_set_network(int fd,char *param,int size );
int ipc_web_server_get_wifi_list(int fd,char *param,int size);
int ipc_web_server_connect_ap(int fd,char *param,int size );
int ipc_web_server_start_upg(int fd,char *param,int size );
int ipc_web_server_get_upg_result(int fd,char *param,int size );





/*************************************************
 * Function:  sipc_msg_process_fun
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
static _sipc_msg_process_fun sipc_msg_process_fun[]=
{
    {WEB_GET_NETWORK_STATUS, ipc_web_server_get_network_status},
    {WEB_SET_DEVICE_NAME, ipc_web_server_set_device_name},
    {WEB_SET_NETWORK, ipc_web_server_set_network},
    {WEB_GET_WIFI_LIST, ipc_web_server_get_wifi_list},
    {WEB_CONNECT_WIFI, ipc_web_server_connect_ap},   
    {WEB_FIRMWARE_UPGRADE, ipc_web_server_start_upg},
    {WEB_GET_UPG_RESULT, ipc_web_server_get_upg_result},    
    {NULL}
};




/*************************************************
 * Function:  a_web_ipc_server_thread
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int web_ipc_server_thread(void)
{
	fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
	s_comm_set_ThreadName((char*)APP_NAME);
	s_comm_save_pid_to_file((char*)APP_NAME,getpid());
	sipc_serverBindSocket(APP_WEB_IPC,sipc_msg_process_fun);

}


/*************************************************
 * Function:  a_web_ipc_server_create
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int web_ipc_server_create(void)
{

	fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);

	pthread_attr_t thread_attr;

	pthread_t web_sipc;	

	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if(-1==pthread_create(&web_sipc, &thread_attr, web_ipc_server_thread, NULL))
	{
		fprintf(stderr, "Create  web_sipc failed !\n");
		pthread_attr_destroy(&thread_attr);
		return -1;
	}
	pthread_attr_destroy(&thread_attr);

}



/*************************************************
 * Function:  ipc_web_server_get_network_status
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int ipc_web_server_get_network_status(int fd,char *param,int size)
{
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
    if(NULL == param)
    {
        fprintf(stderr,"The para is error!\r\n");

        return 0;
    }

    tonlyweb_callback_list->web_get_network_status(&network_stt);

    fprintf(stderr,"[%s] network_stt.cnt_type is %d\n",__FUNCTION__,network_stt.cnt_type);
    fprintf(stderr,"[%s] network_stt.cnt_type is %d\n",__FUNCTION__,network_stt.signal_strength);
    fprintf(stderr,"[%s] network_stt.ssid is {%s}\n",__FUNCTION__,network_stt.ssid);
    fprintf(stderr,"[%s] network_stt.ip_addr is {%s}\n",__FUNCTION__,network_stt.ip_addr);
    fprintf(stderr,"[%s] network_stt.wps_pin is %d\n",__FUNCTION__,network_stt.wps_pin);

    char* Param_Send;
    Param_Send = (char*)&network_stt;
    sipc_sendCmd(fd,WEB_NETWORK_STATUS_RESPONSE, Param_Send, sizeof(network_stt));

    return 0;
}



/*************************************************
 * Function:  ipc_web_set_device_name
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int ipc_web_server_set_device_name(int fd,char *param,int size)
{
	fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
	if(NULL == param)
	{
		fprintf(stderr,"The para is error!\r\n");

		return 0;
	}
	memcpy(ipc_device_name,param,size);


	tonlyweb_callback_list->web_set_device_name(ipc_device_name);

	return 0;
}





/*************************************************
 * Function:  ipc_web_set_network
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int ipc_web_server_set_network(int fd,char *param,int size )
{
	fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);

	if(NULL == param)
	{
		fprintf(stderr,"The para is error!\r\n");

		return 0;
	}
	memcpy(&ip_config_info, (WEB_NETWORK_IP_CONFIG*)param,size);
	
	tonlyweb_callback_list->web_set_network(ip_config_info);
	
	return 0;
}




/*************************************************
 * Function:  ipc_web_server_get_wifi_list
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int ipc_web_server_get_wifi_list(int fd,char *param,int size)
{
	fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
	if(NULL == param)
	{
		fprintf(stderr,"The para is error!\r\n");

		return 0;
	}

    memset(ap_info, 0, sizeof(ap_info));
    tonlyweb_callback_list->web_get_wifi_list(ap_info);

    int counter = 0;

    for(counter=0; counter < AP_NUM_MAX; counter++)
    {
        fprintf(stderr,"[%s] ssid is {%s}\n",__FUNCTION__,ap_info[counter].ssid);
        fprintf(stderr,"[%s] signal_strength is %d\n",__FUNCTION__,ap_info[counter].signal_strength);
        fprintf(stderr,"[%s] secure_mode is %d\n",__FUNCTION__,ap_info[counter].secure_mode);
        fprintf(stderr,"[%s] security is %s\n",__FUNCTION__,ap_info[counter].security);
    }

    char* Param_Send;
    Param_Send = (char*)ap_info;
    sipc_sendCmd(fd,WEB_WIFI_LIST_RESPONSE, Param_Send, sizeof(WEB_AP_INFO)*AP_NUM_MAX);

    return 0;
}


/*************************************************
 * Function:  ipc_web_server_connect_ap
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int ipc_web_server_connect_ap(int fd,char *param,int size )
{
	fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);

	if(NULL == param)
	{
		fprintf(stderr,"The para is error!\r\n");

		return 0;
	}
	memcpy(&ssid_pw, (WEB_WIFI_SSID_PW*)param,size);
	
	tonlyweb_callback_list->web_connect_ap(ssid_pw);
	
	return 0;
}


/*************************************************
 * Function:  ipc_web_server_start_upg
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int ipc_web_server_start_upg(int fd,char *param,int size )
{
	fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);

	if(NULL == param)
	{
		fprintf(stderr,"The para is error!\r\n");

		return 0;
	}
//	memcpy(&ssid_pw,param,size);
	
	tonlyweb_callback_list->web_firmware_upg();

	return 0;
}


/*************************************************
 * Function:  ipc_web_server_get_upg_result
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int ipc_web_server_get_upg_result(int fd,char *param,int size)
{
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
    if(NULL == param)
    {
        fprintf(stderr,"The para is error!\r\n");

        return 0;
    }

//    tonlyweb_callback_list->web_get_upg_result(&upg_result);

    fprintf(stderr,"[%s] upg_result is %d\n",__FUNCTION__,upg_result);

    char* Param_Send;
    WEB_SET_CMD_STT status;
     
    if(100 == upg_result)
    {  
        status = WEB_SET_CMD_SUCCESS;
         Param_Send = (char*)&status;
   //     sipc_sendCmd(fd,WEB_NETWORK_STATUS_RESPONSE, Param_Send, sizeof(status));
        sipc_serverSendAll(APP_WEB_IPC,WEB_UPG_RESULT_RESPONSE,Param_Send,sizeof(status));
    }
    else if(200 == upg_result)
    {     
        status = WEB_SET_CMD_FAIL;
         Param_Send = (char*)&status;
//        sipc_sendCmd(fd,WEB_NETWORK_STATUS_RESPONSE, Param_Send, sizeof(status));
        sipc_serverSendAll(APP_WEB_IPC,WEB_UPG_RESULT_RESPONSE,Param_Send,sizeof(status));
    }

    return 0;
}



/*************************************************
 * Function: tonlyweb_server_init 
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int tonlyweb_server_init(TONLYWEB_CALLBACK_T *callback)
{
	fprintf(stderr,"web server run[%s]\n",__FUNCTION__);
	
	tonlyweb_callback_list = calloc(1,sizeof(TONLYWEB_CALLBACK_T));
	if(NULL == tonlyweb_callback_list)
	{
		fprintf(stderr,"Can not malloc more memory!\r\n");
		return -1;
	}
	memcpy(tonlyweb_callback_list,callback,sizeof(TONLYWEB_CALLBACK_T));	

    /*===1====create web  ipc  server  communication==============*/
	web_ipc_server_create();
	fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
    
	return 0;
}



/*************************************************
 * Function: tonlyweb_server_response_status 
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int tonlyweb_server_response_status(WEB_SET_CMD cmd, WEB_SET_CMD_STT status)
{
    char* Param_Send;
    
    fprintf(stderr,"web server run[%s], cmd: %d, para: %d\n",__FUNCTION__,cmd ,status);

    Param_Send = (char*)&status;

    switch(cmd)
    {
        case WEB_CMD_SET_DEVICE_NAME:
            sipc_serverSendAll(APP_WEB_IPC,WEB_SET_DEVICE_NAME_RESPONSE,Param_Send,sizeof(status));
            break;
            
         case WEB_CMD_SET_IP_AUTO:
            sipc_serverSendAll(APP_WEB_IPC,WEB_SET_IP_AUTO_RESPONSE,Param_Send,sizeof(status));
            break;
            
        case WEB_CMD_SET_IP_MANUAL:
            sipc_serverSendAll(APP_WEB_IPC,WEB_SET_IP_MANUAL_RESPONSE,Param_Send,sizeof(status));
            break;
            
        case WEB_CMD_CONNECT_WIFI:
            sipc_serverSendAll(APP_WEB_IPC,WEB_CONNECT_WIFI_BY_CUSTOM_RESPONSE,Param_Send,sizeof(status));
            sipc_serverSendAll(APP_WEB_IPC,WEB_CONNECT_WIFI_BY_SCAN_RESPONSE,Param_Send,sizeof(status));
            break;

        case WEB_CMD_START_UPGRADE:
            sipc_serverSendAll(APP_WEB_IPC,WEB_UPG_STATUS_RESPONSE,Param_Send,sizeof(status));
            break;
            
        case WEB_CMD_UPGRADE:
            if(WEB_SET_CMD_SUCCESS == status)
            {
                upg_result = 100;
             //   sipc_serverSendAll(APP_WEB_IPC,WEB_UPG_RESULT_RESPONSE,Param_Send,sizeof(status));
            }
            else
            {
                upg_result = 200;
             //   sipc_serverSendAll(APP_WEB_IPC,WEB_UPG_RESULT_RESPONSE,Param_Send,sizeof(status));
            }
            break;

        default:
            break;
    }


    return 0;
}





