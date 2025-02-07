#include <stdio.h>
#include <helper.h>

void printbin(unsigned int num) {
    for (int i = sizeof(num) * 8 - 1; i >= 0; i--) {
        unsigned int bit = (num >> i) & 1;  // Extrai o bit na posição i
        printf("%d", bit);
        if (i % 8 == 0)  // Insere um espaço a cada 8 bits
            printf(" ");
    }
    printf("\n");
}