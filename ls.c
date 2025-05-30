// ls.c

#include "ls.h"
#include "header.h"
#include <stdio.h>
#include <string.h>

/**
 * @brief 노드의 mode 숫자(e.g. 755)를 "drwxr-xr-x" 문자열로 변환
 */
static void format_mode(const TreeNode *n, char *out) {
    out[0] = (n->type == 'd') ? 'd' : '-';
    int mode = n->mode;             // e.g. 0755
    int cls[3] = {                  // user, group, other
        (mode / 100) % 10,
        (mode / 10)  % 10,
        mode         % 10
    };
    for (int i = 0; i < 3; i++) {
        out[1 + 3*i + 0] = (cls[i] & 4) ? 'r' : '-';
        out[1 + 3*i + 1] = (cls[i] & 2) ? 'w' : '-';
        out[1 + 3*i + 2] = (cls[i] & 1) ? 'x' : '-';
    }
    out[10] = '\0';
}

/**
 * @brief 단일 엔트리를 long format으로 출력
 */
static void print_long(const TreeNode *n) {
    char mode_str[11];
    format_mode(n, mode_str);
    int links = 1;  // 단순화: 항상 1
    // 출력 포맷: mode links UID GID size month/day HH:MM name
    printf("%s %2d %3d %3d %5d %2d/%2d %02d:%02d %s\n",
           mode_str, links,
           n->UID, n->GID,
           n->size,
           n->month, n->day,
           n->hour, n->minute,
           n->name);
}

void ls(DirectoryTree* dTree) {
    TreeNode* c = dTree->current->left;
    if (!c) {
        printf("directory is empty\n");
        return;
    }
    int col = 0;
    while (c) {
        printf("%-16s", c->name);
        if (++col % 5 == 0) printf("\n");
        c = c->right;
    }
    if (col % 5) printf("\n");
}

void ls_a(DirectoryTree* dTree) {
    // 항상 "."과 ".."부터
    printf("%-16s%-16s", ".", "..");
    TreeNode* c = dTree->current->left;
    int col = 2;
    while (c) {
        printf("%-16s", c->name);
        if (++col % 5 == 0) printf("\n");
        c = c->right;
    }
    if (col % 5) printf("\n");
}

void ls_l(DirectoryTree* dTree) {
    TreeNode* c = dTree->current->left;
    if (!c) {
        printf("directory is empty\n");
        return;
    }
    while (c) {
        print_long(c);
        c = c->right;
    }
}

void ls_al(DirectoryTree* dTree) {
    // "."과 ".."을 long format으로
    {
        TreeNode dot = { .name = ".", .type='d',
                        .mode=0755, .size=4096,
                        .UID=dTree->current->UID,
                        .GID=dTree->current->GID,
                        .month=dTree->current->month,
                        .day=dTree->current->day,
                        .hour=dTree->current->hour,
                        .minute=dTree->current->minute };
        print_long(&dot);
        TreeNode dotdot = dot;
        strcpy(dotdot.name, "..");
        print_long(&dotdot);
    }
    // 실제 자식들 long format
    TreeNode* c = dTree->current->left;
    while (c) {
        print_long(c);
        c = c->right;
    }
}
