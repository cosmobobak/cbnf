# cbnf
The CBNF (cat-boy network format) neural network header format.

## What is CBNF?

CBNF is a neural network header format for use with [efficiently updateable neural networks]() for chess. It is in a very early stage of development, and is currently subject to change.

## Data Format

CBNF is a fixed-size 64-byte header format, shown here with C++ and Rust structs:

```cpp
struct __attribute__((packed)) CBNFHeader {
    std::array<char, 4> magic;
    std::uint16_t version;
    std::uint16_t flags;
    std::uint8_t padding;
    std::uint8_t arch;
    std::uint8_t activation;
    std::uint16_t hiddenSize;
    std::uint8_t inputBuckets;
    std::uint8_t outputBuckets;
    std::uint8_t nameLen;
    std::array<char, 48> name;
};
```

```rust
#[repr(C, packed)]
pub struct CBNFHeader {
    pub magic: [u8; 4],
    pub version: u16,
    pub flags: u16,
    pub padding: u8,
    pub arch: u8,
    pub activation: u8,
    pub hidden_size: u16,
    pub input_buckets: u8,
    pub output_buckets: u8,
    pub name_len: u8,
    pub name: [u8; 48],
}
```

The magic bytes must equal `CBNF`, or the header is considered ill-formed, no diagnostic required, instant UB, nasal demons, etc etc.

## Existing Implementations

### cbnf-rs
This repository contains a Rust implementation of CBNF, as a crate that provides rudimentary convenience methods, such as header parsing and utf-8 name validation.