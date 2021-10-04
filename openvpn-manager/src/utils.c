#include "main.h"

void free_multiple(int argc, ...)
{
    va_list ap;
    void *curr_ptr;

    va_start(ap, argc);
    for (int i = 0; i < argc; i++) {
        curr_ptr = va_arg(ap, void *);

        if (curr_ptr != NULL) {
            free(curr_ptr);
            curr_ptr = NULL;
        }
    }

    va_end(ap);
}

int count_lines(char *text)
{
    int count = 0;
    for (int i = 0; i < strlen(text)-1; i++) {
        if (text[i] == '\n') ++count;
    }

    return count;
}
