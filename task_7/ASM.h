#pragma once

#include "ProgLang.h"

void CProgLang::ToASM()
{
    char file[256];
    printf("Input path to file to write in.\n");
    scanf("%s", file);
    FILE* f = fopen(file, "w");
    int lab = 0;
    TreeToASM(f, root, -2, lab);
    fclose(f);
}

void CProgLang::TreeToASM(FILE* f, Node* node, int func, int &lab)
{
    switch (node->type) {
    case T_VAR:
        fprintf(f, "push [%d]\n", node->value);
        break;
    case T_NUM:
        fprintf(f, "push %d\n", node->value);
        break;
    case T_OP:
        if (node->value == OP_ASSIGN) {
            TreeToASM(f, node->childs[1], func, lab);
            fprintf(f, "pop [%d]\n", node->childs[0]->value);
            break;
        }
        TreeToASM(f, node->childs[0], func, lab);
        TreeToASM(f, node->childs[1], func, lab);
        switch (node->value) {
        case OP_ADD:
            fprintf(f, "add\n");
            break;
        case OP_SUB:
            fprintf(f, "sub\n");
            break;
        case OP_MUL:
            fprintf(f, "mul\n");
            break;
        case OP_DIV:
            fprintf(f, "div\n");
            break;
        }
        break;
    case T_SFUNC:
        switch (node->value) {
        case SF_IN:
            fprintf(f, "in\npop [%d]\n", node->childs[0]->value);
            break;
        case SF_OUT:
            TreeToASM(f, node->childs[0], func, lab);
            fprintf(f, "out\n");
            break;
        case SF_SQRT:
            fprintf(f, "push [%d]\nsqr\npop [%d]\n", node->childs[0]->value, node->childs[0]->value);
            break;
        case SF_CALL:
            fprintf(f, "call %s\n", funcs[node->childs[0]->value].c_str());
            break;
        default:
            printf("CProgLang::TreeToASM: Warning: Some unknown type of function.\n");
        }
        break;
    case T_LOG:
    {
        char while_label[] = "aaa";
        if (node->value == LOG_WHILE) {
            while_label[2] += lab;
            ++lab;
            fprintf(f, "label %s\n", while_label);
        }
        Node* cmp_node = node->childs[0]->childs[0];
        TreeToASM(f, cmp_node->childs[0], func, lab);
        fprintf(f, "pop rax\n");
        TreeToASM(f, cmp_node->childs[1], func, lab);
        fprintf(f, "pop rbx\ncmp rax rbx\n");
        char label[] = "aaa";
        label[2] += lab;
        ++lab;
        switch(cmp_node->value) {
        case SIGN_EQ:
            fprintf(f, "jne %s\n", label);
            break;
        case SIGN_NEQ:
            fprintf(f, "je %s\n", label);
            break;
        case SIGN_LOW:
            fprintf(f, "ja %s\n", label);
            break;
        case SIGN_GREAT:
            fprintf(f, "jb %s\n", label);
            break;
        }
        for (int i = 0; i < node->childs[1]->childs.size(); ++i) {
            TreeToASM(f, node->childs[1]->childs[i], func, lab);
        }
        if (node->value == LOG_WHILE) {
            fprintf(f, "jump %s\n", while_label);
        }
        fprintf(f, "label %s\n", label);
        break;
    }
    case T_UFUNC:
        fprintf(f, "label %s\n", funcs[node->value].c_str());
        for (int i = 1; i < node->childs.size(); ++i) {
            TreeToASM(f, node->childs[i], node->value, lab);
        }
        fprintf(f, "label %s_END\nret\n\n", funcs[node->value].c_str());
        break;
    case T_RET:
        if (func < 0) {
            fprintf(f, "jump MAIN_END\n");
        } else {
            fprintf(f, "jump %s_END\n", funcs[func].c_str());
        }
        break;
    case T_ROOT:
    {
        for (int i = 0; i < node->childs[1]->childs.size(); ++i) {
            TreeToASM(f, node->childs[1]->childs[i], func, lab);
        }

        fprintf(f, "begin\n");

        for (int i = 0; i < node->childs[0]->childs.size(); ++i) {
            fprintf(f, "push 0\npop [%d]\n", i);
        }

        Node* main_node = node->childs[2];
        for (int i = 0; i < main_node->childs.size(); ++i) {
            TreeToASM(f, main_node->childs[i], -1, lab);
        }

        fprintf(f, "label MAIN_END\nend\n");
        break;
    }
    }
}
