#include <syslog.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "canaryusb.h"
#include "utils/util.h"

int main(int argc, char *argv[])
{       
        if (argc < 2) {
                parse_configuration_file(); 
        } else {
                parse_command_line(argc, argv);
        }
        
        if (!kill_canaryusb)
                check_if_running();
        else
                kill_canaryusb_inst();
       
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

        monitor_usb();
        free_canaries();
        
        return EXIT_SUCCESS;
}

