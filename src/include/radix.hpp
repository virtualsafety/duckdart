#pragma once

#include <limits.h>

#include <cfloat>
#include <cstring>  // strlen() on Solaris
 

#include "bswap.hpp"
#include "common.hpp"
#include "exception.hpp"
#include "string_type.hpp"
 

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

    private:
	template <class T>
	static void EncodeSigned(data_ptr_t dataptr, T value);
	template <class T>
	static T DecodeSigned(data_ptr_t input);
};

template <class T>
void Radix::EncodeSigned(data_ptr_t dataptr, T value) {
	using UNSIGNED = typename MakeUnsigned<T>::type;   
	UNSIGNED bytes;
	Store<T>(value, data_ptr_cast(&bytes));
	Store<UNSIGNED>(BSwap(bytes), dataptr);
	dataptr[0] = FlipSign(dataptr[0]);
}

template <>
inline void Radix::EncodeData(data_ptr_t dataptr, int8_t value) {
	EncodeSigned<int8_t>(dataptr, value);
}

template <>
inline void Radix::EncodeData(data_ptr_t dataptr, int16_t value) {
	EncodeSigned<int16_t>(dataptr, value);
}

template <>
inline void Radix::EncodeData(data_ptr_t dataptr, int32_t value) {
	EncodeSigned<int32_t>(dataptr, value);
}

template <>
inline void Radix::EncodeData(data_ptr_t dataptr, int64_t value) {
	EncodeSigned<int64_t>(dataptr, value);
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

// decode
template <class T>
T Radix::DecodeSigned(data_ptr_t input) {
	using UNSIGNED = typename MakeUnsigned<T>::type;   
	UNSIGNED bytes = Load<UNSIGNED>(input);
	auto bytes_data = data_ptr_cast(&bytes);
	bytes_data[0] = FlipSign(bytes_data[0]);
	T result;
	Store<UNSIGNED>(BSwap(bytes), data_ptr_cast(&result));
	return result;
}

template <>
inline int8_t Radix::DecodeData(data_ptr_t input) {
	return DecodeSigned<int8_t>(input);
}

template <>
inline int16_t Radix::DecodeData(data_ptr_t input) {
	return DecodeSigned<int16_t>(input);
}

template <>
inline int32_t Radix::DecodeData(data_ptr_t input) {
	return DecodeSigned<int32_t>(input);
}

template <>
inline int64_t Radix::DecodeData(data_ptr_t input) {
	return DecodeSigned<int64_t>(input);
}

template <>
inline uint8_t Radix::DecodeData(data_ptr_t input) {
	return Load<uint8_t>(input);
}

template <>
inline uint16_t Radix::DecodeData(data_ptr_t input) {
	return BSwap(Load<uint16_t>(input));
}

template <>
inline uint32_t Radix::DecodeData(data_ptr_t input) {
	return BSwap(Load<uint32_t>(input));
}

template <>
inline uint64_t Radix::DecodeData(data_ptr_t input) {
	return BSwap(Load<uint64_t>(input));
}
}  // namespace duckart
