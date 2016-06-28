#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "api_webpage.h"


#include "tonly_typedef.h"
#include "system_ipc.h"


static unsigned int web_status;

static int fg_thread_exit = TRUE;


int web_show_home_page(char*signal_strength, char* ap_name, char* ip_addr, 
char* wps_pin)
{
//    printf("Content-type:text/html\n\n");	
    printf("Content-type:text/html;charset=UTF-8\n\n");	

    printf("<html>\n");

    printf("<script type='text/javascript'>\n");
    printf("function alert_if_null(id) { \n");
    printf("var value=document.getElementById(id).value;\n");
    printf("if (!value) {\n");
    printf("alert(id + ' can not be null'); return false; }}\n");
    printf("</script>\n");

    printf("<head>\n");
    printf("<meta http-equiv='Refresh' content='60'>\n");
    printf("</head>\n");

    printf("<head><title>Airplay Audio Speaker</title></head>\n");
    printf("<img src='./cgi-bin/img/topbar.jpg' alt='tonly wireless speaker' />\n");
//    printf("<body><h1>TCL Airplay Audio Speaker</h1>");

    printf("<br />\n");

#if USE_BACKGROND_COLOR
    printf("<body bgcolor=rgb(%d,%d,%d)></body>\n", BG_COLOR_R, BG_COLOR_G, BG_COLOR_B);
#else if(USE_BACKGROND_IMAGE)
    printf("<body style='background-image:url(%s)'></body>\n",HOME_PAGE_BACKGROUND_IMAGE_PATH);
#endif

//    printf("<body><h3>Airplay Current Status:</h3>");
//    printf("<p><b>Airplay Current Status:</b></p>");

    printf("<table>\n");
    printf("<tr><td><b>Airplay Current Status:</b></td></tr>\n");
//    printf("<tr><td>Signal Strength:  %s</td></tr>\n",signal_strength);
    printf("<tr><td>AP Name:  %s</td></tr>\n",ap_name);
    printf("<tr><td>IP Address:  %s</td></tr>\n",ip_addr);
//    printf("<tr><td>WPS Pin:  %s</td></tr>\n",wps_pin);
    printf("</table>\n");


    printf("<br />\n");


    printf("<form action='/cgi-bin/web_change_device_name.cgi' method='get' onsubmit='return alert_if_null(\"device_name\")'>\n");
    printf("Device name: <input type='text' id='device_name' name='device_name' size=32 maxlength=32 />\n");
    printf("<input type='submit' value='rename' />\n");
    printf("</form>\n");

    printf("<br />\n");


    printf("<form action='/cgi-bin/ip.cgi'>\n");
    printf("<input type='radio' name='ip_setting' value='automatically' checked />Obtain an IP address automatically<br />\n");
    printf("<input type='radio' name='ip_setting' value='manually' />Use the following IP address<br />\n");
    printf("<table>\n");
    printf("<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;Ip address:</td><td><input type='text' name='ip_address' /></td></tr>\n");
    printf("<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;Subnet mask:</td><td><input type='text' name='subnet_mask' /></td></tr>\n");

	#if OPEN_WEB_DNS_SETTING
    printf("<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;Gateway:</td><td><input type='text' name='gateway' /></td></tr>\n");
    printf("<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;Dns1:</td><td><input type='text' name='dns_server1' /></td></tr>\n");
    printf("<tr><td>&nbsp;&nbsp;&nbsp;&nbsp;Dns2:</td><td><input type='text' name='dns_server2' /></td></tr>\n");
	#endif
	
    printf("<tr><td><input type='submit' value='Apply' /></td></tr>\n");
    printf("</table>\n");
    printf("</form>\n");

    printf("<br />\n");


    printf("<form action='/cgi-bin/wifi.cgi'>\n");
    printf("<input type='radio' name='wifi_setting' value='scan' checked />Scan wifi APs<br />\n");
    printf("<input type='radio' name='wifi_setting' value='manually' />Connect wifi<br />\n");
    printf("&nbsp;&nbsp;&nbsp;&nbsp;SSID:<input type='text' name='ssid' /><br/>\n");
    printf("&nbsp;&nbsp;&nbsp;&nbsp;Secure mode: <select name='secure_mode' size=1>\n");
    printf("<option selected>NONE\n");
    printf("<option>WEP\n");
    printf("<option>WPA\n");
    printf("<option>WPA2\n");
    printf("<option>WPA/WPA2\n");

    printf("</select><br />\n");
    printf("&nbsp;&nbsp;&nbsp;&nbsp;Password: <input type='password' name='password' /><br />\n");
    printf("<input type='submit' value='Apply' />\n");
    printf("</form>\n");

    printf("<br />\n");


    printf("<a href='cgi-bin/upgrade_firmware_select.cgi'>Firmware upgrade</a>\n");

    printf("</body></html>\n");

    fflush(stdout);

    return 0;
}


/*************************************************
 * Function:  ipc_web_client_get_current_status
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
void ipc_web_client_get_current_status(vold)
{
    char Param_Send[10] = {0};
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);

    sipc_sendCmd(web_status,WEB_GET_NETWORK_STATUS, Param_Send, sizeof(Param_Send));
}




static void web_clientCurrentStatusRecv(int fd,char *param,int size)
{
    char signal_strength[10];
    char wps_pin[10];
    
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
    
    if(NULL == param)
    {
        fprintf(stderr,"The para is error!\r\n");
        return;
    }

    WEB_NETWORK_STT networkStatusInfo;

    networkStatusInfo = *((WEB_NETWORK_STT *)param);

    fprintf(stderr,"[%s] networkStatusInfo.cnt_type is %d\n",__FUNCTION__,networkStatusInfo.cnt_type);
    fprintf(stderr,"[%s] networkStatusInfo.signal_strength is %d\n",__FUNCTION__,networkStatusInfo.signal_strength);
    fprintf(stderr,"[%s] networkStatusInfo.ssid is {%s}\n",__FUNCTION__,networkStatusInfo.ssid);
    fprintf(stderr,"[%s] networkStatusInfo.ip_addr is {%s}\n",__FUNCTION__,networkStatusInfo.ip_addr);
    fprintf(stderr,"[%s] networkStatusInfo.wps_pin is %d\n",__FUNCTION__,networkStatusInfo.wps_pin);
    
    sprintf(signal_strength, "%d dBm",networkStatusInfo.signal_strength);
    sprintf(wps_pin, "%d",networkStatusInfo.wps_pin);

    web_show_home_page(signal_strength, networkStatusInfo.ssid, networkStatusInfo.ip_addr, wps_pin);

    fg_thread_exit = FALSE;
}




/*************************************************
 * Function:  web_sipc_status_msg_list
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
static _sipc_msg_process_fun web_sipc_status_msg_list[]=
{
    {WEB_NETWORK_STATUS_RESPONSE, web_clientCurrentStatusRecv},
    {NULL,NULL},
};





/*************************************************
 * Function:  tonlyWeb_wifi_client_init
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int tonlyWeb_status_client_init()
{
	fprintf(stderr,"[%s]line is %d\n",__FUNCTION__,__LINE__);
	if(tonly_module_register((CHAR*)APP_STATUS_CLEINT_NAME,(CHAR*)APP_WEB_IPC)!=0)
	{
		return -1;
	}
	if(tonly_callback_register((CHAR*)APP_STATUS_CLEINT_NAME,web_sipc_status_msg_list,NULL)!=0)
	{
		return -1;
	}
	web_status = tonly_module_handle_read(APP_STATUS_CLEINT_NAME);
	return 0;
}


/*************************************************
 * Function:  web_ipc_gcs_client_init
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int web_ipc_gcs_client_init(void)
{
	fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
	s_comm_set_ThreadName((char*)APP_GCS_NAME);
	s_comm_save_pid_to_file((char*)APP_GCS_NAME,getpid());
    
	tonlyWeb_status_client_init();
}


int main(int argc, char* argv[])
{
	int counter = 20;
    web_ipc_gcs_client_init();

//    usleep(100*1000);
    ipc_web_client_get_current_status();

    while(fg_thread_exit&&counter)
    {
    	counter--;
		
        usleep(50*1000);
        fprintf(stderr,"web_get_current_status waiting!!\n");
    }

//    usleep(20*1000);
    fprintf(stderr,"web_get_current_status exit!!\n");

#if FIX_HREATH_CANNOT_EXIT_ISSUE
    kill(getpid(),SIGHUP);
#endif

    return 0;
}
