#include <libudev.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <getopt.h>

#include "canaryusb.h"
#include "usbs/usbs.h"
#include "canary/canaries.h"
#include "utils/trusted_list.h"
#include "utils/util.h"

int usb_fingerprint = 0;
int trusted_list = 0;
char *canary_token;
char *trusted_list_value;

static void canary_usb(struct udev_device *dev) 
{
        UsbAttrs usbattrs = get_usb_attributes(dev);
        size_t fingp_len = strlen(usbattrs.vendor) + strlen(usbattrs.product) + strlen(usbattrs.product_name) + strlen(usbattrs.serial) + 5;

        dprintf("USB device with %s name and vendor/product %s:%s and %s serial. Connected at: %s\n",
                        usbattrs.product_name,
                        usbattrs.vendor,
                        usbattrs.product,
                        usbattrs.serial,
                        udev_device_get_devnode(dev));
       
        
        char tmp_usb_fingprt[fingp_len];
        char *usb_fingrprnt = get_usb_fingerprint(usbattrs, tmp_usb_fingprt);
        char *base32_usb_fingprt = (char *) malloc(TOTAL_MAX_BASE_32_MESSAGE_LENGTH + 1);
        check_memory_allocation(base32_usb_fingprt);
        get_canary_encoded_usb_fingerprint(usb_fingrprnt, base32_usb_fingprt);

        // Check if we have a trusted list and the device is in the list.
        int is_in_list = 0;
        if (trusted_list) {
                is_in_list = is_usb_device_in_trust_list(trusted_list_value, usb_fingrprnt, TRUSTED_LIST_DELIMITER);
        }

        // if we want to the related fingerprint with the connected usb, print it!
        // else, call canary token
        if (usb_fingerprint) {
                printf("usb_fingerprint: %s\n", usb_fingrprnt);
        } else {
                if (is_in_list) {
                        syslog(LOG_NOTICE, "usb device: %s connected, but is at trusted list, not calling canary token", usb_fingrprnt);
                        dprintf("usb device: %s connected, but is at trusted list, not calling canary token\n", usb_fingrprnt);
                } else {
                        deal_with_canaries(base32_usb_fingprt, usb_fingrprnt, canary_token); 
                }
        }
        free(base32_usb_fingprt);
}

static void monitor_usb(struct udev* udev) 
{
        struct udev_monitor* mon = udev_monitor_new_from_netlink(udev, "udev");

        udev_monitor_filter_add_match_subsystem_devtype(mon, SUBSYSTEM, NULL);
        udev_monitor_enable_receiving(mon);

        int fd = udev_monitor_get_fd(mon);

        while (1) {
                fd_set fds;
                FD_ZERO(&fds);
                FD_SET(fd, &fds);

                int ret = select(fd+1, &fds, NULL, NULL, NULL);
                if (ret <= 0)
                        break;

                if (FD_ISSET(fd, &fds)) {
                        struct udev_device* dev = udev_monitor_receive_device(mon);
                        if (dev) {
                                if (udev_device_get_devnode(dev)) {
                                        const char* action = udev_device_get_action(dev);
                                        if (action != NULL) {
                                                if (strcmp(action,CANARY_ON_ACTION) == 0) 
                                                        canary_usb(dev);
                                        }

                                }
                        }
                }
        }
}

static struct option long_options[] = 
{
       {"trust_list", required_argument, 0, 't'},
       {"usb_fingerprint", no_argument, 0, 'u'},
       {"canary_token", required_argument, 0, 'c'},
       {"help", no_argument, 0, 'h'},
       {0, 0 , 0, 0}
};

static void parse_command_line(int argc, char *argc[], int *trstlst, int *usbfngr, char *ctrstlst, char *cusbfngr, char *cnrytkn)
{
        while (1) {
                int option_index = 0;

                c = getopt_long(argc, argv, "uht:c:", long_options, &option_index);
                if (c == -1)
                        break;

                switch (c) {
                            case 't':
                                    trusted_list = 1;
                                    trstlst = 1;
                                    if ((strlen(optarg)+1) > MAX_TRUSTED_LIST_LENGTH) {
                                          fprintf(stderr, "The trusted list characters exceeds the limit of %d\n", MAX_TRUSTED_LIST_LENGTH);
                                          exit(EXIT_FAILURE);
                                    }
                                    trusted_list_value = (char*) malloc(strlen(optarg)+1);
                                    check_memory_allocation(trusted_list_value);
                                    trusted_list_value = strcpy(trusted_list_value, optarg);
                                    ctrstlst = trusted_list_value;
                                    break;
                            case 'h':
                                    show_help();
                                    break;
                            case 'u':
                                    usb_fingerprint = 1;
                                    usbfngr = 1;
                                    break;
                            case 'c':
                                    if ((strlen(optarg)+1) > MAX_CANARY_TOKEN_LENGTH) {
                                          fprintf(stderr, "The canary token characters exceeds the limit of %d\n", MAX_CANARY_TOKEN_LENGTH);
                                          exit(EXIT_FAILURE);
                                    }
                                    canary_token = (char *) malloc(strlen(optarg)+1);
                                    check_memory_allocation(canary_token);
                                    canary_token = strcpy(canary_token, optarg);
                                    cnrytkn = canary_token;
                                    break;
                            case '?':
                                    show_help();
                                    break;
                            default:
                                    printf("?? getopt returned character code 0%o ??\n", c);
                }
        }
}

int main(int argc, char *argv[])
{       
     	int c;

        if (argc < 2) {
                int cfr = config_file_handler();
                printf("config file res: %d\n", cfr);
                exit(1);
                //TODO: check if the config file exists and if it has values
                //if exists and has values, use these as parameters
                //otherwise is this error
                fprintf(stderr, "ERROR: missing parameters, check the usage: \n");
                show_help();
        } else {
                parse_command_line(argc, argv);
        }
        
        struct udev* udev = udev_new();
        if (!udev) {
                fprintf(stderr, "udev error\n");
                exit(EXIT_FAILURE); 
        }

        check_if_running();

        pid_t pid;
        pid = fork();

        if (pid < 0)
                exit(EXIT_FAILURE);
        if (pid > 0) 
                exit(EXIT_SUCCESS);
        if (setsid() < 0)
                exit(EXIT_FAILURE);

        dprintf("%s daemon started\n", _NAME_);
        syslog(LOG_NOTICE, "%s daemon started", _NAME_);

        monitor_usb(udev);
        udev_unref(udev);
        
        if (canary_token != NULL)
                free(canary_token);

        if (trusted_list_value != NULL)
                free(trusted_list_value);
        
        return EXIT_SUCCESS;
}

