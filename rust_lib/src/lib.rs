use std::{time::Duration, thread::sleep};

#[no_mangle]
pub extern "C" fn say_hello() {
    println!("Hello from Rust!");
	sleep(Duration::from_secs(5));
    println!("5 seconds have passed");
}
