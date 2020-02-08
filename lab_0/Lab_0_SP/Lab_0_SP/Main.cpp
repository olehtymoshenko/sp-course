#include <stdio.h>
#include "windows.h"

int main()
{
    printf("Basic data types\n");
    printf("+-----------+------+-------------+-------------+ \n");
    printf("|    name   | size |     MAX     |     MIN     | \n");
    printf("+-----------+------+-------------+-------------+ \n");
    printf("|    BYTE   |   %d  |     255     |      0      |\n", sizeof(BYTE));
    printf("+-----------+------+-------------+-------------+ \n");
    printf("|    SHORT  |   %d  |    32767    |   -32768    |\n", sizeof(SHORT));
    printf("+-----------+------+-------------+-------------+ \n");
    printf("|    WORD   |   %d  |    65535    |      0      |\n", sizeof(WORD));
    printf("+-----------+------+-------------+-------------+ \n");
    printf("|    INT    |   %d  | 2147483647  | -2147483648 |\n", sizeof(INT));
    printf("+-----------+------+-------------+-------------+ \n");
    printf("|UNIT(DWORD)|   %d  | 4294967295  |      0      |\n", sizeof(DWORD));
    printf("+-----------+------+-------------+-------------+ \n");
    return 0;
}