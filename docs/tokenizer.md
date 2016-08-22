# Tokenizer

[TOC]

### Motivations

Unlike `kbase::SplitString`, `Tokenizer` provides an iterative approach to get every tokens in terms of `BasicStringView`, and demarcated by a set of delimiters, in a given string.

Therefore, if modifications on a token are not needed, `Tokenizer` is both memory efficient and performance efficient.

### Iterate Tokens

```c++
std::string str = "anything that cannot kill you makes you stronger.\n\tsaid by Bruce Wayne\n";
std::vector<std::string> exp { "anything", "that", "cannot", "kill", "you", "makes", "you",
                               "stronger", "said", "by", "Bruce", "Wayne" };
Tokenizer tokenizer(str, " .\n\t");
size_t i = 0;
for (auto&& token : tokenizer) {
    if (!token.empty()) {
        EXPECT_EQ(exp[i], token.ToString());
        ++i;
    }
}
```