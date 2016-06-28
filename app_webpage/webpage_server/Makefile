C_SRCS = webpage.c \
C_OBJS = $(patsubst %.c, %.o, $(C_SRCS))

CLI_SRCS = cli_web_server.c
CLI_OBJS = $(patsubst %.c, %.o, $(CLI_SRCS))

CLI_SRCS1 = ../source/web_lib/UrlToUtf8.c
CLI_OBJS1 = $(patsubst %.c, %.o, $(CLI_SRCS1))

LIB_SRCS = webpage.c
LIB_OBJS = $(patsubst %.c, %.o, $(LIB_SRCS))

DEFINE += 

SRC_DIR			= ./


CPPFLAGS += -I.  

LIB_PATH  += $(LDFLAGS) 

LIBS_L += -lm
LIBS_L += -lpthread
LIBS_L += -lrt
LIBS_L += -lc
LIBS_L += -lmwrap 
LIBS_L += -lsysipc
LIBS_L += -lsystimer
LIBS_L += -lsyscomm

EXE_OBJ = webserver
CLI_OBJ = cli_web_server
LIB_OBJ = libwebpage.so

all:lib ser
ser:
	$(CC)  -rdynamic -o $(CLI_OBJ) $(CLI_SRCS) $(CLI_SRCS1) $(CPPFLAGS) $(LDFLAGS) $(LIBS_L) -lwebpage
	cp $(CLI_OBJ)  ../../bin -a

lib:$(LIB_OBJ)
$(LIB_OBJ):$(LIB_OBJS)
	$(CC)  -shared -rdynamic -o $(LIB_OBJ) $(LIB_OBJS)  $(CPPFLAGS)  $(LIB_PATH)  $(LIBS_L)  
	cp $(LIB_OBJ)  ./../../lib/ -rfv
	
%.o:%.c
	$(CC) $(DEFINE) -g -c -fPIC -o $@ $< $(CPPFLAGS)

.PHONY:clean
clean:
	@-rm -f *.o  $(LIB_OBJ)
	rm $(EXE_OBJ) $(CLI_OBJ) $(LIB_OBJ) -rf
	
