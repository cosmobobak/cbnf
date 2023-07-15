#![warn(clippy::all, clippy::pedantic, clippy::nursery, clippy::cargo)]
#![deny(clippy::unwrap_used, clippy::expect_used)]

pub const MAX_LAYER_COUNT: u8 = 32;

#[repr(u8)]
pub enum Activation {
    RELU = 0,
    CRELU,
    SCRELU,
    SIGMOID,
    TANH
}

bitflags::bitflags! {
    pub struct Flags: u16 {
        const ZSTD_COMPRESSED = 0x0001;
        const RELATIVE = 0x0002;
        const HALF = 0x0004;
        const HORIZONTALLY_MIRRORED = 0x0008;

        const ARCH_MASK = Self::RELATIVE.bits()
            | Self::HALF.bits()
            | Self::HORIZONTALLY_MIRRORED.bits();

        const ALL = Self::ZSTD_COMPRESSED.bits()
            | Self::RELATIVE.bits()
            | Self::HALF.bits()
            | Self::HORIZONTALLY_MIRRORED.bits();
    }
}

/// The header of a CBNF file.
#[repr(C, packed)]
pub struct CBNFHeader {
    pub magic: [u8; 4],
    pub version: u8,
    pub flags: Flags,
    pub layer_count: u8,
    pub layer_size: [u16; MAX_LAYER_COUNT as usize],
    pub layer_quantization: [u8; MAX_LAYER_COUNT as usize],
    pub activations: [Activation; MAX_LAYER_COUNT as usize],
    pub input_king_bucketing: [u8; 64],
    pub output_buckets: u8,
    pub reserved: [u8; 6],
    pub name_len: u8,
    pub name: [u8; 48],
}

const _ASSERT_SIZE: () = assert!(core::mem::size_of::<CBNFHeader>() == 256);

pub const SUPPORTED_HEADER_VERSION: u8 = 2;

impl CBNFHeader {
    /// Parse a CBNF header from a byte slice.
    /// Returns `None` if the data is too short or the magic number is incorrect.
    #[must_use]
    pub fn parse(data: &[u8], validate: bool) -> Option<&Self> {
        if data.len() < core::mem::size_of::<Self>() {
            return None;
        }

        if &data[0..4] != b"CBNF" {
            return None;
        }

        // SAFETY: We just checked that the data is at least as long as the header,
        // and the header is packed, so there are no padding or alignment issues.
        let header = unsafe { &*data.as_ptr().cast::<Self>() };

        if header.version != SUPPORTED_HEADER_VERSION {
            return None;
        }

        if validate {
            if (header.flags & Flags::ALL) != header.flags {
                return None;
            }

            if header.output_buckets == 0 {
                return None;
            }

            if header.layer_count == 0 || header.layer_count > MAX_LAYER_COUNT {
                return None;
            }

            for layer in 0..header.layer_count {
                if header.layer_size[layer as usize] == 0 {
                    return None;
                }
            }

            if header.name[header.name_len as usize] != 0 {
                return None;
            }
        }

        Some(header)
    }

    /// Get the name of the network as a UTF-8 string.
    /// Truncates the name if the length field indicates a name longer than the
    /// 47 bytes available in the header.
    ///
    /// # Errors
    ///
    /// Returns an error if the name is not valid UTF-8.
    pub fn name(&self) -> Result<&str, core::str::Utf8Error> {
        let rhs = core::cmp::min(self.name_len as usize, 47usize);
        core::str::from_utf8(&self.name[0..rhs])
    }

    /// Get the flags of this network that are relevant to the architecture.
    pub fn arch_flags(&self) -> u16 {
        self.flags & Flags::ARCH_MASK
    }

    /// Get the header as a byte slice.
    #[must_use]
    pub const fn as_bytes(&self) -> &[u8] {
        const LEN: usize = core::mem::size_of::<CBNFHeader>();
        let data = (self as *const Self).cast::<u8>();
        // SAFETY: The header is packed, so there are no padding or alignment issues.
        unsafe { core::slice::from_raw_parts(data, LEN) }
    }
}
