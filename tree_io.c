#include "tree_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 재귀 헬퍼: 노드 정보 및 파일 내용을 파일로 출력
void save_tree_helper(FILE* file, TreeNode* node, int level) {
    if (!node) return;
    // 타입과 이름
    fprintf(file, "%*s%c %s\n", level * 2, "", node->type, node->name);
    // 파일 노드면 content 덤프
    if (node->type == 'f' && node->content) {
        // content 크기 기록
        fprintf(file, "%*s[content:%d]\n", level * 2, "", node->size);
        // raw data
        fwrite(node->content, 1, node->size, file);
        // 마무리 newline
        fprintf(file, "\n");
    }
    // 자식·형제 순으로 재귀
    save_tree_helper(file, node->left,  level + 1);
    save_tree_helper(file, node->right, level);
}

// 재귀 헬퍼: 파일로부터 노드와 content 복원
TreeNode* load_tree_helper(FILE* file, int level) {
    char line[1024];
    if (!fgets(line, sizeof(line), file)) return NULL;

    // 들여쓰기 레벨 확인
    int cur_lvl = 0;
    while (line[cur_lvl] == ' ' && cur_lvl < level * 2) cur_lvl++;
    if (cur_lvl != level * 2) {
        // 잘못된 레벨이면 포인터 복원
        fseek(file, -strlen(line), SEEK_CUR);
        return NULL;
    }

    // 노드 생성 및 기본 필드 설정
    TreeNode* node = malloc(sizeof(TreeNode));
    sscanf(line + cur_lvl, "%c %s", &node->type, node->name);
    node->left = node->right = node->parent = NULL;
    node->content = NULL;
    node->size    = 0;

    // 파일 노드이면 content 읽기 시도
    if (node->type == 'f') {
        long pos = ftell(file);
        char peek[64];
        if (fgets(peek, sizeof(peek), file)) {
            int sz;
            if (sscanf(peek + cur_lvl, "[content:%d]", &sz) == 1) {
                node->size = sz;
                node->content = malloc(sz);
                fread(node->content, 1, sz, file);
                // 다음 newline 소비
                fgetc(file);
            } else {
                // marker 아니라면 복원
                fseek(file, pos, SEEK_SET);
            }
        }
    }

    // 자식·형제 로드
    node->left  = load_tree_helper(file, level + 1);
    if (node->left)  node->left->parent  = node;
    node->right = load_tree_helper(file, level);
    if (node->right) node->right->parent = node->parent;

    return node;
}

void save_tree_to_file(DirectoryTree* dTree, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) { perror("fopen"); return; }
    save_tree_helper(f, dTree->root, 0);
    fclose(f);
}

void load_tree_from_file(DirectoryTree* dTree, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        // 초기화: 빈 루트 디렉터리
        TreeNode* root = malloc(sizeof(TreeNode));
        strcpy(root->name, "/");
        root->type    = 'd';
        root->mode    = 0;
        root->size    = 0;
        root->content = NULL;
        root->left = root->right = root->parent = NULL;
        dTree->root    = dTree->current = root;
        strcpy(dTree->current_path, "/");
        return;
    }
    dTree->root    = load_tree_helper(f, 0);
    dTree->current = dTree->root;
    strcpy(dTree->current_path, "/");
    fclose(f);
}