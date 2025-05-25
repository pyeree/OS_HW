#include "cd.h"
#include "pwd.h"
#include <stdio.h>
#include <string.h>


void update_current_path(DirectoryTree *dTree) {
    // 버퍼 초기화
    char buf[MAX_NAME_LENGTH*10] = "";
    TreeNode *node = dTree->current;
    // 루트까지 올라가며 path segment 를 스택처럼 쌓기
    while (node) {
        char segment[MAX_NAME_LENGTH + 2];
        if (node->parent == NULL) {
            // 루트
            strcpy(segment, "/");
        } else {
            snprintf(segment, sizeof(segment), "/%s", node->name);
        }
        // buf = segment + buf
        char tmp[MAX_NAME_LENGTH*10];
        strcpy(tmp, buf);
        strcpy(buf, segment);
        strcat(buf, tmp);
        node = node->parent;
    }
    // 비어 있으면 루트
    if (buf[0] == '\0') strcpy(buf, "/");
    // dTree.current_path 에 복사
    strncpy(dTree->current_path, buf, sizeof(dTree->current_path)-1);
    dTree->current_path[sizeof(dTree->current_path)-1] = '\0';
}

void cd(DirectoryTree *dTree, const char *path) {
    if (strcmp(path, "/") == 0) {
        dTree->current = dTree->root;
    }
    else if (strcmp(path, "..") == 0) {
        if (dTree->current->parent)
            dTree->current = dTree->current->parent;
    }
    else {
        TreeNode *ch = dTree->current->left;
        while (ch) {
            if (ch->type == 'd' && strcmp(ch->name, path) == 0) {
                dTree->current = ch;
                break;
            }
            ch = ch->right;
        }
        if (!ch) {
            printf("cd: no such directory: %s\n", path);
            return;
        }
    }

    // **이 한 줄**이 빠져 있어서 current_path 가 갱신되지 않았습니다!
    update_current_path(dTree);
}