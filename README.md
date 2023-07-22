# cbnf
The CBNF (CB network format) neural network header format.

## What is CBNF?

CBNF is a neural network header format for use with [efficiently updatable neural networks](https://en.wikipedia.org/wiki/Efficiently_updatable_neural_network) for chess. It is in a very early stage of development, and is currently subject to change.

## Data Format

CBNF is a fixed-size 256-byte header format, shown here with C++ and Rust structs:

```cpp
struct __attribute__((packed)) CBNFHeader {
	std::array<char, 4> magic;
	std::uint8_t version;
	std::uint16_t flags;
	std::uint8_t layerCount;
	std::array<std::uint16_t, 32> layerSize;
	std::array<std::uint8_t, 32> layerQuantization;
	std::array<std::uint8_t, 32> activations;
	std::array<std::uint8_t, 64> inputKingBucketing;
	std::uint8_t outputBuckets;
	std::array<std::uint8_t, 6> reserved;
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
    pub layer_count: u8,
	pub layer_size: [u16, 32],
	pub layer_quantization: [u8, 32],
	pub activations: [u8, 32],
	pub input_king_bucketing: [u8, 64],
    pub output_buckets: u8,
	pub reserved: [u8; 6],
    pub name_len: u8,
    pub name: [u8; 48],
}
```

The magic bytes must equal `CBNF` (0x43, 0x32, 0x4E, 0x46), or the header is considered ill-formed, no diagnostic required, instant UB, nasal demons, etc etc.

The indices of the input bucket map (`inputKingBucketing`/`input_king_bucketing`) correspond to squares (rank-major, 0 = A1), and the values correspond to bucket indices.

## Existing Implementations

### cbnf-rs
This repository contains a Rust implementation of CBNF as a crate that provides rudimentary convenience methods, such as header parsing and utf-8 name validation.

### cbnf++
This repository also contains a header-only C++17 implementation of CBNF that provides almost the same convenience methods as cbnf-rs. cbnf++ is usable as-is, but primarily intended as an example, and it will not compile on MSVC. Due to the awful state of Unicode support in standard C++, it does not make an attempt to validate UTF-8 network names.

### c-bnf
This repository also contains a header-only C11 implementation of CBNF that provides the same convenience methods as cbnf++. Like cbnf++, c-bnf is usable as-is, but primarily intended as an example, and it will not compile on MSVC. Due to the nonexistent state of Unicode support in standard C, it does not make an attempt to validate UTF-8 network names.
