/*

  MIT License

  Copyright (c) carvilsi [char] 
  ct
  https://github.com/carvilsi

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

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
                printf(WHT "\t%d- " GRN  "%s " WHT "%s" RST, nbr_tst, CHK, msgssrt); 
                nbr_ptst++;
        } else { 
                printf(WHT "\t%d- " RED "%s " "%s" RST, nbr_tst, CRS, msgssrt);
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

