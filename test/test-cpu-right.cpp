// Unit test to check '>' instruction

#include <exception>
#include <string>
#include <vector>
#include <iostream>
#include <istream>

#include "expect.h"
#include "memory.h"
#include "bofsim.h"
#include "iodev.h"
#include "config.h"

#define BUFSIZE 4096

int main() {
    Configuration cpuCfg;
    cpuCfg.cfg = { {"tl", 10},
                   {"tw", 8},
                   {"nm", 3},
                   {"sd", 1},
                   {"il", BUFSIZE}
    };
    
    /* Add Objects */
    Memory tape("tape");
    Memory acodeInstr("ainstr");
    Memory scodeInstr("sinstr");
    IODev  io("io");
    BfCpu  cpu("cpu", cpuCfg, tape, acodeInstr, scodeInstr, io);

    /* Initialize acodeInstr */
    std::vector<char> buf(BUFSIZE);
    buf.assign(BUFSIZE,'>');
    acodeInstr.LoadRaw(buf.data(), BUFSIZE);
    
    Configuration cpuregs;
    /* Do simulation */
    cpuregs = cpu.GetRegs();
    std::cout << cpuregs.Dump();
    cpu.ExecuteOneStep();
    std::cout << "After\n";
    std::cout << cpuregs.Dump();
    
//     TestExpectEqual(0xff, val, "Write(0, 0xff), Read(0) -> 0xff");

    return 0;
}
