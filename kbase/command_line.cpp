/*
 @ Kingsley Chen
*/

#include "kbase\command_line.h"

#include <Windows.h>

#include <algorithm>
#include <utility>

#include "kbase\error_exception_util.h"
#include "kbase\strings\string_util.h"

namespace {

using kbase::CommandLine;
using CharType = CommandLine::CharType;
using StringType = CommandLine::StringType;
using ArgList = CommandLine::ArgList;
using SwitchPair = std::pair<StringType, StringType>;

const CharType* kSwitchPrefixes[] {L"-", L"--", L"/"};
const CharType kSwitchValueDelimiter[] = L"=";

bool IsArgumentSwitch(const StringType& arg)
{
    return std::any_of(std::begin(kSwitchPrefixes), std::end(kSwitchPrefixes),
                       [&arg](const CharType* prefix)->bool {
        return kbase::StartsWith(arg, prefix);
    });
}

SwitchPair UnstickSwitch(StringType* switch_token)
{
    kbase::TrimLeadingStr(*switch_token, L"-/", switch_token);
    std::vector<StringType> members;
    kbase::Tokenize(*switch_token, kSwitchValueDelimiter, &members);
    // Note that the switch may not carry a value.
    members.resize(2);

    return {members[0], members[1]};
}

void AddArguments(CommandLine* cmdline, const ArgList& argv)
{
    for (auto arg = argv.cbegin() + 1; arg != argv.cend(); ++arg) {
        StringType sanitized_arg;
        kbase::TrimString(*arg, L" \t", &sanitized_arg);
        if (IsArgumentSwitch(sanitized_arg)) {
            auto switch_member = UnstickSwitch(&sanitized_arg);
            cmdline->AppendSwitch(switch_member.first, switch_member.second);
        } else {
            cmdline->AppendParameter(sanitized_arg);
        }
    }
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
    AddArguments(this, argv);
}

void CommandLine::AppendSwitch(const StringType& name, const StringType& value)
{

}

void CommandLine::AppendParameter(const StringType& parameter)
{

}

}   // namespace kbase