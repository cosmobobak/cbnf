#![warn(clippy::all, clippy::pedantic, clippy::nursery, clippy::cargo)]
#![deny(clippy::unwrap_used, clippy::expect_used)]
#![no_std]

/// The header of a CBNF file.
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

const _ASSERT_SIZE: () = assert!(core::mem::size_of::<CBNFHeader>() == 64);

impl CBNFHeader {
    /// Parse a CBNF header from a byte slice.
    /// Returns `None` if the data is too short or the magic number is incorrect.
    #[must_use]
    pub fn parse(data: &[u8]) -> Option<&Self> {
        if data.len() < core::mem::size_of::<Self>() {
            return None;
        }

        if &data[0..4] != b"CBNF" {
            return None;
        }

        // SAFETY: We just checked that the data is at least as long as the header,
        // and the header is packed, so there are no padding or alignment issues.
        let header = unsafe { &*data.as_ptr().cast::<Self>() };

        Some(header)
    }

    /// Get the name of the network as a UTF-8 string.
    /// Truncates the name if the length field indicates a name longer than the
    /// 48 bytes available in the header.
    ///
    /// # Errors
    ///
    /// Returns an error if the name is not valid UTF-8.
    pub fn name(&self) -> Result<&str, core::str::Utf8Error> {
        let rhs = core::cmp::min(self.name_len as usize, self.name.len());
        core::str::from_utf8(&self.name[0..rhs])
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
