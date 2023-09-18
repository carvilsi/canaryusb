/*
 * MIT License
 *
 * Copyright (c) 2023 Carlos Villanueva <char@omg.lol> 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#define SUBSYSTEM "usb"
#define DEVICE_TYPE "usb_device"
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
#define _VERSION_ "3.2.0"

#define BOLD_TEXT "\e[1m"
#define NO_BOLD_TEXT "\e[m"

#define MAX_PID_LEN 10

//These are only for testing reasons.
extern int usb_fingerprint;
extern int trusted_list;
extern char *canary_token;
extern char *trusted_list_value;
extern int kill_canaryusb;

void monitor_usb();
void free_canaries();
void parse_command_line(int argc, char *argv[]);
void parse_configuration_file();

