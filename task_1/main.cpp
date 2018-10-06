#include <iostream>
#include <cmath>
#include <assert.h>
#include <vector>

using std::vector;

const double PRECISION = 0.00001; //Precision of calculating
const double EPSILON = 0.000001;  //Precision of comparison of doubles;

enum Num_of_roots {
    INF_ROOTS = 8, // Infinite number of roots.
    COMPLEX_ROOTS = 3, // Roots are complex.
};

const char* TESTS = "tests.txt";

struct Test
{
    double a;
    double b;
    double c;

    Test(double _a, double _b, double _c)
    {
        a = _a;
        b = _b;
        c = _c;
    }
};

struct Answer
{
    int num; //number of roots
    double x1;
    double x2;

    Answer(double _num, double _x1, double _x2)
    {
        num = _num;
        x1 = _x1;
        x2 = _x2;
    }
};

// Solves an equation with c = 0.
int SolveConstantEquation(double c);
// Solve the linear equation b * x + c = 0, where b != 0. The result fills into x.
int SolveLinearEquation(double b, double c, double *x);
// Solve the quadratic equation a * x * x + b * x + c = 0. The result fills into x1 and x2.
// in the case of complex roots inserts their real part in x1, and in x2 - imaginary,
// In this case, it is enough to remember one root: the second is complex conjugate.
// Returns the number of roots.
int SolveSquareEquation(double a, double b, double c, double *x1, double *x2);
int PrintRes(int num_of_roots, double x1, double x2);
void EquationSolver();
void MakeTests(vector<Test> &tests, vector<Answer> &answers);
int Testing(const vector<Test>& tests, const vector<Answer>& answers);

int main()
{
    vector<Test> tests;
    vector<Answer> answers;
    MakeTests(tests, answers);
    Testing(tests, answers);
    while (true) {
        std::cout << "# This program solve square equation a*x*x + b*x + c = 0" << std::endl;
        std::cout << "# Enter a, b, c" << std::endl;
        double a = 0, b = 0, c = 0;
        std::cin >> a >> b >> c;
        double x1 = 0, x2 = 0;
        int num_of_roots = SolveSquareEquation(a, b, c, &x1, &x2);
        if (PrintRes(num_of_roots, x1, x2)) {
            std::cout << "# Some error in PrintRes" << std::endl;
        }
        std::cout << "# Input 'continue' to continue" << std::endl;
        std::string command;
        std::cin >> command;
        if (command != "continue") {
            break;
        }
    }
}

int SolveConstantEquation(double c)
{
    printf("# It's a Constant Equation\n");
    if (abs(c) < EPSILON) { //Those the equation has the form 0 = 0.
        return INF_ROOTS;
    } else {     // The equation has the form c = 0.
        return 0;
    }
}

int SolveLinearEquation(double b, double c, double *x)
{
    if (abs(b) < EPSILON) {
        printf("# It's not a Linear Equation\n");
        return SolveConstantEquation(c); // The equation degenerates into linear.
    }

    printf("# It's a Linear Equation\n");
    *x = -c/b;
    return 1;
}

int SolveSquareEquation(double a, double b, double c, double *x1, double *x2)
{
    assert(std::isfinite(a));
    assert(std::isfinite(b));
    assert(std::isfinite(c));

    assert(x1 != nullptr);
    assert(x2 != nullptr);
    assert(x1 != x2);

    if (abs(a) < EPSILON) {
        printf("# It's not a Square Equation\n");
        return SolveLinearEquation(b, c, x1); // The equation degenerates into linear.
    }

    if (abs(c) < EPSILON) {
        printf("# It's a Square Equation, where c = 0\n");
        *x1 = 0;
        SolveLinearEquation(a, b, x2);
        return 2;
    }

    printf("# It's a Square Equation\n");
    long double discriminant = b * b - 4 * a * c;
    printf("# Discriminant = %lg\n", discriminant);
    if (discriminant < -EPSILON) { // Complex roots.
        *x1 = -b / (2 * a);
        *x2 = sqrt(-discriminant) / (2 * a);
        return COMPLEX_ROOTS;
    }
    double sqrt_of_discriminant = sqrt(discriminant);
    *x1 = (-b + sqrt_of_discriminant) / (2 * a);
    if (abs(discriminant) < EPSILON) {               // Roots are the same
        *x2 = *x1;
        return 1;
    } else {  // discriminant > 0
        *x2 = (-b - sqrt_of_discriminant) / (2 * a);
        return 2;
    }
}

int PrintRes(int num_of_roots, double x1, double x2)
{
    switch (num_of_roots) {
        case 0: printf ("No roots\n");
                break;
        case 1: printf ("x = %lg\n", x1);
                break;
        case 2: printf ("x1 = %lg, x2 = %lg\n", x1, x2);
                break;
        // In the case of complex roots, their real and imaginary parts, respectively, are stored in x1 and x2.
        case COMPLEX_ROOTS: printf ("x1 = %lg + %lgi, x2 = %lg - %lgi\n", x1, x2, x1, x2);
                            break;
        case INF_ROOTS: printf("Any number\n");
                        break;
        default: printf ("ERROR: num_of_roots = %d\n", num_of_roots);
                 return 1;
        }
    return 0;
}

void MakeTests(vector<Test> &tests, vector<Answer> &answers)
{
    FILE* f = fopen(TESTS, "r");
    double a = 0, b = 0, c = 0, x1 = 0, x2 = 0;
    int num = 0;
    while ((fscanf(f, "%lg", &a)) != EOF) {
        if ((fscanf(f, "%lg", &b)) == EOF) {
            printf ("Incorrect test: a=%lg;\n b, c, num, x1, x2 are not given\n", a);
            return;
        }
        if ((fscanf(f, "%lg", &c)) == EOF) {
            printf ("Incorrect test: a=%lg, b=%lg;\n c, num, x1, x2 are not given\n", a, b);
            return;
        }
        if ((fscanf(f, "%d", &num)) == EOF) {
            printf ("Incorrect test: a=%lg, b=%lg, c=%lg;\n num, x1, x2 are not given\n", a, b, c);
            return;
        }
        if ((fscanf(f, "%lg", &x1)) == EOF) {
            printf ("Incorrect test: a=%lg, b=%lg, c=%lg, num=%d\n; x1, x2 are not given\n", a, b, c, num);
            return;
        }
        if ((fscanf(f, "%lg", &x2)) == EOF) {
            printf ("Incorrect test: a=%lg, b=%lg, c=%lg, num=%d, x1=%lg\n; x2 is not given\n", a, b, c, num, x1);
            return;
        }
        tests.push_back(Test(a, b, c));
        answers.push_back(Answer(num, x1, x2));
    }
    fclose(f);
}

int Testing(const vector<Test>& tests, const vector<Answer>& answers)
{
    Answer program_answer(0, 0, 0);
    for (int i = 0; i < tests.size(); ++i) {
        printf("----------------\n");
        printf("Test %d: %lgx^2+%lgx+%lg=0\n", i + 1, tests[i].a, tests[i].b, tests[i].c);
        program_answer.num = SolveSquareEquation(tests[i].a, tests[i].b, tests[i].c, &program_answer.x1, &program_answer.x2);
        PrintRes(program_answer.num, program_answer.x1, program_answer.x2);
        try {
            if (program_answer.num != answers[i].num) {
                throw;
            }
            if (answers[i].num == 1 && abs(answers[i].x1 - program_answer.x1) > PRECISION) {
                throw;
            }
            // If there are 2 roots and they are different, then don't forget to take into account that they can go in the wrong order.
            if (answers[i].num == 2 && (abs(answers[i].x1 - program_answer.x1) > PRECISION ||
                                        abs(answers[i].x2 - program_answer.x2) > PRECISION) &&
                                       (abs(answers[i].x1 - program_answer.x2) > PRECISION ||
                                        abs(answers[i].x2 - program_answer.x1) > PRECISION)) {
                throw;
            }
            if (answers[i].num == COMPLEX_ROOTS && (answers[i].num != program_answer.num ||
                                                    answers[i].x1 != program_answer.x1 ||
                                                    answers[i].x2 != program_answer.x2)) {
                throw;
            }
        }
        catch(...) {
            printf("!!!\n");
            printf("FALSE!\nThe real answer is:\n");
            PrintRes(answers[i].num, answers[i].x1, answers[i].x2);
            continue;
        }
        printf("TRUE!\n");
    }
    return 0;
}
