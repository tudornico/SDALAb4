cmake_minimum_required(VERSION 3.25)
project(SDALAb4)

set(CMAKE_CXX_STANDARD 17)

add_executable(SDALAb4 main.cpp ExtendedTest.cpp ExtendedTest.h ShortTest.cpp ShortTest.h SMMIterator.cpp SMMIterator.h SortedMultiMap.cpp SortedMultiMap.h)
