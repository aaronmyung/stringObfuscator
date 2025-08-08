# string-obfuscator

**Header-only C++17 library for compile-time string obfuscation.**  
Prevents sensitive string literals from appearing in plaintext within compiled binaries.

---

## Overview

This utility provides a lightweight method to obfuscate string literals at compile time. Encrypted strings are stored in the binary and only decrypted at runtime into stack memory. Upon destruction, decrypted buffers are securely wiped.

Use cases include:

- Hiding API keys, file paths, internal flags, or debug messages
- Reducing visibility of sensitive data in reverse engineering
- Obfuscating strings in software with tamper-resistance requirements

---

## Features

- 🔐 Compile-time encryption using `constexpr` and a build-unique key  
- 🧠 Decryption at runtime into a stack-based RAII object  
- 🧼 Automatic memory zeroing on object destruction  
- 🚫 No heap usage, no static plaintext, no dynamic allocation  
- 📦 Single-header, zero dependencies

---

## Example

```cpp
#include "string_obfuscator.h"

void load_config() {
    auto api_key = OBFSTR("my_secret_api_key");
    send_authentication(api_key.c_str());

    // `api_key` is securely zeroed when it goes out of scope
}