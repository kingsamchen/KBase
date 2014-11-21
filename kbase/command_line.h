/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_COMMAND_LINE_H_
#define KBASE_COMMAND_LINE_H_

#include <list>
#include <map>
#include <string>
#include <vector>

#include "kbase\files\file_path.h"
#include "kbase\memory\lazy.h"

namespace kbase {

// A command line on Windows consists of one or more arguments, which are tokens
// separated by spaces or tabs.
// Arguments with preceded '--', '-', and '/' are switches. Switches can optionally
// have values, delimited by '='.
class CommandLine {
public:
    typedef std::wstring StringType;
    typedef StringType::value_type CharType;

    explicit CommandLine(const FilePath& program);

    CommandLine(int argc, const CharType* const* argv);

    explicit CommandLine(const std::vector<StringType>& argv);

    explicit CommandLine(const StringType& cmdline);

    ~CommandLine() = default;

    // TODO: add copy-semantics and move-semantics process

    static CommandLine FromString(const StringType& cmdline);

    static const CommandLine& ForCurrentProcess();

    void ParseFromArgv(int argc, const CharType* const* argv);

    void ParseFromArgv(const std::vector<StringType>& argv);

    void ParseFromString(const StringType& cmdline);

    FilePath GetProgram() const;

    void SetProgram(const FilePath& program);

private:
    typedef std::list<StringType> Argv;
    typedef std::map<StringType, StringType> SwitchTable;

    static Lazy<CommandLine> current_process_cmdline_;
    Argv argv_;
    Argv::iterator last_not_args_;
    SwitchTable switches_;    
};

}   // namespace kbase

#endif  // KBASE_COMMAND_LINE_H_