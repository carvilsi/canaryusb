/*
 * MIT License
 *
 * Copyright (c) 2023 Carlos Villanueva <char@omg.lol> 
 * https://github.com/carvilsi/canaryusb
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

int dev_fingerprint = 0;
int trusted_list = 0;
int kill_canaryusb = 0;
char *canary_token;
char *trusted_list_value;

static char *get_device_fingerprint(sd_device *dev, const char *subsystem)
{
        char *dev_fngrprnt;
        
        if (strcmp(USB_SUBSYSTEM, subsystem) == 0) {
                UsbAttrs usb_attrs = get_usb_attributes(dev);
                size_t usb_fngrp_len = strlen(usb_attrs.vendor) + 
                        strlen(usb_attrs.product) + 
                        strlen(usb_attrs.product_name) + 
                        strlen(usb_attrs.serial) + 5;
                char tmp_usb_fngrp[usb_fngrp_len];
                dev_fngrprnt = get_usb_fingerprint(usb_attrs, tmp_usb_fngrp);
        } 
 
        if (strcmp(SDCARD_SUBSYSTEM, subsystem) == 0) {
                SDCardAttrs sdcrd_attrs = get_sdcard_attributes(dev);
                size_t sdcrd_fngrp_len = strlen(sdcrd_attrs.id_name) +
                        strlen(sdcrd_attrs.id_serial) +
                        strlen(sdcrd_attrs.size) +
                        strlen(sdcrd_attrs.blcksz_prtbltype) + 5;
                char tmp_sdcrd_fngrp[sdcrd_fngrp_len];
                dev_fngrprnt = get_sdcard_fingerprint(sdcrd_attrs, tmp_sdcrd_fngrp);
        } 

        return dev_fngrprnt;
}

static int device_monitor_handler(sd_device_monitor *m, sd_device *dev, void *userdata) 
{
        sd_device_action_t actions;
        sd_device_get_action(dev, &actions);
        
        if (actions == SD_DEVICE_ADD) {
                const char *subsystem;
                sd_device_get_subsystem(dev, &subsystem);

                char *dev_fngrprnt;
                dev_fngrprnt = get_device_fingerprint(dev, subsystem);

                char *base32_fngrprnt = (char *) malloc(TOTAL_MAX_BASE_32_MESSAGE_LENGTH + 1);
                check_memory_allocation(base32_fngrprnt);
                get_canary_encoded_usb_fingerprint(dev_fngrprnt, base32_fngrprnt);

                // Check if we have a trusted list and the device is in the list.
                int is_in_list = 0;
                if (trusted_list) {
                        is_in_list = is_device_in_trust_list(trusted_list_value, dev_fngrprnt, 
                                        TRUSTED_LIST_DELIMITER);
                }

                // if we want to the related fingerprint with the connected usb, print it!
                // else, call canary token
                if (dev_fingerprint) {
                        printf("%s fingerprint: %s\n", strcmp(subsystem, SDCARD_SUBSYSTEM) == 0 ? "SDCard" : subsystem, dev_fngrprnt);
                } else {
                        if (is_in_list) {
                                syslog(LOG_NOTICE, 
                                                "%s device: %s connected, but is at trusted list, not calling canary token", 
                                                subsystem, dev_fngrprnt);
                                dprintf("%s device: %s connected, but is at trusted list, not calling canary token\n", 
                                                subsystem, dev_fngrprnt);
                        } else {
                                deal_with_canaries(base32_fngrprnt, dev_fngrprnt, canary_token); 
                        }
                }

                free(base32_fngrprnt);
        }

        return 0;
}

void monitor_devices() 
{
        int r;
        sd_device_monitor *sddm = NULL;
        r = sd_device_monitor_new(&sddm);
        if (r < 0) 
                goto finish;

        r = sd_device_monitor_filter_add_match_subsystem_devtype(sddm, USB_SUBSYSTEM, USB_DEVICE_TYPE);
        if (r < 0)
                goto finish;

        r = sd_device_monitor_filter_add_match_subsystem_devtype(sddm, SDCARD_SUBSYSTEM, SDCARD_DEVICE_TYPE);
        if (r < 0)
                goto finish;

        r = sd_device_monitor_start(sddm, device_monitor_handler, NULL);
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
       {"kill", no_argument, 0, 'k'},
       {0, 0 , 0, 0}
};

void parse_command_line(int argc, char *argv[])
{
        int c;
        while (1) {
                int option_index = 0;

                c = getopt_long(argc, argv, "hukt:c:", long_options, &option_index);
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
                                    dev_fingerprint = 1;
                                    break;
                            case 'k':
                                    kill_canaryusb = 1;
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

