// cd.c

#include "cd.h"
#include "pwd.h"         // for update_current_path()
#include "header.h"
#include <stdio.h>
#include <string.h>

void cd(DirectoryTree *dTree, const char *path) {
    char buf[MAX_PATH_LENGTH];
    TreeNode *cur;
    char *token;
    char *saveptr;
    char orig[MAX_PATH_LENGTH];

    // 원본 경로 복사 (에러 메시지용)
    strncpy(orig, path, sizeof(orig));
    orig[sizeof(orig)-1] = '\0';

    // 1) 절대 경로? '/' 로 시작하면 root, 아니면 current
    if (path[0] == '/') {
        cur = dTree->root;
    } else {
        cur = dTree->current;
    }

    // 2) buf 에 복사해서 "/" 로 분할
    strncpy(buf, path, sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    token = strtok_r(buf, "/", &saveptr);
    while (token) {
        if (strcmp(token, ".") == 0) {
            // 현재 위치 그대로
        }
        else if (strcmp(token, "..") == 0) {
            // 상위로
            if (cur->parent) {
                cur = cur->parent;
            }
        }
        else if (token[0] != '\0') {
            // 자식 중 같은 이름의 디렉터리로 이동
            TreeNode *child = cur->children;
            while (child && !(child->type == 'd' && strcmp(child->name, token) == 0)) {
                child = child->next_sibling;
            }
            if (!child) {
                printf("cd: no such directory: %s\n", orig);
                return;
            }
            cur = child;
        }
        token = strtok_r(NULL, "/", &saveptr);
    }

    // 3) 위치 확정 → current 갱신 & prompt 경로 업데이트
    dTree->current = cur;
    update_current_path(dTree);
}
