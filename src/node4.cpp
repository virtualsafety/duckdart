#include "node4.hpp"

#include <iostream>
#include <memory>
#include <vector>

#include "node16.hpp"

namespace duckart {

// NODE4
Node4 &Node4::New(ART &art, Node &node) {
    LOG_DEBUG("new Node4...");

    node = Node::GetAllocator(art, NType::NODE_4).New();
    D_ASSERT(node.getPointer() != nullptr);
    node.setTag(NType::NODE_4);  // set Node Type
    auto &n4 = Node::RefMutable<Node4>(art, node, NType::NODE_4);

    // prefix
    n4.prefix = Node{};   

    // child count
    n4.count = 0;

    // for test
    /*
    node.prefix[PREFIX_SIZE] = 5;
    uint8_t sourceArray[5] = {1, 2, 3, 4, 5};
    std::copy(std::begin(sourceArray), std::end(sourceArray),
    std::begin(node.prefix));
    */

    return n4;
}

void Node4::Free(ART &art, Node &node) {
    D_ASSERT(!node.IsCleared());
    auto &n4 = Node::RefMutable<Node4>(art, node, NType::NODE_4);

    // free all children
    for (idx_t i = 0; i < n4.count; i++) {
        Node::Free(art, n4.children[i]);
    }
}

void Node4::ReplaceChild(const uint8_t byte, const Node child) {
    for (idx_t i = 0; i < count; i++) {
        if (key[i] == byte) {
            children[i] = child;
            return;
        }
    }
}

void Node4::InsertChild(ART &art, Node &node, const uint8_t byte,
                        const Node child) {
    LOG_DEBUG("Node4::InsertChild ...");
    D_ASSERT(!node.IsCleared());
    auto &n4 = Node::RefMutable<Node4>(art, node, NType::NODE_4);

    // ensure that there is no other child at the same byte
    for (idx_t i = 0; i < n4.count; i++) {
        D_ASSERT(n4.key[i] != byte);
    }

    // insert new child node into node
    if (n4.count < NODE_4_CAPACITY) {
        // still space, just insert the child
        idx_t child_pos = 0;
        while (child_pos < n4.count && n4.key[child_pos] < byte) {
            child_pos++;
        }
        // move children backwards to make space
        for (idx_t i = n4.count; i > child_pos; i--) {
            n4.key[i] = n4.key[i - 1];
            n4.children[i] = n4.children[i - 1];
        }

        n4.key[child_pos] = byte;
        n4.children[child_pos] = child;
        n4.count++;

    } else {
        // node is full, grow to Node16

        auto node4 = node;
        Node16::GrowNode4(art, node, node4);
        Node16::InsertChild(art, node, byte, child);
    }
}

void Node4::DeleteChild(ART &art, Node &node, const uint8_t byte) {
    D_ASSERT(!node.IsCleared());
    auto &n4 = Node::RefMutable<Node4>(art, node, NType::NODE_4);

    idx_t child_pos = 0;
    for (; child_pos < n4.count; child_pos++) {
        if (n4.key[child_pos] == byte) {
            break;
        }
    }

    D_ASSERT(child_pos < n4.count); 
    D_ASSERT(n4.count > 1);

    // free the child and decrease the count
    Node::Free(art, n4.children[child_pos]);
    n4.count--;

    // potentially move any children backwards
    for (idx_t i = child_pos; i < n4.count; i++) {
        n4.key[i] = n4.key[i + 1];
        n4.children[i] = n4.children[i + 1];
    }

    // this is a one way node, compress
	if (n4.count == 1) {

		// we need to keep track of the old node pointer
		// because Concatenate() might overwrite that pointer while appending bytes to
		// the prefix (and by doing so overwriting the subsequent node with
		// new prefix nodes)
		auto old_n4_node = node;

		// get only child and concatenate prefixes
		auto child = n4.GetChild(n4.key[0]);
        auto child_prefix = child.GetPrefix(art);
        auto n4_prefix = n4.prefix;
		Prefix::Concatenate(art, n4_prefix, n4.key[0], child_prefix);
        child.SetPrefix(art,n4_prefix);
        
        Node::Swap(node, child);

		n4.count--;
		Node::Free(art, old_n4_node);
	}
}

Node4 &Node4::ShrinkNode16(ART &art, Node &node4, Node &node16) {
    auto &n4 = New(art, node4);
    auto &n16 = Node::RefMutable<Node16>(art, node16, NType::NODE_16);

    D_ASSERT(n16.count <= NODE_4_CAPACITY);
    n4.count = n16.count;
    n4.prefix = n16.prefix;
    for (idx_t i = 0; i < n16.count; i++) {
        n4.key[i] = n16.key[i];
        n4.children[i] = n16.children[i];
    }

    n16.count = 0;
    Node::Free(art, node16);
    return n4;
}

const Node Node4::GetChild(const uint8_t byte) const {
    for (idx_t i = 0; i < count; i++) {
        if (key[i] == byte) {
            D_ASSERT(!children[i].IsCleared());
            return children[i];
        }
    }
    return Node{};
}

std::string Node4::ToString(ART &art) const {
    std::stringstream ss;

    ss << "{node_type:Node4, prefix:" << Prefix::ToString(art, prefix)
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
