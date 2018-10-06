#include <algorithm>
#include <fcntl.h>
#include <string.h>
#include <sys\stat.h>
#include <vector>

using std::vector;

const char CHAR_INIT = 20;
//Text settings
const int NUM_OF_VERCE = 33; //Number of verces
const int VERCE_SIZE = 14; //Size of verce. Now it is like in Onegin
//Files settings
const char* TEXT = "test.txt";
const char* SORT_TEXT = "sort.txt";
const char* R_SORT_TEXT = "r_sort.txt";
const char* NEW_TEXT = "nonsense.txt";
//language settings
const int NUM_OF_VEWELS = 12;
const char vowels_en[12] = {'A', 'E', 'I', 'O', 'U', 'Y', 'a', 'e', 'i', 'o', 'u', 'y'};


// The function fills the list of strings. It returns 0 if it is successful.
// Also the function replaces the line breaks in the text with '\0'. Options:
// text - processed text (array of strings)
// lines - the empty list that the function fills in
// num_of_lines - the number of lines in the text, is filled with the function.
int makeListOfLines(char** text, char*** lines, int* num_of_lines);
// The function reads the text from the file and performs its initial processing. It returns 0 if it is successful. Options:
// fd - text.
// text - processed text (array of strings). It is passed to the function empty and is filled in to it.
// lines is the empty list that the function fills (by calling makeListOfLines).
// num_of_lines - the number of lines in the text, is filled with a function (by calling makeListOfLines).
int readText(int fd, char** text, char*** lines, int* num_of_lines);
// The function compares 2 lines. The lines are compared lexicographically.
// Returns true if a < b, false otherwise.
bool comparatorOfStrings(char* a, char* b);
// The function compares 2 lines. The strings are compared lexicographically, but starting from the end.
// Returns true if a < b, false otherwise.
bool reversComparatorOfStrings(char* a, char* b);
// Sorts the lines in lines. The lines are compared lexicographically.
void sortStrings(char*** lines, int num_of_lines);
// Sorts the lines in lines. The lines are compared lexicographically, but from the end.
void reversSortStrings(char*** lines, int num_of_lines);//Options:
//fd - file.
//lines - text.
int printText(FILE* fd, char**& lines);
// Checks if the simbol is a vowel.

struct Rhyming_lines
{
    int first;
    int last;
    Rhyming_lines() {
        first = -1;
        last = -1;
    }
};

bool isVowel(char a);
// Check if 2 lines rhyme. For this, it is checked whether their endings coincide.
bool isLinesRhyme(char* line_1, char* line_2);
// The function finds 2 rhyming strings. Options:
// lines - a list of lines of text.
// num_of_lines - number of lines (length of lines).
// rhymes is an array where for each line there is a pointer to the beginning of the range of rhyming lines with it. It is filled lazy.
// res_1, res_2 - the numbers of the found rhyming lines are entered here.
// is_used - contains information about which lines are already used.
void makeRhyme(char*** lines, int num_of_lines, Rhyming_lines** rhymes, int* res_1, int* res_2, bool** is_used);
// Generates a poem by accidentally rearranging rhyming lines. Options:
// lines - a list of lines of text.
// num_of_lines - number of lines (length of lines).
// generate_text - array for writing the generated poem. To be completed with the function.
// rhymes is an array where for each line there is a pointer to the beginning of the range of rhyming lines with it. It is filled lazy.
// num_generate_lines - the size of the generated text.
int nonsenseGenerator(char*** lines, int num_of_lines, char*** generate_text, Rhyming_lines** rhymes, int num_generate_lines);

int main()
{
    char* text = nullptr;
    char** lines = nullptr;
    int num_of_lines = 0;
    // Open file
    int f = open(TEXT, O_RDONLY);
    if (!f) {
        printf("readText: Cannot open file.\n");
        return 1;
    }
    //Reading of text
    if (readText(f, &text, &lines, &num_of_lines)) {
        printf("main: error with reading of text\n");
        return 1;
    }

    //Sort text
    sortStrings(&lines, num_of_lines);
    FILE* fd = fopen(SORT_TEXT, "w");
    printText(fd, lines);
    fclose(fd);

    //Revers sort text
    reversSortStrings(&lines, num_of_lines);
    fd = fopen(R_SORT_TEXT, "w");
    printText(fd, lines);
    fclose(fd);

    //Generate nonsense-text
    char** generate_text = nullptr;
    Rhyming_lines* rhymes = new Rhyming_lines[num_of_lines];
    if (nonsenseGenerator(&lines, num_of_lines, &generate_text, &rhymes, VERCE_SIZE * NUM_OF_VERCE)) {
        printf("main: error in generator\n");
        return 1;
    }
    fd = fopen(NEW_TEXT, "w");
    printText(fd, generate_text);
    fclose(fd);

    //Free memory
    delete[] rhymes;
    delete[] generate_text;
    delete[] lines;
    delete[] text;
    return 0;
}

int makeListOfLines(char** text, char*** lines, int* num_of_lines)
{
    if (*text == nullptr || *lines != nullptr) {
        printf("makeListOfLines: Incorrect input data\n");
        return 1;
    }
    // Count the number of lines in the text
    *num_of_lines = 1;
    for (int i = 0; (*text)[i + 1] != '\0'; ++i) {
        if ((*text)[i] == '\n') {
            ++(*num_of_lines);
        }
    }
    // Fill the list of strings
    *lines = new char*[*num_of_lines + 1]{nullptr};
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

int readText(int fd, char** text, char*** lines, int* num_of_lines)
{
    if (*text != nullptr || *lines != nullptr) {
        printf("readText: Incorrect input data\n");
        return 1;
    }
    struct stat buff = {};
    // Filling the buff structure
    fstat(fd, &buff);
    *text = new char[buff.st_size + 1]{CHAR_INIT};
    // Read the text
    int text_lean = read(fd, *text, buff.st_size);
    (*text)[text_lean] = '\0';
    // Preparing the text for further use
    if (makeListOfLines(text, lines, num_of_lines)) {
        printf("readText: Error in makeListOfLines\n");
        return 1;
    }
    return 0;
}

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

void sortStrings(char*** lines, int num_of_lines)
{
    std::sort((*lines), (*lines) + num_of_lines, comparatorOfStrings);
}

void reversSortStrings(char*** lines, int num_of_lines)
{
    std::sort((*lines), (*lines) + num_of_lines, reversComparatorOfStrings);
}

int printText(FILE* fd, char**& lines)
{
    if (fd == nullptr) {
        printf("main: Cannot open file %s.\n", R_SORT_TEXT);
        return 1;
    }
    for (int i = 0; lines[i] != nullptr; ++i) {
        fprintf(fd, "%s\n", lines[i]);
        // Make an empty line at the end of each verce.
        if (i % VERCE_SIZE == VERCE_SIZE - 1) {
            fprintf(fd, "\n");
        }
    }
    return 0;
}

bool isVowel(char a)
{
    for (int i = 0; i < NUM_OF_VEWELS; ++i) {
        if (a == vowels_en[i]) {
            return true;
        }
    }
    return false;
}

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
    } while (!isVowel(line_1[i + 1]));
    return true;
}

void makeRhyme(char*** lines, int num_of_lines, Rhyming_lines** rhymes, int* res_1, int* res_2, bool** is_used)
{
    // Look for an unused string. Generate random string and find nearest unused by iteration
    for (*res_1 = rand() % num_of_lines; (*is_used)[*res_1]; (*res_1) = ((*res_1) + 1) % num_of_lines) {}
    (*is_used)[*res_1] = true;
    // Find the pair for line *res_1.
    // Perhaps this part of lines hasn't marked in 'rhymes' yet
    if ((*rhymes)[*res_1].first == -1) {
        int f_r_line = *res_1; // first rhyme line
        while (f_r_line != 0 && !isLinesRhyme((*lines)[f_r_line], (*lines)[f_r_line - 1])) {
            --f_r_line;
        }
        int l_r_line = *res_1; // first rhyme line
        while (l_r_line != num_of_lines && !isLinesRhyme((*lines)[l_r_line], (*lines)[l_r_line + 1])) {
            ++l_r_line;
        }
        // Mark rhyming lines
        for (int i = f_r_line; i <= l_r_line; ++i) {
            (*rhymes)[i].first = f_r_line;
            (*rhymes)[i].last = l_r_line;
        }
    }

    int f_r_line = (*rhymes)[*res_1].first;
    int l_r_line = (*rhymes)[*res_1].last;
    // Generate second line
    *res_2 = rand() % (l_r_line - f_r_line + 1) + f_r_line;
    for (int i = 0; (*is_used)[*res_2]; ++i) {
    //printf("%d\n", i);
        // If there is no rhyme try to take neighbour line.
        if (i > l_r_line - f_r_line) {
            if (*res_1 % 2 == 0) {
                *res_2 = *res_1 + 1;
            } else {
                *res_2 = *res_1 - 1;
            }
            if (!(*is_used)[*res_2]) {
                break;
            } else {
                // the attempt failed
                printf("makeRhyme: Can not find rhyme to '%s'\n", (*lines)[*res_1]);
                return makeRhyme(lines, num_of_lines, rhymes, res_1, res_2, is_used);
            }
        }
        *res_2 = *res_2 + 1;
        if (*res_2 > l_r_line) {
            *res_2 = f_r_line;
        }
    }
    (*is_used)[*res_2] = true;
}

int nonsenseGenerator(char*** lines, int num_of_lines, char*** generate_text, Rhyming_lines** rhymes, int num_generate_lines)
{
    if (*generate_text != nullptr || num_generate_lines % VERCE_SIZE != 0) {
        printf("nonsenseGenerator: Incorrect input data\n");
        return 1;
    }
    if (num_generate_lines > num_of_lines) {
        printf("nonsenseGenerator: Too long generated text\n");
        return 1;
    }

    *generate_text = new char*[num_generate_lines + 1]{nullptr};
    (*generate_text)[num_generate_lines] = nullptr;

    bool* is_used = new bool[num_of_lines]{false};
    // Generate text. At each iteration, one stanza is obtained.
    // Default order rhyme as like in Onegin.
    for (int i = 0; i < num_generate_lines; i += VERCE_SIZE) {
        int line_1, line_2;
        // Quatrain with cross rhyme
        makeRhyme(lines, num_of_lines, rhymes, &line_1, &line_2, &is_used);
        (*generate_text)[i] = (*lines)[line_1];
        (*generate_text)[i + 2] = (*lines)[line_2];
        makeRhyme(lines, num_of_lines, rhymes, &line_1, &line_2, &is_used);
        (*generate_text)[i + 1] = (*lines)[line_1];
        (*generate_text)[i + 3] = (*lines)[line_2];
        // Quatrain with adjacent rhyme
        makeRhyme(lines, num_of_lines, rhymes, &line_1, &line_2, &is_used);
        (*generate_text)[i + 4] = (*lines)[line_1];
        (*generate_text)[i + 5] = (*lines)[line_2];
        (*generate_text)[i + 7] = (*lines)[line_2];
        makeRhyme(lines, num_of_lines, rhymes, &line_1, &line_2, &is_used);
        (*generate_text)[i + 6] = (*lines)[line_1];
        // Quatrain with ring rhyme
        makeRhyme(lines, num_of_lines, rhymes, &line_1, &line_2, &is_used);
        (*generate_text)[i + 8] = (*lines)[line_1];
        (*generate_text)[i + 11] = (*lines)[line_2];
        makeRhyme(lines, num_of_lines, rhymes, &line_1, &line_2, &is_used);
        (*generate_text)[i + 9] = (*lines)[line_1];
        (*generate_text)[i + 10] = (*lines)[line_2];
        // Another 2 rhymed lines.
        makeRhyme(lines, num_of_lines, rhymes, &line_1, &line_2, &is_used);
        (*generate_text)[i + 12] = (*lines)[line_1];
        (*generate_text)[i + 13] = (*lines)[line_2];
    }
    delete[] is_used;
    return 0;
}
