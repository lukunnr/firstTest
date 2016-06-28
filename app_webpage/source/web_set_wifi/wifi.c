#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "api_webpage.h"


#include "tonly_typedef.h"
#include "system_ipc.h"

#define BUFFER_LEN  512

static unsigned int web_wifi;
static int fg_thread_exit = TRUE;

typedef enum
{
    WEB_WIFI_CONFIG_INPUT_NULL,
    WEB_WIFI_CONFIG_SSID_INPUT_NULL,
    WEB_WIFI_CONFIG_PASSWORD_INPUT_NULL,
    WEB_WIFI_CONFIG_FAIL,
    WEB_WIFI_CONFIG_SCAN_SUCCESS,
    WEB_WIFI_CONFIG_CUSTOM_SUCCESS,
}WEB_WIFI_CONFIG_STT;


/*************************************************
 * Function:  web_show_wifi_config_result_page
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int web_show_wifi_config_result_page(WEB_WIFI_CONFIG_STT status, WEB_AP_INFO* ap_info)
{
    char signal_strength[10];
    char secure_mode[10];
    int counter = 0;

//    printf("Content-type:text/html\n\n");
    printf("Content-type:text/html;charset=UTF-8\n\n");

    printf("<html>\n");
    
#if USE_BACKGROND_COLOR
    printf("<body bgcolor=rgb(%d,%d,%d)></body>\n", BG_COLOR_R, BG_COLOR_G, BG_COLOR_B);
#else if(USE_BACKGROND_IMAGE)
    printf("<body style='background-image:url(%s)'></body>\n",BACKGROUND_IMAGE_PATH);
#endif
  
    switch(status)
    {
        case WEB_WIFI_CONFIG_INPUT_NULL:        
#if OPEN_WEB_DEBUG_INFO
            printf("<h3>The input can not be empty.</h3>\n");
#endif
            break;

        case WEB_WIFI_CONFIG_SSID_INPUT_NULL:        
#if OPEN_WEB_DEBUG_INFO
                printf("<h3>The ssid can not be empty.</h3>\n");
#endif
                break;

        case WEB_WIFI_CONFIG_PASSWORD_INPUT_NULL:        
#if OPEN_WEB_DEBUG_INFO
                printf("<h3>The password can not be empty.</h3>\n");
#endif
                break;

        case WEB_WIFI_CONFIG_FAIL:
#if OPEN_WEB_DEBUG_INFO
            printf("<h3>The wifi connected failed.</h3>\n");
#endif            
            break;

        case WEB_WIFI_CONFIG_SCAN_SUCCESS:
            if(NULL == ap_info)
            {
                break;
            }
            
            printf("<br />\n");

            printf("<form action='wifi_connect.cgi'>\n");
            
      //      printf("<table width='500'>\n");
            printf("<table width='1000'>\n");
            printf("<tr><td><b>SSID</b></td><td><b>Signal</b></td><td><b>Secure mode</b></td></tr>\n");

            for(counter=0; counter < AP_NUM_MAX; counter++)
            {
                if(ap_info[counter].signal_strength == 0)
                {
                    fprintf(stderr,"[%s] network_stt list num[%d] is NULL!!\n",__FUNCTION__ ,counter);
                    continue;
                }
                
                memset(signal_strength, 0, sizeof(signal_strength));
                sprintf(signal_strength, "%d",ap_info[counter].signal_strength);

                memset(secure_mode, 0, sizeof(secure_mode));
                switch(ap_info[counter].secure_mode)
                {
                    case  WEB_SECURE_NONE:
                        sprintf(secure_mode, "none");
                        break;
                        
                    case  WEB_SECURE_WEP:
                        sprintf(secure_mode, "wep");
                        break;
                        
                    case  WEB_SECURE_WPA:
                        sprintf(secure_mode, "wpa");
                        break;
                        
                    case  WEB_SECURE_WPA2:
                        sprintf(secure_mode, "wpa2");
                        break;
                        
                    case  WEB_SECURE_WPA_WPA2:
                        sprintf(secure_mode, "wpa/wpa2");
                        break;
                        
                    default:
                        break;
                }
                
                printf("<tr><td><input type='radio' name='ssid' value=%s>%s</td><td>%s dB</td><td>%s</td></tr>\n",ap_info[counter].ssid,ap_info[counter].ssid,signal_strength,ap_info[counter].security);
            }

            printf("</table>\n");
            
            printf("Please input password:<input type='password' name='password' />\n");
            printf("<input type='submit' value='Connect' />\n");
            printf("</form>\n");
            break;

        case WEB_WIFI_CONFIG_CUSTOM_SUCCESS:
#if OPEN_WEB_DEBUG_INFO
            printf("<meta http-equiv='Refresh' content='1; url=./wifi_connect_waitting.cgi'>\n");
//            printf("<h3>The wifi connected successful.</h3>\n");
            printf("<h3>Connecting to the network.Please wait...</h3>\n");
//            printf("<br />\n");
            printf("<h3>By the way, please change your network and refresh web page manually after the network connected successfully.</h3>\n");
#endif
            break;

        default:
            break;
    }

    printf("<br />\n");
    printf("<form action='/index.cgi'>\n");
    printf("<input type='submit' value='Back to main' />\n");
    printf("</form>\n");
    printf("</html>\n");
    
    fflush(stdout);
    
    return 0;
}


/*************************************************
 * Function:  ipc_cb_wac_get_wifi_status
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
void ipc_web_client_set_wifi(WEB_WIFI_SSID_PW* ssid_pw)
{
    char* Param_Send;
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);

    Param_Send = (char*)ssid_pw;
    sipc_sendCmd(web_wifi,WEB_CONNECT_WIFI, Param_Send, sizeof(WEB_WIFI_SSID_PW));
}



/*************************************************
 * Function:  ipc_cb_wac_get_wifi_status
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
void ipc_web_client_get_wifi_list(vold)
{
    char Param_Send[10] = {0};
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);

    sipc_sendCmd(web_wifi,WEB_GET_WIFI_LIST, Param_Send, sizeof(Param_Send));
}



static void web_clientWifiListRecv(int fd,char *param,int size)
{
    int counter = 0;
    WEB_AP_INFO ap_info[AP_NUM_MAX]={0};
    
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
    
    if(NULL == param)
    {
        fprintf(stderr,"The para is error!\r\n");
        return;
    }

    memcpy(ap_info, param, size);

    for(counter=0; counter < AP_NUM_MAX; counter++)
    {
        fprintf(stderr,"[%s] ssid len:%d, {%s}\n",__FUNCTION__,strlen(ap_info[counter].ssid), ap_info[counter].ssid);        
        fprintf(stderr,"[%s] signal_strength is %d\n",__FUNCTION__,ap_info[counter].signal_strength);
        fprintf(stderr,"[%s] secure_mode is %d\n",__FUNCTION__,ap_info[counter].secure_mode);
        fprintf(stderr,"[%s] security is %s\n",__FUNCTION__,ap_info[counter].security);
    }

    web_show_wifi_config_result_page(WEB_WIFI_CONFIG_SCAN_SUCCESS, ap_info);

    fg_thread_exit = FALSE;
}


/*************************************************
 * Function:  web_clientConnectWifiByCustomStatusRecv
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
static void web_clientConnectWifiByCustomStatusRecv(int fd,char *param,int size)
{
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
    
    if(NULL == param)
    {
        fprintf(stderr,"The para is error!\r\n");
        return;
    }

    WEB_SET_CMD_STT status;

    status = *((WEB_SET_CMD_STT *)param);
    fprintf(stderr,"[%s] status is %d\n",__FUNCTION__,status);

    if(WEB_SET_CMD_SUCCESS == status)
    {
        web_show_wifi_config_result_page(WEB_WIFI_CONFIG_CUSTOM_SUCCESS, NULL);
    }
    else
    {
        web_show_wifi_config_result_page(WEB_WIFI_CONFIG_FAIL, NULL);
    }
    
    fg_thread_exit = FALSE;
}



/*************************************************
 * Function:  ipc_cb_wac_get_wifi_status
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
_sipc_msg_process_fun web_sipc_wifi_msg_list[]=
{
    {WEB_WIFI_LIST_RESPONSE, web_clientWifiListRecv},
    {WEB_CONNECT_WIFI_BY_CUSTOM_RESPONSE, web_clientConnectWifiByCustomStatusRecv},
    {NULL,NULL},
};





/*************************************************
 * Function:  tonlyWeb_wifi_client_init
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int tonlyWeb_wifi_client_init()
{
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
    
    if(tonly_module_register((CHAR*)APP_WIFI_CLEINT_NAME,(CHAR*)APP_WEB_IPC)!=0)
    {
        return -1;
    }
    if(tonly_callback_register((CHAR*)APP_WIFI_CLEINT_NAME,web_sipc_wifi_msg_list,NULL)!=0)
    {
        return -1;
    }
    web_wifi = tonly_module_handle_read(APP_WIFI_CLEINT_NAME);
    
    return 0;
}


/*************************************************
 * Function:  web_ipc_wifi_client_init
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int web_ipc_wifi_client_init(void)
{
	fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
	s_comm_set_ThreadName((char*)APP_WIFI_NAME);
	s_comm_save_pid_to_file((char*)APP_WIFI_NAME,getpid());
    
	tonlyWeb_wifi_client_init();
}


int main(int argc, char* argv[])
{
    char* content = NULL;
    char wifi_set_type[BUFFER_LEN] = {0};
    char url_ssid[BUFFER_LEN] = {0};
    char ssid[BUFFER_LEN] = {0};
    char secure_mode[BUFFER_LEN] = {0};
    char password[BUFFER_LEN] = {0};
    WEB_WIFI_SSID_PW ssid_pw = {0};
    int counter = 20*3;
    
    char* ap_name = "TP-LINK_LIN";
    char* signal_strength = "70";
    char* secu_mode = "WPA2";

    web_ipc_wifi_client_init();

//    usleep(100*1000);

    content = getenv("QUERY_STRING");
    if(content == NULL)
    {
#if OPEN_SPK_DEBUG_INFO
        fprintf(stderr, "content is NULL!!\n");
#endif

        web_show_wifi_config_result_page(WEB_WIFI_CONFIG_INPUT_NULL, NULL);
    }
    else 
    {
//        printf("<h1>Input is {%s}</h1>\n", content);

        sscanf(content, "wifi_setting=%[^&]&ssid=%[^&]&secure_mode=%[^&]&password=%s",wifi_set_type,url_ssid,secure_mode,password);

#if OPEN_SPK_DEBUG_INFO
        fprintf(stderr, "len:%d ,wifi_set_type:{%s}\n",strlen(wifi_set_type), wifi_set_type);
        fprintf(stderr, "len:%d ,ssid:{%s}\n",strlen(url_ssid), url_ssid);
        fprintf(stderr, "len:%d ,secure_mode:{%s}\n",strlen(secure_mode), secure_mode);
        fprintf(stderr, "len:%d ,password:{%s}\n",strlen(password), password);
#endif

        //automatically   ,  manually
        if(0 == strncmp(wifi_set_type, "scan", strlen(wifi_set_type)))
        {
            ipc_web_client_get_wifi_list();
        }
        else if(0 == strncmp(wifi_set_type, "manually", strlen(wifi_set_type)))
        {
            UrlDecodeToUtf8(url_ssid, strlen(url_ssid),ssid);
            memcpy(&ssid_pw.ssid, ssid, strlen(ssid)+1);

            if(0 == strlen(ssid))
            {
                web_show_wifi_config_result_page(WEB_WIFI_CONFIG_SSID_INPUT_NULL, NULL);

                fprintf(stderr,"web_wifi exit!!\n");
                
                #if FIX_HREATH_CANNOT_EXIT_ISSUE
                kill(getpid(),SIGHUP);
                #endif
                
                return 0;
            }
            else if((0 == strlen(password))&&(strncmp(secure_mode, SECURE_NONE, strlen(SECURE_NONE))))
            {
                web_show_wifi_config_result_page(WEB_WIFI_CONFIG_PASSWORD_INPUT_NULL, NULL);

                fprintf(stderr,"web_wifi exit!!\n");
                
                #if FIX_HREATH_CANNOT_EXIT_ISSUE
                kill(getpid(),SIGHUP);
                #endif
                
                return 0;
            }
            
            if(0 ==  strncmp(secure_mode, SECURE_NONE, strlen(SECURE_NONE)))
            {
                ssid_pw.secure_mode = WEB_SECURE_NONE;
            }
            else if(0 ==  strncmp(secure_mode, SECURE_WEP, strlen(SECURE_WEP)))
            {
                ssid_pw.secure_mode = WEB_SECURE_WEP;
                memcpy(&ssid_pw.pw, password, strlen(password)+1);
            } 
            else if(0 ==  strncmp(secure_mode, SECURE_WPA_WPA2, strlen(SECURE_WPA_WPA2)))
            {
                ssid_pw.secure_mode = WEB_SECURE_WPA_WPA2;
                memcpy(&ssid_pw.pw, password, strlen(password)+1);
            }
            else if(0 ==  strncmp(secure_mode, SECURE_WPA2, strlen(SECURE_WPA2)))
            {
                ssid_pw.secure_mode = WEB_SECURE_WPA2;
                memcpy(&ssid_pw.pw, password, strlen(password)+1);
            }
            else if(0 ==  strncmp(secure_mode, SECURE_WPA, strlen(SECURE_WPA)))
            {
                ssid_pw.secure_mode = WEB_SECURE_WPA;
                memcpy(&ssid_pw.pw, password, strlen(password)+1);
            }
            else
            {
                web_show_wifi_config_result_page(WEB_WIFI_CONFIG_FAIL, NULL);
                
                fprintf(stderr,"web_wifi exit!!\n");

                #if FIX_HREATH_CANNOT_EXIT_ISSUE
                kill(getpid(),SIGHUP);
                #endif
                
                return 0;
            }
            
            ipc_web_client_set_wifi(&ssid_pw);
        }
        else
        {

        }
    }

    while(fg_thread_exit&&counter)
    {
    	counter--;
		
        usleep(50*1000);
        fprintf(stderr,"web_wifi waiting!!\n");
    }

     if(fg_thread_exit&&(0 == counter))
    {
        web_show_wifi_config_result_page(WEB_WIFI_CONFIG_FAIL, NULL);
    }
    
//    usleep(20*1000);
    fprintf(stderr,"web_wifi exit!!\n");

#if FIX_HREATH_CANNOT_EXIT_ISSUE
    kill(getpid(),SIGHUP);
#endif

    return 0;
}
