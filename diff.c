#include "diff.h"
#include <stdio.h>
#include <string.h>

void diff(const char *file1, const char *file2) {
    FILE *fp1 = fopen(file1, "r");
    FILE *fp2 = fopen(file2, "r");

    if (!fp1 || !fp2) {
        printf("diff: cannot open file(s)\n");
        if (fp1) fclose(fp1);
        if (fp2) fclose(fp2);
        return;
    }

    char buf1[1024], buf2[1024];
    int line = 1;
    int difference = 0;

    while (fgets(buf1, sizeof(buf1), fp1) && fgets(buf2, sizeof(buf2), fp2)) {
        if (strcmp(buf1, buf2) != 0) {
            printf("Line %d differs\n", line);
            difference = 1;
        }
        line++;
    }

    if (fgets(buf1, sizeof(buf1), fp1) || fgets(buf2, sizeof(buf2), fp2)) {
        printf("Files have different lengths\n");
        difference = 1;
    }

    if (!difference)
        printf("Files are identical\n");

    fclose(fp1);
    fclose(fp2);
}