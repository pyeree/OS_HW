#pragma once
#include "header.h"
#include <stdbool.h>


#define MAX_PATH_LENGTH 1024

// 스택 타입은 헤더에 한 번만 선언
typedef struct {
    int   top;
    char* path[MAX_PATH_LENGTH];
} Stack;

// 스택 함수 원형
int    init_stack(Stack* s);
int    is_empty(Stack* s);
int    push(Stack* s, const char* dir);
char*  pop(Stack* s);
