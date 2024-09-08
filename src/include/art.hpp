#pragma once

#include <memory>
#include <vector>

#include "common.hpp"
#include "fixed_size_allocator.hpp"
#include "node.hpp"
#include "artkey.hpp"


namespace duckart {

class Node;
class FixedSizeAllocator;

class ART {
   public:
    std::unique_ptr<Node> root;
    std::vector<std::unique_ptr<FixedSizeAllocator>> allocators;   

    ART();

    FixedSizeAllocator& GetAllocator(NType type) const {
        auto index = static_cast<size_t>(type) - 1;
        return *allocators[index];
    }

   bool Insert(Node &node, const ARTKey &key, const Node &leaf, idx_t depth);
   bool InsertIntoNode(Node& node, const ARTKey& key, const Node& leaf, idx_t depth);
    
   bool Delete(Node &node, const ARTKey &key, idx_t depth);
    
   Node Search(Node &node,const ARTKey &key, idx_t depth); 
};

}  // namespace duckart
