#ifndef TREE_IO_H
#define TREE_IO_H

#include "header.h"

void save_tree_to_file(DirectoryTree* dTree, const char* filename);
void load_tree_from_file(DirectoryTree* dTree, const char* filename);

void save_tree_helper(FILE* file, TreeNode* node, int level);
TreeNode* load_tree_helper(FILE* file, int level);

#endif // TREE_IO_H