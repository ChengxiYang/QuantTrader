#pragma once

#include <unordered_map>
#include <unordered_set>

namespace tsl {

template <class Key,
          class T,
          class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>,
          class Alloc = std::allocator<std::pair<const Key, T>>>
using robin_map = std::unordered_map<Key, T, Hash, KeyEqual, Alloc>;

}  // namespace tsl
