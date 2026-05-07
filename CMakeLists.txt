cmake_minimum_required(VERSION 3.10)

project(trieflix)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_executable(
	trieflix
	src/main.cpp
    	src/Trie.cpp
    	src/SearchEngine.cpp
    	src/CsvReader.cpp
    	src/Utils.cpp
)
