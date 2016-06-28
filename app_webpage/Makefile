#CROSS_COMPILE = 
#CC		= $(CROSS_COMPILE)gcc
TOPDIR := $(shell pwd)

SRC_DIR			= ./
TONLY_MW_DIR = ./../
TONLY_SDK_DIR = ./../../tonly_SDK


TARGET1 := web_change_device_name.cgi
TARGET2 := index.cgi
TARGET3 := ip.cgi
TARGET4 := wifi.cgi
TARGET5 := wifi_connect.cgi
TARGET6 := upgrade_firmware_select.cgi
TARGET7 := upgrade_firmware_start.cgi
TARGET8 := upgrade_firmware_waitting.cgi
TARGET9 := wifi_connect_waitting.cgi
OBJS	:= $(shell pwd)/*.O
obj-y += web_change_device_name/
obj-y += web_home_page/
obj-y += web_set_ip/
obj-y += web_set_wifi/
obj-y += web_upgrade_firmware/


INCLUDE += -I $(shell pwd)/   
INCLUDE += -I./webpage_server
INCLUDE += -I$(TONLY_SDK_DIR)/include


LIB_PATH +=  -L$(TONLY_MW_DIR)/lib
LIB_PATH +=  -L$(TONLY_SDK_DIR)/lib

LDFLAGS += -lm      
LDFLAGS += -lpthread 
LDFLAGS += -liwpriv
LDFLAGS += -lsysipc
LDFLAGS += -lsystimer
LDFLAGS += -lsyscomm
LDFLAGS += -lrt
LDFLAGS += -lmwrap



all : 
	make -C $(shell pwd)/webpage_server 
	$(CC)  -o $(TARGET1)  $(shell pwd)/source/web_change_device_name/web_change_device_name.c	 $(shell pwd)/source/web_lib/UrlToUtf8.c  $(INCLUDE)  $(LIB_PATH) $(LDFLAGS)
	$(CC)  -o $(TARGET2)  $(shell pwd)/source/web_home_page/index.c	 $(shell pwd)/source/web_lib/UrlToUtf8.c  $(INCLUDE)	$(LIB_PATH) $(LDFLAGS)
	$(CC)  -o $(TARGET3)  $(shell pwd)/source/web_set_ip/ip.c	 $(shell pwd)/source/web_lib/UrlToUtf8.c  $(INCLUDE)	$(LIB_PATH) $(LDFLAGS)
	$(CC)  -o $(TARGET4)  $(shell pwd)/source/web_set_wifi/wifi.c	 $(shell pwd)/source/web_lib/UrlToUtf8.c  $(INCLUDE)	$(LIB_PATH) $(LDFLAGS)
	$(CC)  -o $(TARGET5)  $(shell pwd)/source/web_set_wifi/wifi_connect.c	 $(shell pwd)/source/web_lib/UrlToUtf8.c  $(INCLUDE)	$(LIB_PATH) $(LDFLAGS)
	$(CC)  -o $(TARGET6)  $(shell pwd)/source/web_upgrade_firmware/upgrade_firmware_select.c $(shell pwd)/source/web_lib/UrlToUtf8.c  $(INCLUDE)	$(LIB_PATH) $(LDFLAGS)
	$(CC)  -o $(TARGET7)  $(shell pwd)/source/web_upgrade_firmware/upgrade_firmware_start.c $(shell pwd)/source/web_lib/UrlToUtf8.c  $(shell pwd)/source/web_upgrade_firmware/cgic.c	 $(INCLUDE)	$(LIB_PATH) $(LDFLAGS)
	$(CC)  -o $(TARGET8)  $(shell pwd)/source/web_upgrade_firmware/upgrade_firmware_waitting.c $(shell pwd)/source/web_lib/UrlToUtf8.c  $(INCLUDE)	$(LIB_PATH) $(LDFLAGS)
	$(CC)  -o $(TARGET9)  $(shell pwd)/source/web_set_wifi/wifi_connect_waitting.c	 $(shell pwd)/source/web_lib/UrlToUtf8.c  $(INCLUDE)	$(LIB_PATH) $(LDFLAGS)
	cp  $(shell pwd)/$(TARGET1)     $(shell pwd)/htdocs/cgi-bin/ -rfv
	cp  $(shell pwd)/$(TARGET2)	$(shell pwd)/htdocs/ -rfv
	cp  $(shell pwd)/$(TARGET3)	$(shell pwd)/htdocs/cgi-bin/ -rfv
	cp  $(shell pwd)/$(TARGET4)	$(shell pwd)/htdocs/cgi-bin/ -rfv
	cp  $(shell pwd)/$(TARGET5)	$(shell pwd)/htdocs/cgi-bin/ -rfv
	cp  $(shell pwd)/$(TARGET6)	$(shell pwd)/htdocs/cgi-bin/ -rfv
	cp  $(shell pwd)/$(TARGET7)	$(shell pwd)/htdocs/cgi-bin/ -rfv
	cp  $(shell pwd)/$(TARGET8)	$(shell pwd)/htdocs/cgi-bin/ -rfv
	cp  $(shell pwd)/$(TARGET9)	$(shell pwd)/htdocs/cgi-bin/ -rfv	
	cp  $(shell pwd)/img   $(shell pwd)/htdocs/cgi-bin/  -rfv	
	cp  $(shell pwd)/htdocs      $(shell pwd)/../bin/  -rfv
	rm  $(shell pwd)/*.cgi  -rfv

	
	@echo "make successful"
	
PHONY := clean	
clean:
	rm -f $(shell find ./source -name "*.cgi")
	rm -rfv  ./htdocs/cgi-bin/*
	rm -rfv ./webpage_server/*.o 
	rm -rfv ./webpage_server/*.so	
	echo "Clean OK"	

