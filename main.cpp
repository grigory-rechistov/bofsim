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

#include <iostream>
#include <cassert>

#include "bofsim.h"
#include "memory.h"
#include "iodev.h"
#include "optionparser.h"

typedef struct cli_options {
    step_t steps = 1;
    const char *scode_file;
    const char *acode_file;
    const char *tape_file;    
} cli_options_t;

/* Parses command-line options, exits program on error. 
 * Returns options on success.
 */
static cli_options_t parse_argv(int argc, char** argv) {
    cli_options_t result = {};
    /* Parse command line options */
    enum  optionIndex {UNKNOWN, HELP, STEPS, ACODE, SCODE, TAPE};
    const option::Descriptor usage[] = {
        {UNKNOWN, 0, "" , ""     , option::Arg::None, 
                "Usage: bofsim [--help] [--steps=steps] "
                "[-scode=file] [--tape=file] --acode=file"
                "\n\n"
                "Options:" },
        {HELP,    0, "h", "help" , option::Arg::None, 
                "  --help       Print usage and exit." },
        {STEPS,   0, "s", "steps", option::Arg::Optional, 
                "  --steps, -s  Exit after specified simulation steps." },
        {SCODE,   0, "", "scode", option::Arg::Optional, 
                "  --scode,     File with supervisor mode program." },
        {TAPE,    0, "", "tape", option::Arg::Optional, 
                "  --tape,      File with initial tape state." },
        {ACODE,   0, "", "acode", option::Arg::Optional, 
                "  --scode,     File with application mode program." },
        {0,0,0,0,0,0}
    };

    argc -= (argc > 0); 
    argv += (argc > 0); // Skip program name argv[0] if present
    option::Stats  stats(usage, argc, argv);
    option::Option options[stats.options_max], buffer[stats.buffer_max];
    option::Parser parse(usage, argc, argv, options, buffer);

    if (parse.error())
        exit(1);
    if (options[UNKNOWN]) {
        std::cerr << "Unknown option: " << options[UNKNOWN].name << std::endl;
        option::printUsage(std::cout, usage);
        exit(1);
    }
    if (options[HELP]) {
        option::printUsage(std::cout, usage);
        exit(0);
    }
    if (options[STEPS]) {
        if (!options[STEPS].arg) {
            std::cerr << "Number of steps cannot be zero or empty.\n";
            option::printUsage(std::cout, usage);
            exit(1);
        }
        result.steps = std::stoi(options[STEPS].arg);
    }
    if (options[SCODE]) {
        if (!options[SCODE].arg) {
            std::cerr << "Empty supervisor code file name.\n";
            option::printUsage(std::cout, usage);
            exit(1);
        }
        result.scode_file = options[SCODE].arg;
    }
    if (options[TAPE]) {
        if (!options[TAPE].arg) {
            std::cerr << "Empty tape file name.\n";
            option::printUsage(std::cout, usage);
            exit(1);
        }
        result.tape_file = options[TAPE].arg;
    }
    if (options[ACODE]) {
        if (!options[ACODE].arg) {
            std::cerr << "Empty application code file name.\n";
            option::printUsage(std::cout, usage);
            exit(1);
        }
        result.acode_file = options[ACODE].arg;
    }
    
//     /* Handle non-positional sarguments */
//     for (int i = 0; i < parse.nonOptionsCount(); ++i) {
//         std::cout << "Non-option #" << i << ": " << parse.nonOption(i) << "\n";
//     }
    return result;
} // parse_argv()

int main(int argc, char** argv) {
    cli_options_t r = parse_argv(argc, argv);
    
    /* Prepare architectural configuration */
    /* TODO allow to load it from file or command-line */
    Configuration cpuCfg;
    cpuCfg.cfg = { {"tl", 9999},
                   {"tw", 8},
                   {"nm", 3},
                   {"sd", 16},
                   {"il", 4096}
    };
    
    /* Add Objects */
    Memory tape("tape");
    Memory acodeInstr("ainstr");
    Memory scodeInstr("sinstr");
    IODev  io("io");
    BfCpu  cpu("cpu", cpuCfg, tape, acodeInstr, scodeInstr, io);

    /* Buffer to be used to load Memory's contents */
    std::streampos bufsize{0};
    char*  buf = nullptr;
    
    /* Load application program data from file */
    if (not r.acode_file) {
        std::cerr << "Required application code file is not specified!\n";
        return 1;
    } else {
        std::ifstream acode_stream(r.acode_file, std::ifstream::ate); // open at end
        bufsize = acode_stream.tellg(); // get size
        if (bufsize == -1) {
            std::cerr << "Cannot determine file length for " 
                    << r.acode_file << std::endl;
            return 1;
        }
    //     std::cerr << "size = " << bufsize << "\n";
        acode_stream.seekg(0, acode_stream.beg); // rewind to the beginning
        // Check if file is longer than instruction memory, warn user if so
        if (cpuCfg.Get("il") < (my_uint128_t)bufsize) {
            std::cerr << "Application code file size is bigger than "
                        " confgured application memory size " << cpuCfg.Get("il") <<
                        ", will truncate code\n";
            bufsize = (size_t)cpuCfg.Get("il");
        }
        if (bufsize > 0) {
            buf = new char[bufsize];
            acode_stream.read(buf, bufsize);
            assert(acode_stream.tellg() == bufsize);
            acodeInstr.LoadRaw(buf, bufsize); // not very efficient double buffering
            delete[] buf;
        }
        acode_stream.close();
        std::cerr << "Acode:\n" << acodeInstr.Dump() << std::endl;
    }
    
    /* Load supervisor program data from file */
    if (not r.scode_file) {
        std::cerr << "Supervisor code file is not specified, leaving empty\n";
    } else {
        std::ifstream scode_stream(r.scode_file, std::ifstream::ate); // open at end
        bufsize = scode_stream.tellg(); // get size
        if (bufsize == -1) {
            std::cerr << "Cannot determine file length for " 
                    << r.scode_file << std::endl;
            return 1;
        }
    //     std::cerr << "size = " << bufsize << "\n";
        scode_stream.seekg(0, scode_stream.beg); // rewind to the beginning
        // Check if file is longer than instruction memory, warn user if so
        if (cpuCfg.Get("il") < (my_uint128_t)bufsize) {
            std::cerr << "Supervisor code file size is bigger than "
                        " confgured supervisor memory size " << cpuCfg.Get("il") <<
                        ", will truncate code\n";
            bufsize = (size_t)cpuCfg.Get("il");
        }
        if (bufsize > 0) {
            buf = new char[bufsize];
            scode_stream.read(buf, bufsize);
            assert(scode_stream.tellg() == bufsize);
            scodeInstr.LoadRaw(buf, bufsize); // not very efficient double buffering
            delete[] buf;
        }
        scode_stream.close();
        std::cerr << "Scode:\n" << scodeInstr.Dump() << std::endl;
    }
    
    /* Load tape data from file */
    if (not r.tape_file) {
        std::cerr << "Tape file is not specified, leaving empty\n";
    } else {
        std::ifstream tape_stream(r.tape_file, std::ifstream::ate); // open at end
        bufsize = tape_stream.tellg(); // get size
        if (bufsize == -1) {
            std::cerr << "Cannot determine file length for " 
                    << r.tape_file << std::endl;
            return 1;
        }
    //     std::cerr << "size = " << bufsize << "\n";
        tape_stream.seekg(0, tape_stream.beg); // rewind to the beginning
        // Check if file is longer than instruction memory, warn user if so
        my_uint128_t real_tl = cpuCfg.Get("tl") == 9999 ? 9999: 
                                        (my_uint128_t)1 << cpuCfg.Get("tl");
        if (real_tl < (my_uint128_t)bufsize) {
            std::cerr << "Tape file size is bigger than "
                        " confgured tape length " << real_tl <<
                        ", will truncate.\n";
            bufsize = (size_t)real_tl;
        }
        if (bufsize > 0) {
            buf = new char[bufsize];
            tape_stream.read(buf, bufsize);
            assert(tape_stream.tellg() == bufsize);
            tape.LoadRaw(buf, bufsize); // not very efficient double buffering
            delete[] buf;
        }
        tape_stream.close();
        std::cerr << "Tape:\n" << tape.Dump() << std::endl;
    }
    /* Simulate */
    for (step_t step = 0; step < r.steps; step++) {
        cpu.ExecuteOneStep();
    }
    
    return 0;
}

