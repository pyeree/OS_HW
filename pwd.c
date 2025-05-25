#include "pwd.h"
#include <stdlib.h>
#include <string.h>

// Stack 함수 구현
int init_stack(Stack* s) {
    s->top = -1;
    return 0;
}
bool IsEmpty(Stack* s) {
    return s->top == -1;
}
int push(Stack* s, const char* dir) {
    s->path[++(s->top)] = strdup(dir);
    return 0;
}
char* pop(Stack* s) {
    if (IsEmpty(s)) return NULL;
    return s->path[(s->top)--];
}

// current_path 갱신 로직
void update_current_path(DirectoryTree *dTree) {
    Stack stk;
    init_stack(&stk);

    // 1) current 에서 root 까지 이름 쌓기
    TreeNode *cur = dTree->current;
    while (cur->parent) {
        push(&stk, cur->name);
        cur = cur->parent;
    }

    // 2) 스택에서 꺼내며 "/" 붙이며 합치기
    char path[MAX_PATH_LENGTH] = "";
    if (IsEmpty(&stk)) {
        // root 디렉토리
        strcpy(path, "/");
    } else {
        char *seg;
        while ((seg = pop(&stk)) != NULL) {
            strcat(path, "/");
            strcat(path, seg);
            free(seg);
        }
    }

    // 3) dTree->current_path 에 복사
    strncpy(dTree->current_path, path, sizeof(dTree->current_path));
    dTree->current_path[sizeof(dTree->current_path)-1] = '\0';
}
