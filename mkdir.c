//mkdir.c
#include "mkdir.h"

void run_mkdir(DirectoryTree* tree, const char* dirname) {
    if (dirname == NULL || strlen(dirname) == 0) {
        printf("mkdir: missing operand\n");
        printf("Usage: mkdir <directory_name>\n");
        return;
    }

    // 중복 확인
    TreeNode* node = tree->current->children;
    while (node != NULL) {
        if (strcmp(node->name, dirname) == 0 && node->type == 'd') {
            printf("mkdir: cannot create directory '%s': File exists\n", dirname);
            return;
        }
        node = node->next_sibling;
    }

    // 새 노드 할당
    TreeNode* new_dir = malloc(sizeof(TreeNode));
    if (!new_dir) {
        perror("malloc failed");
        return;
    }

    // 속성 설정
    strncpy(new_dir->name, dirname, MAX_NAME_LENGTH);
    new_dir->type = 'd';
    new_dir->mode = 755;
    new_dir->size = 4096;
    new_dir->UID = 0;  // 기본값, 확장 시 사용자 UID 반영 가능
    new_dir->GID = 0;

    time_t t = time(NULL);
    struct tm* tm_info = localtime(&t);
    new_dir->month = tm_info->tm_mon + 1;
    new_dir->day = tm_info->tm_mday;
    new_dir->hour = tm_info->tm_hour;
    new_dir->minute = tm_info->tm_min;

    // 트리 연결
    new_dir->parent = tree->current;
    new_dir->children = NULL;
    new_dir->next_sibling = NULL;

    if (tree->current->children == NULL) {
        tree->current->children = new_dir;
    } else {
        TreeNode* last = tree->current->children;
        while (last->next_sibling != NULL) {
            last = last->next_sibling;
        }
        last->next_sibling = new_dir;
    }

    printf("Directory '%s' created successfully.\n", dirname);
}
