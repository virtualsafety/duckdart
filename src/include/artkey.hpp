#pragma once

#include <cstring> 
#include <iomanip>

#include "common.hpp"
#include "string_type.hpp"
#include "exception.hpp"
#include "radix.hpp"

namespace duckart {

class ARTKey {
   public:
    ARTKey() : len(0), data(nullptr) {}
    ARTKey(const data_ptr_t &data, const uint32_t &len)
        : len(len), data(new data_t[len]) {
        std::memcpy(this->data, data, len);
    }
    ARTKey(const uint32_t &len) : len(len), data(new data_t[len]) {}
    ~ARTKey() { delete[] data; }

    // Copy constructor
    ARTKey(const ARTKey &other) : len(other.len), data(new data_t[other.len]) {
        std::memcpy(data, other.data, len);
    }

    // Move constructor
    ARTKey(ARTKey &&other) noexcept : len(other.len), data(other.data) {
        other.data = nullptr;
        other.len = 0;
    }

    // Copy assignment operator
    ARTKey &operator=(const ARTKey &other) {
        if (this != &other) {
            delete[] data;
            len = other.len;
            data = new data_t[len];
            std::memcpy(data, other.data, len);
        }
        return *this;
    }

    // Move assignment operator
    ARTKey &operator=(ARTKey &&other) noexcept {
        if (this != &other) {
            delete[] data;
            len = other.len;
            data = other.data;
            other.data = nullptr;
            other.len = 0;
        }
        return *this;
    }

    uint32_t len;
    data_ptr_t data;

   public:
    template <class T>
    static inline ARTKey CreateARTKey(T element) {
        auto data = ARTKey::CreateData<T>(element);
        return ARTKey(data, sizeof(element));
    }

    template <class T>
    static inline void CreateARTKey(ARTKey &key, T element) {
        key.data = ARTKey::CreateData<T>(element);
        key.len = sizeof(element);
    }

	void Print() const {
        std::cout << "ARTKey: length = " << len << ", data = ";
        for (uint32_t i = 0; i < len; ++i) {
            std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(data[i]) << " ";
        }
        std::cout << std::dec << std::endl;
    }

   public:
    data_t &operator[](size_t i) { return data[i]; }
    const data_t &operator[](size_t i) const { return data[i]; }
    bool operator>(const ARTKey &k) const;
    bool operator>=(const ARTKey &k) const;
    bool operator==(const ARTKey &k) const;

    inline bool ByteMatches(const ARTKey &other, const uint32_t &depth) const {
        return data[depth] == other[depth];
    }
    inline bool Empty() const { return len == 0; }
    void ConcatenateARTKey(ARTKey &concat_key);

   private:
    template <class T>
    static inline data_ptr_t CreateData(T value) {
        auto data = new data_t[sizeof(value)];
        Radix::EncodeData<T>(data, value);
        return data;
    }
};

template <>
ARTKey ARTKey::CreateARTKey(string_t value);
template <>
ARTKey ARTKey::CreateARTKey(const char *value);
template <>
void ARTKey::CreateARTKey( ARTKey &key, string_t value);

}  // namespace duckart
