#pragma once

#include "CRPNHandler.h"

#define PARSER_ERROR(...) \
printf(__VA_ARGS__); \
return 1;

#define CHECK_BUFF_END(...) \
if (i == comm_buff.size() - 1) { \
    PARSER_ERROR(__VA_ARGS__) \
}

int CRPNHandler::PrintNumbers(FILE* fd)
{
    if (fd == nullptr) {
        PARSER_ERROR("CRPNHandler::PrintNumbers: Cannot open file for write\n")
    }

    for (int i = 0; i < comm_buff.size(); ++i) {
        fprintf(fd, "%d", comm_buff[i]);

        if (
        #define DEF_CMD(name, n, str, func_title, func_with_args, parser, printer) \
        comm_buff[i] == CMD_##name ||
        #include "commands/comms_with_1_args.h"
        #include "commands/comms_with_1_args_without_word.h"
        #include "commands/comms_with_2_args.h"
        #include "commands/comms_spec_with_1_args.h"
        #undef DEF_CMD

        #define DEF_CMD(name, n, str, cond) \
        comm_buff[i] == CMD_##name ||
        #include "commands/comms_jump.h"
        #undef DEF_CMD
        0) {
            if (i == comm_buff.size() - 1) {
                PARSER_ERROR("CRPNHandler::PrintNumbers: Incorrect command push (no argument)\n")
            }

            fprintf(fd, " %d", comm_buff[++i]);
        }

        if (
        #define DEF_CMD(name, n, str, func_title, func_with_args, parser, printer) \
        comm_buff[i] == CMD_##name ||
        #include "commands/comms_with_2_args.h"
        #undef DEF_CMD
        0) {
            if (i == comm_buff.size() - 1) {
                PARSER_ERROR("CRPNHandler::PrintNumbers: Incorrect command push (no argument)\n")
            }

            fprintf(fd, " %d", comm_buff[++i]);
        }

        fprintf(fd, "\n");
    }
    return 0;
}

int CRPNHandler::PrintWords(FILE* fd)
{
    if (fd == nullptr) {
        PARSER_ERROR("CRPNHandler::PrintNumbers: Cannot open file for write\n")
    }

    for (int i = 0; i < comm_buff.size(); ++i)
    {
        switch (comm_buff[i]){
        #define DEF_CMD(name, n, str, func_title, func_with_args, parser, printer) \
        case CMD_##name: \
            printer \
            break;
        #include "commands/comms_with_0_args.h"
        #include "commands/comms_with_1_args.h"
        #include "commands/comms_with_1_args_without_word.h"
        #include "commands/comms_with_2_args.h"
        #include "commands/comms_spec_with_0_args.h"
        #include "commands/comms_spec_with_1_args.h"
        #undef DEF_CMD

        #define DEF_CMD(name, n, str, cond) \
        case CMD_##name: \
            CHECK_BUFF_END("CRPNHandler::PrintWords: Incorrect command jump (no argument)\n") \
            fprintf(fd, "%s %d\n", str, comm_buff[++i]); \
            break;
        #include "commands/comms_jump.h"
        #undef DEF_CMD

        default:
            PARSER_ERROR("CRPNHandler::PrintWords: Error: command %d does not exist\n", comm_buff[i])
        }
    }
    return 0;
}

int CRPNHandler::PrintCodes(FILE* fd)
{
    if (fd == nullptr) {
        PARSER_ERROR("CRPNHandler::PrintNumbers: Cannot open file for write\n")
    }

    fprintf(fd, "%s", "IRA0");  //Signature

    for (int i = 0; i < comm_buff.size(); ++i) {
        fprintf(fd, "%c", static_cast<char>(comm_buff[i]));

        if (
        #define DEF_CMD(name, n, str, func_title, func_with_args, parser, printer) \
        comm_buff[i] == CMD_##name ||
        #include "commands/comms_with_1_args.h"
        #include "commands/comms_with_1_args_without_word.h"
        #include "commands/comms_with_2_args.h"
        #include "commands/comms_spec_with_1_args.h"
        #undef DEF_CMD

        #define DEF_CMD(name, n, str, cond) \
        comm_buff[i] == CMD_##name ||
        #include "commands/comms_jump.h"
        #undef DEF_CMD
        0) {
            if (PrintNumberCode(fd, i)) {
                return 1;
            }
        }

        if (
        #define DEF_CMD(name, n, str, func_title, func_with_args, parser, printer) \
        comm_buff[i] == CMD_##name ||
        #include "commands/comms_with_2_args.h"
        #undef DEF_CMD
        0) {
            if (PrintNumberCode(fd, i)) {
                return 1;
            }
        }
    }
    return 0;
}

int CRPNHandler::PrintNumberCode(FILE* fd, int& i)
{
    CHECK_BUFF_END("CRPNHandler::PrintNumbers: Incorrect command push (no argument)\n")

    int value = comm_buff[++i];
    for (int j = 3; j >= 0; --j) {
        fprintf(fd, "%c", static_cast<char>(value << (8 * j) >> 24));
    }
    return 0;
}

int CRPNHandler::PrintRegister(FILE* fd, int& i)
{
    switch (comm_buff[++i]) {
    #define DEF_R(name, n, var_name, str) \
    case R##name: \
        fprintf(fd, "%s ", str); \
        break;
    #include "commands/registers.h"
    #undef DEF_R
    default:
        PARSER_ERROR("CRPNHandler::PrintWords: Error: pop argument %d does not exist\n", comm_buff[i])
    }
    return 0;
}
