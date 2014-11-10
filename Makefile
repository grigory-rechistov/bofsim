# Makefile for bofsim

CXX=g++-4.8
CC=g++-4.8
CXXFLAGS=  --std=c++11 -std=c++1y # http://stackoverflow.com/questions/21258062/warning-with-automatic-return-type-deduction-why-do-we-need-decltype-when-retur

.PHONY: test
all: bofsim

clean: 
	rm -rf bofsim *.o

bofsim: main.o bofsim.o memory.o

test:
	$(MAKE) -C test run
