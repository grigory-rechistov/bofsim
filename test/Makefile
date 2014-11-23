CFLAGS=-std=c11 -Wall -Wfatal-errors -Werror  -I ..
CXXFLAGS=-std=c++11 -Wall -Wfatal-errors -Werror  -I .. -std=c++1y # http://stackoverflow.com/questions/21258062/warning-with-automatic-return-type-deduction-why-do-we-need-decltype-when-retur
SUFF=.exe # Yes, even for Linux

ifeq ($(OS), Windows_NT) # Windows
CC = gcc
CXX = g++
else # Linux ?
CC = gcc-4.8
CXX = g++-4.8
endif

ENABLED_TESTS = \
        test-io$(SUFF) \
        test-mem$(SUFF) \
        test-cpu-right-01$(SUFF) \
        test-cpu-right-02$(SUFF) \
        test-cpu-left-01$(SUFF) \
        test-cpu-left-02$(SUFF) \


#

DISABLED_TESTS = \
#

run: all
	./runtests.sh

all: $(ENABLED_TESTS)

*.cpp: ../*.h expect.h

test-%$(SUFF): test-%.cpp ../bofsim.o
	$(CXX) $(CXXFLAGS) -o $@ $^


clean:
	rm *.exe
