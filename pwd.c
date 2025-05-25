#include "header.h"
#include "pwd.h"
#include "tree_io.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int init_stack(Stack* s) {
    if (s == NULL) {
        fprintf(stderr, "Stack Buffer doesn't exist.\n");
        return -1;
    }
    s->top = -1;
    return 0;
}

bool IsFull(Stack* s) {
    return s->top == MAXB - 1;
}

bool IsEmpty(Stack* s) {
    return s->top == -1;
}

int push(Stack* s, const char* dir) {
    if (IsFull(s)) {
        fprintf(stderr, "Stack is Full.\n");
        return -1;
    }
    s->path[++(s->top)] = strdup(dir);
    return 0;
}

char* pop(Stack* s) {
    if (IsEmpty(s)) {
        return NULL;
    }
    return s->path[(s->top)--];
}

int get_pwd(DirectoryTree *dTree) {
    Stack buff;
    init_stack(&buff);

    TreeNode *current = dTree->current;

    // 루트까지 올라가면서 경로 스택에 저장
    while (current->parent != NULL) {
        push(&buff, current->name);
        current = current->parent;
    }

    // 문자열 경로 조합
    char path_str[MAX_PATH_LENGTH] = "/";
    while (!IsEmpty(&buff)) {
        char *name = pop(&buff);
        strcat(path_str, name);
        strcat(path_str, "/");
        free(name);
    }

    // 마지막 슬래시 제거 (루트 제외)
    if (strlen(path_str) > 1 && path_str[strlen(path_str) - 1] == '/') {
        path_str[strlen(path_str) - 1] = '\0';
    }

    // 경로 출력
    printf("%s\n", path_str);

    // 프롬프트에 반영될 current_path 갱신
    strncpy(dTree->current_path, path_str, MAX_PATH_LENGTH);

    return 0;
}
