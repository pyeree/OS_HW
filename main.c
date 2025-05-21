#include "header.h"
#include "tree_io.h"
#include "cat.h"
#include "cd.h"
// #include "chmod.h"
#include "clear.h"
#include "diff.h"
#include "ls.h"
// #include "mkdir.h"
#include "mv.h"
// #include "pwd.h"
#include "touch.h"
#include "whereis.h"

#define SAVE_FILE "tree_state.txt"

DirectoryTree dTree;

void initialize_directory_tree(DirectoryTree* dTree) {
    TreeNode* root = malloc(sizeof(TreeNode)); // 메모리 할당
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
    strcpy(dTree->current_path, "team4@ubuntu: ");
}

int main() {

    load_tree_from_file(&dTree, SAVE_FILE);
    if (dTree.root == NULL) { // 트리가 비어있다면 초기화
        initialize_directory_tree(&dTree);
    }
    else {
        printf("Directory tree loaded successfully from %s\n", SAVE_FILE);
    }

    printf("Initial current path: %s\n", dTree.current_path);
    char command[256];
    while (1) {
        printf("%s ", dTree.current_path);
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0'; // 개행 문자 제거

        int argc = 0;
        int length = strlen(command);
        for (int i = 0; i < length; i++) {
            if (command[i] == ' ') {
                argc++;
            }
        }

        char* cmd = strtok(command, " ");
        if (cmd == NULL) continue;

        if (strcmp(cmd, "cat") == 0) {
            char *arg = strtok(NULL, " ");
            if (arg != NULL) {
                cat(&dTree, arg);
                save_tree_to_file(&dTree, SAVE_FILE); // 항상 명령어 실행 후 트리 저장
            } else {
                printf("cat: missing argument\n");
                printf("Try 'man cat' for more information.\n");
            }
        } 
        else if (strcmp(cmd, "cd") == 0) {
				    char *arg = strtok(NULL, " ");
				    if (arg != NULL) {
				        cd(&dTree, arg);
				        save_tree_to_file(&dTree, SAVE_FILE);
				    } else {
		        printf("cd: missing argument\n");
		    }
        }
        // else if (strcmp(cmd, "chmod") == 0)
        else if (strcmp(cmd, "clear") == 0) { clear(); }
        else if (strcmp(cmd, "diff") == 0) {
            char *arg1 = strtok(NULL, " ");
            char *arg2 = strtok(NULL, " ");
            if (arg1 != NULL && arg2 != NULL) {
                diff(arg1, arg2);
            } else {
                printf("diff: missing file operand\n");
                printf("Try 'man diff' for more information.\n");
            }
        }
        else if (strcmp(cmd, "ls") == 0) {
            char *arg = strtok(NULL, " ");
            if (arg == NULL) {
                ls(&dTree);
            } else if (strcmp(arg, "-a") == 0) {
                ls_a(&dTree);
            } else if (strcmp(arg, "-l") == 0) {
                ls_l(&dTree);
            } else if (strcmp(arg, "-al") == 0) {
                ls_al(&dTree);
            } else {
                printf("ls: missing operand\n");
                printf("Try 'man ls' for more information.\n");
            }
        } 
        /*
        else if (strcmp(cmd, "mkdir") == 0) {
            if (argc != 0) {
                char *argv[256];
                argv[0] = "mkdir";
                for (int d = 1; d < argc + 1; d++) {
                argv[d] = strtok(NULL, " ");
                }

                a(argc, argv);
                save_tree_to_file(&dTree, SAVE_FILE); // 항상 명령어 실행 후 트리 저장
            }
            else {
                printf("mkdir: missing operand\n");
                printf("Try 'man mkdir' for more information.\n");
            }
        } */
       /*
        else if (strcmp(cmd, "mv") == 0) {
            char *src = strtok(NULL, " ");
            char *dest = strtok(NULL, " ");
            if (src != NULL && dest != NULL) {
                mv(&dTree, src, dest);
                save_tree_to_file(&dTree, SAVE_FILE);
            } else {
                printf("mv: missing operand\n");
                printf("Try 'man mv' for more information.\n");
            }
        } */
        /*
        else if (strcmp(cmd, "pwd") == 0) {
            load_tree_from_file(&dTree, "filesystem.txt");  // 파일에서 트리 로드
            get_pwd(&dTree);  // 트리에서 현재 경로 출력
        } */
        else if (strcmp(cmd, "touch") == 0) {
            char *arg = strtok(NULL, " ");
            if (arg != NULL) {
                touch(&dTree, arg);
                save_tree_to_file(&dTree, SAVE_FILE);
            } else {
                printf("touch: missing argument\n");
            }
        }
        else if (strcmp(cmd, "whereis") == 0) {
            char *arg = strtok(NULL, " ");
            if (arg != NULL) {
                    whereis(&dTree, arg);
            } else {
                printf("whereis: missing argument\n");
            }
        }
        else if (strcmp(cmd, "exit") == 0) {
            save_tree_to_file(&dTree, SAVE_FILE); // ���� �� Ʈ�� ����
            break;
        }
        else {
            printf("Invalid command: %s\n", cmd);
        }
    }
}