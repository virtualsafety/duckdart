#include "node16.hpp"
#include "node4.hpp"
#include "node48.hpp"
#include <cstring>
#include <iostream>

namespace duckart {

Node16 &Node16::New(ART &art, Node &node) {    
    LOG_DEBUG("new Node16..." );
	node = Node::GetAllocator(art, NType::NODE_16).New();
    D_ASSERT(node.getPointer()!=nullptr);
	node.setTag(NType::NODE_16);
	auto &n16 = Node::RefMutable<Node16>(art, node, NType::NODE_16);

	n16.count = 0;
	return n16;
}

void Node16::Free(ART &art, Node &node) {

	D_ASSERT(!node.IsCleared());
	auto &n16 = Node::RefMutable<Node16>(art, node, NType::NODE_16);

	// free all children
	for (idx_t i = 0; i < n16.count; i++) {
		Node::Free(art, n16.children[i]);
	}
}


Node16 &Node16::GrowNode4(ART &art, Node &node16, Node &node4) {
    auto &n4 = Node::RefMutable<Node4>(art, node4, NType::NODE_4);
    auto &n16 = New(art, node16);

    //copy perfix
    //std::memcpy(node16.prefix,node4.prefix,PREFIX_SIZE+1);
    n16.prefix = n4.prefix;

    // copy child
    n16.count = n4.count;
    for (idx_t i = 0; i < n4.count; i++) {
        n16.key[i] = n4.key[i];
        n16.children[i] = n4.children[i];
    }

    n4.count = 0;
    Node::Free(art, node4);
    return n16;
}

Node16 &Node16::ShrinkNode48(ART &art, Node &node16, Node &node48) {
    auto &n16 = New(art, node16);
    auto &n48 = Node::RefMutable<Node48>(art, node48, NType::NODE_48);

    //copy perfix
    //std::memcpy(node16.prefix,node48.prefix,PREFIX_SIZE+1);
    n16.prefix = n48.prefix;

    // copy child
    n16.count = 0;
    for (idx_t i = 0; i < NODE_256_CAPACITY; i++) {
        D_ASSERT(n16.count <= NODE_16_CAPACITY);
        if (n48.child_index[i] != EMPTY_MARKER) {
            n16.key[n16.count] = static_cast<uint8_t>(i);
            n16.children[n16.count] = n48.children[n48.child_index[i]];
            n16.count++;
        }
    }

    n48.count = 0;
    Node::Free(art, node48);
    return n16;
}

void Node16::InsertChild(ART &art, Node &node, const uint8_t byte,
                         const Node child) {
    D_ASSERT(!node.IsCleared());
    auto &n16 = Node::RefMutable<Node16>(art, node, NType::NODE_16);

    // ensure that there is no other child at the same byte
    for (idx_t i = 0; i < n16.count; i++) {
        D_ASSERT(n16.key[i] != byte);
    }

    // insert new child node into node
    if (n16.count < NODE_16_CAPACITY) {
        // still space, just insert the child
        idx_t child_pos = 0;
        while (child_pos < n16.count && n16.key[child_pos] < byte) {
            child_pos++;
        }
        // move children backwards to make space
        for (idx_t i = n16.count; i > child_pos; i--) {
            n16.key[i] = n16.key[i - 1];
            n16.children[i] = n16.children[i - 1];
        }

        n16.key[child_pos] = byte;
        n16.children[child_pos] = child;
        n16.count++;

    } else {
        // node is full, grow to Node48
        auto node16 = node;
        Node48::GrowNode16(art, node, node16);
        Node48::InsertChild(art, node, byte, child);
    }
}

void Node16::DeleteChild(ART &art, Node &node, const uint8_t byte) {
    D_ASSERT(!node.IsCleared());
    auto &n16 = Node::RefMutable<Node16>(art, node, NType::NODE_16);

    idx_t child_pos = 0;
    for (; child_pos < n16.count; child_pos++) {
        if (n16.key[child_pos] == byte) {
            break;
        }
    }

    D_ASSERT(child_pos < n16.count);

    // free the child and decrease the count
    Node::Free(art, n16.children[child_pos]);
    n16.count--;

    // potentially move any children backwards
    for (idx_t i = child_pos; i < n16.count; i++) {
        n16.key[i] = n16.key[i + 1];
        n16.children[i] = n16.children[i + 1];
    }

    // shrink node to Node4
    if (n16.count < NODE_4_CAPACITY) {
        auto node16 = node;
        Node4::ShrinkNode16(art, node, node16);
    }
}

void Node16::ReplaceChild(const uint8_t byte, const Node child) {
    for (idx_t i = 0; i < count; i++) {
        if (key[i] == byte) {
            children[i] = child;
            return;
        }
    }
}

const Node Node16::GetChild(const uint8_t byte) const {
    for (idx_t i = 0; i < count; i++) {
        if (key[i] == byte) {
            D_ASSERT(!children[i].IsCleared());
            return children[i];
        }
    }
    return Node{};
}

std::string Node16::ToString(ART &art) const {
    std::stringstream ss;

    ss << "{node_type:Node16, prefix:" << Prefix::ToString(art, prefix)
       << ", size:" << static_cast<int>(count) << ", child:[";

    bool first = true;
    for (int i = 0; i < count; ++i) {
        if (children[i].getTag() != NType::NODE_DUMMY) {
            if (!first) {
                ss << ", ";
            }
            ss << "{child:" << i << ", byte:" << static_cast<char>(key[i])
               <<",node_type:"
               << static_cast<int>(children[i].getTag())
               << ", prefix:"
               << Prefix::ToString(art, children[i].GetPrefix(art)) 
               //<<","<<children[i].GetPrefix(art).AddrToString()
               <<"}";
            first = false;
        }
    }

    ss << "]}";
    return ss.str();
}
}  // namespace duckart
