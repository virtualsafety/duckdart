#pragma once

namespace duckart {

#define BSWAP16(x)                                \
    ((uint16_t)((((uint16_t)(x) & 0xff00) >> 8) | \
                (((uint16_t)(x) & 0x00ff) << 8)))

#define BSWAP32(x)                                     \
    ((uint32_t)((((uint32_t)(x) & 0xff000000) >> 24) | \
                (((uint32_t)(x) & 0x00ff0000) >> 8) |  \
                (((uint32_t)(x) & 0x0000ff00) << 8) |  \
                (((uint32_t)(x) & 0x000000ff) << 24)))

#define BSWAP64(x)                                                \
    ((uint64_t)((((uint64_t)(x) & 0xff00000000000000ull) >> 56) | \
                (((uint64_t)(x) & 0x00ff000000000000ull) >> 40) | \
                (((uint64_t)(x) & 0x0000ff0000000000ull) >> 24) | \
                (((uint64_t)(x) & 0x000000ff00000000ull) >> 8) |  \
                (((uint64_t)(x) & 0x00000000ff000000ull) << 8) |  \
                (((uint64_t)(x) & 0x0000000000ff0000ull) << 24) | \
                (((uint64_t)(x) & 0x000000000000ff00ull) << 40) | \
                (((uint64_t)(x) & 0x00000000000000ffull) << 56)))

static inline uint8_t BSwap(const uint8_t &x) { return x; }

static inline uint16_t BSwap(const uint16_t &x) { return BSWAP16(x); }

static inline uint32_t BSwap(const uint32_t &x) { return BSWAP32(x); }

static inline uint64_t BSwap(const uint64_t &x) { return BSWAP64(x); }

static inline int64_t BSwap(const int64_t &x) {
    return static_cast<int64_t>(BSWAP64(x));
}

template <class T>
static inline T BSwap(const T &x) {
	static_assert(sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8,
	              "Size of type must be 1, 2, 4, or 8 for BSwap");
	if (sizeof(T) == 1) {
		return x;
	} else if (sizeof(T) == 2) {
		return BSWAP16(x);
	} else if (sizeof(T) == 4) {		 
		return  BSWAP32(x) ;
	} else {		 
		return  BSWAP64(x) ;
	}
}
}
