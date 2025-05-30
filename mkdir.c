// mkdir.c
#include "mkdir.h"
#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define MAX_TOKENS 50
#define BUF_SIZE   1024

// ── memory‐only 트리에 디렉터리 노드 추가
int my_mkdir(DirectoryTree* dTree, const char* dirname, int mode) {
    // 1) OS 파일시스템에도 실제 생성 (optional)
    if (mkdir(dirname, mode) != 0 && errno != EEXIST) {
        perror("mkdir");
        return -1;
    }

    // 2) in‐memory 트리에도 추가
    TreeNode* New = malloc(sizeof(TreeNode));
    if (!New) { perror("malloc"); return -1; }
    time_t t; struct tm* now;
    time(&t); now = localtime(&t);

    New->left   = NULL;
    New->right  = NULL;
    strncpy(New->name, dirname, MAX_NAME_LENGTH);
    New->name[MAX_NAME_LENGTH-1] = '\0';
    New->type   = 'd';
    New->mode   = mode;
    New->size   = 4096;
    New->UID    = getuid();
    New->GID    = getgid();
    New->month  = now->tm_mon + 1;
    New->day    = now->tm_mday;
    New->hour   = now->tm_hour;
    New->minute = now->tm_min;
    New->parent = dTree->current;

    if (!dTree->current->left) {
        dTree->current->left = New;
    } else {
        TreeNode* tmp = dTree->current->left;
        while (tmp->right) tmp = tmp->right;
        tmp->right = New;
    }
    return 0;
}

// ── -p 옵션용: "a/b/c" 경로상의 모든 디렉터리 순차 생성
static int mkdir_parents(DirectoryTree* tree, const char* path, int mode) {
    TreeNode* orig = tree->current;
    char buf[BUF_SIZE];
    strncpy(buf, path, sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    char* tok = strtok(buf, "/");
    while (tok) {
        // in‐memory: 이미 있으면 건너뛰고, 없으면 my_mkdir
        TreeNode* child = tree->current->left;
        while (child && !(child->type=='d' && strcmp(child->name, tok)==0))
            child = child->right;
        if (!child) {
            if (my_mkdir(tree, tok, mode) != 0) {
                tree->current = orig;
                return -1;
            }
            // 방금 추가된 노드로 이동
            child = tree->current->left;
            while (child->right) child = child->right;
        }
        tree->current = child;
        tok = strtok(NULL, "/");
    }

    tree->current = orig;
    return 0;
}

static void* mkdir_thread_worker(void* arg) {
    MkdirThreadArg* a = (MkdirThreadArg*)arg;
    my_mkdir(a->tree, a->dirname, a->mode);
    free(a);
    return NULL;
}

void run_mkdir_multithread(DirectoryTree* tree, const char* arg, int default_mode) {
    if (!arg || !*arg) {
        printf("mkdir: missing operand\n");
        return;
    }

    // 1) 토큰별 분리
    char buf[BUF_SIZE];
    strncpy(buf, arg, sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    char* tokens[MAX_TOKENS] = {0};
    int   ntok  = 0;
    int   pflag = 0;
    int   mode  = default_mode;

    char* tok = strtok(buf, " ");
    while (tok && ntok < MAX_TOKENS) {
        if (strcmp(tok, "-p") == 0) {
            pflag = 1;
        }
        else if (strcmp(tok, "-m") == 0) {
            char* mstr = strtok(NULL, " ");
            if (mstr) mode = strtol(mstr, NULL, 8);
        }
        else {
            tokens[ntok++] = tok;
        }
        tok = strtok(NULL, " ");
    }

    // 2) -p 없을 때 "/" 포함 경로는 에러
    for (int i = 0; i < ntok; i++) {
        if (!pflag && tokens[i] && strchr(tokens[i], '/')) {
            printf("mkdir: cannot create directory '%s': No such file or directory\n", tokens[i]);
            tokens[i] = NULL;
        }
    }

    // 3) -p 옵션 처리 (단계별 생성)
    if (pflag) {
        for (int i = 0; i < ntok; i++) {
            if (tokens[i]) {
                if (mkdir_parents(tree, tokens[i], mode) != 0)
                    printf("mkdir: failed to create '%s'\n", tokens[i]);
            }
        }
        return;
    }

    // 4) 그 외엔 멀티스레드로 개별 생성
    pthread_t tids[MAX_TOKENS];
    int thcount = 0;
    for (int i = 0; i < ntok; i++) {
        if (!tokens[i]) continue;
        MkdirThreadArg* a = malloc(sizeof(*a));
        a->tree = tree;
        a->mode = mode;
        strncpy(a->dirname, tokens[i], MAX_NAME_LENGTH);
        a->dirname[MAX_NAME_LENGTH-1] = '\0';
        if (pthread_create(&tids[thcount], NULL, mkdir_thread_worker, a) == 0)
            thcount++;
        else {
            perror("pthread_create");
            free(a);
        }
    }
    for (int i = 0; i < thcount; i++)
        pthread_join(tids[i], NULL);
}
