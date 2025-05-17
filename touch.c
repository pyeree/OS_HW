#include "touch.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void touch(DirectoryTree *dTree, const char *filename) {
    FILE *fp = fopen(filename, "a");
    if (!fp) {
        printf("touch: failed to create %s\n", filename);
        return;
    }
    fclose(fp);

    TreeNode *new = malloc(sizeof(TreeNode));
    if (!new) return;

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);

    strcpy(new->name, filename);
    new->type = 'f';
    new->mode = 0644;
    new->size = 0;
    new->UID = 1234;
    new->GID = 1234;
    new->month = tm->tm_mon + 1;
    new->day = tm->tm_mday;
    new->hour = tm->tm_hour;
    new->minute = tm->tm_min;
    new->left = NULL;
    new->right = NULL;
    new->parent = dTree->current;

    TreeNode *cur = dTree->current->left;
    if (!cur) {
        dTree->current->left = new;
    } else {
        while (cur->right) cur = cur->right;
        cur->right = new;
    }
}