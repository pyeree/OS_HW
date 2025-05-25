#pragma once
#include "header.h"
#include <stdbool.h>

#define MAX_PATH_LENGTH 1024

typedef struct {
    char* path[MAX_PATH_LENGTH];
    int   top;
} Stack;

// 스택 헬퍼
int    init_stack       (Stack* s);
bool   IsEmpty          (Stack* s);
int    push             (Stack* s, const char* dir);
char*  pop              (Stack* s);

// dTree->current 를 기준으로 dTree->current_path 에
// 절대경로 문자열을 만들어 저장
void   update_current_path(DirectoryTree *dTree);
