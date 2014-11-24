/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_COMMAND_LINE_H_
#define KBASE_COMMAND_LINE_H_

#include <array>
#include <list>
#include <map>
#include <string>
#include <vector>

#include "kbase\files\file_path.h"
#include "kbase\memory\lazy.h"

namespace kbase {

// A command line on Windows consists of one or more arguments, which are tokens
// separated by spaces or tabs.
// Arguments with preceded '--', '-', and '/' are switches. A switch can optionally
// have a value that is delimited by '='.
// Arguments that are not switches are called parameters. They are just specific
// values.
// Besides, the first argument is called program, since it always refers to the full
// path of the program.
// The general order of arguments in command line is as follows:
// { program, [(-|--|/)switchs[=value]], [parameters] } 
// that is, switches always precede with arguments.
class CommandLine {
public:
    using StringType = std::wstring;
    using CharType = StringType::value_type;
    using ArgList = std::vector<StringType>;

    enum DefaultSwitchPrefix : size_t {
        PREFIX_DASH = 0,
        PREFIX_DOUBLE_DASH,
        PREFIX_SLASH
    };

    explicit CommandLine(const FilePath& program);

    CommandLine(int argc, const CharType* const* argv);

    explicit CommandLine(const ArgList& argv);

    // |cmdline| has same requirement as in ParseFromString.
    explicit CommandLine(const StringType& cmdline);

    ~CommandLine() = default;

    // TODO: add copy-semantics and move-semantics process

    static CommandLine FromString(const StringType& cmdline);

    static const CommandLine& ForCurrentProcess();

    void ParseFromArgv(int argc, const CharType* const* argv);

    void ParseFromArgv(const ArgList& argv);

    // Since Windows uses spaces or tabs to separate command line arguments, please
    // make sure that file path of the program, i.e. argv[0], is enclosed with
    // quotation marks, if the path may contain spaces.
    void ParseFromString(const StringType& cmdline);

    // Default switch prefix is used when stringizing switches.
    // No matter which prefix a switch uses, it eventually will be eliminated when
    // the switch is stashed inside, and thus being prepended with default switch
    // prefix if you want to stringify switches.
    DefaultSwitchPrefix GetDefaultSwitchPrefix() const;

    void SetDefaultSwitchPrefix(DefaultSwitchPrefix prefix_type);

    FilePath GetProgram() const;

    void SetProgram(const FilePath& program);

    void AppendSwitch(const StringType& name, const StringType& value = StringType());

    void AppendParameter(const StringType& arg);

private:
    using Argv = std::list<StringType>;
    using SwitchTable = std::map<StringType, StringType>;

    static Lazy<CommandLine> current_process_cmdline_;
    Argv argv_;
    Argv::iterator last_not_param_;
    SwitchTable switches_;
    static const size_t kSwitchPrefixSizeLimit = 3;
    // Use '-' as out initial default switch prefix.
    // HACK: VS2013 doesn't support explicit initializer for class member array yet.
    std::array<wchar_t, kSwitchPrefixSizeLimit> default_switch_prefix_ {{L"-"}};
};

}   // namespace kbase

#endif  // KBASE_COMMAND_LINE_H_