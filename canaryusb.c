#include <libudev.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <syslog.h>
#include <unistd.h>
#include <getopt.h>

#include "utils/base32.h"
#include "utils/trusted_list.h"

#define SUBSYSTEM "usb"
#define TOTAL_MAX_BASE_32_MESSAGE_LENGTH 118
#define MAX_BASE_32_MESSAGE_LENGTH 59
#define CANARY_ON_ACTION "add"
#define CANARY_PORT "80"
#define MAGIC_STRING "G42"
#define TRUSTED_LIST_DELIMITER ",\n"

#ifdef DEBUG                             
#define dprintf(...) printf (__VA_ARGS__)
#else                                    
#define dprintf(...) {}                  
#endif                                   

#define _NAME_ "canaryusb" 
#define _VERSION_ "1.1.0"

#define BOLD_TEXT "\e[1m"
#define NO_BOLD_TEXT "\e[m"

int usb_fingerprint = 0;
int trusted_list = 0;
char *canary_token;
char *trusted_list_value;

struct UsbAttr_s 
{
        char *vendor;
        char *product;
        char *product_name;
        char *serial;
} UsbAttrs_default = {"0000", "0000", "no", "no"};

typedef struct UsbAttr_s UsbAttrs; 

static int call_the_canary(const char *canary_dns_token) 
{
        int canaryrsp;
        struct addrinfo hints_1, *res_1;
        memset(&hints_1, '\0', sizeof(hints_1));
        canaryrsp = getaddrinfo(canary_dns_token, CANARY_PORT, &hints_1, &res_1);
        return canaryrsp;
}

static void get_usb_fingerprint(UsbAttrs usbattrs, char *usb_fingprt) 
{
        sprintf(usb_fingprt, "%s:%s-%s-%s", usbattrs.vendor, usbattrs.product, usbattrs.product_name, usbattrs.serial);
        dprintf("usb fingerprint: %s\n", usb_fingprt);
}

static void build_canary_dns_token(char *buf_sub_fingerptr, char *canary_dns_token) 
{
        if (strlen(buf_sub_fingerptr) > MAX_BASE_32_MESSAGE_LENGTH) {
                int i;
                for (i = 0; i < MAX_BASE_32_MESSAGE_LENGTH; i++) {
                        canary_dns_token[i] = buf_sub_fingerptr[i];
                }
                strcat(canary_dns_token, ".");
                for (i = MAX_BASE_32_MESSAGE_LENGTH + 1; i <= strlen(buf_sub_fingerptr); i++) {
                        canary_dns_token[i] = buf_sub_fingerptr[i - 1];
                }
        } else {
                strcpy(canary_dns_token, buf_sub_fingerptr);
        }
        strcat(canary_dns_token, "."); 
        strcat(canary_dns_token, MAGIC_STRING); 
        strcat(canary_dns_token, ".");
        strcat(canary_dns_token, canary_token);
        dprintf("canary_dns_token to send: %s\n", canary_dns_token);
}

static void get_canary_encoded_usb_fingerprint(char *usb_fingprt, char *buf_sub_fingerptr) 
{
        size_t buflen = (size_t)TOTAL_MAX_BASE_32_MESSAGE_LENGTH + 1;
        int size_enc = base32_encode(buf_sub_fingerptr, &buflen, usb_fingprt, strlen(usb_fingprt)); 
        dprintf("Encoded %d characters as: %s\n", size_enc, buf_sub_fingerptr);
}

static UsbAttrs get_usb_attributes(struct udev_device *dev) 
{
        UsbAttrs usbattr = UsbAttrs_default;
        const char *vendor = udev_device_get_sysattr_value(dev, "idVendor");
        if (vendor)
                usbattr.vendor = (char*)vendor;
        const char *product = udev_device_get_sysattr_value(dev, "idProduct");
        if (product)
                usbattr.product = (char*)product; 
        const char *product_name = udev_device_get_sysattr_value(dev, "product");
        if (product_name)
                usbattr.product_name = (char*)product_name;
        const char *serial = udev_device_get_sysattr_value(dev, "serial");
        if (serial)
                usbattr.serial = (char*)serial;
        return usbattr;
}

static void canary_usb(struct udev_device *dev) 
{
        UsbAttrs usbattrs = get_usb_attributes(dev);
        size_t fingp_len = strlen(usbattrs.vendor) + strlen(usbattrs.product) + strlen(usbattrs.product_name) + strlen(usbattrs.serial) + 5;
        char *usb_fingrprnt = (char*)malloc(fingp_len);
        check_memory_allocation(usb_fingrprnt);

        dprintf("USB device with %s name and vendor/product %s:%s and %s serial: connected at: %s\n",
                        usbattrs.product_name,
                        usbattrs.vendor,
                        usbattrs.product,
                        usbattrs.serial,
                        udev_device_get_devnode(dev));
       
        get_usb_fingerprint(usbattrs, usb_fingrprnt);
        char *base32_usb_fingprt = (char*)malloc(TOTAL_MAX_BASE_32_MESSAGE_LENGTH + 1);
        check_memory_allocation(base32_usb_fingprt);
        get_canary_encoded_usb_fingerprint(usb_fingrprnt, base32_usb_fingprt);
        // if we want to the related fingerprint with the connected usb, print it!
        // else, not call canary token
        if (usb_fingerprint) {
                printf("usb_fingerprint: %s\n", usb_fingrprnt);
                printf("another: %s\n", base32_usb_fingprt);
        } else {
                char *canary_dns_token = (char*)malloc(strlen(base32_usb_fingprt) + strlen(MAGIC_STRING) + 2 + strlen(canary_token));
                check_memory_allocation(canary_dns_token);
                build_canary_dns_token(base32_usb_fingprt, canary_dns_token);

                /*int canaryrsp = call_the_canary(canary_dns_token);*/
/*TODO: remove this*/
                free(canary_dns_token);
                int canaryrsp = 0;
                if (canaryrsp != 0) {
                        dprintf("ERROR canaryusb: When calling canary tokens site, for connected USB: %s, run it on debug mode for more insights", usb_fingrprnt);
                        syslog(LOG_ERR, "canaryusb errored when trying to advice about new connected USB %s", usb_fingrprnt);
                } else {
                        syslog(LOG_NOTICE, "canary token sent for connected USB device: %s", usb_fingrprnt);
                        dprintf("canary token sent");
                }
        }
        free(usb_fingrprnt);
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
        printf("\t\tthis prints the fingerprint related with a USB device that  is plugged into computer\n");
        printf("\t\tand could be used to create the list for " BOLD_TEXT "trust_list" NO_BOLD_TEXT " option.\n");
        printf("\t\tIn this mode, will not be any call to Canary Tokens, only the usb fingerprint will be printed.\n");
        printf(BOLD_TEXT "-t, --trust_list [comma separated usb_fingerprint list]\n" NO_BOLD_TEXT);
        printf("\t\tlist of usb fingerprints, comma seprated, to not notify when the related deviced is connected\n");
        printf("\t\tcheck " BOLD_TEXT "usb_fingerprint" NO_BOLD_TEXT " option to retrieve device fingerprint for connected USB device\n");

        exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{       
     	int c;
	
        if (argc < 2) {
                fprintf(stderr, "ERROR: missing parameters, check the usage: \n");
                show_help();
        }

    	while (1) {
                int option_index = 0;

                c = getopt_long(argc, argv, "uht:c:", long_options, &option_index);
                if (c == -1)
                        break;

                switch (c) {
                            case 't':
                                    trusted_list = 1;
                                    trusted_list_value = (char*) malloc(strlen(optarg)+1);
                                    check_memory_allocation(trusted_list_value);
                                    trusted_list_value = strcpy(trusted_list_value, optarg);
                                    break;
                            case 'h':
                                    show_help();
                                    break;
                            case 'u':
                                    usb_fingerprint = 1;
                                    break;
                            case 'c':
                                    canary_token = (char*)malloc(strlen(optarg)+1);
                                    check_memory_allocation(canary_token);
                                    canary_token = strcpy(canary_token, optarg);
                                    break;
                            case '?':
                                    show_help();
                                    break;
                            default:
                                    printf("?? getopt returned character code 0%o ??\n", c);
                }
        }       
        
        struct udev* udev = udev_new();
        if (!udev) {
                fprintf(stderr, "udev error\n");
                exit(EXIT_FAILURE); 
        }

        /*TODO: needs to not duplicate daemon*/
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

        printf("The list: %s\n", trusted_list_value);
        const int is_there = is_usb_device_in_trust_list(trusted_list_value, "foobar", TRUSTED_LIST_DELIMITER);
        printf("Is there? %s\n", is_there ? "yes" : "no"); 
        printf("the canary token: %s\n", canary_token);


        monitor_usb(udev);
        udev_unref(udev);
        
        if (canary_token != NULL)
                free(canary_token);

        if (trusted_list_value != NULL)
                free(trusted_list_value);
        
        return EXIT_SUCCESS;
}
