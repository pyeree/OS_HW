#include <stdio.h>
#include "clear.h"

void clear() {
    printf("\033[H");   // 커서를 화면의 왼쪽 상단(홈 위치)으로 이동
    printf("\033[J");   // 현재 커서 위치부터 화면의 끝까지 모든 내용 지우기
    printf("\033[3J");  // 터미널의 스크롤백(history) 버퍼까지 모두 지우기
}