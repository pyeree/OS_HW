#pragma once
#include "header.h" // ✅ DirectoryTree 사용

// 주어진 이름의 빈 파일을 생성하고, 디렉토리 트리에 파일 노드를 추가하는 함수 선언
void touch(DirectoryTree *dTree, const char *filename);