#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "canaryusb.h"

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

void show_help()
{
        printf(BOLD_TEXT "%s v%s\n" NO_BOLD_TEXT, _NAME_, _VERSION_);
        printf("\n");
        printf("Sends email notification when a USB device is plugged into your computer, powered by Canary Tokens\n");
        printf("\n");
        printf("Options:\n");
        printf(BOLD_TEXT "-c, --canary_token [DNS canary token]\n" NO_BOLD_TEXT);
        printf("\t\tcreated token from Canary token site: https://canarytokens.org/generate\n");
        printf("\t\tyou'll receive the notification based on the created DNS token\n");
        printf("\t\tto know more about it check: https://docs.canarytokens.org/guide/dns-token.html\n");
        printf(BOLD_TEXT "-u, --usb_fingerprint\n" NO_BOLD_TEXT);
        printf("\t\tthis prints the fingerprint related with a USB device that is plugged into computer\n");
        printf("\t\tand could be used to create the list for " BOLD_TEXT "trust_list" NO_BOLD_TEXT " option.\n");
        printf("\t\tIn this mode, will not be any call to Canary Tokens, only the usb fingerprint will be printed.\n");
        printf(BOLD_TEXT "-t, --trust_list [comma separated usb_fingerprint list]\n" NO_BOLD_TEXT);
        printf("\t\tlist of usb fingerprints, comma seprated, to not notify when the related deviced is connected\n");
        printf("\t\tcheck " BOLD_TEXT "usb_fingerprint" NO_BOLD_TEXT " option to retrieve device fingerprint for connected USB device\n");

        exit(EXIT_SUCCESS);
}
