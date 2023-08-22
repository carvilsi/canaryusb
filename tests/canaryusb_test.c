#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "ct.h"

#include "./../src/canaryusb.h"
#include "./../src/utils/util.h"
#include "./../src/canary/canaries.h"
#include "./../src/utils/trusted_list.h"

#define PROVIDED_CANARY_TOKEN "555whateverYouGetFrom.canarytokens.com"
#define CONFIGURED_CANARY_TOKEN "testing.configuration.file.canarytokens.com"
#define PROVIDED_TRUSTED_LIST "list,of,foobar,trusted-devices"
#define CONFIGURED_TRUSTED_LIST "1af3:0001-ZOWIE_Gaming_mouse-no,594d:604d-YD60MQ-no"
#define BASE32_ENCODED_USB_FINGERPRINT "GFQWMMZ2GAYDAMJNLJHVOSKFL5DWC3LJNZTV63LPOVZWKLLON4"
#define CANARY_DNS_TOKEN "GFQWMMZ2GAYDAMJNLJHVOSKFL5DWC3LJNZTV63LPOVZWKLLON4.G42.555whateverYouGetFrom.canarytokens.com" 

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

static void get_canary_encoded_usb_fingerprint_test() {
        char *usb_fingerprint = "1af3:0001-ZOWIE_Gaming_mouse-no";
        char *base32_usb_fingerprint= (char *) malloc(TOTAL_MAX_BASE_32_MESSAGE_LENGTH + 1);
        get_canary_encoded_usb_fingerprint(usb_fingerprint, base32_usb_fingerprint);
        ct_assert(__func__, "usb should be base32 encoded", 
                        strcmp(base32_usb_fingerprint, BASE32_ENCODED_USB_FINGERPRINT) == 0); 
        free(base32_usb_fingerprint);
}

static void usb_fingerprint_and_trusted_list()
{
        char *usb_fingrprnt = "foobar";
        int is_in_list = is_usb_device_in_trust_list(trusted_list_value, usb_fingrprnt, TRUSTED_LIST_DELIMITER);
        ct_assert(__func__, "should ignore usb fingerprint if is at trusted list",
                        is_in_list == 1);
        usb_fingrprnt = "tar";
        is_in_list = is_usb_device_in_trust_list(trusted_list_value, usb_fingrprnt, TRUSTED_LIST_DELIMITER);
        ct_assert(__func__, "should not ignore usb fingerprint if is not at trusted list",
                        is_in_list == 0);
}

static void canaries()
{
        char *canary_dns_token = (char *) malloc(strlen(BASE32_ENCODED_USB_FINGERPRINT) + strlen(MAGIC_STRING) + 2 + strlen(canary_token));
        build_canary_dns_token(BASE32_ENCODED_USB_FINGERPRINT, canary_dns_token, canary_token);
        ct_assert(__func__, "should create a canary dns token",
                        strcmp(canary_dns_token, CANARY_DNS_TOKEN) == 0);
        free(canary_dns_token);
}

static void all_tests()
{
        ct_run(config_file_reading_only_canary_token);
        ct_run(config_file_reading_canary_token_and_trusted_list);
        ct_run(command_line_arguments_canary_token);
        ct_run(command_line_arguments_canary_token_and_trusted_list);
        ct_run(command_list_arguments_usb_fingerprint);
        ct_run(get_canary_encoded_usb_fingerprint_test);
        ct_run(usb_fingerprint_and_trusted_list);
        ct_run(canaries);
}

int main(void)  
{
        all_tests();
        ct_print_results(__FILE__);
        return (nbr_ftst == 0 ? 0 : 1);
}

