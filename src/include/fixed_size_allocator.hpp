// https://arxiv.org/pdf/2210.16471
#pragma once

#include <memory>
#include <string>
#include <sstream>
#include <iostream>

#include "node.hpp"

namespace duckart {
class Node;

class FixedSizeAllocator {
public:
    FixedSizeAllocator(size_t elementSize, size_t blockCapacity = 256)
        : elementSize(elementSize),
          blockCapacity(blockCapacity),
          used(0),
          freeList(nullptr) {
        addBlock();
    }

    void* New() {
        if (freeList) {
            void* block = freeList;
            freeList = *reinterpret_cast<void**>(freeList);
            used++;
            return block;
        }
        if (used % blockCapacity == 0) {
            addBlock();
        }
        size_t blockIndex = used / blockCapacity;
        size_t indexInBlock = used % blockCapacity;
        used++;
        return &blocks[blockIndex][indexInBlock * elementSize];
    }

    void Free(void* ptr) {
        for (size_t i = 0; i < blocks.size(); ++i) {
            if (ptr >= blocks[i].get() && ptr < blocks[i].get() + blockCapacity * elementSize) {
                *reinterpret_cast<void**>(ptr) = freeList;
                freeList = ptr;
                used--;
                return;
            }
        }
    }
	
	   template <typename T>
    T* Get(const Node& ptr) const ;

    size_t GetUsed() const { return used; }
    size_t GetCapacity() const { return blocks.size() * blockCapacity; }

private:
    std::vector<std::unique_ptr<char[]>> blocks;
    size_t elementSize;
    size_t blockCapacity;
    size_t used;
    void* freeList;

    void addBlock() {
        blocks.push_back(std::make_unique<char[]>(elementSize * blockCapacity));
    }
};
}  // namespace duckart
