# Command Line Processing

A command line consists of one or more arguments, which are tokens separated by spaces or tabs.

The first argument is always _program_, and usually in the full path.

Arguments preceding with `-`, `--`, or `/` are called _switches_. A switch can optionally have a value that is delimited by the `=`. By the way, the default switch prefix chosen is `--`.

Note: unlike some other commandline libraries, you must use `=` to delimit a switch and its value, which means you cannot use `--mode on` to opt-in the switch value; because the `on` will be evaluated as a _parameter_.

Arguments that are not switches are called _parameters_. They are specific input values.

## Usage At A Glance

### Create a command-line

If you have an array of arguments at disposal, you can create a command-line either by

```c++
int argc;
const char* argv[];

kbase::CommandLine cmdline(argc, argv);
```

or by creating from an `ArgList`:

```c++
CommandLine::ArgList args {
    "/usr/local/bin/test", "--test", "--dry-run=true", "1234"
};

CommandLine cmdline(args);
```

### Command-line for Current Process

If you want to retrieve the command-line for the current process, just:

```c++
int main(int argc, char* argv[])
{
    kbase::CommandLine::Init(argc, argv);

    const kbase::CommandLine& current_cmdline = kbase::CommandLine::ForCurrentProcess();

    return 0;
}
```

On Windows, the `argc` and `argv` are both ignored, becase CRT's parsing algorithm is not reliable and we do parsing via native platform-specific approaches.

As you can see, the command-line for current process is **a constant reference**, which means you cannot modify it directly.

Also, this command-line object is **lazy-initialized**. You afford no cost if you don't access it.

### Handling Switches

Class `CommandLine` offers you several ways to query if a switch present, and retrieve a switch value.

Fundamentally, you can use `HasSwitch()` to check if a switch present, and use `GetSwitchValue()` to fetch its value:

```c++
auto& cmdline = kbase::CommandLine::ForCurrentProcess();

if (cmdline.HasSwitch("dry-run")) {
    // do something...
    // we actually use this case as a boolean flag.
}

const std::string& mode = cmdline.GetSwitchValue("mode");
```

If the switch not present, `GetSwitchValue()` returns a default value, i.e. an empty `std::string` instead.

You can specify your default string to return when a switch is not present:

```c++
auto mode = cmdline.GetSwitchValue("mode", "off");
```

#### Switch Value with Data Type Conversion

By default, switch values are in `std::string`; but you can choose to retrieve a switch value in specific data type, by using `GetSwitchValueAs()`:

```c++
// For commandline "/usr/bin/test --port=9876 --dry-run=true"

auto port = cmdline.GetSwitchValueAs<int>("port", 8888);
auto dry_run = cmdline.GetSwitchValueAs<bool>("dry-run");
```

and yes, you still can give your default value when a switch is not present.

If you need to customize the data conversion, you can use your own type converter:

```c++
auto cvt = [](const auto& v) { return static_cast<unsigned short>(std::stoi(v)); };
CHECK(cmdline.GetSwitchValueAs<unsigned short>("port", 9876, cvt) == 9876);
```

Note: If data conversion fails to complete, `GetSwitchValueAs()` function will throw a `CommandLineValueParseError` exception.

For your own converters, you should throw an exception to indicate a conversion failure.

### Handle Parameters

Parameter values are determined by its relative index among all parameters.

You can use `parameter_count()` to get the count of total paramters, and use `GetParameter()` to fetch the parameter at the given index.

Out of index access to a paramter will throw an exception.

Like switch scenario above, you can perform parameter value type conversion via `GetParameterAs()` function.

### Stringify the Command-Line

If you need a string corresponding to a command-line, you can get the string by either

```c++
auto& cmdline = kbase::CommandLine::ForCurrentProcess();
auto cmdline_str = cmdline.GetCommandLineString();
```

or without program part:

```c++
auto cmdline_str = cmdline.GetCommandLineStringWithoutProgram();
```

