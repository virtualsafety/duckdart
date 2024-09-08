#pragma once

#include <sstream>
#include <string>

#include "art.hpp"
#include "common.hpp"
#include "fixed_size_allocator.hpp"
#include "node.hpp"
#include "logger.hpp"

namespace duckart {

//! Node16 holds up to 16 Node children sorted by their key byte
class Node16 {
   public:
    //! Delete copy constructors, as any Node16 can never own its memory
    Node16(const Node16 &) = delete;
    Node16 &operator=(const Node16 &) = delete;

    //!prefix
    Node prefix;
    //! Number of non-null children
    uint8_t count;
    //! Array containing all partial key bytes
    uint8_t key[NODE_16_CAPACITY];
    //! Node pointers to the child nodes
    Node children[NODE_16_CAPACITY];

   public:
    //! Get a new Node16, might cause a new buffer allocation, and initialize it
    static Node16 &New(ART &art, Node &node);
    //! Free the node (and its subtree)
    static void Free(ART &art, Node &node);

    //! Initializes all the fields of the node while growing a Node4 to a Node16
	static Node16 &GrowNode4(ART &art, Node &node16, Node &node4);
	//! Initializes all fields of the node while shrinking a Node48 to a Node16
	static Node16 &ShrinkNode48(ART &art, Node &node16, Node &node48); 

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
