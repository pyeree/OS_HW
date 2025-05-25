#include "pwd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void update_current_path(DirectoryTree *dTree) {
    Stack buff;
    init_stack(&buff);

    // 1) 현재 노드에서 루트까지 올라가며 이름을 스택에 쌓고
    TreeNode *cur = dTree->current;
    while (cur->parent) {
        push(&buff, cur->name);
        cur = cur->parent;
    }

    // 2) 스택에서 꺼내면서 "/"로 이어 붙이기
    char path[MAX_PATH_LENGTH] = "";
    if (IsEmpty(&buff)) {
        // 루트에 있다면 그냥 "/"
        strcpy(path, "/");
    } else {
        while (!IsEmpty(&buff)) {
            char *name = pop(&buff);
            strcat(path, "/");
            strcat(path, name);
            free(name);
        }
    }

    // 3) 최종 결과를 dTree->current_path 에 복사
    strncpy(dTree->current_path, path, sizeof(dTree->current_path));
}
