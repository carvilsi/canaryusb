#include <libudev.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <syslog.h>
#include <unistd.h>

#include "base32.h"

#define NAME_VERSION "canaryusb_v1.0.0" 

#define SUBSYSTEM "usb"
#define TOTAL_MAX_BASE_32_MESSAGE_LENGTH 118
#define MAX_BASE_32_MESSAGE_LENGTH 59
#define CANARY_ON_ACTION "add"
#define CANARY_PORT "80"
#define MAGIC_STRING "G42"

#ifdef DEBUG                             
#define dprintf(...) printf (__VA_ARGS__)
#else                                    
#define dprintf(...) {}                  
#endif                                   

char *canary_token;

struct UsbAttr_s {
        char *vendor;
        char *product;
        char *product_name;
        char *serial;
} UsbAttrs_default = {"0000", "0000", "no", "no"};

typedef struct UsbAttr_s UsbAttrs; 

static int call_the_canary(const char *canary_dns_token) {
        int canaryrsp;
        struct addrinfo hints_1, *res_1;
        memset(&hints_1, '\0', sizeof(hints_1));
        canaryrsp = getaddrinfo(canary_dns_token, CANARY_PORT, &hints_1, &res_1);
        return canaryrsp;
}

static char *get_usb_fingerprint(UsbAttrs usbattrs) {
        size_t fingp_len = strlen(usbattrs.vendor) + strlen(usbattrs.product) + strlen(usbattrs.product_name) + strlen(usbattrs.serial) + 5;
        char *usb_fingprt = (char*)malloc(fingp_len);
        sprintf(usb_fingprt, "%s:%s-%s-%s", usbattrs.vendor, usbattrs.product, usbattrs.product_name, usbattrs.serial);
        dprintf("usb fingerprint: %s\n", usb_fingprt);
        return usb_fingprt;
}

static char *build_canary_dns_token(char *buf_sub_fingerptr) {
        char *canary_dns_token = (char*)malloc(strlen(buf_sub_fingerptr) + strlen(MAGIC_STRING) + 2 + strlen(canary_token));
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
        return canary_dns_token;
}

static char *get_canary_encoded_usb_fingerprint(char *usb_fingprt) {
        char *buf_sub_fingerptr = (char*)malloc(TOTAL_MAX_BASE_32_MESSAGE_LENGTH + 1);
        size_t buflen = (size_t)TOTAL_MAX_BASE_32_MESSAGE_LENGTH + 1;
        int size_enc = base32_encode(buf_sub_fingerptr, &buflen, usb_fingprt, strlen(usb_fingprt)); 
        dprintf("Encoded %d characters as: %s\n", size_enc, buf_sub_fingerptr);
        return buf_sub_fingerptr;
}

static UsbAttrs get_usb_attributes(struct udev_device *dev) {
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

static void canary_usb(struct udev_device *dev) {
        UsbAttrs usbattrs = get_usb_attributes(dev); 

        dprintf("USB device with %s name and vendor/product %s:%s and %s serial: connected at: %s\n",
                        usbattrs.product_name,
                        usbattrs.vendor,
                        usbattrs.product,
                        usbattrs.serial,
                        udev_device_get_devnode(dev));
       
        char *usb_fingprt = get_usb_fingerprint(usbattrs);
        char *base32_usb_fingprt = get_canary_encoded_usb_fingerprint(usb_fingprt);
        char *canary_dns_token = build_canary_dns_token(base32_usb_fingprt);
        int canaryrsp = call_the_canary(canary_dns_token);
        if (canaryrsp != 0) {
                dprintf("ERROR canaryusb: When calling canary tokens site, for connected USB: %s, run it on debug mode for more insights", usb_fingprt);
                syslog(LOG_ERR, "canaryusb errored when trying to advice about new connected USB %s", usb_fingprt);
        } else {
                syslog(LOG_NOTICE, "canary token sent for connected USB device: %s", usb_fingprt);
                dprintf("canary token sent");
        }
}

static void monitor_usb(struct udev* udev) {
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

int main(int argc, char *argv[]) {
        if (argc < 2) {
                fprintf(stderr, "ERROR: missing parameters: needs a canary DNS token\n");
                exit(EXIT_FAILURE); 
        }
        
        canary_token = (char*)malloc(strlen(argv[1])+1);
        canary_token = argv[1];
        
        struct udev* udev = udev_new();
        if (!udev) {
                fprintf(stderr, "udev error\n");
                exit(EXIT_FAILURE); 
        }

        pid_t pid;
        pid = fork();

        if (pid < 0)
                exit(EXIT_FAILURE);
        if (pid > 0) 
                exit(EXIT_SUCCESS);
        if (setsid() < 0)
                exit(EXIT_FAILURE);

        dprintf("%s daemon started\n", NAME_VERSION);
        syslog(LOG_NOTICE, "%s daemon started", NAME_VERSION);

        monitor_usb(udev);
        udev_unref(udev);
        
        return EXIT_SUCCESS;
}
