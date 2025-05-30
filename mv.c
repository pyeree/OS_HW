// mv.c

#include "mv.h"
#include "header.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>   // for rename()

// 절대/상대 경로에서 'want_type' 노드 찾기 (type=='f' 또는 'd')
static TreeNode* search_path(DirectoryTree *dTree,
                             const char *path,
                             char want_type)
{
    TreeNode *cur = (path[0]=='/') ? dTree->root : dTree->current;
    char buf[MAX_PATH_LENGTH];
    // leading '/' 제거
    strncpy(buf, path + (path[0]=='/'?1:0), sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    // "/" 기준 토큰 탐색
    char *tok = strtok(buf, "/");
    while (tok) {
        TreeNode *child = cur->left;
        while (child && strcmp(child->name, tok) != 0)
            child = child->right;
        if (!child) return NULL;
        cur = child;
        tok = strtok(NULL, "/");
    }
    return (want_type=='\0' || cur->type==want_type) ? cur : NULL;
}

// 트리에서 node 분리
static void detach_node(TreeNode *node) {
    TreeNode *parent = node->parent;
    TreeNode *prev = NULL, *cur = parent->left;
    while (cur && cur != node) {
        prev = cur;
        cur  = cur->right;
    }
    if (!cur) return;
    if (prev) prev->right  = cur->right;
    else       parent->left = cur->right;
    cur->right = NULL;
    node->parent = NULL;
}

// 트리에 node 붙이기 (마지막 자식으로)
static void attach_node(TreeNode *parent, TreeNode *node) {
    node->parent     = parent;
    node->right      = NULL;
    if (!parent->left) {
        parent->left = node;
    } else {
        TreeNode *t = parent->left;
        while (t->right) t = t->right;
        t->right = node;
    }
}

void mv(DirectoryTree *dTree, char *src, char *dest) {
    // 0) 파일 노드일 경우, 실제 디스크에도 rename
    TreeNode *fnode = search_path(dTree, src, 'f');
    if (fnode) {
        if (rename(src, dest) != 0) {
            perror("mv");
            return;
        }
    }

    // 1) 메모리 트리에서 node 찾기
    TreeNode *node = search_path(dTree, src, 'f');
    if (!node) node = search_path(dTree, src, 'd');
    if (!node) {
        printf("mv: cannot stat '%s': No such file or directory\n", src);
        return;
    }

    // 2) 트리에서 분리(detach)
    detach_node(node);

    // 3) dest 파싱: “path/to/”와 “basename”
    int  is_dir_target = 0;
    char dirpart[MAX_PATH_LENGTH]  = "";
    char basename[MAX_NAME_LENGTH] = "";
    char *slash = strrchr(dest, '/');
    if (slash) {
        size_t dlen = slash - dest;
        strncpy(dirpart, dest, dlen);
        dirpart[dlen] = '\0';
        strcpy(basename, slash+1);
        // “foo/” 처럼 끝이 '/' 면 디렉터리 이동만
        if (basename[0]=='\0') is_dir_target = 1;
    } else {
        // “mv x existingDir” 형태인지 확인
        TreeNode *td = search_path(dTree, dest, 'd');
        if (td) {
            is_dir_target = 1;
            strcpy(dirpart, dest);
        } else {
            strcpy(basename, dest);
        }
    }

    // 4) 대상 디렉터리 찾기 (없으면 에러)
    TreeNode *targetDir = NULL;
    if (is_dir_target) {
        targetDir = (dirpart[0]=='\0')
                    ? dTree->current
                    : search_path(dTree, dirpart, 'd');
        if (!targetDir) {
            printf("mv: target directory '%s' not found\n", dirpart);
            // 실패 시 원래 자리에 재붙일 수도 있습니다.
            return;
        }
    }

    // 5) 트리에 재붙이기 (attach) & 이름 변경
    if (is_dir_target) {
        // 디렉터리 이동
        if (basename[0]) {
            // “aa/newname.txt” 같이 새 이름이 있을 때만 갱신
            strncpy(node->name, basename, MAX_NAME_LENGTH);
            node->name[MAX_NAME_LENGTH-1] = '\0';
        }
        attach_node(targetDir, node);
        printf("mv: '%s' -> '%s/%s'\n",
               src,
               (dirpart[0]? dirpart : "."),
               node->name);
    } else {
        // 단순 파일 이름 변경: cwd에 붙이기
        TreeNode *cwd = dTree->current;
        strncpy(node->name, basename, MAX_NAME_LENGTH);
        node->name[MAX_NAME_LENGTH-1] = '\0';
        attach_node(cwd, node);
        printf("mv: '%s' renamed to '%s'\n", src, node->name);
    }
}
