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
#include "iodev.h"

void BfCpu::ProcessViolation(uint8_t opc, uint8_t tap) {
    if (sr.mode != ApplicationMode) {
        info(2, "Violation in non-application mode, going to Halt");
        sr.mode = HaltMode;
        return;
    }
    info(4, "Violation in application mode, switching to supervisor");
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
    
    /* Instruction execution result 
     * Currently affects whether PC will be advanced.
     */
    enum class ExecuteResult {
        Regular = 0,
        ControlFlow,
        Violation,
        Skipping,
        Nop,
        Halt,
    };
    
    if (sr.mode == HaltMode) {// processor is disabled
        info(4, "CPU is disabled");
        return {0,1};
    }
    
    char opcode{0};
    my_uint128_t tape_val{0};
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
    
    if ((sk > 0) and (opcode != '[' and
                      opcode != ']' and
                      opcode != '\0')
    ) {
        info(4, "Skipping...");
        res = ExecuteResult::Skipping;
    } else switch (opcode) {
    case '\0':
        sr.mode = HaltMode;
        res = ExecuteResult::Halt;
        break;
    case '>':
        if (tp >= tl-1) {
            uint8_t tape8 = (uint8_t)dynamic_cast<MemoryIface&>(tape).Read(tp);
            ProcessViolation(opcode, tape8);
            res = ExecuteResult::Violation;
        } else {
            tp++;
            res = ExecuteResult::Regular;
        }
        break;
    case '<':
        if (tp == 0 ) {
            uint8_t tape8 = (uint8_t)dynamic_cast<MemoryIface&>(tape).Read(tp);
            ProcessViolation(opcode, tape8);
            res = ExecuteResult::Violation;
        } else {
            tp--;
            res = ExecuteResult::Regular;
        }
        break;
    case '+':
        tape_val = dynamic_cast<MemoryIface&>(tape).Read(tp);
        tape_val = (tape_val+1) & tape_mask; // increase and handle overflow
        dynamic_cast<MemoryIface&>(tape).Write(tp, tape_val);
        res = ExecuteResult::Regular;
        break;
    case '-':
        tape_val = dynamic_cast<MemoryIface&>(tape).Read(tp);
        tape_val = (tape_val-1) & tape_mask; // increase and handle overflow
        dynamic_cast<MemoryIface&>(tape).Write(tp, tape_val);
        res = ExecuteResult::Regular;
        break;
    case '[':
        tape_val = dynamic_cast<MemoryIface&>(tape).Read(tp);
        if (sk > 0) {
            sk++;
            res = ExecuteResult::Skipping;
        } else if (tape_val == 0) {
            info(2, std::string("Entering skipping mode at PC = ") + std::to_string(pc));
            sk = 1;
            res = ExecuteResult::Skipping;
        } else {
            if (sp > sd) {
                ProcessViolation(opcode, (uint8_t)tape_val);
                res = ExecuteResult::Violation;
            } else {
                info(2, std::string("Entering loop at PC = ") + std::to_string(pc));
                call_stack[sp] = pc;
                sp ++;
                res = ExecuteResult::Regular;
            }
        }
        break;
    case ']':
        tape_val = dynamic_cast<MemoryIface&>(tape).Read(tp);
        if (sk == 0) {
            if (sp == 0) {
                    ProcessViolation(opcode, (uint8_t)tape_val);
                    res = ExecuteResult::Violation;
            } else {
                sp--;
                if (tape_val != 0) {
                    pc = call_stack[sp];
                    info(2, std::string("Loop to PC = ") + std::to_string(pc));
                    res = ExecuteResult::ControlFlow;
                } else {
                    info (2, std::string("Exiting loop at PC = ") + std::to_string(pc));
                    res = ExecuteResult::Regular;
                }
            }
        } else {
            sk--;
            if (sk == 0) {
                info(2, "Leaving skipping mode");
                res = ExecuteResult::Regular;
            } else {
                res = ExecuteResult::Skipping;
            }
        }
        break;
    case '.': // output
        tape_val = dynamic_cast<MemoryIface&>(tape).Read(tp);
        dynamic_cast<IOIface&>(iodev).Write(tape_val);
        res = ExecuteResult::Regular;
        break;
    case ',': // input
        tape_val = dynamic_cast<IOIface&>(iodev).Read();
        res = ExecuteResult::Regular;
        break;
    default:
        res = ExecuteResult::Nop;
        spent = 0; // No cycles are spent processing comments
        break;
    } // switch (opcode)
    
    /* Advance PC depending on execution outcome */
    switch(res) {
    case ExecuteResult::Regular:
    case ExecuteResult::Nop:
    case ExecuteResult::Skipping:
        pc +=1;
    case ExecuteResult::ControlFlow:
    case ExecuteResult::Violation:
    case ExecuteResult::Halt:
        // PC is already changed.
        break;
    default:
        assert(0 && "Unreachable");
        break;
    }
    return {1, spent};
} // ExecuteOneStep
    
void BfCpu::SetRegister(const std::string &name, const my_uint128_t &val) {
    /* TODO add validation for val */
    if (!name.compare("pc")) {
        pc = val;
    } else if (!name.compare("tp")) {
        tp = val;
    } else {
        error(std::string("Unknown or unsupported register ") + name);
//         throw Exception
    }
    
}

