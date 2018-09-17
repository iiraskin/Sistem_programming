#include <algorithm>
#include <fcntl.h>
#include <string.h>
#include <sys\stat.h>
#include <vector>

using namespace std;

const int NUM_OF_VERCE = 33; //Количество строф

const int VERCE_SIZE = 14; //Размер строфы. Сейчас стоит как в Онегине.

//Настройки языка
const int NUM_OF_VEWELS = 12;
const char* TEXT = "test.txt";
const char* SORT_TEXT = "sort.txt";
const char* R_SORT_TEXT = "r_sort.txt";
const char* NEW_TEXT = "nonsense.txt";


char vowels_en[12] = {'A', 'E', 'I', 'O', 'U', 'Y', 'a', 'e', 'i', 'o', 'u', 'y'};
char vowels_ru[20] = {'А', 'Е', 'Ё', 'И', 'О', 'У', 'Ы', 'Э', 'Ю', 'Я',
                      'а', 'е', 'ё', 'и', 'о', 'у', 'ы', 'э', 'ю', 'я'};

//Функция заполняет список строк. Возвращает 0 в случае успеха.
//Также функция заменяет переносы строки в тексте на '\0'. Параметры:
//text - обрабатываемый текст (массив строк)
//lines - пустой список, который заполняет функция
//num_of_lines - количество строк в тексте, заполняется функцией.
int makeListOfLines(char** text, char*** lines, int* num_of_lines)
{
    if (*text == nullptr || *lines != nullptr) {
        printf("makeListOfLines: Incorrect input data\n");
        return 1;
    }
    //Подсчитываем количество строк в тексте
    *num_of_lines = 1;
    for (int i = 0; (*text)[i] != '\0'; ++i) {
        if ((*text)[i] == '\n') {
            ++(*num_of_lines);
        }
    }
    //Заполняем список строк
    *lines = new char*[*num_of_lines + 1];
    (*lines)[0] = *text;
    int j = 1;
    for (int i = 1; (*text)[i] != '\0'; ++i) {
        if ((*text)[i - 1] == '\n') {
            (*lines)[j] = &((*text)[i]);
            ++j;
            (*text)[i - 1] = '\0';
        }
    }
    if (j != *num_of_lines) {
        printf("makeListOfLines: Some strange error\n");
        return 1;
    }
    (*lines)[*num_of_lines] = nullptr;
    return 0;
}

//Функция читает текст из файла и выполняет его первичную обработку. Возвращает 0 в случае успеха. Параметры:
//text - обрабатываемый текст (массив строк). Передаётся в функцию пустым и заполняется ей.
//lines - пустой список, который заполняет функция (вызовом makeListOfLines).
//num_of_lines - количество строк в тексте, заполняется функцией (вызовом makeListOfLines).
int readText(char** text, char*** lines, int* num_of_lines)
{
    if (*text != nullptr || *lines != nullptr) {
        printf("readText: Incorrect input data\n");
        return 1;
    }
    //Открытие файла
    int fd;
    if ((fd = open(TEXT, O_RDONLY)) == 0) {
        printf("readText: Cannot open file.\n");
        return 1;
    }
    struct stat buff;
    //Заполнение структуры buff
    fstat(fd, &buff);
    *text = new char[buff.st_size + 1];
    (*text)[buff.st_size] = '\0';
    //Чтение текста
    read(fd, *text, buff.st_size);
    //Подготовка текста к дальнейшему использованию
    if (makeListOfLines(text, lines, num_of_lines)) {
        printf("readText: Error in makeListOfLines\n");
        return 1;
    }
    close(fd);
    return 0;
}

//Функция сравнивает 2 строки. Строки сравниваются лексикографически.
//Возвращает true, если a < b, false иначе.
bool comparatorOfStrings(char* a, char* b)
{
    int i = 0, j = 0;
    while (true) {
        while (!isalpha(a[i]) && a[i] != '\0') {
            ++i;
        }
        while (!isalpha(b[j]) && b[j] != '\0') {
            ++j;
        }
        if (b[j] == '\0') {
            return false;
        }
        if (a[i] == '\0') {
            return true;
        }
        if (a[i] < b[j]) {
            return true;
        }
        if (a[i] > b[j]) {
            return false;
        }
        ++i;
        ++j;
    }
}

//Функция сравнивает 2 строки.Строки сравниваются лексикографически, но начиная с конца.
//Возвращает true, если a < b, false иначе.
bool reversComparatorOfStrings(char* a, char* b)
{
    int i = strlen(a) - 1, j = strlen(b) - 1;
    while (true) {
        while (!isalpha(a[i]) && i != 0) {
            --i;
        }
        while (!isalpha(b[j]) && j != 0) {
            --j;
        }
        if (j == 0) {
            return false;
        }
        if (i == 0) {
            return true;
        }
        if (a[i] < b[j]) {
            return true;
        }
        if (a[i] > b[j]) {
            return false;
        }
        --i;
        --j;
    }
}

//Сортирует строки в lines. Строки сравниваются лексикографически.
void sortStrings(char*** lines, int num_of_lines)
{
    sort((*lines), (*lines) + num_of_lines, comparatorOfStrings);
}

//Сортирует строки в lines. Строки сравниваются лексикографически, но с конца.
void reversSortStrings(char*** lines, int num_of_lines)
{
    sort((*lines), (*lines) + num_of_lines, reversComparatorOfStrings);
}

//Проверяет, является ли символ гласной буквой.
bool isVowel(char a)
{
    for (int i = 0; i < NUM_OF_VEWELS; ++i) {
        if (a == vowels_en[i]) {
            return true;
        }
    }
    return false;
}

//Проверят, рифмуются ли 2 строки. Для этого проверяется, совпадают ли их окончания.
bool isLinesRhyme(char* line_1, char* line_2)
{
    int i = strlen(line_1) - 1, j = strlen(line_2) - 1;
    do {
        while (!isalpha(line_1[i]) && i != 0) {
            --i;
        }
        while (!isalpha(line_2[j]) && j != 0) {
            --j;
        }
        if (line_1[i] != line_2[j]) {
            return false;
        }
        --i;
        --j;
    } while (!isVowel(line_1[i]));
    return true;
}

//Функция находит 2 рифмующиеся строки. Параметры:
//lines - список строк текста.
//num_of_lines - количество строк (длина lines).
//rhymes - массив, где для каждой строки лежит указатель на начало диапазона рифмующихся с ней строк. Запоняется лениво.
//res_1, res_2 - сюда заносятся номера найденных рифмующихся строк.
//is_used - содержит информацию, какие строки уже использованы.
void makeRhyme(char*** lines, int num_of_lines, char**** rhymes, int* res_1, int* res_2, bool** is_used)
{
    //Ищем не использованную строку.
    for (*res_1 = rand() % num_of_lines; (*is_used)[*res_1]; (*res_1) = ((*res_1) + 1) % num_of_lines) {}
    (*is_used)[*res_1] = true;
    int first_rhyme_line = *res_1;//Первая строка, рифмующаяся с данной.
    while (first_rhyme_line != 0 && (*rhymes)[first_rhyme_line] == nullptr &&
           isLinesRhyme((*lines)[first_rhyme_line], (*lines)[first_rhyme_line - 1])) {
        --first_rhyme_line;
    }
    (*rhymes)[first_rhyme_line] = &((*lines)[first_rhyme_line]);
    int i;
    for (i = first_rhyme_line + 1; i <= *res_1; ++i) {
        (*rhymes)[i] = &((*lines)[first_rhyme_line]);
    }
    for (i = *res_1 + 1; i < num_of_lines && isLinesRhyme((*lines)[first_rhyme_line], (*lines)[i]); ++i) {
        (*rhymes)[i] = &((*lines)[first_rhyme_line]);
    }
    for (*res_2 = rand() % (i - first_rhyme_line) + first_rhyme_line;
         (*is_used)[*res_2]; (*res_2) = ((*res_2) + 1)) {
         if (*res_2 == i) {
            *res_2 -= first_rhyme_line;
         }
    }
    (*is_used)[*res_2] = true;
}

//Генерирует стихотворение, случайно переставляя рифмующиеся строки. Параметры:
//lines - список строк текста.
//num_of_lines - количество строк (длина lines).
//generate_text - массив для записи генерируемого стихотворения. Заполняется функцией.
//rhymes - массив, где для каждой строки лежит указатель на начало диапазона рифмующихся с ней строк. Запоняется лениво.
//num_generate_lines - размер генерируемого текста.
int nonsenseGenerator(char*** lines, int num_of_lines, char*** generate_text, char**** rhymes, int num_generate_lines)
{
    if (*generate_text != nullptr || num_generate_lines % VERCE_SIZE != 0) {
        printf("nonsenseGenerator: Incorrect input data\n");
        return 1;
    }
    if (num_generate_lines > num_of_lines) {
        printf("nonsenseGenerator: Too long generated text\n");
        return 1;
    }
    *generate_text = new char*[num_generate_lines + 1];
    (*generate_text)[num_generate_lines] = nullptr;

    bool* is_used = new bool[num_of_lines];
    for (int i = 0; i < num_of_lines; ++i) {
        is_used[i] = false;
    }
    //Генерация текста. На каждой итерации получается одна строфа.
    //По умолчанию порядок рифм как в Онегине.
    for (int i = 0; i < num_generate_lines; i += VERCE_SIZE) {
        int line_1, line_2;
        //Четверостишие с перекрёстной рифмой
        makeRhyme(lines, num_of_lines, rhymes, &line_1, &line_2, &is_used);
        (*generate_text)[i] = (*lines)[line_1];
        (*generate_text)[i + 2] = (*lines)[line_2];
        makeRhyme(lines, num_of_lines, rhymes, &line_1, &line_2, &is_used);
        (*generate_text)[i + 1] = (*lines)[line_1];
        (*generate_text)[i + 3] = (*lines)[line_2];
        //Четверостишие со смежной рифмой
        makeRhyme(lines, num_of_lines, rhymes, &line_1, &line_2, &is_used);
        (*generate_text)[i + 4] = (*lines)[line_1];
        (*generate_text)[i + 5] = (*lines)[line_2];
        makeRhyme(lines, num_of_lines, rhymes, &line_1, &line_2, &is_used);
        (*generate_text)[i + 6] = (*lines)[line_1];
        (*generate_text)[i + 7] = (*lines)[line_2];
        //Четверостишие с кольцевой рифмой
        makeRhyme(lines, num_of_lines, rhymes, &line_1, &line_2, &is_used);
        (*generate_text)[i + 8] = (*lines)[line_1];
        (*generate_text)[i + 11] = (*lines)[line_2];
        makeRhyme(lines, num_of_lines, rhymes, &line_1, &line_2, &is_used);
        (*generate_text)[i + 9] = (*lines)[line_1];
        (*generate_text)[i + 10] = (*lines)[line_2];
        //Ещё 2 рифмованные строки.
        makeRhyme(lines, num_of_lines, rhymes, &line_1, &line_2, &is_used);
        (*generate_text)[i + 12] = (*lines)[line_1];
        (*generate_text)[i + 13] = (*lines)[line_2];
    }
    delete[] is_used;
    return 0;
}

int main()
{
    char* text = nullptr;
    char** lines = nullptr;
    int num_of_lines = 0;
    if (readText(&text, &lines, &num_of_lines)) {
        printf("main: error with reading of text\n");
        return 1;
    }
    int fd;

    sortStrings(&lines, num_of_lines);
    if ((fd = open(SORT_TEXT, O_WRONLY)) == 0) {
        printf("main: Cannot open file %s.\n", SORT_TEXT);
    } else {
        for (int i = 0; lines[i] != nullptr; ++i) {
            write(fd, lines[i], strlen(lines[i]));
            write(fd, "\n", 1);
        }
    }

    reversSortStrings(&lines, num_of_lines);
    if ((fd = open(R_SORT_TEXT, O_WRONLY)) == 0) {
        printf("main: Cannot open file %s.\n", R_SORT_TEXT);
    } else {
        for (int i = 0; lines[i] != nullptr; ++i) {
            write(fd, lines[i], strlen(lines[i]));
            write(fd, "\n", 1);
        }
    }

    char** generate_text = nullptr;
    char*** rhymes = new char**[num_of_lines];
    if (nonsenseGenerator(&lines, num_of_lines, &generate_text, &rhymes, VERCE_SIZE * NUM_OF_VERCE)) {
        printf("main: error in generator\n");
        return 1;
    }
    if ((fd = open(NEW_TEXT, O_WRONLY)) == 0) {
        printf("main: Cannot open file %s.\n", NEW_TEXT);
    } else {
        for (int i = 0; generate_text[i] != nullptr; ++i) {
            write(fd, generate_text[i], strlen(generate_text[i]));
            write(fd, "\n", 1);
            //Сделаем пустую строку в конце каждой строфы.
            if (i % VERCE_SIZE == VERCE_SIZE - 1) {
                write(fd, "\n", 1);
            }
        }
    }

    delete[] rhymes;
    delete[] generate_text;
    delete[] lines;
    delete[] text;
    return 0;
}
