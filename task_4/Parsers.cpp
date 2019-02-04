#pragma once

#include <unordered_map>
#include "CRPNHandler.h"

#define SKIP_SPACES while (IsSpace(commands[i]) && i < commands_len) {++i;}

#define PARSER_ERROR(...) \
printf(__VA_ARGS__); \
return 1;

#define CHECK_COMMS_END(...) \
if (i == commands_len - 1) { \
    PARSER_ERROR(__VA_ARGS__) \
}

int CRPNHandler::ParserNumbers(const char* commands, const int commands_len)
{
    comm_buff = {};
    int i = 0;
    int num = 0;
    bool sign = false;

    SKIP_SPACES

    while (i < commands_len) {

        if (commands[i] == '-') {
            sign = true;
            ++i;
        }

        while (!IsSpace(commands[i]) && i < commands_len) {
            int n = static_cast<int>(commands[i]);
            if (n < ZERO_CODE || n > NINE_CODE) {
                PARSER_ERROR("CRPNHandler::ParserNumbers: Incorrect input symbol %c\n", commands[i])
            }
            num = num * 10 +  n - ZERO_CODE;
            ++i;
        }

        if (sign) {
            num = -num;
        }
        comm_buff.push_back(num);
        num = 0;
        sign = false;

        SKIP_SPACES
    }
    return 0;
}

int CRPNHandler::ParserWords(const char* commands, const int commands_len)
{
    comm_buff = {};
    std::unordered_map<std::string, int> labels = {};
    int i = 0;

    SKIP_SPACES

    while (i < commands_len) {

        std::string com = {};
        for (i; !IsSpace(commands[i]) && i < commands_len; ++i) {
            com += commands[i];
        }

        #define DEF_CMD(name, n, str, func_title, func_with_args, parser, printer) \
        if (com == str) { \
            parser \
        } else
        #include "commands/comms_with_0_args.h"
        #include "commands/comms_with_1_args.h"
        #include "commands/comms_with_2_args.h"
        #include "commands/comms_spec_with_0_args.h"
        #include "commands/comms_spec_with_1_args.h"
        #undef DEF_CMD

        #define DEF_CMD(name, n, str, cond) \
        if (com == str) { \
            comm_buff.push_back(CMD_##name); \
            std::string lab = {}; \
            if (ReadLabel(i, commands, commands_len, lab)) { \
                PARSER_ERROR("CRPNHandler::ParserWords: Error in CRPNHandler::ReadLabel\n") \
            } \
            if (labels.find(lab) == labels.end()) { \
                labels.insert({lab, i});  \
                comm_buff.push_back(i); \
            } else { \
                comm_buff.push_back(labels[lab]); \
            } \
        } else
        #include "commands/comms_jump.h"
        #undef DEF_CMD
        {
            PARSER_ERROR("CRPNHandler::ParserWords: Command %s does not exist\n", com)
        }

        SKIP_SPACES
    }
    return 0;
}

int CRPNHandler::ParserCodes(const char* commands, const int commands_len)
{
    if (commands_len < 4 || commands[0] != 'I' || commands[1] != 'R' || commands[2] != 'A' || commands[3] != '0') {
        PARSER_ERROR("CRPNHandler::ParserCodes: Incorrect input file: incorrect signature\n")
    }
    comm_buff = {};

    for (int i = 4; i < commands_len; ++i) {
        int num = static_cast<int>(commands[i]);
        comm_buff.push_back(num);

        if (
        #define DEF_CMD(name, n, str, func_title, func_with_args, parser, printer) \
        comm_buff[i] == CMD_##name ||
        #include "commands/comms_with_1_args.h"
        #include "commands/comms_with_2_args.h"
        #include "commands/comms_spec_with_1_args.h"
        #undef DEF_CMD

        #define DEF_CMD(name, n, str, cond) \
        comm_buff[i] == CMD_##name ||
        #include "commands/comms_jump.h"
        #undef DEF_CMD
        0) {
            if (ParseNumberCode(i, commands, commands_len)) {
                PARSER_ERROR("CRPNHandler::ParserCodes: Error in CRPNHandler::ParseNumberCode\n")
            }
        }

        if (
        #define DEF_CMD(name, n, str, func_title, func_with_args, parser, printer) \
        comm_buff[i] == CMD_##name ||
        #include "commands/comms_with_2_args.h"
        #undef DEF_CMD
        0) {
            if (ParseNumberCode(i, commands, commands_len)) {
                PARSER_ERROR("CRPNHandler::ParserCodes: Error in CRPNHandler::ParseNumberCode\n")
            }
        }
    }
    return 0;
}

bool CRPNHandler::IsSpace(char c) const
{
    for (space: spaces) {
        if (c == space) {
            return true;
        }
    }
    return false;
}

int CRPNHandler::ParseNumber(int& i, const char* commands, const int commands_len)
{
    SKIP_SPACES

    bool sign = false;
    if (commands[i] == '-') {
        sign = true;
        ++i;
    }

    CHECK_COMMS_END("CRPNHandler::ParseNumber: Command 'push' does not has an argument\n")

    int num = 0;
    for (i; !IsSpace(commands[i]) && commands[i] != ']' && i < commands_len; ++i) {
        int n = static_cast<int>(commands[i]);

        if (n < ZERO_CODE || n > NINE_CODE) {
            PARSER_ERROR("CRPNHandler::ParseNumber: Incorrect argument for command 'push' with %c\n", commands[i])
        }

        num = num * 10 +  n - ZERO_CODE;
    }

    if (sign) {
        num = -num;
    }

    comm_buff.push_back(num);
    return 0;
}

int CRPNHandler::ParseRegister(int& i, const char* commands, const int commands_len)
{
    SKIP_SPACES
    std::string r = {};
    for (i; !IsSpace(commands[i]) && i < commands_len; ++i) {
        r += commands[i];
    }
    #define DEF_R(name, n, var_name, str) \
    if (r == (str)) { \
        comm_buff.push_back(R##name); \
    } else
    #include "commands/registers.h"
    #undef DEF_R
    {
        PARSER_ERROR("CRPNHandler::ParseRegister: Unknown argument %s\n", r)
    }
    return 0;
}

int CRPNHandler::ReadLabel(int& i, const char* commands, const int commands_len, std::string& lab)
{
    SKIP_SPACES

    CHECK_COMMS_END("CRPNHandler::ParseReadLabel: Command 'label' does not has an argument\n")

    for (i; !IsSpace(commands[i]) && i < commands_len; ++i) {
        lab += commands[i];
    }
    return 0;
}

int CRPNHandler::ParseNumberCode(int& i, const char* commands, const int commands_len)
{
    if (i > commands_len - 4) {
        PARSER_ERROR("CRPNHandler::ParserCodes: Command does not has an argument\n")
    }

    int value = (int)((unsigned char)(commands[++i]) | (unsigned char)(commands[++i]) << 8 |
                      (unsigned char)(commands[++i]) << 16 | (unsigned char)(commands[++i]) << 24);
    comm_buff.push_back(value);
    return 0;
}
