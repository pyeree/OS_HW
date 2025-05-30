// diff.h
#ifndef DIFF_H
#define DIFF_H

/**
 * @brief 두 파일의 차이점을 Unix diff 스타일로 출력합니다.
 * @param file1 첫 번째 파일 경로
 * @param file2 두 번째 파일 경로
 */
void diff(const char* file1, const char* file2);

#endif // DIFF_H
