#include "stdio.h"
#include <string.h>
#include "api_webpage.h"

static TONLYWEB_CALLBACK_T callback;

static void webGetNetworkStatusCallBack(WEB_NETWORK_STT *in_param)
{
    char* ap_name = "TP-LINK_LIN ÎÒÄãËûABcd";
    char* ip_addr = "192.168.1.11";
    
    if(NULL == in_param)
    {
        fprintf(stderr,"The para is error!\r\n");

        return;
    }

    in_param->cnt_type = WEB_TY_WIFI;
    memcpy(in_param->ip_addr, ip_addr, strlen(ip_addr)+1);
    in_param->signal_strength = 70;
    memcpy(in_param->ssid, ap_name, strlen(ap_name)+1);
    in_param->wps_pin = 64428147;
    
}


static void webSetDeviceNameCallBack(char *name)
{
    if(NULL == name)
    {
        fprintf(stderr,"The para is error!\r\n");

         return;
    }

    fprintf(stderr,"webSetDeviceNameCallBack name -> %s \r\n",name);

    usleep(250*1000);
 //   tonlyweb_server_response_status(WEB_CMD_SET_DEVICE_NAME, WEB_SET_CMD_SUCCESS);
}


static void webSetNetworkCallBack(WEB_NETWORK_IP_CONFIG configInfo)
{
    if(WEB_SET_IP_AUTO == configInfo.ip_mode)
    {
        fprintf(stderr,"webSetNetworkCallBack ip_mode -> WEB_SET_IP_AUTO \r\n");

        usleep(250*1000);
     //   tonlyweb_server_response_status(WEB_CMD_SET_IP_AUTO, WEB_SET_CMD_SUCCESS);
    }
    else if(WEB_SET_IP_MANUALLY == configInfo.ip_mode)
    {
        fprintf(stderr,"webSetNetworkCallBack ip_mode -> WEB_SET_IP_MANUALLY \r\n");

        usleep(500*1000);
     //   tonlyweb_server_response_status(WEB_CMD_SET_IP_MANUAL, WEB_SET_CMD_SUCCESS);
    }
    else
    {
        fprintf(stderr,"The para is error!\r\n");

         return;
    }
    
    fprintf(stderr,"webSetNetworkCallBack len:%d, ip_addr -> %s \r\n",strlen(configInfo.ip_addr), configInfo.ip_addr);
    fprintf(stderr,"webSetNetworkCallBack len:%d, sub_mask -> %s \r\n",strlen(configInfo.sub_mask), configInfo.sub_mask);

	#if OPEN_WEB_DNS_SETTING
    fprintf(stderr,"webSetNetworkCallBack len:%d, gateway -> %s \r\n",strlen(configInfo.gateway), configInfo.gateway);
    fprintf(stderr,"webSetNetworkCallBack len:%d, dns1 -> %s \r\n",strlen(configInfo.dns1), configInfo.dns1);
    fprintf(stderr,"webSetNetworkCallBack len:%d, dns2 -> %s \r\n",strlen(configInfo.dns2), configInfo.dns2);
	#endif
    
}


static void webgetWifiListCallBack(WEB_AP_INFO *wifiListInfo)
{
    char* signal_strength = "good";
    char* ap_name = "TP-LINK_LIN";
    char* ip_addr = "192.168.1.11";
    int counter = 0;
    
    if(NULL == wifiListInfo)
    {
        fprintf(stderr,"The para is error!\r\n");

         return;
    }

    for(counter = 0; counter < AP_NUM_MAX; counter++)
    {
        memcpy(wifiListInfo[counter].ssid, ap_name, strlen(ap_name)+1);
        wifiListInfo[counter].secure_mode = WEB_SECURE_WPA2;
        memcpy(wifiListInfo[counter].security, "WPA2PSK/AES", strlen("WPA2PSK/AES")+1);
        wifiListInfo[counter].signal_strength = 80;
    }
        
    fprintf(stderr,"webgetWifiListCallBack \r\n");
}


static void webConnectApCallBack(WEB_WIFI_SSID_PW wifiInfo)
{
    if(WEB_SECURE_NONE == wifiInfo.secure_mode)
    {
        fprintf(stderr,"webConnectApCallBack secure_mode -> WEB_SECURE_NONE \r\n");

        fprintf(stderr,"webConnectApCallBack ssid -> %s \r\n",wifiInfo.ssid);
    }
    else
    {
        fprintf(stderr,"webConnectApCallBack secure_mode -> %d \r\n",wifiInfo.secure_mode);
    
        fprintf(stderr,"webConnectApCallBack ssid -> %s \r\n",wifiInfo.ssid);
        fprintf(stderr,"webConnectApCallBack pw -> %s \r\n",wifiInfo.pw);
    }

    sleep(5);
 //   tonlyweb_server_response_status(WEB_CMD_CONNECT_WIFI, WEB_SET_CMD_SUCCESS);
}


static void webFirmwareUpgCallBack(void)
{
    fprintf(stderr,"webFirmwareUpgCallBack \r\n");

    sleep(10);
 //   tonlyweb_server_response_status(WEB_CMD_UPGRADE, WEB_SET_CMD_SUCCESS);
}


void tonlyweb_callback_init(TONLYWEB_CALLBACK_T *callback)
{
    callback->web_get_network_status = webGetNetworkStatusCallBack;
    callback->web_set_device_name = webSetDeviceNameCallBack;

    callback->web_set_network = webSetNetworkCallBack;
    callback->web_get_wifi_list = webgetWifiListCallBack;
    callback->web_connect_ap = webConnectApCallBack;
    callback->web_firmware_upg = webFirmwareUpgCallBack;

}

int main(void)
{
	tonlyweb_callback_init(&callback);

	tonlyweb_server_init(&callback);
		
	while(1)
	{
		usleep(200*1000);
	}

	return 0;
}




