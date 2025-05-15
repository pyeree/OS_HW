#include "header.h"
#include "tree_io.h"

#define SAVE_FILE "tree_state.txt"

DirectoryTree dTree;

void initialize_directory_tree(DirectoryTree* dTree) {
    TreeNode* root = malloc(sizeof(TreeNode)); // 루트 노드 초기화
    strcpy(root->name, "/");
    root->type = 'd';
    root->left = NULL;
    root->right = NULL;
    root->parent = NULL;
    strncpy(dTree->root->name, "/", MAX_NAME_LENGTH);
    dTree->root->type = 'd';
    dTree->root->mode = 755;
    dTree->root->size = 4096;
    dTree->root->UID = 0;
    dTree->root->GID = 0;
    dTree->root->month = 1;
    dTree->root->day = 1;
    dTree->root->hour = 0;
    dTree->root->minute = 0;
    dTree->current = dTree->root;
    dTree->root = root;
    dTree->current = root;
    strcpy(dTree->current_path, "team6@ubuntu: ");
}

int main() {

    load_tree_from_file(&dTree, SAVE_FILE);
    if (dTree.root == NULL) {
        initialize_directory_tree(&dTree); // 파일이 없거나 루트가 NULL일 경우 초기화
    }
    else {

        printf("Directory tree loaded successfully from %s\n", SAVE_FILE); // 수정된 부분: 파일 로드 성공 메시지

    }
    char command[256];
    while (1) {
        printf("%s ", dTree.current_path);
        fgets(command, sizeof(command), stdin);

        command[strcspn(command, "\n")] = '\0';  // Remove newline character

        int argc = 0;
        int length = strlen(command);
        for (int i = 0; i < length; i++) {
            if (command[i] == ' ') {
                argc++;
            }
        }

        char* cmd = strtok(command, " ");
        if (cmd == NULL) continue;

        if (strcmp(cmd, "ls") == 0) {
            char* arg = strtok(NULL, " ");
            if (arg == NULL) {
                ls(&dTree);
            }
            else if (strcmp(arg, "-a") == 0) {
                ls_a(&dTree);
            }
            else if (strcmp(arg, "-l") == 0) {
                ls_l(&dTree);
            }
            else if (strcmp(arg, "-al") == 0) {
                ls_al(&dTree);
            }
            else {
                printf("ls: missing operand\n");
                printf("Try 'man ls' for more information.\n");
            }
        }
        else if (strcmp(cmd, "clear") == 0)
        {
            clear();
        }
        else if (strcmp(cmd, "exit") == 0)
        {
            save_tree_to_file(&dTree, SAVE_FILE); // 종료 시 트리 저장
            break;
        }
        else
        {
            printf("Invalid command: %s\n", cmd);
        }
    }
}