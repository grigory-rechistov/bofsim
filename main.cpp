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
#include "optionparser.h"

int main(int argc, char** argv) {
    cycle_t steps = 1;
    
    /* Parse command line options */
    enum  optionIndex {UNKNOWN, HELP, STEPS};
    const option::Descriptor usage[] = {
        {UNKNOWN, 0, "" , ""     , option::Arg::None, "Usage: bofsim [--help] [--steps=steps] \n\n"
                                                      "Options:" },
        {HELP,    0, "h", "help" , option::Arg::None, "  --help      Print usage and exit." },
        {STEPS,   0, "s", "steps", option::Arg::Optional, "  --steps, -s Exit after specified simulation steps." },
        {0,0,0,0,0,0}
    };

    argc-=(argc>0); argv+=(argc>0); // skip program name argv[0] if present
    option::Stats  stats(usage, argc, argv);
    option::Option options[stats.options_max], buffer[stats.buffer_max];
    option::Parser parse(usage, argc, argv, options, buffer);

    if (parse.error())
        return 1;
    if (options[UNKNOWN]) {
        std::cerr << "Unknown option: " << options[UNKNOWN].name << std::endl;
        option::printUsage(std::cout, usage);
        return 1;
    }
    if (options[HELP]) {
        option::printUsage(std::cout, usage);
        return 0;
    }
    if (options[STEPS]) {
        if (!options[STEPS].arg) {
            std::cerr << "Number of steps cannot be zero or empty.\n";
            option::printUsage(std::cout, usage);
            return 1;
        }
//         std::cout << options[STEPS].arg << "\n";
        steps = std::stoi(options[STEPS].arg);
    }
    
    /* Prepare architectural configuration */
    Configuration cpuCfg;
    cpuCfg.cfg = { {"tl", 9999},
                   {"tw", 8},
                   {"nm", 3},
                   {"sd", 16},
                   {"il", 1024}
    };
    
    /* Add Objects */
    Memory tape("tape");
    Memory amodeInstr("ainstr");
    Memory smodeInstr("sinstr");
    IODev  io("io");
    BfCpu  cpu("cpu", cpuCfg, tape, amodeInstr, smodeInstr, io);

    /* Load program */
    
    /* Simulate */
    for (cycle_t step = 0; step < steps; step++) {
        cpu.ExecuteOneStep();
    }
    
    return 0;
}

