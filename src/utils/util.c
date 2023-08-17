#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <limits.h>

#include "../canaryusb.h"

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

void config_file_handler()
{
        char fp[PATH_MAX];
        sprintf(fp, "%s/%s", getenv("HOME"), CONFIG_PATH);
        DIR *dir = opendir(fp);
        if (dir) {
                closedir(dir);
        } else if (ENOENT == errno) {
                int mdr = mkdir(fp, S_IRWXU);
                if (mdr != 0) {
                        fprintf(stderr, "ERROR: %d when creating dir %s\n", errno, fp);
                        exit(EXIT_FAILURE);
                } else {
                        /*TODO: check if config file exists*/
                        char cf[PATH_MAX];
                        sprintf(cf, "%s/%s/%s", getenv("HOME"), CONFIG_PATH, CONFIG_FILE);
                        FILE *fd = fopen(cf, "w");
                        if (fd == NULL) {
                                fprintf(stderr, "ERROR: %d when creating config file %s\n", errno, cf);
                                exit(EXIT_FAILURE);
                        } else {
                                fprintf(fd, "[canaryusb]\n");
                                fprintf(fd, "#uncomment next line and replace with your own DNS canary token; get one at: https://canarytokens.org/generate\n");
                                fprintf(fd, "#canary_token=555whateverYouGetFrom.canarytokens.com\n");
                                fprintf(fd, "\n");
                                fprintf(fd, "#uncomment next line and place your list for trusted devices, check README.md for more info\n");
                                fprintf(fd, "#trust_list=1af3:0001-ZOWIE_Gaming_mouse-no,594d:604d-YD60MQ-no\n");
                                fprintf(fd, "\n");
                                fclose(fd);
                        }
                }
        } else {
                fprintf(stderr, "ERROR: %d opening dir %s\n", errno, CONFIG_PATH);
                exit(EXIT_FAILURE);
        }
}

