#ifndef DBMS_PRIORITYARRAY_H
#define DBMS_PRIORITYARRAY_H

#include <array>

template <class K, class V>
struct Pair {
    K k;
    V v;
};

template <size_t Size, class K, class V>
class PriorityArray {
public:
    PriorityArray();

    void insert(K&, V&);
    void insert(Pair<K, V>&);

    V* get(K) const;
private:
//    std::array<Size, Pair<K, V>> data;
};


#endif //DBMS_PRIORITYVECTOR_H
