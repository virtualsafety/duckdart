#include "node.hpp"

#include "exception.hpp"
#include "leaf.hpp"
#include "logger.hpp"
#include "node16.hpp"
#include "node256.hpp"
#include "node4.hpp"
#include "node48.hpp"
#include "prefix.hpp"
#include "string_type.hpp"

namespace duckart {

// NODE
FixedSizeAllocator& Node::GetAllocator(const ART& art, const NType type) {
    return art.GetAllocator(type);
}

template <typename NODE>
NODE& Node::RefMutable(const ART& art, const Node& ptr, const NType type) {
    return *static_cast<NODE*>(GetAllocator(art, type).Get<NODE>(ptr));
}
// begin why ?
template <>
Prefix& Node::RefMutable<Prefix>(const ART& art, const Node& ptr,
                                 const NType type) {
    return *static_cast<Prefix*>(GetAllocator(art, type).Get<Prefix>(ptr));
}

template <>
Node4& Node::RefMutable<Node4>(const ART& art, const Node& ptr,
                               const NType type) {
    return *static_cast<Node4*>(GetAllocator(art, type).Get<Node4>(ptr));
}

template <>
Node16& Node::RefMutable<Node16>(const ART& art, const Node& ptr,
                                 const NType type) {
    return *static_cast<Node16*>(GetAllocator(art, type).Get<Node16>(ptr));
}

template <>
Node48& Node::RefMutable<Node48>(const ART& art, const Node& ptr,
                                 const NType type) {
    return *static_cast<Node48*>(GetAllocator(art, type).Get<Node48>(ptr));
}

template <>
Node256& Node::RefMutable<Node256>(const ART& art, const Node& ptr,
                                   const NType type) {
    return *static_cast<Node256*>(GetAllocator(art, type).Get<Node256>(ptr));
}

template <>
Leaf& Node::RefMutable<Leaf>(const ART& art, const Node& ptr,
                             const NType type) {
    return *static_cast<Leaf*>(GetAllocator(art, type).Get<Leaf>(ptr));
}
// end why?

template <typename NODE>
const NODE& Node::Ref(const ART& art, const Node& ptr, const NType type) {
    return *static_cast<const NODE*>(
        GetAllocator(art, type).Get<const NODE>(ptr));
}

template <>
const Prefix& Node::Ref<const Prefix>(const ART& art, const Node& ptr,
                                      const NType type) {
    return *static_cast<const Prefix*>(
        GetAllocator(art, type).Get<const Prefix>(ptr));
}

void Node::New(ART& art, Node& node, const NType type) {
    switch (type) {
        case NType::NODE_4:
            Node4::New(art, node);
            break;
        case NType::NODE_16:
            Node16::New(art, node);
            break;
        case NType::NODE_48:
            Node48::New(art, node);
            break;
        case NType::NODE_256:
            Node256::New(art, node);
            break;
        default:
            throw InternalException("Invalid node type for New.");
    }
}

void Node::Free(ART& art, Node& node) {
    LOG_DEBUG("node type:" + std::to_string(static_cast<int>(node.getTag())));

    if (node.IsCleared()) {
        return node.Clear();
    }

    // free the children of the nodes
    auto type = node.getTag();
    switch (type) {
        // iterative
        case NType::LEAF:
            return Leaf::Free(art, node);
        // iterative
        case NType::PREFIX:
            return Prefix::Free(art, node);
        case NType::NODE_4:
            Node4::Free(art, node);
            break;
        case NType::NODE_16:
            Node16::Free(art, node);
            break;
        case NType::NODE_48:
            Node48::Free(art, node);
            break;
        case NType::NODE_256:
            Node256::Free(art, node);
            break;
    }

    GetAllocator(art, type).Free(node.getPointer());
    node.Clear();
}

//===--------------------------------------------------------------------===//
// Inserts
//===--------------------------------------------------------------------===//
void Node::ReplaceChild(const ART& art, const uint8_t byte,
                        const Node child) const {
    LOG_DEBUG("node type:" + std::to_string(static_cast<int>(getTag())));

    auto type = getTag();
    switch (type) {
        case NType::NODE_4:
            return RefMutable<Node4>(art, *this, NType::NODE_4)
                .ReplaceChild(byte, child);
        case NType::NODE_16:
            return RefMutable<Node16>(art, *this, NType::NODE_16)
                .ReplaceChild(byte, child);
        case NType::NODE_48:
            return RefMutable<Node48>(art, *this, NType::NODE_48)
                .ReplaceChild(byte, child);
        case NType::NODE_256:
            return RefMutable<Node256>(art, *this, NType::NODE_256)
                .ReplaceChild(byte, child);
        default:
            throw InternalException("Invalid node type for ReplaceChild.");
    }
}

void Node::InsertChild(ART& art, Node& node, const uint8_t byte,
                       const Node child) {
    auto type = node.getTag();
    switch (type) {
        case NType::NODE_4:
            return Node4::InsertChild(art, node, byte, child);
        case NType::NODE_16:
            return Node16::InsertChild(art, node, byte, child);
        case NType::NODE_48:
            return Node48::InsertChild(art, node, byte, child);
        case NType::NODE_256:
            return Node256::InsertChild(art, node, byte, child);
        default:
            throw InternalException("Invalid node type for InsertChild.");
    }
}

//===--------------------------------------------------------------------===//
// Deletes
//===--------------------------------------------------------------------===//

void Node::DeleteChild(ART& art, Node& node, const uint8_t byte) {
    LOG_DEBUG("node type:" + std::to_string(static_cast<int>(node.getTag())));

    auto type = node.getTag();
    switch (type) {
        case NType::NODE_4:
            return Node4::DeleteChild(art, node, byte);
        case NType::NODE_16:
            return Node16::DeleteChild(art, node, byte);
        case NType::NODE_48:
            return Node48::DeleteChild(art, node, byte);
        case NType::NODE_256:
            return Node256::DeleteChild(art, node, byte);
        default:
            throw InternalException("Invalid node type for DeleteChild.");
    }
}

//===--------------------------------------------------------------------===//
// Get functions
//===--------------------------------------------------------------------===//
const Node Node::GetChild(ART& art, const uint8_t byte) const {
    D_ASSERT(!IsCleared());
    LOG_DEBUG("get child,node type:" +
              std::to_string(static_cast<int>(getTag())));

    auto type = getTag();
    switch (type) {
        case NType::NODE_4:
            return Ref<const Node4>(art, *this, NType::NODE_4).GetChild(byte);
        case NType::NODE_16:
            return Ref<const Node16>(art, *this, NType::NODE_16).GetChild(byte);
        case NType::NODE_48:
            return Ref<const Node48>(art, *this, NType::NODE_48).GetChild(byte);
        case NType::NODE_256:
            return Ref<const Node256>(art, *this, NType::NODE_256)
                .GetChild(byte);
        default:
            throw InternalException("Invalid node type for GetChild.");
    }
}

const Node Node::GetPrefix(ART& art) const {
    D_ASSERT(!IsCleared());
    LOG_DEBUG("get prefix,node type:" +
              std::to_string(static_cast<int>(getTag())));

    auto type = getTag();
    switch (type) {
        case NType::LEAF:
            return Ref<const Leaf>(art, *this, NType::LEAF).prefix;
        case NType::NODE_4:
            return Ref<const Node4>(art, *this, NType::NODE_4).prefix;
        case NType::NODE_16:
            return Ref<const Node16>(art, *this, NType::NODE_16).prefix;
        case NType::NODE_48:
            return Ref<const Node48>(art, *this, NType::NODE_48).prefix;
        case NType::NODE_256:
            return Ref<const Node256>(art, *this, NType::NODE_256).prefix;
        default:
            throw InternalException("Invalid node type for GetPrefix.");
    }
}

void Node::SetPrefix(ART& art, Node& node) const {
    D_ASSERT(!IsCleared());
    LOG_DEBUG("set prefix,node type:" +
              std::to_string(static_cast<int>(getTag())));

    auto type = getTag();
    switch (type) {
        case NType::LEAF:
            RefMutable<Leaf>(art, *this, NType::LEAF).prefix = node;
            break;
        case NType::NODE_4:
            RefMutable<Node4>(art, *this, NType::NODE_4).prefix = node;
            break;
        case NType::NODE_16:
            RefMutable<Node16>(art, *this, NType::NODE_16).prefix = node;
            break;
        case NType::NODE_48:
            RefMutable<Node48>(art, *this, NType::NODE_48).prefix = node;
            break;
        case NType::NODE_256:
            RefMutable<Node256>(art, *this, NType::NODE_256).prefix = node;
            break;
        default:
            throw InternalException("Invalid node type for SetPrefix.");
    }
}

void Node::Print(ART& art, Node& node) {
    D_ASSERT(!node.IsCleared());

    auto type = node.getTag();
    std::string node_string;
    switch (type) {
        case NType::NODE_4:
            node_string =
                RefMutable<Node4>(art, node, NType::NODE_4).ToString(art);
            break;
        case NType::NODE_16:
            node_string =
                RefMutable<Node16>(art, node, NType::NODE_16).ToString(art);
            break;
        case NType::NODE_48:
            node_string =
                RefMutable<Node48>(art, node, NType::NODE_48).ToString(art);
            break;
        case NType::NODE_256:
            node_string =
                RefMutable<Node256>(art, node, NType::NODE_256).ToString(art);
            break;
        default:
            node_string =
                "node_type:" + std::to_string(static_cast<int>(type)) +
                ",not inner node";
    }

    std::cout << node_string << std::endl;
} 
}  // namespace duckart
