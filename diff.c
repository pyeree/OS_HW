//diff.c

#include "diff.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

#define MAX_LINES 1000
#define MAX_LEN   1024

static char lines1[MAX_LINES][MAX_LEN];
static char lines2[MAX_LINES][MAX_LEN];
static int dp[MAX_LINES+1][MAX_LINES+1];

typedef struct {
    int order;
    char text[2048];
} DiffOutput;

static DiffOutput diffs[MAX_LINES * 4];
static int diff_count = 0;

static int diff_cmp(const void* a, const void* b) {
    return ((DiffOutput*)a)->order - ((DiffOutput*)b)->order;
}

static void record_diff(int order, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(diffs[diff_count].text, sizeof(diffs[diff_count].text), fmt, ap);
    va_end(ap);
    diffs[diff_count].order = order;
    diff_count++;
}

void diff(const char* file1, const char* file2) {
    diff_count = 0;

    FILE *fp1 = fopen(file1, "r");
    FILE *fp2 = fopen(file2, "r");
    if (!fp1) {
        printf("diff: %s: No such file or directory\n", file1);
        return;
    }
    if (!fp2) {
        fclose(fp1);
        printf("diff: %s: No such file or directory\n", file2);
        return;
    }

    int n = 0, m = 0;
    while (n < MAX_LINES && fgets(lines1[n], MAX_LEN, fp1)) {
        size_t len = strlen(lines1[n]);
        if (len > 0 && lines1[n][len - 1] == '\n') lines1[n][len - 1] = '\0';
        n++;
    }
    while (m < MAX_LINES && fgets(lines2[m], MAX_LEN, fp2)) {
        size_t len = strlen(lines2[m]);
        if (len > 0 && lines2[m][len - 1] == '\n') lines2[m][len - 1] = '\0';
        m++;
    }
    fclose(fp1);
    fclose(fp2);

    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            if (i == 0 || j == 0) dp[i][j] = 0;
            else if (strcmp(lines1[i-1], lines2[j-1]) == 0)
                dp[i][j] = dp[i-1][j-1] + 1;
            else
                dp[i][j] = dp[i-1][j] > dp[i][j-1]
                         ? dp[i-1][j]
                         : dp[i][j-1];
        }
    }

    int i = 0, j = 0;
    while (i < n || j < m) {
        if (i < n && j < m && strcmp(lines1[i], lines2[j]) == 0) {
            i++; j++; 
        }
        else if (i < n && j < m && strcmp(lines1[i], lines2[j]) != 0) {
            int start_i = i;
            int start_j = j;
            while (i < n && j < m && strcmp(lines1[i], lines2[j]) != 0) {
                i++; j++;  
            }
            if (i > start_i && j > start_j) {
                record_diff(start_i, "%d,%dc%d,%d\n", start_i + 1, i, start_j + 1, j);
                for (int x = start_i; x < i; x++) 
                    record_diff(start_i, "< %s\n", lines1[x]);  
                record_diff(start_i, "---\n");
                for (int x = start_j; x < j; x++) 
                    record_diff(start_i, "> %s\n", lines2[x]);  
            }
        }
        else if (j < m && (i == n || dp[i + 1][j] < dp[i][j + 1])) {
            int start_j = j;
            while (j < m && (i == n || dp[i + 1][j] < dp[i][j + 1])) j++;  
            record_diff(i, "%da%d,%d\n", i, start_j + 1, j);
            for (int x = start_j; x < j; x++) 
                record_diff(i, "> %s\n", lines2[x]);
        }
        else if (i < n) {
            int start_i = i;
            while (i < n && (j == m || dp[i + 1][j] >= dp[i][j + 1])) i++; 
            record_diff(start_i, "%d,%dd%d\n", start_i + 1, i, j);
            for (int x = start_i; x < i; x++) 
                record_diff(start_i, "< %s\n", lines1[x]);
        }
    }

    qsort(diffs, diff_count, sizeof(DiffOutput), diff_cmp);
    for (int k = 0; k < diff_count; k++) {
        printf("%s", diffs[k].text);
    }
}
