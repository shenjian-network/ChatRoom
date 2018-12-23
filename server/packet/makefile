#makefile for packetHead

cc = g++
srcs := $(wildcard *.cpp)
obj := $(patsubst %.cpp,  %.o, $(srcs))
deps := $(wildcard *.h)

xmlflag := -I../XML

all: $(obj) 

%.o: %.cpp $(deps)
	$(cc) -c $< $(xmlflag)


.PHONY:clean 

clean:
	rm -rf  *.o 

