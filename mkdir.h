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
 * @brief 트리에 단일 디렉터리 노드를 추가
 * @param dTree   디렉터리 트리
 * @param dirname 디렉터리 이름 (경로 분리되지 않은 단일 이름)
 * @param mode    퍼미션 비트 (8진수)
 * @return 0=성공, -1=실패
 */
int my_mkdir(DirectoryTree* dTree, const char* dirname, int mode);

/**
 * @brief mkdir 명령 전체 처리
 *   - 공백으로 구분된 arg 에서 옵션 -p, -m <mode> 를 파싱
 *   - 기본 모드를 default_mode 로 사용 (예: 0755)
 *   - -p 없으면 각 이름을 멀티스레드로 my_mkdir 호출
 *   - -p 있으면 부모 경로까지 순차적으로 생성
 *
 * @param tree         디렉터리 트리
 * @param arg          "[-p] [-m 700] name1 name2/child name3" 형태 문자열
 * @param default_mode 기본 퍼미션 비트 (8진수, 예: 0755)
 */
void run_mkdir_multithread(DirectoryTree* tree, const char* arg, int default_mode);

#endif // MKDIR_H
