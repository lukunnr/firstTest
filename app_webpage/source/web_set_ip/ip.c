#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "api_webpage.h"


#include "tonly_typedef.h"
#include "system_ipc.h"


#define BUFFER_LEN  512

static unsigned int web_ip;
static int fg_thread_exit = TRUE;
WEB_NETWORK_IP_CONFIG ip_config = {0};

typedef enum
{
    WEB_IP_SETTING_INPUT_NULL,
    WEB_IP_SETTING_INPUT_INVALID,
    WEB_IP_SETTING_FAIL,
    WEB_IP_SETTING_AUTO_SUCCESS,
    WEB_IP_SETTING_MANUAL_SUCCESS,
}WEB_IP_SETTING_STT;

/*************************************************
 * Function:  ipc_cb_wac_get_wifi_status
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
void ipc_web_client_set_network(WEB_NETWORK_IP_CONFIG* ip_config)
{
    char* Param_Send;
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);

    Param_Send = (char*)ip_config;
    sipc_sendCmd(web_ip,WEB_SET_NETWORK, Param_Send, sizeof(WEB_NETWORK_IP_CONFIG));
}



/*************************************************
 * Function:  web_clientSetIpAutoStatusRecv
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
static void web_clientSetIpAutoStatusRecv(int fd,char *param,int size)
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
        web_show_ip_setting_result_page(WEB_IP_SETTING_AUTO_SUCCESS);
    }
    else
    {
        web_show_ip_setting_result_page(WEB_IP_SETTING_FAIL);	
    }
    
    fg_thread_exit = FALSE;
}



/*************************************************
 * Function:  web_clientSetIpManualStatusRecv
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
static void web_clientSetIpManualStatusRecv(int fd,char *param,int size)
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
        web_show_ip_setting_result_page(WEB_IP_SETTING_MANUAL_SUCCESS);
    }
    else
    {
        web_show_ip_setting_result_page(WEB_IP_SETTING_FAIL);	
    }
    
    fg_thread_exit = FALSE;
}


/*************************************************
 * Function:  web_sipc_ip_msg_list
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
_sipc_msg_process_fun web_sipc_ip_msg_list[]=
{
    {WEB_SET_IP_AUTO_RESPONSE, web_clientSetIpAutoStatusRecv},
    {WEB_SET_IP_MANUAL_RESPONSE, web_clientSetIpManualStatusRecv},
    {NULL,NULL},
};





/*************************************************
 * Function:  tonlyWeb_ip_client_init
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int tonlyWeb_ip_client_init()
{
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
    if(tonly_module_register((CHAR*)APP_IP_CLEINT_NAME,(CHAR*)APP_WEB_IPC)!=0)
    {
        return -1;
    }
    if(tonly_callback_register((CHAR*)APP_IP_CLEINT_NAME,web_sipc_ip_msg_list,NULL)!=0)
    {
        return -1;
    }
    web_ip = tonly_module_handle_read(APP_IP_CLEINT_NAME);
    
    return 0;
}


bool isValidIP(char *ip)
{
    char temp[4];
    int count=0;
    int index=0;
            
    if(ip==NULL)
    {
        #if OPEN_SPK_DEBUG_INFO
        fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
        #endif
        
        return false;
    }
    
    while(true)
    {
        while(*ip!='\0' && *ip!='.' && count<4)
        {
            temp[index++]=*ip;
            ip++;
        }

        if(index==4)
        {
            #if OPEN_SPK_DEBUG_INFO
            fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
            #endif
            
            return false;
        }
        
        temp[index]='\0';
        index = 0;

        #if OPEN_SPK_DEBUG_INFO
        fprintf(stderr,"[%s] line is %d, temp is %s\n",__FUNCTION__,__LINE__,temp);
        #endif
        
        int num=atoi(temp);

        #if OPEN_SPK_DEBUG_INFO
        fprintf(stderr,"[%s] line is %d, num is %d\n",__FUNCTION__,__LINE__,num);
        #endif
        
        if(!(num>=0 && num<=255))
        {
            return false;
        }

        #if OPEN_SPK_DEBUG_INFO
        fprintf(stderr,"[%s] line is %d,ip[%d] is %c \n",__FUNCTION__,__LINE__,count,*ip);
        #endif
        
        count++;
        if(*ip=='\0')
        {
            if(count==4)
            {
                #if OPEN_SPK_DEBUG_INFO
                fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
                #endif
                
                return true;
            }
            else
            {
                #if OPEN_SPK_DEBUG_INFO
                fprintf(stderr,"[%s] line is %d, count is %d\n",__FUNCTION__,__LINE__,count);
                #endif
                
                return false;
            }
        }
        else
        {
            ip++;
        }
    }
}


/*************************************************
 * Function:  web_ipc_ip_client_init
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int web_ipc_ip_client_init(void)
{
	fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
	s_comm_set_ThreadName((char*)APP_IP_NAME);
	s_comm_save_pid_to_file((char*)APP_IP_NAME,getpid());
    
	tonlyWeb_ip_client_init();
}


/*************************************************
 * Function:  web_show_ip_setting_result_page
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int web_show_ip_setting_result_page(WEB_IP_SETTING_STT status)
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
        case WEB_IP_SETTING_INPUT_NULL:  
#if OPEN_WEB_DEBUG_INFO
            printf("<h3>The input can not be empty.</h3>\n");
#endif
            break;

        case WEB_IP_SETTING_INPUT_INVALID:
#if OPEN_WEB_DEBUG_INFO
            printf("<h3>The input is not valid. Please check your input.</h3>\n");
#endif
            break;

        case WEB_IP_SETTING_FAIL:
#if OPEN_WEB_DEBUG_INFO
            printf("<h3>The setup failed.</h3>\n");
#endif            
            break;

        case WEB_IP_SETTING_AUTO_SUCCESS:
#if OPEN_WEB_DEBUG_INFO
            printf("<h3>The auto setup is successful.</h3>\n");
#endif
            break;

        case WEB_IP_SETTING_MANUAL_SUCCESS:
#if OPEN_WEB_DEBUG_INFO
            printf("<h3>The manual setup is successful.</h3>\n");         
 //           printf("<h3>Now configure the network.Please wait...</h3>\n");
 //           printf("<h3>By the way, please refresh web page manually after the network configured successfully.</h3>\n");
#endif

//            printf("<head>\n");
//            printf("<meta http-equiv='Refresh' content='10; url=http://%s/index.cgi'>\n",ip_config.ip_addr);
//            printf("</head>\n");

            printf("<br />\n");
            printf("<form action='http://%s/index.cgi'>\n",ip_config.ip_addr);
            printf("<input type='submit' value='Back to main' />\n");
            printf("</form>\n");
            printf("</html>\n");
            
            fflush(stdout);
            return 0;
         //   break;

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
    int counter = 10;
    char* content = NULL;
    char ip_setting[BUFFER_LEN] = {0};
    char ip_address[BUFFER_LEN] = {0};
    char subnet_mask[BUFFER_LEN] = {0};

	#if OPEN_WEB_DNS_SETTING
    char gateway[BUFFER_LEN] = {0};
    char dns1[BUFFER_LEN] = {0};
    char dns2[BUFFER_LEN] = {0};
	#endif
//    WEB_NETWORK_IP_CONFIG ip_config = {0};

    web_ipc_ip_client_init();

//    usleep(100*1000);

    content = getenv("QUERY_STRING");
    if(content == NULL)
    {
#if OPEN_SPK_DEBUG_INFO
        fprintf(stderr, "content is NULL!!\n");
#endif
        
        web_show_ip_setting_result_page(WEB_IP_SETTING_INPUT_NULL);
    }
    else 
    {
//        printf("<h1>Input is {%s}</h1>", content);
		#if OPEN_WEB_DNS_SETTING
        sscanf(content, "ip_setting=%[^&]&ip_address=%[^&]&subnet_mask=%[^&]&gateway=%[^&]&dns_server1=%[^&]&dns_server2=%s",   
        ip_setting,ip_address,subnet_mask,gateway,dns1,dns2);
		#else
		sscanf(content, "ip_setting=%[^&]&ip_address=%[^&]&subnet_mask=%s",   
        ip_setting,ip_address,subnet_mask);
		#endif

#if OPEN_SPK_DEBUG_INFO
        fprintf(stderr, "len:%d, ip_setting is {%s}\n",strlen(ip_setting), ip_setting);
        fprintf(stderr, "len:%d, ip_address is {%s}\n",strlen(ip_address), ip_address);
        fprintf(stderr, "len:%d, subnet_mask is {%s}\n",strlen(subnet_mask), subnet_mask);

		#if OPEN_WEB_DNS_SETTING
        fprintf(stderr, "len:%d, gateway is {%s}\n",strlen(gateway), gateway);
        fprintf(stderr, "len:%d, dns1 is {%s}\n",strlen(dns1), dns1);
        fprintf(stderr, "len:%d, dns2 is {%s}\n",strlen(dns2), dns2);
#endif
#endif

        //automatically   ,  manually
        if(0 == strncmp(ip_setting, "automatically", strlen(ip_setting)))
        {
            ip_config.ip_mode = WEB_SET_IP_AUTO;
            ipc_web_client_set_network(&ip_config);
        }
        else if(0 == strncmp(ip_setting, "manually", strlen(ip_setting)))
        {			
            ip_config.ip_mode = WEB_SET_IP_MANUALLY;

            if((0 == strlen(ip_address))
            ||(0 == strlen(subnet_mask))
            #if OPEN_WEB_DNS_SETTING
            ||(0 == strlen(gateway))
            ||((0 == strlen(dns1))&&(0 == strlen(dns2)))
            #endif
            )
            {
                web_show_ip_setting_result_page(WEB_IP_SETTING_INPUT_NULL);

                #if FIX_HREATH_CANNOT_EXIT_ISSUE
                kill(getpid(),SIGHUP);
                #endif
                
                return 0;
            }
            
            if(isValidIP(ip_address) 
                && isValidIP(subnet_mask)
                #if OPEN_WEB_DNS_SETTING
                && isValidIP(gateway)
                &&( (strlen(dns1)&&isValidIP(dns1))||(strlen(dns2)&&isValidIP(dns2)) )
                #endif
                )
            {
            	   #if OPEN_SPK_DEBUG_INFO
                 fprintf(stderr, "Input subnet_mask and ip is valid !!\n");
                #endif
            }
            else
            {   
                #if OPEN_SPK_DEBUG_INFO
                fprintf(stderr, "Input subnet_mask and ip is not valid !!\n");
                #endif

                web_show_ip_setting_result_page(WEB_IP_SETTING_INPUT_INVALID);

                
#if FIX_HREATH_CANNOT_EXIT_ISSUE
                kill(getpid(),SIGHUP);
#endif
                return 0;
            }

            int counter = 20;
            
            memcpy(ip_config.ip_addr, ip_address, strlen(ip_address)+1);
            memcpy(ip_config.sub_mask, subnet_mask, strlen(subnet_mask)+1);

			#if OPEN_WEB_DNS_SETTING
            memcpy(ip_config.gateway, gateway, strlen(gateway)+1);
            memcpy(ip_config.dns1, dns1, strlen(dns1)+1);
            memcpy(ip_config.dns2, dns2, strlen(dns2)+1);
            #endif

            ipc_web_client_set_network(&ip_config);
        }
        else
        {

        }
    }

    while(fg_thread_exit&&counter)
    {
        counter--;

        usleep(50*1000);
        fprintf(stderr,"web_get_set_ip_status waiting!!\n");
    }

     if(fg_thread_exit&&(0 == counter))
    {
        web_show_ip_setting_result_page(WEB_IP_SETTING_FAIL);
    }
     
    fprintf(stderr,"web_ip exit!!\n");
    
#if FIX_HREATH_CANNOT_EXIT_ISSUE
    kill(getpid(),SIGHUP);
#endif
    
    return 0;
}
