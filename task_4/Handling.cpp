#pragma once

#include <cmath>
#include <unordered_map>
#include "CRPNHandler.h"

#define HANDLING_ERROR(...) \
printf(__VA_ARGS__); \
return 1;

int CRPNHandler::Handling()
{
    std::unordered_map<int, int> labels = {};

    for (int j = 0; j < comm_buff.size(); ++j) {
        if (comm_buff[j] == CMD_LABEL) {
            int lab = comm_buff[++j];

            if (labels.find(lab) != labels.end()) {
                HANDLING_ERROR("CRPNHandler::LabelHandling: This label is not unique: %s\n", lab)
            }

            labels.insert({lab, j});
        }
    }

    int i = 0;
    for (i; i < comm_buff.size() && comm_buff[i] != CMD_BEGIN; ++i) {}
    ++i;

    for (i; i < comm_buff.size(); ++i)
    {
        switch (comm_buff[i]) {
        #define DEF_CMD(name, n, str, cond) \
        case CMD_##name:
        #include "commands/comms_jump.h"
        #undef DEF_CMD

        #define DEF_CMD(name, n, str, func_title, func_with_args, parser, printer) \
        case CMD_##name: \
            if ( func_title ) { \
                HANDLING_ERROR("CRPNHandler::Handling: Error in command handling function\n") \
            } \
            break;
        #include "commands/comms_with_1_args.h"
        #include "commands/comms_with_1_args_without_word.h"
        #include "commands/comms_with_0_args.h"
        #include "commands/comms_with_2_args.h"
        #undef DEF_CMD
        case CMD_LABEL:
            ++i;
            break;
        case CMD_BEGIN:
            HANDLING_ERROR("CRPNHandler::Handling: Error: unexpected command 'begin'\n")
        case CMD_END:
            return 0;
        default:
            HANDLING_ERROR("CRPNHandler::Handling: Error: command %d does not exist\n", comm_buff[i]);
        }
    }

    HANDLING_ERROR("CRPNHandler::Handling: Error: command 'begin' or 'end' is missed\n")
}

int CRPNHandler::Arg_for_cmp(int& i, int& var) {
    if (i == comm_buff.size() - 1) {
        HANDLING_ERROR("CRPNHandler::PopRHandling: Error: command 'push' does not has an argument\n")
    }
    switch (comm_buff[++i]) {
        #define DEF_R(name, n, var_name, str) \
        case R##name: \
            var = var_name; \
            break;
        #include "commands/registers.h"
        #undef DEF_R
        default:
            HANDLING_ERROR("CRPNHandler::PopRHandling: Error: push argument %d does not exist\n", comm_buff[i])
    }
    return 0;
}

int CRPNHandler::AddHandling()
{
    int a = 0;
    int b = 0;
    if (!st.Pop(a)) {
        HANDLING_ERROR("CRPNHandler::AddHandling: Error: perhaps stack is empty (first argument)\n")
    }
    if (!st.Pop(b)) {
        HANDLING_ERROR("CRPNHandler::AddHandling: Error: perhaps stack is empty (second argument)\n")
    }
    return 1 - static_cast<int>(st.Push(a + b));
}

int CRPNHandler::SubHandling()
{
    int a = 0, b = 0;
    if (!st.Pop(a)) {
        HANDLING_ERROR("CRPNHandler::SubHandling: Error: perhaps stack is empty (first argument)\n")
    }
    if (!st.Pop(b)) {
        HANDLING_ERROR("CRPNHandler::SubHandling: Error: perhaps stack is empty (second argument)\n")
    }
    return 1 - static_cast<int>(st.Push(b - a));
}

int CRPNHandler::MulHandling()
{
    int a = 0, b = 0;
    if (!st.Pop(a)) {
        HANDLING_ERROR("CRPNHandler::MulHandling: Error: perhaps stack is empty (first argument)\n")
    }
    if (!st.Pop(b)) {
        HANDLING_ERROR("CRPNHandler::MulHandling: Error: perhaps stack is empty (second argument)\n")
    }
    return 1 - static_cast<int>(st.Push(a * b));
}

int CRPNHandler::DivHandling()
{
    int a = 0, b = 0;
    if (!st.Pop(a)) {
        HANDLING_ERROR("CRPNHandler::DivHandling: Error: perhaps stack is empty (first argument)\n")
    }
    if (!st.Pop(b)) {
        HANDLING_ERROR("CRPNHandler::DivHandling: Error: perhaps stack is empty (second argument)\n")
    }
    if (a == 0) {
        HANDLING_ERROR("CRPNHandler::DivHandling: Error: divider is 0\n")
    }
    return 1 - static_cast<int>(st.Push(b / a));
}

int CRPNHandler::SqrHandling()
{
    int a = 0;
    if (!st.Pop(a)) {
        HANDLING_ERROR("CRPNHandler::SqrHandling: Error: perhaps stack is empty\n")
    }
    if (a < 0) {
        HANDLING_ERROR("CRPNHandler::SqrHandling: Error: argument is below 0\n")
    }
    return 1 - static_cast<int>(st.Push(round(sqrt(a))));
}

int CRPNHandler::InHandling()
{
    int a = 0;
    printf("Please input an argument\n");
    scanf("%d", &a);
    return 1 - static_cast<int>(st.Push(a));
}

int CRPNHandler::OutHandling()
{
    int a = 0;
    if (!st.Pop(a)) {
        HANDLING_ERROR("CRPNHandler::OutHandling: Error: perhaps stack is empty\n")
    }
    printf("Result: %d\n", a);
    return 0;
}

int CRPNHandler::PushHandling(int& i)
{
    if (i == comm_buff.size() - 1) {
        HANDLING_ERROR("CRPNHandler::PushHandling: Error: command 'push' does not has an argument\n")
    }
    return 1 - static_cast<int>(st.Push(comm_buff[++i]));
}

int CRPNHandler::PushRHandling(int& i)
{
    if (i == comm_buff.size() - 1) {
        HANDLING_ERROR("CRPNHandler::PushRHandling: Error: command 'push' does not has an argument\n")
    }
    switch (comm_buff[++i]) {
    #define DEF_R(name, n, var_name, str) \
    case R##name: \
        st.Push(var_name); \
        break;
    #include "commands/registers.h"
    #undef DEF_R
    default:
        HANDLING_ERROR("CRPNHandler::PushRHandling: Error: push argument %d does not exist\n", comm_buff[i])
    }
    return 0;
}

int CRPNHandler::PopRHandling(int& i)
{
    if (i == comm_buff.size() - 1) {
        HANDLING_ERROR("CRPNHandler::PopRHandling: Error: command 'push' does not has an argument\n")
    }
    switch (comm_buff[++i]) {
    #define DEF_R(name, n, var_name, str) \
    case R##name: \
        st.Pop(var_name); \
        break;
    #include "commands/registers.h"
    #undef DEF_R
    default:
        HANDLING_ERROR("CRPNHandler::PopRHandling: Error: push argument %d does not exist\n", comm_buff[i])
    }
    return 0;
}

int CRPNHandler::PushRAMHandling(int& i)
{
    if (i == comm_buff.size() - 1) {
        HANDLING_ERROR("CRPNHandler::PushRAMHandling: Error: command 'push' does not has an argument\n")
    }

    int value = 0;
    if (ram.Read(comm_buff[++i], &value)) {
        HANDLING_ERROR("CRPNHandler::PushRAMHandling: Error in RAM::Read\n")
    }

    return 1 - static_cast<int>(st.Push((value)));
}

int CRPNHandler::PopRAMHandling(int& i)
{
    if (i == comm_buff.size() - 1) {
        HANDLING_ERROR("CRPNHandler::PushRAMHandling: Error: command 'push' does not has an argument\n")
    }

    int value = 0;
    if (!st.Pop(value)) {
        HANDLING_ERROR("CRPNHandler::PushRAMHandling: Error in my_stack::Pop\n")
    }

    return ram.Write(comm_buff[++i], value);
}

int CRPNHandler::CmpHandling(int& i)
{
    int a = 0, b = 0;

    if (Arg_for_cmp(i, a) || Arg_for_cmp(i, b)) {
        return 1;
    }

    if (a < b) {
        cmp_res = LESS;
    } else if (a > b) {
        cmp_res = MORE;
    } else {
        cmp_res = EQUAL;
    }
    return 0;
}

int CRPNHandler::JumpHandling(int& i, std::unordered_map<int, int>& labels, int cmd)
{
    if (cmd != CMD_JUMP && cmp_res == UNDEFINED) {
        HANDLING_ERROR("CRPNHandler::JumpHandling: Error: cmp has not been used yet\n")
    }

    ++i;
    switch (cmd) {
    #define DEF_CMD(name, n, str, cond) \
    case CMD_##name: \
        if (cmp_res cond) { \
            return 0; \
        } \
        break;
    #include "commands/comms_jump.h"
    #undef DEF_CMD
    }

    int lab = comm_buff[i];

    if (labels.find(lab) == labels.end()) {
        HANDLING_ERROR("CRPNHandler::JumpHandling: This label is missing: %s\n", lab)
    }

    i = labels[lab];
    return 0;
}

int CRPNHandler::CallHandling(int& i, std::unordered_map<int, int>& labels)
{
    int lab = comm_buff[++i];

    if (labels.find(lab) == labels.end()) {
        HANDLING_ERROR("CRPNHandler::JumpHandling: This label is not unique: %s\n", lab)
    }

    if (!func_stack.Push(i)) {
        HANDLING_ERROR("CRPNHandler::JumpHandling: Error in CStack.Push(): %s\n", lab)
    }
    i = labels[lab];
    return 0;
}

int CRPNHandler::RetHandling(int& i)
{
    if (!func_stack.Pop(i)) {
        HANDLING_ERROR("CRPNHandler::RetHandling: Error in CStack.Pop():\nMaybe the stack is empty because program is not in function\n")
    }
    return 0;
}

int CRPNHandler::MeowHandling()
{
    int fd = open("subsidiary/kitty.txt", O_RDONLY);
    struct stat buff = {};

    if (fstat(fd, &buff)) {
        printf("CRPNHandler::MeowHandling: Path to file is incorrect\n");
        return 1;
    }

    char* kitty = new char[buff.st_size + 1]{CHAR_INIT};
    int kitty_len = read(fd, kitty, buff.st_size);
    kitty[kitty_len] = '\0';
    close(fd);

    system("color F0");
    printf("%s\n", kitty);
}
