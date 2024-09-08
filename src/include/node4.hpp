#pragma once

#include <sstream>
#include <string>

#include "art.hpp"
#include "common.hpp"
#include "fixed_size_allocator.hpp"
#include "node.hpp"
#include "prefix.hpp"
#include "logger.hpp"

namespace duckart {
class Node; 

//! Node4 holds up to four Node children sorted by their key byte
class Node4 {
   public:
    //! Delete copy constructors, as any Node4 can never own its memory
    Node4(const Node4 &) = delete;
    Node4 &operator=(const Node4 &) = delete;

    //!prefix
    Node prefix;
     //! Number of non-null children
    uint8_t count;
    //! Array containing all partial key bytes
    uint8_t key[NODE_4_CAPACITY];
    //! Node pointers to the child nodes
    Node children[NODE_4_CAPACITY];

   public:
    //! Get a new Node4, might cause a new buffer allocation, and initialize it
    static Node4& New(ART& art, Node& node);
    //! Free the node (and its subtree)
    static void Free(ART &art, Node &node);

	//! Initializes all fields of the node while shrinking a Node16 to a Node4
	static Node4 &ShrinkNode16(ART &art, Node &node4, Node &node16);

    //! Insert a child node at byte
	static void InsertChild(ART &art, Node &node, const uint8_t byte, const Node child);
	//! Delete the child node at byte
	static void DeleteChild(ART &art, Node &node, const uint8_t byte);

	//! Replace the child node at byte
	void ReplaceChild(const uint8_t byte, const Node child);

	//! Get the (immutable) child for the respective byte in the node
	const Node GetChild(const uint8_t byte) const;

    //! Returns the string representation of the node
    std::string ToString(ART &art) const;
};

}  // namespace duckart
