#pragma once

#include "art.hpp"
#include "common.hpp"
#include "fixed_size_allocator.hpp"
#include "tagged_pointer.hpp"
#include <iostream>


namespace duckart {
class ART;
class FixedSizeAllocator; 
 
class Node : public TaggedPointer<void> {
   public:
    using TaggedPointer<void>::TaggedPointer;

    template <typename T>
    Node(T *ptr, NType type)
        : TaggedPointer<void>(static_cast<void *>(ptr), type) {}

    Node() : TaggedPointer<void>(nullptr, NType::NODE_DUMMY) {}

    //! Destructor
    ~Node() {
         //std::cout <<" node being Destructor:"  << AddrToString() << std::endl;
    }

    //!
    static FixedSizeAllocator &GetAllocator(const ART &art, const NType type);

    template <typename NODE>
    static NODE &RefMutable(const ART &art, const Node &ptr, const NType type);

    template <class NODE>
    static const NODE &Ref(const ART &art, const Node &ptr,  const NType type);

    //! Get a new pointer to a node, might cause a new buffer allocation, and
    //! initialize it
    static void New(ART &art, Node &node, const NType type);
    //! Free the node (and its subtree)
    static void Free(ART &art, Node &node);

    //! Insert the child node at byte
    static void InsertChild(ART &art, Node &node, const uint8_t byte,
                            const Node child);
    //! Delete the child node at byte
    static void DeleteChild(ART &art, Node &node, const uint8_t byte);

    //! Replace the child node at byte
    void ReplaceChild(const ART &art, const uint8_t byte,
                      const Node child) const;

    //! Get the child (immutable) for the respective byte in the node
    const Node GetChild(ART &art, const uint8_t byte) const;

    //！ Get Prefix 
   const Node GetPrefix(ART& art) const;
   //!  Set Prefix
   void SetPrefix(ART& art,Node& node) const;

   //! Returns the string representation of the node
   static void Print(ART &art, Node &node);
};

}  // namespace duckart
