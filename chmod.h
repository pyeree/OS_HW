//chmod.h
#ifndef CHMOD_H
#define CHMOD_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "header.h"  // TreeNode, DirectoryTree 정의 포함

// chmod 명령어 실행 함수
void run_chmod(DirectoryTree* tree, const char* arg);

#endif // CHMOD_H
