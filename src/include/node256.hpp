#pragma once

#include <sstream>
#include <string>


#include "art.hpp"
#include "common.hpp"
#include "fixed_size_allocator.hpp"
#include "node.hpp"
#include "prefix.hpp"

namespace duckart {

//! Node256 holds up to 256 Node children which can be directly indexed by the
//! key byte
class Node256 {
   public:
    //! Delete copy constructors, as any Node256 can never own its memory
    Node256(const Node256 &) = delete;
    Node256 &operator=(const Node256 &) = delete;


    //! prefix
    Node prefix;
    //! Number of non-null children
    uint16_t count;
    //! Node pointers to the child nodes
    Node children[NODE_256_CAPACITY];

    public:
	//! Get a new Node256, might cause a new buffer allocation, and initialize it
	static Node256 &New(ART &art, Node &node);
	//! Free the node (and its subtree)
	static void Free(ART &art, Node &node);

    //! Initializes all the fields of the node while growing a Node48 to a Node256
	static Node256 &GrowNode48(ART &art, Node &node256, Node &node48);

	//! Insert a child node at byte
	static void InsertChild(ART &art, Node &node, const uint8_t byte, const Node child);
	//! Delete the child node at byte
	static void DeleteChild(ART &art, Node &node, const uint8_t byte);

	//! Replace the child node at byte
	inline void ReplaceChild(const uint8_t byte, const Node child) {
		children[byte] = child;
	}

	//! Get the (immutable) child for the respective byte in the node
	const Node GetChild(const uint8_t byte) const;

     //! Returns the string representation of the node
	std::string ToString(ART &art) const;
};
}  // namespace duckart
