#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[37m"
#define RST "\x1B[0m"

extern int nbr_tst;
extern int nbr_ptst;
extern int nbr_ftst;

void ct_run(void (*f)()); 
void ct_assert(const char *fncn, char *msgssrt, int test);
void ct_print_results();
