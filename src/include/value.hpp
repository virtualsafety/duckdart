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
    static inline void CreateValue(Value &val, T element) {
        val.data = Value::CreateData<T>(element);
        val.len = sizeof(element);
    }

    template <class T>
    static inline T ExtractValue(Value &val) {
        auto result = Value::ExtractData<T>(val);
        return result;
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

    template <class T>
    static inline T ExtractData(data_ptr_t value) {        
        auto result =Radix::EncodeData<T>(value);
        return result;
    }
};

template <>
Value Value::CreateValue(string_t value);
template <>
Value Value::CreateValue(const char *value);
template <>
void Value::CreateValue( Value &val, string_t value);

//
template <>
string_t Value::ExtractValue(Value &val);

}  // namespace duckart
