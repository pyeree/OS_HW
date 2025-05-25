#include "cd.h"
#include "pwd.h"
#include <stdio.h>
#include <string.h>

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
            if (ch->type=='d' && strcmp(ch->name, path)==0) {
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
    // 변경 후 절대경로만 갱신
    update_current_path(dTree);
}
