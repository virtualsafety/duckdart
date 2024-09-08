#pragma once

#include <sstream>
#include <string>

#include "art.hpp"
#include "common.hpp"
#include "fixed_size_allocator.hpp"
#include "node.hpp"
#include "prefix.hpp"

namespace duckart {

class Node48 {
   public:
    //! Delete copy constructors, as any Node48 can never own its memory
    Node48(const Node48 &) = delete;
    Node48 &operator=(const Node48 &) = delete;
 
    //!prefix
    Node prefix;
    //! Number of non-null children
    uint8_t count;
    //! Array containing all possible partial key bytes, those not set have an
    //! EMPTY_MARKER
    uint8_t child_index[NODE_256_CAPACITY];
    //! Node pointers to the child nodes
    Node children[NODE_48_CAPACITY];

   public:
    //! Get a new Node48, might cause a new buffer allocation, and initialize it
    static Node48 &New(ART &art, Node &node);
    //! Free the node (and its subtree)
    static void Free(ART &art, Node &node);

    //! Initializes all the fields of the node while growing a Node16 to a Node48
	static Node48 &GrowNode16(ART &art, Node &node48, Node &node16);
	//! Initializes all fields of the node while shrinking a Node256 to a Node48
	static Node48 &ShrinkNode256(ART &art, Node &node48, Node &node256);

 	//! Insert a child node at byte
	static void InsertChild(ART &art, Node &node, const uint8_t byte, const Node child);
	//! Delete the child node at byte
	static void DeleteChild(ART &art, Node &node, const uint8_t byte);

	//! Replace the child node at byte
	inline void ReplaceChild(const uint8_t byte, const Node child) {
		D_ASSERT(child_index[byte] != EMPTY_MARKER);
		children[child_index[byte]] = child;
	}

	//! Get the (immutable) child for the respective byte in the node
	 const Node GetChild(const uint8_t byte) const;

      //! Returns the string representation of the node
     std::string ToString(ART &art) const;
};

}  // namespace duckart
