#include <iostream>

const long long BEGIN_CANARY = 0xBCA00BCA;
const long long END_CANARY = 0xECA00ECA;
const unsigned int DEFAULT_STACK_SIZE = 100;

template <class st_type> class CStack
{
private:
    char* st; //Pointer on stack - place, where all data is.
    long long* begin_canary;
    unsigned int* buf_size;
    unsigned int* num_of_els; //Size of the filled part of the stack.
    st_type* buffer;
    size_t* control_sum;
    long long* end_canary;

public:
    CStack(unsigned int buf_size_ = DEFAULT_STACK_SIZE) {
        st = new char[2 * sizeof(long long) + 2 * sizeof(unsigned int) +
                    buf_size_ * sizeof(st_type) + sizeof(size_t) + 1];
        begin_canary = (long long*)(st);
        buf_size = (unsigned int*)((char*)(st) + sizeof(long long));
        num_of_els = (unsigned int*)((char*)(buf_size) + sizeof(long long));
        buffer = (st_type*)((char*)(num_of_els) + sizeof(unsigned int));
        control_sum = (size_t*)((char*)(buffer) + buf_size_ * sizeof(st_type));
        end_canary = (long long*)((char*)(control_sum) + sizeof(size_t));

        *begin_canary = BEGIN_CANARY;
        *buf_size = buf_size_;
        *num_of_els = 0;
        *control_sum = Make_hash();
        *end_canary = END_CANARY;
        printf("#Stack created\n");
    }

    ~CStack() {
        delete[] st;
        printf("#Stack ceased to exist\n");
    }

    bool Push(st_type el) {
        if (Is_stack_OK()) {
            if (*num_of_els == *buf_size - 1) {
                printf("#CStack::Top: stack is full\n");
                return false;
            }
            buffer[*num_of_els] = el;
            ++(*num_of_els);
            Change_Hash(el);
            return true;
        } else {
            printf("#Stack is broken. For more information use function Error_inf\n");
            return false;
        }
    }

    bool Pop() {
        if (Is_stack_OK()) {
            if (*num_of_els == 0) {
                return false;
            }
            Change_Hash(buffer[*num_of_els - 1]);
            --(*num_of_els);
            return true;
        } else {
            printf("#Stack is broken. For more information use function Error_inf\n");
            return false;
        }
    }

    st_type Top() const
    {
        if (Is_stack_OK()) {
            if (*num_of_els == 0) {
                printf("#CStack::Top: stack is empty\n");
                return NULL;
            }
            return buffer[*num_of_els - 1];
        } else {
            printf("#Stack is broken. For more information use function Error_inf\n");
        }
    }

    void Error_inf() const
    {
        if (!this) {
            printf("Stack pointer is nullptr now\n");
            return;
        }
        if ((void*)(begin_canary) != (void*)(st)) {
            printf("Pointer on stack's content and begin of this content became different\n");
            return;
        }
        if (*begin_canary != BEGIN_CANARY) {
            printf("Stack is damaged from its begin\n");
            return;
        }
        if (*end_canary != END_CANARY) {
            printf("Stack is damaged from its end\n");
            return;
        }
        if (*num_of_els < 0) {
            printf("Number of element in the stack became < 0\n");
            return;
        }
        //One element in buffer is 'reserve'
        if (*num_of_els >= *buf_size - 1) {
            printf("Number of element in the stack became > then its size\n");
            return;
        }
        if (*control_sum != Make_hash()) {
            printf("Incorrect control sum\n");
            return;
        }
        printf("No error found\n");
    }

private:
    bool Is_stack_OK() const {
        return this &&
        (void*)(begin_canary) == (void*)(st) &&
        *begin_canary == BEGIN_CANARY && *end_canary == END_CANARY &&
        *num_of_els >= 0 && *num_of_els <= *buf_size - 1 &&
        *control_sum == Make_hash();
    }

    size_t Make_hash() const {
        std::hash<st_type> hash_fn;
        size_t res = 0;
        for (int i = 0; i < *num_of_els; ++i) {
            res ^= hash_fn(buffer[i]);
        }
        return res;
    }

    void Change_Hash(st_type el) {
        std::hash<st_type> hash_fn;
        *control_sum ^= hash_fn(el);
    }
};
