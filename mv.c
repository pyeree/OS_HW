// mv.c

#include "mv.h"
#include "header.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>   // for rename()

// ── 트리에서 절대/상대 경로로 노드 찾기
static TreeNode* search_path(DirectoryTree *dTree,
                             const char *path,
                             char want_type)
{
    TreeNode *cur = (path[0]=='/') ? dTree->root : dTree->current;
    // 경로 앞의 '/' 제거
    char buf[MAX_PATH_LENGTH];
    strncpy(buf, path + (path[0]=='/' ? 1 : 0), sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    char *tok = strtok(buf, "/");
    while (tok) {
        TreeNode *child = cur->children;
        while (child && strcmp(child->name, tok) != 0)
            child = child->next_sibling;
        if (!child) return NULL;
        cur = child;
        tok = strtok(NULL, "/");
    }
    // 타입 검사 (want_type=='\0' 이면 무시)
    if (want_type=='\0' || cur->type == want_type)
        return cur;
    return NULL;
}

// ── 트리에서 node 분리(detach)
static void detach_node(TreeNode *node) {
    TreeNode *parent = node->parent;
    TreeNode *prev = NULL, *cur = parent->children;
    while (cur) {
        if (cur == node) {
            if (prev) prev->next_sibling = cur->next_sibling;
            else      parent->children  = cur->next_sibling;
            node->next_sibling = NULL;
            node->parent = NULL;
            return;
        }
        prev = cur;
        cur  = cur->next_sibling;
    }
}

// ── 트리에 node 붙이기(attach) : 마지막 자식으로
static void attach_node(TreeNode *parent, TreeNode *node) {
    node->parent = parent;
    node->next_sibling = NULL;
    if (!parent->children) {
        parent->children = node;
    } else {
        TreeNode *t = parent->children;
        while (t->next_sibling) t = t->next_sibling;
        t->next_sibling = node;
    }
}

void mv(DirectoryTree *dTree, char *src, char *dest) {
    // 0) 파일 노드라면 실제 디스크에도 rename
    TreeNode *fnode = search_path(dTree, src, 'f');
    if (fnode) {
        if (rename(src, dest) != 0) {
            perror("mv");
            return;
        }
    }

    // 1) 메모리 트리에서 노드 찾기 (파일 또는 디렉터리)
    TreeNode *node = search_path(dTree, src, 'f');
    if (!node) node = search_path(dTree, src, 'd');
    if (!node) {
        printf("mv: cannot stat '%s': No such file or directory\n", src);
        return;
    }

    // 2) 부모에서 분리
    detach_node(node);

    // 3) dest 파싱: 슬래시 유무로 디렉터리 이동 vs 이름 변경 판단
    int  is_dir_target = 0;
    char dirpart[MAX_PATH_LENGTH]  = "";
    char basename[MAX_NAME_LENGTH] = "";
    char *slash = strrchr(dest, '/');
    if (slash) {
        // “path/to/…/” 과 “name” 분리
        size_t dlen = slash - dest;
        strncpy(dirpart, dest, dlen);
        dirpart[dlen] = '\0';
        strcpy(basename, slash + 1);
        if (basename[0] == '\0') {
            // “foo/” 처럼 끝이 slash 면, 디렉터리만 지정
            is_dir_target = 1;
        }
    } else {
        // “mv x dir” 꼴인지 확인
        TreeNode *td = search_path(dTree, dest, 'd');
        if (td) {
            is_dir_target = 1;
            strcpy(dirpart, dest);
        } else {
            // 파일 이름 변경
            strcpy(basename, dest);
        }
    }

    // 4) 대상 디렉터리 찾기(존재 검사)
    TreeNode *targetDir = NULL;
    if (is_dir_target) {
        if (dirpart[0] == '\0')
            targetDir = dTree->current;             // 현재 디렉터리
        else
            targetDir = search_path(dTree, dirpart, 'd');
        if (!targetDir) {
            printf("mv: target directory '%s' not found\n", dirpart);
            // 원래 자리에 다시 붙여 주는 건 생략
            return;
        }
    }

    // 5) 메모리 트리에 다시 붙이기 & 이름 변경
    if (is_dir_target) {
        // 디렉터리 이동
        if (basename[0]) {
            // foo/newname 형식일 때만 이름 갱신
            strncpy(node->name, basename, MAX_NAME_LENGTH);
            node->name[MAX_NAME_LENGTH-1] = '\0';
        }
        attach_node(targetDir, node);
        printf("mv: '%s' -> '%s/%s'\n",
               src,
               (dirpart[0] ? dirpart : "."),
               node->name);
    } else {
        // 단순 파일 이름 변경: cwd 에 다시 붙임
        TreeNode *cwd = dTree->current;
        strncpy(node->name, basename, MAX_NAME_LENGTH);
        node->name[MAX_NAME_LENGTH-1] = '\0';
        attach_node(cwd, node);
        printf("mv: '%s' renamed to '%s'\n", src, node->name);
    }
}
