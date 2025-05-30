// mv.c

#include "mv.h"
#include "header.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

static TreeNode* search_path(DirectoryTree *dTree,
                             const char *path,
                             char want_type)
{
    // 절대 vs 상대
    TreeNode *cur = (path[0]=='/') ? dTree->root : dTree->current;
    // “/” 삭제하고 분할
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
    if (want_type=='\0' || cur->type==want_type)
        return cur;
    return NULL;
}

void mv(DirectoryTree *dTree, char *src, char *dest) {
    // ── 0) 파일이면 실제 디스크도 rename
    TreeNode *fnode = search_path(dTree, src, 'f');
    if (fnode) {
        if (rename(src, dest) != 0) {
            perror("mv");
            return;
        }
    }

    // ── 1) 메모리 트리에서 노드 검색
    TreeNode *node = search_path(dTree, src, 'f');
    if (!node) node = search_path(dTree, src, 'd');
    if (!node) {
        printf("mv: cannot stat '%s': No such file or directory\n", src);
        return;
    }

    // ── 2) 부모에서 detatch
    TreeNode *parent = node->parent;
    TreeNode *prev = NULL, *iter = parent->left;
    while (iter && iter != node) {
        prev = iter;
        iter = iter->right;
    }
    if (iter) {
        if (prev) prev->right = iter->right;
        else      parent->left  = iter->right;
        node->right = NULL;
    }

    // ── 3) dest 파싱: slash 유무로 디렉터리 vs 파일이름 결정
    int  is_dir_target = 0;
    char dirpart[MAX_PATH_LENGTH]  = "";
    char basename[MAX_NAME_LENGTH] = "";
    char *slash = strrchr(dest, '/');
    if (slash) {
        size_t dlen = slash - dest;
        strncpy(dirpart, dest, dlen);
        dirpart[dlen] = '\0';
        strcpy(basename, slash+1);
        if (basename[0]=='\0') is_dir_target = 1;
    } else {
        TreeNode *td = search_path(dTree, dest, 'd');
        if (td) {
            is_dir_target = 1;
            strcpy(dirpart, dest);
        } else {
            strcpy(basename, dest);
        }
    }

    // ── 4) targetDir 찾기
    TreeNode *targetDir = NULL;
    if (is_dir_target) {
        if (dirpart[0]=='\0') targetDir = dTree->current;
        else                  targetDir = search_path(dTree, dirpart, 'd');
        if (!targetDir) {
            printf("mv: target directory '%s' not found\n", dirpart);
            return;
        }
    }

    // ── 5) 트리에 attach & 이름 변경
    if (is_dir_target) {
        if (basename[0]) {
            strncpy(node->name, basename, MAX_NAME_LENGTH);
            node->name[MAX_NAME_LENGTH-1] = '\0';
        }
        node->parent = targetDir;
        if (!targetDir->left) {
            targetDir->left = node;
        } else {
            TreeNode *t = targetDir->left;
            while (t->right) t = t->right;
            t->right = node;
        }
        printf("mv: '%s' -> '%s/%s'\n",
               src, (dirpart[0]?dirpart:"."), node->name);
    } else {
        // 단순 파일명 변경, cwd에 붙이기
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
