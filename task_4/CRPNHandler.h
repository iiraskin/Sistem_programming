#pragma once

#include <vector>
#include <unordered_map>
#include "CStack.h"
#include "RAM.h"

class CRPNHandler
{
//Constants
private:
    const char CHAR_INIT = 20;
    const char spaces[4] = {' ', '\n', '\t', '\r'};
    const int ZERO_CODE = '0';
    const int NINE_CODE = '9';

    enum commands {
        #define DEF_CMD(name, n, str, func_title, func_with_args, parser, printer) \
        CMD_##name = (n),
        #include "commands/comms_with_1_args.h"
        #include "commands/comms_with_1_args_without_word.h"
        #include "commands/comms_with_0_args.h"
        #include "commands/comms_with_2_args.h"
        #include "commands/comms_spec_with_0_args.h"
        #include "commands/comms_spec_with_1_args.h"
        #undef DEF_CMD

        #define DEF_CMD(name, n, str, cond) \
        CMD_##name = (n),
        #include "commands/comms_jump.h"
        #undef DEF_CMD
        DUMMY_CMD
    };

    enum registers {
        #define DEF_R(name, n, var_name, str) \
        R##name = (n),
        #include "commands/registers.h"
        #undef DEF_R
        DUMMY_R
    };

    enum compares {EQUAL, LESS, MORE, UNDEFINED};

private:
    std::vector<int> comm_buff = {};
    CStack<int> st;

    #define DEF_R(name, n, var_name, str) \
    int var_name = 0;
    #include "commands/registers.h"
    #undef DEF_R

    RAM ram;
    CStack<int> func_stack;
    int cmp_res = UNDEFINED;

public:
    void Work();
    int Convert();

private:
    int ReadCommands();

    int ParserNumbers(const char* commands, const int commands_len);
    int ParserWords(const char* commands, const int commands_len);
    int ParserCodes(const char* commands, const int commands_len);

    int ParseNumber(int& i, const char* commands, const int commands_len);
    int ParseRegister(int& i, const char* commands, const int commands_len);
    int ReadLabel(int& i, const char* commands, const int commands_len, std::string& lab);
    int ParseNumberCode(int& i, const char* commands, const int commands_len);
    bool IsSpace(char c) const;

    int PrintNumbers(FILE* fd);
    int PrintWords(FILE* fd);
    int PrintCodes(FILE* fd);

    int PrintNumberCode(FILE* fd, int& i);
    int PrintRegister(FILE* fd, int& i);

    int Handling();
    #define DEF_CMD(name, n, str, func_title, func_with_args, parser, printer) \
    int func_with_args;
    #include "commands/comms_with_1_args.h"
    #include "commands/comms_with_1_args_without_word.h"
    #include "commands/comms_with_0_args.h"
    #include "commands/comms_with_2_args.h"
    #undef DEF_CMD
    int Arg_for_cmp(int& i, int& var);
};
