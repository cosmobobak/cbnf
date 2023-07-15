#pragma once

#include <cstdint>
#include <array>
#include <string_view>

namespace cbnfpp {
    constexpr std::size_t MaxLayerCount = 32;

    enum class Activation : std::uint8_t {
        Relu = 0,
        Crelu,
        Screlu,
        Sigmoid,
        Tanh
    };

    struct Flags {
        static constexpr std::uint16_t ZstdCompressed = 0x0001;
        static constexpr std::uint16_t Relative = 0x0002;
        static constexpr std::uint16_t Half = 0x0004;
        static constexpr std::uint16_t HorizontallyMirrored = 0x0008;

        static constexpr std::uint16_t ArchMask
            = Relative
            | Half
            | HorizontallyMirrored;

        static constexpr std::uint16_t All
            = ZstdCompressed
            | Relative
            | Half
            | HorizontallyMirrored;
    };

    struct __attribute__((packed)) CbnfHeader {
        std::array<char, 4> magic;
        std::uint8_t version;
        std::uint16_t flags;
        std::uint8_t layerCount;
        std::array<std::uint16_t, MaxLayerCount> layerSize;
        std::array<std::uint8_t, MaxLayerCount> layerQuantization;
        std::array<Activation, MaxLayerCount> activations;
        std::array<std::uint8_t, 64> inputKingBucketing;
        std::uint8_t outputBuckets;
        [[maybe_unused]] std::array<std::uint8_t, 6> reserved;
        std::uint8_t nameLen;
        std::array<char, 48> name;

        [[nodiscard]] inline std::string_view getName() const {
            return {name.data(), std::min<std::size_t>(nameLen, 47)};
        }

        [[nodiscard]] inline std::uint16_t getArchFlags() const {
            return flags & Flags::ArchMask;
        }
    };

    static_assert(sizeof(CbnfHeader) == 256);

    constexpr std::uint16_t SupportedHeaderVersion = 2;

    // Parse a CBNF header from a region of data
    // Returns `nullptr` if: 
    //   - the data is too short
    //   - the magic bytes are incorrect
    //   - the header version is not supported by this version of cbnf++
    //   - if `validate` is true:
    //     - any undefined flags are set
    //     - the number of output buckets is 0
    //     - the number of hidden layers is 0 or greater than 32
    //     - any of the hidden layer sizes within the number of hidden layers are 0
    //     - the network name is not null-terminated
    [[nodiscard]] inline const CbnfHeader *parseHeader(const void *data, std::size_t len, bool validate = true) noexcept {
        if (len < sizeof(CbnfHeader)) {
            return nullptr;
        }

        const auto &header = *reinterpret_cast<const CbnfHeader *>(data);

        if (header.magic != std::array{'C', 'B', 'N', 'F'}) {
            return nullptr;
        }

        if (header.version != SupportedHeaderVersion) {
            return nullptr;
        }

        if (validate) {
            if ((header.flags & Flags::All) != header.flags) {
                return nullptr;
            }

            if (header.outputBuckets == 0) {
                return nullptr;
            }

            if (header.layerCount == 0 || header.layerCount > MaxLayerCount) {
                return nullptr;
            }

            for (std::uint8_t layer = 0; layer < header.layerCount; ++layer) {
                if (header.layerSize[layer] == 0) {
                    return nullptr;
                }
            }

            const auto nameLen = std::min<std::size_t>(header.nameLen, 47);

            if (header.name[nameLen] != '\0') {
                return nullptr;
            }
        }

        return &header;
    }
}
