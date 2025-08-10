cmake_minimum_required(VERSION 3.16)

project(SkipList
    VERSION 1.0.0
    DESCRIPTION "A C++ implementation of Skip List data structure"
    LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# 设置构建类型的默认值
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# 包含目录
include_directories(src)

# 创建头文件库
add_library(skiplist INTERFACE)
target_include_directories(skiplist INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<INSTALL_INTERFACE:include/skiplist>)
target_compile_features(skiplist INTERFACE cxx_std_17)

# 示例程序
add_executable(skiplist_example examples/main.cpp)
target_link_libraries(skiplist_example skiplist)

# 测试选项
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    # 查找或下载 Google Test
    find_package(GTest QUIET)

    if(NOT GTest_FOUND)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50f33f9142fd2db563d69b413611e6.zip
        )
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()

    enable_testing()

    # 测试可执行文件
    add_executable(skiplist_tests tests/test_skiplist.cpp)
    target_link_libraries(skiplist_tests skiplist gtest_main gtest pthread)

    # 添加测试
    include(GoogleTest)
    gtest_discover_tests(skiplist_tests)
endif()

# 安装规则
install(FILES src/skiplist.hpp src/node.hpp
        DESTINATION include/skiplist)

# 包配置
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/SkipListConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/SkipListConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/SkipListConfig.cmake"
    INSTALL_DESTINATION lib/cmake/SkipList
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/SkipListConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/SkipListConfigVersion.cmake"
    DESTINATION lib/cmake/SkipList
)

install(TARGETS skiplist
        EXPORT SkipListTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include)

install(EXPORT SkipListTargets
        FILE SkipListTargets.cmake
        NAMESPACE SkipList::
        DESTINATION lib/cmake/SkipList)
