#makefile for server

cc = g++
prom := server
dirs := $(shell find . -maxdepth 1 -type d)
dirs := $(basename $(patsubst ./%,%,$(dirs)))
srcs := ./XML/*.cpp ./user/*.cpp ./packet/*.cpp ./database/*.cpp ./XML/*.h ./user/*.h ./packet/*.h ./database/*.cpp

xmlobj := ./XML/*.o
userobj := ./user/*.o
packetobj := ./packet/*.o
databaseobj := ./database/*.o

xmlflag := -I./XML
sqlflag := -L /usr/lib64/mysql -lmysqlclient -I/usr/include/mysql



.PHONY:clean all

all: $(prom)

$(prom):$(srcs)
	@for dir in $(dirs); do \
		$(MAKE) -C $$dir; \
	done
	$(cc) $(xmlobj) $(userobj) $(packetobj) $(databaseobj) -o $@ $(sqlflag) $(xmlflag)

	
clean:
	rm -rf $(prom) *.o
	@for dir in $(dirs); do \
		$(MAKE) -C $$dir clean; \
	done
