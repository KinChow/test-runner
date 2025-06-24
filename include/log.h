#ifndef LOG_H
#define LOG_H

#ifndef PROJECT_NAME
#define PROJECT_NAME "GpuMgr"
#endif  // PROJECT_NAME

#include <stdio.h>

#define LOGF(format, ...)                                                                                     \
    printf("[FATAL] [%s] [%-24.24s] [%-4d] [" format "]\n", PROJECT_NAME, __func__, __LINE__, ##__VA_ARGS__); \
    fflush(stdout)

#define LOGE(format, ...)                                                                                     \
    printf("[ERROR] [%s] [%-24.24s] [%-4d] [" format "]\n", PROJECT_NAME, __func__, __LINE__, ##__VA_ARGS__); \
    fflush(stdout)

#define LOGW(format, ...)                                                                                       \
    printf("[WARNING] [%s] [%-24.24s] [%-4d] [" format "]\n", PROJECT_NAME, __func__, __LINE__, ##__VA_ARGS__); \
    fflush(stdout)

#define LOGI(format, ...)                                                                                    \
    printf("[INFO] [%s] [%-24.24s] [%-4d] [" format "]\n", PROJECT_NAME, __func__, __LINE__, ##__VA_ARGS__); \
    fflush(stdout)

#define LOGD(format, ...)                                                                                     \
    printf("[DEBUG] [%s] [%-24.24s] [%-4d] [" format "]\n", PROJECT_NAME, __func__, __LINE__, ##__VA_ARGS__); \
    fflush(stdout)

#define LOGV(format, ...)                                                                                       \
    printf("[VERBOSE] [%s] [%-24.24s] [%-4d] [" format "]\n", PROJECT_NAME, __func__, __LINE__, ##__VA_ARGS__); \
    fflush(stdout)

#endif  // LOG_H