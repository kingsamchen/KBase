PathService
===

---

[TOC]

`PathService` is a facility that retrieves the path of a file or a direcotry which is associated with a pre-defined `PathKey`.

The whole architecture of `PathService` is devided into a **path service** module and a **path provider** module, that is, the classical rule of design: *separation of policy from mechanism*.

There is a default provider, named `BasePathProvider` and defined in `base_path_provider.h/cpp` files, and it provides pathes of some common asked files or directories.

Certainly, library users can provide their own path-provider and make it available to `PathService` by calling `PathService::RegisterPathProvider`. However, there is some requirements you must to meet, which will be described later.

### ctor/dtor/operator=

---

The `PathService` itself intends to be a static class, hence it has both constructor and destructor being **delete**.

### Path and Path Provider

---

#### FilePath Get(PathKey key) [*static*]

Returns the path of a file or directory that is associated with `key`.

If the `key` does not indicate an available path, the function returns an **empty** file path.

You can look up the base-path-provider key in `base_path_provider.h`.

**NOTE:** This function can ensure the path returned must be a absolute path. Therefore, if you provide your own paht-provider, make sure meet this requirement.

#### void RegisterPathProvider(ProviderFunc provider, PathKey start, PathKey end) [*static*]

Call this function to register your own path-provider.

Two critical rules you must obey:

1. The range/interval of your path-key mustn't overlap with other providers'.
2. Your provider should definitely-never call `PathService::Get`.
3. The path returned by provider must be a absolute path.

### Internal Cache

---

#### void PathService::DisableCache() [*static*]

Disables internal cache.

Since the `PathService` internally uses a cache for reducing query time for a path-key that has already been queried, this function can disable this cache mechanism.

But don't panic, path of current directory would never be cached; and the entire `PathService` is thread-safe.

Please note, when you disable the internal cache, the cache will be clear.

#### void PathService::EnableCache() [*static*]

Enables internal cache.

