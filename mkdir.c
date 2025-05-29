// mkdir.c
#include "mkdir.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>

typedef struct {
    DirectoryTree* tree;
    char dirname[MAX_NAME_LENGTH];
    int mode;
} MkdirThreadArg;

/**
 * 스레드 워커: my_mkdir을 호출하여 트리에 노드를 추가하고 실제 디렉토리도 생성
 */
void* mkdir_thread_worker(void* arg) {
    MkdirThreadArg* data = (MkdirThreadArg*)arg;
    my_mkdir(data->tree, data->dirname, data->mode);  // mkdir.h의 my_mkdir 사용:contentReference[oaicite:0]{index=0}
    free(data);
    return NULL;
}

/**
 * 공백으로 구분된 여러 디렉터리를 멀티스레드로 생성
 * mode: 퍼미션 비트 (예: 0755)
 */
void run_mkdir_multithread(DirectoryTree* tree, const char* arg, int mode) {
    if (!arg || *arg == '\0') {
        printf("mkdir: missing operand\n");
        return;
    }

    char temp[1024];
    strncpy(temp, arg, sizeof(temp));
    temp[sizeof(temp)-1] = '\0';

    pthread_t tids[50];
    int count = 0;
    char* token = strtok(temp, " ");
    while (token && count < 50) {
        MkdirThreadArg* tArg = malloc(sizeof(MkdirThreadArg));
        if (!tArg) {
            perror("malloc");
            break;
        }
        tArg->tree = tree;
        tArg->mode = mode;
        strncpy(tArg->dirname, token, MAX_NAME_LENGTH);
        tArg->dirname[MAX_NAME_LENGTH-1] = '\0';

        if (pthread_create(&tids[count], NULL, mkdir_thread_worker, tArg) != 0) {
            perror("pthread_create");
            free(tArg);
        } else {
            count++;
        }
        token = strtok(NULL, " ");
    }

    for (int i = 0; i < count; i++) {
        pthread_join(tids[i], NULL);
    }
}
