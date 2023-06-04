#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void check_memory_allocation(void *check_me)
{
        if (check_me == NULL) {
                fprintf(stderr, "ERROR allocating memory");
                exit(EXIT_FAILURE);
        }
}

void replace_in_string(char *to_replace, char replace_this, char replace_with)
{
        int i = 0;
        for (i; i <= strlen(to_replace); i++) {
                if (to_replace[i] == replace_this)
                        to_replace[i] = replace_with;
        }
}
