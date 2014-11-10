CFLAGS=-std=c11 -Wall -Wfatal-errors -Werror -g -I ..
CXXFLAGS=-std=c++11 -Wall -Wfatal-errors -Werror -g -I ..
SUFF=.exe # Yes, even for Linux

ifeq ($(OS), Windows_NT) # Windows
CC = gcc
CXX = g++
else # Linux ?
CC = gcc-4.8
CXX = g++-4.8
endif

ENABLED_TESTS = \
        test-io$(SUFF)

#

DISABLED_TESTS = \
#

run: all
	./runtests.sh

all: $(ENABLED_TESTS)

*.cpp: ../*.h expect.h

test-io$(SUFF): test-io.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^


clean:
	rm *.exe
