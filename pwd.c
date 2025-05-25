#include "pwd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 스택 초기화
int init_stack(Stack* s) {
    if (!s) return -1;
    s->top = -1;
    return 0;
}

// 스택이 비었는지
int is_empty(Stack* s) {
    return s->top < 0;
}

// 스택에 push
int push(Stack* s, const char* dir) {
    if (s->top + 1 >= MAX_PATH_LENGTH) return -1;
    s->path[++s->top] = strdup(dir);
    return 0;
}

// 스택에서 pop
char* pop(Stack* s) {
    if (is_empty(s)) return NULL;
    return s->path[s->top--];
}

// 현재 디렉토리 기준으로 절대경로 문자열 갱신
void update_current_path(DirectoryTree *dTree) {
    Stack st;
    init_stack(&st);
    TreeNode *cur = dTree->current;

    // 부모 노드를 거슬러 올라가며 스택에 저장
    while (cur->parent) {
        push(&st, cur->name);
        cur = cur->parent;
    }

    // 스택에 아무것도 없으면 루트
    char buf[MAX_PATH_LENGTH] = "";
    if (is_empty(&st)) {
        strcpy(buf, "/");
    } else {
        // 스택에서 꺼내면서 경로 조립
        char *seg;
        while ((seg = pop(&st)) != NULL) {
            strcat(buf, "/");
            strcat(buf, seg);
            free(seg);
        }
    }

    // current_path에 복사
    strncpy(dTree->current_path, buf, MAX_PATH_LENGTH);
}

// pwd 명령: 경로 갱신 및 출력
int get_pwd(DirectoryTree *dTree) {
    update_current_path(dTree);
    printf("%s\n", dTree->current_path);
    return 0;
}
