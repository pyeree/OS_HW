#pragma once
#include "cd.h"
#include "header.h"
#define MAX_PATH_LENGTH 255

TreeNode *SearchPathWithType(TreeNode *start, const char *path, char type);
void mv(DirectoryTree *dTree, char *src, char *dest);
