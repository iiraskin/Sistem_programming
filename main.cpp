#include <iostream>
#include "ProgLang.cpp"

int main()
{
    CProgLang pl;
    pl.MakeTree();
    pl.Print();
    pl.Scan();
    pl.ToCode();
    pl.ToASM();
    return 0;
}
