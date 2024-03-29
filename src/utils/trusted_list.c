#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "../canaryusb.h"
#include "./util.h"

int compare_str(char *sub_list, char *cmp)
{
        int res = 0;
        if (strlen(sub_list) != strlen(cmp))
                        return res;
        int i;
        for (i=0; i < strlen(cmp); i++) {
                if (sub_list[i] == cmp[i])
                        res = 1;
                else
                        return 0;
        }

        return res;
}

int is_device_in_trust_list(const char *trusted_list, char *fingrprnt, char *delim) 
{
        int is_equal = 0;
        char *tl = (char*) malloc(strlen(trusted_list));
        check_memory_allocation(tl);
        strcpy(tl, trusted_list);

        char *sub_list = strtok(tl, delim);
        while (sub_list != NULL && !is_equal) {
                is_equal = compare_str(sub_list, fingrprnt);
                sub_list = strtok(NULL, delim);
        }

        free(tl);
        return is_equal;
}
