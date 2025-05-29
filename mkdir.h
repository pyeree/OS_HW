// mkdir.h
#ifndef MKDIR_H
#define MKDIR_H

#include "header.h"    // DirectoryTree, TreeNode 정의 :contentReference[oaicite:0]{index=0}

/**
 * @brief 단일 디렉터리를 생성하여 트리와 실제 파일시스템에 추가
 * @param dTree 디렉터리 트리 구조체
 * @param dirname 생성할 디렉터리 이름
 * @param mode 퍼미션 비트(예: 0755)
 * @return 0 성공, -1 실패
 */
int my_mkdir(DirectoryTree* dTree, const char* dirname, int mode);

/**
 * @brief 내부 사용: 쓰레드 워커 함수
 * @param arg MkdirThreadArg* 포인터
 */
void* mkdir_thread_worker(void* arg);

/**
 * @brief 공백으로 구분된 다중 디렉터리 이름을 멀티스레드로 생성
 * @param tree 디렉터리 트리
 * @param arg "dir1 dir2 dir3" 형태의 문자열
 * @param mode 퍼미션 비트
 */
void run_mkdir_multithread(DirectoryTree* tree, const char* arg, int mode);

#endif // MKDIR_H
