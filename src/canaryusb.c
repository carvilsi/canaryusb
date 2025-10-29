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
#include <stdbool.h>

#include "canaryusb.h"
#include "usbs/usbs.h"
#include "canary/canaries.h"
#include "utils/trusted_list.h"
#include "utils/util.h"

static int device_monitor_handler(sd_device_monitor *m, sd_device *dev, void *userdata) 
{
        ConfigCanrayUSB *opts = (ConfigCanrayUSB*) userdata;
        sd_device_action_t actions;
        sd_device_get_action(dev, &actions);
        if (actions == SD_DEVICE_ADD) {
                const char *subsystem;
                sd_device_get_subsystem(dev, &subsystem);

                char *dev_fngrprnt = get_device_fingerprint(dev, subsystem);

                char *base32_fngrprnt = (char *)malloc(TOTAL_MAX_BASE_32_MESSAGE_LENGTH + 1);
                check_memory_allocation(base32_fngrprnt);
                get_canary_encoded_usb_fingerprint(dev_fngrprnt, base32_fngrprnt);

                // Check if we have a trusted list and the device is in the list.
                int is_in_list = 0;
                if (opts->trusted_list) {
                        is_in_list = is_device_in_trust_list(opts->trusted_list_value, 
                                        dev_fngrprnt, TRUSTED_LIST_DELIMITER);
                }

                // if we want to the related fingerprint with the connected usb, print it!
                // else, call canary token
                if (opts->dev_fingerprint) {
                        printf("%s fingerprint: %s\n", 
                                        strcmp(subsystem, SDCARD_SUBSYSTEM) == 0 ? "SDCard" : 
                                        subsystem, dev_fngrprnt);
                } else {
                        if (is_in_list) {
                                syslog(LOG_NOTICE, 
                                                "%s device: %s connected, but is at trusted " 
                                                "list, not calling canary token", 
                                                subsystem, dev_fngrprnt);
                                dprintf("%s device: %s connected, but is at trusted list, "
                                                "not calling canary token\n", 
                                                subsystem, dev_fngrprnt);
                        } else {
                                deal_with_canaries(base32_fngrprnt, dev_fngrprnt, opts); 

                                // check if we want to de-authorize the not in list device
                                // and is a USB
                                // and de-authorize it and so some loggin
                                // TODO: maybe add the de-authorized device to a list on file
                                // XXX: note that right now only support de-authorization for USB
                                if (opts->deauth_dev && strcmp(USB_SUBSYSTEM, subsystem) == 0) {
                                        char *de_auth_syspath = get_device_authorize_syspath(dev, subsystem);
                                        
                                        FILE *auth_file = fopen(de_auth_syspath, "w");
                                        if (auth_file != NULL) {
                                                fprintf(auth_file, "0");
                                                fclose(auth_file);
                                                syslog(LOG_NOTICE,
                                                        "%s device: %s connected and de-authorized "
                                                        "at:\n\t %s\n",
                                                        subsystem, dev_fngrprnt, de_auth_syspath);
                                                dprintf("%s device: %s connected and de-authorized "
                                                        "at:\n\t %s\n",
                                                        subsystem, dev_fngrprnt, de_auth_syspath);
                                        } else {
                                                syslog(LOG_ERR, 
                                                       "not possible to write at "
                                                       "authorized file: %s "
                                                       "Could possible that you don't "
                                                       "have enough permission, try "
                                                       "to run it as sudoer",
                                                       de_auth_syspath);
                                                dprintf("not possible to write at "
                                                       "authorized file: %s "
                                                       "Could possible that you don't "
                                                       "have enough permission, try "
                                                       "to run it as sudoer",
                                                       de_auth_syspath);
                                        }

                                        free(de_auth_syspath);
                                }
                        }
                }
                free(base32_fngrprnt);
                free(dev_fngrprnt);
        }

        return 0;
}

void monitor_devices(ConfigCanrayUSB *opts) 
{
        int r;
        sd_device_monitor *sddm = NULL;
        r = sd_device_monitor_new(&sddm);
        if (r < 0) 
                goto finish;

        if (opts->monitor_usb || (!opts->monitor_usb && !opts->monitor_sdcard)) {
                r = sd_device_monitor_filter_add_match_subsystem_devtype(sddm, 
                        USB_SUBSYSTEM, USB_DEVICE_TYPE);
                dprintf("Monitoring USB devices\n");
                syslog(LOG_NOTICE, "Monitoring USB devices");

                if (r < 0)
                        goto finish;
        }

        if (opts->monitor_sdcard || (!opts->monitor_usb && !opts->monitor_sdcard)) {
                r = sd_device_monitor_filter_add_match_subsystem_devtype(sddm, 
                                SDCARD_SUBSYSTEM, SDCARD_DEVICE_TYPE);
                dprintf("Monitoring SDCard devices\n");
                syslog(LOG_NOTICE, "Monitoring SDCard devices");

                if (r < 0)
                        goto finish;
        }

        if (opts->deauth_dev) {
                dprintf("de-authorize device enable\n");
                syslog(LOG_NOTICE, "de-authorize device enable");

                /*if (r < 0)*/
                        /*goto finish;*/
        }

        r = sd_device_monitor_start(sddm, device_monitor_handler, opts);
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
       {"trust-list", required_argument, 0, 't'},
       {"fingerprint-device", no_argument, 0, 'f'},
       {"usb-monitor", no_argument, 0, 'u'},
       {"sdcard-monitor", no_argument, 0, 's'},
       {"canary-token", required_argument, 0, 'c'},
       {"help", no_argument, 0, 'h'},
       {"kill", no_argument, 0, 'k'},
       {"version", no_argument, 0, 'v'},
       {"de-authorize-device", no_argument, 0, 'd'},
       {0, 0 , 0, 0}
};

void parse_command_line(int argc, char *argv[], ConfigCanrayUSB *opts)
{
        int p;
        int ct = false;
        for (;;) {
                int option_index = 0;

                p = getopt_long(argc, argv, "vhfuskdt:c:", long_options, &option_index);
                if (p == -1)
                        break;

                switch (p) {
                        case 't':
                                opts->trusted_list = true;
                                check_argument_length(optarg, TYPE_TRUSTEDLIST_LENGTH_CHECK);
                                char *trusted_list = (char *)malloc(strlen(optarg) + 1);
                                check_memory_allocation(trusted_list);
                                strcpy(trusted_list, optarg);
                                opts->trusted_list_value = strdup(trusted_list);
                                free(trusted_list);
                                break;
                        case 'h':
                                show_help();
                                break;
                        case 'f':
                                opts->dev_fingerprint = true;
                                break;
                        case 'u':
                                opts->monitor_usb = true;
                                break;
                        case 's':
                                opts->monitor_sdcard = true;
                                break;
                        case 'k':
                                opts->kill_canaryusb = true;
                                break;
                        case 'c':
                                check_argument_length(optarg, TYPE_CANARYTOKEN_LENGTH_CHECK);
                                char *canary_tkn = (char *)malloc(strlen(optarg) + 1);
                                check_memory_allocation(canary_tkn);
                                strcpy(canary_tkn, optarg);
                                opts->canary_token = strdup(canary_tkn);
                                ct = true;
                                free(canary_tkn);
                                break;
                        case 'v':
                                opts->version = true;
                                break;
                        case 'd':
                                opts->deauth_dev = true;
                                int perm = check_permissions_and_user();
                                // if we do not have permission to deal
                                // with de-authorization on /sys/devices/
                                // folder, we prefer to leave the execution
                                if (perm != 0) {
                                        syslog(LOG_ERR,
                                               "de-authorize a device requires "
                                               "to have permissions on %s",
                                               SYSTEM_DEVICES_FOLDER);
                                        printf("ERROR: de-authorize a device requires "
                                                "to have permissions on %s\n",
                                                SYSTEM_DEVICES_FOLDER);
                                        int u = geteuid();
                                        if (u != 0) {
                                                syslog(LOG_WARNING,
                                                       "hint: you are not running "
                                                       "this as a root");
                                                printf("hint: you are not running "
                                                        "this as a root\n");
                                        }

                                        exit(EXIT_FAILURE);
                                }
                                break;
                        case '?':
                                show_help();
                                break;
                        default:
                                printf("?? getopt returned character code 0%o ??\n", p);
                }
        }

        if (opts->monitor_usb && opts->monitor_sdcard) {
                opts->monitor_usb = false;
                opts->monitor_sdcard = false;
        }

        if (!ct && (opts->monitor_usb || opts->monitor_sdcard))
               config_file_handler(opts); 
}

