#include <vector>

namespace skiplist {

template<typename K, typename V>
class Node {
public:
	K key;
	V value;
	std::vector<Node<K, V>*> forward;

	Node(K k, V v, int level) : key(k), value(v), forward(level+1, nullptr){}
};

}
