#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "canaryusb.h"
#include "utils/util.h"

int main(int argc, char *argv[])
{       
        check_if_running();
        
        if (argc < 2) {
                parse_configuration_file(); 
        } else {
                parse_command_line(argc, argv);
        }

        if (!usb_fingerprint) {
                pid_t pid;
                pid = fork();

                if (pid < 0)
                        exit(EXIT_FAILURE);
                if (pid > 0) 
                        exit(EXIT_SUCCESS);
                if (setsid() < 0)
                        exit(EXIT_FAILURE);

                dprintf("%s daemon started\n", _NAME_);
                syslog(LOG_NOTICE, "%s daemon started", _NAME_);
        } else {
                printf("%s in usb_fingerprint mode\n", _NAME_);
                printf("waiting for a new USB device connections, ctrl+c to stop it\n");
        }

        monitor_usb();
        free_canaries();
        
        return EXIT_SUCCESS;
}

