#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <libudev.h>
#include <unistd.h>

#include "canaryusb.h"
#include "utils/util.h"

int main(int argc, char *argv[])
{       
        int i = 0;
        for (i; i < argc; i++) {
                printf("%s\n", argv[i]);
        } 

        if (argc < 2) {
                int cfr = config_file_handler();
                printf("config file res: %d\n", cfr);
                exit(1);
                //TODO: check if the config file exists and if it has values
                //if exists and has values, use these as parameters
                //otherwise is this error
                fprintf(stderr, "ERROR: missing parameters, check the usage: \n");
                show_help();
        } else {
                parse_command_line(argc, argv);
        }
        
        struct udev* udev = udev_new();
        if (!udev) {
                fprintf(stderr, "udev error\n");
                exit(EXIT_FAILURE); 
        }

        check_if_running();

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

        monitor_usb(udev);
        udev_unref(udev);

        free_canaries();
        
        return EXIT_SUCCESS;
}
