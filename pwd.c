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

int get_pwd(DirectoryTree *dTree) {
    Stack buff;
    init_stack(&buff);

    TreeNode *current = dTree->current;
    while (current->parent != NULL) {
        push(&buff, current->name);
        current = current->parent;
    }

    // 루트부터 역순으로 쌓아올려 출력
    if (IsEmpty(&buff)) {
        printf("/\n");
    } else {
        char *name;
        while ((name = pop(&buff)) != NULL) {
            printf("/%s", name);
            free(name);
        }
        printf("\n");
    }
    return 0;
}
