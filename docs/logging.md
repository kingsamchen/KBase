# Logging

[TOC]

### Motivation

The logging module, just like error-and-exception-util, is an infrastructure of the entire lib.

This module offers you an uniform and succinct interface to log messages, in different severity level (e.g. `INFO`, `WARNING` etc.), to specified destination (`stderr` or a file).


### DLOG/LOG macro

The general syntax for using this facility is

``` c++
#include "kbase/logging.h"

// Configurate logging with default settings.
kbase::ConfigureLoggingSettings(kbase::LoggingSettings());

LOG(INFO) << "something happend";
LOG(ERROR) << "something goes wrong";
LOG_IF(WARNING, result != true) << "the expression doesn't evaluate to true!";
```

`DLOG` is same as `LOG` but only enabled in DEBUG mode.

A log message with the most detailed header looks like:

> [20160127 01:27:07,416 INFO logging_unittest.cpp(80)]something happend.


### Severity Levels

As you can see from the sample above, `logging` supports the hierarchy of severity levels, providing a convenient approach to distinct log messages in different severity levels.

You can also set up a **minimum severity level**, any log message that has lower severity level will be simply  ignored and discarded.

Doing that by calling `ConfigureLoggingSettings`. See next section.


### Configure Logging Settings

For being flexible, `logging` allows you to configure its settings to meet your needs.

Configurable settings include:

- minimum severity level

- message header parameters

- logging destination

- how to dispose(replace or append) an old file if logging to it

- specify logging file name/path

Generally, you should configurate these settings at the start of the program, because calling the configuration function is **not thread-safe**.

class `LoggingSettings` contains setting information, and its default constructor uses default settings too.

Note that, if you didn't call `ConfigureLoggingSettings()` at the start, default settings are employed; However, if you also want logging to file, there will be a race condition in creation of the file.

Therefore, again, **you are supposed to call `ConfigureLoggingSettings()` at the start of the program**.


### Some Details About Logging To File

If it being the case, it's better for you to know some details.

Logging to the file is **not only thread-safe but also process-safe**. Thanks to Windows kernel.

As for the file name, if you didn't specify one, `logging` first tries to use the name like `[your-exe-name]_debug_message.log`, and put the file in the same folder that contains the executable; If this fails, it then tries to put the file in the current working directory.

If both trials failed, `logging` automatically skips file writting.