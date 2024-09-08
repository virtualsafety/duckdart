#include "leaf.hpp"

#include "string_type.hpp"

namespace duckart {
// Leaf
Leaf& Leaf::New(ART& art, Node& node, const Value& value) {    
    node = Node::GetAllocator(art, NType::LEAF).New();      
    node.setTag(NType::LEAF);  // set Node Type
    auto& lnode = Node::RefMutable<Leaf>(art, node, NType::LEAF);
    lnode.value = value;   
    return lnode;
}


void Leaf::Free(ART& art, Node& node) {
    Node::GetAllocator(art, NType::LEAF).Free(node.getPointer());
    node.Clear();
}


}  // namespace duckart
