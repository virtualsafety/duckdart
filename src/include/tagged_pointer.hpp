#pragma once

#include "common.hpp"
#include <utility>
#include <string>
#include <sstream>

namespace duckart {

template <typename T = void>
class TaggedPointer {
   private:
    uintptr_t ptr;
    static constexpr uintptr_t TAG_MASK = 0x7;
    static constexpr uintptr_t PTR_MASK = ~TAG_MASK;

   public:
    TaggedPointer(T* rawPtr = nullptr, NType tag = NType::NODE_DUMMY) {
        ptr = reinterpret_cast<uintptr_t>(rawPtr);
        if (ptr & TAG_MASK) {
            throw std::runtime_error("Pointer is not aligned");
        }
        ptr |= static_cast<uintptr_t>(tag);
    }

    T* getPointer() const { return reinterpret_cast<T*>(ptr & PTR_MASK); }

    NType getTag() const { return static_cast<NType>(ptr & TAG_MASK); }

    void setTag(NType tag) {
        ptr = (ptr & PTR_MASK) | static_cast<uintptr_t>(tag);
    }

    void Clear() {
        ptr = 0;  // This sets both the pointer and tag to 0
    }

    bool IsCleared() const { return ptr == 0; }

    void Reset(T* rawPtr = nullptr, NType tag = NType::NODE_DUMMY) {
        *this = TaggedPointer(rawPtr, tag);
    }

      // Swap function
    static void Swap(TaggedPointer& lhs,TaggedPointer& rhs) noexcept {
        std::swap(lhs.ptr, rhs.ptr);
    }

    // ToString function to get string representation of the pointer and tag
    std::string AddrToString() const {
        std::ostringstream oss;
        oss << "Pointer: {" << getPointer()
            << ", Tag: " << static_cast<uintptr_t>(getTag()) <<"}";
        return oss.str();
    }
};
}  // namespace duckart
