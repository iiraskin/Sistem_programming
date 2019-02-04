#pragma once

#include <vector>
#include <unordered_map>

using std::vector;
using std::string;

struct Node
{
    int type = 0;
    int value = 0;
    vector<Node*> childs;

    Node(){}
    Node(int _type, int _value): type(_type), value(_value) {}
    Node(int _type): type(_type), value(0) {}
};

class CProgLang
{
private:
    enum types {T_ROOT=0, T_FUNCINIT=1, T_UFUNC=2, T_NUM=3, T_VAR=4, T_OP=6, T_LOG=7, T_MAIN=8, T_SFUNC=9, T_VARINIT=10, T_RET=11, T_BRAC=20};
    enum operators {OP_ADD=2, OP_SUB=3, OP_MUL=4, OP_DIV=5, OP_ASSIGN=1};
    enum signs {SIGN_EQ=7, SIGN_NEQ=8, SIGN_LOW=9, SIGN_GREAT=10};
    enum stand_funcs {SF_IN=1, SF_OUT=2, SF_CALL=6, SF_SQRT=8};
    enum logic {LOG_IF=1, LOG_WHILE=2, LOG_COND=4, LOG_COND_MET=5};
    enum brackets {BR_LP=1, BR_RP=2, BR_LCB=3, BR_RCB=4};

    Node* root = nullptr;
    vector<string> vars = {};
    vector<string> funcs = {};

public:
    CProgLang();
    ~CProgLang();

    int MakeTree();
    void ToASM();
    void ToCode();
    void Print();
    int Scan();

private:
    int MakeNodes(vector<Node*> &nodes, char* code);
    int ReadCode(char** code);
    void DeleteTree(Node* _root);

    int MakeG(vector<Node*> &nodes, int i, Node** node);
    int MakeM(vector<Node*> &nodes, int i, Node** node);
    int MakeB(vector<Node*> &nodes, int i, Node** node);
    int MakeA(vector<Node*> &nodes, int i, Node** node);
    int MakeWI(vector<Node*> &nodes, int i, Node** node);
    int MakeS(vector<Node*> &nodes, int i, Node** node);
    int MakeC(vector<Node*> &nodes, int i, Node** node);
    int MakeTP(vector<Node*> &nodes, int i, Node** node);
    int MakeTM(vector<Node*> &nodes, int i, Node** node);
    int MakeF(vector<Node*> &nodes, int i, Node** node);

    void TreeToASM(FILE* f, Node* node, int func, int &lab);
    void TreeToCode(FILE* f, Node* node);
    void PrintTree(FILE* f, Node* node);
    int ScanTree(char* tree, int &i, Node** node);

    bool IsSpace(char c);
    bool InWord(char c);

    void SkipSpaces(char* code, int& i);
    int ScanNumber(char* str, int& i);
    bool TryScanWord(char* str, int& i, const char* word);
};

CProgLang::CProgLang() {}

CProgLang::~CProgLang()
{
    DeleteTree(root);
}

bool CProgLang::IsSpace(char c)
{
    return (c == ' ' || c == '\n' || c == '\r' || c == '\t');
}

void CProgLang::SkipSpaces(char* str, int& i)
{
    while (IsSpace(str[i])) {++i;}
}
