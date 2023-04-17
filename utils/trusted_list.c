#include <stdio.h>
#include <string.h>

void create_array_for_trusted_list(const char *trusted_list) {
        int i = 0;
        for (i; i < strlen(trusted_list); i++)
        {
                printf("%c", trusted_list[i]);
                printf("\n");
        }
}
