// rmdir.h
#ifndef RMDIR_H
#define RMDIR_H

#include "header.h"    // DirectoryTree, TreeNode 정의 :contentReference[oaicite:1]{index=1}
#include "tree_io.h"   // save_tree_to_file, load_tree_from_file 선언 :contentReference[oaicite:2]{index=2}

/**
 * @brief 현재 디렉터리(dTree->current) 아래에서 단일 디렉터리 제거
 * @param dTree 디렉터리 트리
 * @param dir_name 제거할 디렉터리 이름
 * @return 0 성공, -1 실패
 */
int remove_dir(DirectoryTree* dTree, const char* dir_name);

/**
 * @brief 경로(path)의 마지막 요소만 제거 (예: "a/b/c" → c만 제거)
 * @param dTree 디렉터리 트리
 * @param path "/" 구분 경로 문자열
 * @return 0 성공, -1 실패
 */
int remove_dir_path(DirectoryTree* dTree, const char* path);

/**
 * @brief -p 옵션처럼 경로상의 모든 요소를 뒤에서부터 순차 제거
 * @param dTree 디렉터리 트리
 * @param path "/" 구분 경로 문자열
 * @return 0 성공, -1 실패
 */
int remove_dir_p_path(DirectoryTree* dTree, const char* path);

#endif // RMDIR_H
