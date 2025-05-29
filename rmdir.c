// rmdir.c

#include "rmdir.h"
#include "header.h"
#include "tree_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PARTS     100
#define MAX_PART_LEN  256

// ── 재귀: left/right 기반 subtree 전체 메모리 해제
static void free_subtree(TreeNode* node) {
    if (!node) return;
    free_subtree(node->left);
    free_subtree(node->right);
    free(node);
}

// ── 빈 디렉터리인지 확인 (left 가 NULL 이면 비어 있음)
int is_empty_directory(TreeNode* dir) {
    return dir->left == NULL;
}

// ── parent->left/right 리스트에서 target 노드를 잘라내고 해제
void remove_child(TreeNode* parent, TreeNode* target) {
    TreeNode *curr = parent->left, *prev = NULL;
    while (curr) {
        if (curr == target) {
            if (prev) prev->right = curr->right;
            else      parent->left  = curr->right;
            free_subtree(curr);
            return;
        }
        prev = curr;
        curr = curr->right;
    }
}

// ── 이름으로 child 디렉터리 찾기 (left/right siblings)
TreeNode* find_child_dir(TreeNode* parent, const char* name) {
    for (TreeNode* c = parent->left; c; c = c->right) {
        if (c->type == 'd' && strcmp(c->name, name) == 0)
            return c;
    }
    return NULL;
}

// ── 경로("/a/b/c") 분할 → ["a","b","c"]
int split_path(const char* path, char parts[MAX_PARTS][MAX_PART_LEN]) {
    int cnt = 0, j = 0;
    while (*path) {
        if (*path == '/') {
            if (j > 0) {
                parts[cnt][j] = '\0';
                cnt++; j = 0;
            }
            path++; continue;
        }
        if (j < MAX_PART_LEN-1) parts[cnt][j++] = *path;
        path++;
    }
    if (j > 0) {
        parts[cnt][j] = '\0';
        cnt++;
    }
    return cnt;
}

// ── 현재 디렉터리(dTree->current) 바로 아래 단일 삭제
int remove_dir(DirectoryTree* dTree, const char* dir_name) {
    TreeNode* parent = dTree->current;
    TreeNode* target = find_child_dir(parent, dir_name);
    if (!target) {
        printf("rmdir: '%s': No such directory\n", dir_name);
        return -1;
    }
    if (!is_empty_directory(target)) {
        printf("rmdir: '%s': Directory not empty\n", dir_name);
        return -1;
    }
    remove_child(parent, target);
    save_tree_to_file(dTree, "tree_state.txt");
    printf("Directory '%s' removed successfully.\n", dir_name);
    return 0;
}

// ── 경로의 마지막 요소만 삭제 (예: "a/b/c" → c만)
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
            printf("rmdir: '%s': No such directory\n", path);
            dTree->current = orig;
            return -1;
        }
        dTree->current = next;
    }
    int res = remove_dir(dTree, parts[cnt - 1]);
    dTree->current = orig;
    return res;
}

// ── -p 옵션처럼 경로상의 모든 요소 뒤에서부터 삭제
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
                printf("rmdir: '%s': No such directory\n", path);
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
