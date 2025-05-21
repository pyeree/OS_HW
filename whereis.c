#include "whereis.h"
#include <stdio.h>
#include <string.h>

void whereis_helper(TreeNode *node, const char *target, char *path) {
    if (node == NULL) return;

    char new_path[1024];
    snprintf(new_path, sizeof(new_path), "%s/%s", path, node->name);

    if (strcmp(node->name, target) == 0) {
        printf("Found: %s\n", new_path);
    }

    whereis_helper(node->left, target, new_path);
    whereis_helper(node->right, target, path);
}

void whereis(DirectoryTree *dTree, const char *target) {
    whereis_helper(dTree->root, target, "");
}