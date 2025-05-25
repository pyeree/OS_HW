#include "cd.h"
#include <stdio.h>
#include <string.h>

void cd(DirectoryTree *dTree, const char *path) {
    if (strcmp(path, "/") == 0) {
        dTree->current = dTree->root;
        strcpy(dTree->current_path, "team6@ubuntu: /");
        return;
    }
    if (strcmp(path, "..") == 0) {
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
    //char full_path[1024] = "";
    TreeNode *cur = dTree->current;
    char temp[1024] = "";
    while (cur != NULL && cur->parent != NULL) {
        char segment[MAX_NAME_LENGTH + 2];
        snprintf(segment, sizeof(segment), "/%s", cur->name);
        strcat(segment, temp);
        strcpy(temp, segment);
        cur = cur->parent;
    }
    if (strlen(temp) == 0)
        strcpy(temp, "/");
    snprintf(dTree->current_path, sizeof(dTree->current_path), "team6@ubuntu: %s", temp);
}