Command-Line
===

A command line consists of one or more arguments, which are tokens separated by spaces or tabs.

The first argument is always program, and usually in the full path.

Arguments with `-`, `--`, or `/` preceded are called switches. A switch can optionally have a value that is delimited by the `=`. By the way, the default switch prefix chosen is `-`.

Arguments that are not switches are called parameters. They are specific input values.

The general accepted order of all arguments in a command line is as follows:

`program [(-|--|/)switch[=value]] parameter`

that is, switches are always in front of parameters.

## Usage At A Glance

### Create a command-line

If you have an array of arguments at disposal, you can create a command-line by

```c++
int argc;
wchar_t* argv[];

kbase::CommandLine cmdline(argc, argv);
```

or you even can create a command-line from a string literal

```c++
kbase::CommandLine cmdline(L"C:\\Programs\\Test.exe -type=render -p D:\\test.txt");
``` 

### Command-line for Current Process

If you want to retrieve the command-line for current process, just call `ForCurrentProcess()`:

```c++
const kbase::CommandLine& current_cmdline = kbase::CommandLine::ForCurrentProcess();
```

As you can see, the command-line for current process is a constant reference, which means you cannot modify it directly.

Also, this command-line object is **lazy-initialized**. You afford no cost if you don't access it.

### Switches and Parameters

The most needed scenarios for modifying a command-line are adding switches or parameters, which you can do with

```c++
kbase::CommandLine cmdline(L"...");
cmdline.AppendSwitch(L"type", L"main").AppendSwitch(L"Embedding", L"");
cmdline.AppendParameter(L"BDCS");
``` 

Sometimes, you need to check whether a command-line contains a switch, and what value it is, if there is one. In this situation, you can use `HashSwitch()` and `GetSwitchValue()`

```c++
auto& cmdline = kbase::CommandLine::ForCurrentProcess();
if (cmdline.HasSwitch(L"type")) {
	...
}

std::wstring degree;
bool ret = cmdline.GetSwitchValue(L"degree", &degree);
```

### Stringify the Command-Line

If you need a string that corresponds to a command-line, you can get the string by

```c++
auto& cmdline = kbase::CommandLine::ForCurrentProcess();
auto cmdline_str = cmdline.GetCommandLineString();
```