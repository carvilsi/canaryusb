/*

  MIT License

  Copyright (c) carvilsi [char] 
  cst
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
#include <stdlib.h>

#include "cst.h"

int nbr_tst = 0;
int nbr_ptst = 0;
int nbr_ftst = 0;
int nbr_shld = 0;

char *fncssrt;

void cst_run(void (*f)())  
{
        (*f)();
        nbr_shld++;
}

void cst_i_(const int actual, const char *msgssrt, const int expect, const char *fncn, const int b)
{
        int test;
        if (b)
                test = actual == expect;
        else
                test = actual != expect;

        cst_assert(fncn, msgssrt, test, actual, expect, d);
}

void cst_s_(const char *actual, const char *msgssrt, const char *expect, const char *fncn, const int b)
{
        int test;
        if (b)
                test = strcmp(actual, expect) == 0;
        else
                test = strcmp(actual, expect) != 0;

        cst_assert(fncn, msgssrt, test, actual, expect, s);
}

void cst_a_(const char *msgssrt, const int test, const char *fncn)
{
        cst_true_assert(fncn, msgssrt, test);
}

void cst_results_(const char *tstnm)
{
        printf("\n");
        printf(GRY "Tests %s %s %s resume:\n" RST, tstnm, __DATE__, __TIME__);
        printf(YEL "Total excuted tests: %d\n" RST, nbr_tst);
        if (nbr_ptst)
                printf(GRN "\tTotal passed tests: %d\n" RST, nbr_ptst);
        if (nbr_ftst != 0)
                printf(RED "\tTotal failed tests: %d\n" RST, nbr_ftst);
        printf("\n");
        exit(nbr_ftst == 0 ? 0 : 1);
}

