#include "header.h"
#include "pwd.h"
#include <stdbool.h>  // bool 타입 사용을 위해

typedef struct {
    int top;
    char* path[MAXB];
} Stack;

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
    
    // 루트 디렉터리까지 올라갈 때까지 반복
    while (current->parent != NULL) {
        push(&buff, current->name);
        current = current->parent;
    }

    // 루트 경로인 "/" 출력
    if (IsEmpty(&buff)) {
        printf("/\n");
    } else {
        // 스택에 저장된 경로를 출력 (루트부터 현재까지)
        char* name;
        while ((name = pop(&buff)) != NULL) {
            printf("/%s", name);
            free(name);
        }
        printf("\n");
    }

    return 0;
}

int main() {
    DirectoryTree dTree;
    load_tree_from_file(&dTree, "filesystem.txt");  // 파일에서 트리 로드
    
    return get_pwd(&dTree);  // 트리에서 현재 경로 출력
}