//whereis.c

#include "whereis.h"
#include <stdio.h>
#include <string.h>

void whereis_helper(TreeNode *node, const char *target, char *path) {
    if (node == NULL) return;

    char new_path[1024];

    if (node->parent == NULL || strcmp(node->name, "") == 0) {
        strcpy(new_path, "/");
    } else {
        if (strcmp(path, "/") == 0)
            snprintf(new_path, sizeof(new_path), "/%s", node->name);
        else
            snprintf(new_path, sizeof(new_path), "%s/%s", path, node->name);
    }

    if (node->type == 'd' && strcmp(node->name, target) == 0) {
        printf("%s\n", new_path); 
        return;
    }

    if (node->type != 'd' && strcmp(node->name, target) == 0) {
        char *dot_pos = strrchr(node->name, '.');
        if (dot_pos) {
            *dot_pos = '\0'; 
        }
        printf("%s:\n", node->name); 
    }

    whereis_helper(node->left, target, new_path);
    whereis_helper(node->right, target, path);
}

void whereis(DirectoryTree *dTree, const char *target) {
    whereis_helper(dTree->root, target, "/");
}