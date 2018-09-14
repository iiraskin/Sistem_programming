#include <iostream>
#include <cmath>
#include <assert.h>
#include <vector>
#include <array>

using namespace std;

const int INF_ROOTS = 8; //Бесконечное количество корней.
const int COMPLEX_ROOTS = 3; //Корни комплексные.

//Решает уравнение вида c = 0.
int SolveConstantEquation(double c)
{

    printf("# It's a Constant Equation\n");
    if (c == 0) { //Т.е. уравнение имеет вид 0=0.
        return INF_ROOTS;
    } else {     //Уравнение имеет вид с=0.
        return 0;
    }
}

//Решает линейное уравнение b*x + c = 0, где b != 0. Результат заносит в x.
int SolveLinearEquation(double b, double c, double *x)
{
    if (b == 0) {
        printf("# It's not a Linear Equation\n");
        return SolveConstantEquation(c); //Уравнение вырождается в линейное.
    }

    printf("# It's a Linear Equation\n");
    *x = -c/b;
    return 1;
}

//Решает квадратное уравнение a*x*x + b*x + c = 0. Результат заносит в x1 и x2.
// в случае с комплексными корнями заносит в x1 их действительную часть, а в x2 - мнимую,
//при этом достаточно помнить один корень: второй - комплексно-сопряжённый.
//Возвращает количество корней.
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
        return SolveLinearEquation(b, c, x1); //Уравнение вырождается в линейное.
    }

    printf("# It's a Square Equation\n");
    long double discriminant = b * b - 4 * a * c;
    printf("# Discriminant = %lg\n", discriminant);
    if (discriminant < 0) { //Комплексные корни.
        *x1 = -b / (2 * a);
        *x2 = sqrt(-discriminant) / (2 * a);
        return COMPLEX_ROOTS;
    }
    *x1 = (-b + sqrt(discriminant)) / (2 * a);
    if (discriminant == 0) {               //Корни совпадают
        *x2 = *x1;
        return 1;
    } else {  //discriminant > 0
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
        //В случае комплексных корней в x1 и x2 хранятся их действительная и мнимая части соответственно.
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
    vector< array<double, 3> > tests; //вектор тестов, состоящий из троек a,b,c.
    vector< pair<int, pair<double, double> > > answers;
    MakeTests(tests, answers);
    pair<int, pair<double, double> > program_answer; //количество корней, x1, x2
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
            //Если корней 2 и они разные, то не забыть учесть, что они могут идти в ответе не в том порядке.
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
