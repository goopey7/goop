#ifndef RUST_HELLO_LIB_H
#define RUST_HELLO_LIB_H
#include <cstdint>
#ifdef __cplusplus
extern "C" {
#endif

void say_hello(const char* name);

#ifdef __cplusplus
}
#endif
#endif // RUST_HELLO_LIB_H
