#pragma once
#include "formula.h"

int CFormulaTree::TexTree(FormulaNode* _root, FILE* f, int &len)
{
    if (len > 120) {
        fprintf(f, " \\\\ \n");
        len = 0;
    }

    if (_root->IsLeaf()) {
        if (_root->is_number_or_func) {
            fprintf(f, "%d", _root->value);
            len += 4;
            return 0;
        }
        switch (_root->value) {
        #define DEF_VAR(name, value) \
        case VAR_##name: \
            fprintf(f, "%c", value); \
            return 0;
        #include "variables.h"
        #undef DEF_VAR
        default:
            printf("CFormulaTree::PrintTree: Error. Unknown variable\n");
            return 1;
        }
        len += 1;
    }

    if (_root->is_number_or_func) {
        switch (_root->value) {
        #define DEF_COM(name, val, num, diff, opt) \
        case COM_##name: \
            fprintf(f, "%s", val); \
            fprintf(f, "("); \
            break;
        #include "commands.h"
        #undef DEF_COM
        default:
            printf("\nCFormulaTree::PrintTree: Error. Unknown function\n");
            return 1;
        }
        len += 3;
        TexTree(_root->left, f, len);
        fprintf(f, ")");
        return 0;
    }

    if ((_root->value == OP_ADD || _root->value == OP_SUB) && _root != root) {
        fprintf(f, "(");
    }
    if (_root->value == OP_POW && !_root->left->IsLeaf()) {
        fprintf(f, "(");
        TexTree(_root->left, f, len);
        fprintf(f, ")");
    } else {
        TexTree(_root->left, f, len);
    }

    switch (_root->value) {
    #define DEF_OP(name, val, num, diff, opt) \
    case OP_##name: \
        fprintf(f, " %c ", val); \
        break;
    #include "operands.h"
    #undef DEF_OP
    default:
        printf("\nCFormulaTree::PrintTree: Error. Unknown operand\n");
        return 1;
    }
    if (_root->value == OP_POW) {
        fprintf(f, "{");
    }
    len += 3;
    TexTree(_root->right, f, len);
    if ((_root->value == OP_ADD || _root->value == OP_SUB) && _root != root) {
        fprintf(f, ")");
    } else if (_root->value == OP_POW) {
        fprintf(f, "}");
    }
    return 0;
}

int CFormulaTree::TexDiff(FILE* f, FormulaNode* _root, FormulaNode* diff_root)
{
    int len = 0;
    fprintf(f, "%s", comments.GetComment());
    fprintf(f, "\n\\begin{eqnarray*}\n(");
    TexTree(_root, f, len);
    fprintf(f, ")' = ");
    TexTree(diff_root, f, len);
    fprintf(f, "\n\\end{eqnarray*}\n");
    return 0;
}

int CFormulaTree::TexFormula(FILE* f)
{
    int len = 0;
    fprintf(f, "\n\\begin{eqnarray*}\n");
    int res =  TexTree(root, f, len);
    fprintf(f, "\n\\end{eqnarray*}\n");
    return res;
}
