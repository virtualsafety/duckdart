#include "art.hpp"

#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "common.hpp"
#include "leaf.hpp"
#include "logger.hpp"
#include "node.hpp"
#include "node16.hpp"
#include "node256.hpp"
#include "node4.hpp"
#include "node48.hpp"
#include "prefix.hpp"
#include "string_type.hpp"

namespace duckart {

// ART
ART::ART() : root(std::make_unique<Node>()) {
    allocators.push_back(
        std::make_unique<FixedSizeAllocator>(sizeof(Leaf), 512));
    allocators.push_back(
        std::make_unique<FixedSizeAllocator>(sizeof(Node4), 512));
    allocators.push_back(
        std::make_unique<FixedSizeAllocator>(sizeof(Node16), 512));
    allocators.push_back(
        std::make_unique<FixedSizeAllocator>(sizeof(Node48), 512));
    allocators.push_back(
        std::make_unique<FixedSizeAllocator>(sizeof(Node256), 512));
    allocators.push_back(
        std::make_unique<FixedSizeAllocator>(sizeof(Prefix), 512));
}

// https://github.com/armon/libart/blob/master/src/art.c#L549
// v1.0
// https://github.com/duckdb/duckdb/blob/v1.0.0/src/execution/index/art/art.cpp#L567
// newest
// https://github.com/duckdb/duckdb/blob/main/src/execution/index/art/art.cpp#L607
bool ART::Insert(Node& node, const ARTKey& key, const Node& leaf, idx_t depth) {
    // node is currently empty, create a leaf here with the key
    if (node.getTag() == NType::NODE_DUMMY) {
        LOG_DEBUG("node is currently empty...");
        D_ASSERT(depth <= key.len);

        auto& leaf_node =
            Node::RefMutable<Leaf>(*this, leaf, NType::LEAF);

        // copy key to prefix of Leaf
        Node prefix;
        reference<Node> ref_prefix(prefix);
        Prefix::New(*this, ref_prefix, key, depth, key.len - depth);
        leaf_node.prefix = prefix;

        node = leaf;
        return true;
    }

    auto node_type = node.getTag();

    // If at a leaf
    if (node_type == NType::LEAF) {
        auto& leaf_node =
            Node::RefMutable<Leaf>(*this, node, NType::LEAF);
        auto& leaf_node_new =
            Node::RefMutable<Leaf>(*this, leaf, NType::LEAF);

        // record first Perfix of perfix chain
        reference<Node> l_first = leaf_node.prefix;

        // find out mismatch position
        reference<Node> l_prefix = leaf_node.prefix;
        auto mis_match_pos =
            Prefix::TraverseMutable(*this, l_prefix, key, depth);

        // if match
        if (mis_match_pos == INVALID_INDEX) {
            LOG_DEBUG("leaf is match...");

            // update value
            leaf_node.value = leaf_node_new.value;
            return true;
        }

        // if not match , we must split the leaf into a node4
        if (mis_match_pos != INVALID_INDEX) {
            LOG_DEBUG("leaf is not match...");
            Node node4;
            reference<Node> ref_node4(node4);
            Node4::New(*this, ref_node4);
            auto& n4 = Node::RefMutable<Node4>(*this, ref_node4, NType::NODE_4);

            //(1) add first Leaf to Node4
            Node l_child;
            std::cout << " mis_match_pos: " << mis_match_pos << std::endl;
            auto l_prefix_byte =
                Prefix::GetByte(*this, l_prefix, mis_match_pos);
            Prefix::Split(*this, l_prefix, l_child, mis_match_pos);

            std::cout << "l_prefix_byte: " << static_cast<char>(l_prefix_byte)
                      << std::endl;

            Node4::InsertChild(*this, node4, l_prefix_byte, node);
            // set first half  of Prefix chain  to prefix of Node4
            n4.prefix = l_first;
            // set second half Prefix chain to leaf
            // leaf_node.prefix = l_child;
            LOG_DEBUG("for debug 0: " + l_child.AddrToString());
            node.SetPrefix(*this, l_child);

            LOG_DEBUG("for debug 1: " + node4.GetPrefix(*this).AddrToString());
            Prefix::Print(*this, n4.prefix);

            LOG_DEBUG("for debug 2: " + node.GetPrefix(*this).AddrToString());
            Prefix::Print(*this, leaf_node.prefix);

            //(2)add second Leaf to Node4
            auto r_prefix_byte = key[depth];
            std::cout << "r_prefix_byte : " << static_cast<char>(r_prefix_byte)
                      << std::endl;

            Node r_prefix;
            reference<Node> ref_prefix(r_prefix);
            Prefix::New(*this, ref_prefix, key, depth + 1, key.len - depth - 1);
            leaf_node_new.prefix = r_prefix;
            Node4::InsertChild(*this, node4, r_prefix_byte, leaf);
            Prefix::Print(*this, leaf_node_new.prefix);
            LOG_DEBUG("for debug 3: " + r_prefix.AddrToString());

            // swap pointer
            Node::Swap(node, node4);

            std::cout << n4.ToString(*this) << std::endl;

            return true;
        }
    }

    // if at a Node
    switch (node_type) {
        case NType::NODE_4:
        case NType::NODE_16:
        case NType::NODE_48:
        case NType::NODE_256:
            return InsertIntoNode(node, key, leaf, depth);
        default:
            throw InternalException("Invalid node type for Insert.");
    }

    return false;
}
bool ART::InsertIntoNode(Node& node, const ARTKey& key, const Node& leaf,
                         idx_t depth) {
    LOG_DEBUG("Insert Into Node...");
    D_ASSERT(depth < key.len);

    // get Prefix chain of Node
    Node p_node;
    reference<Node> ref_prefix_node(p_node);
    p_node = node.GetPrefix(*this);

    // record first Perfix of perfix chain
    reference<Node> l_first = ref_prefix_node;
    auto temp_depth = depth;

    // if node have prefix
    if (ref_prefix_node.get().getTag() != NType::NODE_DUMMY) {
        auto mis_match_pos =
            Prefix::TraverseMutable(*this, ref_prefix_node, key, depth);

        auto len = depth - temp_depth;
        auto& prefix =
            Node::RefMutable<Prefix>(*this, ref_prefix_node, NType::PREFIX);

        // if key contain prefix of Node
        if (mis_match_pos == INVALID_INDEX) {
            LOG_DEBUG("key contain prefix of Node ...");
            auto prefix_byte = key[depth];
            auto child = node.GetChild(*this, prefix_byte);
            // if child exists
            if (child.getTag() != NType::NODE_DUMMY) {
                // return Insert(child, key, leaf, depth + 1);
                auto isOK = Insert(child, key, leaf, depth + 1);
                if (isOK) {
                    node.ReplaceChild(*this, prefix_byte,
                                      child);  // add in v0.84
                }
                return isOK;
            } else {
                Node prefix_node;
                reference<Node> ref_prefix(prefix_node);
                Prefix::New(*this, ref_prefix, key, depth + 1,
                            key.len - depth - 1);

                auto& leaf_node =
                    Node::RefMutable<Leaf>(*this, leaf, NType::LEAF);
                leaf_node.prefix = ref_prefix;
                Node::InsertChild(*this, node, prefix_byte, leaf);
                return true;
            }

        } else {
            // should new a Node4
            LOG_DEBUG("part match or not match ...");
            Node node4;
            reference<Node> ref_node4(node4);
            Node4::New(*this, ref_node4);
            auto& n4 = Node::RefMutable<Node4>(*this, ref_node4, NType::NODE_4);

            // split prefix chain of old Node
            Node second_part;
            auto prefix_byte =
                Prefix::GetByte(*this, ref_prefix_node, mis_match_pos);
            Prefix::Split(*this, ref_prefix_node, second_part, mis_match_pos);

            // set  prefix of new Node4
            n4.prefix = l_first;
            node.SetPrefix(*this, second_part);

            // swap pointer between old Node and Node4
            Node::Swap(node, node4);

            // add old node to Node4
            Node4::InsertChild(*this, node, prefix_byte, node4);

            // add child
            auto& leaf_node_new =
                Node::RefMutable<Leaf>(*this, leaf, NType::LEAF);
            auto c_prefix_byte = key[depth];
            std::cout << "prefix byte of child : "
                      << static_cast<char>(c_prefix_byte) << std::endl;

            Node c_prefix;
            reference<Node> ref_c_prefix(c_prefix);
            Prefix::New(*this, ref_c_prefix, key, depth + 1,
                        key.len - depth - 1);
            leaf_node_new.prefix = ref_c_prefix;
            Node4::InsertChild(*this, node, c_prefix_byte, leaf);

            return true;
        }
    } else {
        LOG_DEBUG("node has no prefix ...");
        // if node has no prefix
        auto prefix_byte = key[depth];
        auto child = node.GetChild(*this, prefix_byte);
        // if child exists
        if (child.getTag() != NType::NODE_DUMMY) {
            auto isOK = Insert(child, key, leaf, depth + 1);
            if (isOK) {
                node.ReplaceChild(*this, prefix_byte, child);  // add in v0.84
            }
            return isOK;
        } else {
            // No child, insert new leaf
            Node prefix_node;
            reference<Node> ref_prefix(prefix_node);
            Prefix::New(*this, ref_prefix, key, depth + 1, key.len - depth - 1);

            auto& leaf_node =
                Node::RefMutable<Leaf>(*this, leaf, NType::LEAF);
            leaf_node.prefix = ref_prefix;

            LOG_DEBUG("for debug 4: " + prefix_node.AddrToString());

            Node::InsertChild(*this, node, prefix_byte, leaf);

            return true;
        }
    }

    return false;
}
Node ART::Search(Node& node, const ARTKey& key, idx_t depth) {
    if (node.getTag() == NType::NODE_DUMMY) {
        // Empty node, key not found
        return Node();
    }

    if (node.getTag() == NType::LEAF) {
        // At a leaf node, check if the key matches
        auto& leaf = Node::RefMutable<Leaf>(*this, node, NType::LEAF);

        // Check if the key matches the prefix
        reference<Node> prefix = leaf.prefix;
        auto mis_match_pos = Prefix::TraverseMutable(*this, prefix, key, depth);
        // if match
        if (mis_match_pos == INVALID_INDEX) {
            return node;
        }
        return Node();  // Key not found
    }

    // Check the node's prefix
    Node p_node;
    reference<Node> prefix_node(p_node);
    p_node = node.GetPrefix(*this);
    if (prefix_node.get().getTag() != NType::NODE_DUMMY) {
        auto mismatch_pos =
            Prefix::TraverseMutable(*this, prefix_node, key, depth);
        if (mismatch_pos != INVALID_INDEX) {
            // Prefix mismatch, key not found
            return Node();
        }
    }

    // If we've reached the end of the key, but we're not at a leaf, the key is
    // not found
    if (depth >= key.len) {
        return Node();
    }

    // Get the next child based on the current byte of the key
    uint8_t next_byte = key[depth];
    Node child = node.GetChild(*this, next_byte);

    if (child.getTag() == NType::NODE_DUMMY) {
        // No child for this byte, key not found
        return Node();
    }

    // Recursively search in the child node
    return Search(child, key, depth + 1);
}

bool ART::Delete(Node& node, const ARTKey& key, idx_t depth) {
    if (node.getTag() == NType::NODE_DUMMY) {
        // Empty node, key not found
        return false;
    }

    if (node.getTag() == NType::LEAF) {
        // At a leaf node, check if the key matches
        auto& leaf = Node::RefMutable<Leaf>(*this, node, NType::LEAF);

        reference<Node> prefix = leaf.prefix;      
        auto mis_match_pos = Prefix::TraverseMutable(*this, prefix, key, depth);       
        // if match
        if (mis_match_pos == INVALID_INDEX) {
            node = Node();
            return true;
        }

        return false;  // Key doesn't match
    }

    // Check the node's prefix
    Node p_node;
    reference<Node> prefix_node(p_node);
    p_node = node.GetPrefix(*this);
    if (prefix_node.get().getTag() != NType::NODE_DUMMY) {           
        auto mismatch_pos =    Prefix::TraverseMutable(*this, prefix_node, key, depth);        
        if (mismatch_pos != INVALID_INDEX) {
            // Prefix mismatch, key not found
            return false;
        }
    }

    // If we've reached the end of the key, but we're not at a leaf, the key is
    // not found
    if (depth >= key.len) {
        return false;
    }

    // Get the next child based on the current byte of the key
    uint8_t next_byte = key[depth];
    Node child = node.GetChild(*this, next_byte);

    if (child.getTag() == NType::NODE_DUMMY) {
        // No child for this byte, key not found
        return false;
    }

    // Recursively delete in the child node
    bool deleted = Delete(child, key, depth + 1);

    if (deleted) {
        // Remove the child if it's now empty
        if (child.getTag() == NType::NODE_DUMMY) {
            Node::DeleteChild(*this, node, next_byte);
        } else {
            // Update the child node if it has changed
            node.ReplaceChild(*this, next_byte, child);
        }         
    }

    return deleted;
}

}  // namespace duckart
