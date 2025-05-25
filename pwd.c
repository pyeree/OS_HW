#include "pwd.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    int top;
    char* items[MAX_PATH_LENGTH];
} Stack;

static void init_stack(Stack *s)      { s->top = -1; }
static bool is_empty(Stack *s)         { return s->top < 0; }
static void push(Stack *s, char *str)  { s->items[++s->top] = strdup(str); }
static char* pop_(Stack *s)            { return is_empty(s) ? NULL : s->items[s->top--]; }

void update_current_path(DirectoryTree *dTree) {
    Stack st; init_stack(&st);
    TreeNode *cur = dTree->current;
    while (cur->parent) {
        push(&st, cur->name);
        cur = cur->parent;
    }

    char buf[MAX_PATH_LENGTH] = "";
    if (is_empty(&st)) {
        strcpy(buf, "/");
    } else {
        while (!is_empty(&st)) {
            char *seg = pop_(&st);
            strcat(buf, "/");
            strcat(buf, seg);
            free(seg);
        }
    }
    strncpy(dTree->current_path, buf, MAX_PATH_LENGTH);
}

int get_pwd(DirectoryTree *dTree) {
    update_current_path(dTree);
    printf("%s\n", dTree->current_path);
    return 0;
}
