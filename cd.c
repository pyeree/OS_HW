#include "cd.h"
#include "pwd.h"    // update_current_path 선언
#include <stdio.h>
#include <string.h>

void cd(DirectoryTree *dTree, const char *path) {
    // 1) 루트로 이동
    if (strcmp(path, "/") == 0) {
        dTree->current = dTree->root;
    }
    // 2) 상위로 이동
    else if (strcmp(path, "..") == 0) {
        if (dTree->current->parent)
            dTree->current = dTree->current->parent;
    }
    // 3) 자식 디렉토리로 이동
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

    // 프롬프트용 경로 문자열 갱신
    update_current_path(dTree);
}
