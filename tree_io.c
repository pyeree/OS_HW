#include "header.h"
#include "tree_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 트리 저장용 도우미 함수
void save_tree_helper(FILE* file, TreeNode* node, int level) {
    if (!node) return;
    fprintf(file, "%*s%c %s\n", level * 2, "", node->type, node->name);
    save_tree_helper(file, node->left, level + 1);  // 자식
    save_tree_helper(file, node->right, level);     // 형제
}

void save_tree_to_file(DirectoryTree* dTree, const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        perror("fopen");
        return;
    }
    save_tree_helper(file, dTree->root, 0);
    fclose(file);
}

// 트리 복원용 도우미 함수
TreeNode* load_tree_helper(FILE* file, int level) {
    char line[1024];
    TreeNode* node = NULL;

    if (fgets(line, sizeof(line), file) == NULL)
        return NULL;

    int current_level = 0;
    while (line[current_level] == ' ' && current_level < level * 2)
        current_level++;
    if (current_level != level * 2) {
        fseek(file, -strlen(line), SEEK_CUR);  // 줄 되돌리기
        return NULL;
    }

    node = malloc(sizeof(TreeNode));
    sscanf(line + current_level, "%c %s\n", &node->type, node->name);
    node->left = load_tree_helper(file, level + 1);   // 자식
    node->right = load_tree_helper(file, level);      // 형제

    return node;
}

// 🔧 핵심: 트리 구조 내 모든 노드에 parent 연결 복구
void set_parents(TreeNode *node, TreeNode *parent) {
    if (!node) return;
    node->parent = parent;
    set_parents(node->left, node);    // 자식 → 부모 자신
    set_parents(node->right, parent); // 형제 → 같은 부모
}

void load_tree_from_file(DirectoryTree* dTree, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        // 파일 없으면 루트 디렉토리 초기화
        perror("fopen");
        TreeNode* root = malloc(sizeof(TreeNode));
        strcpy(root->name, "/");
        root->type = 'd';
        root->left = NULL;
        root->right = NULL;
        root->parent = NULL;

        dTree->root = root;
        dTree->current = root;
        strcpy(dTree->current_path, "team4@ubuntu: /");
        return;
    }

    dTree->root = load_tree_helper(file, 0);
    fclose(file);

    // 🔥 반드시 parent 복구!
    set_parents(dTree->root->left, dTree->root);

    dTree->current = dTree->root;
    strcpy(dTree->current_path, "team4@ubuntu: /");
}
