#pragma once

#include <unordered_map>
#include <unordered_set>

namespace ankerl {
namespace unordered_dense {

template <class Key,
          class T,
          class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>,
          class Alloc = std::allocator<std::pair<const Key, T>>>
using map = std::unordered_map<Key, T, Hash, KeyEqual, Alloc>;

template <class Key,
          class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>,
          class Alloc = std::allocator<Key>>
using set = std::unordered_set<Key, Hash, KeyEqual, Alloc>;

}  // namespace unordered_dense
}  // namespace ankerl
