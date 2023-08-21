#include <stdio.h>
#include <string.h>

#include "ct.h"

int nbr_tst = 0;
int nbr_ptst = 0;
int nbr_ftst = 0;
int nbr_shld = 0;

char *fncssrt;

void ct_run(void (*f)())  
{
        (*f)();
        nbr_shld++;
}

void ct_assert(const char *fncn, char *msgssrt, int test)
{       
        if (nbr_tst == 0) {
                printf("\n");
        }
        nbr_tst++;

        if (fncssrt == NULL || strcmp(fncssrt, fncn) != 0) {
                printf("\n");
                fncssrt = fncn;
                printf(WHT "%d- " RST BLD "%s\n" RST, nbr_shld, fncn);
        } 
       
        if (test) {
                printf("\t%d- " GRN  "%s " WHT "%s" RST, nbr_tst, CHK, msgssrt); 
                nbr_ptst++;
        } else { 
                printf("\t%d- " RED "%s " "%s" RST, nbr_tst, CRS, msgssrt);
                nbr_ftst++;
        }
        printf("\n");
}

void ct_print_results(char *tstn)
{
      printf("\n");
      printf(MAG "Tests %s resume:\n\n" RST, tstn);
      printf(YEL "Total excuted tests: %d\n" RST, nbr_tst);
      if (nbr_ptst)
              printf(GRN "\tTotal passed tests: %d\n" RST, nbr_ptst);
      if (nbr_ftst != 0)
              printf(RED "\tTotal failed tests: %d\n" RST, nbr_ftst);
      printf("\n");
}

