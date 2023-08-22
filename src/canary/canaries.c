#include <stdlib.h>
#include <string.h>
#include <libudev.h>
#include <stdio.h>
#include <netdb.h>
#include <syslog.h>

#include "../canaryusb.h"
#include "../usbs/usbs.h"
#include "../utils/base32.h"
#include "../utils/util.h"

static int call_the_canary(const char *canary_dns_token) 
{
        int canaryrsp;
        struct addrinfo hints_1, *res_1;
        memset(&hints_1, '\0', sizeof(hints_1));
        canaryrsp = getaddrinfo(canary_dns_token, CANARY_PORT, &hints_1, &res_1);
        return canaryrsp;
}

static void build_canary_dns_token(char *buf_sub_fingerptr, char *canary_dns_token, char *canary_token) 
{
        if (strlen(buf_sub_fingerptr) > MAX_BASE_32_MESSAGE_LENGTH) {
                int i;
                for (i = 0; i < MAX_BASE_32_MESSAGE_LENGTH; i++) {
                        canary_dns_token[i] = buf_sub_fingerptr[i];
                }
                strcat(canary_dns_token, DOT);
                for (i = MAX_BASE_32_MESSAGE_LENGTH + 1; i <= strlen(buf_sub_fingerptr); i++) {
                        canary_dns_token[i] = buf_sub_fingerptr[i - 1];
                }
        } else {
                strcpy(canary_dns_token, buf_sub_fingerptr);
        }
        strcat(canary_dns_token, DOT); 
        strcat(canary_dns_token, MAGIC_STRING); 
        strcat(canary_dns_token, DOT);
        strcat(canary_dns_token, canary_token);
        dprintf("canary_dns_token to send: %s\n", canary_dns_token);
}

void get_canary_encoded_usb_fingerprint(char *usb_fingprt, char *buf_sub_fingerptr) 
{
        size_t buflen = (size_t)TOTAL_MAX_BASE_32_MESSAGE_LENGTH + 1;
        int size_enc = base32_encode(buf_sub_fingerptr, &buflen, usb_fingprt, strlen(usb_fingprt)); 
        dprintf("Encoded %d characters as: %s\n", size_enc, buf_sub_fingerptr);
}

void deal_with_canaries(char *base32_usb_fingprt, char *usb_fingrprnt, char *canary_token)
{
        char *canary_dns_token = (char *) malloc(strlen(base32_usb_fingprt) + strlen(MAGIC_STRING) + 2 + strlen(canary_token));
        check_memory_allocation(canary_dns_token);
        build_canary_dns_token(base32_usb_fingprt, canary_dns_token, canary_token);

        int canaryrsp = 0; 
#ifdef SILENCE
        canaryrsp = -1;
#else        
        canaryrsp = call_the_canary(canary_dns_token);
#endif
        free(canary_dns_token);
        
        if (canaryrsp > 0) {
                dprintf("ERROR canaryusb: When calling canary tokens site, for connected USB: %s, run it on debug mode for more insights", usb_fingrprnt);
                syslog(LOG_ERR, "canaryusb errored when trying to advice about new connected USB %s", usb_fingrprnt);
        } else if (canaryrsp == -1) {
                dprintf("Executing on SILENCE and DEBUG mode do NOT call to canary site\n"); 
        } else {
                syslog(LOG_NOTICE, "canary token sent for connected USB device: %s", usb_fingrprnt);
                dprintf("canary token sent\n");
        }
}

