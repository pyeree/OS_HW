//mkdir.h
#ifndef MKDIR_H
#define MKDIR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "header.h"

// 기본 mkdir 명령어 실행 함수
void run_mkdir(DirectoryTree* tree, const char* dirname);

#endif // MKDIR_H
