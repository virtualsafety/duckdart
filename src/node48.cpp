#include "node48.hpp"
#include "node16.hpp"
#include "node256.hpp"
#include <cstring>
#include "logger.hpp"


namespace duckart {

Node48 &Node48::New(ART &art, Node &node) {
    LOG_DEBUG("new Node48..." );

    node = Node::GetAllocator(art, NType::NODE_48).New();
    D_ASSERT(node.getPointer()!=nullptr);
    node.setTag(NType::NODE_48);
    auto &n48 = Node::RefMutable<Node48>(art, node, NType::NODE_48);

    n48.count = 0;
    for (idx_t i = 0; i < NODE_256_CAPACITY; i++) {
        n48.child_index[i] = EMPTY_MARKER;
    }
    for (idx_t i = 0; i < NODE_48_CAPACITY; i++) {
        n48.children[i].Clear();
    }

    return n48;
}

void Node48::Free(ART &art, Node &node) {
    D_ASSERT(!node.IsCleared());
    auto &n48 = Node::RefMutable<Node48>(art, node, NType::NODE_48);

    if (!n48.count) {
        return;
    }

    // free all children
    for (idx_t i = 0; i < NODE_256_CAPACITY; i++) {
        if (n48.child_index[i] != EMPTY_MARKER) {
            Node::Free(art, n48.children[n48.child_index[i]]);
        }
    }
}

Node48 &Node48::GrowNode16(ART &art, Node &node48, Node &node16) {
    auto &n16 = Node::RefMutable<Node16>(art, node16, NType::NODE_16);
    auto &n48 = New(art, node48);

    // copy perfix
    //std::memcpy(node48.prefix,node16.prefix,PREFIX_SIZE+1);
    n48.prefix = n16.prefix;

    // copy child
    n48.count = n16.count;
    for (idx_t i = 0; i < NODE_256_CAPACITY; i++) {
        n48.child_index[i] = EMPTY_MARKER;
    }

    for (idx_t i = 0; i < n16.count; i++) {
        n48.child_index[n16.key[i]] = static_cast<uint8_t>(i);
        n48.children[i] = n16.children[i];
    }

    // necessary for faster child insertion/deletion
    for (idx_t i = n16.count; i < NODE_48_CAPACITY; i++) {
        n48.children[i].Clear();
    }

    n16.count = 0;
    Node::Free(art, node16);
    return n48;
}

Node48 &Node48::ShrinkNode256(ART &art, Node &node48, Node &node256) {
    auto &n48 = New(art, node48);
    auto &n256 = Node::RefMutable<Node256>(art, node256, NType::NODE_256);

    //copy perfix
    //std::memcpy(node48.prefix,node256.prefix,PREFIX_SIZE+1);
    n48.prefix = n256.prefix;

    //copy child
    n48.count = 0;
    for (idx_t i = 0; i < NODE_256_CAPACITY; i++) {
        D_ASSERT(n48.count <= NODE_48_CAPACITY);
        if (!n256.children[i].IsCleared()) {
            n48.child_index[i] = n48.count;
            n48.children[n48.count] = n256.children[i];
            n48.count++;
        } else {
            n48.child_index[i] = EMPTY_MARKER;
        }
    }

    // necessary for faster child insertion/deletion
    for (idx_t i = n48.count; i < NODE_48_CAPACITY; i++) {
        n48.children[i].Clear();
    }

    n256.count = 0;
    Node::Free(art, node256);
    return n48;
}

void Node48::InsertChild(ART &art, Node &node, const uint8_t byte,
                         const Node child) {
    D_ASSERT(!node.IsCleared());
    auto &n48 = Node::RefMutable<Node48>(art, node, NType::NODE_48);

    // ensure that there is no other child at the same byte
    D_ASSERT(n48.child_index[byte] == EMPTY_MARKER);

    // insert new child node into node
    if (n48.count < NODE_48_CAPACITY) {
        // still space, just insert the child
        idx_t child_pos = n48.count;
        if (!n48.children[child_pos].IsCleared()) {
            // find an empty position in the node list if the current position
            // is occupied
            child_pos = 0;
            while (!n48.children[child_pos].IsCleared()) {
                child_pos++;
            }
        }
        n48.children[child_pos] = child;
        n48.child_index[byte] = static_cast<uint8_t>(child_pos);
        n48.count++;

    } else {
        // node is full, grow to Node256
        auto node48 = node;
        Node256::GrowNode48(art, node, node48);
        Node256::InsertChild(art, node, byte, child);
    }
}

void Node48::DeleteChild(ART &art, Node &node, const uint8_t byte) {
    D_ASSERT(!node.IsCleared());
    auto &n48 = Node::RefMutable<Node48>(art, node, NType::NODE_48);

    // free the child and decrease the count
    Node::Free(art, n48.children[n48.child_index[byte]]);
    n48.child_index[byte] = EMPTY_MARKER;
    n48.count--;

    // shrink node to Node16
    if (n48.count < NODE_48_SHRINK_THRESHOLD) {
        auto node48 = node;
        Node16::ShrinkNode48(art, node, node48);
    }
}

const Node Node48::GetChild(const uint8_t byte) const {
    if (child_index[byte] != EMPTY_MARKER) {
        D_ASSERT(!children[child_index[byte]].IsCleared());
        return children[child_index[byte]];
    }
    return Node{};
}

std::string Node48::ToString(ART &art) const {
    std::stringstream ss;

    ss << "{node_type:Node48, prefix:" << Prefix::ToString(art, prefix)
       << ", size:" << static_cast<int>(count) << ", child:[";

    bool first = true;
    for (int i = 0; i < NODE_256_CAPACITY; ++i) {
        if (child_index[i] != EMPTY_MARKER) {
            if (!first) {
                ss << ", ";
            }
            const Node& child = children[child_index[i]];
            ss << "{child:" << static_cast<int>(child_index[i]) 
               << ", byte:" << static_cast<char>(i)
               << ", node_type:" << static_cast<int>(child.getTag())
               << ", prefix:" << Prefix::ToString(art, child.GetPrefix(art))
               << "}";
            first = false;
        }
    }

    ss << "]}";
    return ss.str();
}
}  // namespace duckart
