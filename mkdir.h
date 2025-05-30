// mkdir.h
#ifndef MKDIR_H
#define MKDIR_H

#include "header.h"

typedef struct {
    DirectoryTree* tree;
    char dirname[MAX_NAME_LENGTH];
    int mode;
} MkdirThreadArg;

/**
 * @brief 트리에 단일 디렉터리 노드를 추가 (in‐memory)
 * @return 0=성공, -1=실패
 */
int my_mkdir(DirectoryTree* dTree, const char* dirname, int mode);

/**
 * @brief mkdir 명령 전체 처리
 *   옵션: -p (parents), -m <mode>
 * @param tree         디렉터리 트리
 * @param arg          "[-p] [-m 700] name1 name2/child name3" 꼴
 * @param default_mode 기본 모드 (예: 0755)
 */
void run_mkdir_multithread(DirectoryTree* tree, const char* arg, int default_mode);

#endif // MKDIR_H
