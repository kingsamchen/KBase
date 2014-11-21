/*
 @ Kingsley Chen
*/

#include "kbase\command_line.h"

namespace kbase {

// TODO: set up ctor for CommandLine.
Lazy<CommandLine> CommandLine::current_process_cmdline_([]() {});

}