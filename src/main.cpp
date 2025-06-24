#include "log.h"

int main(int argc, char* argv[]) {
    LOGI("Starting application with %d arguments", argc);
    for (int i = 0; i < argc; ++i) {
        LOGI("Argument %d: %s", i, argv[i]);
    }
    return 0;
}