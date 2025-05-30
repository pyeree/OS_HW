// mv.c

#include "mv.h"
#include "header.h"
#include <stdio.h>
#include <string.h>

// ───────────────────────────────────────────────────────
// 주어진 path(절대/상대)에서 타입 want_type('f' 또는 'd')의 노드를 찾음
static TreeNode* search_path(DirectoryTree *dTree,
                             const char *path,
                             char want_type)
{
    TreeNode *cur = (path[0]=='/') ? dTree->root : dTree->current;
    // 앞의 '/' 한 글자만 건너뛰고 복사
    char buf[MAX_PATH_LENGTH];
    strncpy(buf, path + (path[0]=='/' ? 1 : 0), sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    // '/' 로 토큰 분리하며 내려가기
    char *tok = strtok(buf, "/");
    while (tok) {
        TreeNode *child = cur->left;  // 첫 자식
        while (child && strcmp(child->name, tok) != 0)
            child = child->right;     // 다음 형제
        if (!child) return NULL;
        cur = child;
        tok = strtok(NULL, "/");
    }
    // 타입 검사 (want_type=='\0' 면 무시)
    if (want_type == '\0' || cur->type == want_type)
        return cur;
    return NULL;
}

// ───────────────────────────────────────────────────────
// 트리에서 node를 떼어냄 (detach)
static void detach_node(TreeNode *node) {
    TreeNode *p = node->parent;
    if (!p) return;
    // 부모의 첫 자식이면
    if (p->left == node) {
        p->left = node->right;
    } else {
        // 형제 체인에서 찾아 제거
        TreeNode *it = p->left;
        while (it->right && it->right != node)
            it = it->right;
        if (it->right)
            it->right = node->right;
    }
    node->parent = NULL;
    node->right  = NULL;
}

// ───────────────────────────────────────────────────────
// 트리에 node를 붙임 (attach) — 마지막 자식으로
static void attach_node(TreeNode *parent, TreeNode *node) {
    node->parent = parent;
    node->right  = NULL;
    if (!parent->left) {
        parent->left = node;
    } else {
        TreeNode *it = parent->left;
        while (it->right)
            it = it->right;
        it->right = node;
    }
}

// ───────────────────────────────────────────────────────
// mv 구현: 실제 디스크는 전혀 건드리지 않고,
// 오직 메모리 트리만 바꿉니다.
void mv(DirectoryTree *dTree, char *src, char *dest) {
    // 1) 메모리 트리에서 src 노드 찾기
    TreeNode *node = search_path(dTree, src, 'f');
    if (!node) node = search_path(dTree, src, 'd');
    if (!node) {
        printf("mv: cannot stat '%s': No such file or directory\n", src);
        return;
    }

    // 2) 트리에서 분리
    detach_node(node);

    // 3) dest 파싱: 슬래시가 있으면 디렉터리 이동+이름 변경,
    //    없으면 existing dir vs 새 이름 변경
    int  is_dir_target = 0;
    char dirpart[MAX_PATH_LENGTH]  = "";
    char basename[MAX_NAME_LENGTH] = "";
    char *slash = strrchr(dest, '/');
    if (slash) {
        // "aa/hello1.txt" 꼴
        size_t dlen = slash - dest;
        strncpy(dirpart, dest, dlen);
        dirpart[dlen]   = '\0';
        strcpy(basename, slash + 1);
        is_dir_target = 1;
    } else {
        // "mv x existingDir" 꼴인지?
        TreeNode *td = search_path(dTree, dest, 'd');
        if (td) {
            is_dir_target = 1;
            strcpy(dirpart, dest);
        } else {
            // 단순 이름 변경
            strcpy(basename, dest);
        }
    }

    // 4) 대상 디렉터리 찾기
    TreeNode *targetDir = NULL;
    if (is_dir_target) {
        if (dirpart[0] == '\0')
            targetDir = dTree->current;
        else
            targetDir = search_path(dTree, dirpart, 'd');
        if (!targetDir) {
            printf("mv: target directory '%s' not found\n", dirpart);
            // 실패하면 원래 자리에 다시 붙이는 로직을 넣어도 됩니다.
            return;
        }
    }

    // 5) 메모리 트리에 재붙이고 이름 갱신
    if (is_dir_target) {
        // 디렉터리 이동 + (basename이 있으면) 이름 변경
        if (basename[0]) {
            strncpy(node->name, basename, MAX_NAME_LENGTH);
            node->name[MAX_NAME_LENGTH-1] = '\0';
        }
        attach_node(targetDir, node);
        printf("mv: '%s' -> '%s/%s'\n",
               src,
               (dirpart[0] ? dirpart : "."),
               node->name);
    } else {
        // 단순 이름 변경: current 디렉터리에 붙임
        strncpy(node->name, basename, MAX_NAME_LENGTH);
        node->name[MAX_NAME_LENGTH-1] = '\0';
        attach_node(dTree->current, node);
        printf("mv: '%s' renamed to '%s'\n", src, node->name);
    }
}
