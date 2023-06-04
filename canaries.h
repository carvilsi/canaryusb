static int call_the_canary(const char *canary_dns_token);
static void build_canary_dns_token(char *buf_sub_fingerptr, char *canary_dns_token, char *canary_token);
void get_canary_encoded_usb_fingerprint(char *usb_fingprt, char *buf_sub_fingerptr);
void deal_with_canaries(char *base32_usb_fingprt, char *usb_fingrprnt, char *canary_token);
