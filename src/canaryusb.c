#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <getopt.h>
#include <systemd/sd-event.h>
#include <systemd/sd-device.h>
#include <errno.h>

#include "canaryusb.h"
#include "usbs/usbs.h"
#include "canary/canaries.h"
#include "utils/trusted_list.h"
#include "utils/util.h"

int usb_fingerprint = 0;
int trusted_list = 0;
char *canary_token;
char *trusted_list_value;

static int usb_monitor_handler(sd_device_monitor *m, sd_device *dev, void *userdata) 
{
        sd_device_action_t actions;
        sd_device_get_action(dev, &actions);

        if (actions == SD_DEVICE_ADD) {
                UsbAttrs usbattrs = get_usb_attributes(dev);
                size_t fingp_len = strlen(usbattrs.vendor) + 
                        strlen(usbattrs.product) + 
                        strlen(usbattrs.product_name) + 
                        strlen(usbattrs.serial) + 5;

                dprintf("USB device with %s name and vendor/product %s:%s and %s serial. \
                                Connected at: %s\n",
                                usbattrs.product_name,
                                usbattrs.vendor,
                                usbattrs.product,
                                usbattrs.serial,
                                usbattrs.syspath);
                
                char tmp_usb_fingprt[fingp_len];
                char *usb_fingrprnt = get_usb_fingerprint(usbattrs, tmp_usb_fingprt);
                char *base32_usb_fingprt = (char *) malloc(TOTAL_MAX_BASE_32_MESSAGE_LENGTH + 1);
                check_memory_allocation(base32_usb_fingprt);
                get_canary_encoded_usb_fingerprint(usb_fingrprnt, base32_usb_fingprt);

                // Check if we have a trusted list and the device is in the list.
                int is_in_list = 0;
                if (trusted_list) {
                        is_in_list = is_usb_device_in_trust_list(trusted_list_value, usb_fingrprnt, 
                                        TRUSTED_LIST_DELIMITER);
                }

                // if we want to the related fingerprint with the connected usb, print it!
                // else, call canary token
                if (usb_fingerprint) {
                        printf("usb_fingerprint: %s\n", usb_fingrprnt);
                } else {
                        if (is_in_list) {
                                syslog(LOG_NOTICE, 
                                                "usb device: %s connected, but is at trusted list, not calling canary token", 
                                                usb_fingrprnt);
                                dprintf("usb device: %s connected, but is at trusted list, not calling canary token\n", 
                                                usb_fingrprnt);
                        } else {
                                deal_with_canaries(base32_usb_fingprt, usb_fingrprnt, canary_token); 
                        }
                }
                free(base32_usb_fingprt);
                
        }

        return 0;
}

void monitor_usb() 
{
        int r;
        sd_device_monitor *sddm = NULL;
        r = sd_device_monitor_new(&sddm);
        if (r < 0) 
                goto finish;

        r = sd_device_monitor_filter_add_match_subsystem_devtype(sddm, SUBSYSTEM, DEVICE_TYPE);
        if (r < 0)
                goto finish;

        r = sd_device_monitor_start(sddm, usb_monitor_handler, NULL);
        if (r < 0) 
                goto finish;

        sd_event *event = sd_device_monitor_get_event(sddm);
        r = sd_event_loop(event);
        if (r < 0)
                goto finish;

finish:
        if (r < 0) {
                errno = -r;
                fprintf(stderr, "Error: %m\n");
        }
        sd_device_monitor_stop(sddm);
        sd_device_monitor_unref(sddm);
        sd_event_unref(event);
        if (r < 0) 
                exit(EXIT_FAILURE);
        else
                exit(EXIT_SUCCESS);
}

static struct option long_options[] = 
{
       {"trust_list", required_argument, 0, 't'},
       {"usb_fingerprint", no_argument, 0, 'u'},
       {"canary_token", required_argument, 0, 'c'},
       {"help", no_argument, 0, 'h'},
       {0, 0 , 0, 0}
};

void parse_command_line(int argc, char *argv[])
{
        int c;
        while (1) {
                int option_index = 0;

                c = getopt_long(argc, argv, "hut:c:", long_options, &option_index);
                if (c == -1)
                        break;

                switch (c) {
                            case 't':
                                    trusted_list = 1;
                                    check_argument_length(optarg, TRUSTEDLIST);
                                    trusted_list_value = (char *) malloc(strlen(optarg)+1);
                                    check_memory_allocation(trusted_list_value);
                                    strcpy(trusted_list_value, optarg);
                                    break;
                            case 'h':
                                    show_help();
                                    break;
                            case 'u':
                                    usb_fingerprint = 1;
                                    break;
                            case 'c':
                                    check_argument_length(optarg, CANARYTOKEN);
                                    canary_token = (char *) malloc(strlen(optarg)+1);
                                    check_memory_allocation(canary_token);
                                    strcpy(canary_token, optarg);
                                    break;
                            case '?':
                                    show_help();
                                    break;
                            default:
                                    printf("?? getopt returned character code 0%o ??\n", c);
                }
        }
}

void parse_configuration_file()
{
        canary_token = (char *) malloc(MAX_CANARY_TOKEN_LENGTH);
        trusted_list_value = (char *) malloc(MAX_TRUSTED_LIST_LENGTH);
        check_memory_allocation(canary_token);
        check_memory_allocation(trusted_list_value);
        config_file_handler(canary_token, trusted_list_value);
        
        if (strcmp(trusted_list_value, "") == 0) {
                trusted_list = 0;
                trusted_list_value = NULL;
        } else {
                trusted_list = 1;
        }
}

void free_canaries()
{
        if (canary_token != NULL)
                free(canary_token);

        if (trusted_list_value != NULL)
                free(trusted_list_value);
}

