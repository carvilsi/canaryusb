#include <stdio.h>
#include <string.h>

#include "./../src/utils/base32.h"

#define STR "foobar"

#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define RESET "\x1B[0m"

int main()
{
       char buf[10];
       size_t s = 10;
       int res = base32_encode(buf, &s, STR, strlen(STR));  
       printf("the buff: %s\n", buf);
       printf(KRED "red lol\n" RESET);
       printf(KGRN "green foo\n" RESET);
       printf("%sreset\n", RESET);
} 
