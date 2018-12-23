#makefile for XML lib

cc = g++
srcs := $(wildcard *.cpp)
obj := $(patsubst %.cpp, %.o, $(srcs))
deps := $(wildcard *.h)

all:$(obj)
	
%.o : %.cpp $(deps)
	$(cc) -c $< 


.PHONY: clean 

clean: 
	rm -rf $(prom) *.o 

