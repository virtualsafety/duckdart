#pragma once
#include <cstring>

#include "art.hpp"
#include "node.hpp"
#include "value.hpp"

namespace duckart {


class Leaf {
   public:
    //! Delete copy constructors, as any Leaf can never own its memory
    Leaf(const Leaf&) = delete;
    Leaf& operator=(const Leaf&) = delete;

    //! prefix
    Node prefix;

    //! value
    Value value;

   public:
    //! new Leaf
    static Leaf& New(ART& art, Node& node, const Value& value);
    //! Free the leaf
    static void Free(ART& art, Node& node);
};


}  // namespace duckart
