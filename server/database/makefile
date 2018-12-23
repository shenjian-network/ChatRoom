#makefile for database

cc = g++
srcs := $(wildcard *.cpp)
obj := $(patsubst %.cpp, %.o, $(srcs))
deps := $(wildcard *.h)

sqlflag := -L /usr/lib64/mysql -lmysqlclient -I/usr/include/mysql
xmlflag := -I../XML

all:$(obj)
	
%.o : %.cpp $(deps)
	$(cc) -c $< $(sqlflag) $(xmlflag)


.PHONY: clean 

clean: 
	rm -rf $(prom) *.o 

