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
    RUN_TEST(test_negation);
    
    // Overflow detection tests
    RUN_TEST(test_overflow_detection_int32);
    
    // 64-bit tests
    RUN_TEST(test_int64_conversion);
    RUN_TEST(test_int64_negative);
    
    // String conversion tests
    RUN_TEST(test_to_string_basic);
    RUN_TEST(test_to_string_zero);
    
    return UNITY_END();
}