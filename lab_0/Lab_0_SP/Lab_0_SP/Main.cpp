#include <stdio.h>
#include <limits.h>
#include <float.h>

#define UNSIGNED_MIN 0

int main(void)
{
    printf("                      *** Integer types ***\n");
    printf("+---------------+-----+---------------------+---------------------+\n");
    printf("|     Type      |Bytes|        Min DEC      |       Max DEC       |\n");
    printf("+---------------+-----+---------------------+---------------------+\n");
    printf("| unsigned char |%-5d|%-21d|%-21d|\n",     sizeof(unsigned char),      UNSIGNED_MIN, UCHAR_MAX);
    printf("+---------------+-----+---------------------+---------------------+\n");
    printf("|  signed char  |%-5d|%-21d|%-21d|\n",     sizeof(char),               CHAR_MIN, CHAR_MAX);
    printf("+---------------+-----+---------------------+---------------------+\n");
    printf("| unsigned short|%-5d|%-21d|%-21u|\n",     sizeof(unsigned short),     UNSIGNED_MIN, USHRT_MAX);
    printf("+---------------+-----+---------------------+---------------------+\n");
    printf("|      short    |%-5d|%-21d|%-21d|\n",     sizeof(short),              SHRT_MIN, SHRT_MAX);
    printf("+---------------+-----+---------------------+---------------------+\n");
    printf("| unsigned int  |%-5d|%-21d|%-21u|\n",     sizeof(unsigned int),       UNSIGNED_MIN, UINT_MAX);
    printf("+---------------+-----+---------------------+---------------------+\n");
    printf("|      int      |%-5d|%-21d|%-21d|\n",     sizeof(int),                INT_MIN, INT_MAX);
    printf("+---------------+-----+---------------------+---------------------+\n");
    printf("| unsigned long |%-5d|%-21d|%-21u|\n",     sizeof(unsigned long),      UNSIGNED_MIN, ULONG_MAX);
    printf("+---------------+-----+---------------------+---------------------+\n");
    printf("|      long     |%-5d|%-21ld|%-21ld|\n",   sizeof(long),               LONG_MIN, LONG_MAX);
    printf("+---------------+-----+---------------------+---------------------+\n");
    printf("|uns. long long |%-5d|%-21d|%-21llu|\n",   sizeof(unsigned long long), UNSIGNED_MIN, ULLONG_MAX);
    printf("+---------------+-----+---------------------+---------------------+\n");
    printf("|   long long   |%-5d|%-21lld|%-21lld|\n", sizeof(long long),          LLONG_MIN, LLONG_MAX);
    printf("+---------------+-----+---------------------+---------------------+\n");

    printf("\n\n                   *** Floating-point types ***\n");
    printf("+---------------+-----+---------------------+---------------------+\n");
    printf("|     Type      |Bytes|        Min DEC      |       Max DEC       |\n");
    printf("+---------------+-----+---------------------+---------------------+\n");
    printf("|     float     |%-5d|%-21G|%-21G|\n", sizeof(float),       FLT_MIN, FLT_MAX);
    printf("+---------------+-----+---------------------+---------------------+\n");
    printf("|     double    |%-5d|%-21G|%-21G|\n", sizeof(double),      DBL_MIN, DBL_MAX);
    printf("+---------------+-----+---------------------+---------------------+\n");
    printf("|  long double  |%-5d|%-21G|%-21G|\n", sizeof(long double), LDBL_MIN, LDBL_MAX);
    printf("+---------------+-----+---------------------+---------------------+\n");

    return 0;
}
