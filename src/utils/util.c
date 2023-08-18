#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <limits.h>

#include "../canaryusb.h"
#include "./toml.h"

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

void check_if_running()
{
        char *cmd_pgrep = "pgrep";
        char *cmd_cnt = "| wc -l";
        char *cmd = (char *) malloc(strlen(cmd_pgrep) + strlen(_NAME_) + strlen(cmd_cnt) + 2);
        check_memory_allocation(cmd);
        sprintf(cmd, "%s %s %s", cmd_pgrep, _NAME_, cmd_cnt);
        FILE *fd = popen(cmd, "r");
        free(cmd);

        if (fd == NULL) {
                fprintf(stderr, "ERROR not possible to get file descriptor\n");
                exit(EXIT_FAILURE);
        }

        char cmdo[MAX_PID_LEN];
        fgets(cmdo, MAX_PID_LEN, fd);
        int nmb_prcss = atoi(cmdo);

        if (pclose(fd) == -1) {
                fprintf(stderr, "ERROR not possible to close stream\n");
                exit(EXIT_FAILURE);
        }

        if (nmb_prcss > 1) {
                fprintf(stderr, "there is another instance of canaryusb running\n");
                exit(EXIT_FAILURE);
        }
}

int config_file_handler()
{
        FILE* fp;
        char errbuf[200];

        fp = fopen(CONFIG_FILE, "r");
        if (!fp) {
                dprintf("No config file at %s\n", CONFIG_FILE);
                show_help();
        }

        toml_table_t* conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
        fclose(fp);

        if (!conf) {
                dprintf("not possible to parse config file\n");
                show_help();
        }

        toml_table_t* canary_conf = toml_table_in(conf, _NAME_);
        if (!canary_conf) {
                fprintf(stderr, "ERROR: config file exists but is missing [%s] table\n please check README.md", _NAME_);
                exit(EXIT_FAILURE);
        }

        toml_datum_t canary_token = toml_string_in(canary_conf, "canary_token");
        if (!canary_token.ok) {
                fprintf(stderr, "ERROR: no canary_token value at config file\n please check README.md\n");
                exit(EXIT_FAILURE);
        } else {
                dprintf("canary_token config value: %s\n", canary_token.u.s);
        }

        toml_datum_t trust_list = toml_string_in(canary_conf, "trust_list");
        if (!trust_list.ok) {
                dprintf("WARN: no trust_list value at config file\n");
        } else {
                dprintf("trust_list config value: %s\n", trust_list.u.s);
        }
        free(canary_token.u.s);
        toml_free(canary_conf);
        return 0;
}
