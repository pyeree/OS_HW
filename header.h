#ifndef HEADER_H
#define HEADER_H

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define MAX_NAME_LENGTH 255
#define MAX_PATH_LENGTH 1024

#define MAXB 1024

typedef struct TreeNode {
	char name[MAX_NAME_LENGTH];
	int User_ID;
	int Group_ID;
	char type;
	int mode;
	int size;
	int UID;
	int GID;
	int month;
	int day;
	int hour;
	int minute;
	struct TreeNode* left;
	struct TreeNode* right;
	struct TreeNode* parent;
	struct TreeNode* children;
	struct TreeNode* next_sibling;
} TreeNode;

typedef struct DirectoryTree {
	TreeNode* root;
	TreeNode* current;
	char current_path[MAX_NAME_LENGTH * 10];
} DirectoryTree;

typedef struct ThreadArg {
	DirectoryTree* dTree;
	char name[MAX_NAME_LENGTH];
	int mode;
} ThreadArg;

typedef struct UserList {
	int User_ID;
	int Group_ID;
	struct UserList* next;
} UserList;

#endif // HEADER_H
