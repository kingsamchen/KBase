/*
 @ 0xCCCCCCCC
*/

#ifndef KBASE_COMMAND_LINE_H_
#define KBASE_COMMAND_LINE_H_

#include <string>
#include <unordered_map>
#include <vector>

#include "kbase/path.h"
#include "kbase/string_util.h"

namespace kbase {

class CommandLineValueParseError : public std::invalid_argument {
public:
    explicit CommandLineValueParseError(const std::string& what)
        : invalid_argument(what)
    {}
};

namespace internal {

template<typename C>
struct ValueConverter;

template<>
struct ValueConverter<int32_t> {
    int32_t operator()(const std::string& value) const
    {
        return std::stoi(value, nullptr, 0);
    }
};

template<>
struct ValueConverter<uint32_t> {
    uint32_t operator()(const std::string& value) const
    {
        long long n = std::stoll(value, nullptr, 0);
        if (n > UINT32_MAX || n < INT32_MIN) {
            throw std::out_of_range("convert to uint32_t: out of range");
        }

        return static_cast<uint32_t>(n);
    }
};

template<>
struct ValueConverter<int64_t> {
    int64_t operator()(const std::string& value) const
    {
        return std::stoll(value, nullptr, 0);
    }
};

template<>
struct ValueConverter<uint64_t> {
    uint64_t operator()(const std::string& value) const
    {
        return std::stoull(value, nullptr, 0);
    }
};

template<>
struct ValueConverter<int16_t> {
    int16_t operator()(const std::string& value) const
    {
        int n = std::stoi(value, nullptr, 0);
        if (n > INT16_MAX || n < INT16_MIN) {
            throw std::out_of_range("convert to int16_t: out of range");
        }

        return static_cast<int16_t>(n);
    }
};

template<>
struct ValueConverter<uint16_t> {
    uint16_t operator()(const std::string& value) const
    {
        int n = std::stoi(value, nullptr, 0);
        if (n > UINT16_MAX || n < INT16_MIN) {
            throw std::out_of_range("convert to uint16_t: out of range");
        }

        return static_cast<uint16_t>(n);
    }
};

template<>
struct ValueConverter<double> {
    double operator()(const std::string& value) const
    {
        return std::stod(value);
    }
};

template<>
struct ValueConverter<bool> {
    bool operator()(const std::string& value) const
    {
        if ((value == "1") || kbase::ASCIIStringEqualCaseInsensitive(value, "true")) {
            return true;
        }

        if ((value == "0") || kbase::ASCIIStringEqualCaseInsensitive(value, "false")) {
            return false;
        }

        throw std::invalid_argument("inconvertible to bool");
    }
};

}   // namespace internal

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

    ~CommandLine() = default;

    CommandLine(const CommandLine&) = default;

    CommandLine& operator=(const CommandLine&) = default;

    CommandLine(CommandLine&&) = default;

    CommandLine& operator=(CommandLine&&) = default;

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
    // Returns true if a switch that matches the given `name` is found.
    bool HasSwitch(const std::string& name) const;

    // Returns value of the switch matching the given `name`.
    // The function uses the `default_value` if the target switch is not present.
    const std::string& GetSwitchValue(const std::string& name,
                                      const std::string& default_value = std::string()) const;

    // Returns value of the switch matching the given `name` in specified data type.
    // The function uses the `default_value` if the target switch is not present.
    // If the data conversion failed, the function throws a CommandLineValueParseError exception.
    template<typename T, typename Cvt>
    T GetSwitchValueAs(const std::string& name, const T& default_value, Cvt cvt) const
    {
        auto it = switches_.find(name);
        if (it == switches_.end()) {
            return T(default_value);
        }

        try {
            return cvt(it->second);
        } catch (const std::exception& ex) {
            std::string what = "failed to convert ";
            what.append(it->second).append(": ").append(ex.what());
            throw CommandLineValueParseError(what);
        }
    }

    // Similar with the above version except using predefined value converters, and default
    // value is optional.
    template<typename T>
    T GetSwitchValueAs(const std::string& name, const T& default_value = T()) const
    {
        return GetSwitchValueAs<T>(name, default_value, internal::ValueConverter<T>());
    }

    const SwitchTable& GetSwitches() const noexcept
    {
        return switches_;
    }

    // Count does not include the program part and switches.
    size_t parameter_count() const noexcept
    {
        return args_.size() - arg_not_param_count_;
    }

    const std::string& GetParameter(size_t idx) const;

    template<typename T, typename Cvt>
    T GetParameterAs(size_t idx, Cvt cvt) const
    {
        const auto& value = GetParameter(idx);
        try {
            return cvt(value);
        } catch (const std::exception& ex) {
            std::string what = "failed to convert ";
            what.append(value).append(": ").append(ex.what());
            throw CommandLineValueParseError(what);
        }
    }

    // Similar with the above version except using predefined value converters internally.
    template<typename T>
    T GetParameterAs(size_t idx) const
    {
        return GetParameterAs<T>(idx, internal::ValueConverter<T>());
    }

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
