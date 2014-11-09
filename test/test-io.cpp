// Unit test to check IO

#include <exception>
#include <string>
#include <iostream>

#include "iodev.h"
// #include "expect.h"

void TestExpectTrue(bool condition, const std::string descr) {
    std::exception e;
    if (!condition) {
        std::cerr << "Condition failed: " << descr << std::endl;
        throw e;
    }
}

void TestExpectEqual(my_uint128_t a, my_uint128_t b, const std::string descr) {
    std::exception e;
    if (a != b) {
        std::cerr << "Equalty failed: expected " << a << ", got" << b
                  << descr << std::endl;
        throw e;
    }
}


int main() {
    IODev dev("dev");
    
    my_uint128_t val = dev.Read();
    TestExpectEqual('a', val, "Input is 'a'");
    dev.Write('b');    
    return 0;
}
