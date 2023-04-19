CC  = arm-himix100-linux-gcc
CXX = g++

CXXFLAGS  += -g -Wall -Werror -pthread
INCLUDES += -I 
LFLAGS += -L 
LIBS = -lpthread 

all : send rec

#SRC_PATH := src/
#CXX_SRC = ${wildcard $(SRC_PATH)/*.c}

CC_SRC = ${wildcard rec.c}

CXX_SRC =${wildcard send.c}



TARGET_SEND=send
$(TARGET_SEND): $(CXX_SRC)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(INCLUDES) $(LFLAGS) $(LIBS)


TARGET=rec
$(TARGET): $(CC_SRC)
	$(CC) -o $@ $^ $(CXXFLAGS) $(INCLUDES) $(LFLAGS) $(LIBS)  



clean:
	rm -f *.so *.o $(TARGET)  $(TARGET_SEND)

