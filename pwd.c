#include "pwd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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

    char path_str[MAX_PATH_LENGTH] = "/";
    while (!IsEmpty(&buff)) {
        char *name = pop(&buff);
        strcat(path_str, name);
        strcat(path_str, "/");
        free(name);
    }

    if (strlen(path_str) > 1 && path_str[strlen(path_str) - 1] == '/')
        path_str[strlen(path_str) - 1] = '\0';

    printf("%s\n", path_str);
    strncpy(dTree->current_path, path_str, MAX_PATH_LENGTH);

    return 0;
}
