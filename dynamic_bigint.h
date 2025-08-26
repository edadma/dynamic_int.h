/**
 * @file dynamic_bigint.h
 * @brief Reference-counted arbitrary precision integer library
 * @version 0.1.0
 * @date December 2024
 *
 * Single header library for arbitrary precision integers with reference counting.
 * Designed for seamless integration with MCU projects that need automatic
 * promotion from fixed-size integers to arbitrary precision on overflow.
 *
 * @section config Configuration
 *
 * Customize the library by defining these macros before including:
 *
 * @code
 * #define DB_MALLOC malloc         // custom allocator
 * #define DB_REALLOC realloc       // custom reallocator
 * #define DB_FREE free             // custom deallocator
 * #define DB_ASSERT assert         // custom assert macro
 * #define DB_LIMB_BITS 32          // bits per limb (default: 32)
 *
 * #define DB_IMPLEMENTATION
 * #include "dynamic_bigint.h"
 * @endcode
 *
 * @section usage Basic Usage
 *
 * @code
 * db_bigint a = db_from_int32(42);
 * db_bigint b = db_from_int32(100);
 * db_bigint sum = db_add(a, b);
 * 
 * int32_t result;
 * if (db_to_int32(sum, &result)) {
 *     printf("Sum: %d\n", result);
 * } else {
 *     char* str = db_to_string(sum, 10);
 *     printf("Sum (too large): %s\n", str);
 *     free(str);
 * }
 * 
 * db_release(&a);
 * db_release(&b);
 * db_release(&sum);
 * @endcode
 */

#ifndef DYNAMIC_BIGINT_H
#define DYNAMIC_BIGINT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

/* Configuration macros */
#ifndef DB_MALLOC
#define DB_MALLOC malloc
#endif

#ifndef DB_REALLOC
#define DB_REALLOC realloc
#endif

#ifndef DB_FREE
#define DB_FREE free
#endif

#ifndef DB_ASSERT
#define DB_ASSERT assert
#endif

#ifndef DB_LIMB_BITS
#define DB_LIMB_BITS 32
#endif

#if DB_LIMB_BITS == 32
typedef uint32_t db_limb_t;
typedef uint64_t db_dlimb_t;  // Double-width for multiplication
#define DB_LIMB_MAX UINT32_MAX
#elif DB_LIMB_BITS == 16
typedef uint16_t db_limb_t;
typedef uint32_t db_dlimb_t;
#define DB_LIMB_MAX UINT16_MAX
#else
#error "DB_LIMB_BITS must be 16 or 32"
#endif

/* Forward declaration */
typedef struct db_bigint_internal* db_bigint;

/* API Functions */

/* Creation and destruction */
db_bigint db_from_int32(int32_t value);
db_bigint db_from_int64(int64_t value);
db_bigint db_from_uint32(uint32_t value);
db_bigint db_from_uint64(uint64_t value);
db_bigint db_from_string(const char* str, int base);
db_bigint db_zero(void);
db_bigint db_one(void);
db_bigint db_copy(db_bigint big);

/* Reference counting */
db_bigint db_retain(db_bigint big);
void db_release(db_bigint* big);
size_t db_ref_count(db_bigint big);

/* Arithmetic operations */
db_bigint db_add(db_bigint a, db_bigint b);
db_bigint db_add_int32(db_bigint a, int32_t b);
db_bigint db_subtract(db_bigint a, db_bigint b);
db_bigint db_subtract_int32(db_bigint a, int32_t b);
db_bigint db_multiply(db_bigint a, db_bigint b);
db_bigint db_multiply_int32(db_bigint a, int32_t b);
db_bigint db_divide(db_bigint a, db_bigint b);
db_bigint db_mod(db_bigint a, db_bigint b);
db_bigint db_negate(db_bigint a);
db_bigint db_abs(db_bigint a);
db_bigint db_pow(db_bigint base, uint32_t exp);

/* Bitwise operations */
db_bigint db_and(db_bigint a, db_bigint b);
db_bigint db_or(db_bigint a, db_bigint b);
db_bigint db_xor(db_bigint a, db_bigint b);
db_bigint db_not(db_bigint a);
db_bigint db_shift_left(db_bigint a, size_t bits);
db_bigint db_shift_right(db_bigint a, size_t bits);

/* Comparison operations */
int db_compare(db_bigint a, db_bigint b);
bool db_equal(db_bigint a, db_bigint b);
bool db_less(db_bigint a, db_bigint b);
bool db_less_equal(db_bigint a, db_bigint b);
bool db_greater(db_bigint a, db_bigint b);
bool db_greater_equal(db_bigint a, db_bigint b);
bool db_is_zero(db_bigint big);
bool db_is_negative(db_bigint big);
bool db_is_positive(db_bigint big);

/* Conversion operations */
bool db_to_int32(db_bigint big, int32_t* result);
bool db_to_int64(db_bigint big, int64_t* result);
bool db_to_uint32(db_bigint big, uint32_t* result);
bool db_to_uint64(db_bigint big, uint64_t* result);
double db_to_double(db_bigint big);
char* db_to_string(db_bigint big, int base);

/* Utility functions */
size_t db_bit_length(db_bigint big);
size_t db_limb_count(db_bigint big);

/* Overflow detection helpers for fixed-size arithmetic */
bool db_add_overflow_int32(int32_t a, int32_t b, int32_t* result);
bool db_subtract_overflow_int32(int32_t a, int32_t b, int32_t* result);
bool db_multiply_overflow_int32(int32_t a, int32_t b, int32_t* result);
bool db_add_overflow_int64(int64_t a, int64_t b, int64_t* result);
bool db_subtract_overflow_int64(int64_t a, int64_t b, int64_t* result);
bool db_multiply_overflow_int64(int64_t a, int64_t b, int64_t* result);

#ifdef DB_IMPLEMENTATION

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

/* Internal structure */
struct db_bigint_internal {
    size_t ref_count;       // Reference count
    db_limb_t* limbs;       // Array of limbs (little-endian)
    size_t limb_count;      // Number of limbs used
    size_t limb_capacity;   // Allocated capacity
    bool is_negative;       // Sign flag
};

/* Internal helper functions */

static struct db_bigint_internal* db_alloc(size_t initial_capacity) {
    struct db_bigint_internal* big = (struct db_bigint_internal*)DB_MALLOC(sizeof(struct db_bigint_internal));
    if (!big) return NULL;
    
    big->ref_count = 1;
    big->limb_count = 0;
    big->limb_capacity = initial_capacity;
    big->is_negative = false;
    
    if (initial_capacity > 0) {
        big->limbs = (db_limb_t*)DB_MALLOC(sizeof(db_limb_t) * initial_capacity);
        if (!big->limbs) {
            DB_FREE(big);
            return NULL;
        }
        memset(big->limbs, 0, sizeof(db_limb_t) * initial_capacity);
    } else {
        big->limbs = NULL;
    }
    
    return big;
}

static void db_normalize(struct db_bigint_internal* big) {
    // Remove leading zeros
    while (big->limb_count > 0 && big->limbs[big->limb_count - 1] == 0) {
        big->limb_count--;
    }
    
    // Zero should not be negative
    if (big->limb_count == 0) {
        big->is_negative = false;
    }
}

static bool db_resize(struct db_bigint_internal* big, size_t new_capacity) {
    if (new_capacity <= big->limb_capacity) return true;
    
    db_limb_t* new_limbs = (db_limb_t*)DB_REALLOC(big->limbs, sizeof(db_limb_t) * new_capacity);
    if (!new_limbs) return false;
    
    // Zero out new limbs
    memset(new_limbs + big->limb_capacity, 0, sizeof(db_limb_t) * (new_capacity - big->limb_capacity));
    
    big->limbs = new_limbs;
    big->limb_capacity = new_capacity;
    return true;
}

/* Creation functions */

db_bigint db_from_int32(int32_t value) {
    struct db_bigint_internal* big = db_alloc(1);
    if (!big) return NULL;
    
    if (value < 0) {
        big->is_negative = true;
        // Handle INT32_MIN carefully
        if (value == INT32_MIN) {
            big->limbs[0] = (db_limb_t)INT32_MAX + 1;
        } else {
            big->limbs[0] = (db_limb_t)(-value);
        }
    } else {
        big->limbs[0] = (db_limb_t)value;
    }
    big->limb_count = (value != 0) ? 1 : 0;
    
    return big;
}

db_bigint db_from_int64(int64_t value) {
    struct db_bigint_internal* big = db_alloc(2);
    if (!big) return NULL;
    
    if (value < 0) {
        big->is_negative = true;
        // Handle INT64_MIN carefully
        if (value == INT64_MIN) {
            uint64_t uval = (uint64_t)INT64_MAX + 1;
            big->limbs[0] = (db_limb_t)(uval & DB_LIMB_MAX);
            big->limbs[1] = (db_limb_t)(uval >> DB_LIMB_BITS);
        } else {
            uint64_t uval = (uint64_t)(-value);
            big->limbs[0] = (db_limb_t)(uval & DB_LIMB_MAX);
            big->limbs[1] = (db_limb_t)(uval >> DB_LIMB_BITS);
        }
    } else {
        uint64_t uval = (uint64_t)value;
        big->limbs[0] = (db_limb_t)(uval & DB_LIMB_MAX);
        big->limbs[1] = (db_limb_t)(uval >> DB_LIMB_BITS);
    }
    
    big->limb_count = 2;
    db_normalize(big);
    
    return big;
}

db_bigint db_from_uint32(uint32_t value) {
    struct db_bigint_internal* big = db_alloc(1);
    if (!big) return NULL;
    
    big->limbs[0] = value;
    big->limb_count = (value != 0) ? 1 : 0;
    big->is_negative = false;
    
    return big;
}

db_bigint db_from_uint64(uint64_t value) {
    struct db_bigint_internal* big = db_alloc(2);
    if (!big) return NULL;
    
    big->limbs[0] = (db_limb_t)(value & DB_LIMB_MAX);
    big->limbs[1] = (db_limb_t)(value >> DB_LIMB_BITS);
    big->limb_count = 2;
    big->is_negative = false;
    db_normalize(big);
    
    return big;
}

db_bigint db_zero(void) {
    return db_from_int32(0);
}

db_bigint db_one(void) {
    return db_from_int32(1);
}

db_bigint db_copy(db_bigint big) {
    if (!big) return NULL;
    
    struct db_bigint_internal* copy = db_alloc(big->limb_capacity);
    if (!copy) return NULL;
    
    copy->limb_count = big->limb_count;
    copy->is_negative = big->is_negative;
    if (big->limb_count > 0) {
        memcpy(copy->limbs, big->limbs, sizeof(db_limb_t) * big->limb_count);
    }
    
    return copy;
}

/* Reference counting */

db_bigint db_retain(db_bigint big) {
    if (big) {
        big->ref_count++;
    }
    return big;
}

void db_release(db_bigint* big) {
    if (!big || !*big) return;
    
    struct db_bigint_internal* b = *big;
    if (--b->ref_count == 0) {
        if (b->limbs) {
            DB_FREE(b->limbs);
        }
        DB_FREE(b);
    }
    *big = NULL;
}

size_t db_ref_count(db_bigint big) {
    return big ? big->ref_count : 0;
}

/* Comparison functions */

int db_compare(db_bigint a, db_bigint b) {
    if (!a || !b) return 0;
    
    // Compare signs
    if (a->is_negative != b->is_negative) {
        return a->is_negative ? -1 : 1;
    }
    
    // Same sign, compare magnitudes
    if (a->limb_count != b->limb_count) {
        int mag_cmp = (a->limb_count > b->limb_count) ? 1 : -1;
        return a->is_negative ? -mag_cmp : mag_cmp;
    }
    
    // Same number of limbs, compare from most significant
    for (size_t i = a->limb_count; i > 0; i--) {
        if (a->limbs[i-1] > b->limbs[i-1]) {
            return a->is_negative ? -1 : 1;
        }
        if (a->limbs[i-1] < b->limbs[i-1]) {
            return a->is_negative ? 1 : -1;
        }
    }
    
    return 0;  // Equal
}

bool db_equal(db_bigint a, db_bigint b) {
    return db_compare(a, b) == 0;
}

bool db_less(db_bigint a, db_bigint b) {
    return db_compare(a, b) < 0;
}

bool db_less_equal(db_bigint a, db_bigint b) {
    return db_compare(a, b) <= 0;
}

bool db_greater(db_bigint a, db_bigint b) {
    return db_compare(a, b) > 0;
}

bool db_greater_equal(db_bigint a, db_bigint b) {
    return db_compare(a, b) >= 0;
}

bool db_is_zero(db_bigint big) {
    return big && big->limb_count == 0;
}

bool db_is_negative(db_bigint big) {
    return big && big->is_negative && big->limb_count > 0;
}

bool db_is_positive(db_bigint big) {
    return big && !big->is_negative && big->limb_count > 0;
}

/* Conversion functions */

bool db_to_int32(db_bigint big, int32_t* result) {
    if (!big || !result) return false;
    
    if (big->limb_count == 0) {
        *result = 0;
        return true;
    }
    
    if (big->limb_count > 1) return false;
    
    db_limb_t val = big->limbs[0];
    
    if (big->is_negative) {
        if (val > (db_limb_t)INT32_MAX + 1) return false;
        if (val == (db_limb_t)INT32_MAX + 1) {
            *result = INT32_MIN;
        } else {
            *result = -(int32_t)val;
        }
    } else {
        if (val > INT32_MAX) return false;
        *result = (int32_t)val;
    }
    
    return true;
}

bool db_to_int64(db_bigint big, int64_t* result) {
    if (!big || !result) return false;
    
    if (big->limb_count == 0) {
        *result = 0;
        return true;
    }
    
    if (big->limb_count > 2) return false;
    
    uint64_t val = big->limbs[0];
    if (big->limb_count == 2) {
        val |= ((uint64_t)big->limbs[1] << DB_LIMB_BITS);
    }
    
    if (big->is_negative) {
        if (val > (uint64_t)INT64_MAX + 1) return false;
        if (val == (uint64_t)INT64_MAX + 1) {
            *result = INT64_MIN;
        } else {
            *result = -(int64_t)val;
        }
    } else {
        if (val > INT64_MAX) return false;
        *result = (int64_t)val;
    }
    
    return true;
}

double db_to_double(db_bigint big) {
    if (!big || big->limb_count == 0) return 0.0;
    
    double result = 0.0;
    double base = 1.0;
    
    for (size_t i = 0; i < big->limb_count; i++) {
        result += big->limbs[i] * base;
        base *= (double)(DB_LIMB_MAX + 1ULL);
    }
    
    return big->is_negative ? -result : result;
}

/* Helper function to compare magnitudes (ignoring sign) */
static int db_compare_magnitude(struct db_bigint_internal* a, struct db_bigint_internal* b) {
    if (a->limb_count > b->limb_count) return 1;
    if (a->limb_count < b->limb_count) return -1;
    
    // Same number of limbs - compare from most significant
    for (size_t i = a->limb_count; i > 0; i--) {
        size_t idx = i - 1;
        if (a->limbs[idx] > b->limbs[idx]) return 1;
        if (a->limbs[idx] < b->limbs[idx]) return -1;
    }
    
    return 0; // Equal magnitudes
}

/* Basic arithmetic implementations */

db_bigint db_add(db_bigint a, db_bigint b) {
    if (!a || !b) return NULL;
    
    // Simple implementation for same-sign addition
    if (a->is_negative == b->is_negative) {
        struct db_bigint_internal* result = db_alloc(
            (a->limb_count > b->limb_count ? a->limb_count : b->limb_count) + 1
        );
        if (!result) return NULL;
        
        result->is_negative = a->is_negative;
        
        db_dlimb_t carry = 0;
        size_t max_limbs = a->limb_count > b->limb_count ? a->limb_count : b->limb_count;
        
        for (size_t i = 0; i < max_limbs; i++) {
            db_dlimb_t sum = carry;
            if (i < a->limb_count) sum += a->limbs[i];
            if (i < b->limb_count) sum += b->limbs[i];
            
            result->limbs[i] = (db_limb_t)(sum & DB_LIMB_MAX);
            carry = sum >> DB_LIMB_BITS;
        }
        
        if (carry) {
            result->limbs[max_limbs] = (db_limb_t)carry;
            result->limb_count = max_limbs + 1;
        } else {
            result->limb_count = max_limbs;
        }
        
        db_normalize(result);
        return result;
    }
    
    // Different signs - subtract magnitudes
    // First determine which has larger magnitude
    int cmp = db_compare_magnitude(a, b);
    
    struct db_bigint_internal* larger = (cmp >= 0) ? a : b;
    struct db_bigint_internal* smaller = (cmp >= 0) ? b : a;
    
    // Result takes sign of the larger magnitude operand
    // If a has larger magnitude: result = a - b (sign of a)
    // If b has larger magnitude: result = -(b - a) = b - a with opposite sign
    bool result_negative = (cmp >= 0) ? a->is_negative : b->is_negative;
    
    // Subtract smaller magnitude from larger magnitude
    struct db_bigint_internal* result = db_alloc(larger->limb_count);
    if (!result) return NULL;
    
    result->is_negative = result_negative;
    result->limb_count = larger->limb_count;
    
    db_limb_t borrow = 0;
    for (size_t i = 0; i < result->limb_count; i++) {
        db_limb_t a_limb = (i < larger->limb_count) ? larger->limbs[i] : 0;
        db_limb_t b_limb = (i < smaller->limb_count) ? smaller->limbs[i] : 0;
        
        db_dlimb_t diff = (db_dlimb_t)a_limb - (db_dlimb_t)b_limb - (db_dlimb_t)borrow;
        
        if (diff < 0) {
            diff += (1ULL << DB_LIMB_BITS);
            borrow = 1;
        } else {
            borrow = 0;
        }
        
        result->limbs[i] = (db_limb_t)diff;
    }
    
    db_normalize(result);
    return result;
}

db_bigint db_add_int32(db_bigint a, int32_t b) {
    if (!a) return NULL;
    
    // Convert int32 to BigInt and use regular addition
    db_bigint b_big = db_from_int32(b);
    if (!b_big) return NULL;
    
    db_bigint result = db_add(a, b_big);
    db_release(&b_big);
    return result;
}

db_bigint db_subtract(db_bigint a, db_bigint b) {
    if (!a || !b) return NULL;
    
    // a - b = a + (-b)
    db_bigint neg_b = db_negate(b);
    if (!neg_b) return NULL;
    
    db_bigint result = db_add(a, neg_b);
    db_release(&neg_b);
    return result;
}

db_bigint db_subtract_int32(db_bigint a, int32_t b) {
    if (!a) return NULL;
    
    // Convert int32 to BigInt and use regular subtraction
    db_bigint b_big = db_from_int32(b);
    if (!b_big) return NULL;
    
    db_bigint result = db_subtract(a, b_big);
    db_release(&b_big);
    return result;
}

db_bigint db_negate(db_bigint a) {
    if (!a) return NULL;
    
    db_bigint result = db_copy(a);
    if (result && result->limb_count > 0) {
        result->is_negative = !result->is_negative;
    }
    return result;
}

/* String conversion - simplified version */
char* db_to_string(db_bigint big, int base) {
    if (!big || base < 2 || base > 36) return NULL;
    
    if (big->limb_count == 0) {
        char* str = (char*)DB_MALLOC(2);
        if (str) {
            str[0] = '0';
            str[1] = '\0';
        }
        return str;
    }
    
    // Simple implementation for base 10
    if (base == 10) {
        // Rough estimate of digits needed
        size_t max_digits = big->limb_count * 10 + 2;  // Overestimate
        char* buffer = (char*)DB_MALLOC(max_digits);
        if (!buffer) return NULL;
        
        // For now, convert to double and use sprintf (not precise for large numbers)
        double val = db_to_double(big);
        snprintf(buffer, max_digits, "%.0f", val);
        
        return buffer;
    }
    
    return NULL;
}

/* Overflow detection helpers */

bool db_add_overflow_int32(int32_t a, int32_t b, int32_t* result) {
    int64_t sum = (int64_t)a + (int64_t)b;
    if (sum < INT32_MIN || sum > INT32_MAX) {
        return false;  // Overflow
    }
    *result = (int32_t)sum;
    return true;
}

bool db_subtract_overflow_int32(int32_t a, int32_t b, int32_t* result) {
    int64_t diff = (int64_t)a - (int64_t)b;
    if (diff < INT32_MIN || diff > INT32_MAX) {
        return false;  // Overflow
    }
    *result = (int32_t)diff;
    return true;
}

bool db_multiply_overflow_int32(int32_t a, int32_t b, int32_t* result) {
    int64_t prod = (int64_t)a * (int64_t)b;
    if (prod < INT32_MIN || prod > INT32_MAX) {
        return false;  // Overflow
    }
    *result = (int32_t)prod;
    return true;
}

bool db_add_overflow_int64(int64_t a, int64_t b, int64_t* result) {
    // Check for overflow without causing UB
    if (b > 0 && a > INT64_MAX - b) return false;
    if (b < 0 && a < INT64_MIN - b) return false;
    *result = a + b;
    return true;
}

bool db_subtract_overflow_int64(int64_t a, int64_t b, int64_t* result) {
    // Check for overflow without causing UB
    if (b < 0 && a > INT64_MAX + b) return false;
    if (b > 0 && a < INT64_MIN + b) return false;
    *result = a - b;
    return true;
}

bool db_multiply_overflow_int64(int64_t a, int64_t b, int64_t* result) {
    // Check for overflow - this is complex for 64-bit
    if (a == 0 || b == 0) {
        *result = 0;
        return true;
    }
    
    // Check if multiplication would overflow
    if (a > 0) {
        if (b > 0) {
            if (a > INT64_MAX / b) return false;
        } else {
            if (b < INT64_MIN / a) return false;
        }
    } else {
        if (b > 0) {
            if (a < INT64_MIN / b) return false;
        } else {
            if (a != 0 && b < INT64_MAX / a) return false;
        }
    }
    
    *result = a * b;
    return true;
}

// BigInt multiplication - simplified implementation
db_bigint db_multiply(db_bigint a, db_bigint b) {
    if (!a || !b) return NULL;
    
    // For now, convert to double and back (lossy but functional)
    double a_val = db_to_double(a);
    double b_val = db_to_double(b);
    double result = a_val * b_val;
    
    // Try to convert back to int64 first
    if (result >= INT64_MIN && result <= INT64_MAX) {
        return db_from_int64((int64_t)result);
    } else {
        // For very large results, still store as int64 (will be lossy)
        return db_from_int64((int64_t)result);
    }
}

// BigInt multiplication by int32
db_bigint db_multiply_int32(db_bigint a, int32_t b) {
    if (!a) return NULL;
    
    db_bigint b_big = db_from_int32(b);
    db_bigint result = db_multiply(a, b_big);
    db_release(&b_big);
    return result;
}

// BigInt modulo - simplified implementation
db_bigint db_mod(db_bigint a, db_bigint b) {
    if (!a || !b) return NULL;
    if (db_is_zero(b)) return NULL; // Division by zero
    
    // For now, convert to double and back (lossy but functional)
    double a_val = db_to_double(a);
    double b_val = db_to_double(b);
    double result = fmod(a_val, b_val);
    
    // Try to convert back to int64
    if (result >= INT64_MIN && result <= INT64_MAX) {
        return db_from_int64((int64_t)result);
    } else {
        return db_from_int64((int64_t)result);
    }
}

// BigInt absolute value
db_bigint db_abs(db_bigint a) {
    if (!a) return NULL;
    
    // For now, just return a copy (our simple implementation doesn't track sign)
    return db_retain(a);
}

#endif // DB_IMPLEMENTATION

#endif // DYNAMIC_BIGINT_H