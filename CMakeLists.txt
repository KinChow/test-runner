cmake_minimum_required(VERSION 3.10)

# 项目声明
set(PROJECT_NAME "test-runner")
project(${PROJECT_NAME})
set(CMAKE_PROJECT_DESCRIPTION "A simple test runner for C++ projects")
set(CMAKE_PROJECT_HOMEPAGE_URL "")

# 设置 C 和 C++ 标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 生成时间戳
string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S")

# ----- 版本设置 ----- #
# 项目版本号
set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 0)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")



# 根据构建类型设置不同的优化选项
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -fno-common -DNDEBUG")

# 项目内部头文件路径
set(PROJECT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# 收集源文件
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SOURCE_DIR)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/task TASK_SOURCE_DIR)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/test_runner TEST_RUNNER_SOURCE_DIR)
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src/elementwise ELEMENTWISE_SOURCE_DIR)

add_executable(${PROJECT_NAME} ${SOURCE_DIR} ${TASK_SOURCE_DIR} ${TEST_RUNNER_SOURCE_DIR} ${ELEMENTWISE_SOURCE_DIR})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_INCLUDE_DIR}
)

# ----- 编译选项设置 ----- #
# 通用编译选项
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4                      # MSVC的警告等级
        # /WX                      # 将警告视为错误
        /sdl                     # 启用安全检查
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall                    # 启用所有常见警告
        -Wextra                  # 启用额外警告
        -Wpedantic               # 严格遵循标准
        -Werror                  # 将警告视为错误
        -fstack-protector-strong # 堆栈保护
        -Wformat                 # 格式字符串检查
        -Wformat-security        # 格式安全检查
        -Wconversion             # 类型转换警告
        -fPIC                    # 生成位置无关代码
    )
endif()
