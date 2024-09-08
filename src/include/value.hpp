#pragma once

#include <cstring> 
#include <iomanip>

#include "common.hpp"
#include "string_type.hpp"
#include "exception.hpp"
#include "radix.hpp"

namespace duckart {

class Value {
   public:
    Value() : len(0), data(nullptr) {}
    Value(const data_ptr_t &data, const uint32_t &len)
        : len(len), data(new data_t[len]) {
        std::memcpy(this->data, data, len);
    }
    Value(const uint32_t &len) : len(len), data(new data_t[len]) {}
    ~Value() { delete[] data; }

    // Copy constructor
    Value(const Value &other) : len(other.len), data(new data_t[other.len]) {
        std::memcpy(data, other.data, len);
    }

    // Move constructor
    Value(Value &&other) noexcept : len(other.len), data(other.data) {
        other.data = nullptr;
        other.len = 0;
    }

    // Copy assignment operator
    Value &operator=(const Value &other) {
        if (this != &other) {
            delete[] data;
            len = other.len;
            data = new data_t[len];
            std::memcpy(data, other.data, len);
        }
        return *this;
    }

    // Move assignment operator
    Value &operator=(Value &&other) noexcept {
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
    static inline Value CreateValue(T element) {
        auto data = Value::CreateData<T>(element);
        return Value(data, sizeof(element));
    }

    template <class T>
    static inline void CreateValue(Value &key, T element) {
        key.data = Value::CreateData<T>(element);
        key.len = sizeof(element);
    }

	void Print() const {
        std::cout << "Value: length = " << len << ", data = ";
        for (uint32_t i = 0; i < len; ++i) {
            std::cout << std::setw(2) << std::setfill('0') << std::hex << static_cast<int>(data[i]) << " ";
        }
        std::cout << std::dec << std::endl;
    }

   public:
    data_t &operator[](size_t i) { return data[i]; }
    const data_t &operator[](size_t i) const { return data[i]; }
    bool operator>(const Value &k) const;
    bool operator>=(const Value &k) const;
    bool operator==(const Value &k) const;
    
   private:
    template <class T>
    static inline data_ptr_t CreateData(T value) {
        auto data = new data_t[sizeof(value)];
        Radix::EncodeData<T>(data, value);
        return data;
    }
};

template <>
Value Value::CreateValue(string_t value);
template <>
Value Value::CreateValue(const char *value);
template <>
void Value::CreateValue( Value &key, string_t value);

}  // namespace duckart
