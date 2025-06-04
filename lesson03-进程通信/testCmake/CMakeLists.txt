
# 表明项目的最低版本号 3.10
cmake_minimum_required(VERSION 3.10)

# 声明项目名称
project(testCmake)

# bin 存放生成的可执行文件
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# 将当前目录的所有源文件保存在SRC_DIR中
aux_source_directory(src SRC_DIR)

# include 存放头文件
include_directories(include)

# 声明目标文件和依赖文件
add_executable(main ${SRC_DIR})