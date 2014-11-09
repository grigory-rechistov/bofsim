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

#include <string>
#include <stdint>

using std::string;

typedef uint64_t cycle_t;
typedef uint64_t address_t;

typedef enum {
    NoError = 0,
    GeneralError,
} error_t;

class SimObject {
    int log_level;
    string name;
    string classname;
    clas
public:
    virtual SimObject(const string &_name) = 0;
    virtual Msg(string msg) = 0;

    
}; // SimObject

class IfaceMapping{

public:
    template<Size> error_t ReadData(address_t addr, Size &result) = 0;
    
    error_t AddMapping (SimObject &obj, address_t start, address_t length) = 0;
}

typedef (*EventHandler)(SimObject obj, void* data); // FIXME data must be better typed

// TODO abstract this away
class Event {
public:
    EventHandler handler;
    SimObject obj;
    void * data;
    cycle_t delta; // should not be here
}

class IfaceQueue {
 
public:
    virtual void DumpQueue() = 0;
    virtual void AddEvent(cycle_t delta, Event event) = 0;
    virtual Event PopEvent() = 0;
    virtual CancelEvent(const Event & match);
}

