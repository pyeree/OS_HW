// mkdir.c
#include "mkdir.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void run_mkdir(DirectoryTree *tree, const char *dirname) {
    if (dirname == NULL || strlen(dirname) == 0) {
        printf("mkdir: missing operand\n");
        printf("Usage: mkdir <directory_name>\n");
        return;
    }

    // 중복 확인 (left-right 구조 기준)
    TreeNode *node = tree->current->left;
    while (node != NULL) {
        if (strcmp(node->name, dirname) == 0 && node->type == 'd') {
            printf("mkdir: cannot create directory '%s': File exists\n", dirname);
            return;
        }
        node = node->right;
    }

    // 새 노드 할당
    TreeNode *new_dir = malloc(sizeof(TreeNode));
    if (!new_dir) {
        perror("malloc failed");
        return;
    }

    // 속성 설정
    strncpy(new_dir->name, dirname, MAX_NAME_LENGTH);
    new_dir->type = 'd';
    new_dir->mode = 755;
    new_dir->size = 4096;
    new_dir->UID = 0;
    new_dir->GID = 0;

    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    new_dir->month = tm_info->tm_mon + 1;
    new_dir->day = tm_info->tm_mday;
    new_dir->hour = tm_info->tm_hour;
    new_dir->minute = tm_info->tm_min;

    new_dir->left = NULL;
    new_dir->right = NULL;
    new_dir->parent = tree->current;

    // 트리에 연결 (left-right 형식)
    if (tree->current->left == NULL) {
        tree->current->left = new_dir;
    } else {
        TreeNode *last = tree->current->left;
        while (last->right != NULL) {
            last = last->right;
        }
        last->right = new_dir;
    }

    printf("Directory '%s' created successfully.\n", dirname);
}
// chmod.c
#include "chmod.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void run_chmod(DirectoryTree* tree, const char* arg) {
    if (arg == NULL || strlen(arg) < 5) {
        printf("Usage: chmod <mode: 3-digit number> <filename>\n");
        return;
    }

    char input[256];
    strncpy(input, arg, sizeof(input));
    input[sizeof(input) - 1] = '\0';

    char* mode_str = strtok(input, " ");
    char* filename = strtok(NULL, "");

    if (mode_str == NULL || filename == NULL || strlen(mode_str) != 3) {
        printf("Invalid usage. Example: chmod 755 filename\n");
        return;
    }

    int mode = atoi(mode_str);
    if (mode < 0 || mode > 777) {
        printf("Invalid mode value: must be between 000 and 777\n");
        return;
    }

    TreeNode* node = tree->current->left;
    while (node != NULL) {
        if (strcmp(node->name, filename) == 0 && node->type == 'f') {
            node->mode = mode;
            printf("Permissions for '%s' changed to %03d\n", filename, mode);
            return;
        }
        node = node->right;
    }

    printf("File '%s' not found in current directory.\n", filename);
}
