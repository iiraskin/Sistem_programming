#include <iostream>

const long long BEGIN_CANARY = 0xBCA00BCA;
const long long END_CANARY = 0xECA00ECA;
const unsigned int STACK_SIZE = 100;

template <class st_type> class CStack
{
private:
    long long begin_canary = BEGIN_CANARY;
    unsigned int num_of_els = 0; //Size of the filled part of the stack.
    st_type buffer[STACK_SIZE];
    size_t control_sum = Make_hash();
    long long end_canary = END_CANARY;
    unsigned int num_of_els_copy = 0;

public:
    CStack() {
        printf("# Stack created\n");
    }

    ~CStack() {
        printf("# Stack ceased to exist\n");
    }

    bool Push(st_type el) {
        int er;
        if (er = Is_stack_OK()) {
            printf("# CStack::Push: Stack is broken. Error %d. For more information use function Error_inf\n", er);
            return false;
        }
        if (num_of_els == STACK_SIZE - 1) {
            printf("# CStack::Push: Stack is full\n");
            return false;
        }
        buffer[num_of_els] = el;
        ++num_of_els;
        ++num_of_els_copy;
        Change_Hash(el);
        if (er = Is_stack_OK()) {
            printf("# CStack::Push: Stack is broken by this function. Error %d. because it is incorrect. \
                    For more information use function Error_inf\n", er);
            return false;
        }
        return true;
    }

    bool Pop() {
        int er;
        if (er = Is_stack_OK()) {
            printf("# CStack::Push: Stack is broken. Error %d. For more information use function Error_inf\n", er);
            return false;
        }
        if (num_of_els == 0) {
            printf("# CStack::Pop: Stack is empty\n");
            return false;
        }
        Change_Hash(buffer[num_of_els - 1]);
        --(num_of_els);
        --(num_of_els_copy);
        if (er = Is_stack_OK()) {
            printf("# CStack::Push: Stack is broken by this function. Error %d. because it is incorrect. \
                    For more information use function Error_inf\n", er);
            return false;
        }
        return true;
    }

    st_type Top() const
    {
        int er;
        if (er = Is_stack_OK()) {
            printf("# CStack::Push: Stack is broken. Error %d. For more information use function Error_inf\n", er);
            return false;
        }
        if (num_of_els == 0) {
            printf("# CStack::Top: stack is empty\n");
            return NULL;
        }
        return buffer[num_of_els - 1];
    }

    void Error_inf(int er) const
    {
        switch (er) {
        case 1:
            printf("Stack pointer is nullptr now\n");
            return;
        case 2:
            printf("Stack is damaged from its begin\n");
            return;
        case 3:
            printf("Stack is damaged from its end\n");
            return;
        case 4:
            printf("Number of element in the stack became < 0\n");
            return;
        case 5:
            printf("Number of element in the stack became > then its size\n");
            return;
        case 6:
            printf("Number of element in the stack is incorrect\n");
            return;
        case 7:
            printf("Incorrect control sum\n");
            return;
        case 0:
            printf("No error\n");
            return;
        default:
            printf("Unknown error\n");
        }
    }

private:
    int Is_stack_OK() const {
        if (!this) {
            return 1;
        }
        if (begin_canary != BEGIN_CANARY) {
            return 2;
        }
        if (end_canary != END_CANARY) {
            return 3;
        }
        if (num_of_els < 0) {
            return 4;
        }
        if (num_of_els >= STACK_SIZE - 1) {
            return 5;
        }
        if (num_of_els_copy != num_of_els) {
            return 6;
        }
        if (control_sum != Make_hash()) {
            return 7;
        }
        return 0;
    }

    size_t Make_hash() const {
        std::hash<st_type> hash_fn;
        size_t res = 0;
        for (int i = 0; i < num_of_els; ++i) {
            res ^= hash_fn(buffer[i]);
        }
        return res;
    }

    void Change_Hash(st_type el) {
        std::hash<st_type> hash_fn;
        control_sum ^= hash_fn(el);
    }
};
