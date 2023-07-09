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
        int is_equal = 0;
        char *tl = (char*)malloc(strlen(trusted_list));
        strcpy(tl, trusted_list);

        char *sub_list = strtok(tl, delim);
        while (sub_list != NULL && !is_equal) {
                is_equal = compare_str(sub_list, usb_fngprnt);
                sub_list = strtok(NULL, delim);
        }

        free(tl);
        return is_equal;
}
