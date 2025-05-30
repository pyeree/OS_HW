// cd.c

#include "cd.h"
#include "pwd.h"         // update_current_path 선언
#include "header.h"
#include <stdio.h>
#include <string.h>

void cd(DirectoryTree *dTree, const char *path) {
    char buf[MAX_PATH_LENGTH];
    char orig[MAX_PATH_LENGTH];
    TreeNode *cur;
    char *token, *saveptr;

    // 에러 메시지용 원본 복사
    strncpy(orig, path, sizeof(orig)-1);
    orig[sizeof(orig)-1] = '\0';

    // 절대경로면 root, 아니면 current부터 시작
    if (path[0] == '/') cur = dTree->root;
    else                cur = dTree->current;

    // 입력 복사
    strncpy(buf, path, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';

    // '/' 단위로 분할
    token = strtok_r(buf, "/", &saveptr);
    while (token) {
        if (strcmp(token, ".") == 0) {
            /* 그대로 유지 */
        }
        else if (strcmp(token, "..") == 0) {
            if (cur->parent) cur = cur->parent;
        }
        else if (token[0] != '\0') {
            // left/right 링크로 자식 탐색
            TreeNode *child = cur->left;
            while (child && !(child->type=='d' && strcmp(child->name, token)==0)) {
                child = child->right;
            }
            if (!child) {
                printf("cd: no such directory: %s\n", orig);
                return;
            }
            cur = child;
        }
        token = strtok_r(NULL, "/", &saveptr);
    }

    // 위치 확정 후 prompt 갱신
    dTree->current = cur;
    update_current_path(dTree);
}
