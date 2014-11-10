/* Copyright (c) 2014, Grigory Rechistov
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, 
OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; 
OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef MEMORY_H_
#define MEMORY_H_

#include <vector>

#include "inttypes.h"
#include "object.h"
#include "bofsim.h"
#include "log.h"

class MemoryIface {
public:
    virtual my_uint128_t Read(address_t addr) = 0;
    virtual void Write(address_t addr, my_uint128_t val) = 0;
    virtual void LoadRaw(const char* buf, size_t len) = 0;
};

class Memory: public SimObject, public MemoryIface {
    
    std::vector<char> data;
public:
    Memory(const std::string _name): SimObject(_name) {};

    virtual my_uint128_t Read(address_t addr) {
        info(1, "Not implemented");
        return 0;
    }
    
    virtual void Write(address_t addr, my_uint128_t val) {
        info(1, "Not implemented");
    }
    
    virtual void LoadRaw(const char* buf, size_t len) {
        data.assign(len, *buf);
    }
};

#endif // MEMORY_H_
