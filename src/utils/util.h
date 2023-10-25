#ifdef TESTS
        #define TESTS 1
#else
        #define TESTS 0
#endif

//for testing
extern char *test_config_file;

void check_memory_allocation(void *check_me);
void replace_in_string(char *to_replace, char replace_this, char replace_with);
void show_help();
int is_running();
void kill_canaryusb_instance();
void config_file_handler(ConfigCanrayUSB *opts);
void check_argument_length(char *arg, int type);
