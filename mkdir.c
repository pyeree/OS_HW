#include "mkdir.h"
#include "header.h"
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

int Makefile(DirectoryTree* dTree, char* dName, char type, int k) {
    TreeNode* New = malloc(sizeof(TreeNode));
    if (!New) { perror("malloc"); return -1; }
    time_t t; struct tm *now;
    time(&t); now = localtime(&t);

    New->left  = NULL;
    New->right = NULL;
    strncpy(New->name, dName, MAX_NAME_LENGTH);
    New->type  = type;
    New->mode  = (type=='d'?0755:0644);
    New->size  = k;
    New->UID   = getuid();
    New->GID   = getgid();
    New->month = now->tm_mon + 1;
    New->day   = now->tm_mday;
    New->hour  = now->tm_hour;
    New->minute= now->tm_min;
    New->parent= dTree->current;

    if (!dTree->current->left) {
        dTree->current->left = New;
    } else {
        TreeNode* tmp = dTree->current->left;
        while (tmp->right) tmp = tmp->right;
        tmp->right = New;
    }
    return 0;
}

// ── 파일/디렉터리 트리에 새 노드를 추가하는 내부 헬퍼
//    (header.h 에 선언된 Makefile 함수 사용)
static int add_tree_node(DirectoryTree* dTree, const char* dName) {
    // 'd' 타입, 4096 기본 크기
    return Makefile(dTree, (char*)dName, 'd', 4096);
}

/**
 * @brief 실제 디렉터리 생성 + 트리에 반영
 */
int my_mkdir(DirectoryTree* dTree, const char* dName, int mode) {
    // 1) 트리에 노드 추가
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

/**
 * @brief 스레드 워커: my_mkdir 호출
 */
void* mkdir_thread_worker(void* arg) {
    MkdirThreadArg* data = (MkdirThreadArg*)arg;
    my_mkdir(data->tree, data->dirname, data->mode);
    free(data);
    return NULL;
}

/**
 * @brief 공백 구분된 여러 디렉터리를 멀티스레드로 생성
 */
void run_mkdir_multithread(DirectoryTree* tree, const char* arg, int mode) {
    if (!arg || *arg=='\0') {
        printf("mkdir: missing operand\n");
        return;
    }

    char buf[1024];
    strncpy(buf, arg, sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    pthread_t tids[50];
    int count = 0;
    char* tok = strtok(buf, " ");

    while (tok && count < 50) {
        MkdirThreadArg* a = malloc(sizeof(*a));
        a->tree = tree;
        a->mode = mode;
        strncpy(a->dirname, tok, MAX_NAME_LENGTH);
        a->dirname[MAX_NAME_LENGTH-1] = '\0';

        if (pthread_create(&tids[count], NULL, mkdir_thread_worker, a)==0) {
            count++;
        } else {
            perror("pthread_create");
            free(a);
        }
        tok = strtok(NULL, " ");
    }
    for (int i = 0; i < count; i++) {
        pthread_join(tids[i], NULL);
    }
}
