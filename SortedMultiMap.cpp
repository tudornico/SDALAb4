#include "SMMIterator.h"
#include "SortedMultiMap.h"
#include <iostream>
#include <vector>
#include <exception>

using namespace std;

SortedMultiMap::SortedMultiMap(Relation r) : relation(r) {
    this->currentSize = 0;

    this->divisor = 1093;
    this->firstEmpty = 0;
    this->hashNodes = new HashNode *[this->divisor];

    this->nextIndexes = new int[this->divisor];

    for (int i = 0; i < this->divisor; i++) {
        this->hashNodes[i] = nullptr;
    }

    for (int i = 0; i < this->divisor - 1; i++) {
        this->nextIndexes[i] = i + 1;
    }
    this->nextIndexes[this->divisor - 1] = -1;
}

void SortedMultiMap::add(TKey c, TValue v) {
    if (this->shouldResize()) {
        resize();
    }

    int hashValue = this->hash(c);

    HashNode *currentNode = this->hashNodes[hashValue];
    if (currentNode == nullptr) {
        this->hashNodes[hashValue] = createNewHashNode(c, v);
        ++this->currentSize;
        return;
    }

    int previousIndex = -1;
    int currentIndex = hashValue;
    while (currentIndex != -1) {
        HashNode *currentNode = this->hashNodes[currentIndex];
        if (currentNode->key == c) {
            addToSortedList(currentNode, v);
            return;
        }

        previousIndex = currentIndex;
        currentIndex = nextIndexes[currentIndex];
    }

    int storedIndex = this->firstEmpty;
    this->nextIndexes[previousIndex] = storedIndex;
    this->firstEmpty = this->nextIndexes[storedIndex];
    this->nextIndexes[storedIndex] = -1;
    this->hashNodes[storedIndex] = createNewHashNode(c, v);
    ++this->currentSize;
}


vector<TValue> SortedMultiMap::search(TKey c) const {
    vector<TValue> values{};
    HashNode *node = findHashNodeByKey(c);
    if (node == nullptr) {
        return values;
    }

    ListNode *listNode = node->head;
    while (listNode != nullptr) {
        values.push_back(listNode->value);
        listNode = listNode->next;
    }
    return values;
}

bool SortedMultiMap::remove(TKey c, TValue v) {
    HashNode *hashNode = findHashNodeByKey(c);
    if (hashNode == nullptr) {
        return false;
    }
    ListNode *listNode = hashNode->head;
    ListNode *savedNode = nullptr;

    bool done = false;
    while (listNode != nullptr && !done) {
        if (listNode->value == v) {
            if (savedNode == nullptr) {
                hashNode->head = listNode->next;
            } else {
                savedNode->next = listNode->next;
            }
            delete listNode;
            done = true;
            --this->currentSize;
        } else {
            savedNode = listNode;
            listNode = listNode->next;
        }
    }
    return done;
}


int SortedMultiMap::size() const {
    return this->currentSize;
}

bool SortedMultiMap::isEmpty() const {
    return this->currentSize == 0;
}

SMMIterator SortedMultiMap::iterator() const {
    return SMMIterator(*this);
}

SortedMultiMap::~SortedMultiMap() {
    delete[] hashNodes;
    delete[] nextIndexes;
}

void SortedMultiMap::resize() {

}

bool SortedMultiMap::shouldResize() const {
    return this->size() * 4 > this->divisor * 3;
}

int SortedMultiMap::hash(TKey k) const {
    return abs(k) % this->divisor;
}

HashNode *SortedMultiMap::createNewHashNode(TKey k, TValue v) {
    HashNode *newHashNode = new HashNode;
    newHashNode->key = k;

    ListNode *newHead = new ListNode;
    newHead->next = nullptr;
    newHead->value = v;

    newHashNode->head = newHead;
    return newHashNode;
}

void SortedMultiMap::addToSortedList(HashNode *currentNode, TValue v) {
    ListNode *listNode = currentNode->head;
    ListNode *savedNode = nullptr;

    ListNode *newListNode = new ListNode;
    newListNode->value = v;

    bool done = false;
    while (listNode != nullptr && !done) {
        if (relation(v, listNode->value)) {
            if (savedNode == nullptr) {
                newListNode->next = currentNode->head;
                currentNode->head = newListNode;
                done = true;
                ++this->currentSize;
            } else {
                newListNode->next = listNode;
                savedNode->next = newListNode;
                done = true;
                ++this->currentSize;
            }
        }
        savedNode = listNode;
        listNode = listNode->next;
    }

    if (!done) {
        newListNode->next = nullptr;
        savedNode->next = newListNode;
        ++this->currentSize;
    }
}

HashNode *SortedMultiMap::findHashNodeByKey(TKey k) const {

    int currentIndex = hash(k);
    while (currentIndex != -1) {
        HashNode *currentNode = this->hashNodes[currentIndex];
        if (currentNode == nullptr || currentNode->key == k) {
            return currentNode;
        }

        currentIndex = nextIndexes[currentIndex];
    }
    return nullptr;
}

SortedMultiMap SortedMultiMap::interclass(SortedMultiMap map) {

    int parser = 0;
    SortedMultiMap resultMap = SortedMultiMap(relation);
    int keyCounter = 0;
    //parse until we end both
    while (parser < this->currentSize + map.currentSize) {
        // the key in order is only in this
        if (this->relation(this->hashNodes[parser]->key, map.hashNodes[parser]->key)) {
            resultMap.hashNodes[keyCounter]->key = this->hashNodes[parser]->key;
            resultMap.hashNodes[keyCounter]->head = this->hashNodes[parser]->head;
            //add all the listElements to the resultMap
            ListNode *elementCurrent = this->hashNodes[keyCounter]->head;
            ListNode *elementOther = resultMap.hashNodes[keyCounter]->head;
            while (elementCurrent->next != nullptr) {
                elementOther->next = elementCurrent->next;
                elementCurrent = elementCurrent->next;
                elementOther = elementOther->next;
            }
            keyCounter++;
        } else {
            //the key in order is only in map
            if (this->relation(map.hashNodes[parser]->key, map.hashNodes[parser]->key)) {
                resultMap.hashNodes[keyCounter]->key = map.hashNodes[parser]->key;
                resultMap.hashNodes[keyCounter]->head = map.hashNodes[parser]->head;
                //add all the listElements to the resultMap
                ListNode *elementCurrent = map.hashNodes[keyCounter]->head;
                ListNode *elementOther = resultMap.hashNodes[keyCounter]->head;
                while (elementCurrent->next != nullptr) {
                    elementOther->next = elementCurrent->next;
                    elementCurrent = elementCurrent->next;
                    elementOther = elementOther->next;
                }
                keyCounter++;
            } else {
                resultMap.hashNodes[keyCounter]->key = map.hashNodes[parser]->key;
                resultMap.hashNodes[keyCounter]->head = map.hashNodes[parser]->head;
                //we have the same key interclass the pointers
                if (map.hashNodes[parser]->key == this->hashNodes[parser]->key) {
                    //select the head
                    ListNode *elementCurrent;
                    ListNode *elementOther;
                    if (this->relation(map.hashNodes[keyCounter]->head->value,
                                       this->hashNodes[keyCounter]->head->value)) {
                        resultMap.hashNodes[keyCounter]->head = map.hashNodes[keyCounter]->head;
                        elementCurrent = this->hashNodes[keyCounter]->head;
                        elementOther = map.hashNodes[keyCounter]->head->next;
                    } else {
                        resultMap.hashNodes[keyCounter]->head = this->hashNodes[keyCounter]->head;
                        elementCurrent = this->hashNodes[keyCounter]->head->next;
                        elementOther = map.hashNodes[keyCounter]->head;
                    }
                    ListNode *resultValue = resultMap.hashNodes[keyCounter]->head;
                    while (true) {
                        //if one is empty just spam the other list
                        if(elementOther == nullptr){
                            while(elementCurrent != nullptr){
                                resultValue->next = elementCurrent;
                                elementCurrent = elementCurrent->next;
                            }
                            break;
                        }
                        else{
                            if(elementCurrent == nullptr){
                                while(elementOther != nullptr){
                                    resultValue->next = elementOther;
                                    elementOther = elementOther->next;
                                }
                                break;
                            }
                        }
                        if (this->relation(elementCurrent->value , elementOther->value)){
                            resultValue->next = elementCurrent;
                            elementCurrent = elementCurrent->next;
                        }
                        else{
                            resultValue->next = elementOther;
                            elementOther = elementOther->next;
                        }
                    }
                }
            }
        }
    }


    return SortedMultiMap(resultMap);
}


