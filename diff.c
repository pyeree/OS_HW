// diff.c
#include "diff.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define MAX_LINES 1000
#define MAX_LEN   1024

// 입력 파일을 한 줄씩 보관할 버퍼
static char lines1[MAX_LINES][MAX_LEN];
static char lines2[MAX_LINES][MAX_LEN];
// LCS DP 테이블
static int dp[MAX_LINES+1][MAX_LINES+1];

// diff 명령 출력용
typedef struct {
    int  order;
    char text[2048];
} DiffOutput;
static DiffOutput diffs[MAX_LINES * 4];
static int diff_count = 0;

// 정렬용 비교 함수
static int diff_cmp(const void* a, const void* b) {
    return ((const DiffOutput*)a)->order - ((const DiffOutput*)b)->order;
}

// diffs[]에 한 줄 기록
static void record_diff(int order, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(diffs[diff_count].text, sizeof(diffs[diff_count].text), fmt, ap);
    va_end(ap);
    diffs[diff_count].order = order;
    diff_count++;
}

void diff(const char* file1, const char* file2) {
    // 이전 결과 초기화
    diff_count = 0;

    // 1) 파일 열기
    FILE *fp1 = fopen(file1, "r");
    FILE *fp2 = fopen(file2, "r");
    if (!fp1 || !fp2) {
        printf("diff: cannot open file(s)\n");
        if (fp1) fclose(fp1);
        if (fp2) fclose(fp2);
        return;
    }

    // 2) 각 파일을 lines1/lines2에 로드
    int n = 0, m = 0;
    while (n < MAX_LINES && fgets(lines1[n], MAX_LEN, fp1)) n++;
    while (m < MAX_LINES && fgets(lines2[m], MAX_LEN, fp2)) m++;
    fclose(fp1);
    fclose(fp2);

    // 3) LCS 테이블 계산
    for (int i = 0; i <= n; i++) {
        for (int j = 0; j <= m; j++) {
            if (i == 0 || j == 0) dp[i][j] = 0;
            else if (strcmp(lines1[i-1], lines2[j-1]) == 0)
                dp[i][j] = dp[i-1][j-1] + 1;
            else
                dp[i][j] = dp[i-1][j] > dp[i][j-1] ? dp[i-1][j] : dp[i][j-1];
        }
    }

    // 4) 역추적하며 diff 블록 수집
    int i = n, j = m;
    int cs1=-1, cs2=-1, ce1=-1, ce2=-1;
    while (i > 0 && j > 0) {
        if (strcmp(lines1[i-1], lines2[j-1]) == 0) {
            // 연속 변경 블록이 끝났다면 기록
            if (cs1 != -1) {
                record_diff(cs1, "%d,%dc%d,%d\n", cs1+1, ce1, cs2+1, ce2);
                for (int x = cs1; x < ce1; x++) record_diff(cs1, "< %s", lines1[x]);
                record_diff(cs1, "---\n");
                for (int x = cs2; x < ce2; x++) record_diff(cs1, "> %s", lines2[x]);
                cs1 = cs2 = ce1 = ce2 = -1;
            }
            i--; j--;
        }
        else if (dp[i][j] == dp[i-1][j-1]) {
            // 교체(Changed) 구간
            i--; j--;
            if (cs1 == -1) cs1 = i, cs2 = j;
            ce1 = i+1; ce2 = j+1;
        }
        else if (dp[i-1][j] >= dp[i][j-1]) {
            // 삭제(Delete)
            if (cs1 != -1) {
                record_diff(cs1, "%d,%dc%d,%d\n", cs1+1, ce1, cs2+1, ce2);
                for (int x = cs1; x < ce1; x++) record_diff(cs1, "< %s", lines1[x]);
                record_diff(cs1, "---\n");
                for (int x = cs2; x < ce2; x++) record_diff(cs1, "> %s", lines2[x]);
                cs1 = cs2 = ce1 = ce2 = -1;
            }
            i--;
            record_diff(i, "%dd%d\n< %s", i+1, j, lines1[i]);
        } else {
            // 추가(Add)
            if (cs1 != -1) {
                record_diff(cs1, "%d,%dc%d,%d\n", cs1+1, ce1, cs2+1, ce2);
                for (int x = cs1; x < ce1; x++) record_diff(cs1, "< %s", lines1[x]);
                record_diff(cs1, "---\n");
                for (int x = cs2; x < ce2; x++) record_diff(cs1, "> %s", lines2[x]);
                cs1 = cs2 = ce1 = ce2 = -1;
            }
            j--;
            record_diff(i, "%da%d\n> %s", i, j+1, lines2[j]);
        }
    }
    // 나머지 삭제/추가
    while (i > 0) {
        i--;
        record_diff(i, "%dd0\n< %s", i+1, lines1[i]);
    }
    while (j > 0) {
        j--;
        record_diff(0, "0a%d\n> %s", j+1, lines2[j]);
    }

    // 5) 순서대로 출력
    qsort(diffs, diff_count, sizeof(DiffOutput), diff_cmp);
    for (int k = 0; k < diff_count; k++)
        printf("%s", diffs[k].text);
}
