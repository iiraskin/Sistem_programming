#pragma once

#include <iostream>
#include <sys\stat.h>
#include <fcntl.h>
#include <cctype>

#include "formula.h"
#include "tex.cpp"

CFormulaTree::CFormulaTree() : comments("comments.txt") {}

CFormulaTree::CFormulaTree(FormulaNode* _root) : comments("comments.txt")
{
    root = _root;
}

CFormulaTree::~CFormulaTree()
{
    DeleteTree(root);
}

void CFormulaTree::DeleteTree(FormulaNode* _root)
{
    if (_root) {
        DeleteTree(_root->left);
        DeleteTree(_root->right);
        delete _root;
    }
}

int CFormulaTree::ReadFormula()
{
    char com_file[256];
    printf("Input path to file with formula\n");
    scanf("%s", com_file);
    int fd = open(com_file, O_RDONLY);
    struct stat buff = {};

    if (fstat(fd, &buff)) {
        printf("CFormulaTree::ReadFormula: Path to file is incorrect\n");
        return 1;
    }

    char* formula_row = new char[buff.st_size + 1]{CHAR_INIT};
    int formula_row_len = read(fd, formula_row, buff.st_size);
    formula_row[formula_row_len] = '\0';
    close(fd);

    char* formula = new char[formula_row_len]{CHAR_INIT};
    int formula_len = 0;
    for (int i = 0; i < formula_row_len; ++i) {
        if (formula_row[i] != ' ') {
            formula[formula_len++] = formula_row[i];
        }
    }

    formula[formula_len] = '\0';
    delete[] formula_row;

    int pos = 0;
    root = MakeTree(pos, formula);
    if (formula[pos] != '\0') {
        printf("Warning: incorrect formula. Position %d. End of formula was expected.\n", pos);
    }
    delete[] formula;
    return root ? 0 : 1;
}

FormulaNode* CFormulaTree::MakeTree(int &pos, const char* formula)
{
    if (formula[pos] != '(') {
        printf("Incorrect syntax. Position %d. '(' was expected.\n", pos);
        return nullptr;
    }

    char c = formula[++pos];

    if (isdigit(c)) {
        FormulaNode* new_node = new FormulaNode(true, 0);
        while (isdigit(c)) {
            new_node->value = new_node->value * 10 + c - '0';
            c = formula[++pos];
        }
        if (c != ')') {
            printf("Incorrect syntax. Position %d. ')' was expected.\n", pos);
            delete new_node;
            return nullptr;
        }
        ++pos;
        return new_node;
    }

    #define DEF_VAR(name, value) \
    if (c == value) { \
        FormulaNode* new_node = new FormulaNode(false, VAR_##name); \
        ++pos; \
        if (formula[pos++] != ')') { \
            printf("Incorrect syntax. Position %d. ')' was expected.\n", pos); \
            delete new_node; \
            return nullptr; \
        } \
        return new_node; \
    }
    #include "variables.h"
    #undef DEF_VAR

    if (c == '(') {
        FormulaNode* new_node = new FormulaNode(false, 0);
        FormulaNode* left = MakeTree(pos, formula);

        if (left) {
            new_node->left = left;
        } else {
            delete new_node;
            return nullptr;
        }

        #define DEF_OP(name, val, num, diff, opt) \
        if (formula[pos] == val) { \
            new_node->value = OP_##name; \
        } else
        #include "operands.h"
        #undef DEF_OP
        {
            printf("Incorrect syntax. Position %d. Operand was expected.\n", pos);
            delete new_node;
            return nullptr;
        }

        if (formula[++pos] != '(') {
            printf("Incorrect syntax. Position %d. '(' was expected.\n", pos);
            delete new_node;
            return nullptr;
        }

        FormulaNode* right = MakeTree(pos, formula);
        if (right) {
            new_node->right = right;
        } else {
            delete new_node;
            return nullptr;
        }

        if (formula[pos++] != ')') {
            printf("Incorrect syntax. Position %d. ')' was expected.\n", pos);
            delete new_node;
            return nullptr;
        }
        return new_node;
    }

    FormulaNode* new_node = new FormulaNode(true, 0);
    std::string com = {};
    com += c;
    if (c != '-') {
        com += formula[++pos];
        com += formula[++pos];
    }

    #define DEF_COM(name, val, num, diff, opt) \
    if (com == val) { \
        new_node->value = COM_##name; \
    } else
    #include "commands.h"
    #undef DEF_COM
    {
        printf("Incorrect syntax. Position %d. function was expected.\n", pos - 2);
        delete new_node;
        return nullptr;
    }
    ++pos;

    if (formula[pos] != '(') {
        printf("Incorrect syntax. Position %d. '(' was expected.\n", pos);
        delete new_node;
        return nullptr;
    }

    FormulaNode* left = MakeTree(pos, formula);
    if (left) {
        new_node->left = left;
    } else {
        delete new_node;
        return nullptr;
    }

    if (formula[pos++] != ')') {
        printf("Incorrect syntax. Position %d. ')' was expected.\n", pos);
        delete new_node;
        return nullptr;
    }
    return new_node;
}

CFormulaTree CFormulaTree::Differentiation(FILE* f)
{
    FormulaNode* _root = DiffTree(root, f);
    if (_root) {
        return CFormulaTree(_root);
    }
    return nullptr;
}

FormulaNode* CFormulaTree::DiffTree(FormulaNode* _root, FILE* f)
{
    if (_root->IsLeaf()) {
        if (!_root->is_number_or_func && _root->value == VAR_X) {
            FormulaNode* new_node = new FormulaNode(true, 1);
            if (f) {
                TexDiff(f, _root, new_node);
            }
            return new_node;
        }

        FormulaNode* new_node = new FormulaNode(true, 0);
        if (f) {
            TexDiff(f, _root, new_node);
        }
        return new_node;
    }

    switch (_root->value) {
    #define DEF_COM(name, val, num, diff, opt) \
    case COM_##name: { \
        diff \
    }
    #include "commands.h"
    #undef DEF_COM
    #define DEF_OP(name, val, num, diff, opt) \
    case OP_##name: { \
        diff \
    }
    #include "operands.h"
    #undef DEF_OP
    default: {
        printf("CFormulaTree::DiffTree: Error. Unknown command.\n");
        return nullptr;
    }
    }
}

int CFormulaTree::PrintFormula()
{
    if (!PrintTree(root)) {
        printf("\n");
        return 0;
    }
    return 1;
}

int CFormulaTree::PrintTree(FormulaNode* _root)
{
    if (_root->IsLeaf()) {
        if (_root->is_number_or_func) {
            printf("(%d)", _root->value);
            return 0;
        }

        switch (_root->value) {
        #define DEF_VAR(name, value) \
        case VAR_##name: \
            printf("(x)"); \
            return 0;
        #include "variables.h"
        #undef DEF_VAR
        default:
            printf("CFormulaTree::PrintTree: Error. Unknown variable\n");
            return 1;
        }
    }

    if (_root->is_number_or_func) {
        switch (_root->value) {
        #define DEF_COM(name, val, num, diff, opt) \
        case COM_##name: \
            printf("(%s", val); \
            break;
        #include "commands.h"
        #undef DEF_COM
        default:
            printf("\nCFormulaTree::PrintTree: Error. Unknown function\n");
            return 1;
        }
        PrintTree(_root->left);
        printf(")");
        return 0;
    }

    printf("(");
    PrintTree(_root->left);

    switch (_root->value) {
    #define DEF_OP(name, val, num, diff, opt) \
    case OP_##name: \
        printf(" %c ", val); \
        break;
    #include "operands.h"
    #undef DEF_OP
    default:
        printf("\nCFormulaTree::PrintTree: Error. Unknown operand\n");
        return 1;
    }

    PrintTree(_root->right);
    printf(")");
    return 0;
}

int CFormulaTree::DrawFormula(FILE* f)
{
    fprintf(f, "digraph tree {\n");
    int n = 0;
    if (DrawTree(root, f, n)) {
        fprintf(f, "}");
        return 0;
    }
    return 1;
}

int CFormulaTree::DrawTree(FormulaNode* _root, FILE* f, int& n)
{
    ++n;
    if (_root->IsLeaf()) {
        if (_root->is_number_or_func) {
            fprintf(f, "%d [label=\"%d\"]\n", n, _root->value);
            return n;
        }
        switch (_root->value) {
        #define DEF_VAR(name, value) \
        case VAR_##name: \
            fprintf(f, "%d [label=\"%c\"]\n", n, value); \
            return n;
        #include "variables.h"
        #undef DEF_VAR
        default:
            printf("CFormulaTree::DrawTree: Error. Unknown variable\n");
            return n;
        }
    }

    int number = n;

    if (_root->is_number_or_func) {
        switch (_root->value) {
        #define DEF_COM(name, val, num, diff, opt) \
        case COM_##name: \
            fprintf(f, "%d [label=\"%s\"]\n", n, val); \
            break;
        #include "commands.h"
        #undef DEF_COM
        default:
            printf("\nCFormulaTree::DrawTree: Error. Unknown function\n");
            return 1;
        }
        fprintf(f, "%d -> {%d}\n", number, DrawTree(_root->left, f, n));
        return number;
    }

    switch (_root->value) {
    #define DEF_OP(name, val, num, diff, opt) \
    case OP_##name: \
        fprintf(f, "%d [label=\"%c\"]\n", n, val); \
        break;
    #include "operands.h"
    #undef DEF_OP
    default:
        printf("\nCFormulaTree::PrintTree: Error. Unknown operand\n");
        return 1;
    }

    int l = DrawTree(_root->left, f, n);
    int r = DrawTree(_root->right, f, n);
    fprintf(f, "%d -> {%d, %d}\n", number, l, r);
    return number;
}

void CFormulaTree::Optimization()
{
    OptimiseTree(root);
}

 bool CFormulaTree::IsZero(FormulaNode* node)
 {
     return (node != nullptr && node->IsLeaf() && node->is_number_or_func && node->value == 0);
 }

  bool CFormulaTree::IsOne(FormulaNode* node)
 {
     return (node != nullptr && node->IsLeaf() && node->is_number_or_func && node->value == 1);
 }

void CFormulaTree::OptimiseTree(FormulaNode* &_root)
{
    if (!_root->IsLeaf()) {
        OptimiseTree(_root->left);
        if (_root->right) {
            OptimiseTree(_root->right);
        }
        OptimiseNode(_root);
    }
}

void CFormulaTree::OptimiseNode(FormulaNode* &_root)
{
    switch (_root->value) {
    #define DEF_COM(name, val, num, diff, opt) \
    case COM_##name: { \
        opt \
    }
    #include "commands.h"
    #undef DEF_COM

    #define DEF_OP(name, val, num, diff, opt) \
    case OP_##name: { \
        opt \
    }
    #include "operands.h"
    #undef DEF_OP
    default: {
        return;
    }
    }
}
