#define RED "\x1B[31m"
#define GRN "\x1B[32m"
#define YEL "\x1B[33m"
#define BLU "\x1B[34m"
#define MAG "\x1B[35m"
#define CYN "\x1B[36m"
#define WHT "\x1B[97m"
#define RST "\x1B[0m"
#define GRY "\x1B[90m"
#define BLD "\x1B[1m"
#define CHK "\u2714"
#define CRS "\u2718"

extern int nbr_tst;
extern int nbr_ptst;
extern int nbr_ftst;
extern int nbr_shld;

void ct_run(void (*f)()); 
void ct_assert(const char *fncn, char *msgssrt, int test);
void ct_print_results();
