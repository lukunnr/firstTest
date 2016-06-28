#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "api_webpage.h"


#include "tonly_typedef.h"
#include "system_ipc.h"

#define BUFFER_LEN  512

static unsigned int web_cnt_wifi;
static int fg_thread_exit = TRUE;

typedef enum
{
    WEB_WIFI_CONNECT_INPUT_NULL,
    WEB_WIFI_CONNECT_INPUT_SSID_NULL,
    WEB_WIFI_CONNECT_FAIL,
    WEB_WIFI_CONNECT_SUCCESS,
}WEB_WIFI_CONNECT_STT;


/*************************************************
 * Function:  ipc_web_client_connect_wifi
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
void ipc_web_client_connect_wifi(WEB_WIFI_SSID_PW ssid_pw)
{
    char* Param_Send;
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);

    Param_Send = (char*)&ssid_pw;
    sipc_sendCmd(web_cnt_wifi,WEB_CONNECT_WIFI, Param_Send, sizeof(WEB_WIFI_SSID_PW));
}



/*************************************************
 * Function:  web_clientConnectWifiByScanStatusRecv
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
static void web_clientConnectWifiByScanStatusRecv(int fd,char *param,int size)
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
        web_show_wifi_connect_result_page(WEB_WIFI_CONNECT_SUCCESS);
    }
    else
    {
        web_show_wifi_connect_result_page(WEB_WIFI_CONNECT_FAIL);
    }
    
    fg_thread_exit = FALSE;
}



/*************************************************
 * Function:  web_sipc_cnt_wifi_msg_list
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
_sipc_msg_process_fun web_sipc_cnt_wifi_msg_list[]=
{
    {WEB_CONNECT_WIFI_BY_SCAN_RESPONSE, web_clientConnectWifiByScanStatusRecv},
    {NULL,NULL},
};





/*************************************************
 * Function:  tonlyWeb_wifi_client_init
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int tonlyWeb_cnt_wifi_client_init()
{
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
    
    if(tonly_module_register((CHAR*)APP_CNT_WIFI_CLEINT_NAME,(CHAR*)APP_WEB_IPC)!=0)
    {
        return -1;
    }
    if(tonly_callback_register((CHAR*)APP_CNT_WIFI_CLEINT_NAME,web_sipc_cnt_wifi_msg_list,NULL)!=0)
    {
        return -1;
    }
    web_cnt_wifi = tonly_module_handle_read(APP_CNT_WIFI_CLEINT_NAME);
    
    return 0;
}

/*************************************************
 * Function:  web_ipc_cnt_wifi_client_init
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int web_ipc_cnt_wifi_client_init(void)
{
	fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
	s_comm_set_ThreadName((char*)APP_CNT_WIFI_NAME);
	s_comm_save_pid_to_file((char*)APP_CNT_WIFI_NAME,getpid());
    
	tonlyWeb_cnt_wifi_client_init();
}


/*************************************************
 * Function:  web_show_wifi_config_result_page
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int web_show_wifi_connect_result_page(WEB_WIFI_CONNECT_STT status)
{   
//   printf("Content-type:text/html\n\n");
    printf("Content-type:text/html;charset=UTF-8\n\n");
   
   printf("<html>\n");
#if USE_BACKGROND_COLOR
   printf("<body bgcolor=rgb(%d,%d,%d)></body>\n", BG_COLOR_R, BG_COLOR_G, BG_COLOR_B);
#else if(USE_BACKGROND_IMAGE)
   printf("<body style='background-image:url(%s)'></body>\n",BACKGROUND_IMAGE_PATH);
#endif

    switch(status)
    {
        case WEB_WIFI_CONNECT_INPUT_NULL:        
#if OPEN_WEB_DEBUG_INFO
            printf("<h3>The input can not be empty.</h3>\n");
#endif
            break;

        case WEB_WIFI_CONNECT_INPUT_SSID_NULL:
#if OPEN_WEB_DEBUG_INFO
            printf("<h3>The SSID can not be empty.</h3>\n");
#endif
            break;

        case WEB_WIFI_CONNECT_FAIL:
#if OPEN_WEB_DEBUG_INFO
            printf("<h3>The wifi connected failed.</h3>\n");
#endif            
            break;

        case WEB_WIFI_CONNECT_SUCCESS:
#if OPEN_WEB_DEBUG_INFO
//            printf("<h3>The wifi connected successful.</h3>\n");
            printf("<meta http-equiv='Refresh' content='1; url=./wifi_connect_waitting.cgi'>\n");
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



int main(int argc, char* argv[])
{
    char* content = NULL;
    char password[BUFFER_LEN] = {0};
    char ssid[BUFFER_LEN] = {0};
    char url_ssid[BUFFER_LEN] = {0};
    WEB_WIFI_SSID_PW ssid_pw;
    int counter = 60;

    web_ipc_cnt_wifi_client_init();

//    usleep(100*1000);
    
    content = getenv("QUERY_STRING");
    if(content == NULL)
    {
#if OPEN_SPK_DEBUG_INFO
        fprintf(stderr, "content is NULL!!\n");
#endif
        
        web_show_wifi_connect_result_page(WEB_WIFI_CONNECT_INPUT_NULL);
    }
    else 
    {
  //      printf("<h1>Input is {%s}</h1>\n", content);
      

        if(0 == strncmp(content, "ssid=", 5))
        {
            sscanf(content, "ssid=%[^&]&password=%s",url_ssid,password);

#if OPEN_SPK_DEBUG_INFO
            fprintf(stderr, "len:%d ,ssid:{%s}\n",strlen(url_ssid), url_ssid);
            fprintf(stderr, "len:%d ,password:{%s}\n",strlen(password), password);
#endif

            ssid_pw.secure_mode = WEB_SECURE_AUTO;

            UrlDecodeToUtf8(url_ssid, strlen(url_ssid),ssid);
            memcpy(ssid_pw.ssid, ssid, strlen(ssid)+1);
            memcpy(ssid_pw.pw, password, strlen(password)+1);
            ipc_web_client_connect_wifi(ssid_pw);
        }
        else if(0 == strncmp(content, "password=", 9))
        {
            sscanf(content, "password=%s",password);

#if OPEN_SPK_DEBUG_INFO
            fprintf(stderr, "len:%d ,password:{%s}\n",strlen(password), password);
#endif
            web_show_wifi_connect_result_page(WEB_WIFI_CONNECT_INPUT_SSID_NULL);

            
#if FIX_HREATH_CANNOT_EXIT_ISSUE
            kill(getpid(),SIGHUP);
#endif

            return 0;
        }

    }

    while(fg_thread_exit&&counter)
    {
    	counter--;
		
        sleep(1);
        fprintf(stderr,"web_wifi waiting!!\n");
    }

    if(fg_thread_exit&&(0 == counter))
    {
        web_show_wifi_connect_result_page(WEB_WIFI_CONNECT_FAIL);
    }

    fprintf(stderr,"web_wifi_connect exit!!\n");

#if FIX_HREATH_CANNOT_EXIT_ISSUE
    kill(getpid(),SIGHUP);
#endif

    return 0;
}
