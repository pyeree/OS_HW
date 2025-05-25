//chmod.c
#include "chmod.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void run_chmod(DirectoryTree* tree, const char* arg) {
    if (arg == NULL || strlen(arg) < 5) {
        printf("Usage: chmod <mode: 3-digit number> <filename>\n");
        return;
    }

    // 인자 분리
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

    // 현재 디렉토리에서 탐색
    TreeNode* node = tree->current->children;
    while (node != NULL) {
        if (strcmp(node->name, filename) == 0) {
            node->mode = mode;
            printf("Permissions for '%s' changed to %03d\n", filename, mode);
            return;
        }
        node = node->next_sibling;
    }

    printf("File '%s' not found in current directory.\n", filename);
}
