
message("* * * * * * * * * * * * * * * * * * * * * * * * * * * * *")
message("*     ______   ______                             __    *")
message("*    / ____/  / ____/   _____  ____ _    ____    / /_   *")
message("*   / /      / / __    / ___/ / __ `/   / __ \\  / __ \\  *")
message("*  / /___   / /_/ /   / /    / /_/ /   / /_/ / / / / /  *")
message("*  \\____/   \\____/   /_/     \\__,_/   / .___/ /_/ /_/   *")
message("*                                    /_/ by Chunel Feng *")
message("* * * * * * * * * * * * * * * * * * * * * * * * * * * * *")

cmake_minimum_required(VERSION 3.5.0)

project(CGraph VERSION 2.0.1)

set(CMAKE_CXX_STANDARD 17)

# 如果开启此宏定义，则CGraph执行过程中，不会在控制台打印任何信息
# add_definitions(-D_CGRAPH_SILENCE_)

IF(APPLE)
    # 非mac平台，暂时不支持自动生成session信息
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -finline-functions -Wno-deprecated-declarations")
    add_definitions(-D_GENERATE_SESSION_)
    add_definitions(-D_ENABLE_LIKELY_)
ELSEIF(UNIX)
    # linux平台加入多线程内容
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pthread -Wno-format-overflow")
    add_definitions(-D_ENABLE_LIKELY_)
ENDIF()

file(GLOB_RECURSE SRC_LIST "./src/*.cpp")

set(TUTORIAL_LIST
        T00-HelloCGraph
        T01-Simple
        T02-Cluster
        T03-Region
        T04-Complex
        T05-Param
        T06-Condition
        T07-MultiPipeline
        T08-Template
        T09-Aspect
        T10-AspectParam
        T11-Singleton
        T12-Function
        T13-Daemon
        T14-Hold
        TU01-ThreadPool
        TU02-Lru
        TU03-Trie
        TU04-Timer
        TU05-Distance)

# 编译libCGraph动态库
# add_library(CGraph SHARED ${SRC_LIST})

foreach(tut ${TUTORIAL_LIST})
    add_executable(${tut} tutorial/${tut}.cpp ${SRC_LIST})
endforeach()
