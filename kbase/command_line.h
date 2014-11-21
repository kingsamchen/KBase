/*
 @ Kingsley Chen
*/

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef KBASE_COMMAND_LINE_H_
#define KBASE_COMMAND_LINE_H_

#include <map>
#include <string>
#include <vector>

#include "kbase\files\file_path.h"
#include "kbase\memory\lazy.h"

namespace kbase {

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

private:
    std::vector<StringType> args_;
    std::map<StringType, StringType> options_;
    static Lazy<CommandLine> current_process_cmdline_;
};

}   // namespace kbase

#endif  // KBASE_COMMAND_LINE_H_