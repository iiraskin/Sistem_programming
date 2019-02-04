#pragma once
#include <iostream>

#include "comments.h"

struct FormulaNode
{
    bool is_number_or_func = false;
    int value = 0;
    FormulaNode* left = nullptr;
    FormulaNode* right = nullptr;

    FormulaNode(){}
    FormulaNode(bool _is_number_or_func, int _value) {
        is_number_or_func = _is_number_or_func;
        value = _value;
    }

    FormulaNode* Copy() {
        if (this) {
            FormulaNode* new_node = new FormulaNode(is_number_or_func, value);
            new_node->left =  left->Copy();
            new_node->right = right->Copy();
            return new_node;
        }
        return nullptr;
    }

    bool IsLeaf()
    {
        return (left == nullptr && right == nullptr);
    }
};

class CFormulaTree
{
private:
    const char CHAR_INIT = 20;

    enum variables {
        #define DEF_VAR(name, value) VAR_##name,
        #include "variables.h"
        #undef DEF_VAR
        DUMMY_VAR};

    enum operands {
        #define DEF_OP(name, val, num, diff, opt) OP_##name = num,
        #include "operands.h"
        #undef DEF_OP
        DUMMY_OP};

    enum commands {
        #define DEF_COM(name, val, num, diff, opt) COM_##name = num,
        #include "commands.h"
        #undef DEF_COM
        DUMMY_COM};

private:
    FormulaNode* root = nullptr;
    Comments comments;

public:
    CFormulaTree();
    ~CFormulaTree();

    int ReadFormula();
    int PrintFormula();
    int DrawFormula(FILE* f);
    CFormulaTree Differentiation(FILE* f);
    void Optimization();
    int TexFormula(FILE* f);

private:
    CFormulaTree(FormulaNode* _root);

    void DeleteTree(FormulaNode* _root);
    FormulaNode* MakeTree(int &pos, const char* formula);
    int PrintTree(FormulaNode* _root);
    int DrawTree(FormulaNode* _root, FILE* f, int& n);
    FormulaNode* DiffTree(FormulaNode* _root, FILE* f);
    void OptimiseTree(FormulaNode* &_root);
    void OptimiseNode(FormulaNode* &_root);

    bool IsZero(FormulaNode* node);
    bool IsOne(FormulaNode* node);

    int TexTree(FormulaNode* _root, FILE* f, int &len);
    int TexDiff(FILE* f, FormulaNode* _root, FormulaNode* diff_root);
};
