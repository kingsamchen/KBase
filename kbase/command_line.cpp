/*
 @ 0xCCCCCCCC
*/

#include "kbase\command_line.h"

#include <Windows.h>

#include <algorithm>
#include <cstdlib>
#include <utility>

#include "kbase\error_exception_util.h"
#include "kbase\string_util.h"

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

// Quotes the |arg| if necessary.
// Algorithm comes from http://goo.gl/mxKhoj
StringType QuoteArg(const StringType& arg)
{
    if (arg.find_first_of(L" \t\"") == arg.npos) {
        return arg;
    }

    StringType quoted_arg(1, L'"');
    for (auto it = arg.cbegin(); ; ++it) {
        size_t number_of_backslash = 0U;

        // Count number adjacent backslashes.
        while (it != arg.cend() && *it == L'\\') {
            ++it;
            ++number_of_backslash;
        }

        if (it == arg.cend()) {
            quoted_arg.append(2 * number_of_backslash, L'\\');
            break;
        } else if (*it == L'"') {
            quoted_arg.append(2 * number_of_backslash + 1, L'\\');
            quoted_arg.push_back(*it);
        } else {
            quoted_arg.append(number_of_backslash, L'\\');
            quoted_arg.push_back(*it);
        }
    }

    quoted_arg.push_back(L'"');

    return quoted_arg;
}

}   // namespace

namespace kbase {

Lazy<CommandLine> CommandLine::current_process_cmdline_([]() {
    return new CommandLine(GetCommandLineW());
});

CommandLine::CommandLine(const FilePath& program)
    : argv_(1), last_not_param_(argv_.begin()), default_switch_prefix_({L"-"})
{
    SetProgram(program);
}

CommandLine::CommandLine(const CommandLine& other)
    : argv_(other.argv_), switches_(other.switches_),
      default_switch_prefix_(other.default_switch_prefix_)
{
    // Need to fix the iterator to argv.
    size_t dist = std::distance<Argv::const_iterator>(other.argv_.begin(),
                                                      other.last_not_param_);
    last_not_param_ = std::next(argv_.begin(), dist);
}

CommandLine::CommandLine(CommandLine&& other)
{
    *this = std::move(other);
}

CommandLine::CommandLine(int argc, const CharType* const* argv)
    : argv_(1), last_not_param_(argv_.begin()), default_switch_prefix_({L"-"})
{
    ParseFromArgv(argc, argv);
}

CommandLine::CommandLine(const std::vector<StringType>& argv)
    : argv_(1), last_not_param_(argv_.begin()), default_switch_prefix_({L"-"})
{
    ParseFromArgv(argv);
}

CommandLine::CommandLine(const StringType& cmdline)
    : argv_(1), last_not_param_(argv_.begin()), default_switch_prefix_({L"-"})
{
    ParseFromString(cmdline);
}

CommandLine& CommandLine::operator=(const CommandLine& rhs)
{
    if (this != &rhs) {
        argv_ = rhs.argv_;
        size_t dist = std::distance<Argv::const_iterator>(rhs.argv_.begin(),
                                                          rhs.last_not_param_);
        last_not_param_ = std::next(argv_.begin(), dist);
        switches_ = rhs.switches_;
        default_switch_prefix_ = rhs.default_switch_prefix_;
    }

    return *this;
}

CommandLine& CommandLine::operator=(CommandLine&& rhs)
{
    if (this != &rhs) {
        // Once having moved argv from rhs, we shall never access it again.
        size_t dist = std::distance<Argv::const_iterator>(rhs.argv_.begin(),
                                                          rhs.last_not_param_);
        argv_ = std::move(rhs.argv_);
        last_not_param_ = std::next(argv_.begin(), dist);
        switches_ = std::move(rhs.switches_);
        default_switch_prefix_ = std::move(rhs.default_switch_prefix_);
    }

    return *this;
}

// static
const CommandLine& CommandLine::ForCurrentProcess()
{
    return current_process_cmdline_.value();
}

CommandLine::DefaultSwitchPrefix CommandLine::GetDefaultSwitchPrefix() const
{
    for (int i = 0; i < _countof(kSwitchPrefixes); ++i) {
        if (wcscmp(default_switch_prefix_.data(), kSwitchPrefixes[i]) == 0) {
            return static_cast<DefaultSwitchPrefix>(i);
        }
    }

    // Shall never get here.
    ENSURE(false)(default_switch_prefix_.data()).raise();
    return DefaultSwitchPrefix::PREFIX_DASH;
}

void CommandLine::SetDefaultSwitchPrefix(DefaultSwitchPrefix prefix_type)
{
    size_t index = static_cast<size_t>(prefix_type);
    ENSURE(index < _countof(kSwitchPrefixes))(prefix_type).raise();
    wcscpy_s(default_switch_prefix_.data(), default_switch_prefix_.size(),
             kSwitchPrefixes[index]);
}

FilePath CommandLine::GetProgram() const
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
    last_not_param_ = argv_.begin();
    switches_.clear();

    SetProgram(FilePath(argv[0]));
    AddArguments(this, argv);
}

CommandLine& CommandLine::AppendSwitch(const StringType& name, const StringType& value)
{
    switches_[name] = value;
    // Since we have |last_not_param_| to demarcate switches and parameters, we here
    // leave switch prefix unprepended.
    StringType switch_arg(name);
    if (!value.empty()) {
        switch_arg.append(L"=").append(value);
    }

    argv_.emplace(std::next(last_not_param_), switch_arg);
    ++last_not_param_;

    return *this;
}

CommandLine& CommandLine::AppendParameter(const StringType& parameter)
{
    argv_.emplace_back(parameter);

    return *this;
}

bool CommandLine::HasSwitch(const StringType& name) const
{
    return switches_.find(name) != switches_.end();
}

bool CommandLine::GetSwitchValue(const StringType& name, StringType* value) const
{
    auto it = switches_.find(name);
    if (it == switches_.end()) {
        return false;
    }

    *value = it->second;
    return true;
}

ArgList CommandLine::GetParameters() const
{
    Argv::const_iterator params_begin = std::next(last_not_param_);
    ArgList params;
    std::copy(params_begin, argv_.end(), std::back_inserter(params));

    return params;
}

ArgList CommandLine::GetArgv() const
{
    ArgList str_argv;
    str_argv.reserve(argv_.size());

    // Program part.
    str_argv.emplace_back(GetProgram().value());

    // Switches with prepending default prefix.
    Argv::const_iterator switch_end = std::next(last_not_param_);
    std::transform(std::next(argv_.begin()), switch_end, std::back_inserter(str_argv),
                   [&](const StringType& arg)->StringType {
        size_t index = static_cast<size_t>(GetDefaultSwitchPrefix());
        StringType prepended(kSwitchPrefixes[index]);
        prepended.append(arg);

        return prepended;
    });

    // parameters.
    ArgList&& params = GetParameters();
    std::move(params.begin(), params.end(), std::back_inserter(str_argv));

    return str_argv;
}

StringType CommandLine::GetArgvStringWithoutProgram() const
{
    ArgList argv = GetArgv();

    // Skipt program part.
    ArgList quoted_args;
    std::move(std::next(argv.begin()), argv.end(), std::back_inserter(quoted_args));

    // Quotes each part only if necessary.
    std::transform(quoted_args.begin(), quoted_args.end(), quoted_args.begin(),
                   QuoteArg);

    return JoinString(quoted_args, L" ");
}

StringType CommandLine::GetCommandLineString() const
{
    StringType raw_cmdline_str(QuoteArg(argv_.front()));
    raw_cmdline_str.append(L" ").append(GetArgvStringWithoutProgram());

    return raw_cmdline_str;
}

}   // namespace kbase