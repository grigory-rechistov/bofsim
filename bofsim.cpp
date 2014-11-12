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

void BfCpu::ProcessViolation(uint8_t opc, uint8_t tap) {
    if (sr.mode != ApplicationMode) {
        info(2, "Violation in non-application mode, going to Halt");
        sr.mode = HaltMode;
        return;
    }
    inactive_pc = pc;
    pc = 0;
    inactive_sp = sp;
    sp = 0;
    inactive_sk = sk;
    sk = 0;
    sr.opcode = opc;
    sr.tape = tap;
    sr.mode = SupervisorMode;
}

void BfCpu::ReturnToApplicationMode() {
    info(1, "ReturnToApplicationMode not implemented");
    
}

steps_cycles_t BfCpu::Execute(cycle_t max_cycles) {
    cycle_t i = 0;
    for (i = 0; i < max_cycles; i++)
        ExecuteOneStep();
    return {max_cycles, max_cycles};
}

steps_cycles_t BfCpu::ExecuteOneStep() {
    
    enum class ExecuteResult {
        Regular = 0,
        ControlFlow,
        Violation,
        Nop,
        Halt,
    };
    
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
        error("Unsupported processor mode for execution");
        break;
    }
    info(4, std::string("Opcode read ") + std::string(1, opcode));
        
    
    /* Decode and Execute */
    ExecuteResult res = ExecuteResult::Regular;
    cycle_t spent = 1; // default value for executing instructions.
    
    switch (opcode) {
    case '\0':
        sr.mode = HaltMode;
        res = ExecuteResult::Halt;
        break;
    case '>':
        res = ExecuteResult::Regular;
        break;
    case '<':
        res = ExecuteResult::Regular;
        break;
    case '+':
        res = ExecuteResult::Regular;
        break;
    case '-':
        break;
        res = ExecuteResult::Regular;
    case '[':
        res = ExecuteResult::ControlFlow;
        break;
    case ']':
        res = ExecuteResult::ControlFlow;
        break;
    case '.':
        res = ExecuteResult::Regular;
        break;
    case ',':
        res = ExecuteResult::Regular;
        break;
    default:
        res = ExecuteResult::Nop;
        spent = 0; // Not cycles are spent processing comments
        break;
    }
    
    /* Advance PC depending on instruction outcome */
    
    switch(res) {
    case ExecuteResult::Regular:
    case ExecuteResult::Nop:
        pc +=1;
    case ExecuteResult::ControlFlow:
    case ExecuteResult::Violation:
    case ExecuteResult::Halt:
        // PC is already changed correctly.
        break;
    default:
        pc +=1;
        break;
    }
    return {1, spent};
} // ExecuteOneStep
    
