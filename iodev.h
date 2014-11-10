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

#ifndef IODEV_H_
#define IODEV_H_

#include <iostream>
#include <fstream>

#include "inttypes.h"
#include "object.h"
#include "log.h"

class IOIface {
public:
    virtual my_uint128_t Read() = 0;
    virtual void Write(my_uint128_t val) = 0;
};

class IODev: public SimObject, public IOIface {
    std::ifstream fcin;
    std::ofstream fcout;
    std::istream &cin;
    std::ostream &cout;
    
public:
    IODev(const std::string _name): 
        SimObject(_name),
        fcin(),
        fcout(),
        cin(std::cin),
        cout(std::cout)
        {};
    IODev(const std::string _name,
          const std::string _inname,
          const std::string _outname
    ): SimObject(_name),
       fcin(_inname),
       fcout(_outname, std::ios::out | std::ios::trunc),
       cin(fcin),
       cout(fcout)
       {};

    virtual ~IODev() {
        if (fcin.is_open())  fcin.close();
        if (fcout.is_open()) fcout.close();
    }
        
    virtual my_uint128_t Read() {
        char val;
        cin.get(val);
        return val;
    }
    
    virtual void Write(my_uint128_t val) {
        // TODO parsametrize this to output either ASCII or hex or dec etc
        char v = static_cast<char>(val);
        cout << v;
    }    
};

#endif // IODEV_H_
