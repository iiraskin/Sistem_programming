#include "CRPNHandler.cpp"

int main()
{
    CRPNHandler p;

    while (true) {
        int convert_or_work = 0;
        printf ("Input 0 if you want to convert code, 1 if you want run code\n");
        scanf ("%d", &convert_or_work);

        if (convert_or_work) {
            p.Work();
        } else {
            p.Convert();
        }
    }
}
