CXX = g++
COBJS = main.o
CXXFLAGS = -I ./include/ -Wall
EDLDFLAGS = -lsi446x -lpthread -lm
TARGET = faux.out

all: $(COBJS)
	$(CXX) $(CXXFLAGS) $(COBJS) -o $(TARGET) $(EDLDFLAGS)
	sudo ./$(TARGET)

%.o: %.c
	$(CXX) $(CXXFLAGS) -o $@ -c $<

.PHONY: clean

clean:
	$(RM) *.out
	$(RM) *.o
	$(RM) src/*.o