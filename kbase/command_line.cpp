/*
 @ Kingsley Chen
*/

#include "kbase\command_line.h"

#include <Windows.h>

#include "kbase\error_exception_util.h"
#include "kbase\strings\string_util.h"

namespace {

using kbase::CommandLine;
using ArgList = CommandLine::ArgList;

void AppendSwitchesAndArguments(CommandLine* cmdline, const ArgList& argv)
{

}

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

CommandLine::CommandLine(int argc, const CharType* const* argv)
    : argv_(1), last_not_args_(argv_.begin())
{
    ParseFromArgv(argc, argv);
}

CommandLine::CommandLine(const std::vector<StringType>& argv)
    : argv_(1), last_not_args_(argv_.begin())
{
    ParseFromArgv(argv);
}

CommandLine::CommandLine(const StringType& cmdline)
    : argv_(1), last_not_args_(argv_.begin())
{
    ParseFromString(cmdline);
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

void CommandLine::ParseFromString(const StringType& cmdline)
{
    StringType sanitized_cmdline_str;
    TrimString(cmdline, L" \t", &sanitized_cmdline_str);
    if (sanitized_cmdline_str.empty()) {
        return;
    }

    int argc = 0;
    wchar_t** argv = nullptr;
    argv = CommandLineToArgvW(sanitized_cmdline_str.c_str(), &argc);
    ThrowLastErrorIf(!argv, "Failed to parse command line string");

    ParseFromArgv(argc, argv);
    LocalFree(argv);
}

void CommandLine::ParseFromArgv(int argc, const CharType* const* argv)
{
    std::vector<StringType> bunched_argv(argv, argv + argc);
    ParseFromArgv(bunched_argv);
}

void CommandLine::ParseFromArgv(const ArgList& argv)
{
    if (argv.empty()) {
        return;
    }

    // Anyway, we start from scratch.
    argv_ = Argv(1);
    last_not_args_ = argv_.begin();
    switches_.clear();

    SetProgram(FilePath(argv[0]));
    AppendSwitchesAndArguments(this, argv);
}

}   // namespace kbase