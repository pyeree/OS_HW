#pragma once
#include "header.h"

typedef struct {} Stack;

int init_stack(Stack* s);
bool IsFull(Stack* s);
bool IsEmpty(Stack* s);
int push(Stack* s, const char* dir);
char* pop(Stack* s);

int get_pwd(DirectoryTree *dTree);