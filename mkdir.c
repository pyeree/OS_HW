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
#define BUF_SIZE  1024

// ————————————————————————————————————————————————
// 실제 파일시스템에도 mkdir(2) 호출, 그다음 in-memory 트리에 노드 추가
int my_mkdir(DirectoryTree* dTree, const char* dirname, int mode) {
    // 1) 실제 디스크에 mkdir
    if (mkdir(dirname, mode) != 0 && errno != EEXIST) {
        perror("mkdir");
        return -1;
    }

    // 2) 메모리 트리에 노드 추가
    TreeNode* New = malloc(sizeof(TreeNode));
    if (!New) { perror("malloc"); return -1; }

    time_t t; struct tm* now;
    time(&t); now = localtime(&t);

    New->left   = NULL;
    New->right  = NULL;
    strncpy(New->name, dirname, MAX_NAME_LENGTH);
    New->name[MAX_NAME_LENGTH-1] = '\0';
    New->type   = 'd';
    New->mode   = mode;      // 여기에 0700 or 0755 이 저장됩니다
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
        TreeNode* t = dTree->current->left;
        while (t->right) t = t->right;
        t->right = New;
    }
    return 0;
}

// ————————————————————————————————————————————————
// “mkdir -p a/b/c” 처리용 (상위 디렉터리까지 순차 생성)
static int mkdir_parents(DirectoryTree* tree, const char* path, int mode) {
    TreeNode* save = tree->current;
    char buf[BUF_SIZE];
    strncpy(buf, path, sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    char* tok = strtok(buf, "/");
    while (tok) {
        // 이미 있으면 내려가고, 없으면 my_mkdir
        TreeNode* child = tree->current->left;
        while (child && !(child->type=='d' && strcmp(child->name,tok)==0))
            child = child->right;
        if (!child) {
            if (my_mkdir(tree, tok, mode) != 0) {
                tree->current = save;
                return -1;
            }
            // 방금 추가된 노드로 이동
            child = tree->current->left;
            while (child->right) child = child->right;
        }
        tree->current = child;
        tok = strtok(NULL, "/");
    }

    tree->current = save;
    return 0;
}

// ————————————————————————————————————————————————
// 스레드 워커: my_mkdir 호출
static void* mkdir_thread_worker(void* arg) {
    MkdirThreadArg* a = arg;
    my_mkdir(a->tree, a->dirname, a->mode);
    free(a);
    return NULL;
}

// ————————————————————————————————————————————————
// 전체 “mkdir” 명령 처리: 옵션 -p, -m <mode> 파싱 후 수행
void run_mkdir_multithread(DirectoryTree* tree,
                           const char* arg,
                           int default_mode)
{
    if (!arg || !*arg) {
        printf("mkdir: missing operand\n");
        return;
    }

    // 1) 토큰화
    char buf[BUF_SIZE];
    strncpy(buf, arg, sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    char* tokens[MAX_TOKENS] = {0};
    int   ntok   = 0;
    int   pflag  = 0;
    int   mode   = default_mode;
    char* tk     = strtok(buf, " ");
    while (tk && ntok < MAX_TOKENS) {
        if (strcmp(tk, "-p") == 0) {
            pflag = 1;
        }
        else if (strcmp(tk, "-m") == 0) {
            char* mstr = strtok(NULL, " ");
            if (mstr) {
                // **꼭 8진수로** 해석해야 올바른 비트가 들어갑니다!
                mode = strtol(mstr, NULL, 8);
            }
        }
        else {
            tokens[ntok++] = tk;
        }
        tk = strtok(NULL, " ");
    }

    // 2) -p 없으면 슬래시 포함 경로는 에러
    for (int i = 0; i < ntok; i++) {
        if (!pflag && tokens[i] && strchr(tokens[i], '/')) {
            printf("mkdir: cannot create directory '%s': No such file or directory\n", tokens[i]);
            tokens[i] = NULL;
        }
    }

    // 3) -p 처리 (동기)
    if (pflag) {
        for (int i = 0; i < ntok; i++) {
            if (tokens[i]) {
                if (mkdir_parents(tree, tokens[i], mode) != 0)
                    printf("mkdir: failed to create '%s'\n", tokens[i]);
            }
        }
        return;
    }

    // 4) 멀티스레드 생성
    pthread_t threads[MAX_TOKENS];
    int       thcount = 0;
    for (int i = 0; i < ntok; i++) {
        if (!tokens[i]) continue;
        MkdirThreadArg* a = malloc(sizeof(*a));
        a->tree    = tree;
        a->mode    = mode;
        strncpy(a->dirname, tokens[i], MAX_NAME_LENGTH);
        a->dirname[MAX_NAME_LENGTH-1] = '\0';

        if (pthread_create(&threads[thcount], NULL, mkdir_thread_worker, a) == 0)
            thcount++;
        else {
            perror("pthread_create");
            free(a);
        }
    }
    // 5) join
    for (int i = 0; i < thcount; i++)
        pthread_join(threads[i], NULL);
}
