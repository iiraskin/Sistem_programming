#pragma once

#include <iostream>
#include <sys\stat.h>
#include <fcntl.h>
#include "ProgLang.h"

const char CHAR_INIT = 20;

bool CProgLang::InWord(char c)
{
    return (c >= '0' && c <= '9' ||
            c >= 'a' && c <= 'z' ||
            c >= 'A' && c <= 'Z' ||
            c == '_' || c == '~');
}

int CProgLang::MakeNodes(vector<Node*> &nodes, char* code)
{
    std::unordered_map<string, int> variables;
    std::unordered_map<string, int> functions;

    nodes.resize(0);
    string word = {};
    int var_counter = 0;
    int func_counter = 0;

    int i = 0;
    SkipSpaces(code, i);

    for (i; code[i] != '\0'; ++i) {
        if (word == "") {
            SkipSpaces(code, i);
            if (code[i] == '|') {
                while (code[i] != '\0' &&code[i] != '\n') {++i;}
            }
            SkipSpaces(code, i);
        }

        char c = code[i];
        if (!IsSpace(c)) {
            word.push_back(c);
            if (code[i] == ':' && code[i + 1] == '=') {
                word.push_back(code[i + 1]);
                ++i;
            }
            if (InWord(code[i]) && InWord(code[i + 1])) {
                continue;
            }
        }

        Node* new_node = nullptr;

        if (word == ":=") {
            new_node = new Node(T_OP, OP_ASSIGN);
        } else if (word == "+") {
            new_node = new Node(T_OP, OP_ADD);
        } else if (word == "-") {
            new_node = new Node(T_OP, OP_SUB);
        } else if (word == "*") {
            new_node = new Node(T_OP, OP_MUL);
        } else if (word == "/") {
            new_node = new Node(T_OP, OP_DIV);
        } else if (word == "=") {
            new_node = new Node(T_OP, SIGN_EQ);
        } else if (word == "#") {
            new_node = new Node(T_OP, SIGN_NEQ);
        } else if (word == "<") {
            new_node = new Node(T_OP, SIGN_LOW);
        } else if (word == ">") {
            new_node = new Node(T_OP, SIGN_GREAT);
        } else if (word == "(") {
            new_node = new Node(T_BRAC, BR_LP);
        } else if (word == ")") {
            new_node = new Node(T_BRAC, BR_RP);
        } else if (word == "{") {
            new_node = new Node(T_BRAC, BR_LCB);
        } else if (word == "}") {
            new_node = new Node(T_BRAC, BR_RCB);
        } else if (word == "if") {
            new_node = new Node(T_LOG, LOG_IF);
        } else if (word == "while") {
            new_node = new Node(T_LOG, LOG_WHILE);
        } else if (word == "return") {
            new_node = new Node(T_RET);
        } else if (word == "input") {
            new_node = new Node(T_SFUNC, SF_IN);
        } else if (word == "output") {
            new_node = new Node(T_SFUNC, SF_OUT);
        } else if (word == "@") {
            new_node = new Node(T_SFUNC, SF_CALL);
        } else if (word == "sqrt") {
            new_node = new Node(T_SFUNC, SF_SQRT);
        } else if (word == "MAIN") {
            new_node = new Node(T_MAIN);
        } else if (word[0] >= 'a' && word[0] <= 'z') {
            if (variables.find(word) == variables.end()) {
                variables.insert({word, var_counter});
                new_node = new Node(T_VAR, var_counter);
                ++var_counter;
            } else {
                new_node = new Node(T_VAR, variables[word]);
            }
        } else if (word[0] >= 'A' && word[0] <= 'Z') {
            if (functions.find(word) == functions.end()) {
                functions.insert({word, func_counter});
                new_node = new Node(T_UFUNC, func_counter);
                ++func_counter;
            } else {
                new_node = new Node(T_UFUNC, functions[word]);
            }
        } else {
            if (word == "") {
                return 1;
            }
            int num = 0;
            int is_below_zero = 0;
            if (word[0] == '~') {
                is_below_zero = 1;
            }
            for (int j = is_below_zero; j < word.size(); ++j) {

                if (word[j] < '0' || word[j] > '9') {
                    char word1[256];
                    for (int k = 0; k < word.size(); ++k) {
                        word1[k] = word[k];
                    }
                    word1[word.size()] = '\0';
                    printf("CProgLang::MakeNodes: Incorrect input: %s\n", word1);
                    return 1;
                }

                num = num * 10 + word[j] - '0';
            }
            if (is_below_zero) {
                num = -num;
            }
            new_node = new Node(T_NUM, num);
        }

        nodes.push_back(new_node);
        word = {};
    }

    vars.resize(variables.size());
    for (auto const func: variables) {
        vars[func.second] = func.first;
    }

    funcs.resize(functions.size());
    for (auto const func: functions) {
        funcs[func.second] = func.first;
    }

    return 0;
}

int CProgLang::ReadCode(char** code)
{
    if (*code != nullptr) {
        printf("CProgLang::ReadCode: Warning: code != nullptr\n");
    }

    char code_file[256];
    printf("Input path to file with code\n");
    scanf("%s", code_file);
    int fd = open(code_file, O_RDONLY);
    struct stat buff = {};

    if (fstat(fd, &buff)) {
        printf("CProgLang::ReadCode: Path to file is incorrect\n");
        return 1;
    }

    *code = new char[buff.st_size + 1]{CHAR_INIT};
    int code_len = read(fd, *code, buff.st_size);

    (*code)[code_len] = '\0';
    close(fd);
    return 0;
}
