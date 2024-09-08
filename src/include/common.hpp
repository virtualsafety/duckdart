#pragma once
#include <cstdint>
#include <cassert>
#include <functional> 

#define D_ASSERT assert

namespace duckart {
//! a saner size_t for loop indices etc
typedef uint64_t idx_t;

//! data pointers
typedef uint8_t data_t;
typedef data_t *data_ptr_t;
typedef const data_t *const_data_ptr_t;

template<typename T>
using reference = std::reference_wrapper<T>;

template <class SRC>
char *char_ptr_cast(SRC *src) {  // NOLINT: naming
    return reinterpret_cast<char *>(src);
}

template <class SRC>
const_data_ptr_t const_data_ptr_cast(const SRC *src) {  // NOLINT: naming
    return reinterpret_cast<const_data_ptr_t>(src);
}

template <class SRC>
data_ptr_t data_ptr_cast(SRC *src) { // NOLINT: naming
	return reinterpret_cast<data_ptr_t>(src);
}

template <typename T>
const T Load(const_data_ptr_t ptr) {
    T ret;
    memcpy(&ret, ptr, sizeof(ret));  // NOLINT
    return ret;
}

template <typename T>
void Store(const T &val, data_ptr_t ptr) {
    memcpy(ptr, (void *)&val, sizeof(val));  // NOLINT
}

template <typename T>
constexpr T MinValue(T a, T b) {
    return a < b ? a : b;
}

//! node type
enum class NType : uint8_t {
    LEAF = 1,
    NODE_4 = 2,
    NODE_16 = 3,
    NODE_48 = 4,
    NODE_256 = 5,
    PREFIX = 6,
    NODE_DUMMY = 7    
};


//! Node thresholds
static constexpr uint8_t NODE_48_SHRINK_THRESHOLD = 12;
static constexpr uint8_t NODE_256_SHRINK_THRESHOLD = 36;
//! Node sizes
static constexpr uint8_t NODE_4_CAPACITY = 4;
static constexpr uint8_t NODE_16_CAPACITY = 16;
static constexpr uint8_t NODE_48_CAPACITY = 48;
static constexpr uint16_t NODE_256_CAPACITY = 256;
//! Other constants
static constexpr uint8_t PREFIX_SIZE = 15;
static constexpr uint8_t EMPTY_MARKER = 48;  
static constexpr idx_t INVALID_INDEX =  idx_t(-1);
}
