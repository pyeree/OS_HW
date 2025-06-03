//touch.c

#include "touch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void touch(DirectoryTree *dTree, const char *filename) {
    TreeNode *cur = dTree->current->left;
    while (cur) {
        if (cur->type == 'f' && strcmp(cur->name, filename) == 0) {
            return;
        }
        cur = cur->right;
    }

    FILE *fp = fopen(filename, "a");
    if (!fp) {
        printf("touch: failed to create %s\n", filename);
        return;
    }
    fclose(fp);

    TreeNode *new = malloc(sizeof(TreeNode));
    if (!new) return;

    strcpy(new->name, filename);
    new->type = 'f';  
    new->mode = 0644; 
    new->size = 0;    
    new->UID = 1234;  
    new->GID = 1234;  
    new->left = NULL;
    new->right = NULL;
    new->parent = dTree->current;

    TreeNode *node = dTree->current->left;
    if (!node) {
        dTree->current->left = new;
    } else {
        while (node->right) node = node->right;
        node->right = new;
    }
}
