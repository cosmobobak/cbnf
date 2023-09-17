The CBNF neural network header format.

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

## Header Fields

### Magic
The magic bytes are the first four bytes of the header, and must equal `CBNF`. This is used to verify that the header is well-formed.

### Version
The version field is a 16-bit unsigned integer that specifies the version of the CBNF format. The current version is 1.

### Flags
TBD

### Padding
The padding field is currently unused, and must be zero.

### Architecture
The architecture field is an 8-bit unsigned integer that specifies the architecture of the neural network. The currently defined architectures are:
TBD

### Activation
The activation field is an 8-bit unsigned integer that specifies the activation function of the neural network. The currently defined activation functions are:
0: Clipped ReLU
1: Squared Clipped ReLU

### Hidden Size
The hidden size field is a 16-bit unsigned integer that specifies the number of neurons in the hidden layer of the neural network.

### Input Buckets
The input buckets field is an 8-bit unsigned integer that specifies the number of input buckets in the neural network. This should be 1 for bucketless neural networks, and would be 64 for a HalfKA neural network.

### Output Buckets
The output buckets field is an 8-bit unsigned integer that specifies the number of output buckets in the neural network. This should be 1 for bucketless neural networks.

### Name Length
The name length field is an 8-bit unsigned integer that specifies the length of the name of the neural network, in bytes. The name length must be less than or equal to 48.

### Name
A buffer of 48 bytes that contains the name of the neural network. By this specification the name must be valid ASCII, but implementations are free and encouraged to support UTF-8 names.

## Existing Implementations

### cbnf-rs
This repository contains a Rust implementation of CBNF as a crate that provides rudimentary convenience methods, such as header parsing and utf-8 name validation.

### cbnf++
This repository also contains a header-only C++17 implementation of CBNF that provides almost the same convenience methods as cbnf-rs. cbnf++ is usable as-is, but primarily intended as an example, and it will not compile on MSVC. Due to the awful state of Unicode support in standard C++, it does not make an attempt to validate UTF-8 network names.

### c-bnf
This repository also contains a header-only C11 implementation of CBNF that provides the same convenience methods as cbnf++. Like cbnf++, c-bnf is usable as-is, but primarily intended as an example, and it will not compile on MSVC. Due to the nonexistent state of Unicode support in standard C, it does not make an attempt to validate UTF-8 network names.
