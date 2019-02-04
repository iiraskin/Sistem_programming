#pragma once

#include "ProgLang.h"

void CProgLang::Print()
{
    char file[256];
    printf("Input path to file to write in.\n");
    scanf("%s", file);
    FILE* f = fopen(file, "w");

    fprintf(f, "FUNCS %d\n", funcs.size());
    for (int i = 0; i < funcs.size(); ++i) {
        fprintf(f, "%s: PARAMS 0 NEWVAR 0\n", funcs[i].c_str());
    }

    fprintf(f, "VARS %d", vars.size());
    for (int i = 0; i < vars.size(); ++i) {
        fprintf(f, " %s", vars[i].c_str());
    }
    fprintf(f, "\n");

    PrintTree(f, root);
    fclose(f);
}

void CProgLang::PrintTree(FILE* f, Node* node)
{
    fprintf(f, "[%d %d", node->type, node->value);
    for (int i = 0; i < node->childs.size(); ++i) {
        PrintTree(f, node->childs[i]);
    }
    fprintf(f, "]");
}

int CProgLang::ScanNumber(char* str, int& i)
{
    int res = 0;
    bool is_below_zero = false;
    if (str[i] == '-') {
        is_below_zero = true;
        ++i;
    }
    for (i; str[i] >= '0' && str[i] <= '9'; ++i) {
        res = res * 10 + str[i] - '0';
    }
    if (is_below_zero) {
        res = -res;
    }
    return res;
}

bool CProgLang::TryScanWord(char* str, int& i, const char* word)
{
    for (int j = 0; word[j] != '\0' && str[i] != '\0'; ++j) {
        if (word[j] != str[i]) {
            return false;
        }
        ++i;
    }
    return true;
}

int CProgLang::Scan()
{
    DeleteTree(root);
    vars = {};
    funcs = {};

    char tree_file[256];
    printf("Input path to file with tree\n");
    scanf("%s", tree_file);
    int fd = open(tree_file, O_RDONLY);
    struct stat buff = {};

    if (fstat(fd, &buff)) {
        printf("CProgLang::ReadCode: Path to file is incorrect\n");
        return 1;
    }

    char* tree = new char[buff.st_size + 1]{CHAR_INIT};
    int code_len = read(fd, tree, buff.st_size);

    tree[code_len] = '\0';
    close(fd);

    int i = 0;
    SkipSpaces(tree, i);
    if (!TryScanWord(tree, i, "FUNCS")) {
        printf("Incorrect file.\n");
        return 1;
    }
    SkipSpaces(tree, i);
    int funcs_len = ScanNumber(tree, i);
    funcs.resize(funcs_len);
    SkipSpaces(tree, i);
    for (int j = 0; j < funcs_len; ++j) {
        funcs[j] = {};
        while (!IsSpace(tree[i]) && tree[i] != ':' && tree[i] != '\0') {
            funcs[j].push_back(tree[i]);
            ++i;
        }
        bool b = (tree[i] == ':');
        ++i;
        SkipSpaces(tree, i);
        b &= TryScanWord(tree, i, "PARAMS");
        SkipSpaces(tree, i);
        b &= TryScanWord(tree, i, "0");
        SkipSpaces(tree, i);
        b &= TryScanWord(tree, i, "NEWVAR");
        SkipSpaces(tree, i);
        b &= TryScanWord(tree, i, "0");
        SkipSpaces(tree, i);
        if (!b) {
            printf("Incorrect file.\n");
            return 1;
        }
    }

    if (!TryScanWord(tree, i, "VARS")) {
        printf("Incorrect file.\n");
        return 1;
    }
    SkipSpaces(tree, i);
    int vars_len = ScanNumber(tree, i);
    vars.resize(vars_len);
    SkipSpaces(tree, i);
    for (int j = 0; j < vars_len; ++j) {
        vars[j] = {};
        while (!IsSpace(tree[i]) && tree[i] != '\0') {
            vars[j].push_back(tree[i]);
            ++i;
        }
        SkipSpaces(tree, i);
    }
    if (ScanTree(tree, i, &root)) {
        return 1;
    }
    delete tree;
    return 0;
}

int CProgLang::ScanTree(char* tree, int &i, Node** node)
{
    if (tree[i] != '[') {
        printf("Incorrect file.\n");
        return 1;
    }
    ++i;
    (*node) = new Node();
    SkipSpaces(tree, i);
    (*node)->type = ScanNumber(tree, i);
    SkipSpaces(tree, i);
    (*node)->value = ScanNumber(tree, i);
    SkipSpaces(tree, i);
    while (tree[i] == '[') {
        Node* child;
        if (ScanTree(tree, i, &child)) {
            return 1;
        }
        (*node)->childs.push_back(child);
    }
    if (tree[i] != ']') {
        printf("Incorrect file.\n");
        return 1;
    }
    ++i;
    SkipSpaces(tree, i);
    return 0;
}

void CProgLang::ToCode()
{
    char file[256];
    printf("Input path to file to write in.\n");
    scanf("%s", file);
    FILE* f = fopen(file, "w");
    TreeToCode(f, root);
    fclose(f);
}

void CProgLang::TreeToCode(FILE* f, Node* node)
{
    switch (node->type) {
    case T_VAR:
        fprintf(f, "%s", vars[node->value].c_str());
        break;
    case T_NUM:
        if (node->value < 0) {
            fprintf(f, "~%d", -node->value);
        } else {
            fprintf(f, "%d", node->value);
        }
        break;
    case T_OP:
        if (node->value == OP_ASSIGN) {
            TreeToCode(f, node->childs[0]);
            fprintf(f, " := ", node->childs[0]->value);
            TreeToCode(f, node->childs[1]);
            fprintf(f, "\n");
            break;
        }
        TreeToCode(f, node->childs[0]);
        switch (node->value) {
        case OP_ADD:
            fprintf(f, " + ");
            break;
        case OP_SUB:
            fprintf(f, " - ");
            break;
        case OP_MUL:
            fprintf(f, " * ");
            break;
        case OP_DIV:
            fprintf(f, " / ");
            break;
        }
        TreeToCode(f, node->childs[1]);
        break;
    case T_SFUNC:
        switch (node->value) {
        case SF_IN:
            fprintf(f, "input(%s)\n", vars[node->childs[0]->value].c_str());
            break;
        case SF_OUT:
            fprintf(f, "output(");
            TreeToCode(f, node->childs[0]);
            fprintf(f, ")\n");
            break;
        case SF_SQRT:
            fprintf(f, "sqrt(%s)\n", vars[node->childs[0]->value].c_str());
            break;
        case SF_CALL:
            fprintf(f, "@ %s\n", funcs[node->childs[0]->value].c_str());
            break;
        default:
            printf("CProgLang::TreeToASM: Warning: Some unknown type of function.\n");
        }
        break;
    case T_LOG:
    {
        if (node->value == LOG_WHILE) {
            fprintf(f, "while (");
        } else {
            fprintf(f, "if (");
        }
        Node* cmp_node = node->childs[0]->childs[0];
        TreeToCode(f, cmp_node->childs[0]);
        switch(cmp_node->value) {
        case SIGN_EQ:
            fprintf(f, " = ");
            break;
        case SIGN_NEQ:
            fprintf(f, " # ");
            break;
        case SIGN_LOW:
            fprintf(f, " < ");
            break;
        case SIGN_GREAT:
            fprintf(f, " > ");
            break;
        }
        TreeToCode(f, cmp_node->childs[1]);
        fprintf(f, ") {\n");
        for (int i = 0; i < node->childs[1]->childs.size(); ++i) {
            TreeToCode(f, node->childs[1]->childs[i]);
        }
        fprintf(f, "}\n");
        break;
    }
    case T_UFUNC:
        fprintf(f, "%s\n{\n", funcs[node->value].c_str());
        for (int i = 1; i < node->childs.size(); ++i) {
            TreeToCode(f, node->childs[i]);
        }
        fprintf(f, "}\n");
        break;
    case T_RET:
        fprintf(f, "return\n");
        break;
    case T_ROOT:
    {
        for (int i = 0; i < node->childs[1]->childs.size(); ++i) {
            TreeToCode(f, node->childs[1]->childs[i]);
        }

        fprintf(f, "MAIN\n{\n");

        Node* main_node = node->childs[2];
        for (int i = 0; i < main_node->childs.size(); ++i) {
            TreeToCode(f, main_node->childs[i]);
        }

        fprintf(f, "}\n");
        break;
    }
    }
}
