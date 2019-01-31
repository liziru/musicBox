CC = aarch64-linux-gnu-c++
CFLAGS = #-O2
DBUG = -g
INCLUDE = -I../out/include/ -I../out/include/alize-core/include/ -I.
LIBS = -llog4cpp
LD_FLAG = -L../out/lib
SRCS = $(wildcard *.c)  
OBJS = $(patsubst %c, %o, $(SRCS))

.PHONY : clean
all: $(OBJS)

.c.o:
	$(CC) $(DBUG) -c $(CFLAGS)  $(INCLUDE) $<  
.cpp.o:
	$(CC) $(DBUG) -c $(CFLAGS)  $(INCLUDE) $<  

clean :
	-rm -f $(OBJS)

