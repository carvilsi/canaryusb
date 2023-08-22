#include <libudev.h>

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
#define CONFIG_FILE ".config/canaryusb/config.toml"

// Argument check types
#define CANARYTOKEN 0
#define TRUSTEDLIST 1

#define val_name(v)#v

#ifdef DEBUG                             
#define dprintf(...) printf (__VA_ARGS__)
#else                                    
#define dprintf(...) {}                  
#endif                                   

#define _NAME_ "canaryusb" 
#define _VERSION_ "2.1.2"

#define BOLD_TEXT "\e[1m"
#define NO_BOLD_TEXT "\e[m"

#define MAX_PID_LEN 10

//These are only for testing reasons.
extern int usb_fingerprint;
extern int trusted_list;
extern char *canary_token;
extern char *trusted_list_value;

void monitor_usb(struct udev* udev);
void free_canaries();
void parse_command_line(int argc, char *argv[]);
void parse_configuration_file();

