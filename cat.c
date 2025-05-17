#include "cat.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAXB 1024
#define MAX_NAME_LENGTH 255

int cat(DirectoryTree *dTree, char *cmd) {
    char *str;

    if (strcmp(cmd, ">") == 0) {
        str = strtok(NULL, " ");
        Concatenate(dTree, str, 0);
        return 0;
    } else if (strcmp(cmd, "-n") == 0) {
        str = strtok(NULL, " ");
        Concatenate(dTree, str, 2);
    } else {
        Concatenate(dTree, cmd, 1);
    }

    return 1;
}

int Concatenate(DirectoryTree *dTree, char *fName, int o) {
    //파일을 읽어 출력하거나(o=1/2) 표준입력으로부터 받아 새 파일을 생성하고 저장
    //(o=0)

    FILE *fp;
    char BUF[MAXB];
    int count = 1;

    if (o != 0) {
        fp = fopen(fName, "r");
        if (fp == NULL) {
            printf("cat: %s: No such file\n", fName);
            return 1;
        }

        while (feof(fp) == 0) {
            if (fgets(BUF, sizeof(BUF), fp) == NULL)
                break;
            if (o == 2) {
                printf("%d  ", count);
                count++;
            }
            fputs(BUF, stdout);
        }
        fclose(fp);
    } else {
        int fd = open(fName, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (fd == -1) {
            perror("open");
            return 1;
        }

        char buf[1000];
        int n;
        while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
            if (write(fd, buf, n) != n) {
                perror("write");
                close(fd);
                return 1;
            }
        }

        if (n == -1) {
            perror("read");
            close(fd);
            return 1;
        }

        // 파일 크기 계산
        int k = lseek(fd, 0, SEEK_END);
        if (k == -1) {
            perror("lseek");
            close(fd);
            return 1;
        }

        close(fd);

        Mkfile(dTree, fName, 'f', k); // 파일 생성 전용 함수
    }
    return 0;
}

int Mkfile(DirectoryTree *dTree, char *dName, char type, int k) {
    //트리 구조에 새로운 파일 노드를 생성하고 현재 디렉토리에 연결

    TreeNode *New = (TreeNode *)malloc(sizeof(TreeNode));
    TreeNode *tmpN = NULL;

    time_t ltime;
    struct tm *Now;

    time(&ltime);
    Now = localtime(&ltime);

    New->left = NULL;
    New->right = NULL;

    strncpy(New->name, dName, MAX_NAME_LENGTH);
    New->type = 'f';
    New->mode = 0644;
    New->size = k;
    New->UID = 1234;
    New->GID = 1234;
    New->month = Now->tm_mon + 1;
    New->day = Now->tm_mday;
    New->hour = Now->tm_hour + 9;
    New->minute = Now->tm_min;
    New->parent = dTree->current;

    if (dTree->current->left == NULL) {
        dTree->current->left = New;
    } else {
        tmpN = dTree->current->left;

        while (tmpN->right != NULL) {
            tmpN = tmpN->right;
        }
        tmpN->right = New;
    }

    return 0;
}