#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>
#include <syslog.h>
#include <systemd/sd-device.h>

#include "../canaryusb.h"
#include "../usbs/usbs.h"
#include "../utils/base32.h"
#include "../utils/util.h"

int call_the_canary(const char *canary_dns_token)
{
        int canaryrsp;
        struct addrinfo hints, *res;
        memset(&hints, 0, sizeof(hints));
        canaryrsp = getaddrinfo(canary_dns_token, CANARY_PORT, &hints, &res);
        return canaryrsp;
}

void 
build_canary_dns_token(char *b32usbfngp, char *canary_dns_token, 
                ConfigCanrayUSB *opts)
{
        if (strlen(b32usbfngp) < MAX_BASE_32_MESSAGE_LENGTH) {
                strcpy(canary_dns_token, b32usbfngp);
                strcat(canary_dns_token, DOT); 
        } else {
                memcpy(canary_dns_token, b32usbfngp, MAX_BASE_32_MESSAGE_LENGTH);
                strcat(canary_dns_token, DOT);
                memcpy(&canary_dns_token[MAX_BASE_32_MESSAGE_LENGTH + 1], 
                                &b32usbfngp[MAX_BASE_32_MESSAGE_LENGTH], 
                                MAX_BASE_32_MESSAGE_LENGTH);
                strcat(canary_dns_token, DOT);
        }
        strcat(canary_dns_token, MAGIC_STRING); 
        strcat(canary_dns_token, DOT);
        strcat(canary_dns_token, opts->canary_token);
        dprintf("canary_dns_token to send: %s\n", canary_dns_token);
}

void 
get_canary_encoded_usb_fingerprint(char *dev_fingprt, char *buf_sub_fingerptr) 
{
        size_t buflen = (size_t)TOTAL_MAX_BASE_32_MESSAGE_LENGTH + 1;
        base32_encode(buf_sub_fingerptr, &buflen, dev_fingprt, 
                        strlen(dev_fingprt)); 
}

void 
deal_with_canaries(char *base32_usb_fingprt, char *dev_fingrprnt, 
                ConfigCanrayUSB *opts)
{
        char *canary_dns_token = (char*) malloc(strlen(base32_usb_fingprt) + 
                        strlen(MAGIC_STRING) + 2 + strlen(opts->canary_token));
        check_memory_allocation(canary_dns_token);
        build_canary_dns_token(base32_usb_fingprt, canary_dns_token, opts);

        int canaryrsp = 0; 

#ifdef SILENCE
        canaryrsp = -1;
#else        
        canaryrsp = call_the_canary(canary_dns_token);
#endif

        free(canary_dns_token);
        
        if (canaryrsp > 0) {
                dprintf("ERROR canaryusb: When calling canary tokens site, "
                                "for connected device: %s, run it on debug mode "
                                "for more insights", dev_fingrprnt);
                syslog(LOG_ERR, "canaryusb errored when trying to advice about "
                                "new connected device%s", dev_fingrprnt);
        } else if (canaryrsp == -1) {
                dprintf("Executing on SILENCE and DEBUG mode do NOT call to \
                                canary site\n"); 
        } else {
                syslog(LOG_NOTICE, "canary token sent for connected device: %s", 
                                dev_fingrprnt);
                dprintf("canary token sent\n");
        }
}

