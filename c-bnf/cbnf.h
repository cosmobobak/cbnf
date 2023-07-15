#pragma once

#include <stdint.h>
#include <stddef.h>

#define CBNF_MAX_LAYER_COUNT (32)

enum CbnfActivation {
    CBNF_ACTIVATION_RELU = 0,
    CBNF_ACTIVATION_CRELU,
    CBNF_ACTIVATION_SCRELU,
    CBNF_ACTIVATION_SIGMOID,
    CBNF_ACTIVATION_TANH,
};

#define CBNF_FLAG_ZSTD_COMPRESSED (0x0001)
#define CBNF_FLAG_RELATIVE (0x0002)
#define CBNF_FLAG_HALF (0x0004)
#define CBNF_FLAG_HORIZONTALLY_MIRRORED (0x0008)

#define CBNF_ARCH_FLAGS (CBNF_FLAG_RELATIVE | CBNF_FLAG_HALF | CBNF_FLAG_HORIZONTALLY_MIRRORED)

#define CBNF_ALL_FLAGS (CBNF_FLAG_ZSTD_COMPRESSED | CBNF_FLAG_RELATIVE | CBNF_FLAG_HALF | CBNF_FLAG_HORIZONTALLY_MIRRORED)

typedef struct __attribute__((packed)) {
    char magic[4];
    uint8_t version;
    uint16_t flags;
    uint8_t layerCount;
    uint16_t layerSize[CBNF_MAX_LAYER_COUNT];
    uint8_t layerQuantization[CBNF_MAX_LAYER_COUNT];
    uint8_t activations[CBNF_MAX_LAYER_COUNT];
    uint8_t inputKingBucketing[64];
    uint8_t outputBuckets;
    uint8_t reserved[6];
    uint8_t nameLen;
    char name[48];
} CbnfHeader;

_Static_assert(sizeof(CbnfHeader) == 256, "sizeof(CbnfHeader) != 256");

#define CBNF_SUPPORTED_HEADER_VERSION (2)

// Parse a CBNF header from a region of data
// Returns `NULL` if: 
//   - the data is too short
//   - the magic bytes are incorrect
//   - the header version is not supported by this version of cbnf++
//   - if `validate` is true:
//     - any undefined flags are set
//     - the number of output buckets is 0
//     - the number of hidden layers is 0 or greater than 32
//     - any of the hidden layer sizes within the number of hidden layers are 0
//     - the network name is not null-terminated
const CbnfHeader *cbnfParseHeader(const void *data, size_t len, _Bool validate) {
    if (len < sizeof(CbnfHeader)) {
        return NULL;
    }

    // no actual need to explicitly cast here,
    // but do it for hypothetical c++ compatibility
    const CbnfHeader *header = (const CbnfHeader *)data;

    if (header->magic[0] != 'C'
        || header->magic[1] != 'B'
        || header->magic[2] != 'N'
        || header->magic[3] != 'F') {
        return NULL;
    }

    if (header->version != CBNF_SUPPORTED_HEADER_VERSION) {
        return NULL;
    }

    if (validate) {
        if ((header->flags & CBNF_ALL_FLAGS) != header->flags) {
            return NULL;
        }

        if (header->outputBuckets == 0) {
            return NULL;
        }

        if (header->layerCount == 0 || header->layerCount > CBNF_MAX_LAYER_COUNT) {
            return NULL;
        }

        for (uint8_t layer = 0; layer < header->layerCount; ++layer) {
            if (header->layerSize[layer] == 0) {
                return NULL;
            }
        }

        const size_t nameLen = header->nameLen > 47 ? 47 : header->nameLen;

        if (header->name[nameLen] != '\0') {
            return NULL;
        }
    }

    return header;
}
