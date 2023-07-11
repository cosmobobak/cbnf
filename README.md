# cbnf
The CBNF (catboy network format) neural network header format.

## What is CBNF?

CBNF is a neural network header format for use with [efficiently updatable neural networks](https://en.wikipedia.org/wiki/Efficiently_updatable_neural_network) for chess. It is in a very early stage of development, and is currently subject to change.

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