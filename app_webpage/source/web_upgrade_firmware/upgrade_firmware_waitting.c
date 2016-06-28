#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "api_webpage.h"

#include "tonly_typedef.h"
#include "system_ipc.h"


static unsigned int web_upg_waitting;
static int fg_thread_exit = TRUE;

typedef enum
{
    WEB_UPG_UPGRADING,
    WEB_UPG_FAIL,
    WEB_UPG_SUCCESS,
}WEB_UPG_STT;


/*************************************************
 * Function:  web_show_upg_waitting_page
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int web_show_upg_waitting_page(WEB_UPG_STT upg_stt)
{
//    printf("Content-type:text/html\n\n");
    printf("Content-type:text/html;charset=UTF-8\n\n");
    
    printf("<html>\n");

#if USE_BACKGROND_COLOR
    printf("<body bgcolor=rgb(%d,%d,%d)></body>\n", BG_COLOR_R, BG_COLOR_G, BG_COLOR_B);
#else if(USE_BACKGROND_IMAGE)
    printf("<body style='background-image:url(%s)'></body>\n",BACKGROUND_IMAGE_PATH);
#endif

    switch(upg_stt)
    {
        case WEB_UPG_UPGRADING:
            printf("<head>\n");
            printf("<title>Upgrading</title>\n");
            printf("<meta http-equiv='Refresh' content='1'>\n");
            printf("</head>\n");

            printf("<br />\n");

            printf("<body>\n");
            //    printf("<div align='center' style='position: absolute; width: 100px; height: 100px; z-index: 1; left:158px; top:17px' id='load'><img src='%s' />&nbsp;Upgrading,please wait...</div>\n",WAITTING_IMAGE_PATH);
            printf("<div align='center' id='load'><img src='%s' />&nbsp;Upgrading,please wait...</div>\n",WAITTING_IMAGE_PATH);
            printf("</body>\n");

            break;

        case WEB_UPG_FAIL:
            printf("<h3>File upg failed,please try again.</h3>\n");
            printf("<head>\n");
            printf("<title>Upgrading</title>\n");
            printf("</head>\n");
            printf("<br />\n");
            printf("<form action='/index.cgi'>\n");
            printf("<input type='submit' value='Back to main' />\n");
            printf("</form>\n");
            printf("</html>\n");
     //       printf("<script type='text/javascript' language='javascript'>alert('File upg failed,please try again.');parent.location.replace('../index.cgi');</script>\n");  
            break;

         case WEB_UPG_SUCCESS:
            printf("<h3>Speaker has upgraded successfully. It will automatically restart in 3 seconds.</h3>\n");
     //       printf("<br />\n");
            printf("<h3>Please refresh web page manually after the speaker restart.</h3>\n");

            printf("<head>\n");
            printf("<title>Upgraded successfully</title>\n");
 //           printf("<meta http-equiv='Refresh' content='3; url=../index.cgi'>\n");
            printf("</head>\n");
 
            printf("<form action='/index.cgi'>\n");
            printf("<input type='submit' value='Refresh' />\n");
            printf("</form>\n");
            printf("</html>\n");
    //        printf("<script type='text/javascript' language='javascript'>alert('Speaker has upgraded successfully. It will automatically restart in 3 seconds.');parent.location.replace('../index.cgi');</script>\n");
            break;

        default:
            break;        
    }

    printf("</html>\n");

    fflush(stdout);
  
    return 0;
}


/*************************************************
 * Function:  web_clientUpgResultRecv
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
static void web_clientUpgResultRecv(int fd,char *param,int size)
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
        web_show_upg_waitting_page(WEB_UPG_SUCCESS);
    }
    else
    {
        web_show_upg_waitting_page(WEB_UPG_FAIL);
    }

    fg_thread_exit = FALSE;
}


/*************************************************
 * Function:  web_sipc_upg_waitting_msg_list
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
_sipc_msg_process_fun web_sipc_upg_waitting_msg_list[]=
{
    {WEB_UPG_RESULT_RESPONSE, web_clientUpgResultRecv},
    {NULL,NULL},
};



/*************************************************
 * Function:  tonlyWeb_upg_waitting_client_init
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int tonlyWeb_upg_waitting_client_init()
{
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
    
    if(tonly_module_register((CHAR*)APP_UPG_WAITTING_CLEINT_NAME,(CHAR*)APP_WEB_IPC)!=0)
    {
        return -1;
    }
    if(tonly_callback_register((CHAR*)APP_UPG_WAITTING_CLEINT_NAME,web_sipc_upg_waitting_msg_list,NULL)!=0)
    {
        return -1;
    }
    web_upg_waitting = tonly_module_handle_read(APP_UPG_WAITTING_CLEINT_NAME);
    
    return 0;
}


/*************************************************
 * Function:  web_ipc_upg_waitting_client_init
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int web_ipc_upg_waitting_client_init(void)
{
	fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
	s_comm_set_ThreadName((char*)APP_UPG_WAITTING_NAME);
	s_comm_save_pid_to_file((char*)APP_UPG_WAITTING_NAME,getpid());
    
	tonlyWeb_upg_waitting_client_init();
}

/*************************************************
 * Function:  ipc_web_client_get_current_status
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
void ipc_web_client_get_upg_result(vold)
{
    char Param_Send[10] = {0};
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);

    sipc_sendCmd(web_upg_waitting,WEB_GET_UPG_RESULT, Param_Send, sizeof(Param_Send));
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
    int counter = 15;  
    
    fprintf(stderr,"<WEB>: start upg waitting!!\n");

    web_ipc_upg_waitting_client_init();

    ipc_web_client_get_upg_result();

     while(fg_thread_exit&&counter)
    {
        counter--;

        usleep(50*1000);
        fprintf(stderr,"web_get_upg_result_waiting!!\n");
    }

    if(fg_thread_exit&&(0 == counter))
    {
        web_show_upg_waitting_page(WEB_UPG_UPGRADING);
    }

#if FIX_HREATH_CANNOT_EXIT_ISSUE
    kill(getpid(),SIGHUP);
#endif
		fprintf(stderr,"web_get_upg_result_waiting exit!!\n");
    
    return 0;
}
