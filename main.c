#include "dynamic_bigint.h"
#include "unity.h"

void setUp(void) {
    // Set up code before each test
}

void tearDown(void) {
    // Clean up code after each test
}

// Basic creation tests
void test_create_from_int32(void) {
    db_bigint a = db_from_int32(42);
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_FALSE(db_is_zero(a));
    TEST_ASSERT_TRUE(db_is_positive(a));
    TEST_ASSERT_FALSE(db_is_negative(a));
    db_release(&a);
    TEST_ASSERT_NULL(a);
}

void test_create_zero(void) {
    db_bigint zero = db_zero();
    TEST_ASSERT_NOT_NULL(zero);
    TEST_ASSERT_TRUE(db_is_zero(zero));
    TEST_ASSERT_FALSE(db_is_positive(zero));
    TEST_ASSERT_FALSE(db_is_negative(zero));
    db_release(&zero);
}

void test_create_one(void) {
    db_bigint one = db_one();
    TEST_ASSERT_NOT_NULL(one);
    TEST_ASSERT_FALSE(db_is_zero(one));
    TEST_ASSERT_TRUE(db_is_positive(one));
    TEST_ASSERT_FALSE(db_is_negative(one));
    db_release(&one);
}

void test_create_negative(void) {
    db_bigint neg = db_from_int32(-42);
    TEST_ASSERT_NOT_NULL(neg);
    TEST_ASSERT_FALSE(db_is_zero(neg));
    TEST_ASSERT_FALSE(db_is_positive(neg));
    TEST_ASSERT_TRUE(db_is_negative(neg));
    db_release(&neg);
}

// Conversion tests
void test_to_int32_basic(void) {
    db_bigint a = db_from_int32(123);
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(a, &result));
    TEST_ASSERT_EQUAL_INT32(123, result);
    db_release(&a);
}

void test_to_int32_negative(void) {
    db_bigint a = db_from_int32(-456);
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(a, &result));
    TEST_ASSERT_EQUAL_INT32(-456, result);
    db_release(&a);
}

void test_to_int32_zero(void) {
    db_bigint zero = db_zero();
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(zero, &result));
    TEST_ASSERT_EQUAL_INT32(0, result);
    db_release(&zero);
}

// Reference counting tests
void test_reference_counting(void) {
    db_bigint a = db_from_int32(42);
    TEST_ASSERT_EQUAL_UINT(1, db_ref_count(a));
    
    db_bigint b = db_retain(a);
    TEST_ASSERT_EQUAL_PTR(a, b);
    TEST_ASSERT_EQUAL_UINT(2, db_ref_count(a));
    TEST_ASSERT_EQUAL_UINT(2, db_ref_count(b));
    
    db_release(&a);
    TEST_ASSERT_NULL(a);
    TEST_ASSERT_EQUAL_UINT(1, db_ref_count(b));
    
    db_release(&b);
    TEST_ASSERT_NULL(b);
}

void test_copy(void) {
    db_bigint a = db_from_int32(789);
    db_bigint copy = db_copy(a);
    
    TEST_ASSERT_NOT_NULL(copy);
    TEST_ASSERT_TRUE(a != copy);
    TEST_ASSERT_EQUAL_UINT(1, db_ref_count(a));
    TEST_ASSERT_EQUAL_UINT(1, db_ref_count(copy));
    
    // Test they have same value
    TEST_ASSERT_TRUE(db_equal(a, copy));
    
    db_release(&a);
    db_release(&copy);
}

// Comparison tests
void test_equal(void) {
    db_bigint a = db_from_int32(123);
    db_bigint b = db_from_int32(123);
    db_bigint c = db_from_int32(456);
    
    TEST_ASSERT_TRUE(db_equal(a, b));
    TEST_ASSERT_FALSE(db_equal(a, c));
    TEST_ASSERT_EQUAL_INT(0, db_compare(a, b));
    
    db_release(&a);
    db_release(&b);
    db_release(&c);
}

void test_less_greater(void) {
    db_bigint small = db_from_int32(10);
    db_bigint big = db_from_int32(20);
    
    TEST_ASSERT_TRUE(db_less(small, big));
    TEST_ASSERT_FALSE(db_less(big, small));
    TEST_ASSERT_TRUE(db_greater(big, small));
    TEST_ASSERT_FALSE(db_greater(small, big));
    
    TEST_ASSERT_TRUE(db_less_equal(small, big));
    TEST_ASSERT_TRUE(db_greater_equal(big, small));
    
    db_release(&small);
    db_release(&big);
}

void test_negative_comparison(void) {
    db_bigint pos = db_from_int32(10);
    db_bigint neg = db_from_int32(-10);
    
    TEST_ASSERT_TRUE(db_less(neg, pos));
    TEST_ASSERT_TRUE(db_greater(pos, neg));
    TEST_ASSERT_FALSE(db_equal(pos, neg));
    
    db_release(&pos);
    db_release(&neg);
}

// Basic arithmetic tests
void test_addition_basic(void) {
    db_bigint a = db_from_int32(10);
    db_bigint b = db_from_int32(20);
    db_bigint sum = db_add(a, b);
    
    TEST_ASSERT_NOT_NULL(sum);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(sum, &result));
    TEST_ASSERT_EQUAL_INT32(30, result);
    
    db_release(&a);
    db_release(&b);
    db_release(&sum);
}

void test_addition_int32(void) {
    db_bigint a = db_from_int32(10);
    db_bigint sum = db_add_int32(a, 15);
    
    TEST_ASSERT_NOT_NULL(sum);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(sum, &result));
    TEST_ASSERT_EQUAL_INT32(25, result);
    
    db_release(&a);
    db_release(&sum);
}

void test_addition_with_zero(void) {
    db_bigint a = db_from_int32(42);
    db_bigint zero = db_zero();
    db_bigint sum = db_add(a, zero);
    
    TEST_ASSERT_NOT_NULL(sum);
    TEST_ASSERT_TRUE(db_equal(a, sum));
    
    db_release(&a);
    db_release(&zero);
    db_release(&sum);
}

void test_subtraction_basic(void) {
    db_bigint a = db_from_int32(30);
    db_bigint b = db_from_int32(10);
    db_bigint diff = db_subtract(a, b);
    
    TEST_ASSERT_NOT_NULL(diff);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(diff, &result));
    TEST_ASSERT_EQUAL_INT32(20, result);
    
    db_release(&a);
    db_release(&b);
    db_release(&diff);
}

void test_negation(void) {
    db_bigint pos = db_from_int32(42);
    db_bigint neg = db_negate(pos);
    
    TEST_ASSERT_NOT_NULL(neg);
    TEST_ASSERT_TRUE(db_is_negative(neg));
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(neg, &result));
    TEST_ASSERT_EQUAL_INT32(-42, result);
    
    // Test double negation
    db_bigint pos_again = db_negate(neg);
    TEST_ASSERT_TRUE(db_equal(pos, pos_again));
    
    db_release(&pos);
    db_release(&neg);
    db_release(&pos_again);
}

// Multiplication tests
void test_multiplication_basic(void) {
    db_bigint a = db_from_int32(6);
    db_bigint b = db_from_int32(7);
    db_bigint product = db_multiply(a, b);
    
    TEST_ASSERT_NOT_NULL(product);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(product, &result));
    TEST_ASSERT_EQUAL_INT32(42, result);
    
    db_release(&a);
    db_release(&b);
    db_release(&product);
}

void test_multiplication_int32(void) {
    db_bigint a = db_from_int32(8);
    db_bigint product = db_multiply_int32(a, 5);
    
    TEST_ASSERT_NOT_NULL(product);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(product, &result));
    TEST_ASSERT_EQUAL_INT32(40, result);
    
    db_release(&a);
    db_release(&product);
}

void test_multiplication_with_zero(void) {
    db_bigint a = db_from_int32(42);
    db_bigint zero = db_zero();
    db_bigint product = db_multiply(a, zero);
    
    TEST_ASSERT_NOT_NULL(product);
    TEST_ASSERT_TRUE(db_is_zero(product));
    
    db_release(&a);
    db_release(&zero);
    db_release(&product);
}

void test_multiplication_negative(void) {
    db_bigint a = db_from_int32(-3);
    db_bigint b = db_from_int32(4);
    db_bigint product = db_multiply(a, b);
    
    TEST_ASSERT_NOT_NULL(product);
    TEST_ASSERT_TRUE(db_is_negative(product));
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(product, &result));
    TEST_ASSERT_EQUAL_INT32(-12, result);
    
    db_release(&a);
    db_release(&b);
    db_release(&product);
}

// Modulo tests
void test_modulo_basic(void) {
    db_bigint a = db_from_int32(17);
    db_bigint b = db_from_int32(5);
    db_bigint remainder = db_mod(a, b);
    
    TEST_ASSERT_NOT_NULL(remainder);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(remainder, &result));
    TEST_ASSERT_EQUAL_INT32(2, result);
    
    db_release(&a);
    db_release(&b);
    db_release(&remainder);
}

void test_modulo_exact_division(void) {
    db_bigint a = db_from_int32(15);
    db_bigint b = db_from_int32(3);
    db_bigint remainder = db_mod(a, b);
    
    TEST_ASSERT_NOT_NULL(remainder);
    TEST_ASSERT_TRUE(db_is_zero(remainder));
    
    db_release(&a);
    db_release(&b);
    db_release(&remainder);
}

// Absolute value tests
void test_abs_positive(void) {
    db_bigint pos = db_from_int32(42);
    db_bigint abs_val = db_abs(pos);
    
    TEST_ASSERT_NOT_NULL(abs_val);
    TEST_ASSERT_TRUE(db_equal(pos, abs_val));
    
    db_release(&pos);
    db_release(&abs_val);
}

void test_abs_negative(void) {
    db_bigint neg = db_from_int32(-42);
    db_bigint abs_val = db_abs(neg);
    
    TEST_ASSERT_NOT_NULL(abs_val);
    TEST_ASSERT_FALSE(db_is_negative(abs_val));
    TEST_ASSERT_TRUE(db_is_positive(abs_val));
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(abs_val, &result));
    TEST_ASSERT_EQUAL_INT32(42, result);
    
    db_release(&neg);
    db_release(&abs_val);
}

void test_abs_zero(void) {
    db_bigint zero = db_zero();
    db_bigint abs_val = db_abs(zero);
    
    TEST_ASSERT_NOT_NULL(abs_val);
    TEST_ASSERT_TRUE(db_is_zero(abs_val));
    
    db_release(&zero);
    db_release(&abs_val);
}

// Double conversion tests (using basic float comparison since Unity has double disabled)
void test_to_double_basic(void) {
    db_bigint a = db_from_int32(12345);
    double result = db_to_double(a);
    
    TEST_ASSERT_FLOAT_WITHIN(0.1, 12345.0f, (float)result);
    
    db_release(&a);
}

void test_to_double_negative(void) {
    db_bigint a = db_from_int32(-67890);
    double result = db_to_double(a);
    
    TEST_ASSERT_FLOAT_WITHIN(0.1, -67890.0f, (float)result);
    
    db_release(&a);
}

void test_to_double_zero(void) {
    db_bigint zero = db_zero();
    double result = db_to_double(zero);
    
    TEST_ASSERT_FLOAT_WITHIN(0.1, 0.0f, (float)result);
    
    db_release(&zero);
}

// Subtraction with int32 tests
void test_subtract_int32_basic(void) {
    db_bigint a = db_from_int32(50);
    db_bigint diff = db_subtract_int32(a, 20);
    
    TEST_ASSERT_NOT_NULL(diff);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(diff, &result));
    TEST_ASSERT_EQUAL_INT32(30, result);
    
    db_release(&a);
    db_release(&diff);
}

void test_subtract_int32_negative_result(void) {
    db_bigint a = db_from_int32(10);
    db_bigint diff = db_subtract_int32(a, 25);
    
    TEST_ASSERT_NOT_NULL(diff);
    TEST_ASSERT_TRUE(db_is_negative(diff));
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(diff, &result));
    TEST_ASSERT_EQUAL_INT32(-15, result);
    
    db_release(&a);
    db_release(&diff);
}

// Unsigned integer creation tests
void test_from_uint32(void) {
    db_bigint a = db_from_uint32(4294967295U); // UINT32_MAX
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_FALSE(db_is_negative(a));
    TEST_ASSERT_TRUE(db_is_positive(a));
    
    // Convert to int64 since it won't fit in int32
    int64_t result;
    TEST_ASSERT_TRUE(db_to_int64(a, &result));
    TEST_ASSERT_EQUAL_INT64(4294967295LL, result);
    
    db_release(&a);
}

void test_from_uint64(void) {
    db_bigint a = db_from_uint64(9876543210123ULL);
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_FALSE(db_is_negative(a));
    TEST_ASSERT_TRUE(db_is_positive(a));
    
    int64_t result;
    TEST_ASSERT_TRUE(db_to_int64(a, &result));
    TEST_ASSERT_EQUAL_INT64(9876543210123LL, result);
    
    db_release(&a);
}

// Additional overflow detection tests
void test_overflow_detection_int32_multiply(void) {
    int32_t result;
    
    // Test normal case
    TEST_ASSERT_TRUE(db_multiply_overflow_int32(100, 200, &result));
    TEST_ASSERT_EQUAL_INT32(20000, result);
    
    // Test overflow case
    TEST_ASSERT_FALSE(db_multiply_overflow_int32(INT32_MAX, 2, &result));
    
    // Test zero multiplication
    TEST_ASSERT_TRUE(db_multiply_overflow_int32(INT32_MAX, 0, &result));
    TEST_ASSERT_EQUAL_INT32(0, result);
}

void test_overflow_detection_int64_operations(void) {
    int64_t result;
    
    // Test int64 addition
    TEST_ASSERT_TRUE(db_add_overflow_int64(1000000000000LL, 2000000000000LL, &result));
    TEST_ASSERT_EQUAL_INT64(3000000000000LL, result);
    
    // Test int64 addition overflow
    TEST_ASSERT_FALSE(db_add_overflow_int64(INT64_MAX, 1, &result));
    
    // Test int64 subtraction
    TEST_ASSERT_TRUE(db_subtract_overflow_int64(5000000000000LL, 2000000000000LL, &result));
    TEST_ASSERT_EQUAL_INT64(3000000000000LL, result);
    
    // Test int64 subtraction underflow
    TEST_ASSERT_FALSE(db_subtract_overflow_int64(INT64_MIN, 1, &result));
    
    // Test int64 multiplication
    TEST_ASSERT_TRUE(db_multiply_overflow_int64(1000000LL, 2000000LL, &result));
    TEST_ASSERT_EQUAL_INT64(2000000000000LL, result);
    
    // Test int64 multiplication overflow
    TEST_ASSERT_FALSE(db_multiply_overflow_int64(INT64_MAX, 2, &result));
}

// Overflow detection tests
void test_overflow_detection_int32(void) {
    int32_t result;
    
    // Test normal case
    TEST_ASSERT_TRUE(db_add_overflow_int32(100, 200, &result));
    TEST_ASSERT_EQUAL_INT32(300, result);
    
    // Test overflow case
    TEST_ASSERT_FALSE(db_add_overflow_int32(INT32_MAX, 1, &result));
    
    // Test underflow case
    TEST_ASSERT_FALSE(db_subtract_overflow_int32(INT32_MIN, 1, &result));
}

// Large number tests (using int64)
void test_int64_conversion(void) {
    db_bigint a = db_from_int64(1234567890123LL);
    TEST_ASSERT_NOT_NULL(a);
    
    int64_t result;
    TEST_ASSERT_TRUE(db_to_int64(a, &result));
    TEST_ASSERT_EQUAL_INT64(1234567890123LL, result);
    
    db_release(&a);
}

void test_int64_negative(void) {
    db_bigint a = db_from_int64(-9876543210987LL);
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_TRUE(db_is_negative(a));
    
    int64_t result;
    TEST_ASSERT_TRUE(db_to_int64(a, &result));
    TEST_ASSERT_EQUAL_INT64(-9876543210987LL, result);
    
    db_release(&a);
}

// String conversion tests (basic)
void test_to_string_basic(void) {
    db_bigint a = db_from_int32(12345);
    char* str = db_to_string(a, 10);
    
    TEST_ASSERT_NOT_NULL(str);
    TEST_ASSERT_EQUAL_STRING("12345", str);
    
    free(str);
    db_release(&a);
}

void test_to_string_zero(void) {
    db_bigint zero = db_zero();
    char* str = db_to_string(zero, 10);
    
    TEST_ASSERT_NOT_NULL(str);
    TEST_ASSERT_EQUAL_STRING("0", str);
    
    free(str);
    db_release(&zero);
}

// String parsing tests
void test_from_string_decimal(void) {
    db_bigint a = db_from_string("12345", 10);
    TEST_ASSERT_NOT_NULL(a);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(a, &result));
    TEST_ASSERT_EQUAL_INT32(12345, result);
    
    db_release(&a);
}

void test_from_string_negative(void) {
    db_bigint a = db_from_string("-6789", 10);
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_TRUE(db_is_negative(a));
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(a, &result));
    TEST_ASSERT_EQUAL_INT32(-6789, result);
    
    db_release(&a);
}

void test_from_string_hex(void) {
    db_bigint a = db_from_string("FF", 16);
    TEST_ASSERT_NOT_NULL(a);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(a, &result));
    TEST_ASSERT_EQUAL_INT32(255, result);
    
    db_release(&a);
}

void test_from_string_binary(void) {
    db_bigint a = db_from_string("1010", 2);
    TEST_ASSERT_NOT_NULL(a);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(a, &result));
    TEST_ASSERT_EQUAL_INT32(10, result);
    
    db_release(&a);
}

void test_from_string_zero(void) {
    db_bigint a = db_from_string("0", 10);
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_TRUE(db_is_zero(a));
    
    db_release(&a);
}

// Division tests
void test_divide_basic(void) {
    db_bigint a = db_from_int32(42);
    db_bigint b = db_from_int32(6);
    db_bigint quotient = db_divide(a, b);
    
    TEST_ASSERT_NOT_NULL(quotient);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(quotient, &result));
    TEST_ASSERT_EQUAL_INT32(7, result);
    
    db_release(&a);
    db_release(&b);
    db_release(&quotient);
}

void test_divide_with_remainder(void) {
    db_bigint a = db_from_int32(17);
    db_bigint b = db_from_int32(5);
    db_bigint quotient = db_divide(a, b);
    
    TEST_ASSERT_NOT_NULL(quotient);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(quotient, &result));
    TEST_ASSERT_EQUAL_INT32(3, result);
    
    db_release(&a);
    db_release(&b);
    db_release(&quotient);
}

void test_divide_negative(void) {
    db_bigint a = db_from_int32(-20);
    db_bigint b = db_from_int32(4);
    db_bigint quotient = db_divide(a, b);
    
    TEST_ASSERT_NOT_NULL(quotient);
    TEST_ASSERT_TRUE(db_is_negative(quotient));
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(quotient, &result));
    TEST_ASSERT_EQUAL_INT32(-5, result);
    
    db_release(&a);
    db_release(&b);
    db_release(&quotient);
}

// Modular exponentiation tests
void test_mod_pow_basic(void) {
    db_bigint base = db_from_int32(2);
    db_bigint exp = db_from_int32(8);
    db_bigint mod = db_from_int32(100);
    db_bigint result = db_mod_pow(base, exp, mod);
    
    TEST_ASSERT_NOT_NULL(result);
    
    int32_t val;
    TEST_ASSERT_TRUE(db_to_int32(result, &val));
    TEST_ASSERT_EQUAL_INT32(56, val); // 2^8 % 100 = 256 % 100 = 56
    
    db_release(&base);
    db_release(&exp);
    db_release(&mod);
    db_release(&result);
}

void test_mod_pow_zero_exp(void) {
    db_bigint base = db_from_int32(5);
    db_bigint exp = db_zero();
    db_bigint mod = db_from_int32(7);
    db_bigint result = db_mod_pow(base, exp, mod);
    
    TEST_ASSERT_NOT_NULL(result);
    
    int32_t val;
    TEST_ASSERT_TRUE(db_to_int32(result, &val));
    TEST_ASSERT_EQUAL_INT32(1, val); // Any number to power 0 is 1
    
    db_release(&base);
    db_release(&exp);
    db_release(&mod);
    db_release(&result);
}

// Prime testing tests
void test_is_prime_small_primes(void) {
    db_bigint two = db_from_int32(2);
    db_bigint three = db_from_int32(3);
    db_bigint five = db_from_int32(5);
    db_bigint seven = db_from_int32(7);
    
    TEST_ASSERT_TRUE(db_is_prime(two, 10));
    TEST_ASSERT_TRUE(db_is_prime(three, 10));
    TEST_ASSERT_TRUE(db_is_prime(five, 10));
    TEST_ASSERT_TRUE(db_is_prime(seven, 10));
    
    db_release(&two);
    db_release(&three);
    db_release(&five);
    db_release(&seven);
}

void test_is_prime_composites(void) {
    db_bigint four = db_from_int32(4);
    db_bigint six = db_from_int32(6);
    db_bigint eight = db_from_int32(8);
    db_bigint nine = db_from_int32(9);
    
    TEST_ASSERT_FALSE(db_is_prime(four, 10));
    TEST_ASSERT_FALSE(db_is_prime(six, 10));
    TEST_ASSERT_FALSE(db_is_prime(eight, 10));
    TEST_ASSERT_FALSE(db_is_prime(nine, 10));
    
    db_release(&four);
    db_release(&six);
    db_release(&eight);
    db_release(&nine);
}

void test_next_prime(void) {
    db_bigint ten = db_from_int32(10);
    db_bigint next = db_next_prime(ten);
    
    TEST_ASSERT_NOT_NULL(next);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(next, &result));
    TEST_ASSERT_EQUAL_INT32(11, result);
    
    db_release(&ten);
    db_release(&next);
}

// GCD/LCM tests
void test_gcd_basic(void) {
    db_bigint a = db_from_int32(48);
    db_bigint b = db_from_int32(18);
    db_bigint gcd = db_gcd(a, b);
    
    TEST_ASSERT_NOT_NULL(gcd);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(gcd, &result));
    TEST_ASSERT_EQUAL_INT32(6, result);
    
    db_release(&a);
    db_release(&b);
    db_release(&gcd);
}

void test_lcm_basic(void) {
    db_bigint a = db_from_int32(12);
    db_bigint b = db_from_int32(18);
    db_bigint lcm = db_lcm(a, b);
    
    TEST_ASSERT_NOT_NULL(lcm);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(lcm, &result));
    TEST_ASSERT_EQUAL_INT32(36, result);
    
    db_release(&a);
    db_release(&b);
    db_release(&lcm);
}

void test_extended_gcd(void) {
    db_bigint a = db_from_int32(35);
    db_bigint b = db_from_int32(15);
    db_bigint x = NULL, y = NULL;
    db_bigint gcd = db_extended_gcd(a, b, &x, &y);
    
    TEST_ASSERT_NOT_NULL(gcd);
    TEST_ASSERT_NOT_NULL(x);
    TEST_ASSERT_NOT_NULL(y);
    
    int32_t gcd_val;
    TEST_ASSERT_TRUE(db_to_int32(gcd, &gcd_val));
    TEST_ASSERT_EQUAL_INT32(5, gcd_val);
    
    db_release(&a);
    db_release(&b);
    db_release(&gcd);
    db_release(&x);
    db_release(&y);
}

// Square root tests
void test_sqrt_perfect_square(void) {
    db_bigint num = db_from_int32(144);
    db_bigint root = db_sqrt(num);
    
    TEST_ASSERT_NOT_NULL(root);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(root, &result));
    TEST_ASSERT_EQUAL_INT32(12, result);
    
    db_release(&num);
    db_release(&root);
}

void test_sqrt_non_perfect_square(void) {
    db_bigint num = db_from_int32(10);
    db_bigint root = db_sqrt(num);
    
    TEST_ASSERT_NOT_NULL(root);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(root, &result));
    TEST_ASSERT_EQUAL_INT32(3, result); // floor(sqrt(10)) = 3
    
    db_release(&num);
    db_release(&root);
}

// Factorial tests
void test_factorial_small(void) {
    db_bigint fact5 = db_factorial(5);
    TEST_ASSERT_NOT_NULL(fact5);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(fact5, &result));
    TEST_ASSERT_EQUAL_INT32(120, result);
    
    db_release(&fact5);
}

void test_factorial_zero(void) {
    db_bigint fact0 = db_factorial(0);
    TEST_ASSERT_NOT_NULL(fact0);
    
    int32_t result;
    TEST_ASSERT_TRUE(db_to_int32(fact0, &result));
    TEST_ASSERT_EQUAL_INT32(1, result);
    
    db_release(&fact0);
}

// Bitwise operation tests
void test_bitwise_and(void) {
    db_bigint a = db_from_int32(12); // 1100 in binary
    db_bigint b = db_from_int32(10); // 1010 in binary
    db_bigint result = db_and(a, b);
    
    TEST_ASSERT_NOT_NULL(result);
    
    int32_t val;
    TEST_ASSERT_TRUE(db_to_int32(result, &val));
    TEST_ASSERT_EQUAL_INT32(8, val); // 1000 in binary
    
    db_release(&a);
    db_release(&b);
    db_release(&result);
}

void test_bitwise_or(void) {
    db_bigint a = db_from_int32(12); // 1100 in binary
    db_bigint b = db_from_int32(10); // 1010 in binary
    db_bigint result = db_or(a, b);
    
    TEST_ASSERT_NOT_NULL(result);
    
    int32_t val;
    TEST_ASSERT_TRUE(db_to_int32(result, &val));
    TEST_ASSERT_EQUAL_INT32(14, val); // 1110 in binary
    
    db_release(&a);
    db_release(&b);
    db_release(&result);
}

void test_bitwise_xor(void) {
    db_bigint a = db_from_int32(12); // 1100 in binary
    db_bigint b = db_from_int32(10); // 1010 in binary
    db_bigint result = db_xor(a, b);
    
    TEST_ASSERT_NOT_NULL(result);
    
    int32_t val;
    TEST_ASSERT_TRUE(db_to_int32(result, &val));
    TEST_ASSERT_EQUAL_INT32(6, val); // 0110 in binary
    
    db_release(&a);
    db_release(&b);
    db_release(&result);
}

void test_bitwise_not(void) {
    db_bigint a = db_from_int32(5); // Binary: 101
    db_bigint result = db_not(a);
    
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_FALSE(db_is_negative(result)); // Bitwise NOT returns positive result
    
    // The exact value will be large since it's ~5 with extended precision
    // Just verify it's a valid positive result
    TEST_ASSERT_TRUE(db_is_positive(result));
    
    db_release(&a);
    db_release(&result);
}

void test_shift_left(void) {
    db_bigint a = db_from_int32(5); // Binary: 101
    db_bigint result = db_shift_left(a, 2);
    
    TEST_ASSERT_NOT_NULL(result);
    
    int32_t val;
    TEST_ASSERT_TRUE(db_to_int32(result, &val));
    TEST_ASSERT_EQUAL_INT32(20, val); // 5 << 2 = 20 (Binary: 10100)
    
    db_release(&a);
    db_release(&result);
}

void test_shift_right(void) {
    db_bigint a = db_from_int32(20); // Binary: 10100
    db_bigint result = db_shift_right(a, 2);
    
    TEST_ASSERT_NOT_NULL(result);
    
    int32_t val;
    TEST_ASSERT_TRUE(db_to_int32(result, &val));
    TEST_ASSERT_EQUAL_INT32(5, val); // 20 >> 2 = 5 (Binary: 101)
    
    db_release(&a);
    db_release(&result);
}

// Random number tests (basic - just verify they don't crash)
void test_random_basic(void) {
    db_bigint random_bits = db_random(32);
    TEST_ASSERT_NOT_NULL(random_bits);
    
    db_release(&random_bits);
}

void test_random_range(void) {
    db_bigint min_val = db_from_int32(10);
    db_bigint max_val = db_from_int32(100);
    db_bigint random_val = db_random_range(min_val, max_val);
    
    TEST_ASSERT_NOT_NULL(random_val);
    TEST_ASSERT_TRUE(db_greater_equal(random_val, min_val));
    TEST_ASSERT_TRUE(db_less(random_val, max_val));
    
    db_release(&min_val);
    db_release(&max_val);
    db_release(&random_val);
}

// Utility function tests
void test_bit_length(void) {
    db_bigint a = db_from_int32(7); // Binary: 111
    size_t length = db_bit_length(a);
    
    TEST_ASSERT_EQUAL_size_t(3, length);
    
    db_release(&a);
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
    
    return UNITY_END();
}