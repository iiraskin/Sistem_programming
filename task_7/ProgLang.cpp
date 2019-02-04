#pragma once

#include "ProgLang.h"
#include "Parser.h"
#include "Grammar.h"
#include "Interaction.h"
#include "ASM.h"

void CProgLang::DeleteTree(Node* _root)
{
    if (_root != nullptr) {
        for (int i = 0; i < _root->childs.size(); ++i) {
            DeleteTree(_root->childs[i]);
        }
        delete _root;
        _root = nullptr;
    }
}

int CProgLang::MakeTree()
{
    DeleteTree(root);
    vars = {};
    funcs = {};

    char* code = nullptr;
    if (ReadCode(&code)) {
        printf("CProgLang::MakeTree: Error in ReadCode\n");
        return 1;
    }
    vector<Node*> nodes;
    if (MakeNodes(nodes, code)) {
        printf("CProgLang::MakeTree: Error in MakeNodes\n");
        return 1;
    }
    delete[] code;

    int i = MakeG(nodes, 0, &root);
    if (i == -1) {
        DeleteTree(root);
        for (int j = 0; j < nodes.size(); ++j) {
            if (nodes[j] != nullptr) {
                delete nodes[j];
            }
        }
        return 1;
    }
    return 0;
}
