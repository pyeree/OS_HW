#include "pwd.h"
#include <stdlib.h>
#include <string.h>

#define MAX_DEPTH 1024

// 간단한 스택 구현
typedef struct {
    char *items[MAX_DEPTH];
    int   top;
} Stack;

static void init_stack(Stack *s) {
    s->top = -1;
}
static int push(Stack *s, const char *name) {
    if (s->top + 1 >= MAX_DEPTH) return -1;
    s->items[++s->top] = strdup(name);
    return 0;
}
static char *pop(Stack *s) {
    if (s->top < 0) return NULL;
    return s->items[s->top--];
}

void update_current_path(DirectoryTree *dTree) {
    Stack stk;
    init_stack(&stk);

    // 1) current 에서 루트까지 올라가며 이름을 스택에 쌓음
    TreeNode *cur = dTree->current;
    while (cur->parent) {
        push(&stk, cur->name);
        cur = cur->parent;
    }

    // 2) 스택에서 꺼내면서 "/"로 이어 붙이기
    char path[MAX_PATH_LENGTH] = "";
    if (stk.top < 0) {
        // 루트 디렉토리
        strcpy(path, "/");
    } else {
        char *seg;
        while ((seg = pop(&stk)) != NULL) {
            strcat(path, "/");
            strcat(path, seg);
            free(seg);
        }
    }

    // 3) current_path 에 복사
    strncpy(dTree->current_path, path, sizeof(dTree->current_path)-1);
    dTree->current_path[sizeof(dTree->current_path)-1] = '\0';
}
