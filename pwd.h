#pragma once
#include "header.h"
#include <stdbool.h>

#define MAX_PATH_LENGTH 1024

// 스택 타입
typedef struct {
    int   top;
    char* path[MAX_PATH_LENGTH];
} Stack;

// 스택 함수 (pwd.c 에서만 구현)
int     init_stack(Stack* s);
bool    IsEmpty   (Stack* s);
int     push      (Stack* s, const char* dir);
char*   pop       (Stack* s);

// 트리의 current 노드로부터 current_path 를 생성·갱신
void    update_current_path(DirectoryTree *dTree);
