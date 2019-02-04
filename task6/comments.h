#pragma once

#include <iostream>
#include <time.h>
#include <stdlib.h>

const char CHAR_INIT = 20;

class Comments
{
public:
    Comments(const char* file);
    ~Comments();

    char* GetComment();

private:
    int ReadText(int fd);
    int MakeListOfLines();

    char* comments;
    char** lines;
    int num_of_lines;
};

Comments::Comments(const char* file)
{
    int fd = open(file, O_RDONLY);
    ReadText(fd);
    close(fd);
    srand(time(nullptr));
}

Comments::~Comments()
{
    delete[] comments;
}

int Comments::ReadText(int fd)
{
    struct stat buff = {};

    fstat(fd, &buff);
    comments = new char[buff.st_size + 1]{CHAR_INIT};

    int text_len = read(fd, comments, buff.st_size);
    comments[text_len] = '\0';

    if (MakeListOfLines()) {
        printf("readText: Error in makeListOfLines\n");
        return 1;
    }
    return 0;
}

int Comments::MakeListOfLines()
{
    num_of_lines = 1;
    for (int i = 0; comments[i + 1] != '\0'; ++i) {
        if (comments[i] == '\n') {
            ++num_of_lines;
        }
    }

    lines = new char*[num_of_lines + 1]{nullptr};
    lines[0] = comments;
    int j = 1;
    for (int i = 1; comments[i] != '\0'; ++i) {
        if (comments[i - 1] == '\n') {
            lines[j] = &(comments[i]);
            ++j;
            comments[i - 1] = '\0';
        }
    }
    lines[num_of_lines] = nullptr;
    return 0;
}

char* Comments::GetComment()
{
    int n = rand() % num_of_lines;
    return lines[n];
}
