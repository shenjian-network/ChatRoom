#makefile for user

cc = g++
srcs := $(wildcard *.cpp)
obj := $(patsubst %.cpp, %.o, $(srcs))
deps := $(wildcard *.h)

xmlflag := -I../XML
databaseflag := -I../database
packetflag := -I../packet
sqlflag := -L /usr/lib64/mysql -lmysqlclient -I/usr/include/mysql


all:$(obj)
	
%.o : %.cpp $(deps)
	$(cc) -std=c++11 -c $< $(packetflag) $(xmlflag) $(databaseflag) $(sqlflag)


.PHONY: clean 

clean: 
	rm -rf $(prom) *.o 

