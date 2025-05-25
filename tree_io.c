#include "tree_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 재귀 저장 helper
static void save_tree_helper(FILE* f, TreeNode* node, int lvl) {
    if (!node) return;
    fprintf(f, "%*s%c %s\n", lvl*2, "", node->type, node->name);
    save_tree_helper(f, node->left,  lvl+1);
    save_tree_helper(f, node->right, lvl);
}

void save_tree_to_file(DirectoryTree* dTree, const char* filename) {
    FILE* f = fopen(filename, "w");
    if (!f) { perror("fopen"); return; }
    save_tree_helper(f, dTree->root, 0);
    fclose(f);
}

// 재귀 로드 helper
static TreeNode* load_tree_helper(FILE* f, int lvl) {
    char line[1024];
    if (!fgets(line, sizeof(line), f)) return NULL;

    int indent = 0;
    while (line[indent]==' ' && indent < lvl*2) indent++;
    if (indent != lvl*2) {
        fseek(f, -strlen(line), SEEK_CUR);
        return NULL;
    }

    TreeNode* node = malloc(sizeof(TreeNode));
    sscanf(line+indent, "%c %s\n", &node->type, node->name);
    node->left   = load_tree_helper(f, lvl+1);
    node->right  = load_tree_helper(f, lvl);
    node->parent = NULL;
    return node;
}

// parent 포인터 복원 helper
static void set_parents(TreeNode* node, TreeNode* parent) {
    if (!node) return;
    node->parent = parent;
    set_parents(node->left,  node);
    set_parents(node->right, parent);
}

void load_tree_from_file(DirectoryTree* dTree, const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        // 파일 없으면 루트만 초기화
        perror("fopen");
        TreeNode* root = malloc(sizeof(TreeNode));
        strcpy(root->name, "/");
        root->type = 'd';
        root->left = root->right = root->parent = NULL;
        dTree->root = root;
        dTree->current = root;
        strcpy(dTree->current_path, "/");
        return;
    }

    dTree->root = load_tree_helper(f, 0);
    fclose(f);
    // 부모 복원 (root 자체는 parent=NULL)
    set_parents(dTree->root->left, dTree->root);

    dTree->current = dTree->root;
    strcpy(dTree->current_path, "/");
}
