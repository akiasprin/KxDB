cmake_minimum_required(VERSION 3.1)
project(KxDB)

set(CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
SET( CMAKE_CXX_FLAGS_DEBUG " -Wextra -Wall -g -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -O0 " CACHE STRING "" )

include_directories(AirinSE)
include_directories(utils)

include_directories(/usr/include/)
include_directories(/usr/local/opt/zstd/include)
include_directories(/usr/local/opt/openssl@1.1/include)

link_directories(
        /usr/local/opt/zstd/lib
        /usr/local/opt/openssl@1.1/lib
)

link_libraries(
        -lzstd
        -lpthread
        -lcrypto
)

add_executable(KxDB
        AirinSE/AirinBlockData.cpp
        AirinSE/AirinBlockData.hpp
        AirinSE/AirinCell.cpp
        AirinSE/AirinCell.hpp
        AirinSE/AirinFile.cpp
        AirinSE/AirinFile.hpp
        AirinSE/AirinBlockIndex.cpp
        AirinSE/AirinBlockIndex.hpp
        AirinSE/AirinMem.cpp
        AirinSE/AirinMem.hpp
        test/io.cpp
        utils/LRU.hpp
        utils/Constant.hpp
        main.cpp 
        AirinSE/AirinStore.cpp
        AirinSE/AirinStore.hpp
        utils/Helper.hpp
        AirinSE/AirinStoreScanner.cpp
        AirinSE/AirinStoreScanner.hpp
        AirinSE/AirinFileScanner.cpp
        AirinSE/AirinFileScanner.hpp
        AirinSE/AirinFilePopulation.cpp
        AirinSE/AirinFilePopulation.hpp
        AirinSE/AirinBlockAbstract.hpp
        AirinSE/AirinBlockAbstract.cpp
        AirinSE/AirinBlockBloomFilter.hpp
        AirinSE/AirinBlockBloomFilter.cpp
        utils/Heap.hpp)

