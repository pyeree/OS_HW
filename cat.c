// cat.c

#include "cat.h"
#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CHUNK 1024

// ───────────────────────────────────────────────────────
// 1) 절대/상대 경로에서 파일 노드 검색
static TreeNode* search_path(DirectoryTree *dTree, const char *path) {
    TreeNode *cur = (path[0]=='/') ? dTree->root : dTree->current;
    char buf[MAX_PATH_LENGTH];
    strncpy(buf, path + (path[0]=='/'?1:0), sizeof(buf));
    buf[sizeof(buf)-1] = '\0';
    for (char *tok = strtok(buf, "/"); tok; tok = strtok(NULL, "/")) {
        TreeNode *c = cur->left;
        while (c && strcmp(c->name, tok) != 0) c = c->right;
        if (!c) return NULL;
        cur = c;
    }
    return (cur->type=='f') ? cur : NULL;
}

// ───────────────────────────────────────────────────────
// 2) 트리에서 node 분리(detach)
static void detach_node(TreeNode *node) {
    TreeNode *p = node->parent;
    if (!p) return;
    if (p->left == node) {
        p->left = node->right;
    } else {
        TreeNode *it = p->left;
        while (it->right && it->right != node) it = it->right;
        if (it->right) it->right = node->right;
    }
    node->parent = NULL;
    node->right  = NULL;
}

// ───────────────────────────────────────────────────────
// 3) 트리에 새 파일 노드 추가 (마지막 자식으로)
static void add_file_node(DirectoryTree *dTree,
                          const char *name,
                          char *content,
                          int size) {
    TreeNode *n = malloc(sizeof(TreeNode));
    if (!n) { perror("malloc"); return; }
    strncpy(n->name, name, MAX_NAME_LENGTH);
    n->name[MAX_NAME_LENGTH-1] = '\0';
    n->type    = 'f';
    n->mode    = 0644;
    n->size    = size;
    n->content = content;
    n->left = NULL;
    n->right = NULL;
    n->parent = NULL;
    // attach
    TreeNode *cur = dTree->current;
    if (!cur->left) {
        cur->left = n;
        n->parent = cur;
    } else {
        TreeNode *it = cur->left;
        while (it->right) it = it->right;
        it->right = n;
        n->parent = cur;
    }
}

// ───────────────────────────────────────────────────────
// 4) 기존 파일 노드 free
static void free_file_node(TreeNode *node) {
    if (node->content) free(node->content);
    free(node);
}

// ───────────────────────────────────────────────────────
// 5) cat 명령 전체 구현
int cat(DirectoryTree *dTree, char *cmd) {
    char *filename;
    int   mode;  // 0 = create/overwrite, 1 = print, 2 = print with line#, 
    if (strcmp(cmd, ">") == 0) {
        mode = 0;
        filename = strtok(NULL, " ");
    } else if (strcmp(cmd, "-n") == 0) {
        mode = 2;
        filename = strtok(NULL, " ");
    } else {
        mode = 1;
        filename = cmd;
    }
    if (!filename) {
        printf("cat: missing file operand\n");
        return 1;
    }

    if (mode == 0) {
        // ─ overwrite: stdin → 메모리
        int cap = CHUNK;
        char *buf = malloc(cap);
        if (!buf) { perror("malloc"); return 1; }
        int total = 0, r;
        char tmp[CHUNK];
        while ((r = read(STDIN_FILENO, tmp, CHUNK)) > 0) {
            if (total + r > cap) {
                cap = cap * 2 + r;
                buf = realloc(buf, cap);
                if (!buf) { perror("realloc"); return 1; }
            }
            memcpy(buf + total, tmp, r);
            total += r;
        }
        if (r < 0) { perror("read"); free(buf); return 1; }
        // 기존 파일 노드 제거
        TreeNode *old = search_path(dTree, filename);
        if (old) {
            detach_node(old);
            free_file_node(old);
        }
        // 새 노드 추가
        add_file_node(dTree, filename, buf, total);
        return 0;
    }

    // ─ display
    TreeNode *node = search_path(dTree, filename);
    if (!node) {
        printf("cat: %s: No such file\n", filename);
        return 1;
    }
    if (!node->content || node->size == 0) return 0;

    int lineno = 1;
    char *p = node->content;
    char *end = p + node->size;
    while (p < end) {
        // 한 줄 분리
        char *nl = memchr(p, '\n', end - p);
        int len = nl ? (nl - p + 1) : (end - p);
        if (mode == 2) {
            printf("%6d  ", lineno++);
        }
        fwrite(p, 1, len, stdout);
        p += len;
    }
    return 0;
}
