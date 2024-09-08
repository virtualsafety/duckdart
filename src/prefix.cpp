#include "prefix.hpp"

#include "art.hpp"
#include "artkey.hpp"
#include "logger.hpp"
#include "node.hpp"

namespace duckart {
Prefix &Prefix::New(ART &art, Node &node) {
    LOG_DEBUG("new Prefix...");
    node = Node::GetAllocator(art, NType::PREFIX).New();
    D_ASSERT(node.getPointer() != nullptr);
    node.setTag(NType::PREFIX);
    LOG_DEBUG("new perfix," + node.AddrToString());

    auto &prefix = Node::RefMutable<Prefix>(art, node, NType::PREFIX);

    prefix.data[PREFIX_SIZE] = 0;
    prefix.ptr = Node{};
    return prefix;
}

Prefix &Prefix::New(ART &art, Node &node, uint8_t byte, const Node &next) {
    LOG_DEBUG("new Prefix with one byte...");
    node = Node::GetAllocator(art, NType::PREFIX).New();
    node.setTag(NType::PREFIX);

    auto &prefix = Node::RefMutable<Prefix>(art, node, NType::PREFIX);
    prefix.data[PREFIX_SIZE] = 1;
    prefix.data[0] = byte;
    prefix.ptr = next;
    return prefix;
}

void Prefix::New(ART &art, reference<Node> &node, const ARTKey &key,
                 const uint32_t depth, uint32_t count) {
    LOG_DEBUG("new Prefix by a ARTKey ...");
    if (count == 0) {
        return;
    }
    idx_t copy_count = 0;

    // Save the reference to the first prefix node
    reference<Node> firstNode = node;

    while (count > 0) {
        node.get() = Node::GetAllocator(art, NType::PREFIX).New();
        D_ASSERT(node.get().getPointer() != nullptr);
        node.get().setTag(NType::PREFIX);
        LOG_DEBUG("new Prefix by a ARTKey," + node.get().AddrToString());
        auto &prefix = Node::RefMutable<Prefix>(art, node, NType::PREFIX);
        // set ptr to DUMMY Node
        prefix.ptr = Node{};

        auto this_count = MinValue((uint32_t)PREFIX_SIZE, count);
        prefix.data[PREFIX_SIZE] = (uint8_t)this_count;
        std::memcpy(prefix.data, key.data + depth + copy_count, this_count);

        copy_count += this_count;
        count -= this_count;

        // Move to the next node if there's more to process
        node = prefix.ptr;
    }

    // Update the reference to point to the first prefix node
    node = firstNode;
    LOG_DEBUG("new Prefix by a ARTKey end...");
}

void Prefix::Free(ART &art, Node &node) {
    LOG_DEBUG("free Prefix ...");
    Node current_node = node;
    Node next_node;
    while (!current_node.IsCleared() &&
           current_node.getTag() == NType::PREFIX) {
        LOG_DEBUG("free Prefix," + current_node.AddrToString());
        next_node =  Node::RefMutable<Prefix>(art, current_node, NType::PREFIX).ptr;
        Node::GetAllocator(art, NType::PREFIX).Free(current_node.getPointer());
        current_node = next_node;
    }

    //Node::Free(art, current_node);
    node.Clear();
}

// Function to print the Prefix chain
void Prefix::Print(const ART &art, const Node &node) {
    LOG_DEBUG("Print prefix...");

    auto current = node;
    std::cout << "Prefix chain: ";

    while (current.getTag() == NType::PREFIX) {
        auto &prefix = Node::RefMutable<Prefix>(art, current, NType::PREFIX);
        uint8_t count = prefix.data[PREFIX_SIZE];

        std::cout << "[ ";
        for (uint8_t i = 0; i < count; ++i) {
            std::cout << static_cast<char>(prefix.data[i]);
        }
        std::cout << " ] ";

        current = prefix.ptr;
    }
    std::cout << std::endl;
}


std::string Prefix::ToString(const ART &art, const Node &node) {
    std::stringstream ss;  

    auto current = node;
    while (current.getTag() == NType::PREFIX) {
        auto &prefix = Node::RefMutable<Prefix>(art, current, NType::PREFIX);
        uint8_t count = prefix.data[PREFIX_SIZE];

        ss << "[ ";
        for (uint8_t i = 0; i < count; ++i) {
            ss << static_cast<char>(prefix.data[i]);
        }
        ss << " ] ";

        current = prefix.ptr;
    }

    return ss.str();
}

// Function to check if two chains of Prefix match
bool Prefix::Match(ART &art, reference<Node> node1, reference<Node> node2) {
    while (node1.get().getTag() == NType::PREFIX &&
           node2.get().getTag() == NType::PREFIX) {
        auto &prefix1 = Node::RefMutable<Prefix>(art, node1, NType::PREFIX);
        auto &prefix2 = Node::RefMutable<Prefix>(art, node2, NType::PREFIX);

        if (prefix1.data[PREFIX_SIZE] != prefix2.data[PREFIX_SIZE]) {
            return false;
        }

        if (std::memcmp(prefix1.data, prefix2.data,
                        prefix1.data[PREFIX_SIZE]) != 0) {
            return false;
        }

        node1 = prefix1.ptr;
        node2 = prefix2.ptr;
    }

    return node1.get().getTag() == NType::NODE_DUMMY &&
           node2.get().getTag() == NType::NODE_DUMMY;
}

Prefix &Prefix::Append(ART &art, const uint8_t byte) {
    //LOG_DEBUG("Prefix Append with one byte ...");
    reference<Prefix> prefix(*this);

    // if this prefix node is full ,we need a new prefix node
    if (prefix.get().data[PREFIX_SIZE] == PREFIX_SIZE) {
        prefix = New(art, prefix.get().ptr);
    }

    prefix.get().data[prefix.get().data[PREFIX_SIZE]] = byte;
    prefix.get().data[PREFIX_SIZE]++;
    return prefix.get();
}

void Prefix::Append(ART &art, Node other_prefix) {
    LOG_DEBUG("Prefix Append...");
    D_ASSERT(!other_prefix.IsCleared());

    reference<Prefix> prefix(*this);
    while (other_prefix.getTag() == NType::PREFIX) {
        // copy prefix bytes
        auto &other =
            Node::RefMutable<Prefix>(art, other_prefix, NType::PREFIX);
        for (idx_t i = 0; i < other.data[PREFIX_SIZE]; i++) {
            prefix = prefix.get().Append(art, other.data[i]);          
        }

        D_ASSERT(!other.ptr.IsCleared());

        // prefix.get().ptr = other.ptr;
        auto temp_prefix = other.ptr;
        Node::GetAllocator(art, NType::PREFIX).Free(other_prefix.getPointer());
        other_prefix = temp_prefix;
    }

    D_ASSERT(prefix.get().ptr.getTag() != NType::PREFIX);
}

void Prefix::Split(ART &art, reference<Node> &prefix_node, Node &child_node,
                   idx_t position) {
    LOG_DEBUG("Prefix::Split,node type:" +
              std::to_string(static_cast<int>(prefix_node.get().getTag())));

    D_ASSERT(position < PREFIX_SIZE);
    D_ASSERT(!prefix_node.get().IsCleared());
    D_ASSERT(prefix_node.get().getTag() != NType::NODE_DUMMY);

    auto &prefix = Node::RefMutable<Prefix>(art, prefix_node, NType::PREFIX);

    // the split is at the last byte of this prefix, so the child_node contains
    // all subsequent prefix nodes (prefix.ptr) (if any), and the count of this
    // prefix decreases by one, then, set prefix.ptr to a DUMMY Node
    if (position + 1 == PREFIX_SIZE) {
        prefix.data[PREFIX_SIZE]--;
        child_node = prefix.ptr;
        prefix.ptr = Node{};
        return;
    }

    // Save the reference to the first prefix node
    reference<Node> firstNode = prefix_node;
    reference<Prefix> child_prefix = New(art, child_node);

    // append the remaining bytes after the split
    if (position + 1 < prefix.data[PREFIX_SIZE]) {       
        for (idx_t i = position + 1; i < prefix.data[PREFIX_SIZE]; i++) {
            child_prefix = child_prefix.get().Append(art, prefix.data[i]);
        }

        D_ASSERT(!prefix.ptr.IsCleared());

        if (prefix.ptr.getTag() == NType::PREFIX) {
            child_prefix.get().Append(art, prefix.ptr);
        } else {
            // this is the last prefix node of the prefix chain
            child_prefix.get().ptr = prefix.ptr;           
        }
    }

    // this is the last byte of the prefix 
    if (position + 1 == prefix.data[PREFIX_SIZE]) {
        child_node = prefix.ptr;
    }

    // set the new size of this node
    prefix.data[PREFIX_SIZE] = static_cast<uint8_t>(position);

    // if split at first byte ,free the prefix
    if (position == 0) {
        LOG_DEBUG("split at first byte,current:" + prefix_node.get().AddrToString() +",next:" + prefix.ptr.AddrToString() );
      
        //prefix.ptr.Clear();
        Node::Free(art, prefix_node.get());
        prefix_node.get() = Node{};
        return;
    }

    // bytes left before the split, reference subsequent node
    prefix.ptr = Node{};
    prefix_node = firstNode;
    return;
}

void Prefix::Reduce(ART &art, Node &prefix_node, const idx_t n) {
    D_ASSERT(!prefix_node.IsCleared());
    D_ASSERT(n < PREFIX_SIZE);

    reference<Prefix> prefix =
        Node::RefMutable<Prefix>(art, prefix_node, NType::PREFIX);

    // free this prefix node
    if (n == (idx_t)(prefix.get().data[PREFIX_SIZE] - 1)) {
        auto next_ptr = prefix.get().ptr;
        D_ASSERT(!next_ptr.IsCleared());
        prefix.get().ptr.Clear();
        Node::Free(art, prefix_node);
        prefix_node = next_ptr;
        return;
    }

    // shift by n bytes in the current prefix
    for (idx_t i = 0; i < PREFIX_SIZE - n - 1; i++) {
        prefix.get().data[i] = prefix.get().data[n + i + 1];
    }
    D_ASSERT(n < (idx_t)(prefix.get().data[PREFIX_SIZE] - 1));
    prefix.get().data[PREFIX_SIZE] -= n + 1;

    // append the remaining prefix bytes
    prefix.get().Append(art, prefix.get().ptr);
}

void Prefix::Concatenate(ART &art, Node &prefix_node, const uint8_t byte,
                         Node &child_prefix_node) {
    D_ASSERT(!prefix_node.IsCleared() && !child_prefix_node.IsCleared());

    // append a byte and a child_prefix to prefix
    if (prefix_node.getTag() == NType::PREFIX) {
        // get the tail
        reference<Prefix> prefix =
            Node::RefMutable<Prefix>(art, prefix_node, NType::PREFIX);
        D_ASSERT(!prefix.get().ptr.IsCleared());

        while (prefix.get().ptr.getTag() == NType::PREFIX) {
            prefix =
                Node::RefMutable<Prefix>(art, prefix.get().ptr, NType::PREFIX);
            D_ASSERT(!prefix.get().ptr.IsCleared());
        }

        // append the byte
        prefix = prefix.get().Append(art, byte);

        if (child_prefix_node.getTag() == NType::PREFIX) {
            // append the child prefix
            prefix.get().Append(art, child_prefix_node);
        } else {
            // set child_prefix_node to succeed prefix
            prefix.get().ptr = child_prefix_node;
        }
        return;
    }

    // create a new prefix node containing the byte, then append the
    // child_prefix to it
    if (prefix_node.getTag() != NType::PREFIX &&
        child_prefix_node.getTag() == NType::PREFIX) {
        auto child_prefix = child_prefix_node;
        auto &prefix = New(art, prefix_node, byte);
        prefix.Append(art, child_prefix);
        return;
    }

    // neither prefix nor child_prefix are prefix nodes
    // create a new prefix containing the byte
    New(art, prefix_node, byte, child_prefix_node);
}

// depth : the mismatch position of ARTKey
// prefix_node: the Prefix Node of Prefix chain where first mismatch
// Returns the position of the first mismatch within a prefix node if found
idx_t Prefix::TraverseMutable(ART &art, reference<Node> &prefix_node,
                              const ARTKey &key, idx_t &depth) {
    D_ASSERT(!prefix_node.get().IsCleared()); 
    D_ASSERT(prefix_node.get().getTag() == NType::PREFIX);

    // compare prefix nodes to key bytes
    while (prefix_node.get().getTag() == NType::PREFIX) {
        auto &prefix =
            Node::RefMutable<Prefix>(art, prefix_node, NType::PREFIX);
        for (idx_t i = 0; i < prefix.data[PREFIX_SIZE]; i++) {
            if (prefix.data[i] != key[depth]) {
                return i;
            }
            depth++;
        }
        prefix_node = prefix.ptr;
        D_ASSERT(!prefix_node.get().IsCleared());
    }

    return INVALID_INDEX;
}

//! differ two Prefixe Nodes to find (1) that they match (so far), or (2) that
//! they have a mismatching position, if match return true ,otherwise return false
bool Prefix::Mismatch(ART &art, reference<Node> &l_node,
                      reference<Node> &r_node, idx_t &mismatch_position) {
    auto &l_prefix = Node::RefMutable<Prefix>(art, l_node.get(), NType::PREFIX);
    auto &r_prefix = Node::RefMutable<Prefix>(art, r_node.get(), NType::PREFIX);

    // compare prefix bytes
    idx_t max_count =
        MinValue(l_prefix.data[PREFIX_SIZE], r_prefix.data[PREFIX_SIZE]);
    for (idx_t i = 0; i < max_count; i++) {
        if (l_prefix.data[i] != r_prefix.data[i]) {
            mismatch_position = i;
            break;
        }
    }

    if (mismatch_position == INVALID_INDEX) {
        // prefixes match (so far)
        if (l_prefix.data[PREFIX_SIZE] == r_prefix.data[PREFIX_SIZE]) {
            return true;  // both Prefix Node
        } else {
            mismatch_position = max_count;
            return false;
        }
    }

    return false;
}

 uint8_t Prefix::GetByte(const ART &art, const Node &prefix_node, const idx_t position) {
		auto &prefix = Node::Ref<const Prefix>(art, prefix_node, NType::PREFIX);
		D_ASSERT(position <  PREFIX_SIZE);
		D_ASSERT(position < prefix.data[PREFIX_SIZE]);
		return prefix.data[position];
	}

}  // namespace duckart
