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

#include <vector>
#include <iostream>
#include <exception>
#include <string>
#include <cassert>

#include "inttypes.h"
#include "object.h"
#include "log.h"
#include "config.h"

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
    
    SimObject &tape;
    SimObject &acode;
    SimObject &scode;
    SimObject &iodev;

    /* Arch State */
    address_t pc;
    address_t inactive_pc;
    address_t tp;
    address_t sp;
    address_t inactive_sp;
    
    status_register_t sr;
    
    my_uint128_t sk;
    my_uint128_t inactive_sk;
    
    /* Configuration state */
    unsigned tl; // tape length
    int tw; // tape width
    my_uint128_t tape_mask; // pre-calculated maximum value for tape cell
    int nm; // number of processor modes;
    my_uint128_t sd; // stack depth
    my_uint128_t il; // instruction memory capacity
    
    /* Stack */
    std::vector<address_t> call_stack;
public:
    BfCpu(const std::string & _name,
          const Configuration & cfg,
          SimObject & _tape, 
          SimObject & _acode,
          SimObject & _scode,
          SimObject & _io
         ): 
    SimObject(_name),
    tape(_tape),
    acode(_acode),
    scode(_scode),
    iodev(_io),
    pc(0),
    inactive_pc(0),
    tp(0),
    sp(0),
    inactive_sp(0),
    sr(0),
    sk(0),
    inactive_sk(0)
    {
        tl = cfg.Get("tl");
        if ((tl < 10 || tl > 127) && tl != 9999)
            error("Bad TL value in configuration");
        if (tl != 9999)
            tl = (my_uint128_t)1 << tl;
        tw = cfg.Get("tw");
        if (tw < 8 || tw > 128 || (tw & 0x7))
            error("Bad TW value in configuration");
        tape_mask = (my_uint128_t(1) << tw) - 1;
        assert(tape_mask != 0);
        nm = cfg.Get("nm");
        if (nm != 2 && nm != 3)
            error("Bad NM value in configuration");
        sd = cfg.Get("sd");
        if (sd == 0)
            error("Bad SD value in configuration");
        il = cfg.Get("il");
        if (il < 32)
            error("Bad IL value in configuration");
        call_stack.resize(this->sd);
    }
    
    /* RETURN: [steps, cycles] actually done 
     * [1, 1] - all ok,
     * [1, >1] - ok, but a long instruction encountered
     * [0, 1] - processor disabled
     * [0, >1] - unused currently
     */
    steps_cycles_t ExecuteOneStep();
    
    /* IN: maximum steps to do 
       RETURN: [steps, cycles] actually done */
    steps_cycles_t Execute(step_t max_steps);

    void ProcessViolation(uint8_t opc, uint8_t tap);
    void ReturnToApplicationMode();

}; //BfCpu

#endif // BOFSIM_H_
