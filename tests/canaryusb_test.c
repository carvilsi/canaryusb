#include <stdio.h>
#include <string.h>

#include "ct.h"

#include "./../src/canaryusb.h"
#include "./../src/utils/util.h"

#define PROVIDED_CANARY_TOKEN "555whateverYouGetFrom.canarytokens.com"
#define CONFIGURED_CANARY_TOKEN "testing.configuration.file.canarytokens.com"
#define PROVIDED_TRUSTED_LIST "list,of,foobar,trusted-devices"
#define CONFIGURED_TRUSTED_LIST "1af3:0001-ZOWIE_Gaming_mouse-no,594d:604d-YD60MQ-no"

static void config_file_reading_only_canary_token()
{
        test_config_file = "configurations/config_canary_token.toml";
        
        ct_assert(__func__, "trusted_list variable should be 0", trusted_list == 0);
        parse_configuration_file();
        ct_assert(__func__, "canary_token must exits", canary_token != NULL);
        ct_assert(__func__, "canary_token must be equal to the one at the config file", 
                        strcmp(canary_token, CONFIGURED_CANARY_TOKEN) == 0);
        ct_assert(__func__, "trusted_list variable should be 0, since there is not trusted list at config file", 
                        trusted_list == 0);
        ct_assert(__func__, "trusted list must not exits",trusted_list_value == NULL);
}

static void config_file_reading_canary_token_and_trusted_list()
{
        test_config_file = "configurations/config_canary_token_trusted_list.toml";
        
        ct_assert(__func__, "trusted_list variable should be 0", trusted_list == 0);
        parse_configuration_file();
        ct_assert(__func__, "canary_token must exits", canary_token != NULL);
        ct_assert(__func__, "canary_token must be equal to the one at the config file", 
                        strcmp(canary_token, CONFIGURED_CANARY_TOKEN) == 0);
        ct_assert(__func__, "trusted_list variable should be 1", trusted_list == 1);
        ct_assert(__func__, "trusted list must exits", trusted_list_value != NULL);
        ct_assert(__func__, "trusted_list must be equal to the one at the config file", 
                        strcmp(trusted_list_value, CONFIGURED_TRUSTED_LIST) == 0);
        
        //reseting trusted_list var
        trusted_list = 0;
}

static void command_line_arguments_canary_token()
{
        char *argv[] = {"canaries", "-c", PROVIDED_CANARY_TOKEN};
        parse_command_line(3, argv);
        ct_assert(__func__, "canary_token must exits", canary_token != NULL);
        ct_assert(__func__, "canary_token must be equal to the one provided", 
                        strcmp(canary_token, PROVIDED_CANARY_TOKEN) == 0);
}

static void command_line_arguments_canary_token_and_trusted_list()
{
        ct_assert(__func__, "trusted_list variable should be 0", trusted_list == 0);
        char *argv[] = {"canaries", "-c", PROVIDED_CANARY_TOKEN, "-t", PROVIDED_TRUSTED_LIST};
        parse_command_line(5, argv);
        ct_assert(__func__, "trusted list must exits", trusted_list_value != NULL);
        ct_assert(__func__, "trusted_list must be equal to the one provided list", 
                        strcmp(trusted_list_value, PROVIDED_TRUSTED_LIST) == 0);
        ct_assert(__func__, "trusted_list variable should be 1", trusted_list == 1);
}

static void command_list_arguments_usb_fingerprint()
{
        ct_assert(__func__, "usb_fingerprint variable should be 0", usb_fingerprint == 0);
        char *argv[] = {"canaries", "-c", PROVIDED_CANARY_TOKEN, "-t", PROVIDED_TRUSTED_LIST, "-u"};
        parse_command_line(6, argv);
        ct_assert(__func__, "usb_fingerprint variable should be 1", usb_fingerprint == 1);
}

static void all_tests()
{
        ct_run(config_file_reading_only_canary_token);
        ct_run(config_file_reading_canary_token_and_trusted_list);
        ct_run(command_line_arguments_canary_token);
        ct_run(command_line_arguments_canary_token_and_trusted_list);
        ct_run(command_list_arguments_usb_fingerprint);
}

int main(void)  
{
        all_tests();
        ct_print_results(__FILE__);
        return (nbr_ftst == 0 ? 0 : 1);
}

