// Unit test to check Memory

#include <exception>
#include <string>
#include <iostream>
#include <istream>

#include "memory.h"
#include "expect.h"

int main() {
    Memory dev("dev");
    my_uint128_t val{0};
    
    val = (uint8_t)dev.Read(0);
    TestExpectEqual(0, val, "Read(0) -> 0");
    val = (uint8_t)dev.Read(100);
    TestExpectEqual(0, val, "Read(100) -> 0");
    val = (uint8_t)dev.Read(100000);
    TestExpectEqual(0, val, "Read(100000) -> 0");

    dev.Write(0, 0xff);
    val = (uint8_t)dev.Read(0);
    TestExpectEqual(0xff, val, "Write(0, 0xff), Read(0) -> 0xff");

    dev.Write(100, 0xbb);
    val = (uint8_t)dev.Read(100);
    TestExpectEqual(0xbb, val, "Write(100, 0xbb), Read(100) -> 0xbb");
    
    dev.Write(100000, 0x01);
    val = (uint8_t)dev.Read(100000);
    TestExpectEqual(0x01, val, "Write(100000, 0x01), Read(0) -> 0x01");
    return 0;
}
