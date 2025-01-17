#![allow(dead_code)]

// Include bindgen output
// The bindings are freshly generated on Linux and MacOS builds.
// For Windows the prebuilt bindings.rs from the repository are used.
include!("bindings/bindings.rs");

#[repr(C)]
#[derive(Copy, Clone, Debug)]
pub struct EverCrypt_AEAD_state_s {
    r#impl: u8,
    ek: *mut u8,
}
