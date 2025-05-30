// mv.c

#include "mv.h"
#include "header.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static TreeNode* search_path(DirectoryTree *dTree, const char *path, char want_type) {
    // 절대 vs 상대
    TreeNode *cur = (path[0]=='/') ? dTree->root : dTree->current;

    // 복사해서 strtok
    char buf[MAX_PATH_LENGTH];
    strncpy(buf, path + (path[0]=='/'?1:0), sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    char *tok = strtok(buf, "/");
    while (tok) {
        TreeNode *child = cur->left;
        while (child && strcmp(child->name, tok) != 0)
            child = child->right;
        if (!child) return NULL;
        cur = child;
        tok = strtok(NULL, "/");
    }
    // 타입 검사 (want_type=='\0' 이면 무시)
    if (want_type == '\0' || cur->type == want_type)
        return cur;
    return NULL;
}

void mv(DirectoryTree *dTree, char *src, char *dest) {
    // 1) 소스 찾기 (파일 또는 디렉터리)
    TreeNode *node = search_path(dTree, src, 'f');
    if (!node) node = search_path(dTree, src, 'd');
    if (!node) {
        printf("mv: cannot stat '%s': No such file or directory\n", src);
        return;
    }

    // 2) 부모에서 분리
    TreeNode *parent = node->parent;
    TreeNode *prev = NULL, *iter = parent->left;
    while (iter && iter != node) {
        prev = iter;
        iter = iter->right;
    }
    if (iter) {
        if (prev) prev->right = iter->right;
        else       parent->left  = iter->right;
        node->right = NULL;
    }

    // 3) 목적지 파싱
    int is_dir_target = 0;
    char  dirpart[MAX_PATH_LENGTH] = "";
    char  basename[MAX_NAME_LENGTH] = "";
    char *slash = strrchr(dest, '/');
    if (slash) {
        size_t dlen = slash - dest;
        strncpy(dirpart, dest, dlen);
        dirpart[dlen] = '\0';
        strcpy(basename, slash+1);
        if (basename[0]=='\0') {
            // trailing slash만 있는 경우
            is_dir_target = 1;
        }
    } else {
        // no slash: maybe dest is an existing directory in cwd
        TreeNode *td = search_path(dTree, dest, 'd');
        if (td) {
            is_dir_target = 1;
            strcpy(dirpart, dest);
        } else {
            strcpy(basename, dest);
        }
    }

    // 4) 대상 디렉터리 찾기
    TreeNode *targetDir = NULL;
    if (is_dir_target) {
        // dirpart 비어 있으면 cwd
        if (dirpart[0]=='\0') targetDir = dTree->current;
        else                  targetDir = search_path(dTree, dirpart, 'd');
        if (!targetDir) {
            printf("mv: target directory '%s' not found\n", dirpart);
            return;
        }
    }

    // 5) 이동/이름 변경
    if (is_dir_target) {
        // 같은 이름 유지하거나, basename이 있으면 이름 변경
        if (basename[0]) {
            strncpy(node->name, basename, MAX_NAME_LENGTH);
            node->name[MAX_NAME_LENGTH-1] = '\0';
        }
        node->parent = targetDir;
        // attach as last sibling
        if (!targetDir->left) {
            targetDir->left = node;
        } else {
            TreeNode *t = targetDir->left;
            while (t->right) t = t->right;
            t->right = node;
        }
        printf("mv: '%s' -> '%s/%s'\n", src,
               (dirpart[0]?dirpart:"."), node->name);

    } else {
        // 단순 파일 이름 변경, cwd에 붙이기
        TreeNode *cwd = dTree->current;
        strncpy(node->name, basename, MAX_NAME_LENGTH);
        node->name[MAX_NAME_LENGTH-1] = '\0';
        node->parent = cwd;
        if (!cwd->left) {
            cwd->left = node;
        } else {
            TreeNode *t = cwd->left;
            while (t->right) t = t->right;
            t->right = node;
        }
        printf("mv: '%s' renamed to '%s'\n", src, basename);
    }
}
