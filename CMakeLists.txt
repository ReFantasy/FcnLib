CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(GeneralLibrary)

SET(CMAKE_CXX_FLAGS "-std=c++17")

# 编译版本
SET(CMAKE_BUILD_TYPE "Release")
#SET(CMAKE_BUILD_TYPE "Debug")

#字符集
Add_Definitions(-DUNICODE -D_UNICODE)

# 子目录
ADD_SUBDIRECTORY(ThreadPool) 
ADD_SUBDIRECTORY(SerialPort) 
ADD_SUBDIRECTORY(other) 


#include_directories("${PROJECT_SOURCE_DIR}/ThreadPool")

AUX_SOURCE_DIRECTORY(. SRC_LIST)
ADD_EXECUTABLE(gl ${SRC_LIST})

# 添加链接库
target_link_libraries(gl thread_pool)





