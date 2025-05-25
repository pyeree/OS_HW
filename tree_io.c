// tree_io.c
#include "tree_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 내부 헬퍼: static 제거!
void save_tree_helper(FILE* file, TreeNode* node, int level) {
    if (!node) return;
    fprintf(file, "%*s%c %s\n", level * 2, "", node->type, node->name);
    save_tree_helper(file, node->left,  level + 1);
    save_tree_helper(file, node->right, level);
}

TreeNode* load_tree_helper(FILE* file, int level) {
    char line[1024];
    if (!fgets(line, sizeof(line), file)) return NULL;

    // 들여쓰기 레벨 계산
    int cur_lvl = 0;
    while (line[cur_lvl] == ' ' && cur_lvl < level * 2) cur_lvl++;
    if (cur_lvl != level * 2) {
        // 잘못된 레벨이면 뒤로 파일 포인터 복원
        fseek(file, -strlen(line), SEEK_CUR);
        return NULL;
    }

    // 노드 생성
    TreeNode* node = malloc(sizeof(TreeNode));
    sscanf(line + cur_lvl, "%c %s", &node->type, node->name);

    // 자식·형제 로드
    node->left  = load_tree_helper(file, level + 1);
    if (node->left)  node->left->parent = node;
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
        // 파일 없으면 빈 트리 초기화
        TreeNode* root = malloc(sizeof(TreeNode));
        strcpy(root->name, "/");
        root->type   = 'd';
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
