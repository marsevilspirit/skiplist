#include "node.hpp"

template<typename K, typename V>
class SkipList {
private:
	int maxLevel;
	float p;
	int currentLevel;
	Node<K, V>* header;

public:
	SkipList(int maxLvl, float prob);
	~SkipList();

	int getRandomLevel();

	Node<K, V>* createNode(K key, V value, int level);

	void insertElement(K key, V value);

	Node<K, V>* searchElement(K key);

	void deleteElement(K key);

	void displayList();
};
