#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "api_webpage.h"


#include "tonly_typedef.h"
#include "system_ipc.h"


#define BUFFER_LEN  512
static unsigned int web_cfd;
static int fg_thread_exit = TRUE;

typedef enum
{
    WEB_CDN_INPUT_NULL,
    WEB_CDN_INPUT_TOO_LONG,
    WEB_CDN_FAIL,
    WEB_CDN_SUCCESS,
}WEB_CDN_STT;


/*************************************************
 * Function:  ipc_cb_wac_get_wifi_status
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
void ipc_web_client_set_device_name(char * name)
{
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);

    sipc_sendCmd(web_cfd,WEB_SET_DEVICE_NAME,(char *)name,strlen(name)+1);
}


/*************************************************
 * Function:  web_clientSetDeviceNameStatusRecv
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
static void web_clientSetDeviceNameStatusRecv(int fd,char *param,int size)
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
        web_show_cdn_result_page(WEB_CDN_SUCCESS);	
    }
    else
    {
        web_show_cdn_result_page(WEB_CDN_FAIL);	
    }
    
    fg_thread_exit = FALSE;
}

/*************************************************
 * Function:  web_sipc_cdn_msg_list
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
_sipc_msg_process_fun web_sipc_cdn_msg_list[]=
{
    {WEB_SET_DEVICE_NAME_RESPONSE, web_clientSetDeviceNameStatusRecv},
    {NULL,NULL},
};



/*************************************************
 * Function:  tonlyWeb_cdn_client_init
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int tonlyWeb_cdn_client_init(void)
{
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
    
    if(tonly_module_register((char*)APP_CDN_CLEINT_NAME,(char*)APP_WEB_IPC)!=0)
    {
        return -1;
    }
    if(tonly_callback_register((char*)APP_CDN_CLEINT_NAME,web_sipc_cdn_msg_list,NULL)!=0)
    {
        return -1;
    }
    web_cfd = tonly_module_handle_read(APP_CDN_CLEINT_NAME);
    
    return 0;
}



/*************************************************
 * Function:  web_ipc_cdn_client_init
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int web_ipc_cdn_client_init(void)
{
       int ret = 0;
       
	fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
	ret = s_comm_set_ThreadName((char*)APP_CDN_NAME);        
	ret = s_comm_save_pid_to_file((char*)APP_CDN_NAME,getpid());
    
	tonlyWeb_cdn_client_init();
}

/*************************************************
 * Function:  web_show_cdn_result_page
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int web_show_cdn_result_page(WEB_CDN_STT status)
{
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
        case WEB_CDN_INPUT_NULL:        
#if OPEN_WEB_DEBUG_INFO
            printf("<h3>The  device name can not be empty. Please enter a name.</h3>\n");
#endif
            break;

         case WEB_CDN_INPUT_TOO_LONG:
#if OPEN_WEB_DEBUG_INFO
            printf("<h3>The device name is too long.</h3>\n");
#endif
            break;

        case WEB_CDN_FAIL:
#if OPEN_WEB_DEBUG_INFO
            printf("<h3>The device name is changed failed.</h3>\n");
#endif
            break;

        case WEB_CDN_SUCCESS:
#if OPEN_WEB_DEBUG_INFO
            printf("<h3>The device name is changed ok.</h3>\n");
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
 * Function:  web_ipc_cdn_client_create
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int web_ipc_cdn_client_create(void)
{
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);

	pthread_attr_t thread_attr;

	pthread_t web_client_sipc;	

	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if(-1==pthread_create(&web_client_sipc, &thread_attr, web_ipc_cdn_client_init, NULL))
	{
		fprintf(stderr, "Create  web_sipc failed !\n");
		pthread_attr_destroy(&thread_attr);
		return -1;
	}
	pthread_attr_destroy(&thread_attr); 
}


/*************************************************
 * Function:  main
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int main(int argc, char* argv[])
{
    int counter = 10;
    char* content = NULL;
    char device_name[BUFFER_LEN] = {0};
    char url_name[URL_LEN_MAX] ={0};

//    web_ipc_cdn_client_create();
    web_ipc_cdn_client_init();

    content = getenv("QUERY_STRING");

    if(content == NULL)
    { 
#if OPEN_SPK_DEBUG_INFO
        fprintf(stderr, "content is NULL!!\n");
#endif

        web_show_cdn_result_page(WEB_CDN_INPUT_NULL);
    }
    else 
    {
        sscanf(content, "device_name=%s",url_name);

        #if OPEN_SPK_DEBUG_INFO
        fprintf(stderr, "len:%d ,url device_name:{%s}\n",strlen(url_name), url_name);
        #endif

        UrlDecodeToUtf8(url_name, strlen(url_name),device_name);
        if(strlen(device_name) <= NAME_LEN_MAX)
        {
             ipc_web_client_set_device_name(device_name);
        }
        else
        {
            web_show_cdn_result_page(WEB_CDN_INPUT_TOO_LONG);

            #if FIX_HREATH_CANNOT_EXIT_ISSUE
            kill(getpid(),SIGHUP);
            #endif

            return 0;
        }
		
    }

    while(fg_thread_exit&&counter)
    {
        counter--;

        usleep(50*1000);
        fprintf(stderr,"web_get_set_device_name_status waiting!!\n");
    }

    if(fg_thread_exit&&(0 == counter))
    {
        web_show_cdn_result_page(WEB_CDN_FAIL);
    }

    fprintf(stderr,"web_cdn exit!!\n");

    #if FIX_HREATH_CANNOT_EXIT_ISSUE
    kill(getpid(),SIGHUP);
    #endif
    
    return 0;
}
