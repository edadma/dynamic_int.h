#include <stdlib.h>
#define DI_IMPLEMENTATION
#include "dynamic_int.h"
#include "unity.h"

void setUp(void) {
    // Set up code before each test
}

void tearDown(void) {
    // Clean up code after each test
}

// Basic creation tests
void test_create_from_int32(void) {
    di_int a = di_from_int32(42);
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_FALSE(di_is_zero(a));
    TEST_ASSERT_TRUE(di_is_positive(a));
    TEST_ASSERT_FALSE(di_is_negative(a));
    di_release(&a);
    TEST_ASSERT_NULL(a);
}

void test_create_zero(void) {
    di_int zero = di_zero();
    TEST_ASSERT_NOT_NULL(zero);
    TEST_ASSERT_TRUE(di_is_zero(zero));
    TEST_ASSERT_FALSE(di_is_positive(zero));
    TEST_ASSERT_FALSE(di_is_negative(zero));
    di_release(&zero);
}

void test_create_one(void) {
    di_int one = di_one();
    TEST_ASSERT_NOT_NULL(one);
    TEST_ASSERT_FALSE(di_is_zero(one));
    TEST_ASSERT_TRUE(di_is_positive(one));
    TEST_ASSERT_FALSE(di_is_negative(one));
    di_release(&one);
}

void test_create_negative(void) {
    di_int neg = di_from_int32(-42);
    TEST_ASSERT_NOT_NULL(neg);
    TEST_ASSERT_FALSE(di_is_zero(neg));
    TEST_ASSERT_FALSE(di_is_positive(neg));
    TEST_ASSERT_TRUE(di_is_negative(neg));
    di_release(&neg);
}

// Conversion tests
void test_to_int32_basic(void) {
    di_int a = di_from_int32(123);
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(a, &result));
    TEST_ASSERT_EQUAL_INT32(123, result);
    di_release(&a);
}

void test_to_int32_negative(void) {
    di_int a = di_from_int32(-456);
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(a, &result));
    TEST_ASSERT_EQUAL_INT32(-456, result);
    di_release(&a);
}

void test_to_int32_zero(void) {
    di_int zero = di_zero();
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(zero, &result));
    TEST_ASSERT_EQUAL_INT32(0, result);
    di_release(&zero);
}

// Reference counting tests
void test_reference_counting(void) {
    di_int a = di_from_int32(42);
    TEST_ASSERT_EQUAL_UINT(1, di_ref_count(a));
    
    di_int b = di_retain(a);
    TEST_ASSERT_EQUAL_PTR(a, b);
    TEST_ASSERT_EQUAL_UINT(2, di_ref_count(a));
    TEST_ASSERT_EQUAL_UINT(2, di_ref_count(b));
    
    di_release(&a);
    TEST_ASSERT_NULL(a);
    TEST_ASSERT_EQUAL_UINT(1, di_ref_count(b));
    
    di_release(&b);
    TEST_ASSERT_NULL(b);
}

void test_copy(void) {
    di_int a = di_from_int32(789);
    di_int copy = di_copy(a);
    
    TEST_ASSERT_NOT_NULL(copy);
    TEST_ASSERT_TRUE(a != copy);
    TEST_ASSERT_EQUAL_UINT(1, di_ref_count(a));
    TEST_ASSERT_EQUAL_UINT(1, di_ref_count(copy));
    
    // Test they have same value
    TEST_ASSERT_TRUE(di_eq(a, copy));
    
    di_release(&a);
    di_release(&copy);
}

// Comparison tests
void test_equal(void) {
    di_int a = di_from_int32(123);
    di_int b = di_from_int32(123);
    di_int c = di_from_int32(456);
    
    TEST_ASSERT_TRUE(di_eq(a, b));
    TEST_ASSERT_FALSE(di_eq(a, c));
    TEST_ASSERT_EQUAL_INT(0, di_compare(a, b));
    
    di_release(&a);
    di_release(&b);
    di_release(&c);
}

void test_less_greater(void) {
    di_int small = di_from_int32(10);
    di_int big = di_from_int32(20);
    
    TEST_ASSERT_TRUE(di_lt(small, big));
    TEST_ASSERT_FALSE(di_lt(big, small));
    TEST_ASSERT_TRUE(di_gt(big, small));
    TEST_ASSERT_FALSE(di_gt(small, big));
    
    TEST_ASSERT_TRUE(di_le(small, big));
    TEST_ASSERT_TRUE(di_ge(big, small));
    
    di_release(&small);
    di_release(&big);
}

void test_negative_comparison(void) {
    di_int pos = di_from_int32(10);
    di_int neg = di_from_int32(-10);
    
    TEST_ASSERT_TRUE(di_lt(neg, pos));
    TEST_ASSERT_TRUE(di_gt(pos, neg));
    TEST_ASSERT_FALSE(di_eq(pos, neg));
    
    di_release(&pos);
    di_release(&neg);
}

// Basic arithmetic tests
void test_addition_basic(void) {
    di_int a = di_from_int32(10);
    di_int b = di_from_int32(20);
    di_int sum = di_add(a, b);
    
    TEST_ASSERT_NOT_NULL(sum);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(sum, &result));
    TEST_ASSERT_EQUAL_INT32(30, result);
    
    di_release(&a);
    di_release(&b);
    di_release(&sum);
}

void test_addition_int32(void) {
    di_int a = di_from_int32(10);
    di_int sum = di_add_i32(a, 15);
    
    TEST_ASSERT_NOT_NULL(sum);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(sum, &result));
    TEST_ASSERT_EQUAL_INT32(25, result);
    
    di_release(&a);
    di_release(&sum);
}

void test_addition_with_zero(void) {
    di_int a = di_from_int32(42);
    di_int zero = di_zero();
    di_int sum = di_add(a, zero);
    
    TEST_ASSERT_NOT_NULL(sum);
    TEST_ASSERT_TRUE(di_eq(a, sum));
    
    di_release(&a);
    di_release(&zero);
    di_release(&sum);
}

void test_subtraction_basic(void) {
    di_int a = di_from_int32(30);
    di_int b = di_from_int32(10);
    di_int diff = di_sub(a, b);
    
    TEST_ASSERT_NOT_NULL(diff);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(diff, &result));
    TEST_ASSERT_EQUAL_INT32(20, result);
    
    di_release(&a);
    di_release(&b);
    di_release(&diff);
}

void test_negation(void) {
    di_int pos = di_from_int32(42);
    di_int neg = di_negate(pos);
    
    TEST_ASSERT_NOT_NULL(neg);
    TEST_ASSERT_TRUE(di_is_negative(neg));
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(neg, &result));
    TEST_ASSERT_EQUAL_INT32(-42, result);
    
    // Test double negation
    di_int pos_again = di_negate(neg);
    TEST_ASSERT_TRUE(di_eq(pos, pos_again));
    
    di_release(&pos);
    di_release(&neg);
    di_release(&pos_again);
}

// Multiplication tests
void test_multiplication_basic(void) {
    di_int a = di_from_int32(6);
    di_int b = di_from_int32(7);
    di_int product = di_mul(a, b);
    
    TEST_ASSERT_NOT_NULL(product);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(product, &result));
    TEST_ASSERT_EQUAL_INT32(42, result);
    
    di_release(&a);
    di_release(&b);
    di_release(&product);
}

void test_multiplication_int32(void) {
    di_int a = di_from_int32(8);
    di_int product = di_mul_i32(a, 5);
    
    TEST_ASSERT_NOT_NULL(product);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(product, &result));
    TEST_ASSERT_EQUAL_INT32(40, result);
    
    di_release(&a);
    di_release(&product);
}

void test_multiplication_with_zero(void) {
    di_int a = di_from_int32(42);
    di_int zero = di_zero();
    di_int product = di_mul(a, zero);
    
    TEST_ASSERT_NOT_NULL(product);
    TEST_ASSERT_TRUE(di_is_zero(product));
    
    di_release(&a);
    di_release(&zero);
    di_release(&product);
}

void test_multiplication_negative(void) {
    di_int a = di_from_int32(-3);
    di_int b = di_from_int32(4);
    di_int product = di_mul(a, b);
    
    TEST_ASSERT_NOT_NULL(product);
    TEST_ASSERT_TRUE(di_is_negative(product));
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(product, &result));
    TEST_ASSERT_EQUAL_INT32(-12, result);
    
    di_release(&a);
    di_release(&b);
    di_release(&product);
}

// Modulo tests
void test_modulo_basic(void) {
    di_int a = di_from_int32(17);
    di_int b = di_from_int32(5);
    di_int remainder = di_mod(a, b);
    
    TEST_ASSERT_NOT_NULL(remainder);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(remainder, &result));
    TEST_ASSERT_EQUAL_INT32(2, result);
    
    di_release(&a);
    di_release(&b);
    di_release(&remainder);
}

void test_modulo_exact_division(void) {
    di_int a = di_from_int32(15);
    di_int b = di_from_int32(3);
    di_int remainder = di_mod(a, b);
    
    TEST_ASSERT_NOT_NULL(remainder);
    TEST_ASSERT_TRUE(di_is_zero(remainder));
    
    di_release(&a);
    di_release(&b);
    di_release(&remainder);
}

// Absolute value tests
void test_abs_positive(void) {
    di_int pos = di_from_int32(42);
    di_int abs_val = di_abs(pos);
    
    TEST_ASSERT_NOT_NULL(abs_val);
    TEST_ASSERT_TRUE(di_eq(pos, abs_val));
    
    di_release(&pos);
    di_release(&abs_val);
}

void test_abs_negative(void) {
    di_int neg = di_from_int32(-42);
    di_int abs_val = di_abs(neg);
    
    TEST_ASSERT_NOT_NULL(abs_val);
    TEST_ASSERT_FALSE(di_is_negative(abs_val));
    TEST_ASSERT_TRUE(di_is_positive(abs_val));
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(abs_val, &result));
    TEST_ASSERT_EQUAL_INT32(42, result);
    
    di_release(&neg);
    di_release(&abs_val);
}

void test_abs_zero(void) {
    di_int zero = di_zero();
    di_int abs_val = di_abs(zero);
    
    TEST_ASSERT_NOT_NULL(abs_val);
    TEST_ASSERT_TRUE(di_is_zero(abs_val));
    
    di_release(&zero);
    di_release(&abs_val);
}

// Double conversion tests (using basic float comparison since Unity has double disabled)
void test_to_double_basic(void) {
    di_int a = di_from_int32(12345);
    double result = di_to_double(a);
    
    TEST_ASSERT_FLOAT_WITHIN(0.1, 12345.0f, (float)result);
    
    di_release(&a);
}

void test_to_double_negative(void) {
    di_int a = di_from_int32(-67890);
    double result = di_to_double(a);
    
    TEST_ASSERT_FLOAT_WITHIN(0.1, -67890.0f, (float)result);
    
    di_release(&a);
}

void test_to_double_zero(void) {
    di_int zero = di_zero();
    double result = di_to_double(zero);
    
    TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0f, (float)result);
    
    di_release(&zero);
}

// Subtraction with int32 tests
void test_subtract_int32_basic(void) {
    di_int a = di_from_int32(50);
    di_int diff = di_sub_i32(a, 20);
    
    TEST_ASSERT_NOT_NULL(diff);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(diff, &result));
    TEST_ASSERT_EQUAL_INT32(30, result);
    
    di_release(&a);
    di_release(&diff);
}

void test_subtract_int32_negative_result(void) {
    di_int a = di_from_int32(10);
    di_int diff = di_sub_i32(a, 25);
    
    TEST_ASSERT_NOT_NULL(diff);
    TEST_ASSERT_TRUE(di_is_negative(diff));
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(diff, &result));
    TEST_ASSERT_EQUAL_INT32(-15, result);
    
    di_release(&a);
    di_release(&diff);
}

// Unsigned integer creation tests
void test_from_uint32(void) {
    di_int a = di_from_uint32(4294967295U); // UINT32_MAX
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_FALSE(di_is_negative(a));
    TEST_ASSERT_TRUE(di_is_positive(a));
    
    // Convert to int64 since it won't fit in int32
    int64_t result;
    TEST_ASSERT_TRUE(di_to_int64(a, &result));
    TEST_ASSERT_EQUAL_INT64(4294967295LL, result);
    
    di_release(&a);
}

void test_from_uint64(void) {
    di_int a = di_from_uint64(9876543210123ULL);
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_FALSE(di_is_negative(a));
    TEST_ASSERT_TRUE(di_is_positive(a));
    
    int64_t result;
    TEST_ASSERT_TRUE(di_to_int64(a, &result));
    TEST_ASSERT_EQUAL_INT64(9876543210123LL, result);
    
    di_release(&a);
}

// Additional overflow detection tests
void test_overflow_detection_int32_multiply(void) {
    int32_t result;
    
    // Test normal case
    TEST_ASSERT_TRUE(di_multiply_overflow_int32(100, 200, &result));
    TEST_ASSERT_EQUAL_INT32(20000, result);
    
    // Test overflow case
    TEST_ASSERT_FALSE(di_multiply_overflow_int32(INT32_MAX, 2, &result));
    
    // Test zero multiplication
    TEST_ASSERT_TRUE(di_multiply_overflow_int32(INT32_MAX, 0, &result));
    TEST_ASSERT_EQUAL_INT32(0, result);
}

void test_overflow_detection_int64_operations(void) {
    int64_t result;
    
    // Test int64 addition
    TEST_ASSERT_TRUE(di_add_overflow_int64(1000000000000LL, 2000000000000LL, &result));
    TEST_ASSERT_EQUAL_INT64(3000000000000LL, result);
    
    // Test int64 addition overflow
    TEST_ASSERT_FALSE(di_add_overflow_int64(INT64_MAX, 1, &result));
    
    // Test int64 subtraction
    TEST_ASSERT_TRUE(di_subtract_overflow_int64(5000000000000LL, 2000000000000LL, &result));
    TEST_ASSERT_EQUAL_INT64(3000000000000LL, result);
    
    // Test int64 subtraction underflow
    TEST_ASSERT_FALSE(di_subtract_overflow_int64(INT64_MIN, 1, &result));
    
    // Test int64 multiplication
    TEST_ASSERT_TRUE(di_multiply_overflow_int64(1000000LL, 2000000LL, &result));
    TEST_ASSERT_EQUAL_INT64(2000000000000LL, result);
    
    // Test int64 multiplication overflow
    TEST_ASSERT_FALSE(di_multiply_overflow_int64(INT64_MAX, 2, &result));
}

// Overflow detection tests
void test_overflow_detection_int32(void) {
    int32_t result;
    
    // Test normal case
    TEST_ASSERT_TRUE(di_add_overflow_int32(100, 200, &result));
    TEST_ASSERT_EQUAL_INT32(300, result);
    
    // Test overflow case
    TEST_ASSERT_FALSE(di_add_overflow_int32(INT32_MAX, 1, &result));
    
    // Test underflow case
    TEST_ASSERT_FALSE(di_subtract_overflow_int32(INT32_MIN, 1, &result));
}

// Large number tests (using int64)
void test_int64_conversion(void) {
    di_int a = di_from_int64(1234567890123LL);
    TEST_ASSERT_NOT_NULL(a);
    
    int64_t result;
    TEST_ASSERT_TRUE(di_to_int64(a, &result));
    TEST_ASSERT_EQUAL_INT64(1234567890123LL, result);
    
    di_release(&a);
}

void test_int64_negative(void) {
    di_int a = di_from_int64(-9876543210987LL);
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_TRUE(di_is_negative(a));
    
    int64_t result;
    TEST_ASSERT_TRUE(di_to_int64(a, &result));
    TEST_ASSERT_EQUAL_INT64(-9876543210987LL, result);
    
    di_release(&a);
}

// String conversion tests (basic)
void test_to_string_basic(void) {
    di_int a = di_from_int32(12345);
    char* str = di_to_string(a, 10);
    
    TEST_ASSERT_NOT_NULL(str);
    TEST_ASSERT_EQUAL_STRING("12345", str);
    
    free(str);
    di_release(&a);
}

void test_to_string_zero(void) {
    di_int zero = di_zero();
    char* str = di_to_string(zero, 10);
    
    TEST_ASSERT_NOT_NULL(str);
    TEST_ASSERT_EQUAL_STRING("0", str);
    
    free(str);
    di_release(&zero);
}

// String parsing tests
void test_from_string_decimal(void) {
    di_int a = di_from_string("12345", 10);
    TEST_ASSERT_NOT_NULL(a);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(a, &result));
    TEST_ASSERT_EQUAL_INT32(12345, result);
    
    di_release(&a);
}

void test_from_string_negative(void) {
    di_int a = di_from_string("-6789", 10);
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_TRUE(di_is_negative(a));
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(a, &result));
    TEST_ASSERT_EQUAL_INT32(-6789, result);
    
    di_release(&a);
}

void test_from_string_hex(void) {
    di_int a = di_from_string("FF", 16);
    TEST_ASSERT_NOT_NULL(a);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(a, &result));
    TEST_ASSERT_EQUAL_INT32(255, result);
    
    di_release(&a);
}

void test_from_string_binary(void) {
    di_int a = di_from_string("1010", 2);
    TEST_ASSERT_NOT_NULL(a);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(a, &result));
    TEST_ASSERT_EQUAL_INT32(10, result);
    
    di_release(&a);
}

void test_from_string_zero(void) {
    di_int a = di_from_string("0", 10);
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_TRUE(di_is_zero(a));
    
    di_release(&a);
}

// Division tests
void test_divide_basic(void) {
    di_int a = di_from_int32(42);
    di_int b = di_from_int32(6);
    di_int quotient = di_divide(a, b);
    
    TEST_ASSERT_NOT_NULL(quotient);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(quotient, &result));
    TEST_ASSERT_EQUAL_INT32(7, result);
    
    di_release(&a);
    di_release(&b);
    di_release(&quotient);
}

void test_divide_with_remainder(void) {
    di_int a = di_from_int32(17);
    di_int b = di_from_int32(5);
    di_int quotient = di_divide(a, b);
    
    TEST_ASSERT_NOT_NULL(quotient);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(quotient, &result));
    TEST_ASSERT_EQUAL_INT32(3, result);
    
    di_release(&a);
    di_release(&b);
    di_release(&quotient);
}

void test_divide_negative(void) {
    di_int a = di_from_int32(-20);
    di_int b = di_from_int32(4);
    di_int quotient = di_divide(a, b);
    
    TEST_ASSERT_NOT_NULL(quotient);
    TEST_ASSERT_TRUE(di_is_negative(quotient));
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(quotient, &result));
    TEST_ASSERT_EQUAL_INT32(-5, result);
    
    di_release(&a);
    di_release(&b);
    di_release(&quotient);
}

// Modular exponentiation tests
void test_mod_pow_basic(void) {
    di_int base = di_from_int32(2);
    di_int exp = di_from_int32(8);
    di_int mod = di_from_int32(100);
    di_int result = di_mod_pow(base, exp, mod);
    
    TEST_ASSERT_NOT_NULL(result);
    
    int32_t val;
    TEST_ASSERT_TRUE(di_to_int32(result, &val));
    TEST_ASSERT_EQUAL_INT32(56, val); // 2^8 % 100 = 256 % 100 = 56
    
    di_release(&base);
    di_release(&exp);
    di_release(&mod);
    di_release(&result);
}

void test_mod_pow_zero_exp(void) {
    di_int base = di_from_int32(5);
    di_int exp = di_zero();
    di_int mod = di_from_int32(7);
    di_int result = di_mod_pow(base, exp, mod);
    
    TEST_ASSERT_NOT_NULL(result);
    
    int32_t val;
    TEST_ASSERT_TRUE(di_to_int32(result, &val));
    TEST_ASSERT_EQUAL_INT32(1, val); // Any number to power 0 is 1
    
    di_release(&base);
    di_release(&exp);
    di_release(&mod);
    di_release(&result);
}

// Prime testing tests
void test_is_prime_small_primes(void) {
    di_int two = di_from_int32(2);
    di_int three = di_from_int32(3);
    di_int five = di_from_int32(5);
    di_int seven = di_from_int32(7);
    
    TEST_ASSERT_TRUE(di_is_prime(two, 10));
    TEST_ASSERT_TRUE(di_is_prime(three, 10));
    TEST_ASSERT_TRUE(di_is_prime(five, 10));
    TEST_ASSERT_TRUE(di_is_prime(seven, 10));
    
    di_release(&two);
    di_release(&three);
    di_release(&five);
    di_release(&seven);
}

void test_is_prime_composites(void) {
    di_int four = di_from_int32(4);
    di_int six = di_from_int32(6);
    di_int eight = di_from_int32(8);
    di_int nine = di_from_int32(9);
    
    TEST_ASSERT_FALSE(di_is_prime(four, 10));
    TEST_ASSERT_FALSE(di_is_prime(six, 10));
    TEST_ASSERT_FALSE(di_is_prime(eight, 10));
    TEST_ASSERT_FALSE(di_is_prime(nine, 10));
    
    di_release(&four);
    di_release(&six);
    di_release(&eight);
    di_release(&nine);
}

void test_next_prime(void) {
    di_int ten = di_from_int32(10);
    di_int next = di_next_prime(ten);
    
    TEST_ASSERT_NOT_NULL(next);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(next, &result));
    TEST_ASSERT_EQUAL_INT32(11, result);
    
    di_release(&ten);
    di_release(&next);
}

// GCD/LCM tests
void test_gcd_basic(void) {
    di_int a = di_from_int32(48);
    di_int b = di_from_int32(18);
    di_int gcd = di_gcd(a, b);
    
    TEST_ASSERT_NOT_NULL(gcd);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(gcd, &result));
    TEST_ASSERT_EQUAL_INT32(6, result);
    
    di_release(&a);
    di_release(&b);
    di_release(&gcd);
}

void test_lcm_basic(void) {
    di_int a = di_from_int32(12);
    di_int b = di_from_int32(18);
    di_int lcm = di_lcm(a, b);
    
    TEST_ASSERT_NOT_NULL(lcm);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(lcm, &result));
    TEST_ASSERT_EQUAL_INT32(36, result);
    
    di_release(&a);
    di_release(&b);
    di_release(&lcm);
}

void test_extended_gcd(void) {
    di_int a = di_from_int32(35);
    di_int b = di_from_int32(15);
    di_int x = NULL, y = NULL;
    di_int gcd = di_extended_gcd(a, b, &x, &y);
    
    TEST_ASSERT_NOT_NULL(gcd);
    TEST_ASSERT_NOT_NULL(x);
    TEST_ASSERT_NOT_NULL(y);
    
    int32_t gcd_val;
    TEST_ASSERT_TRUE(di_to_int32(gcd, &gcd_val));
    TEST_ASSERT_EQUAL_INT32(5, gcd_val);
    
    di_release(&a);
    di_release(&b);
    di_release(&gcd);
    di_release(&x);
    di_release(&y);
}

// Square root tests
void test_sqrt_perfect_square(void) {
    di_int num = di_from_int32(144);
    di_int root = di_sqrt(num);
    
    TEST_ASSERT_NOT_NULL(root);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(root, &result));
    TEST_ASSERT_EQUAL_INT32(12, result);
    
    di_release(&num);
    di_release(&root);
}

void test_sqrt_non_perfect_square(void) {
    di_int num = di_from_int32(10);
    di_int root = di_sqrt(num);
    
    TEST_ASSERT_NOT_NULL(root);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(root, &result));
    TEST_ASSERT_EQUAL_INT32(3, result); // floor(sqrt(10)) = 3
    
    di_release(&num);
    di_release(&root);
}

// Factorial tests
void test_factorial_small(void) {
    di_int fact5 = di_factorial(5);
    TEST_ASSERT_NOT_NULL(fact5);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(fact5, &result));
    TEST_ASSERT_EQUAL_INT32(120, result);
    
    di_release(&fact5);
}

void test_factorial_zero(void) {
    di_int fact0 = di_factorial(0);
    TEST_ASSERT_NOT_NULL(fact0);
    
    int32_t result;
    TEST_ASSERT_TRUE(di_to_int32(fact0, &result));
    TEST_ASSERT_EQUAL_INT32(1, result);
    
    di_release(&fact0);
}

// Bitwise operation tests
void test_bitwise_and(void) {
    di_int a = di_from_int32(12); // 1100 in binary
    di_int b = di_from_int32(10); // 1010 in binary
    di_int result = di_and(a, b);
    
    TEST_ASSERT_NOT_NULL(result);
    
    int32_t val;
    TEST_ASSERT_TRUE(di_to_int32(result, &val));
    TEST_ASSERT_EQUAL_INT32(8, val); // 1000 in binary
    
    di_release(&a);
    di_release(&b);
    di_release(&result);
}

void test_bitwise_or(void) {
    di_int a = di_from_int32(12); // 1100 in binary
    di_int b = di_from_int32(10); // 1010 in binary
    di_int result = di_or(a, b);
    
    TEST_ASSERT_NOT_NULL(result);
    
    int32_t val;
    TEST_ASSERT_TRUE(di_to_int32(result, &val));
    TEST_ASSERT_EQUAL_INT32(14, val); // 1110 in binary
    
    di_release(&a);
    di_release(&b);
    di_release(&result);
}

void test_bitwise_xor(void) {
    di_int a = di_from_int32(12); // 1100 in binary
    di_int b = di_from_int32(10); // 1010 in binary
    di_int result = di_xor(a, b);
    
    TEST_ASSERT_NOT_NULL(result);
    
    int32_t val;
    TEST_ASSERT_TRUE(di_to_int32(result, &val));
    TEST_ASSERT_EQUAL_INT32(6, val); // 0110 in binary
    
    di_release(&a);
    di_release(&b);
    di_release(&result);
}

void test_bitwise_not(void) {
    di_int a = di_from_int32(5); // Binary: 101
    di_int result = di_not(a);
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_FALSE(di_is_negative(result)); // Bitwise NOT returns positive result
    
    // The exact value will be large since it's ~5 with extended precision
    // Just verify it's a valid positive result
    TEST_ASSERT_TRUE(di_is_positive(result));
    
    di_release(&a);
    di_release(&result);
}

void test_shift_left(void) {
    di_int a = di_from_int32(5); // Binary: 101
    di_int result = di_shift_left(a, 2);
    
    TEST_ASSERT_NOT_NULL(result);
    
    int32_t val;
    TEST_ASSERT_TRUE(di_to_int32(result, &val));
    TEST_ASSERT_EQUAL_INT32(20, val); // 5 << 2 = 20 (Binary: 10100)
    
    di_release(&a);
    di_release(&result);
}

void test_shift_right(void) {
    di_int a = di_from_int32(20); // Binary: 10100
    di_int result = di_shift_right(a, 2);
    
    TEST_ASSERT_NOT_NULL(result);
    
    int32_t val;
    TEST_ASSERT_TRUE(di_to_int32(result, &val));
    TEST_ASSERT_EQUAL_INT32(5, val); // 20 >> 2 = 5 (Binary: 101)
    
    di_release(&a);
    di_release(&result);
}

// Random number tests (basic - just verify they don't crash)
void test_random_basic(void) {
    di_int random_bits = di_random(32);
    TEST_ASSERT_NOT_NULL(random_bits);
    
    di_release(&random_bits);
}

void test_random_range(void) {
    di_int min_val = di_from_int32(10);
    di_int max_val = di_from_int32(100);
    di_int random_val = di_random_range(min_val, max_val);
    
    TEST_ASSERT_NOT_NULL(random_val);
    TEST_ASSERT_TRUE(di_ge(random_val, min_val));
    TEST_ASSERT_TRUE(di_lt(random_val, max_val));
    
    di_release(&min_val);
    di_release(&max_val);
    di_release(&random_val);
}

// Utility function tests
void test_bit_length(void) {
    di_int a = di_from_int32(7); // Binary: 111
    size_t length = di_bit_length(a);
    
    TEST_ASSERT_EQUAL_size_t(3, length);
    
    di_release(&a);
}

// Large number arithmetic tests
void test_large_multiplication(void) {
    // 999999999999999999 * 888888888888888888 = 888888888888888887111111111111111112
    di_int a = di_from_string("999999999999999999", 10);
    di_int b = di_from_string("888888888888888888", 10);
    di_int product = di_mul(a, b);
    
    TEST_ASSERT_NOT_NULL(product);
    
    char* result_str = di_to_string(product, 10);
    TEST_ASSERT_NOT_NULL(result_str);
    TEST_ASSERT_EQUAL_STRING("888888888888888887111111111111111112", result_str);
    
    free(result_str);
    di_release(&a);
    di_release(&b);
    di_release(&product);
    
    // 123456789012345 * 987654321098765 = 121932631137021071359549253925
    di_int a2 = di_from_string("123456789012345", 10);
    di_int b2 = di_from_string("987654321098765", 10);
    di_int product2 = di_mul(a2, b2);
    
    result_str = di_to_string(product2, 10);
    TEST_ASSERT_EQUAL_STRING("121932631137021071359549253925", result_str);
    
    free(result_str);
    di_release(&a2);
    di_release(&b2);
    di_release(&product2);
}

void test_large_division(void) {
    // 999999999999999999888888888888888888 / 999999999999999999 = 1000000000000000000
    di_int dividend = di_from_string("999999999999999999888888888888888888", 10);
    di_int divisor = di_from_string("999999999999999999", 10);
    di_int quotient = di_divide(dividend, divisor);
    
    TEST_ASSERT_NOT_NULL(quotient);
    
    char* result_str = di_to_string(quotient, 10);
    TEST_ASSERT_EQUAL_STRING("1000000000000000000", result_str);
    
    free(result_str);
    di_release(&dividend);
    di_release(&divisor);
    di_release(&quotient);
    
    // Large number divided by small number: 999999999999999999999999999999 / 7 = 142857142857142857142857142857
    di_int big_num = di_from_string("999999999999999999999999999999", 10);
    di_int small_num = di_from_int32(7);
    di_int big_quotient = di_divide(big_num, small_num);
    
    result_str = di_to_string(big_quotient, 10);
    TEST_ASSERT_EQUAL_STRING("142857142857142857142857142857", result_str);
    
    free(result_str);
    di_release(&big_num);
    di_release(&small_num);
    di_release(&big_quotient);
}

void test_large_modulo(void) {
    // 999999999999999999999999999 % 123456789 = 93951369
    di_int large_num = di_from_string("999999999999999999999999999", 10);
    di_int modulus = di_from_int32(123456789);
    di_int remainder = di_mod(large_num, modulus);
    
    TEST_ASSERT_NOT_NULL(remainder);
    
    char* result_str = di_to_string(remainder, 10);
    TEST_ASSERT_EQUAL_STRING("93951369", result_str);
    
    free(result_str);
    di_release(&large_num);
    di_release(&modulus);
    di_release(&remainder);
    
    // 888888888888888888888888888888 % 77777777 = 888888
    di_int large_num2 = di_from_string("888888888888888888888888888888", 10);
    di_int modulus2 = di_from_int32(77777777);
    di_int remainder2 = di_mod(large_num2, modulus2);
    
    result_str = di_to_string(remainder2, 10);
    TEST_ASSERT_EQUAL_STRING("888888", result_str);
    
    free(result_str);
    di_release(&large_num2);
    di_release(&modulus2);
    di_release(&remainder2);
}

void test_large_factorial(void) {
    // 30! = 265252859812191058636308480000000
    di_int fact30 = di_factorial(30);
    TEST_ASSERT_NOT_NULL(fact30);
    
    char* result_str = di_to_string(fact30, 10);
    TEST_ASSERT_EQUAL_STRING("265252859812191058636308480000000", result_str);
    
    free(result_str);
    di_release(&fact30);
    
    // 40! = 815915283247897734345611269596115894272000000000
    di_int fact40 = di_factorial(40);
    result_str = di_to_string(fact40, 10);
    TEST_ASSERT_EQUAL_STRING("815915283247897734345611269596115894272000000000", result_str);
    
    free(result_str);
    di_release(&fact40);
}

void test_mixed_large_small_operations(void) {
    // Large number + small number
    di_int large = di_from_string("123456789012345678901234567890", 10);
    di_int sum = di_add_i32(large, 12345);
    
    char* result_str = di_to_string(sum, 10);
    TEST_ASSERT_EQUAL_STRING("123456789012345678901234580235", result_str);
    
    free(result_str);
    di_release(&sum);
    
    // Large number * small number
    di_int product = di_mul_i32(large, 999);
    result_str = di_to_string(product, 10);
    TEST_ASSERT_EQUAL_STRING("123333332223333333222333333322110", result_str);
    
    free(result_str);
    di_release(&large);
    di_release(&product);
    
    // Chain operations: (large + small) - small should equal original
    di_int num = di_from_string("987654321098765432109876543210", 10);
    di_int temp = di_add_i32(num, 54321);
    di_int result = di_sub_i32(temp, 54321);
    
    char* num_str = di_to_string(num, 10);
    char* result_str2 = di_to_string(result, 10);
    TEST_ASSERT_EQUAL_STRING(num_str, result_str2);
    
    free(num_str);
    free(result_str2);
    di_release(&num);
    di_release(&temp);
    di_release(&result);
}

void test_extreme_edge_cases(void) {
    // Test with zero
    di_int large = di_from_string("999999999999999999999999999999", 10);
    di_int zero = di_zero();
    
    // Addition with zero
    di_int sum_zero = di_add(large, zero);
    char* large_str = di_to_string(large, 10);
    char* sum_str = di_to_string(sum_zero, 10);
    TEST_ASSERT_EQUAL_STRING(large_str, sum_str);
    
    free(large_str);
    free(sum_str);
    di_release(&sum_zero);
    
    // Multiplication with zero
    di_int mult_zero = di_mul(large, zero);
    TEST_ASSERT_TRUE(di_is_zero(mult_zero));
    di_release(&mult_zero);
    
    // Test with one
    di_int one = di_one();
    di_int mult_one = di_mul(large, one);
    large_str = di_to_string(large, 10);
    char* mult_str = di_to_string(mult_one, 10);
    TEST_ASSERT_EQUAL_STRING(large_str, mult_str);
    
    free(large_str);
    free(mult_str);
    di_release(&mult_one);
    
    // Test negative large numbers
    di_int neg_large = di_negate(large);
    TEST_ASSERT_TRUE(di_is_negative(neg_large));
    
    // Double negation should give original
    di_int pos_again = di_negate(neg_large);
    large_str = di_to_string(large, 10);
    char* pos_str = di_to_string(pos_again, 10);
    TEST_ASSERT_EQUAL_STRING(large_str, pos_str);
    
    free(large_str);
    free(pos_str);
    
    di_release(&large);
    di_release(&zero);
    di_release(&one);
    di_release(&neg_large);
    di_release(&pos_again);
}

void test_string_conversion_large_numbers(void) {
    // Test very large number string conversion
    di_int huge = di_from_string("12345678901234567890123456789012345678901234567890123456789", 10);
    TEST_ASSERT_NOT_NULL(huge);
    
    char* huge_str = di_to_string(huge, 10);
    TEST_ASSERT_EQUAL_STRING("12345678901234567890123456789012345678901234567890123456789", huge_str);
    
    free(huge_str);
    di_release(&huge);
    
    // Test parsing and conversion consistency
    const char* test_numbers[] = {
        "999999999999999999999",
        "123456789123456789123456789",
        "777777777777777777777777777777777",
        "100000000000000000000000000000000000000"
    };
    
    for (int i = 0; i < 4; i++) {
        di_int num = di_from_string(test_numbers[i], 10);
        TEST_ASSERT_NOT_NULL(num);
        
        char* converted = di_to_string(num, 10);
        TEST_ASSERT_EQUAL_STRING(test_numbers[i], converted);
        
        free(converted);
        di_release(&num);
    }
}

int main(void) {
    UNITY_BEGIN();
    
    // Basic creation tests
    RUN_TEST(test_create_from_int32);
    RUN_TEST(test_create_zero);
    RUN_TEST(test_create_one);
    RUN_TEST(test_create_negative);
    
    // Conversion tests
    RUN_TEST(test_to_int32_basic);
    RUN_TEST(test_to_int32_negative);
    RUN_TEST(test_to_int32_zero);
    
    // Reference counting tests
    RUN_TEST(test_reference_counting);
    RUN_TEST(test_copy);
    
    // Comparison tests
    RUN_TEST(test_equal);
    RUN_TEST(test_less_greater);
    RUN_TEST(test_negative_comparison);
    
    // Arithmetic tests
    RUN_TEST(test_addition_basic);
    RUN_TEST(test_addition_int32);
    RUN_TEST(test_addition_with_zero);
    RUN_TEST(test_subtraction_basic);
    RUN_TEST(test_subtract_int32_basic);
    RUN_TEST(test_subtract_int32_negative_result);
    RUN_TEST(test_negation);
    
    // Multiplication tests
    RUN_TEST(test_multiplication_basic);
    RUN_TEST(test_multiplication_int32);
    RUN_TEST(test_multiplication_with_zero);
    RUN_TEST(test_multiplication_negative);
    
    // Modulo tests
    RUN_TEST(test_modulo_basic);
    RUN_TEST(test_modulo_exact_division);
    
    // Absolute value tests
    RUN_TEST(test_abs_positive);
    RUN_TEST(test_abs_negative);
    RUN_TEST(test_abs_zero);
    
    // Double conversion tests
    RUN_TEST(test_to_double_basic);
    RUN_TEST(test_to_double_negative);
    RUN_TEST(test_to_double_zero);
    
    // Unsigned creation tests
    RUN_TEST(test_from_uint32);
    RUN_TEST(test_from_uint64);
    
    // Overflow detection tests
    RUN_TEST(test_overflow_detection_int32);
    RUN_TEST(test_overflow_detection_int32_multiply);
    RUN_TEST(test_overflow_detection_int64_operations);
    
    // 64-bit tests
    RUN_TEST(test_int64_conversion);
    RUN_TEST(test_int64_negative);
    
    // String conversion tests
    RUN_TEST(test_to_string_basic);
    RUN_TEST(test_to_string_zero);
    
    // String parsing tests
    RUN_TEST(test_from_string_decimal);
    RUN_TEST(test_from_string_negative);
    RUN_TEST(test_from_string_hex);
    RUN_TEST(test_from_string_binary);
    RUN_TEST(test_from_string_zero);
    
    // Division tests
    RUN_TEST(test_divide_basic);
    RUN_TEST(test_divide_with_remainder);
    RUN_TEST(test_divide_negative);
    
    // Modular exponentiation tests
    RUN_TEST(test_mod_pow_basic);
    RUN_TEST(test_mod_pow_zero_exp);
    
    // Prime testing tests
    RUN_TEST(test_is_prime_small_primes);
    RUN_TEST(test_is_prime_composites);
    RUN_TEST(test_next_prime);
    
    // GCD/LCM tests
    RUN_TEST(test_gcd_basic);
    RUN_TEST(test_lcm_basic);
    RUN_TEST(test_extended_gcd);
    
    // Square root tests
    RUN_TEST(test_sqrt_perfect_square);
    RUN_TEST(test_sqrt_non_perfect_square);
    
    // Factorial tests
    RUN_TEST(test_factorial_small);
    RUN_TEST(test_factorial_zero);
    
    // Bitwise operation tests
    RUN_TEST(test_bitwise_and);
    RUN_TEST(test_bitwise_or);
    RUN_TEST(test_bitwise_xor);
    RUN_TEST(test_bitwise_not);
    RUN_TEST(test_shift_left);
    RUN_TEST(test_shift_right);
    
    // Random number tests
    RUN_TEST(test_random_basic);
    RUN_TEST(test_random_range);
    
    // Utility function tests
    RUN_TEST(test_bit_length);
    
    // Large number tests
    RUN_TEST(test_large_multiplication);
    RUN_TEST(test_large_division);
    RUN_TEST(test_large_modulo);
    RUN_TEST(test_large_factorial);
    RUN_TEST(test_mixed_large_small_operations);
    RUN_TEST(test_extreme_edge_cases);
    RUN_TEST(test_string_conversion_large_numbers);
    
    return UNITY_END();
}