# Makefile for bofsim

CXX=g++-4.8
CC=g++-4.8
CXXFLAGS=  --std=c++11 -Wall -Wfatal-errors -Werror -std=c++1y # http://stackoverflow.com/questions/21258062/warning-with-automatic-return-type-deduction-why-do-we-need-decltype-when-retur

.PHONY: test
all: bofsim

clean: 
	rm -rf bofsim *.o

*.o : *.h # This rule is lame, but it is better than nothing

bofsim: main.o bofsim.o memory.o

test:
	$(MAKE) -C test run

clean-test:
	$(MAKE) -C test clean
