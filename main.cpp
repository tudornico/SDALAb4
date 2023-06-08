#include <iostream>
#include "assert.h"

#include "ShortTest.h"
#include "ExtendedTest.h"
#include "SortedMultiMap.h"
bool asc1(TKey c1, TKey c2) {
    if (c1 <= c2) {
        return true;
    } else {
        return false;
    }
}
int main(){
    testAll();
    testAllExtended();

    std::cout<<"Finished SMM Tests!"<<std::endl;
    system("pause");

    SortedMultiMap map1 = SortedMultiMap(asc1);
    SortedMultiMap map2 = SortedMultiMap(asc1);

    map1.add(1,3);
    map1.add(1,4);
    map1.add(3,5);

    map2.add(3,4);
    map2.add(2,5);
    map2.add(3,6);

   SortedMultiMap resultMap =  map1.interclass(map2);

    assert(resultMap.size() == 6);
    vector<int> vector1 = {4,5,6};
    assert(resultMap.search(3) == vector1);
    cout<<"aaa";
}