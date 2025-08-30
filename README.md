# dynamic_int.h

[![Version](https://img.shields.io/badge/version-v1.0.0-blue.svg)](https://github.com/edadma/dynamic_int.h/releases)
[![Language](https://img.shields.io/badge/language-C11-blue.svg)](https://en.cppreference.com/w/c/11)
[![License](https://img.shields.io/badge/license-MIT%20OR%20Unlicense-green.svg)](#license)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS%20%7C%20MCU-lightgrey.svg)](#platform-support)

A single-header C library implementing arbitrary precision integers with reference counting, designed for seamless integration with MCU projects that need automatic promotion from fixed-size integers to arbitrary precision on overflow.

## Features

- **Single Header**: Easy integration - just include `dynamic_int.h`
- **Reference Counting**: Automatic memory management
- **Configurable**: Customizable memory allocators and limb sizes
- **MCU Friendly**: Designed for embedded systems
- **Overflow Detection**: Helper functions to detect fixed-size arithmetic overflow

## Quick Start

```c
#define DI_IMPLEMENTATION
#include "dynamic_int.h"

int main() {
    // Create big integers
    di_int a = di_from_int32(42);
    di_int b = di_from_int32(100);
    
    // Perform arithmetic
    di_int sum = di_add(a, b);
    
    // Convert back to native types
    int32_t result;
    if (di_to_int32(sum, &result)) {
        printf("Sum: %d\n", result);
    } else {
        char* str = di_to_string(sum, 10);
        printf("Sum (too large): %s\n", str);
        free(str);
    }
    
    // Clean up
    di_release(&a);
    di_release(&b);
    di_release(&sum);
    
    return 0;
}
```

## Configuration

Customize the library by defining these macros before including:

```c
#define DI_MALLOC malloc         // Custom allocator
#define DI_REALLOC realloc       // Custom reallocator  
#define DI_FREE free             // Custom deallocator
#define DI_ASSERT assert         // Custom assert macro
#define DI_LIMB_BITS 32          // Bits per limb (16 or 32)

#define DI_IMPLEMENTATION
#include "dynamic_int.h"
```

## Building

### With CMake

```bash
mkdir build && cd build
cmake ..
make
./tests  # Run unit tests
```

### Manual Compilation

```bash
gcc -std=c11 -Wall -Wextra main.c -o tests
```

## API Overview

### Creation and Memory Management

- `di_from_int32()`, `di_from_int64()` - Create from native integers
- `di_from_string()` - Create from string representation
- `di_zero()`, `di_one()` - Create common constants
- `di_retain()` - Increment reference count
- `di_release()` - Decrement reference count and free if needed

### Arithmetic Operations

- `di_add()`, `di_sub()`, `di_mul()` - Basic arithmetic
- `di_add_i32()`, `di_mul_i32()` - Mixed-type arithmetic
- `di_negate()`, `di_abs()` - Unary operations
- `di_pow()` - Exponentiation

### Comparison and Conversion

- `di_compare()`, `di_eq()`, `di_lt()` - Comparison functions
- `di_to_int32()`, `di_to_int64()` - Convert to native types (with overflow check)
- `di_to_string()` - Convert to string representation
- `di_to_double()` - Convert to floating point

### Overflow Detection Helpers

- `di_add_overflow_int32()` - Detect int32 addition overflow
- `di_multiply_overflow_int64()` - Detect int64 multiplication overflow
- And more...

## Memory Management

The library uses reference counting for automatic memory management:

```c
di_int a = di_from_int32(42);  // ref_count = 1
di_int b = di_retain(a);       // ref_count = 2
di_release(&a);                 // ref_count = 1, a = NULL
di_release(&b);                 // ref_count = 0, memory freed, b = NULL
```

## License

This project is dual-licensed under:
- MIT License
- The Unlicense

Choose whichever license works best for your project.

## Platform Support

This library supports:
- **Linux**: Full support with GCC and Clang
- **Windows**: Compatible with MSVC and MinGW
- **macOS**: Native Clang support
- **MCU**: Designed for microcontroller environments

## Version History

### v1.0.0 (August 2025)
- ✅ **Complete arithmetic implementation**: Full arbitrary precision multiplication, division, modulo
- ✅ **Comprehensive test suite**: 76+ unit tests with mathematically verified results
- ✅ **String conversion**: Proper base-10 string parsing and generation
- ✅ **Large number support**: Tested with 50+ digit numbers
- ✅ **Memory management**: Robust reference counting system
- ✅ **Documentation**: Complete API documentation and examples

### v0.1.0 (Initial Release)
- Basic integer creation and reference counting
- Simple arithmetic operations
- String conversion framework
- Initial test infrastructure

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.