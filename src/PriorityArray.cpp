#include "PriorityArray.h"

template <size_t Size, class K, class V>
PriorityArray<Size, K, V>::PriorityArray() {}

template <size_t Size, class K, class V>
void PriorityArray<Size, K, V>::insert(K &k, V &v) {
    insert({k, v});
}

template <size_t Size, class K, class V>
void PriorityArray<Size, K, V>::insert(Pair<K, V> &pair) {

}

template <size_t Size, class K, class V>
V *PriorityArray<Size, K, V>::get(K k) const {
//    for (auto i = data.begin(); i != data.end(); ++i);
    return nullptr;
}
