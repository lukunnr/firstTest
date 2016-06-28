#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "cgic.h"


#include "api_webpage.h"


#include "tonly_typedef.h"
#include "system_ipc.h"


#define BufferLen 1024

#define FORM_NAME  "upload"


static unsigned int web_upg;
static int fg_thread_exit = TRUE;

/*************************************************
 * Function:  ipc_cb_wac_get_wifi_status
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
void ipc_web_client_start_upg(void)
{
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);

    sipc_sendCmd(web_upg,WEB_FIRMWARE_UPGRADE, NULL,0);
}


/*************************************************
 * Function:  web_clientUpgStatusRecv
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
static void web_clientUpgStatusRecv(int fd,char *param,int size)
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

    
    printf("<html>\n");
    
    if(WEB_SET_CMD_SUCCESS == status)
    {
//        printf("<script type='text/javascript' language='javascript'>alert('During a software upgrade,do not power off the speaker or use any its controls.');parent.location.replace('./upgrade_firmware_waitting.cgi');</script>\n");  

        printf("<h3>During a software upgrade,do not power off the speaker or use any its controls.</h3>\n");
        printf("<h3>Now start software upgrade, please wait...</h3>\n");
        printf("<head>\n");
        printf("<title>Firmware upg start successfully</title>\n");
        printf("<meta http-equiv='Refresh' content='10; url=./upgrade_firmware_waitting.cgi'>\n");
        printf("</head>\n");
    }
    else
    {
//        printf("<script type='text/javascript' language='javascript'>alert('File upg failed,please try again.');parent.location.replace('../index.cgi');</script>\n");  

        printf("<h3>File upg failed,please try again.</h3>\n");
        printf("<head>\n");
        printf("<title>Firmware upg start failed</title>\n");
        printf("<meta http-equiv='Refresh' content='10; url=../index.cgi'>\n");
        printf("</head>\n");
    }

    printf("</html>\n");

    fg_thread_exit = FALSE;
}


/*************************************************
 * Function:  web_sipc_upg_msg_list
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
_sipc_msg_process_fun web_sipc_upg_msg_list[]=
{
    {WEB_UPG_STATUS_RESPONSE, web_clientUpgStatusRecv},
    {NULL,NULL},
};





/*************************************************
 * Function:  tonlyWeb_upg_client_init
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int tonlyWeb_upg_client_init()
{
	fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
	if(tonly_module_register((CHAR*)APP_UPG_CLEINT_NAME,(CHAR*)APP_WEB_IPC)!=0)
	{
		return -1;
	}
	if(tonly_callback_register((CHAR*)APP_UPG_CLEINT_NAME,web_sipc_upg_msg_list,NULL)!=0)
	{
		return -1;
	}
	web_upg = tonly_module_handle_read(APP_UPG_CLEINT_NAME);
	return 0;
}




/*************************************************
 * Function:  web_ipc_upg_client_init
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int web_ipc_upg_client_init(void)
{
	fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);
	s_comm_set_ThreadName((char*)APP_UPG_NAME);
	s_comm_save_pid_to_file((char*)APP_UPG_NAME,getpid());
    
	tonlyWeb_upg_client_init();
}


/*************************************************
 * Function:  web_ipc_upg_client_create
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int web_ipc_upg_client_create(void)
{
    fprintf(stderr,"[%s] line is %d\n",__FUNCTION__,__LINE__);

	pthread_attr_t thread_attr;

	pthread_t web_client_sipc;	

	pthread_attr_init(&thread_attr);
	pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);
	if(-1==pthread_create(&web_client_sipc, &thread_attr, web_ipc_upg_client_init, NULL))
	{
		fprintf(stderr, "Create  web_sipc failed !\n");
		pthread_attr_destroy(&thread_attr);
		return -1;
	}
	pthread_attr_destroy(&thread_attr); 
}



int cgiMain(void)
{
    cgiFilePtr file;
    int targetFile;
    mode_t mode;
    char name[128];
    char fileNameOnServer[64];
    char contentType[1024];
    char buffer[BufferLen];
    char *tmpStr=NULL;
    int size;
    int got,t;
    int upload_counter = 0;
    int counter = 150;    // 150S

    web_ipc_upg_client_create();
//    web_ipc_upg_client_init();
    usleep(100*1000);

//    cgiHeaderContentType("text/html");
    cgiHeaderContentType("text/html;charset=UTF-8");
    

#if USE_BACKGROND_COLOR
	printf("<body bgcolor=rgb(%d,%d,%d)></body>\n", BG_COLOR_R, BG_COLOR_G, BG_COLOR_B);
#else if(USE_BACKGROND_IMAGE)
	printf("<body style='background-image:url(%s)'></body>\n",BACKGROUND_IMAGE_PATH);
#endif

    //取得html页面中file元素的值，应该是文件在客户机上的路径名
    if (cgiFormFileName(FORM_NAME, name, sizeof(name)) != cgiFormSuccess)   // Jacky 
    {
        fprintf(stderr,"could not retrieve filename\n");
        goto FAIL;
    }
    fprintf(stderr,"name is %s\n",name);
    
    cgiFormFileSize(FORM_NAME, &size);
    fprintf(stderr,"file size is %d\n",size);

    //取得文件类型，不过本例中并未使用
    cgiFormFileContentType(FORM_NAME, contentType, sizeof(contentType));
    fprintf(stderr,"contentType is %s\n",contentType);

    /*目前文件存在于系统临时文件夹中，
    通常为/tmp，通过该命令打开临时文件。
    临时文件的名字与用户文件的名字不同，
    所以不能通过路径/tmp/userfilename的方式获得文件*/
    if (cgiFormFileOpen(FORM_NAME, &file) != cgiFormSuccess)    // Jacky
    {
        fprintf(stderr,"could not open the file\n");
        goto FAIL;
    }

    t=-1;

    //从路径名解析出用户文件名
    while(1)
    {
        tmpStr=strstr(name+t+1, "\\");
        if(NULL==tmpStr)
        {
            tmpStr=strstr(name+t+1,"/");  //if "\" is not path separator, try "/"
        }

        if(NULL!=tmpStr)
        {
                t=(int)(tmpStr-name);
        }
        else
        {
            break;
        }
    } 

    strcpy(fileNameOnServer, name+t+1);
    mode = S_IRWXU|S_IRGRP|S_IROTH;

    fprintf(stderr,"fileNameOnServer is %s\n",fileNameOnServer);
    /*在当前目录下建立新的文件，第一个参数实际
    上是路径名，此处的含义是在cgi程序所在的目录
    （当前目录））建立新文件*/
    targetFile = open(fileNameOnServer,O_RDWR|O_CREAT|O_TRUNC|O_APPEND,mode);
    if(targetFile < 0)
    {
        fprintf(stderr,"could not create the new file,%s\n",fileNameOnServer);
        goto FAIL;
    }

    //从系统临时文件中读出文件内容，并放到刚创建的目标文件中
    while (cgiFormFileRead(file, buffer, BufferLen, &got) == cgiFormSuccess)
    {
        if(got > 0)
        {
            write(targetFile,buffer,got);

            if(size > BufferLen)
            {
                upload_counter += got;
    //            printf("<body>File %d has been uploaded</body>\n",((upload_counter*100)/size));
            }
        }
    }
    cgiFormFileClose(file);
    close(targetFile);
    goto END;

    FAIL:
    fprintf(stderr,"Failed to upload");
    printf("<script type='text/javascript' language='javascript'>alert('Failed to upload!');parent.location.replace('upgrade_firmware_select.cgi');</script>\n");
    fflush(stdout);
    
    return 1;
    
    END:
    ipc_web_client_start_upg();

    while(fg_thread_exit&&counter)
    {
        counter --;
		
        sleep(1);
        fprintf(stderr,"web_wifi waiting!!\n");
    }

     if(fg_thread_exit&&(0 == counter))
    {
         printf("<script type='text/javascript' language='javascript'>alert('File upg failed!');parent.location.replace('../index.cgi');</script>\n");  
    }
    
//    usleep(20*1000);
    fprintf(stderr,"web_upg exit!!\n");

    fflush(stdout);
    /*
    printf("<h1>File %s has been uploaded ok!!</h1>\n",fileNameOnServer);
    printf("<br />\n");
    printf("<form action='/index.cgi'>\n");
    printf("<input type='submit' value='Back to main' />\n");
    printf("</form>\n");
    */
    return 0;
}




