// Unit test to check IO

#include <exception>
#include <string>
#include <iostream>
#include <istream>

#include "iodev.h"
#include "expect.h"

int main() {
    
    IODev dev("dev", "test-io-stdin", "test-io-stdout");
    
    my_uint128_t val = dev.Read();
    TestExpectEqual('a', val, "Input is 'a'");
    dev.Write('b');
    dev.~IODev(); // Close file
    /* Let's check what we just wrote */
    std::ifstream out("test-io-stdout");
    char char_val;
    out.get(char_val);
    TestExpectEqual('b', char_val, "Output is 'b'");
    return 0;
}
