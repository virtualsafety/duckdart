#pragma once

#include <limits.h>

#include <cfloat>
#include <cstring>  // strlen() on Solaris

#include "bswap.hpp"
#include "common.hpp"
#include "exception.hpp"

namespace duckart {

struct Radix {
   public:
    static inline bool IsLittleEndian() {
        int n = 1;
        if (*char_ptr_cast(&n) == 1) {
            return true;
        } else {
            return false;
        }
    }

    template <class T>
    static inline void EncodeData(data_ptr_t dataptr, T value) {
        throw NotImplementedException("Cannot create data from this type");
    }

   template <class T>
    static inline T DecodeData(data_ptr_t dataptr) {
        throw NotImplementedException("Cannot decode data to this type");
    }

    static inline void EncodeStringDataPrefix(data_ptr_t dataptr,
                                              string_t value,
                                              idx_t prefix_len) {
        auto len = value.GetSize();
        memcpy(dataptr, value.GetData(), MinValue(static_cast<idx_t>(len), prefix_len));
        if (len < prefix_len) {
            memset(dataptr + len, '\0', prefix_len - len);
        }
    }

    static inline uint8_t FlipSign(uint8_t key_byte) { return key_byte ^ 128; }
};

template <>
inline void Radix::EncodeData(data_ptr_t dataptr, int8_t value) {
    uint8_t bytes;  // dance around signedness conversion check
    Store<int8_t>(value, data_ptr_cast(&bytes));
    Store<uint8_t>(bytes, dataptr);
    dataptr[0] = FlipSign(dataptr[0]);
}

template <>
inline void Radix::EncodeData(data_ptr_t dataptr, int16_t value) {
    uint16_t bytes;
    Store<int16_t>(value, data_ptr_cast(&bytes));
    Store<uint16_t>(BSwap<uint16_t>(bytes), dataptr);
    dataptr[0] = FlipSign(dataptr[0]);
}

template <>
inline void Radix::EncodeData(data_ptr_t dataptr, int32_t value) {
    uint32_t bytes;
    Store<int32_t>(value, data_ptr_cast(&bytes));
    Store<uint32_t>(BSwap<uint32_t>(bytes), dataptr);
    dataptr[0] = FlipSign(dataptr[0]);
}

template <>
inline void Radix::EncodeData(data_ptr_t dataptr, int64_t value) {
    uint64_t bytes;
    Store<int64_t>(value, data_ptr_cast(&bytes));
    Store<uint64_t>(BSwap<uint64_t>(bytes), dataptr);
    dataptr[0] = FlipSign(dataptr[0]);
}

template <>
inline void Radix::EncodeData(data_ptr_t dataptr, uint8_t value) {
    Store<uint8_t>(value, dataptr);
}

template <>
inline void Radix::EncodeData(data_ptr_t dataptr, uint16_t value) {
    Store<uint16_t>(BSwap<uint16_t>(value), dataptr);
}

template <>
inline void Radix::EncodeData(data_ptr_t dataptr, uint32_t value) {
    Store<uint32_t>(BSwap<uint32_t>(value), dataptr);
}

template <>
inline void Radix::EncodeData(data_ptr_t dataptr, uint64_t value) {
    Store<uint64_t>(BSwap<uint64_t>(value), dataptr);
}

//docde
template <>
inline int8_t Radix::DecodeData(data_ptr_t dataptr) {
    uint8_t bytes = Load<uint8_t>(dataptr);
    bytes = FlipSign(bytes);
    return Load<int8_t>(data_ptr_cast(&bytes));
}

template <>
inline int16_t Radix::DecodeData(data_ptr_t dataptr) {
    uint16_t bytes = BSwap<uint16_t>(Load<uint16_t>(dataptr));
    dataptr[0] = FlipSign(dataptr[0]);
    return Load<int16_t>(data_ptr_cast(&bytes));
}

template <>
inline int32_t Radix::DecodeData(data_ptr_t dataptr) {
    uint32_t bytes = BSwap<uint32_t>(Load<uint32_t>(dataptr));
    dataptr[0] = FlipSign(dataptr[0]);
    return Load<int32_t>(data_ptr_cast(&bytes));
}

template <>
inline int64_t Radix::DecodeData(data_ptr_t dataptr) {
    uint64_t bytes = BSwap<uint64_t>(Load<uint64_t>(dataptr));
    dataptr[0] = FlipSign(dataptr[0]);
    return Load<int64_t>(data_ptr_cast(&bytes));
}

template <>
inline uint8_t Radix::DecodeData(data_ptr_t dataptr) {
    return Load<uint8_t>(dataptr);
}

template <>
inline uint16_t Radix::DecodeData(data_ptr_t dataptr) {
    return BSwap<uint16_t>(Load<uint16_t>(dataptr));
}

template <>
inline uint32_t Radix::DecodeData(data_ptr_t dataptr) {
    return BSwap<uint32_t>(Load<uint32_t>(dataptr));
}

template <>
inline uint64_t Radix::DecodeData(data_ptr_t dataptr) {
    return BSwap<uint64_t>(Load<uint64_t>(dataptr));
}

}  // namespace duckart
