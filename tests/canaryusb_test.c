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
#define BASE32_ENCODED_USB_FINGERPRINT_LONG "IF3GK4TZNRXW4Z2VKNBDUZTJNZTWK4TQORUW45BNORXS2Y3IMVRWWOTGN5XWEYLSMJQXU3DPNQWXI2DFO5XXE3DENFZWC3DMORUGC5DJON2GQZLDMFZWK"
#define CANARY_DNS_TOKEN "GFQWMMZ2GAYDAMJNLJHVOSKFL5DWC3LJNZTV63LPOVZWKLLON4.G42.555whateverYouGetFrom.canarytokens.com" 
#define CANARY_DNS_TOKEN_LONG "IF3GK4TZNRXW4Z2VKNBDUZTJNZTWK4TQORUW45BNORXS2Y3IMVRWWOTGN5X.WEYLSMJQXU3DPNQWXI2DFO5XXE3DENFZWC3DMORUGC5DJON2GQZLDMFZWK.G42.555whateverYouGetFrom.canarytokens.com"
#define ID_VENDOR "1af3"
#define ID_PRODUCT "0001"
#define PRODUCT_NAME "ZOWIE Gaming mouse"
#define SERIAL "no"
#define USB_FINGERPRINT "1af3:0001-ZOWIE_Gaming_mouse-no"
#define USB_FINGERPRINT_LONG "AverylongUSB:fingerptint-to-check:foobarbazlol-theworldisallthatisthecase"

static void config_file_reading_only_canary_token()
{
        test_config_file = "configurations/config_canary_token.toml";
        
        ConfigCanrayUSB opts = config_canary_usb_INIT;
        cst_a("variable trusted list should be false", !opts.trusted_list );
        parse_configuration_file(&opts);
        cst_a("canary_token must exists", opts.canary_token != NULL);
        cst_s(opts.canary_token, "must be equal to the one at the config file", CONFIGURED_CANARY_TOKEN);
        cst_a("variable trusted list should be false after read config file without trusted list", opts.trusted_list == false);
        cst_a("trusted list value must not exists", opts.trusted_list_value == NULL);
}

static void config_file_reading_canary_token_and_trusted_list()
{
        test_config_file = "configurations/config_canary_token_trusted_list.toml";
        
        ConfigCanrayUSB opts = config_canary_usb_INIT;
        cst_a("variable trusted list should be false", opts.trusted_list == false);
        parse_configuration_file(&opts);
        cst_a("canary_token must exists", opts.canary_token != NULL);
        cst_s(opts.canary_token, "must be equal to the one at the config file", CONFIGURED_CANARY_TOKEN);
        cst_a("variable trusted list should be true", opts.trusted_list);
        cst_i(opts.trusted_list, "variable should be", 1);
        cst_a("trusted list value must exists", opts.trusted_list_value != NULL);
        cst_s(opts.trusted_list_value, "must be equal to the one at the config file", CONFIGURED_TRUSTED_LIST);
}

static void command_line_arguments_canary_token()
{
        char *argv[] = {"canaries", "-c", PROVIDED_CANARY_TOKEN};
        ConfigCanrayUSB opts = config_canary_usb_INIT;
        parse_command_line(3, argv, &opts);
        cst_a("canary_token must exists", opts.canary_token != NULL);
        cst_s(opts.canary_token, "must be equal to the one provided", PROVIDED_CANARY_TOKEN);
}

static void command_line_arguments_canary_token_and_trusted_list()
{
        ConfigCanrayUSB opts = config_canary_usb_INIT;
        cst_i(opts.trusted_list, "should be", 0);
        char *argv[] = {"canaries", "-c", PROVIDED_CANARY_TOKEN, "-t", PROVIDED_TRUSTED_LIST};
        parse_command_line(5, argv, &opts);
        cst_a("trusted list must exists", opts.trusted_list_value != NULL);
        cst_s(opts.trusted_list_value, "must be equal to the one provided list", PROVIDED_TRUSTED_LIST);
        cst_i(opts.trusted_list, "should be", 1);
}

static void dev_fingerprint_variable()
{
        ConfigCanrayUSB opts = config_canary_usb_INIT;
        cst_a("dev_fingerprint on start should be false", !opts.dev_fingerprint);
        char *argv[] = {"canaries", "-c", PROVIDED_CANARY_TOKEN, "-t", PROVIDED_TRUSTED_LIST, "-f"};
        parse_command_line(6, argv, &opts);
        cst_a("dev_fingerprint after command line arguments should be true", opts.dev_fingerprint);
}

static void monitor_usb_variable()
{
        ConfigCanrayUSB opts = config_canary_usb_INIT;
        cst_a("monitor_usb on start should be false", !opts.monitor_usb);
        char *argv[] = {"canaries", "-c", PROVIDED_CANARY_TOKEN, "-t", PROVIDED_TRUSTED_LIST, "-f", "-u"};
        parse_command_line(7, argv, &opts);
        cst_a("monitor_usb after command line arguments should be true", opts.monitor_usb);
}

static void monitor_sdcard_variable()
{
        ConfigCanrayUSB opts = config_canary_usb_INIT;
        cst_a("monitor_sdcard on start should be false", !opts.monitor_sdcard);
        char *argv[] = {"canaries", "-c", PROVIDED_CANARY_TOKEN, "-t", PROVIDED_TRUSTED_LIST, "-f", "-u", "-s"};
        parse_command_line(8, argv, &opts);
        cst_a("monitor_sdcard after command line arguments should be true", opts.monitor_sdcard);
}

static void command_line_arguments_version()
{
        ConfigCanrayUSB opts = config_canary_usb_INIT;
        cst_a("version variable should be false on start", !opts.version);
        char *argv[] = {"canaries", "-c", PROVIDED_CANARY_TOKEN, "-t", PROVIDED_TRUSTED_LIST, "-f", "-u", "-s", "-v"};
        parse_command_line(9, argv, &opts);
        cst_a("version variable should be true on set by command line", opts.version);
}

static void get_canary_encoded_usb_fingerprint_test() {
        char *base32_usb_fingerprint= (char *) malloc(TOTAL_MAX_BASE_32_MESSAGE_LENGTH + 1);
        check_memory_allocation(base32_usb_fingerprint);
        get_canary_encoded_usb_fingerprint(USB_FINGERPRINT, base32_usb_fingerprint);
        cst_s(base32_usb_fingerprint, "should be base32 encoded", BASE32_ENCODED_USB_FINGERPRINT); 
        free(base32_usb_fingerprint);
}

static void get_canary_encoded_usb_fingerprint_long_test() {
        char *base32_usb_fingerprint= (char *) malloc(TOTAL_MAX_BASE_32_MESSAGE_LENGTH + 1);
        check_memory_allocation(base32_usb_fingerprint);
        get_canary_encoded_usb_fingerprint(USB_FINGERPRINT_LONG, base32_usb_fingerprint);
        cst_s(base32_usb_fingerprint, "should be base32 encoded", BASE32_ENCODED_USB_FINGERPRINT_LONG); 
        free(base32_usb_fingerprint);
}

static void usb_fingerprint_and_trusted_list()
{
        char *usb_fingrprnt = "foobar";
        int is_in_list = is_device_in_trust_list(PROVIDED_TRUSTED_LIST, usb_fingrprnt, TRUSTED_LIST_DELIMITER);
        cst_a("should ignore usb fingerprint (foobar) if is at trusted list", is_in_list == 1);
        usb_fingrprnt = "tar";
        is_in_list = is_device_in_trust_list(PROVIDED_CANARY_TOKEN, usb_fingrprnt, TRUSTED_LIST_DELIMITER);
        cst_a("should not ignore usb fingerprint (tar) if is not at trusted list", is_in_list == 0);
        usb_fingrprnt = "trufur";
        is_in_list = is_device_in_trust_list(PROVIDED_CANARY_TOKEN, usb_fingrprnt, TRUSTED_LIST_DELIMITER);
        cst_a("should not ignore usb fingerprint (trufur) if is not at trusted list", is_in_list == 0);
        usb_fingrprnt = "foobal";
        is_in_list = is_device_in_trust_list(PROVIDED_CANARY_TOKEN, usb_fingrprnt, TRUSTED_LIST_DELIMITER);
        cst_a("should not ignore usb fingerprint (foobal) if is not at trusted list", is_in_list == 0);
        usb_fingrprnt = "foObar";
        is_in_list = is_device_in_trust_list(PROVIDED_CANARY_TOKEN, usb_fingrprnt, TRUSTED_LIST_DELIMITER);
        cst_a("should not ignore usb fingerprint (foObar) if is not at trusted list", is_in_list == 0);
}

static void build_canary_dns_token_()
{
        char *canary_dns_token = (char *) malloc(strlen(BASE32_ENCODED_USB_FINGERPRINT) + 
                        strlen(MAGIC_STRING) + 2 + strlen(PROVIDED_CANARY_TOKEN));
        ConfigCanrayUSB opts = config_canary_usb_INIT;
        opts.canary_token = PROVIDED_CANARY_TOKEN;
        build_canary_dns_token(BASE32_ENCODED_USB_FINGERPRINT, canary_dns_token, &opts);
        cst_s(canary_dns_token, "should give", CANARY_DNS_TOKEN);
        free(canary_dns_token);
}

static void build_long_canary_dns_token_()
{
        char *canary_dns_token = (char *) malloc(strlen(BASE32_ENCODED_USB_FINGERPRINT_LONG) + 
                        strlen(MAGIC_STRING) + 2 + strlen(PROVIDED_CANARY_TOKEN));
        ConfigCanrayUSB opts = config_canary_usb_INIT;
        opts.canary_token = PROVIDED_CANARY_TOKEN;
        build_canary_dns_token(BASE32_ENCODED_USB_FINGERPRINT_LONG, canary_dns_token, &opts);
        cst_s(canary_dns_token, "should give", CANARY_DNS_TOKEN_LONG);
        free(canary_dns_token);
}

static void call_the_canary_() 
{
        cst_a("should return a non-error value", call_the_canary("google.com") == 0);
        cst_a("should return a nonzero error different than", call_the_canary("google.orc") != 0);
}

static void get_usb_fingerprint_()
{
        UsbAttrs usb_attrs = {"0000", "0000", "no", "no"};
        usb_attrs.vendor = ID_VENDOR;
        usb_attrs.product = ID_PRODUCT;
        usb_attrs.product_name = PRODUCT_NAME;
        usb_attrs.serial = SERIAL;
        size_t fingp_len = strlen(usb_attrs.vendor) + strlen(usb_attrs.product) + 
                strlen(usb_attrs.product_name) + strlen(usb_attrs.serial) + 5;
        char *usb_fingrprnt = (char*) malloc(fingp_len);
        check_memory_allocation(usb_fingrprnt);
        get_usb_fingerprint(usb_attrs, usb_fingrprnt);

        cst_s(usb_fingrprnt, "should build and retieve", USB_FINGERPRINT);
} 

static void all_tests()
{
        cst_run(config_file_reading_only_canary_token);
        cst_run(config_file_reading_canary_token_and_trusted_list);
        cst_run(command_line_arguments_canary_token);
        cst_run(command_line_arguments_canary_token_and_trusted_list);
        cst_run(dev_fingerprint_variable);
        cst_run(monitor_usb_variable);
        cst_run(monitor_sdcard_variable);
        cst_run(command_line_arguments_version);
        cst_run(get_canary_encoded_usb_fingerprint_test);
        cst_run(usb_fingerprint_and_trusted_list);
        cst_run(build_canary_dns_token_);
        cst_run(build_long_canary_dns_token_);
        cst_run(get_canary_encoded_usb_fingerprint_long_test);
        cst_run(call_the_canary_);
        cst_run(get_usb_fingerprint_);
}

int main(void)  
{
        all_tests();
        cst_results();
}

