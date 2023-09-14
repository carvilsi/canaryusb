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

#define cst_assert(f, m, t, r, e, y) if (nbr_tst == 0) { printf("\n"); } nbr_tst++; if (fncssrt == NULL || strcmp(fncssrt, f) != 0) { printf("\n"); fncssrt = f; printf(BLD "%d- " RST WHT "%s\n" RST, nbr_shld, f); } if (t) { printf(WHT "\t%d- " GRN  "%s " RST BLD "%s: %" #y RST, nbr_tst, CHK, m, e); nbr_ptst++; } else { printf(WHT "\t%d- " RED "%s " "%s: %" #y RST GRY "\n\t\t actual: " GRN "%" #y GRY " - expected: " RED "%" #y RST, nbr_tst, CRS, m, e, r, e); nbr_ftst++; } printf("\n");

#define cst_true_assert(f, m, t) if (nbr_tst == 0) { printf("\n"); } nbr_tst++; if (fncssrt == NULL || strcmp(fncssrt, f) != 0) { printf("\n"); fncssrt = f; printf(BLD "%d- " RST WHT "%s\n" RST, nbr_shld, f); } if (t) { printf(WHT "\t%d- " GRN "%s " RST BLD "%s" RST, nbr_tst, CHK, m); nbr_ptst++; } else { printf(WHT "\t%d- " RED "%s " "%s " RST, nbr_tst, CRS, m); nbr_ftst++; } printf("\n");



#define cst_i(a, b, c) cst_i_(a, b, c, __func__, 1)
#define cst_i_ne(a, b, c) cst_i_(a, b, c, __func__, 0)
#define cst_s(a, b, c) cst_s_(a, b, c, __func__, 1)
#define cst_s_ne(a, b, c) cst_s_(a, b, c, __func__, 0)
#define cst_a(a, b) cst_a_(a, b, __func__)
#define cst_results() cst_results_(__FILE__)

void cst_run(void (*f)()); 
void cst_results_(const char *flnm);
void cst_i_(const int actual, const char *msgssrt, const int expect, const char *fncn, const int b);
void cst_s_(const char *actual, const char *msgssrt, const char *expect, const char *fncn, const int b);
void cst_a_(const char *msgssrt, const int test, const char *fncn);

