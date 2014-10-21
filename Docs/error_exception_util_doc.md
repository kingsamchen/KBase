Error and Exception Utils
===

[TOC]

This util contains a set of handy tools for validation and error/exception generation.

Members now consist of a macro `ENSURE`, a Windows last-error wrapper `LastError`, and a validating function `ThrowLastErrorIf` for Windows API invocations.

## ENSURE

`ENSURE` is an enhanced-version of `assert`. It supports rich-context-capture with nearly countless variables.

Unlike `assert`, if `ENSURE` asserts on failure, it throws a C++ exception, and use rich-context information it captured as description.

Moreover, `ENSURE` by default is available on **Release** mode too.

To make debug easier, `ENSURE` will internally invoke `assert`  on **Debug** mode, which gives you a chance to attach your debugger.

## LastError

## ThrowLastErrorIf

