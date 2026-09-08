#pragma once

#include <unordered_set>

namespace tsl {

template <class Key,
          class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>,
          class Alloc = std::allocator<Key>>
using robin_set = std::unordered_set<Key, Hash, KeyEqual, Alloc>;

}  // namespace tsl
