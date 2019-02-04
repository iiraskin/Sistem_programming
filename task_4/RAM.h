#pragma once

#include <windows.h>

#define CHECK_ARGS \
if (i >= buf_size) { \
    printf("RAM::Read: too big index\n"); \
    return 1; \
}

class RAM {
//Constants
private:
    const int DELAY = 500;
    const int DEFAULT_SIZE = 100;

private:
    int* buffer;
    int buf_size;

public:
    RAM(int buf_size_)
    {
        buf_size = buf_size_;
        buffer = new int[buf_size];
    }

    RAM()
    {
        buf_size = DEFAULT_SIZE;
        buffer = new int[buf_size];
    }

    ~RAM()
    {
        delete[] buffer;
    }

    int Read(int i, int* value) {
        Sleep(DELAY);
        CHECK_ARGS

        *value = buffer[i];
        return 0;
    }

    int Write(int i, int value) {
        Sleep(DELAY);
        CHECK_ARGS

        buffer[i] = value;
        return 0;
    }
};
