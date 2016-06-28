#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "api_webpage.h"

#include "tonly_typedef.h"
#include "system_ipc.h"

/*************************************************
 * Function:  web_show_upg_select_page
 * param:        
 * Return:       
 * Description:
 * Others:         
*************************************************/
int web_show_upg_select_page(void)
{
//    printf("Content-type:text/html\n\n");
    printf("Content-type:text/html;charset=UTF-8\n\n");

    printf("<html>\n");
#if USE_BACKGROND_COLOR
    printf("<body bgcolor=rgb(%d,%d,%d)></body>\n", BG_COLOR_R, BG_COLOR_G, BG_COLOR_B);
#else if(USE_BACKGROND_IMAGE)
    printf("<body style='background-image:url(%s)'></body>\n",BACKGROUND_IMAGE_PATH);
#endif

    printf("<head>\n");
    printf("<meta http-equiv='content-Type' content='text/html; charset=utf-8' />\n");
    printf("<meta http-equiv='content-Language' content='utf-8' />\n");

    printf("<title> Firmware upgrade</title>\n");
    printf("</head>\n");

    printf("<body>\n");
    printf("<form action='/cgi-bin/upgrade_firmware_start.cgi' method='post' enctype='multipart/form-data'>Firmware name: <input type='file' name='upload' />\n");
    printf("<br />\n");
    printf("<input type='submit' value='Upgrade Firmware' />\n");
    printf("</form>\n");
    printf("</body>\n");

    printf("<br />\n");
    printf("<form action='/index.cgi'>\n");
    printf("<input type='submit' value='Back to main' />\n");
    printf("</form>\n");
    
    printf("</html>\n");
    
    fflush(stdout);
    
    return 0;
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
    web_show_upg_select_page();

#if FIX_HREATH_CANNOT_EXIT_ISSUE
    kill(getpid(),SIGHUP);
#endif
    
    return 0;
}
