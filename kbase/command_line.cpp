/*
 @ 0xCCCCCCCC
*/

#include "kbase/command_line.h"

#include <algorithm>
#include <utility>

#include "kbase/basic_types.h"
#include "kbase/error_exception_util.h"
#include "kbase/string_util.h"

#if defined(OS_WIN)
#include <Windows.h>

#include "kbase/scope_guard.h"
#include "kbase/string_encoding_conversions.h"
#endif

namespace {

using kbase::CommandLine;

using ArgList = CommandLine::ArgList;
using SwitchPrefix = CommandLine::SwitchPrefix;
using SwitchPair = std::pair<std::string, std::string>;

constexpr const char* kSwitchPrefixes[] {"--", "-", "/"};

constexpr char kSwitchValueDelimiter[] = "=";

constexpr SwitchPrefix kDefaultSwitchPrefix = SwitchPrefix::DoubleDash;

bool IsArgumentSwitch(const std::string& arg)
{
    return std::any_of(std::begin(kSwitchPrefixes), std::end(kSwitchPrefixes),
                       [&arg](const char* prefix) {
                           return kbase::StartsWith(arg, prefix);
                       });
}

SwitchPair UnstickSwitch(std::string& switch_token)
{
    kbase::TrimLeadingString(switch_token, "-/");

    std::vector<std::string> members;
    kbase::SplitString(switch_token, kSwitchValueDelimiter, members);

    // Note that the switch may not carry a value.
    members.resize(2);

    return {members[0], members[1]};
}

#if defined(OS_WIN)

ArgList ParseCommandLineString(std::wstring cmdline)
{
    kbase::TrimString(cmdline, L" \t");
    std::wstring sanitized_cmdline_str(std::move(cmdline));

    int argc = 0;
    auto argv = CommandLineToArgvW(sanitized_cmdline_str.c_str(), &argc);
    if (!argv) {
        auto last_err = kbase::LastError();
        ENSURE(THROW, kbase::NotReached())(last_err)(sanitized_cmdline_str).Require(
            "CommandLineToArgvW() failed");
    }

    ON_SCOPE_EXIT { LocalFree(argv); };

    ArgList args;
    for (int i = 0; i < argc; ++i) {
        args.push_back(kbase::WideToUTF8(argv[i]));
    }

    return args;
}

// Quotes the `arg` if necessary.
// Algorithm comes from http://goo.gl/mxKhoj
std::string QuoteArg(const std::string& arg)
{
    if (arg.find_first_of(" \t\"") == arg.npos) {
        return arg;
    }

    std::string quoted_arg(1, '"');
    for (auto it = arg.cbegin(); ; ++it) {
        size_t number_of_backslash = 0U;

        // Count number adjacent backslashes.
        while (it != arg.cend() && *it == '\\') {
            ++it;
            ++number_of_backslash;
        }

        if (it == arg.cend()) {
            quoted_arg.append(2 * number_of_backslash, '\\');
            break;
        }

        if (*it == '"') {
            quoted_arg.append(2 * number_of_backslash + 1, '\\');
            quoted_arg.push_back(*it);
        } else {
            quoted_arg.append(number_of_backslash, '\\');
            quoted_arg.push_back(*it);
        }
    }

    quoted_arg.push_back('"');

    return quoted_arg;
}
#endif

} // namespace

namespace kbase {

CommandLine* CommandLine::current_process_cmdline_ = nullptr;

CommandLine::CommandLine(const Path& program)
    : args_(1),
      arg_not_param_count_(1),
      switch_prefix_(kDefaultSwitchPrefix)
{
    SetProgram(program);
}

CommandLine::CommandLine(const ArgList& args)
    : args_(1),
      arg_not_param_count_(1),
      switch_prefix_(kDefaultSwitchPrefix)
{
    ParseFromArgs(args);
}

CommandLine::CommandLine(int argc, const char* const* argv)
    : args_(1),
      arg_not_param_count_(1),
      switch_prefix_(kDefaultSwitchPrefix)
{
    ParseFromArgs(argc, argv);
}

// static
void CommandLine::Init(int argc, const char* const* argv)
{
    if (current_process_cmdline_) {
        ENSURE(CHECK, NotReached()).Require();
        return;
    }

#if defined(OS_WIN)
    UNUSED_VAR(argc);
    UNUSED_VAR(argv);
    auto args = ParseCommandLineString(GetCommandLineW());
    static CommandLine instance(args);
#elif defined(OS_POSIX)
    static CommandLine instance(argc, argv);
#endif
    current_process_cmdline_ = &instance;
}

// static
const CommandLine& CommandLine::ForCurrentProcess()
{
    ENSURE(CHECK, current_process_cmdline_ != nullptr).Require();
    return *current_process_cmdline_;
}

SwitchPrefix CommandLine::switch_prefix() const noexcept
{
    return switch_prefix_;
}

void CommandLine::set_switch_prefix(SwitchPrefix prefix) noexcept
{
    switch_prefix_ = prefix;
}

Path CommandLine::GetProgram() const
{
    return Path::FromUTF8(args_[0]);
}

void CommandLine::SetProgram(const Path& program)
{
    args_[0] = program.AsUTF8();
    TrimString(args_[0], " \t");
}

void CommandLine::ParseFromArgs(int argc, const char* const* argv)
{
    ArgList bundled_args(argv, argv + argc);
    ParseFromArgs(bundled_args);
}

void CommandLine::ParseFromArgs(const ArgList& args)
{
    // Anyway, we start from scratch.
    args_ = ArgList(1);
    arg_not_param_count_ = 1;
    switches_.clear();

    SetProgram(Path::FromUTF8(args[0]));
    AddArguments(args);
}

void CommandLine::AddArguments(const ArgList& args)
{
    for (auto arg = std::next(args.cbegin()); arg != args.cend(); ++arg) {
        std::string sanitized_arg = kbase::TrimStringCopy(*arg, " \t");
        if (IsArgumentSwitch(sanitized_arg)) {
            auto switch_member = UnstickSwitch(sanitized_arg);
            AppendSwitch(switch_member.first, switch_member.second);
        } else {
            AppendParameter(sanitized_arg);
        }
    }
}

CommandLine& CommandLine::AppendSwitch(const std::string& name, const std::string& value)
{
    switches_[name] = value;

    // Since we have `last_arg_not_param_` to demarcate switches and parameters, we here
    // leave switch prefix unprepended.
    std::string switch_arg(name);
    if (!value.empty()) {
        switch_arg.append(1, '=').append(value);
    }

    args_.insert(std::next(args_.begin(), arg_not_param_count_), std::move(switch_arg));
    ++arg_not_param_count_;

    return *this;
}

CommandLine& CommandLine::AppendParameter(const std::string& param)
{
    args_.push_back(param);
    return *this;
}

bool CommandLine::HasSwitch(const std::string& name) const
{
    return switches_.find(name) != switches_.end();
}

const std::string& CommandLine::GetSwitchValue(const std::string& name,
                                               const std::string& default_value) const
{
    auto it = switches_.find(name);
    return it == switches_.end() ? default_value : it->second;
}

const std::string& CommandLine::GetParameter(size_t idx) const
{
    size_t arg_idx = arg_not_param_count_ + idx;
    ENSURE(THROW, arg_idx < args_.size())(arg_not_param_count_)(idx).Require();
    return args_[arg_idx];
}

std::string CommandLine::GetCommandLineString() const
{
#if defined(OS_WIN)
    std::string cmdline_str(QuoteArg(args_[0]));
#elif defined(OS_POSIX)
    std::string cmdline_str(args_[0]);
#endif
    cmdline_str.append(1, ' ').append(GetCommandLineStringWithoutProgram());

    return cmdline_str;
}

std::string CommandLine::GetCommandLineStringWithoutProgram() const
{
    ArgList stringified_args;
    stringified_args.reserve(args_.size());

    auto prefix = kSwitchPrefixes[enum_cast(switch_prefix())];
    for (size_t i = 1; i < args_.size(); ++i) {
        std::string arg_str = i + 1 <= arg_not_param_count_ ? std::string(prefix) : std::string();
        arg_str.append(args_[i]);
#if defined(OS_WIN)
        stringified_args.push_back(QuoteArg(arg_str));
#elif defined(OS_POSIX)
        stringified_args.push_back(arg_str);
#endif
    }

    return JoinString(stringified_args, std::string(1, ' '));
}

} // namespace kbase
