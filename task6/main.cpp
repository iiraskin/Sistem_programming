#include "formula.cpp"

#include <iostream>

void TexText(FILE* f, const char* text)
{
    FILE* text_f = fopen(text, "r");
    char str[256];
    while (fgets (str, 256, text_f) != NULL) {
        fputs(str, f);
    }
    fclose(text_f);
}

void DrawTree(CFormulaTree* tree)
{
    FILE* f = fopen("tree.txt", "w");
    tree->DrawFormula(f);
    fclose(f);
}

void DiffWithDump()
{
    CFormulaTree tree;
    tree.ReadFormula();
    DrawTree(&tree);
    FILE* f = fopen("res.txt", "w");
    TexText(f, "begin.txt");
    tree.TexFormula(f);;
    TexText(f, "begin2.txt");
    CFormulaTree d_tree = tree.Differentiation(f);
    TexText(f, "optimize.txt");
    d_tree.Optimization();
    d_tree.TexFormula(f);
    TexText(f, "end.txt");
    fclose(f);
}

int main()
{
    DiffWithDump();
    return 0;
}
