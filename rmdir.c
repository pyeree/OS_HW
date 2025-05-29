// rmdir.c
#include "rmdir.h"
#include "tree_io.h"    // save_tree_to_file 선언
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PARTS     100
#define MAX_PART_LEN  256

// ── 재귀적으로 서브트리 전체 메모리 해제
static void free_subtree(TreeNode* node) {
    if (!node) return;
    free_subtree(node->left);
    free_subtree(node->right);
    free(node);
}

// ── 디렉터리가 비어 있는지 확인 (자식이 없으면 비어 있음)
int is_empty_directory(TreeNode* dir) {
    return dir->left == NULL;
}

// ── parent의 left/right 링크에서 target 노드를 잘라내고 서브트리 해제
void remove_child(TreeNode* parent, TreeNode* target) {
    TreeNode* prev = NULL;
    TreeNode* curr = parent->left;
    while (curr && curr != target) {
        prev = curr;
        curr = curr->right;
    }
    if (!curr) return;
    if (prev) {
        prev->right = curr->right;
    } else {
        parent->left = curr->right;
    }
    free_subtree(curr);
}

// ── parent 아래에 이름이 name인 디렉터리 노드 검색
TreeNode* find_child_dir(TreeNode* parent, const char* name) {
    TreeNode* curr = parent->left;
    while (curr) {
        if (curr->type == 'd' && strcmp(curr->name, name) == 0) {
            return curr;
        }
        curr = curr->right;
    }
    return NULL;
}

// ── 경로 분할 ("/a/b/c" → ["a","b","c"])
int split_path(const char* path, char parts[MAX_PARTS][MAX_PART_LEN]) {
    int count = 0, j = 0;
    while (*path) {
        if (*path == '/') {
            if (j > 0) {
                parts[count][j] = '\0';
                count++; j = 0;
            }
            path++;
            continue;
        }
        if (j < MAX_PART_LEN-1) parts[count][j++] = *path;
        path++;
    }
    if (j > 0) {
        parts[count][j] = '\0';
        count++;
    }
    return count;
}

// ── 현재 디렉터리(dTree->current) 아래서 단일 디렉터리 제거
int remove_dir(DirectoryTree* dTree, const char* dir_name) {
    TreeNode* parent = dTree->current;
    TreeNode* target = find_child_dir(parent, dir_name);
    if (!target) {
        printf("rmdir: failed to remove '%s': No such directory\n", dir_name);
        return -1;
    }
    if (!is_empty_directory(target)) {
        printf("rmdir: failed to remove '%s': Directory not empty\n", dir_name);
        return -1;
    }
    remove_child(parent, target);
    save_tree_to_file(dTree, "filesystem.txt");  // tree_io와 동기화:contentReference[oaicite:1]{index=1}
    printf("Directory '%s' removed successfully.\n", dir_name);
    return 0;
}

// ── 경로 전체를 받아 마지막 요소만 제거 (예: "a/b/c" → c만 제거)
int remove_dir_path(DirectoryTree* dTree, const char* path) {
    char parts[MAX_PARTS][MAX_PART_LEN];
    int cnt = split_path(path, parts);
    if (cnt == 0) {
        printf("rmdir: invalid path\n");
        return -1;
    }

    TreeNode* orig = dTree->current;
    for (int i = 0; i < cnt - 1; i++) {
        TreeNode* next = find_child_dir(dTree->current, parts[i]);
        if (!next) {
            printf("rmdir: failed to remove '%s': No such directory\n", path);
            dTree->current = orig;
            return -1;
        }
        dTree->current = next;
    }
    int res = remove_dir(dTree, parts[cnt - 1]);
    dTree->current = orig;
    return res;
}

// ── -p 옵션처럼 경로상의 모든 요소를 뒤에서부터 제거
int remove_dir_p_path(DirectoryTree* dTree, const char* path) {
    char parts[MAX_PARTS][MAX_PART_LEN];
    int cnt = split_path(path, parts);
    if (cnt == 0) {
        printf("rmdir: invalid path\n");
        return -1;
    }

    TreeNode* orig = dTree->current;
    for (int i = cnt - 1; i >= 0; i--) {
        dTree->current = orig;
        for (int j = 0; j < i; j++) {
            TreeNode* next = find_child_dir(dTree->current, parts[j]);
            if (!next) {
                printf("rmdir: failed to remove '%s': No such directory\n", path);
                dTree->current = orig;
                return -1;
            }
            dTree->current = next;
        }
        if (remove_dir(dTree, parts[i]) != 0) {
            dTree->current = orig;
            return -1;
        }
    }
    dTree->current = orig;
    return 0;
}
