#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cst.h"

#include "./../src/canaryusb.h"
#include "./../src/utils/util.h"
#include "./../src/canary/canaries.h"
#include "./../src/usbs/usbs.h"
#include "./../src/utils/trusted_list.h"

#define PROVIDED_CANARY_TOKEN "555whateverYouGetFrom.canarytokens.com"
#define CONFIGURED_CANARY_TOKEN "testing.configuration.file.canarytokens.com"
#define PROVIDED_TRUSTED_LIST "list,of,foobar,trusted-devices"
#define CONFIGURED_TRUSTED_LIST "1af3:0001-ZOWIE_Gaming_mouse-no,594d:604d-YD60MQ-no"
#define BASE32_ENCODED_USB_FINGERPRINT "GFQWMMZ2GAYDAMJNLJHVOSKFL5DWC3LJNZTV63LPOVZWKLLON4"
#define CANARY_DNS_TOKEN "GFQWMMZ2GAYDAMJNLJHVOSKFL5DWC3LJNZTV63LPOVZWKLLON4.G42.555whateverYouGetFrom.canarytokens.com" 
#define ID_VENDOR "1af3"
#define ID_PRODUCT "0001"
#define PRODUCT_NAME "ZOWIE Gaming mouse"
#define SERIAL "no"
#define USB_FINGERPRINT "1af3:0001-ZOWIE_Gaming_mouse-no"

static void config_file_reading_only_canary_token()
{
        test_config_file = "configurations/config_canary_token.toml";
        
        cst_i(trusted_list, "variable should be", 0);
        parse_configuration_file();
        cst_a("canary_token must exits", canary_token != NULL);
        cst_s(canary_token, "must be equal to the one at the config file", CONFIGURED_CANARY_TOKEN);
        cst_i(trusted_list, "variable should be", 0);
        cst_a("trusted list must not exits", trusted_list_value == NULL);
}

static void config_file_reading_canary_token_and_trusted_list()
{
        test_config_file = "configurations/config_canary_token_trusted_list.toml";
        
        cst_i(trusted_list, "variable should be", 0);
        parse_configuration_file();
        cst_a("canary_token must exits", canary_token != NULL);
        cst_s(canary_token, "must be equal to the one at the config file", CONFIGURED_CANARY_TOKEN);
        cst_i(trusted_list, "variable should be", 1);
        cst_a("trusted list must exits", trusted_list_value != NULL);
        cst_s(trusted_list_value, "must be equal to the one at the config file", CONFIGURED_TRUSTED_LIST);
        
        //reseting trusted_list var
        trusted_list = 0;
}

static void command_line_arguments_canary_token()
{
        char *argv[] = {"canaries", "-c", PROVIDED_CANARY_TOKEN};
        parse_command_line(3, argv);
        cst_a("canary_token must exits", canary_token != NULL);
        cst_s(canary_token, "must be equal to the one provided", PROVIDED_CANARY_TOKEN);
}

static void command_line_arguments_canary_token_and_trusted_list()
{
        cst_i(trusted_list, "should be", 0);
        char *argv[] = {"canaries", "-c", PROVIDED_CANARY_TOKEN, "-t", PROVIDED_TRUSTED_LIST};
        parse_command_line(5, argv);
        cst_a("trusted list must exits", trusted_list_value != NULL);
        cst_s(trusted_list_value, "must be equal to the one provided list", PROVIDED_TRUSTED_LIST);
        cst_i(trusted_list, "should be", 1);
}

static void command_list_arguments_usb_fingerprint()
{
        cst_i(usb_fingerprint, "should be", 0);
        char *argv[] = {"canaries", "-c", PROVIDED_CANARY_TOKEN, "-t", PROVIDED_TRUSTED_LIST, "-u"};
        parse_command_line(6, argv);
        cst_i(usb_fingerprint, "variable should be", 1);
}

static void get_canary_encoded_usb_fingerprint_test() {
        char *usb_fingerprint = "1af3:0001-ZOWIE_Gaming_mouse-no";
        char *base32_usb_fingerprint= (char *) malloc(TOTAL_MAX_BASE_32_MESSAGE_LENGTH + 1);
        get_canary_encoded_usb_fingerprint(usb_fingerprint, base32_usb_fingerprint);
        cst_s(base32_usb_fingerprint, "should be base32 encoded", BASE32_ENCODED_USB_FINGERPRINT); 
        free(base32_usb_fingerprint);
}

static void usb_fingerprint_and_trusted_list()
{
        char *usb_fingrprnt = "foobar";
        int is_in_list = is_usb_device_in_trust_list(trusted_list_value, usb_fingrprnt, TRUSTED_LIST_DELIMITER);
        cst_a("should ignore usb fingerprint if is at trusted list", is_in_list == 1);
        usb_fingrprnt = "tar";
        is_in_list = is_usb_device_in_trust_list(trusted_list_value, usb_fingrprnt, TRUSTED_LIST_DELIMITER);
        cst_a("should not ignore usb fingerprint if is not at trusted list", is_in_list == 0);
}

static void build_canary_dns_token_()
{
        char *canary_dns_token = (char *) malloc(strlen(BASE32_ENCODED_USB_FINGERPRINT) + strlen(MAGIC_STRING) + 2 + strlen(canary_token));
        build_canary_dns_token(BASE32_ENCODED_USB_FINGERPRINT, canary_dns_token, canary_token);
        cst_s(canary_dns_token, "should give", CANARY_DNS_TOKEN);
        free(canary_dns_token);
}

static void call_the_canary_() 
{
        cst_a("should return a non-error value", call_the_canary("google.com") == 0);
        cst_a("should return a nonzero error different than", call_the_canary("google.orc") != 0);
}

static void get_usb_fingerprint_()
{
        UsbAttrs usbattr = {"0000", "0000", "no", "no"};
        usbattr.vendor = ID_VENDOR;
        usbattr.product = ID_PRODUCT;
        usbattr.product_name = PRODUCT_NAME;
        usbattr.serial = SERIAL;
        size_t fingp_len = strlen(usbattr.vendor) + strlen(usbattr.product) + strlen(usbattr.product_name) + strlen(usbattr.serial) + 5;
        char tmp_usb_fingprt[fingp_len];
        char *usb_fingrprnt = get_usb_fingerprint(usbattr, tmp_usb_fingprt);

        cst_s(usb_fingrprnt, "should build and retieve", USB_FINGERPRINT);
} 

static void all_tests()
{
        cst_run(config_file_reading_only_canary_token);
        cst_run(config_file_reading_canary_token_and_trusted_list);
        cst_run(command_line_arguments_canary_token);
        cst_run(command_line_arguments_canary_token_and_trusted_list);
        cst_run(command_list_arguments_usb_fingerprint);
        cst_run(get_canary_encoded_usb_fingerprint_test);
        cst_run(usb_fingerprint_and_trusted_list);
        cst_run(build_canary_dns_token_);
        cst_run(call_the_canary_);
        cst_run(get_usb_fingerprint_);
}

int main(void)  
{
        all_tests();
        cst_results();
}

