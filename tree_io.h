#ifndef TREE_IO_H
#define TREE_IO_H

#include "header.h"

void save_tree_to_file(DirectoryTree* dTree, const char* filename);
void load_tree_from_file(DirectoryTree* dTree, const char* filename);

#endif // TREE_IO_H