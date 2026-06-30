cmake_minimum_required(VERSION 3.10)

project(trieflix)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

add_executable(trieflix
    src/main.cpp
    src/SuffixTrie.cpp
    src/SearchEngine.cpp
    src/CsvReader.cpp
    src/Utils.cpp
    src/SessionManager.cpp
    src/SearchQuery.cpp
    src/MoviePrinter.cpp
    src/ConsoleNotificationSystem.cpp
)

find_package(Threads REQUIRED)
target_link_libraries(trieflix PRIVATE Threads::Threads)

