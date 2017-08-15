## make file for lib-msg

SYSROOT := /home/jack/tools/sysroots
PROJ_DIR := /home/jack/work/mdm9607-1.1/mdm9607-le-1-1_kid_watch_apps_proc/apps_proc

CC := cc

CXX := c++


LINK := $(CC) -bundle `python-config --ldflags` 
#LINK := $(CC)  -dynamiclib 
TARGET = _msg.so

CFLAGS := -Wall -O2 `python-config --cflags` -fPIC
CXXFLAGS := -std=c++11 $(CFLAGS)


INC_DIR = ../inc 
#SO_DIR  = $(SYSROOT)/mdm9607/usr/lib ./libs 


INC += -I$(INC_DIR) -I./ 

LIBS := -lpython
	

LIBS_INC :=  

#C_OBJS = $(patsubst %.c, %.o, $(wildcard *.c))
#CXX_OBJS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))

C_FILES = msg.c msg_wrap.c
C_OBJS = msg.o msg_wrap.o

all: $(TARGET)


$(TARGET):
	swig -python msg.i
	$(CC) -c $(INC) $(CFLAGS) $(C_FILES)
	$(CC) -o test_client msg_test_client.c msg.o
	$(LINK) $(INC)  $(C_OBJS) $(CXX_OBJS) $(LIBS) $(LIBS_INC) -o $(TARGET)


$(C_OBJS):%.o:%.c
	$(CC) $(INC)  $(CFLAGS) -c $^ -o $@

$(CXX_OBJS):%.o:%.cpp
	$(CXX) $(INC) $(CXXFLAGS) -c $^ -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) *.o *.h~ *.c~ *.cpp~ *.pyc 