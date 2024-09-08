// https://github.com/duckdb/duckdb/blob/main/src/include/duckdb/common/types/string_type.hpp
#pragma once

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>

struct string_t {
    union {
        struct {
            uint32_t length;
            char prefix[4];
            char* ptr;
        } pointer;
        struct {
            uint32_t length;
            char inlined[12];
        } inlined;
    } value;

    // Constructor
    string_t(const char* str) {
        size_t len = strlen(str);
        if (len <= 11) {
            value.inlined.length = len;
            strncpy(value.inlined.inlined, str, 12);
        } else {
            value.pointer.length = len;
            strncpy(value.pointer.prefix, str, 4);
            value.pointer.ptr = new char[len + 1];
            strcpy(value.pointer.ptr, str);
        }
    }

    // Copy constructor
    string_t(const string_t& other) {
        if (other.IsInlined()) {
            memcpy(&value, &other.value, sizeof(value));
        } else {
            value.pointer.length = other.value.pointer.length;
            memcpy(value.pointer.prefix, other.value.pointer.prefix, 4);
            value.pointer.ptr = new char[value.pointer.length + 1];
            strcpy(value.pointer.ptr, other.value.pointer.ptr);
        }
    }

    // Destructor
    ~string_t() {
        if (!IsInlined()) {
            delete[] value.pointer.ptr;
        }
    }

    // Check if string is inlined
    bool IsInlined() const { return value.inlined.length <= 11; }

    // Get string length
    uint32_t GetSize() const { return value.inlined.length; }

    // Get string content
    const char* GetData() const {
        return IsInlined() ? value.inlined.inlined : value.pointer.ptr;
    }


    // Display string info
    void display() const {
        std::cout << "String: " << GetData() << std::endl;
        std::cout << "Length: " << GetSize() << std::endl;
        std::cout << "Storage: " << (IsInlined() ? "Inlined" : "Pointer")
                  << std::endl;
        if (!IsInlined()) {
            std::cout << "Prefix: " << std::string(value.pointer.prefix, 4)
                      << std::endl;
        }
        std::cout << std::endl;
    }

    // Static GreaterThan comparison
    static bool GreaterThan(const string_t& a, const string_t& b) {
        const char* a_str = a.GetData();
        const char* b_str = b.GetData();

        // Compare up to the length of the shorter string
        size_t min_len = std::min(a.GetSize(), b.GetSize());
        int cmp = strncmp(a_str, b_str, min_len);

        if (cmp != 0) {
            // If the compared portions are different, return the result
            return cmp > 0;
        } else {
            // If the compared portions are the same, the longer string is
            // greater
            return a.GetSize() > b.GetSize();
        }
    }

    char* GetPointer() const {
        assert(!IsInlined());
        return value.pointer.ptr;
    }

    void SetPointer(char* new_ptr) {
        assert(!IsInlined());
        value.pointer.ptr = new_ptr;
    }
};
