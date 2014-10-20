PathService
===

---

[TOC]

`PathService` is a facility that retrieves the path of a file or a direcotry which is associated with a pre-defined `PathKey`.

The whole architecture of `PathService` is devided into a **path service** module and a **path provider** module, that is, the classical principle of design: *separation of policy from mechanism*.

There is a default provider, named `BasePathProvider` and defined in `base_path_provider.h/cpp` files, and it provides pathes of some common asked files or directories.

Certainly, library users can provide their own path-provider and make it available to `PathService` by calling `PathService::RegisterPathProvider`.

### ctor/dtor/operator=

### Path and Path Provider

### Internal Cache