// chmod.c
#include "chmod.h"
#include "header.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

void run_chmod(DirectoryTree* tree, const char* arg) {
    if (!arg) {
        printf("Usage: chmod <mode> <name>\n");
        return;
    }

    // arg 복사해서 토큰 분리
    char buf[256];
    strncpy(buf, arg, sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    char* mode_str = strtok(buf, " ");
    char* name     = strtok(NULL, "");
    if (!mode_str || !name) {
        printf("Usage: chmod <mode> <name>\n");
        return;
    }
    // 앞 공백 제거
    while (*name == ' ') name++;

    // 트리에서 파일 또는 디렉터리 찾기
    TreeNode* node = tree->current->left;
    while (node) {
        if (strcmp(node->name, name) == 0 && (node->type=='f' || node->type=='d'))
            break;
        node = node->right;
    }
    if (!node) {
        printf("chmod: '%s' not found\n", name);
        return;
    }

    int newmode = 0;
    // ─────────────────────────────
    // 1) 숫자 모드 (e.g. 755)
    // ─────────────────────────────
    bool is_numeric = strlen(mode_str)==3
                && isdigit((unsigned char)mode_str[0])
                && isdigit((unsigned char)mode_str[1])
                && isdigit((unsigned char)mode_str[2]);
    if (is_numeric) {
        int m = atoi(mode_str);
        if (m < 0 || m > 777) {
            printf("Invalid numeric mode: %s\n", mode_str);
            return;
        }
        newmode = m;

    // ─────────────────────────────
    // 2) 심볼릭 모드 (e.g. +x, u+rw, g-w)
    // ─────────────────────────────
    } else if (strchr(mode_str,'+') || strchr(mode_str,'-')) {
        // 2-1) 클래스 지정 (u=1, g=2, o=4)
        int classes = 0;
        bool has_class = false;
        char* p = mode_str;
        while (*p=='u' || *p=='g' || *p=='o' || *p=='a') {
            has_class = true;
            if (*p=='u') classes |= 1;
            if (*p=='g') classes |= 2;
            if (*p=='o') classes |= 4;
            if (*p=='a') classes |= 1|2|4;
            p++;
        }
        if (!has_class) classes = 1|2|4;

        // 2-2) 연산자
        char op = *p++;
        if (op!='+' && op!='-') {
            printf("Invalid symbolic operator: %c\n", op);
            return;
        }

        // 2-3) 권한 문자에서 비트 마스크 추출
        int permbits = 0;
        while (*p) {
            if (*p=='r') permbits |= 4;
            else if (*p=='w') permbits |= 2;
            else if (*p=='x') permbits |= 1;
            p++;
        }
        if (permbits == 0) {
            printf("No permission letters found in '%s'\n", mode_str);
            return;
        }

        // 2-4) 현재 모드 분해
        int cm = node->mode;
        int u = (cm/100)%10, g = (cm/10)%10, o = cm%10;

        // 2-5) 클래스별로 비트 적용
        if (classes & 1) u = (op=='+' ? u|permbits : u&~permbits);
        if (classes & 2) g = (op=='+' ? g|permbits : g&~permbits);
        if (classes & 4) o = (op=='+' ? o|permbits : o&~permbits);

        newmode = u*100 + g*10 + o;

    } else {
        printf("Invalid mode format: %s\n", mode_str);
        return;
    }

    // 최종 적용
    node->mode = newmode;
    printf("Permissions for '%s' set to %03d\n", name, newmode);
}
