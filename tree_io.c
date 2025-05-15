#include "header.h"
#include "tree_io.h"

void save_tree_to_file(DirectoryTree* dTree, const char* filename);
void load_tree_from_file(DirectoryTree* dTree, const char* filename);

void save_tree_helper(FILE* file, TreeNode* node, int level) {
    if (!node)
        return;
    fprintf(file, "%*s%c %s\n", level * 2, "", node->type, node->name);
    save_tree_helper(file, node->left, level + 1);
    save_tree_helper(file, node->right, level);
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

TreeNode* load_tree_helper(FILE* file, int level) {
    char line[1024];
    TreeNode* node = NULL;

    if (fgets(line, sizeof(line), file) == NULL)
        return NULL;

    int current_level = 0;
    while (line[current_level] == ' ' && current_level < level * 2)
        current_level++;
    if (current_level != level * 2) {
        fseek(file, -strlen(line), SEEK_CUR);
        return NULL;
    }

    node = malloc(sizeof(TreeNode));
    sscanf(line + current_level, "%c %s\n", &node->type, node->name);
    node->left = load_tree_helper(file, level + 1);
    node->right = load_tree_helper(file, level);

    return node;
}

void load_tree_from_file(DirectoryTree* dTree, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        // 파일이 존재하지 않으면 빈 트리 초기화
        perror("fopen"); // 수정된 부분: 파일이 없을 때의 오류 메시지 출력
        TreeNode* root = malloc(sizeof(TreeNode)); //빈 트리 초기화
        strcpy(root->name, "/");
        root->type = 'd';
        root->left = NULL;
        root->right = NULL;
        root->parent = NULL;
        dTree->root = root;
        dTree->current = root;
        strcpy(dTree->current_path, "Team4:");
        return;
    }
    dTree->root = load_tree_helper(file, 0);
    dTree->current = dTree->root;
    strcpy(dTree->current_path, "Team4:");
    fclose(file);
}