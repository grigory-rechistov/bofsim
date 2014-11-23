// Unit test to check '<' instruction

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
    buf.assign(BUFSIZE,'<');
    acodeInstr.LoadRaw(buf.data(), BUFSIZE);
    
    // Adjust TP to point to some point in the middle
    cpu.SetRegister("tp", 10);
    
    Configuration cpuregs;
    /* Do simulation */
    
    std::cout << "Before:\n" << cpu.GetRegs().Dump();
    my_uint128_t old_tp = cpu.GetRegs().cfg["tp"];
    cpu.ExecuteOneStep();
    std::cout << "After:\n" << cpu.GetRegs().Dump();
    cpuregs = cpu.GetRegs();
    TestExpectEqual(old_tp-1, cpuregs.cfg["tp"], "TP has changed");
    TestExpectEqual(0x1, cpuregs.cfg["pc"], "PC is 1");
    
    return 0;
}
