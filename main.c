// main.c
#include "header.h"
#include "tree_io.h"
#include "cat.h"
#include "cd.h"
#include "chmod.h"
#include "clear.h"
#include "diff.h"
#include "ls.h"
#include "mkdir.h"
#include "mv.h"
#include "pwd.h"
#include "touch.h"
#include "whereis.h"

#define SAVE_FILE "tree_state.txt"

DirectoryTree dTree;

void initialize_directory_tree(DirectoryTree* dTree) {
    TreeNode* root = malloc(sizeof(TreeNode));
    strcpy(root->name, "/");
    root->type = 'd';
    root->left = root->right = root->parent = NULL;
    dTree->root = root;
    dTree->current = root;
    // 초기 current_path 를 "/" 로 설정
    snprintf(dTree->current_path, sizeof(dTree->current_path), "/");
}

int main() {
    // 1) 트리 로드 또는 초기화
    load_tree_from_file(&dTree, SAVE_FILE);
    if (!dTree.root) {
        initialize_directory_tree(&dTree);
    } else {
        // load_tree_from_file 내부에서 current_path를 "/"로 세팅했다면
        printf("Directory tree loaded successfully from %s\n", SAVE_FILE);
    }

    // 2) REPL 루프
    char line[256];
    while (1) {
        // 항상 절대경로를 포함한 프롬프트
        printf("team4@ubuntu:%s> ", dTree.current_path);

        if (!fgets(line, sizeof(line), stdin))
            break;
        // 개행 제거
        line[strcspn(line, "\n")] = '\0';
        if (line[0] == '\0')
            continue;

        // 토큰화
        char* cmd = strtok(line, " ");
        if (!cmd) continue;

        // 각 명령 분기
        if (strcmp(cmd, "cat") == 0) {
            char* arg = strtok(NULL, " ");
            if (arg) {
                cat(&dTree, arg);
                save_tree_to_file(&dTree, SAVE_FILE);
            } else {
                printf("cat: missing argument\n");
            }

        } else if (strcmp(cmd, "cd") == 0) {
            char *arg = strtok(NULL, " ");
        if (arg) {
            cd(&dTree, arg);
            save_tree_to_file(&dTree, SAVE_FILE);
        } else {
            printf("cd: missing argument\n");
        }

        } else if (strcmp(cmd, "pwd") == 0) {
            // pwd 는 tree 에 저장된 current 경로를 출력
            update_current_path(&dTree);
            printf("%s\n", dTree.current_path);

        } else if (strcmp(cmd, "ls") == 0) {
            char* arg = strtok(NULL, " ");
            if (!arg) {
                ls(&dTree);
            } else if (strcmp(arg, "-a") == 0) {
                ls_a(&dTree);
            } else if (strcmp(arg, "-l") == 0) {
                ls_l(&dTree);
            } else if (strcmp(arg, "-al") == 0) {
                ls_al(&dTree);
            } else {
                printf("ls: invalid option '%s'\n", arg);
            }

        } else if (strcmp(cmd, "mkdir") == 0) {
            char* args = strtok(NULL, "");
            if (args) {
                // mode 0755 로 디렉터리 생성 :contentReference[oaicite:4]{index=4}
                run_mkdir_multithread(&dTree, args, 0755);
                save_tree_to_file(&dTree, SAVE_FILE);
            } else {
                printf("mkdir: missing operand\n");
            }
        }else if (strcmp(cmd, "rmdir") == 0) {
            // 단일 경로나 경로 문자열(“a/b/c”) 지원
            char* path = strtok(NULL, " ");
            if (path) {
                remove_dir_path(&dTree, path);
                save_tree_to_file(&dTree, SAVE_FILE);
            } else {
                printf("rmdir: missing operand\n");
            }
        } else if (strcmp(cmd, "mv") == 0) {
            char* src  = strtok(NULL, " ");
            char* dest = strtok(NULL, " ");
            if (src && dest) {
                mv(&dTree, src, dest);
                save_tree_to_file(&dTree, SAVE_FILE);
            } else {
                printf("mv: missing operand\n");
            }

        } else if (strcmp(cmd, "chmod") == 0) {
            run_chmod(&dTree, strtok(NULL, ""));
            save_tree_to_file(&dTree, SAVE_FILE);

        } else if (strcmp(cmd, "clear") == 0) {
            clear();

        } else if (strcmp(cmd, "diff") == 0) {
            char* f1 = strtok(NULL, " ");
            char* f2 = strtok(NULL, " ");
            if (f1 && f2) {
                diff(f1, f2);
            } else {
                printf("diff: missing file operand\n");
            }

        } else if (strcmp(cmd, "touch") == 0) {
            char* arg = strtok(NULL, " ");
            if (arg) {
                touch(&dTree, arg);
                save_tree_to_file(&dTree, SAVE_FILE);
            } else {
                printf("touch: missing argument\n");
            }

        } else if (strcmp(cmd, "whereis") == 0) {
            char* arg = strtok(NULL, " ");
            if (arg) {
                whereis(&dTree, arg);
            } else {
                printf("whereis: missing argument\n");
            }

        } else if (strcmp(cmd, "exit") == 0) {
            save_tree_to_file(&dTree, SAVE_FILE);
            break;

        } else {
            printf("Invalid command: %s\n", cmd);
        }
    }

    return 0;
}
