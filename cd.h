#pragma once
#include "header.h" // ✅ DirectoryTree 사용

// 현재 디렉토리를 변경하는 함수 선언
void cd(DirectoryTree *dTree, const char *path);
void update_current_path(DirectoryTree *dTree);
