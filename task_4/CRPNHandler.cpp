#pragma once

#include <sys\stat.h>
#include <fcntl.h>
#include "CRPNHandler.h"
#include "Handling.cpp"
#include "Parsers.cpp"
#include "Printers.cpp"

int CRPNHandler::ReadCommands()
{
    int file_flag = -1;
    printf("Input the type of input file:\n\t0 if commands are numbers,\n\t1 if commands are words\n\t2 if commands are code\n");
    scanf("%d", &file_flag);

    char com_file[256];
    printf("Input path to file with commands\n");
    scanf("%s", com_file);
    int fd = open(com_file, O_RDONLY);
    struct stat buff = {};

    if (fstat(fd, &buff)) {
        printf("CRPNHandler::ReadCommands: Path to file is incorrect\n");
        return 1;
    }

    char* commands = new char[buff.st_size + 1]{CHAR_INIT};
    int commands_len = read(fd, commands, buff.st_size);
    commands[commands_len] = '\0';
    close(fd);

    #define READ_COMMANDS_CHECK(func, message) \
    if (func(commands, commands_len)) { \
        printf(message); \
        delete[] commands; \
        return 1; \
    }

    switch (file_flag) {
    case 0:
        READ_COMMANDS_CHECK(ParserNumbers, "CRPNHandler::ReadCommands: Error in CRPNHandler::ParserNumbers\n")
        break;
    case 1:
        READ_COMMANDS_CHECK(ParserWords, "CRPNHandler::ReadCommands: Error in CRPNHandler::ParserWords\n")
        break;
    case 2:
        READ_COMMANDS_CHECK(ParserCodes, "CRPNHandler::ReadCommands: Error in CRPNHandler::ParseCodes\n")
        break;
    default:
        printf("CRPNHandler::ReadCommands: Error: unknown type of input file %d\n", file_flag);
        delete[] commands;
        return 1;
    }

    #undef READ_COMMANDS_CHECK

    delete[] commands;
    return 0;
}

void CRPNHandler::Work() {
    printf("Work started.\n");

    while (true) {
        if (ReadCommands()) {
            continue;
        }
        Handling();
    }
}

int CRPNHandler::Convert() {
    printf("Convert started.\n");

    if (ReadCommands()) {
        printf("CRPNHandler::Convert: Error in CRPNHandler::PrintNumbers\n");
        return 1;
    }

    int out_type = -1;
    char out_file[256];
    printf("Input result type:\n\t0 if commands are numbers,\n\t1 if commands are words\n\t2 if commands are code\n");
    scanf("%d", &out_type);
    printf("Input path to output file\n");
    scanf("%s", &out_file);

    #define CONVERT_CHECK(func, message) \
    if (func(fd)) { \
        printf(message); \
        fclose(fd); \
        return 1; \
    }

    FILE* fd = fopen(out_file, "w");
    switch (out_type) {
    case 0:
        CONVERT_CHECK(PrintNumbers, "CRPNHandler::Convert: Error in CRPNHandler::PrintNumbers\n")
        break;
    case 1:
        CONVERT_CHECK(PrintWords, "CRPNHandler::Convert: Error in CRPNHandler::PrintWords\n")
        break;
    case 2:
        CONVERT_CHECK(PrintCodes, "CRPNHandler::Convert: Error in CRPNHandler::PrintCodes\n")
        break;
    default:
        printf("CRPNHandler::Convert: Error: unknown type of input file %d\n", out_type);
        fclose(fd);
        return 1;
    }

    #undef CONVERT_CHECK

    fclose(fd);
    return 0;
}
