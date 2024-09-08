#include "fixed_size_allocator.hpp"

#include "prefix.hpp"
#include "leaf.hpp"
#include "node.hpp"
#include "node4.hpp"
#include "node16.hpp"
#include "node48.hpp"
#include "node256.hpp"
#include "string_type.hpp"

namespace duckart {

template <typename T>
T* FixedSizeAllocator::Get(const Node& ptr) const {
    return reinterpret_cast<T*>(ptr.getPointer());
}


// Explicit template instantiation
template Prefix* FixedSizeAllocator::Get<Prefix>(const Node& ptr) const;
template Node4* FixedSizeAllocator::Get<Node4>(const Node& ptr) const;
template Node16* FixedSizeAllocator::Get<Node16>(const Node& ptr) const;
template Node48* FixedSizeAllocator::Get<Node48>(const Node& ptr) const;
template Node256* FixedSizeAllocator::Get<Node256>(const Node& ptr) const;
template Leaf* FixedSizeAllocator::Get<Leaf>(const Node& ptr) const;

// Add const versions of the template instantiations
template const Prefix* FixedSizeAllocator::Get<const Prefix>(const Node& ptr) const;
template const Node4* FixedSizeAllocator::Get<const Node4>(const Node& ptr) const;
template const Node16* FixedSizeAllocator::Get<const Node16>(const Node& ptr) const;
template const Node48* FixedSizeAllocator::Get<const Node48>(const Node& ptr) const;
template const Node256* FixedSizeAllocator::Get<const Node256>(const Node& ptr) const;
template const Leaf* FixedSizeAllocator::Get<const Leaf>(const Node& ptr) const;

}  // namespace duckart
