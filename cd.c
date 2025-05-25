#include "cd.h"
#include <stdio.h>
#include <string.h>

void cd(DirectoryTree *dTree, const char *path) {
    if (strcmp(path, "/") == 0) {
        dTree->current = dTree->root;
    } else if (strcmp(path, "..") == 0) {
        if (dTree->current->parent != NULL) {
            dTree->current = dTree->current->parent;
        }
    } else {
        TreeNode *child = dTree->current->left;
        while (child != NULL) {
            if (strcmp(child->name, path) == 0 && child->type == 'd') {
                dTree->current = child;
                break;
            }
            child = child->right;
        }
        if (child == NULL) {
            printf("cd: no such directory: %s\n", path);
            return;
        }
    }

    // 현재 경로 재조립
    char temp[1024] = "";
    TreeNode *cur = dTree->current;
    while (cur != NULL && cur->parent != NULL) {
        char segment[MAX_NAME_LENGTH + 2];
        snprintf(segment, sizeof(segment), "/%s", cur->name);
        memmove(temp + strlen(segment), temp, strlen(temp) + 1); // 오른쪽으로 밀기
        memcpy(temp, segment, strlen(segment));
        cur = cur->parent;
    }

    if (strlen(temp) == 0) {
        strcpy(temp, "/");
    }

    // 안전하게 current_path 설정 (경고 제거)
    snprintf(dTree->current_path, sizeof(dTree->current_path), "team4@ubuntu: %s", temp);
}
