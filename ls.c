#include "header.h"
#include "tree_io.h"
#include "ls.h"

void ls(DirectoryTree* dTree) {
    int ct = 1;

    TreeNode* tmpN = dTree->current;
    if (tmpN->left == NULL)
        printf("directory is empty\n");
    else {
        tmpN = tmpN->left;
        while (tmpN->right != NULL) {
            if (strlen(tmpN->name) < 8)
                printf("%s\t\t", tmpN->name);
            else
                printf("%s\t", tmpN->name);
            tmpN = tmpN->right;
            if (ct % 5 == 0)
                printf("\n");
            ct++;
        }
        printf("%s\t\n", tmpN->name);
    }
}
void ls_a(DirectoryTree* dTree) {
    int ct = 1;
    TreeNode* tmpN = dTree->current;
    if (tmpN->left == NULL) {
        printf(".\t\t..\n");
    }
    else {
        printf(".\t\t..\t\t");
        ct = ct + 2;
        tmpN = tmpN->left;
        while (tmpN->right != NULL) {
            if (strlen(tmpN->name) < 8)
                printf("%s\t\t", tmpN->name);
            else
                printf("%s\t", tmpN->name);
            tmpN = tmpN->right;
            if (ct % 5 == 0)
                printf("\n");
            ct++;
        }
        printf("%s\t\n", tmpN->name);
    }
}

void ls_l(DirectoryTree* dTree) {
    TreeNode* tmpN = dTree->current;
    if (tmpN->left == NULL)
        printf("directory is empty\n");
    else {
        tmpN = tmpN->left;
        while (tmpN->right != NULL) {
            printf("%c", tmpN->type);
            PrintPerm(tmpN);
            printf(" %d %d %d", tmpN->size, tmpN->UID, tmpN->GID);
            printf(" %d(Month) %d(day) %d(hour) %d(min) ", tmpN->month, tmpN->day, tmpN->hour, tmpN->minute);
            printf("%s\n", tmpN->name);
            tmpN = tmpN->right;
        }
        printf("%c", tmpN->type);
        PrintPerm(tmpN);
        printf(" %d %d %d", tmpN->size, tmpN->UID, tmpN->GID);
        printf(" %d(Month) %d(day) %d(hour) %d(min) ", tmpN->month, tmpN->day, tmpN->hour, tmpN->minute);
        printf("%s\n", tmpN->name);
    }
}

void ls_al(DirectoryTree* dTree) {
    TreeNode* tmpN = dTree->current;
    printf("drwxr-xr-x 4096 1234 1234  5(month) 30(day) 15(hour) 5(min) .\n");
    printf("drwxr-xr-x 4096 1234 1234  5(month) 30(day) 15(hour) 5(min) ..\n");
    if (tmpN->left == NULL)
        printf("directory is empty\n");
    else {
        tmpN = tmpN->left;
        while (tmpN->right != NULL) {
            printf("%c", tmpN->type);
            PrintPerm(tmpN);
            printf(" %d %d %d", tmpN->size, tmpN->UID, tmpN->GID);
            printf(" %d(Month) %d(day) %d(hour) %d(min) ", tmpN->month, tmpN->day, tmpN->hour, tmpN->minute);
            printf("%s\n", tmpN->name);
            tmpN = tmpN->right;
        }
        printf("%c", tmpN->type);
        PrintPerm(tmpN);
        printf(" %d %d %d", tmpN->size, tmpN->UID, tmpN->GID);
        printf(" %d(Month) %d(day) %d(hour) %d(min) ", tmpN->month, tmpN->day, tmpN->hour, tmpN->minute);
        printf("%s\n", tmpN->name);
    }
}
