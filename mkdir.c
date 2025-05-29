// mkdir.c

#include "mkdir.h"
#include "header.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>

int Makefile(DirectoryTree* dTree, char* dName, char type, int k) {
    TreeNode* New = malloc(sizeof(TreeNode));
    if (!New) { perror("malloc"); return -1; }

    // 시간 정보 수집
    time_t t; struct tm* now;
    time(&t); now = localtime(&t);

    // 노드 필드 초기화
    New->left         = NULL;
    New->right        = NULL;
    New->children     = NULL;
    New->next_sibling = NULL;
    strncpy(New->name, dName, MAX_NAME_LENGTH);
    New->name[MAX_NAME_LENGTH-1] = '\0';
    New->type   = type;
    New->mode   = (type=='d'? 0755 : 0644);
    New->size   = k;
    New->UID    = getuid();
    New->GID    = getgid();
    New->month  = now->tm_mon + 1;
    New->day    = now->tm_mday;
    New->hour   = now->tm_hour;
    New->minute = now->tm_min;

    // 트리 연결: children / next_sibling 구조
    TreeNode* parent = dTree->current;
    New->parent = parent;
    if (!parent->children) {
        parent->children = New;
    } else {
        TreeNode* tmp = parent->children;
        while (tmp->next_sibling) tmp = tmp->next_sibling;
        tmp->next_sibling = New;
    }

    return 0;
}

static int add_tree_node(DirectoryTree* dTree, const char* dName) {
    // 디렉터리(d) 타입, 기본 크기 4096
    return Makefile(dTree, (char*)dName, 'd', 4096);
}

int my_mkdir(DirectoryTree* dTree, const char* dName, int mode) {
    // 메모리 트리에만 반영 (실제 파일시스템에는 영향 없음)
    if (add_tree_node(dTree, dName) != 0) {
        return -1;
    }
    return 0;
}

typedef struct {
    DirectoryTree* tree;
    char dirname[MAX_NAME_LENGTH];
    int mode;
} MkdirThreadArg;

void* mkdir_thread_worker(void* arg) {
    MkdirThreadArg* data = (MkdirThreadArg*)arg;
    my_mkdir(data->tree, data->dirname, data->mode);
    free(data);
    return NULL;
}

void run_mkdir_multithread(DirectoryTree* tree, const char* arg, int mode) {
    if (!arg || *arg == '\0') {
        printf("mkdir: missing operand\n");
        return;
    }

    // 인자 복사 및 토큰 분리
    char buf[1024];
    strncpy(buf, arg, sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    pthread_t tids[50];
    int count = 0;
    char* tok = strtok(buf, " ");
    while (tok && count < 50) {
        MkdirThreadArg* a = malloc(sizeof(*a));
        if (!a) { perror("malloc"); return; }
        a->tree = tree;
        a->mode = mode;
        strncpy(a->dirname, tok, MAX_NAME_LENGTH);
        a->dirname[MAX_NAME_LENGTH-1] = '\0';

        if (pthread_create(&tids[count], NULL, mkdir_thread_worker, a) == 0) {
            count++;
        } else {
            perror("pthread_create");
            free(a);
        }
        tok = strtok(NULL, " ");
    }

    // 스레드 종료 대기
    for (int i = 0; i < count; i++) {
        pthread_join(tids[i], NULL);
    }
}
