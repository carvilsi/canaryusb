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

#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "canaryusb.h"
#include "utils/util.h"

int main(int argc, char *argv[])
{ 
        ConfigCanrayUSB opts = config_canary_usb_init;

        if (argc < 2)
                config_file_handler(&opts);
        else
                parse_command_line(argc, argv, &opts);

        if (opts.version) {
                printf("%s %s\n", NAME, VERSION);
                exit(0);
        }

        if (!opts.kill_canaryusb) {
                if (is_running()) {
                        fprintf(stderr, "there is another instance of %s running\n"
                                "you can stop it with '%s -k'\n", NAME, NAME);
                        exit(EXIT_FAILURE);
                }
        } else {
                kill_canaryusb_instance();
        }

        /*if (!opts.dev_fingerprint) {*/
                /*pid_t pid;*/
                /*pid = fork();*/

                /*if (pid < 0)*/
                        /*exit(EXIT_FAILURE);*/
                /*if (pid > 0) */
                        /*exit(EXIT_SUCCESS);*/
                /*if (setsid() < 0)*/
                        /*exit(EXIT_FAILURE);*/

                /*dprintf("%s daemon started\n", NAME);*/
                /*syslog(LOG_NOTICE, "%s daemon started", NAME);*/
        /*} else {*/
                printf("%s in fingerprint mode\n", NAME);
                printf("waiting for a new USB device or SDCard connections, "
                                "ctrl+c to stop it\n");
        /*}*/

        monitor_devices(&opts);
        
        return EXIT_SUCCESS;
}

