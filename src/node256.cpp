#include "node256.hpp"
#include "node48.hpp"
#include <cstring>
#include "logger.hpp"


namespace duckart {

Node256 &Node256::New(ART &art, Node &node) {    
    LOG_DEBUG("new Node256..." );

    node = Node::GetAllocator(art, NType::NODE_256).New();
    D_ASSERT(node.getPointer()!=nullptr);
    node.setTag(NType::NODE_256);
    auto &n256 = Node::RefMutable<Node256>(art, node, NType::NODE_256);

    n256.count = 0;
    for (idx_t i = 0; i < NODE_256_CAPACITY; i++) {
        n256.children[i].Clear();
    }

    return n256;
}

void Node256::Free(ART &art, Node &node) {
    D_ASSERT(!node.IsCleared());
    auto &n256 = Node::RefMutable<Node256>(art, node, NType::NODE_256);

    if (!n256.count) {
        return;
    }

    // free all children
    for (idx_t i = 0; i < NODE_256_CAPACITY; i++) {
        if (!n256.children[i].IsCleared()) {
            Node::Free(art, n256.children[i]);
        }
    }
}

Node256 &Node256::GrowNode48(ART &art, Node &node256, Node &node48) {
    auto &n48 = Node::RefMutable<Node48>(art, node48, NType::NODE_48);
    auto &n256 = New(art, node256);

    //copy perfix
    //std::memcpy(node256.prefix,node48.prefix,PREFIX_SIZE+1);
    n256.prefix = n48.prefix;

    // copy child
    n256.count = n48.count;
    for (idx_t i = 0; i < NODE_256_CAPACITY; i++) {
        if (n48.child_index[i] != EMPTY_MARKER) {
            n256.children[i] = n48.children[n48.child_index[i]];
        } else {
            n256.children[i].Clear();
        }
    }

    n48.count = 0;
    
    LOG_DEBUG("GrowNode48 ...");

    Node::Free(art, node48);
    return n256;
}

void Node256::InsertChild(ART &art, Node &node, const uint8_t byte,
                          const Node child) {
    D_ASSERT(!node.IsCleared());
    auto &n256 = Node::RefMutable<Node256>(art, node, NType::NODE_256);

    // ensure that there is no other child at the same byte
    D_ASSERT(n256.children[byte].IsCleared());

    n256.count++;
    D_ASSERT(n256.count <= NODE_256_CAPACITY);
    n256.children[byte] = child;
}

void Node256::DeleteChild(ART &art, Node &node, const uint8_t byte) {    
    LOG_DEBUG("DeleteChild ...." );

    D_ASSERT(!node.IsCleared());
    auto &n256 = Node::RefMutable<Node256>(art, node, NType::NODE_256);
   
    // free the child and decrease the count
    Node::Free(art, n256.children[byte]);
    n256.count--;  


    // shrink node to Node48
    if (n256.count <= NODE_256_SHRINK_THRESHOLD) {
        auto node256 = node;
        Node48::ShrinkNode256(art, node, node256);
    }
   
}

const Node Node256::GetChild(const uint8_t byte) const {
    if (!children[byte].IsCleared()) {
        return  children[byte];
    }
    return Node{};
}

std::string Node256::ToString(ART &art) const {
    std::stringstream ss;

    ss << "{node_type:Node256, prefix:" << Prefix::ToString(art, prefix)
       << ", size:" << static_cast<int>(count) << ", child:[";

    bool first = true;
    for (int i = 0; i < NODE_256_CAPACITY; ++i) {
        if (!children[i].IsCleared()) {
            if (!first) {
                ss << ", ";
            }
            ss << "{child:" << i 
               << ", byte:" << static_cast<char>(i)
               << ", node_type:" << static_cast<int>(children[i].getTag())
               << ", prefix:" << Prefix::ToString(art, children[i].GetPrefix(art))
               << "}";
            first = false;
        }
    }

    ss << "]}";
    return ss.str();
}
}  // namespace duckart
