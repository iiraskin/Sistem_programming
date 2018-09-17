#include <iostream>
#include <cmath>
#include <assert.h>
#include <vector>
#include <array>

using namespace std;

const int INF_ROOTS = 8; // Infinite number of roots.
const int COMPLEX_ROOTS = 3; // Roots are complex.
const double EPSILON = 0.000001;

// Solves an equation with c = 0.
int SolveConstantEquation(double c)
{
    printf("# It's a Constant Equation\n");
    if (c == 0) { //Those the equation has the form 0 = 0.
        return INF_ROOTS;
    } else {     // The equation has the form c = 0.
        return 0;
    }
}

// Solve the linear equation b * x + c = 0, where b != 0. The result fills into x.
int SolveLinearEquation(double b, double c, double *x)
{
    if (b == 0) {
        printf("# It's not a Linear Equation\n");
        return SolveConstantEquation(c); // The equation degenerates into linear.
    }

    printf("# It's a Linear Equation\n");
    *x = -c/b;
    return 1;
}

// Solve the quadratic equation a * x * x + b * x + c = 0. The result fills into x1 and x2.
// in the case of complex roots inserts their real part in x1, and in x2 - imaginary,
// In this case, it is enough to remember one root: the second is complex conjugate.
// Returns the number of roots.
int SolveSquareEquation(double a, double b, double c, double *x1, double *x2)
{
    assert(isfinite(a));
    assert(isfinite(b));
    assert(isfinite(c));

    assert(x1 != nullptr);
    assert(x2 != nullptr);
    assert(x1 != x2);

    if (a == 0) {
        printf("# It's not a Square Equation\n");
        return SolveLinearEquation(b, c, x1); // The equation degenerates into linear.
    }

    printf("# It's a Square Equation\n");
    long double discriminant = b * b - 4 * a * c;
    printf("# Discriminant = %lg\n", discriminant);
    if (discriminant - 0 < EPSILON) { // Complex roots.
        *x1 = -b / (2 * a);
        *x2 = sqrt(-discriminant) / (2 * a);
        return COMPLEX_ROOTS;
    }
    *x1 = (-b + sqrt(discriminant)) / (2 * a);
    if (discriminant == 0) {               // Roots are the same
        *x2 = *x1;
        return 1;
    } else {  // discriminant > 0
        *x2 = (-b - sqrt(discriminant)) / (2 * a);
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

int EquationSolver()
{
    printf ("# This program solve square equation a*x*x + b*x + c = 0\n");
    printf ("# Enter a, b, c\n");
    double a, b, c;
    scanf("%lg %lg %lg",&a, &b, &c);
    double x1, x2;
    int num_of_roots = SolveSquareEquation(a, b, c, &x1, &x2);
    return PrintRes(num_of_roots, x1, x2);
}

void MakeTests(vector< array<double, 3> > &tests, vector< pair<int, pair<double, double> > > &answers)
{
    tests.push_back({1, 3, 2});
    answers.push_back(make_pair(2, make_pair(-2, -1)));
    tests.push_back({1, 4, 4});
    answers.push_back(make_pair(1, make_pair(-2, -2)));
    tests.push_back({0, 1, 4});
    answers.push_back(make_pair(1, make_pair(-4, -4)));
    tests.push_back({0, 0, 3});
    answers.push_back(make_pair(0, make_pair(0, 0)));
    tests.push_back({0, 0, 0});
    answers.push_back(make_pair(INF_ROOTS, make_pair(0, 0)));
    tests.push_back({1, 3, -4});
    answers.push_back(make_pair(2, make_pair(-4, 1)));
    tests.push_back({-2, 4, -2});
    answers.push_back(make_pair(1, make_pair(1, 1)));
    tests.push_back({1, 6, 10});
    answers.push_back(make_pair(COMPLEX_ROOTS, make_pair(-3, 1)));
}

const double PRECISION = 0.00001;

int Testing()
{
    vector< array<double, 3> > tests; // vector of tests, consisting of triples a, b, c..
    vector< pair<int, pair<double, double> > > answers;
    MakeTests(tests, answers);
    pair<int, pair<double, double> > program_answer; // number of roots, x1, x2
    for (int i = 0; i < tests.size(); ++i) {
        printf("----------------\n");
        printf("Test %d: %lgx^2+%lgx+%lg=0\n", i + 1, tests[i][0], tests[i][1], tests[i][2]);
        program_answer.first = SolveSquareEquation(tests[i][0], tests[i][1], tests[i][2],
                                                   &program_answer.second.first, &program_answer.second.second);
        PrintRes(program_answer.first, program_answer.second.first, program_answer.second.second);
        try {
            if (program_answer.first != answers[i].first) {
                throw;
            }
            if (answers[i].first == 1 && abs(answers[i].second.first - program_answer.second.first) > PRECISION) {
                throw;
            }
            // If there are 2 roots and they are different, then don't forget to take into account that they can go in the wrong order.
            if (answers[i].first == 2 && (abs(answers[i].second.first - program_answer.second.first) > PRECISION ||
                                          abs(answers[i].second.second - program_answer.second.second) > PRECISION) &&
                (abs(answers[i].second.first - program_answer.second.second) > PRECISION ||
                 abs(answers[i].second.second - program_answer.second.first) > PRECISION)) {
                throw;
            }
            if (answers[i].first == COMPLEX_ROOTS && answers[i] != program_answer) {
                throw;
            }
        }
        catch(...) {
            printf("FALSE!\nThe real answer is:\n");
            PrintRes(answers[i].first, answers[i].second.first, answers[i].second.second);
            continue;
        }
        printf("TRUE!\n");
    }
    return 0;
}

int main()
{
    Testing();
    EquationSolver();
}
