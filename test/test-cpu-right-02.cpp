// Unit test to check '>' instruction violation

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
    
    /* Initialize Tape */
    buf[1023] = 'a';
    tape.LoadRaw(buf.data(), 1024);
    
    // Adjust TP to point to end of tape
    cpu.SetRegister("tp", 1023);
    
    Configuration cpuregs;
    /* Do simulation */
    
    std::cout << "Before:\n" << cpu.GetRegs().Dump();
    my_uint128_t old_tp = cpu.GetRegs().cfg["tp"];
    cpu.ExecuteOneStep();
    std::cout << "After:\n" << cpu.GetRegs().Dump();
    cpuregs = cpu.GetRegs();
    TestExpectEqual(old_tp, cpuregs.cfg["tp"], "TP hasn't changed");
    TestExpectEqual(0x0, cpuregs.cfg["pc"], "PC is reset");
    TestExpectEqual(0x1613E, cpuregs.cfg["sr"], "SR is correct");
    
    return 0;
}
