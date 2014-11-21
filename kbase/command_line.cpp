/*
 @ Kingsley Chen
*/

#include "kbase\command_line.h"

#include <Windows.h>

#include "kbase\strings\string_util.h"

namespace {

using kbase::CommandLine;

}   // namespace

namespace kbase {

Lazy<CommandLine> CommandLine::current_process_cmdline_([]() {
    return new CommandLine(GetCommandLineW());
});

CommandLine::CommandLine(const FilePath& program)
    : argv_(1), last_not_args_(argv_.begin())
{
    SetProgram(program);
}

CommandLine::CommandLine(const StringType& cmdline)
{
    (cmdline);
}

// static
const CommandLine& CommandLine::ForCurrentProcess()
{
    return current_process_cmdline_.value();
}

FilePath CommandLine::GetProgram()  const
{
    return FilePath(argv_.front());
}

void CommandLine::SetProgram(const FilePath& program)
{
    TrimString(program.value(), L" \t", &argv_.front());
}

}   // namespace kbase