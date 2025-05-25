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

    // 경로 문자열 조립 (루트 기준으로)
    char temp[1024] = "";
    TreeNode *cur = dTree->current;
    while (cur != NULL && cur->parent != NULL) {
        char segment[MAX_NAME_LENGTH + 2];
        snprintf(segment, sizeof(segment), "/%s", cur->name);
        memmove(temp + strlen(segment), temp, strlen(temp) + 1); // 오른쪽으로 밀기
        memcpy(temp, segment, strlen(segment));
        cur = cur->parent;
    }

    if (strlen(temp) == 0) strcpy(temp, "/");

    snprintf(dTree->current_path, MAX_PATH_LENGTH, "team4@ubuntu: %s", temp);
}
