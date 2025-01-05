cmake_minimum_required(VERSION 3.10)
project(mprpc)

# 生成debug版本，可以用gdb调试
set(CMAKE_BUILD_TYPE "Debug")

# 可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 库文件输出路径
set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

# 编译头文件查找路径
include_directories(${PROJECT_SOURCE_DIR}/src/include)
include_directories(${PROJECT_SOURCE_DIR}/example)
# 链接库文件查找路径
link_directories(${PROJECT_SOURCE_DIR}/lib)

# src包含了mprpc框架的所有相关代码
add_subdirectory(src)
#example包含了mprpc框架使用的示例代码
add_subdirectory(example)