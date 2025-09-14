# Release Notes

## Version 1.1.0 - September 2025

### New Features

- **Added `di_is_one()` predicate function** - Test if a di_int equals 1, complementing existing `di_is_zero()`, `di_is_positive()`, and `di_is_negative()` functions

### Breaking Changes

- **Implemented fail-fast error handling** - Functions now assert on invalid inputs rather than returning NULL
  - NULL pointer parameters that should not be NULL now trigger assertions
  - Memory allocation failures now trigger assertions immediately
  - Only `di_release()` gracefully handles NULL (like `free(NULL)`)
  - This change makes bugs obvious immediately rather than allowing them to propagate

### API Changes

- **Predicate functions now assert on NULL input**: `di_is_zero()`, `di_is_negative()`, `di_is_positive()`, `di_is_one()`
- **Comparison functions now assert on NULL input**: `di_compare()`, `di_eq()`, `di_lt()`, etc.
- **Conversion functions now assert on NULL input**: `di_to_int32()`, `di_to_string()`, etc.
- **Core operations already had assertions**: `di_add()`, `di_sub()`, `di_mul()`, `di_div()`
- **Memory management functions**: `di_copy()` and `di_retain()` now assert on NULL input
- **Memory allocation**: All internal allocations now assert on failure

### Technical Improvements

- **Better debugging experience** - Assertions provide clear error messages at the exact failure location
- **Simplified error handling** - No need to check return values for allocation failures
- **More predictable behavior** - Functions either succeed or abort, never return invalid state
- **Embedded systems friendly** - Continuing after allocation failure is usually pointless anyway

### Migration Guide

If you were previously checking for NULL returns from functions like `di_copy()`, `di_retain()`, or predicate functions, you can remove those checks as they will now assert on invalid input:

```c
// Before v1.1.0
di_int copy = di_copy(original);
if (!copy) {
    // Handle error
    return -1;
}

// After v1.1.0 - no need to check, will assert if original is NULL
di_int copy = di_copy(original);
```

### Testing

- All 92 unit tests pass
- Comprehensive test coverage for new assertion behavior
- Memory allocation failure testing
- NULL pointer handling verification

---

*Note: This library follows fail-fast principles for better debugging and reliability. Invalid inputs are programming errors and should be caught during development, not handled gracefully at runtime.*