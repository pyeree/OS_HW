#pragma once
#include "header.h" // ✅ DirectoryTree, TreeNode 사용

// 파일을 읽어 출력하거나(o=1/2) 표준입력으로부터 받아 새 파일을 생성하고 저장하는 함수 선언 (o=0)
int Concatenate(DirectoryTree *dTree, char *fName, int o);

// 트리 구조에 새로운 파일 노드를 생성하고 현재 디렉토리에 연결하는 함수 선언
int Mkfile(DirectoryTree *dTree, char *dName, char type, int k);

// cat 명령어 처리 함수 선언
int cat(DirectoryTree *dTree, char *cmd);