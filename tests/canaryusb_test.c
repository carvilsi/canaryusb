#include <stdio.h>
#include <string.h>

#include "ct.h"

#include "./../src/canaryusb.h"

static void command_line_arguments_canary_token()
{
        
        char *provided_canary_token = "555ThisIsTheDNSCanaryToken.canarytokens.com";
        char *argv[] = {"canaries", "-c", provided_canary_token};
        parse_command_line(3, argv);
        printf("%s\n", canary_token);
        ct_assert(__func__, "canary_token must exits", canary_token != NULL);
        ct_assert(__func__, "canary_token must be equal to the one provided", strcmp(canary_token, provided_canary_token) == 0);
}

static void all_tests()
{
      ct_run(command_line_arguments_canary_token);
}

int main(void)  
{
        all_tests();
        ct_print_results();
        return (nbr_ftst == 0 ? 0 : 1);
}
