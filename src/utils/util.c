#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <limits.h>
#include <unistd.h>
#include <syslog.h>

#include "../canaryusb.h"
#include "./toml.h"
#include "./util.h"

// this is here just for testing purposes
char *test_config_file;

void check_memory_allocation(void *check_me)
{
        if (check_me == NULL) {
                fprintf(stderr, "ERROR allocating memory");
                exit(EXIT_FAILURE);
        }
}

void replace_in_string(char *to_replace, char replace_this, char replace_with)
{
        int i;
        for (i=0; i <= strlen(to_replace); i++) {
                if (to_replace[i] == replace_this)
                        to_replace[i] = replace_with;
        }
}

void show_help()
{
        printf(BOLD_TEXT "%s v%s by char 2023\n" NO_BOLD_TEXT, _NAME_, _VERSION_);
        printf("\n");
        printf("Sends email notification when a USB or SDCard device is plugged into your computer, powered by Canary Tokens\n");
        printf("\n");
        printf("Options:\n");
        printf(BOLD_TEXT "-v, --version\n" NO_BOLD_TEXT);
        printf("\t\tthis prints the canaryusb version\n");
        printf(BOLD_TEXT "-c, --canary-token [DNS canary token]\n" NO_BOLD_TEXT);
        printf("\t\tcreated token from Canary token site: https://canarytokens.org/generate\n");
        printf("\t\tyou'll receive the notification based on the created DNS token\n");
        printf("\t\tto know more about it check: https://docs.canarytokens.org/guide/dns-token.html\n");
        printf(BOLD_TEXT "-f, --fingerprint-device\n" NO_BOLD_TEXT);
        printf("\t\tthis prints the fingerprint related with a USB or SDCard device that is plugged into computer\n");
        printf("\t\tand could be used to create the list for " BOLD_TEXT "trust-list" NO_BOLD_TEXT " option.\n");
        printf("\t\tIn this mode, will not be any call to Canary Tokens, only the device fingerprint will be printed.\n");
        printf("\t\tAlso will not start a daemon, to quit" BOLD_TEXT " ctrl+c\n" NO_BOLD_TEXT);
        printf(BOLD_TEXT "-u, --usb-monitor\n" NO_BOLD_TEXT);
        printf("\t\twill " BOLD_TEXT "only monitor USB" NO_BOLD_TEXT " devices.\n");
        printf(BOLD_TEXT "-s, --sdcard-monitor\n" NO_BOLD_TEXT);
        printf("\t\twill " BOLD_TEXT "only monitor SDCard" NO_BOLD_TEXT " devices.\n");
        printf("\t\t" BOLD_TEXT "note:" NO_BOLD_TEXT " that the USB storage devices will be also monitored.\n");
        printf(BOLD_TEXT "-t, --trust-list [comma separated device fingerprint list]\n" NO_BOLD_TEXT);
        printf("\t\tlist of device fingerprints, comma seprated, to not notify when the related deviced is connected\n");
        printf("\t\tcheck " BOLD_TEXT "fingerprint-device" NO_BOLD_TEXT " option to retrieve device fingerprint for connected USB or SDCard device\n");
        printf(BOLD_TEXT "-k, --kill\n" NO_BOLD_TEXT);
        printf("\t\tkills the daemon, if it's running\n");
        printf("\n");
        printf(BOLD_TEXT "Note:\n" NO_BOLD_TEXT);
        printf("If any option is not provided the default behaviour is try to retrieve the options from the a config file located at " BOLD_TEXT "~/.config/canaryusb/config.toml\n" NO_BOLD_TEXT);
        printf("An example of this configuration file is under " BOLD_TEXT "configuration/ " NO_BOLD_TEXT "directory at the repo.\n");
        printf("https://github.com/carvilsi/canaryusb");
        printf("\n\n");
        
        exit(EXIT_FAILURE);
}

FILE *command_file_descriptor_exec(char *cmnd)
{
        FILE *fd = popen(cmnd, "r");
        free(cmnd);

        if (fd == NULL) {
                fprintf(stderr, "ERROR not possible to get file descriptor\n");
                exit(EXIT_FAILURE);
        }

        return fd;
}

static void close_file_descriptor(FILE *fd)
{
        if (pclose(fd) == -1) {
                fprintf(stderr, "ERROR not possible to close stream\n");
                exit(EXIT_FAILURE);
        }
}

int is_running()
{
        char *cmd_pgrep = "pgrep";
        char *cmd_cnt = "| wc -l";
        char *cmd = (char *) malloc(strlen(cmd_pgrep) + strlen(_NAME_) + strlen(cmd_cnt) + 2);
        check_memory_allocation(cmd);
        sprintf(cmd, "%s %s %s", cmd_pgrep, _NAME_, cmd_cnt);

        FILE *fd = command_file_descriptor_exec(cmd);

        char cmdo[MAX_PID_LEN];
        fgets(cmdo, MAX_PID_LEN, fd);
        int nprcss = atoi(cmdo);

        close_file_descriptor(fd);    

        if (nprcss > 1)
                return 1;
        else
                return 0;
}

void kill_canaryusb_instance()
{
       if (is_running()) {
               char *cmd_pgrep = "kill $(pgrep";
               char *cmd_cnt = "| head -1)";
               char *cmd = (char *) malloc(strlen(cmd_pgrep) + strlen(_NAME_) + strlen(cmd_cnt) + 2);
               check_memory_allocation(cmd);
               sprintf(cmd, "%s %s %s", cmd_pgrep, _NAME_, cmd_cnt);

               FILE *fd = command_file_descriptor_exec(cmd);
               
               char cmdo[MAX_PID_LEN];
               fgets(cmdo, MAX_PID_LEN, fd);
               close_file_descriptor(fd);    

               syslog(LOG_NOTICE, "%s daemon stopped", _NAME_);
               printf("%s daemon stopped\n", _NAME_);
       } else {
               printf("no %s daemon running, nothing to stop\n", _NAME_);
       }
       
       exit(EXIT_SUCCESS);
}

void check_argument_length(char *arg, int type)
{ 
        size_t len = strlen(arg) + 1;
        if (type == TYPE_TRUSTEDLIST_LENGTH_CHECK) {
                printf("the length of trusted list is %ld\n", len);
                if (len > MAX_TRUSTED_LIST_LENGTH) {
                        fprintf(stderr, "The trusted list characters exceeds the limit of %d\n", 
                                        MAX_TRUSTED_LIST_LENGTH);
                        exit(EXIT_FAILURE);
                }
        }

        if (type == TYPE_CANARYTOKEN_LENGTH_CHECK) {
                dprintf("the length of canary token is %ld\n", len);
                if (len > MAX_CANARY_TOKEN_LENGTH) {
                        fprintf(stderr, "The canary token characters exceeds the limit of %d\n", 
                                        MAX_CANARY_TOKEN_LENGTH);
                        exit(EXIT_FAILURE);
                }
        }
}

void config_file_handler(char *cnrytkn, char *trstdlst)
{
        FILE *fp;
        char errbuf[200];
        
        char config_file[PATH_MAX];

        if (TESTS) {
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) != NULL) {
                        dprintf("Current working dir for testing: %s\n", cwd);
                        sprintf(config_file, "%s/%s", cwd, test_config_file);
                } else {
                        fprintf(stderr, "TESTING ERROR: not possible to retrieve current directory\n");
                        exit(EXIT_FAILURE);
                }
        } else {
                sprintf(config_file, "%s/%s", getenv("HOME"), CONFIG_FILE);
        }

        fp = fopen(config_file, "r");
        if (!fp) {
                dprintf("No config file at %s\n", config_file);
                show_help();
        }

        toml_table_t *conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
        fclose(fp);

        if (!conf) {
                dprintf("not possible to parse config file\n");
                show_help();
        }

        toml_table_t *canary_conf = toml_table_in(conf, _NAME_);
        if (!canary_conf) {
                fprintf(stderr, "ERROR: config file exists but is missing [%s] "
                                "table please check README.md", _NAME_);
                show_help();
        }

        toml_datum_t cnry_tkn = toml_string_in(canary_conf, "canary_token");
        if (!cnry_tkn.ok) {
                fprintf(stderr, "ERROR: no canary_token value at config file "
                                "please check README.md\n");
                show_help();
        } else {
                dprintf("canary_token config value: %s\n", cnry_tkn.u.s);
                check_argument_length(cnry_tkn.u.s, TYPE_CANARYTOKEN_LENGTH_CHECK);
                strcpy(cnrytkn, cnry_tkn.u.s);
        }

        toml_datum_t trust_list = toml_string_in(canary_conf, "trust_list");
        if (!trust_list.ok) {
                dprintf("WARN: no trust_list value at config file\n");
                trstdlst = NULL;
        } else {
                dprintf("trust_list config value: %s\n", trust_list.u.s);
                check_argument_length(trust_list.u.s, TYPE_TRUSTEDLIST_LENGTH_CHECK);
                strcpy(trstdlst, trust_list.u.s);
        }
        
        free(cnry_tkn.u.s);
        toml_free(canary_conf);
}

