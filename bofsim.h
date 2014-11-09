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

#ifndef BOFSIM_H_
#define BOFSIM_H_

#include <cstdint>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <exception>
#include <string>

typedef uint64_t address_t; // TODO make it 128 bit wide
typedef uint64_t my_uint128_t; // TODO make it 128 bit wide
typedef uint64_t cycle_t;


class Log { // TODO move to log.h
    // TODO add log levels
public:
    void info(int level, const std::string msg) { std::cout << msg << std::endl;}
    void error(const std::string msg) { 
        std::cout << msg << std::endl;
        std::exception e(/*msg.c_str()*/);
        throw e;
    }
};

class SimObject: public Log { // TODO move to object.h
private:
    const std::string name;
    SimObject(); // forbidden
public:
    SimObject(const std::string _name): name(_name) {
        info(4, std::string("Creating object ") + name );
    };
//     SimObject(const char *_name): name(std::string(_name)) {
//         info(4, std::string("Creating object ") + name );
//     };
    
    const std::string & getName() {return this->name;};
    
};

class Configuration { // TODO move to config.h
public:
    std::unordered_map<std::string, my_uint128_t> cfg; // no point in hiding so far
    auto Get(const std::string &key) const {return cfg.at(key);};
    void Set(const std::string &key, my_uint128_t val){cfg[key] = val; };
};

typedef enum {
    ApplicationMode = 0,
    SupervisorMode  = 1,
    HaltMode        = 2,
} processor_mode_t;

struct status_register_t {
    uint8_t opcode;
    uint8_t tape;
    processor_mode_t mode;
    
    uint64_t val() {
        return ((uint64_t)opcode <<  0) |
               ((uint64_t)tape   <<  8) |
               ((uint64_t)mode   << 16);
    }
    
    status_register_t(): opcode(0), tape(0), mode(ApplicationMode) {};
    status_register_t(uint64_t value): 
        opcode(value        & 0xff), 
        tape( (value >>  8) & 0xff),
        mode( processor_mode_t((value >> 16) & 0xff)) {};
};

class BfCpu: public SimObject {
    
    SimObject &tape_mem;
    SimObject &application_mem;
    SimObject &supervisor_mem;

    /* Arch State */
    address_t pc;
    address_t inactive_pc;
    address_t tp;
    address_t sp;
    address_t inactive_sp;
    
    status_register_t sr;
    
    my_uint128_t sk;
    
    /* Configuration state */
    int tl; // tape length
    int tw; // tape width
    int nm; // number of processor modes;
    my_uint128_t sd; // stack depth
    my_uint128_t il; // instruction memory capacity
    
    /* Stack */
    std::vector<address_t> call_stack;
public:
    BfCpu(const std::string & _name,
          const Configuration & cfg,
          SimObject & _tape_mem, 
          SimObject & _application_mem,
          SimObject & _supervisor_mem
         ): 
    SimObject(_name),
    tape_mem(_tape_mem),
    application_mem(_application_mem),
    supervisor_mem(_supervisor_mem),
    pc(0),
    inactive_pc(0),
    tp(0),
    sp(0),
    inactive_sp(0),
    sr(0),
    sk(0)
    {
        tl = cfg.Get("tl");
        if (tl != 9999 || tl < 10 || tl > 127)
            error("Bad TL value in configuration");
        if (tl != 9999)
            tl = (my_uint128_t)1 << tl;
        tw = cfg.Get("tw");
        if (tw < 8 || tw > 128 || (tw & 0x7))
            error("Bad TW value in configuration");
        nm = cfg.Get("nm");
        if (nm != 2 || nm != 3)
            error("Bad NM value in configuration");
        sd = cfg.Get("sd");
        if (sd == 0)
            error("Bad SD value in configuration");
        il = cfg.Get("il");
        if (il < 32)
            error("Bad IL value in configuration");
        call_stack.resize(this->sd);
    }
    
    cycle_t ExecuteOneStep();
    
    cycle_t Execute(cycle_t max_cycles);
    
}; //BfCpu

#endif // BOFSIM_H_
