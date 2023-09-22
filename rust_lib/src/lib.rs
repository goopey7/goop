use std::ffi::{c_char, CStr};

#[no_mangle]
pub extern "C" fn say_hello(name: *const c_char)
{
	let c_str = unsafe { CStr::from_ptr(name) };
	println!("Hello to {} from Rust!", c_str.to_string_lossy());
}
