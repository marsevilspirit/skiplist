#include <cstdlib>
#include <iostream>

#include "skiplist.hpp"

template<typename K, typename V>
SkipList<K,V>::SkipList(int maxLvl, float prob) : maxLevel(maxLvl), p(prob), currentLevel(0) {
	K dummyKey;
	V dummyValue;
	header = new Node<K, V>(dummyKey, dummyValue, maxLevel);
}

template<typename K, typename V>
SkipList<K, V>::~SkipList() {
	Node<K, V>* current = header->forward[0];
	while (current != nullptr) {
		Node<K, V>* temp = current;
		current = current->forward[0];
		delete temp;
	}
	delete header;
}

template<typename K, typename V>
int SkipList<K, V>::getRandomLevel() {
    int lvl = 0;
    while ((float)rand() / RAND_MAX < p && lvl < maxLevel) {
        lvl++;
    }
    return lvl;
}

template<typename K, typename V>
Node<K, V>* SkipList<K, V>::createNode(K key, V value, int level) {
    return new Node<K, V>(key, value, level);
}

template<typename K, typename V>
void SkipList<K, V>::insertElement(K key, V value) {
	std::vector<Node<K, V>*> update(maxLevel + 1, nullptr);
	Node<K, V>* current = header;

	for (int i = currentLevel; i >= 0; i--) {
		while (current->forward[i] != nullptr && current->forward[i]->key < key) {
			current = current->forward[i];
		}
		update[i] = current;
	}

	current = current->forward[0];

	if (current != nullptr && current->key == key) {
		std::cout << "Key " << " already exists. Insertion failed." << std::endl;
		return;
	}
	
	int randomLvl = getRandomLevel();

	if (randomLvl > currentLevel) {
		for (int i = currentLevel + 1; i <= randomLvl; i++) {
			update[i] = header;
		}

		currentLevel = randomLvl;
	}

	Node<K, V> newNode = createNode(key, value, randomLvl);

	for (int i = 0; i <= randomLvl; i++) {
		newNode.forward[i] = update[i]->forward[i];
		update[i]->forward[i] = newNode;
	}

	std::cout << "Successfully inserted key " << key << std::endl;
}

template<typename K, typename V>
Node<K, V>* SkipList<K, V>:: searchElement(K key) {
	Node<K, V>* current = header;

	for (int i = currentLevel; i >= 0; i--) {
        while (current->forward[i] != nullptr && current->forward[i]->key < key) {
            current = current->forward[i];
        }
	}

    // 移动到第 0 层，此时 current 的下一个节点可能是我们要找的
    current = current->forward[0];

    // 检查第 0 层的下一个节点是不是就是我们要找的
    if (current != nullptr && current->key == key) {
        std::cout << "Found key " << key << ", value: " << current->value << std::endl;
        return current;
    }

    std::cout << "Key " << key << " not found." << std::endl;
    return nullptr;
}
