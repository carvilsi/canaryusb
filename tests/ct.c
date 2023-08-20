#include <stdio.h>
#include <string.h>

#include "ct.h"

int nbr_tst = 0;
int nbr_ptst = 0;
int nbr_ftst = 0;

char *fncssrt;

void ct_run(void (*f)())  
{
        (*f)();
}

void ct_assert(const char *fncn, char *msgssrt, int test)
{
        if (fncssrt == NULL || strcmp(fncssrt, fncn) != 0)
                fncssrt = fncn;
        else
                fncn = "";

        if (nbr_tst == 0) {
                printf("\n");
        }
        nbr_tst++;

        printf(WHT "%s\n" RST, fncn);

        if (test) {
                printf("\t%d- " WHT "%s: " GRN "passed" RST, nbr_tst, msgssrt); 
                nbr_ptst++;
        } else { 
                printf("\t%d- " RED "%s: failed" RST, nbr_tst, msgssrt);
                nbr_ftst++;
        }
        printf("\n");
}

void ct_print_results()
{
      printf("\n");
      printf(YEL "Total excuted tests: %d\n" RST, nbr_tst);
      if (nbr_ptst)
              printf(GRN "\tTotal passed tests: %d\n" RST, nbr_ptst);
      if (nbr_ftst != 0)
              printf(RED "\tTotal failed tests: %d\n" RST, nbr_ftst);
}

