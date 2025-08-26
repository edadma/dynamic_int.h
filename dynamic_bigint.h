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

/* Advanced mathematical operations */
db_bigint db_mod_pow(db_bigint base, db_bigint exp, db_bigint mod);
db_bigint db_gcd(db_bigint a, db_bigint b);
db_bigint db_lcm(db_bigint a, db_bigint b);
db_bigint db_extended_gcd(db_bigint a, db_bigint b, db_bigint* x, db_bigint* y);
db_bigint db_sqrt(db_bigint n);
db_bigint db_factorial(uint32_t n);

/* Prime number functions */
bool db_is_prime(db_bigint n, int certainty);
db_bigint db_next_prime(db_bigint n);

/* Random number generation */
db_bigint db_random(size_t bits);
db_bigint db_random_range(db_bigint min, db_bigint max);

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

db_bigint db_from_string(const char* str, int base) {
    if (!str || base < 2 || base > 36) return NULL;
    
    // Skip whitespace
    while (*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r') str++;
    if (*str == '\0') return NULL;
    
    // Check for sign
    bool is_negative = false;
    if (*str == '-') {
        is_negative = true;
        str++;
    } else if (*str == '+') {
        str++;
    }
    
    // Skip leading zeros
    while (*str == '0') str++;
    if (*str == '\0') return db_zero();
    
    // Count valid digits to estimate capacity
    const char* p = str;
    size_t digit_count = 0;
    while (*p) {
        char c = *p;
        int digit_val;
        if (c >= '0' && c <= '9') {
            digit_val = c - '0';
        } else if (c >= 'a' && c <= 'z') {
            digit_val = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'Z') {
            digit_val = c - 'A' + 10;
        } else {
            break; // Invalid character
        }
        
        if (digit_val >= base) break; // Invalid digit for this base
        digit_count++;
        p++;
    }
    
    if (digit_count == 0) return NULL;
    
    // Estimate capacity: log2(base^n) = n * log2(base) 
    // Rough estimate: each decimal digit needs ~3.32 bits, so use digit_count limbs
    size_t capacity = (digit_count + 7) / 8 + 1; // Conservative estimate
    if (capacity < 1) capacity = 1;
    
    struct db_bigint_internal* result = db_alloc(capacity);
    if (!result) return NULL;
    
    // Convert digit by digit using Horner's method: result = result * base + digit
    db_bigint base_big = db_from_int32(base);
    if (!base_big) {
        db_release((db_bigint*)&result);
        return NULL;
    }
    
    p = str;
    for (size_t i = 0; i < digit_count; i++) {
        char c = *p++;
        int digit_val;
        if (c >= '0' && c <= '9') {
            digit_val = c - '0';
        } else if (c >= 'a' && c <= 'z') {
            digit_val = c - 'a' + 10;
        } else if (c >= 'A' && c <= 'Z') {
            digit_val = c - 'A' + 10;
        } else {
            break;
        }
        
        // result = result * base + digit
        db_bigint temp = db_multiply(result, base_big);
        if (!temp) {
            db_release(&base_big);
            db_release((db_bigint*)&result);
            return NULL;
        }
        
        db_bigint digit_big = db_from_int32(digit_val);
        if (!digit_big) {
            db_release(&temp);
            db_release(&base_big);
            db_release((db_bigint*)&result);
            return NULL;
        }
        
        db_bigint new_result = db_add(temp, digit_big);
        db_release(&temp);
        db_release(&digit_big);
        
        if (!new_result) {
            db_release(&base_big);
            db_release((db_bigint*)&result);
            return NULL;
        }
        
        db_release((db_bigint*)&result);
        result = new_result;
    }
    
    db_release(&base_big);
    
    // Set sign
    if (is_negative && result->limb_count > 0) {
        result->is_negative = true;
    }
    
    return result;
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

// BigInt division - returns quotient
db_bigint db_divide(db_bigint a, db_bigint b) {
    if (!a || !b) return NULL;
    if (db_is_zero(b)) return NULL; // Division by zero
    
    // Special cases
    if (db_is_zero(a)) return db_zero();
    if (db_equal(a, b)) return db_one();
    
    // Compare absolute values
    db_bigint abs_a = db_abs(a);
    db_bigint abs_b = db_abs(b);
    
    if (db_less(abs_a, abs_b)) {
        db_release(&abs_a);
        db_release(&abs_b);
        return db_zero();
    }
    
    // Simple division by repeated subtraction (inefficient but correct)
    // TODO: Implement proper long division algorithm for better performance
    db_bigint quotient = db_zero();
    db_bigint remainder = db_copy(abs_a);
    db_bigint one = db_one();
    
    while (db_greater_equal(remainder, abs_b)) {
        db_bigint temp_remainder = db_subtract(remainder, abs_b);
        db_bigint temp_quotient = db_add(quotient, one);
        
        db_release(&remainder);
        db_release(&quotient);
        remainder = temp_remainder;
        quotient = temp_quotient;
        
        if (!remainder || !quotient) {
            db_release(&abs_a);
            db_release(&abs_b);
            db_release(&one);
            db_release(&remainder);
            db_release(&quotient);
            return NULL;
        }
    }
    
    // Set result sign: negative if operands have different signs
    bool result_negative = (a->is_negative != b->is_negative);
    if (result_negative && quotient->limb_count > 0) {
        quotient->is_negative = true;
    }
    
    db_release(&abs_a);
    db_release(&abs_b);
    db_release(&remainder);
    db_release(&one);
    
    return quotient;
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
    
    // If already positive or zero, just return a copy
    if (!a->is_negative) {
        return db_copy(a);
    }
    
    // Create a positive copy of the negative number
    db_bigint result = db_copy(a);
    if (result && result->limb_count > 0) {
        result->is_negative = false;
    }
    return result;
}

// Bitwise operations
db_bigint db_and(db_bigint a, db_bigint b) {
    if (!a || !b) return NULL;
    
    size_t max_limbs = (a->limb_count > b->limb_count) ? a->limb_count : b->limb_count;
    struct db_bigint_internal* result = db_alloc(max_limbs);
    if (!result) return NULL;
    
    // AND operation on limbs
    for (size_t i = 0; i < max_limbs; i++) {
        db_limb_t a_limb = (i < a->limb_count) ? a->limbs[i] : 0;
        db_limb_t b_limb = (i < b->limb_count) ? b->limbs[i] : 0;
        result->limbs[i] = a_limb & b_limb;
    }
    result->limb_count = max_limbs;
    
    // Result is positive (bitwise operations on magnitudes)
    result->is_negative = false;
    db_normalize(result);
    
    return result;
}

db_bigint db_or(db_bigint a, db_bigint b) {
    if (!a || !b) return NULL;
    
    size_t max_limbs = (a->limb_count > b->limb_count) ? a->limb_count : b->limb_count;
    struct db_bigint_internal* result = db_alloc(max_limbs);
    if (!result) return NULL;
    
    // OR operation on limbs
    for (size_t i = 0; i < max_limbs; i++) {
        db_limb_t a_limb = (i < a->limb_count) ? a->limbs[i] : 0;
        db_limb_t b_limb = (i < b->limb_count) ? b->limbs[i] : 0;
        result->limbs[i] = a_limb | b_limb;
    }
    result->limb_count = max_limbs;
    
    // Result is positive (bitwise operations on magnitudes)
    result->is_negative = false;
    db_normalize(result);
    
    return result;
}

db_bigint db_xor(db_bigint a, db_bigint b) {
    if (!a || !b) return NULL;
    
    size_t max_limbs = (a->limb_count > b->limb_count) ? a->limb_count : b->limb_count;
    struct db_bigint_internal* result = db_alloc(max_limbs);
    if (!result) return NULL;
    
    // XOR operation on limbs
    for (size_t i = 0; i < max_limbs; i++) {
        db_limb_t a_limb = (i < a->limb_count) ? a->limbs[i] : 0;
        db_limb_t b_limb = (i < b->limb_count) ? b->limbs[i] : 0;
        result->limbs[i] = a_limb ^ b_limb;
    }
    result->limb_count = max_limbs;
    
    // Result is positive (bitwise operations on magnitudes)
    result->is_negative = false;
    db_normalize(result);
    
    return result;
}

db_bigint db_not(db_bigint a) {
    if (!a) return NULL;
    
    // For simplicity, NOT operation on fixed width (one limb beyond significant bits)
    size_t result_limbs = a->limb_count + 1;
    struct db_bigint_internal* result = db_alloc(result_limbs);
    if (!result) return NULL;
    
    // NOT operation on limbs
    for (size_t i = 0; i < a->limb_count; i++) {
        result->limbs[i] = ~a->limbs[i];
    }
    result->limbs[a->limb_count] = ~((db_limb_t)0); // Set high limb to all 1s
    result->limb_count = result_limbs;
    
    // Result is positive (bitwise operations on magnitudes)
    result->is_negative = false;
    db_normalize(result);
    
    return result;
}

db_bigint db_shift_left(db_bigint a, size_t bits) {
    if (!a || bits == 0) return db_copy(a);
    
    size_t limb_shift = bits / DB_LIMB_BITS;
    size_t bit_shift = bits % DB_LIMB_BITS;
    
    size_t new_limb_count = a->limb_count + limb_shift + (bit_shift > 0 ? 1 : 0);
    struct db_bigint_internal* result = db_alloc(new_limb_count);
    if (!result) return NULL;
    
    // Clear the result limbs
    for (size_t i = 0; i < new_limb_count; i++) {
        result->limbs[i] = 0;
    }
    
    // Copy and shift limbs
    if (bit_shift == 0) {
        // Simple limb shift
        for (size_t i = 0; i < a->limb_count; i++) {
            result->limbs[i + limb_shift] = a->limbs[i];
        }
    } else {
        // Bit shift within limbs
        db_limb_t carry = 0;
        for (size_t i = 0; i < a->limb_count; i++) {
            db_limb_t shifted = (a->limbs[i] << bit_shift) | carry;
            carry = a->limbs[i] >> (DB_LIMB_BITS - bit_shift);
            result->limbs[i + limb_shift] = shifted;
        }
        if (carry > 0) {
            result->limbs[a->limb_count + limb_shift] = carry;
        }
    }
    
    result->limb_count = new_limb_count;
    result->is_negative = a->is_negative;
    db_normalize(result);
    
    return result;
}

db_bigint db_shift_right(db_bigint a, size_t bits) {
    if (!a || bits == 0) return db_copy(a);
    
    size_t limb_shift = bits / DB_LIMB_BITS;
    size_t bit_shift = bits % DB_LIMB_BITS;
    
    // If shifting more limbs than we have, result is zero
    if (limb_shift >= a->limb_count) {
        return db_zero();
    }
    
    size_t new_limb_count = a->limb_count - limb_shift;
    struct db_bigint_internal* result = db_alloc(new_limb_count);
    if (!result) return NULL;
    
    if (bit_shift == 0) {
        // Simple limb shift
        for (size_t i = 0; i < new_limb_count; i++) {
            result->limbs[i] = a->limbs[i + limb_shift];
        }
    } else {
        // Bit shift within limbs
        for (size_t i = 0; i < new_limb_count; i++) {
            db_limb_t current = a->limbs[i + limb_shift];
            db_limb_t next = (i + limb_shift + 1 < a->limb_count) ? 
                            a->limbs[i + limb_shift + 1] : 0;
            result->limbs[i] = (current >> bit_shift) | 
                              (next << (DB_LIMB_BITS - bit_shift));
        }
    }
    
    result->limb_count = new_limb_count;
    result->is_negative = a->is_negative;
    db_normalize(result);
    
    return result;
}

// GCD using Euclidean algorithm
db_bigint db_gcd(db_bigint a, db_bigint b) {
    if (!a || !b) return NULL;
    
    db_bigint abs_a = db_abs(a);
    db_bigint abs_b = db_abs(b);
    
    if (db_is_zero(abs_a)) {
        db_release(&abs_a);
        return abs_b;
    }
    if (db_is_zero(abs_b)) {
        db_release(&abs_b);
        return abs_a;
    }
    
    // Euclidean algorithm: gcd(a,b) = gcd(b, a mod b)
    while (!db_is_zero(abs_b)) {
        db_bigint remainder = db_mod(abs_a, abs_b);
        if (!remainder) {
            db_release(&abs_a);
            db_release(&abs_b);
            return NULL;
        }
        
        db_release(&abs_a);
        abs_a = abs_b;
        abs_b = remainder;
    }
    
    db_release(&abs_b);
    return abs_a;
}

// LCM using the identity: lcm(a,b) = |a*b| / gcd(a,b)
db_bigint db_lcm(db_bigint a, db_bigint b) {
    if (!a || !b) return NULL;
    if (db_is_zero(a) || db_is_zero(b)) return db_zero();
    
    db_bigint gcd = db_gcd(a, b);
    if (!gcd) return NULL;
    
    db_bigint product = db_multiply(a, b);
    if (!product) {
        db_release(&gcd);
        return NULL;
    }
    
    db_bigint abs_product = db_abs(product);
    db_release(&product);
    if (!abs_product) {
        db_release(&gcd);
        return NULL;
    }
    
    db_bigint result = db_divide(abs_product, gcd);
    db_release(&abs_product);
    db_release(&gcd);
    
    return result;
}

// Simple integer square root using Newton's method
db_bigint db_sqrt(db_bigint n) {
    if (!n) return NULL;
    if (db_is_negative(n)) return NULL; // Square root of negative number
    if (db_is_zero(n)) return db_zero();
    
    db_bigint one = db_one();
    if (db_equal(n, one)) {
        db_release(&one);
        return db_one();
    }
    
    // Initial guess: x = n / 2
    db_bigint two = db_from_int32(2);
    db_bigint x = db_divide(n, two);
    if (!x) {
        db_release(&one);
        db_release(&two);
        return NULL;
    }
    
    // Newton's method: x_new = (x + n/x) / 2
    for (int iterations = 0; iterations < 100; iterations++) { // Limit iterations
        db_bigint quotient = db_divide(n, x);
        if (!quotient) break;
        
        db_bigint sum = db_add(x, quotient);
        db_release(&quotient);
        if (!sum) break;
        
        db_bigint x_new = db_divide(sum, two);
        db_release(&sum);
        if (!x_new) break;
        
        // Check for convergence
        if (db_greater_equal(x_new, x)) {
            db_release(&x_new);
            break;
        }
        
        db_release(&x);
        x = x_new;
    }
    
    db_release(&one);
    db_release(&two);
    return x;
}

// Factorial function
db_bigint db_factorial(uint32_t n) {
    if (n <= 1) return db_one();
    
    db_bigint result = db_one();
    if (!result) return NULL;
    
    for (uint32_t i = 2; i <= n; i++) {
        db_bigint i_big = db_from_uint32(i);
        if (!i_big) {
            db_release(&result);
            return NULL;
        }
        
        db_bigint new_result = db_multiply(result, i_big);
        db_release(&i_big);
        db_release(&result);
        
        if (!new_result) return NULL;
        result = new_result;
    }
    
    return result;
}

// Modular exponentiation: (base^exp) mod mod
// Uses binary exponentiation for efficiency
db_bigint db_mod_pow(db_bigint base, db_bigint exp, db_bigint mod) {
    if (!base || !exp || !mod) return NULL;
    if (db_is_zero(mod)) return NULL; // Division by zero
    if (db_equal(mod, db_one())) return db_zero(); // x mod 1 = 0
    
    if (db_is_zero(exp)) return db_one(); // base^0 = 1
    if (db_is_zero(base)) return db_zero(); // 0^exp = 0
    
    db_bigint result = db_one();
    db_bigint base_mod = db_mod(base, mod); // Reduce base first
    db_bigint exp_copy = db_copy(exp);
    
    if (!result || !base_mod || !exp_copy) {
        db_release(&result);
        db_release(&base_mod);
        db_release(&exp_copy);
        return NULL;
    }
    
    // Binary exponentiation
    while (!db_is_zero(exp_copy)) {
        // If exp is odd, multiply result by base_mod
        db_bigint remainder = db_mod(exp_copy, db_from_int32(2));
        if (remainder && !db_is_zero(remainder)) {
            db_bigint temp = db_multiply(result, base_mod);
            if (temp) {
                db_bigint new_result = db_mod(temp, mod);
                db_release(&temp);
                db_release(&result);
                result = new_result;
            }
        }
        db_release(&remainder);
        
        // Square base_mod and divide exp by 2
        db_bigint base_squared = db_multiply(base_mod, base_mod);
        if (base_squared) {
            db_bigint new_base = db_mod(base_squared, mod);
            db_release(&base_squared);
            db_release(&base_mod);
            base_mod = new_base;
        }
        
        db_bigint two = db_from_int32(2);
        db_bigint new_exp = db_divide(exp_copy, two);
        db_release(&two);
        db_release(&exp_copy);
        exp_copy = new_exp;
        
        if (!result || !base_mod || !exp_copy) break;
    }
    
    db_release(&base_mod);
    db_release(&exp_copy);
    
    return result;
}

// Simple primality test using trial division
bool db_is_prime(db_bigint n, int certainty) {
    (void)certainty; // Unused in this simple implementation
    
    if (!n) return false;
    if (db_is_negative(n)) return false;
    
    // Handle small cases
    db_bigint two = db_from_int32(2);
    db_bigint three = db_from_int32(3);
    
    if (db_less(n, two)) {
        db_release(&two);
        db_release(&three);
        return false;
    }
    if (db_equal(n, two) || db_equal(n, three)) {
        db_release(&two);
        db_release(&three);
        return true;
    }
    
    // Check if even
    db_bigint remainder = db_mod(n, two);
    if (db_is_zero(remainder)) {
        db_release(&remainder);
        db_release(&two);
        db_release(&three);
        return false;
    }
    db_release(&remainder);
    
    // Check odd divisors up to sqrt(n)
    db_bigint sqrt_n = db_sqrt(n);
    db_bigint i = db_copy(three);
    
    while (db_less_equal(i, sqrt_n)) {
        db_bigint remainder = db_mod(n, i);
        if (db_is_zero(remainder)) {
            db_release(&remainder);
            db_release(&two);
            db_release(&three);
            db_release(&sqrt_n);
            db_release(&i);
            return false; // Found a divisor
        }
        db_release(&remainder);
        
        // i += 2 (check only odd numbers)
        db_bigint new_i = db_add(i, two);
        db_release(&i);
        i = new_i;
        
        if (!i) break;
    }
    
    db_release(&two);
    db_release(&three);
    db_release(&sqrt_n);
    db_release(&i);
    
    return true;
}

// Find next prime number >= n
db_bigint db_next_prime(db_bigint n) {
    if (!n) return NULL;
    
    db_bigint candidate = db_copy(n);
    if (!candidate) return NULL;
    
    // If n is even, make it odd
    db_bigint two = db_from_int32(2);
    db_bigint remainder = db_mod(candidate, two);
    if (db_is_zero(remainder)) {
        db_bigint new_candidate = db_add(candidate, db_one());
        db_release(&candidate);
        candidate = new_candidate;
    }
    db_release(&remainder);
    
    // Check odd numbers until we find a prime
    while (candidate && !db_is_prime(candidate, 10)) {
        db_bigint new_candidate = db_add(candidate, two);
        db_release(&candidate);
        candidate = new_candidate;
    }
    
    db_release(&two);
    return candidate;
}

// Simple random number generator (NOT cryptographically secure)
// This is a placeholder implementation - real applications should use
// a proper CSPRNG like /dev/urandom or Windows CryptoAPI
db_bigint db_random(size_t bits) {
    if (bits == 0) return db_zero();
    
    size_t limbs_needed = (bits + DB_LIMB_BITS - 1) / DB_LIMB_BITS;
    struct db_bigint_internal* result = db_alloc(limbs_needed);
    if (!result) return NULL;
    
    // Use simple rand() - NOT suitable for cryptographic use
    for (size_t i = 0; i < limbs_needed; i++) {
        result->limbs[i] = 0;
        for (int j = 0; j < (int)(sizeof(db_limb_t)); j++) {
            result->limbs[i] |= ((db_limb_t)(rand() & 0xFF)) << (j * 8);
        }
    }
    
    // Mask the high bits to get exactly 'bits' bits
    size_t high_bits = bits % DB_LIMB_BITS;
    if (high_bits > 0) {
        db_limb_t mask = (1UL << high_bits) - 1;
        result->limbs[limbs_needed - 1] &= mask;
    }
    
    result->limb_count = limbs_needed;
    result->is_negative = false;
    db_normalize(result);
    
    return result;
}

// Random number in range [min, max)
db_bigint db_random_range(db_bigint min, db_bigint max) {
    if (!min || !max) return NULL;
    if (db_greater_equal(min, max)) return NULL;
    
    db_bigint range = db_subtract(max, min);
    if (!range) return NULL;
    
    // Get bit length of range to generate appropriate random number
    size_t range_bits = db_bit_length(range);
    
    // Generate random numbers until we get one in range
    // (Rejection sampling to avoid bias)
    for (int attempts = 0; attempts < 100; attempts++) {
        db_bigint random = db_random(range_bits + 8); // Extra bits to reduce rejection
        if (!random) continue;
        
        db_bigint mod_result = db_mod(random, range);
        db_release(&random);
        
        if (mod_result) {
            db_bigint result = db_add(min, mod_result);
            db_release(&mod_result);
            db_release(&range);
            return result;
        }
    }
    
    db_release(&range);
    return NULL; // Failed to generate
}

// Calculate bit length of a big integer
size_t db_bit_length(db_bigint big) {
    if (!big || big->limb_count == 0) return 0;
    
    // Find the most significant limb
    size_t high_limb_idx = big->limb_count - 1;
    db_limb_t high_limb = big->limbs[high_limb_idx];
    
    // Count bits in the high limb
    size_t high_limb_bits = 0;
    if (high_limb != 0) {
        db_limb_t temp = high_limb;
        while (temp > 0) {
            high_limb_bits++;
            temp >>= 1;
        }
    }
    
    return high_limb_idx * DB_LIMB_BITS + high_limb_bits;
}

// Count of limbs used
size_t db_limb_count(db_bigint big) {
    return big ? big->limb_count : 0;
}

// Extended Euclidean Algorithm: finds gcd(a,b) and coefficients x,y such that ax + by = gcd(a,b)
db_bigint db_extended_gcd(db_bigint a, db_bigint b, db_bigint* x, db_bigint* y) {
    if (!a || !b || !x || !y) return NULL;
    
    // Initialize
    db_bigint old_r = db_abs(a);
    db_bigint r = db_abs(b);
    db_bigint old_s = db_one();
    db_bigint s = db_zero();
    db_bigint old_t = db_zero();
    db_bigint t = db_one();
    
    if (!old_r || !r || !old_s || !s || !old_t || !t) {
        db_release(&old_r); db_release(&r);
        db_release(&old_s); db_release(&s);
        db_release(&old_t); db_release(&t);
        return NULL;
    }
    
    while (!db_is_zero(r)) {
        db_bigint quotient = db_divide(old_r, r);
        if (!quotient) break;
        
        // (old_r, r) := (r, old_r - quotient * r)
        db_bigint temp1 = db_multiply(quotient, r);
        db_bigint new_r = db_subtract(old_r, temp1);
        db_release(&temp1);
        db_release(&old_r);
        old_r = r;
        r = new_r;
        
        // (old_s, s) := (s, old_s - quotient * s)
        db_bigint temp2 = db_multiply(quotient, s);
        db_bigint new_s = db_subtract(old_s, temp2);
        db_release(&temp2);
        db_release(&old_s);
        old_s = s;
        s = new_s;
        
        // (old_t, t) := (t, old_t - quotient * t)
        db_bigint temp3 = db_multiply(quotient, t);
        db_bigint new_t = db_subtract(old_t, temp3);
        db_release(&temp3);
        db_release(&quotient);
        db_release(&old_t);
        old_t = t;
        t = new_t;
        
        if (!r || !s || !t) break;
    }
    
    // Set output coefficients
    *x = old_s;
    *y = old_t;
    
    db_release(&r);
    db_release(&s);
    db_release(&t);
    
    return old_r; // This is gcd(a,b)
}

#endif // DB_IMPLEMENTATION

#endif // DYNAMIC_BIGINT_H