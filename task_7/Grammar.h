#pragma once

#include "ProgLang.h"

const bool PRINT_ERRORS = true;

int CProgLang::MakeG(vector<Node*> &nodes, int i, Node** node)
{
    *node = new Node(T_ROOT);
    Node* varibs = new Node(T_VARINIT);
    (*node)->childs.push_back(varibs);
    for (int j = 0; j < vars.size(); ++j) {
        Node* var_val = new Node(T_NUM, 0);
        varibs->childs.push_back(var_val);
    }

    Node* funcs = new Node(T_FUNCINIT);
    (*node)->childs.push_back(funcs);

    Node* new_node = nullptr;
    while (nodes[i]->type == T_UFUNC) {
        i = MakeF(nodes, i, &new_node);
        if (i == -1 || nodes.size() <= i) {
            return -1;
        }
        funcs->childs.push_back(new_node);
    }

    if (nodes[i]->type != T_MAIN) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeG: Error: word %d. MAIN was expected.\n", i);
        }
        return -1;
    }
    i = MakeM(nodes, i, &new_node);
    if (i == -1) {
        return -1;
    }
    (*node)->childs.push_back(new_node);
    return i;
}

int CProgLang::MakeM(vector<Node*> &nodes, int i, Node** node)
{
    if (nodes.size() <= i + 3) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeM: Error: word %d. Unexpected end of code.\n", i);
        }
        return -1;
    }

    if (nodes[i]->type != T_MAIN) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeM: Error: word %d. It is not a main function.\n", i);
        }
        return -1;
    }
    *node = nodes[i];
    ++i;
    if (nodes[i]->type != T_BRAC || nodes[i]->value != BR_LCB) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeM: Error: word %d. { was expected.\n", i);
        }
        return -1;
    }
    delete nodes[i];
    nodes[i] = nullptr;

    Node* new_node = nullptr;
    i = MakeB(nodes, i + 1, &new_node);
    if (i == -1 || nodes.size() <= i || new_node == nullptr) {
        return -1;
    }
    (*node)->childs = new_node->childs;
    delete new_node;

    if (nodes[i]->type != T_BRAC || nodes[i]->value != BR_RCB) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeM: Error: word %d. } was expected.\n", i);
        }
        return -1;
    }
    delete nodes[i];
    nodes[i] = nullptr;
    return i + 1;
}

int CProgLang::MakeB(vector<Node*> &nodes, int i, Node** node)
{
    Node* new_node = nullptr;
    (*node) = new Node(T_LOG, LOG_COND_MET);
    if (nodes[i]->type == T_VAR) {
        i = MakeA(nodes, i, &new_node);
    } else if (nodes[i]->type == T_LOG) {
        i = MakeWI(nodes, i, &new_node);
    } else if (nodes[i]->type == T_SFUNC) {
        i = MakeS(nodes, i, &new_node);
    } else if (nodes[i]->type == T_RET) {
        new_node = nodes[i];
        ++i;
    } else if (nodes[i]->type == T_BRAC && nodes[i]->value == BR_RCB) {
        return i;
    } else {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeB: Error: word number %d: %d %d. Syntax error.\n", i, nodes[i]->type, nodes[i]->value);
        }
        return -1;
    }
    if (i == -1 || nodes.size() <= i) {
        return -1;
    }

    i = MakeB(nodes, i, node);
    if (new_node != nullptr) {
        (*node)->childs.insert((*node)->childs.begin(), new_node);
    }
    return i;
}

int CProgLang::MakeA(vector<Node*> &nodes, int i, Node** node)
{
    if (nodes.size() <= i + 2) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeA: Error: word %d. Unexpected end of code.\n", i);
        }
        return -1;
    }

    if (nodes[i]->type == T_VAR && nodes[i + 1]->type == T_OP && nodes[i + 1]->value == OP_ASSIGN) {
        *node = nodes[i + 1];
        (*node)->childs.push_back(nodes[i]);
        Node* new_node = nullptr;
        i = MakeTP(nodes, i + 2, &new_node);
        if (i == -1 || nodes.size() <= i) {
            return -1;
        }
        (*node)->childs.push_back(new_node);
    } else {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeA: Parsing error on word %d.\n", i);
        }
        return -1;
    }
    return i;
}

int CProgLang::MakeWI(vector<Node*> &nodes, int i, Node** node)
{
    if (nodes.size() <= i + 6) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeWI: Error: word %d. Unexpected end of code.\n", i);
        }
        return -1;
    }

    if (nodes[i]->type != T_LOG || (nodes[i]->value != LOG_WHILE && nodes[i]->value != LOG_IF)) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeWI: Error: word %d. Incorrect syntax.\n", i);
        }
        return -1;
    }
    *node = nodes[i];
    ++i;
    if (nodes[i]->type != T_BRAC || nodes[i]->value != BR_LP) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeWI: Error: word %d. ( was expected.\n", i);
        }
        return -1;
    }
    delete nodes[i];
    nodes[i] = nullptr;
    ++i;

    Node* new_node = nullptr;
    i = MakeC(nodes, i, &new_node);
    if (i == -1 || nodes.size() <= i) {
        return -1;
    }
    (*node)->childs.push_back(new_node);

    if (nodes[i]->type != T_BRAC || nodes[i]->value != BR_RP) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeWI: Error: word %d. ) was expected.\n", i);
        }
        return -1;
    }
    delete nodes[i];
    nodes[i] = nullptr;
    ++i;

    if (nodes[i]->type != T_BRAC || nodes[i]->value != BR_LCB) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeWI: Error: word %d. { was expected.\n", i);
        }
        return -1;
    }
    delete nodes[i];
    nodes[i] = nullptr;
    ++i;

    new_node = nullptr;
    i = MakeB(nodes, i, &new_node);
    if (i == -1 || nodes.size() <= i || new_node == nullptr) {
        return -1;
    }
    (*node)->childs.push_back(new_node);

    if (nodes[i]->type != T_BRAC || nodes[i]->value != BR_RCB) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeWI: Error: word %d. } was expected.\n", i);
        }
        return -1;
    }
    delete nodes[i];
    nodes[i] = nullptr;
    ++i;
    return i;
}

int CProgLang::MakeS(vector<Node*> &nodes, int i, Node** node)
{
    if (nodes.size() <= i + 2) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeS: Error: word %d. Unexpected end of code.\n", i);
        }
        return -1;
    }

    if (nodes[i]->type != T_SFUNC) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeS: Error: word %d. Incorrect syntax.\n", i);
        }
        return -1;
    }
    *node = nodes[i];
    if (nodes[i]->value == SF_CALL) {
        if (nodes[i + 1]->type == T_UFUNC) {
            (*node)->childs.push_back(nodes[i + 1]);
            return i + 2;
        }
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeS: Error: word %d. Function's name was expected.\n", i);
        }
        return -1;
    }

    ++i;
    if (nodes[i]->type != T_BRAC || nodes[i]->value != BR_LP) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeS: Error: word %d. ( was expected.\n", i);
        }
        return -1;
    }
    delete nodes[i];
    nodes[i] = nullptr;
    ++i;

    if ((*node)->value == SF_IN) {
        if (nodes[i]->type == T_VAR) {
            (*node)->childs.push_back(nodes[i]);
            ++i;
        } else {
            if (PRINT_ERRORS) {
                printf("CProgLang::MakeS: Error: word %d. Variable was expected.\n", i);
            }
            return -1;
        }
    } else {
        Node* new_node = nullptr;
        i = MakeTP(nodes, i, &new_node);
        if (i == -1 || nodes.size() <= i) {
            return -1;
        }
        (*node)->childs.push_back(new_node);
    }

    if (nodes[i]->type != T_BRAC || nodes[i]->value != BR_RP) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeS: Error: word %d. ) was expected.\n", i);
        }
        return -1;
    }
    delete nodes[i];
    nodes[i] = nullptr;
    return i + 1;
}

int CProgLang::MakeC(vector<Node*> &nodes, int i, Node** node)
{
    if (nodes.size() <= i) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeC: Error: word %d. Unexpected end of code.\n", i);
        }
        return -1;
    }

    Node* new_node = nullptr;
    i = MakeTP(nodes, i, &new_node);
    if (i == -1 || nodes.size() <= i) {
        return -1;
    }
    *node = nodes[i];
    (*node)->childs.push_back(new_node);
    if (nodes[i]->type == T_OP && (nodes[i]->value == SIGN_EQ || nodes[i]->value == SIGN_NEQ ||
                                   nodes[i]->value == SIGN_LOW || nodes[i]->value == SIGN_GREAT)) {
        i = MakeTP(nodes, i + 1, &new_node);
        if (i == -1 || nodes.size() <= i) {
            return -1;
        }
        (*node)->childs.push_back(new_node);
        Node* cond = new Node(T_LOG, LOG_COND);
        cond->childs.push_back(*node);
        (*node) = cond;
    }
    else {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeC: Parsing error.\n");
        }
        return -1;
    }
    return i;
}

int CProgLang::MakeTP(vector<Node*> &nodes, int i, Node** node)
{
    if (nodes.size() <= i) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeTP: Error: word %d. Unexpected end of code.\n", i);
        }
        return -1;
    }

    Node* new_node = nullptr;
    i = MakeTM(nodes, i, &new_node);
    if (i == -1 || nodes.size() <= i) {
        return -1;
    }
    if (nodes[i]->type == T_OP && (nodes[i]->value == OP_ADD || nodes[i]->value == OP_SUB)) {
        *node = nodes[i];
        (*node)->childs.push_back(new_node);
        i = MakeTP(nodes, i + 1, &new_node);
        if (i == -1 || nodes.size() <= i) {
            return -1;
        }
        (*node)->childs.push_back(new_node);
    } else {
        *node = new_node;
    }
    return i;
}

int CProgLang::MakeTM(vector<Node*> &nodes, int i, Node** node)
{
    if (nodes.size() <= i + 1) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeTM: Error: word %d. Unexpected end of code.\n", i);
        }
        return -1;
    }

    if (!(nodes[i]->type == T_VAR) && !(nodes[i]->type == T_NUM)) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeTM: Error: word %d. Unexpected end of code.\n", i);
        }
        return -1;
    }
    ++i;
    if (nodes[i]->type == T_OP && (nodes[i]->value == OP_MUL || nodes[i]->value == OP_DIV)) {
        *node = nodes[i];
        (*node)->childs.push_back(nodes[i - 1]);
        Node* new_node = nullptr;
        i = MakeTM(nodes, i + 1, &new_node);
        if (i == -1 || nodes.size() <= i) {
            return -1;
        }
        (*node)->childs.push_back(new_node);
    } else {
        *node = nodes[i - 1];
    }
    return i;
}

int CProgLang::MakeF(vector<Node*> &nodes, int i, Node** node)
{
    if (nodes.size() <= i + 3) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeF: Error: word %d. Unexpected end of code.\n", i);
        }
        return -1;
    }

    if (nodes[i]->type != T_UFUNC) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeF: Error: word %d. It is not a function.\n", i);
        }
        return -1;
    }
    *node = nodes[i];
    ++i;
    if (nodes[i]->type != T_BRAC || nodes[i]->value != BR_LCB) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeF: Error: word %d. { was expected.\n", i);
        }
        return -1;
    }
    delete nodes[i];
    nodes[i] = nullptr;

    Node* new_node = nullptr;
    i = MakeB(nodes, i + 1, &new_node);
    if (i == -1 || nodes.size() <= i + 1 || new_node == nullptr) {
        return -1;
    }
    (*node)->childs = new_node->childs;
    delete new_node;
    Node* vars = new Node(T_VARINIT);
    (*node)->childs.insert((*node)->childs.begin(), vars);

    if (nodes[i]->type != T_BRAC || nodes[i]->value != BR_RCB) {
        if (PRINT_ERRORS) {
            printf("CProgLang::MakeF: Error: word %d. } was expected.\n", i);
        }
        return -1;
    }
    delete nodes[i];
    nodes[i] = nullptr;
    return i + 1;
}
