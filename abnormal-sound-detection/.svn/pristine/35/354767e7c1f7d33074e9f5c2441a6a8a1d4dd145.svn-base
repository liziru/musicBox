BIN_NAME = sounder
CC = aarch64-linux-gnu-c++
CFLAGS = -O2
INCLUDE = -I./out/include/ -I.
LD_FLAG = -L./out/lib
LIBS = -lasound -lpthread -lTwirlingCapture
OBJS = sounder.o ring_buf.o
ADDR=pi@192.168.0.112
.PHONY : clean
all: $(BIN_NAME)

.c.o:
	$(CC) -c $(CFLAGS)  $(INCLUDE) $<  
.cpp.o:
	$(CC) -c $(CFLAGS)  $(INCLUDE) $<  

$(BIN_NAME) : $(OBJS)
	$(CC) -o $(BIN_NAME) $(OBJS) $(LD_FLAG) $(LIBS) $(INCLUDE)
test_fifo:
	g++ -o test_fifo ./test_fifo.cpp ./fifo.cpp $(LD_FLAG) $(INCLUDE) -lpthread
	#scp ./test_fifo pi@192.168.0.112:~/workspace/

alsa-record-example : $(OBJS)
	cc
clean :
	-rm -f $(OBJS) $(BIN_NAME)
install: all
	scp ./sounder $(ADDR):~/workspace
	#scp ./run.sh $(ADDR):~/workspace
	#scp ./sounder_script $(ADDR):~/workspace
	#scp ./out/lib/libTwirlingCapture.so $(ADDR):~/workspace
