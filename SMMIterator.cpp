#include "SMMIterator.h"
#include "SortedMultiMap.h"

SMMIterator::SMMIterator(const SortedMultiMap &d) : map(d) {
    this->first();
}

void SMMIterator::first() {
    this->currentIndex = -1;
    this->currentListNode = nullptr;
    this->currentKey = -1;

    this->getMinimumIndex();
}

void SMMIterator::next() {
    if (!valid()) {
        throw exception();
    }

    if (this->currentListNode == nullptr) {
        throw exception();
    }

    if (this->currentListNode->next != nullptr) {
        this->currentListNode = this->currentListNode->next;
        return;
    }

    this->getMinimumIndex();
}

bool SMMIterator::valid() const {
    return this->currentListNode != nullptr && this->currentIndex < this->map.divisor && this->currentIndex > -1;
}

TElem SMMIterator::getCurrent() const {
    if (!valid()) {
        throw exception();
    }
    return {this->map.hashNodes[this->currentIndex]->key, this->currentListNode->value};
}

void SMMIterator::getMinimumIndex() {

    TKey minKey;
    int minIdx;
    bool found_one = false;

    for (int i = 0; i < this->map.divisor; i++) {
        if (this->map.hashNodes[i] != nullptr) {
            TKey key = this->map.hashNodes[i]->key;

            // CURRENT KEY < SEARCHED_KEY < ANY OTHER KEY
            if (this->currentKey == -1 || !this->map.relation(key, currentKey)) {
                if (!found_one || this->map.relation(key, minKey)) {
                    minKey = key;
                    minIdx = i;
                    found_one = true;
                }
            }
        }
    }

    if (!found_one) {
        this->currentIndex = -1;
        this->currentKey = -1;
        this->currentListNode = nullptr;
    } else {
        this->currentIndex = minIdx;
        this->currentKey = minKey;
        this->currentListNode = this->map.hashNodes[this->currentIndex]->head;
    }
}

