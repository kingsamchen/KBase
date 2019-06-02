/*
 @ 0xCCCCCCCC
*/

#ifndef KBASE_COMMAND_LINE_H_
#define KBASE_COMMAND_LINE_H_

#include <unordered_map>
#include <vector>

#include "kbase/basic_macros.h"
#include "kbase/path.h"

namespace kbase {

// A command line consists of one or more arguments, which are tokens separated by
// one or more spaces or tabs.
// Arguments preceded with '--', '-', and '/' are switches. A switch can optionally
// have a value which is delimited by '='.
// Arguments that are not switches are called parameters. They are just specific
// values.
// Besides, the first argument is called program, since it always refers to the full
// path of the program.
class CommandLine {
public:
    using ArgList = std::vector<std::string>;
    using SwitchTable = std::unordered_map<std::string, std::string>;

    enum class SwitchPrefix : unsigned int {
        DoubleDash = 0,
        Dash,
        Slash
    };

    CommandLine(int argc, const char* const* argv);

    explicit CommandLine(const Path& program);

    explicit CommandLine(const ArgList& args);

    DEFAULT_COPY(CommandLine);

    DEFAULT_MOVE(CommandLine);

    ~CommandLine() = default;

    // Initializes the singleton CommandLine instance for the current process. On Windows,
    // be sure to ignore its arguments, we internally use GetCommandLineW() instead, because
    // CRT's parsing algorithm on Windows is not reliable.
    static void Init(int argc, const char* const* argv);

    // Returns the current process's singleton CommandLine instance.
    static const CommandLine& ForCurrentProcess();

    SwitchPrefix switch_prefix() const noexcept;

    void set_switch_prefix(SwitchPrefix prefix) noexcept;

    Path GetProgram() const;

    void SetProgram(const Path& program);

    // `name` should not be preceded with prefix.
    CommandLine& AppendSwitch(const std::string& name, const std::string& value = std::string());

    CommandLine& AppendParameter(const std::string& param);

    // `name` should not be preceded with prefix.
    bool HasSwitch(const std::string& name) const;

    // Returns true if succeeded in querying the value associated with the switch.
    // Returns false if no such switch was found, and `value` remains unchanged.
    bool GetSwitchValue(const std::string& name, std::string& value) const;

    const SwitchTable& GetSwitches() const noexcept
    {
        return switches_;
    }

    size_t parameter_count() const noexcept
    {
        return args_.size() - arg_not_param_count_;
    }

    const std::string& GetParameter(size_t idx) const;

    const ArgList& GetArgs() const noexcept
    {
        return args_;
    }

    // The general order of arguments in output string is as follows:
    // { program, [(-|--|/)switch[=value]], [parameter] }
    // that is, switches always precede with arguments.
    std::string GetCommandLineString() const;

    // Similar with GetCommandLineString() but without including program.
    std::string GetCommandLineStringWithoutProgram() const;

private:
    void ParseFromArgs(int argc, const char* const* argv);

    void ParseFromArgs(const ArgList& args);

    void AddArguments(const ArgList& args);

private:
    ArgList args_;
    size_t arg_not_param_count_;
    SwitchTable switches_;
    SwitchPrefix switch_prefix_;
    static CommandLine* current_process_cmdline_;
};

}   // namespace kbase

#endif  // KBASE_COMMAND_LINE_H_
