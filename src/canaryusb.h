#define SUBSYSTEM "usb"
#define TOTAL_MAX_BASE_32_MESSAGE_LENGTH 118
#define MAX_BASE_32_MESSAGE_LENGTH 59
#define CANARY_ON_ACTION "add"
#define CANARY_PORT "80"
#define MAGIC_STRING "G42"
#define TRUSTED_LIST_DELIMITER ","
#define REPLACE_THIS ' '
#define REPLACE_WITH '_' 
#define DOT "."
#define MAX_CANARY_TOKEN_LENGTH 200
#define MAX_TRUSTED_LIST_LENGTH 1040

#ifdef DEBUG                             
#define dprintf(...) printf (__VA_ARGS__)
#else                                    
#define dprintf(...) {}                  
#endif                                   

#define _NAME_ "canaryusb" 
#define _VERSION_ "2.0.0"

#define BOLD_TEXT "\e[1m"
#define NO_BOLD_TEXT "\e[m"

#define MAX_PID_LEN 10
