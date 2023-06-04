#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int compare_str(char *sub_list, char *cmp)
{
        int res = 0;
        if (strlen(sub_list) != strlen(cmp))
                        return res;
        int i = 0;
        for (i; i < strlen(cmp); i++) {
                if (sub_list[i] == cmp[i])
                        res = 1;
                else
                        res = 0;
        }
        return res;
}

int is_usb_device_in_trust_list(char *trusted_list, char *usb_fngprnt, char *delim) 
{
        printf("The trusted list: %s\n",trusted_list);
        printf("The usb_fingerprint: %s\n",usb_fngprnt);
        char *sub_list;
        int is_equal;

        sub_list = strtok(trusted_list, delim);
        do {
                is_equal = compare_str(sub_list, usb_fngprnt);
                if (is_equal) {
                        return is_equal;
                        break;
                }
        } while (sub_list = strtok(NULL, delim));

        return 0;
}

void check_memory_allocation(void *check_me)
{
        if (check_me == NULL) {
                fprintf(stderr, "ERROR allocating memory");
                exit(EXIT_FAILURE);
        }
}
