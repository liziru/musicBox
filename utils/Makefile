CC = aarch64-linux-gnu-c++
CFLAGS = -std=c++11 #-O2
DBUG = -g
INCLUDE = -I../out/include/ -I../out/include/alize-core/include/ -I.. -I.
LIBS = 
LD_FLAG = -L../out/lib
SRCS = $(wildcard *.cpp)  
OBJS = $(patsubst %cpp, %o, $(SRCS))

.PHONY : clean
all: $(OBJS)

.c.o:
	$(CC) $(DBUG) -c $(CFLAGS)  $(INCLUDE) $<  
.cpp.o:
	$(CC) $(DBUG) -c $(CFLAGS)  $(INCLUDE) $<  

clean :
	-rm -f $(OBJS)

