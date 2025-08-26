# Dynamic BigInt

A single-header C library implementing arbitrary precision integers with reference counting, designed for seamless integration with MCU projects that need automatic promotion from fixed-size integers to arbitrary precision on overflow.

## Features

- **Single Header**: Easy integration - just include `dynamic_bigint.h`
- **Reference Counting**: Automatic memory management
- **Configurable**: Customizable memory allocators and limb sizes
- **MCU Friendly**: Designed for embedded systems
- **Overflow Detection**: Helper functions to detect fixed-size arithmetic overflow

## Quick Start

```c
#define DB_IMPLEMENTATION
#include "dynamic_bigint.h"

int main() {
    // Create big integers
    db_bigint a = db_from_int32(42);
    db_bigint b = db_from_int32(100);
    
    // Perform arithmetic
    db_bigint sum = db_add(a, b);
    
    // Convert back to native types
    int32_t result;
    if (db_to_int32(sum, &result)) {
        printf("Sum: %d\n", result);
    } else {
        char* str = db_to_string(sum, 10);
        printf("Sum (too large): %s\n", str);
        free(str);
    }
    
    // Clean up
    db_release(&a);
    db_release(&b);
    db_release(&sum);
    
    return 0;
}
```

## Configuration

Customize the library by defining these macros before including:

```c
#define DB_MALLOC malloc         // Custom allocator
#define DB_REALLOC realloc       // Custom reallocator  
#define DB_FREE free             // Custom deallocator
#define DB_ASSERT assert         // Custom assert macro
#define DB_LIMB_BITS 32          // Bits per limb (16 or 32)

#define DB_IMPLEMENTATION
#include "dynamic_bigint.h"
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
gcc -std=c99 -Wall -Wextra main.c -o tests
```

## API Overview

### Creation and Memory Management

- `db_from_int32()`, `db_from_int64()` - Create from native integers
- `db_from_string()` - Create from string representation
- `db_zero()`, `db_one()` - Create common constants
- `db_retain()` - Increment reference count
- `db_release()` - Decrement reference count and free if needed

### Arithmetic Operations

- `db_add()`, `db_subtract()`, `db_multiply()` - Basic arithmetic
- `db_add_int32()`, `db_multiply_int32()` - Mixed-type arithmetic
- `db_negate()`, `db_abs()` - Unary operations
- `db_pow()` - Exponentiation

### Comparison and Conversion

- `db_compare()`, `db_equal()`, `db_less()` - Comparison functions
- `db_to_int32()`, `db_to_int64()` - Convert to native types (with overflow check)
- `db_to_string()` - Convert to string representation
- `db_to_double()` - Convert to floating point

### Overflow Detection Helpers

- `db_add_overflow_int32()` - Detect int32 addition overflow
- `db_multiply_overflow_int64()` - Detect int64 multiplication overflow
- And more...

## Memory Management

The library uses reference counting for automatic memory management:

```c
db_bigint a = db_from_int32(42);  // ref_count = 1
db_bigint b = db_retain(a);       // ref_count = 2
db_release(&a);                   // ref_count = 1, a = NULL
db_release(&b);                   // ref_count = 0, memory freed, b = NULL
```

## License

This project is dual-licensed under:
- MIT License
- The Unlicense

Choose whichever license works best for your project.

## Status

This library is currently in development. Basic arithmetic operations are implemented, with more advanced features planned.

## Contributing

Contributions are welcome! Please feel free to submit issues and pull requests.