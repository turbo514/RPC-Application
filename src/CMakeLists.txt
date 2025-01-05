aux_source_directory(. SRC_LIST)
add_library(mprpc ${SRC_LIST})

# 设置静态库的输出目录
set_target_properties(mprpc PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
)

# 设置动态库的输出目录
set_target_properties(mprpc PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
)
# 定义 THREADED 宏，确保多线程代码被编译
add_definitions(-DTHREADED)

target_link_libraries(mprpc muduo_net muduo_base zookeeper_mt pthread )
