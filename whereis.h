#pragma once
#include "header.h" // ✅ DirectoryTree, TreeNode 사용

// 실제 탐색을 수행하는 helper 함수 선언
void whereis_helper(TreeNode *node, const char *target, char *path);

// 주어진 이름을 가진 파일 또는 디렉토리의 전체 경로를 찾는 함수 선언
void whereis(DirectoryTree *dTree, const char *target);