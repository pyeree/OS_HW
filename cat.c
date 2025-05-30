// cat.c

#include "cat.h"
#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define READ_CHUNK 1024

// ───────────────────────────────────────────────────────
// 경로에 해당하는 ‘파일’ 노드만 찾아서 돌려줍니다.
static TreeNode* search_file(DirectoryTree *dTree, const char *path) {
    TreeNode *cur = (path[0]=='/') ? dTree->root : dTree->current;
    char buf[MAX_PATH_LENGTH];
    strncpy(buf, path + (path[0]=='/' ? 1 : 0), sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    for (char *tok = strtok(buf, "/"); tok; tok = strtok(NULL, "/")) {
        TreeNode *child = cur->left;
        while (child && strcmp(child->name, tok) != 0)
            child = child->right;
        if (!child) return NULL;
        cur = child;
    }
    return (cur->type == 'f') ? cur : NULL;
}

// ───────────────────────────────────────────────────────
// 노드를 트리에서 떼어냅니다 (detach)
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
// 가상FS에 새 파일 노드를 생성·붙입니다 (attach)
static void attach_file_node(DirectoryTree *dTree,
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
    n->left    = n->right = NULL;
    n->parent  = NULL;

    TreeNode *cwd = dTree->current;
    if (!cwd->left) {
        cwd->left = n;
        n->parent = cwd;
    } else {
        TreeNode *it = cwd->left;
        while (it->right) it = it->right;
        it->right = n;
        n->parent = cwd;
    }
}

// ───────────────────────────────────────────────────────
// 기존 파일 노드를 메모리 해제
static void free_file_node(TreeNode *node) {
    if (node->content) free(node->content);
    free(node);
}

// ───────────────────────────────────────────────────────
// cat 구현: “> filename” 은 stdin → 메모리, 그 외는 가상FS 상의 content 출력
int cat(DirectoryTree *dTree, char *cmd) {
    char *filename;
    int   mode;   // 0=overwrite, 1=print, 2=print with line#
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
        // ─ stdin 전부 읽어서 메모리에 저장
        int cap = READ_CHUNK;
        char *buf = malloc(cap);
        if (!buf) { perror("malloc"); return 1; }
        int total = 0, r;
        char tmp[READ_CHUNK];
        while ((r = read(STDIN_FILENO, tmp, READ_CHUNK)) > 0) {
            if (total + r > cap) {
                cap = cap * 2 + r;
                buf = realloc(buf, cap);
                if (!buf) { perror("realloc"); return 1; }
            }
            memcpy(buf + total, tmp, r);
            total += r;
        }
        if (r < 0) { perror("read"); free(buf); return 1; }

        // 기존 노드 제거
        TreeNode *old = search_file(dTree, filename);
        if (old) {
            detach_node(old);
            free_file_node(old);
        }
        // 새 노드 추가
        attach_file_node(dTree, filename, buf, total);
        return 0;
    }

    // ─ print modes
    TreeNode *node = search_file(dTree, filename);
    if (!node) {
        printf("cat: %s: No such file\n", filename);
        return 1;
    }
    if (node->size <= 0) return 0;

    int lineno = 1;
    char *p = node->content;
    char *end = node->content + node->size;
    while (p < end) {
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
