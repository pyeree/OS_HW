#include <stdio.h>
#include "clear.h"

void clear() {
    printf("\033[H\033[J");
    printf("\033[3J");
}