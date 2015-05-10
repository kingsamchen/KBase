Base64
===

The `Base64*` utilities make you capable of encoding a **string or a chunk of data** to, or decoding to get which from a base64 text.

To conform with a common requirement, padding here is mandatory.

## Usage At A Glance
 
when we need to encode a string to base64, we just

```c++
#include "kbase\base64.h"

std::string str = "any carnal pleasure";
std::string encoded = kbase::Base64Encode(str);
// the `encoded` contains corresponding base64 code: YW55IGNhcm5hbCBwbGVhc3VyZQ==
```

Similarly, if we want to decode from a base64 text, we simply call `kbase::Base64Decode`.

```c++
#include "kbase\base64.h"

std::string base64_code = ...;
auto text = kbase::Base64Decode(base64_code);
```