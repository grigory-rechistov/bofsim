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


#include "bofsim.h"
#include "memory.h"

steps_cycles_t BfCpu::Execute(cycle_t max_cycles) {
    cycle_t i = 0;
    for (i = 0; i < max_cycles; i++)
        ExecuteOneStep();
    return {max_cycles, max_cycles};
}

steps_cycles_t BfCpu::ExecuteOneStep() {
    
    if (sr.mode == HaltMode) {// processor is disabled
        info(4, "CPU is disabled");
        return {0,1};
    }
    
    char opcode{0};
    /* Fetch */
    switch (sr.mode) {
    case ApplicationMode:
        opcode = dynamic_cast<MemoryIface&>(acode).Read(pc);
        break;
    case SupervisorMode:
        opcode = dynamic_cast<MemoryIface&>(scode).Read(pc);
        break;
    default:
        error("Unsupported processor mode");
        break;
    }
        
    
    /* Decode and Execute */
    info(4, std::string("Opcode read ") + std::string(1, opcode));
    switch (opcode) {
    case '\0':
        break;
    default:
        break;
    }
    pc +=1;
    
    /* Advance PC */
    
    return {1,1};
} // ExecuteOneStep
    
