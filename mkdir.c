// mkdir.c
#include "mkdir.h"
#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#define MAX_TOKENS  50
#define BUF_SIZE   1024

// ── 트리에 단일 디렉터리 노드 추가 (left/right 방식)
int my_mkdir(DirectoryTree* dTree, const char* dirname, int mode) {
    TreeNode* New = malloc(sizeof(TreeNode));
    if (!New) { perror("malloc"); return -1; }
    time_t t; struct tm* now;
    time(&t); now = localtime(&t);

    // 필드 초기화
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

    // 트리 연결: left = first child, right = next sibling
    if (!dTree->current->left) {
        dTree->current->left = New;
    } else {
        TreeNode* tmp = dTree->current->left;
        while (tmp->right) tmp = tmp->right;
        tmp->right = New;
    }
    return 0;
}

// ── -p 옵션용: 경로("a/b/c") 전체를 순차 생성
static int mkdir_parents(DirectoryTree* tree, const char* path, int mode) {
    // 원위치 저장
    TreeNode* orig = tree->current;
    char buf[BUF_SIZE];
    strncpy(buf, path, sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    char* tok = strtok(buf, "/");
    while (tok) {
        // 이미 존재하는지 확인
        TreeNode* child = tree->current->left;
        while (child && !(child->type=='d' && strcmp(child->name, tok)==0)) {
            child = child->right;
        }
        if (!child) {
            if (my_mkdir(tree, tok, mode) != 0) {
                tree->current = orig;
                return -1;
            }
            // 새로 추가된 노드로 이동
            child = tree->current->left;
            while (child->right) child = child->right;
        }
        tree->current = child;
        tok = strtok(NULL, "/");
    }

    // 원위치 복원
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

    // 토큰화
    char buf[BUF_SIZE];
    strncpy(buf, arg, sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    char* tokens[MAX_TOKENS] = {0};
    int   ntok = 0;
    int   pflag = 0;
    int   mode  = default_mode;

    char* tok = strtok(buf, " ");
    while (tok && ntok < MAX_TOKENS) {
        if (strcmp(tok, "-p") == 0) {
            pflag = 1;
        }
        else if (strcmp(tok, "-m") == 0) {
            char* mstr = strtok(NULL, " ");
            if (mstr) {
                // 8진수로 파싱: ex) "700"
                mode = strtol(mstr, NULL, 8);
            }
        }
        else {
            tokens[ntok++] = tok;
        }
        tok = strtok(NULL, " ");
    }

    // 부모 옵션 없이 슬래시 경로 경고
    for (int i = 0; i < ntok; i++) {
        if (!pflag && strchr(tokens[i], '/')) {
            printf("mkdir: cannot create directory '%s': No such file or directory\n", tokens[i]);
            tokens[i] = NULL;
        }
    }

    // -p 처리 (동기적)
    if (pflag) {
        for (int i = 0; i < ntok; i++) {
            if (!tokens[i]) continue;
            if (mkdir_parents(tree, tokens[i], mode) != 0) {
                printf("mkdir: failed to create '%s'\n", tokens[i]);
            }
        }
        return;
    }

    // -p 없으면 멀티스레드로 개별 생성
    pthread_t tids[MAX_TOKENS];
    int thcount = 0;
    for (int i = 0; i < ntok; i++) {
        if (!tokens[i]) continue;
        MkdirThreadArg* a = malloc(sizeof(*a));
        a->tree = tree;
        a->mode = mode;
        strncpy(a->dirname, tokens[i], MAX_NAME_LENGTH);
        a->dirname[MAX_NAME_LENGTH-1] = '\0';

        if (pthread_create(&tids[thcount], NULL, mkdir_thread_worker, a) == 0) {
            thcount++;
        } else {
            perror("pthread_create");
            free(a);
        }
    }
    // 스레드 합류
    for (int i = 0; i < thcount; i++) {
        pthread_join(tids[i], NULL);
    }
}
