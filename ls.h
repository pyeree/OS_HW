// ls.h
#ifndef LS_H
#define LS_H

#include "header.h" // DirectoryTree 구조체 정의를 위해 포함

// ls.c에 정의된 함수의 선언
void ls(DirectoryTree* dTree);
void ls_a(DirectoryTree* dTree);
void ls_l(DirectoryTree* dTree);
void ls_al(DirectoryTree* dTree);

// ls 기능 관련 구조체, 상수 등의 선언 (필요한 경우)

#endif