#include <stdio.h>

#include "ct.h"

int nbr_tst = 0;
int nbr_ptst = 0;
int nbr_ftst = 0;

void ct_run(void (*f)())  
{
        (*f)();
        nbr_tst++;
}

void ct_assert(const char *fncn, char *msgssrt, int test)
{
        if (nbr_tst == 0) {
                printf("\n");
        }

        if (test) {
                printf(GRN "%d- " WHT "%s: " GRN "passed" RST "\n", nbr_tst, fncn); 
                nbr_ptst++;
        } else { 
                printf(RED "%d- " WHT "%s: " RED "failed \n\t" "%s" RST "\n", nbr_tst, fncn, msgssrt);
                nbr_ftst++;
        }
        printf("\n");
}

void ct_print_results()
{
      printf(YEL "Total excuted tests: %d\n" RST, nbr_tst);
      if (nbr_ptst)
              printf(GRN "\tTotal passed tests: %d\n" RST, nbr_ptst);
      if (nbr_ftst != 0)
              printf(RED "\tTotal failed tests: %d\n" RST, nbr_ftst);
}

