#include <stdio.h>
#include <helper.h>

void printbin(unsigned int num) {
    for (int i = sizeof(num) * 8 - 1; i >= 0; i--) {
        unsigned int bit = (num >> i) & 1;
        printf("%d", bit);
        if (i % 8 == 0)
            printf(" ");
    }
    printf("\n");
}