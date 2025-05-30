// mkdir.h
#ifndef MKDIR_H
#define MKDIR_H

#include "header.h"

// thread-worker arg
typedef struct {
    DirectoryTree* tree;
    char dirname[MAX_NAME_LENGTH];
    int  mode;
} MkdirThreadArg;

/**
 * @brief Create one directory in memory *and* on the real filesystem.
 * @return 0 on success, –1 on failure
 */
int my_mkdir(DirectoryTree* dTree, const char* dirname, int mode);

/**
 * @brief Handle “mkdir [ -p ] [ -m MODE ] name…” all in one go.
 *        Supports:
 *           -p        create parent directories as needed
 *           -m 700    set mode bits
 */
void run_mkdir_multithread(DirectoryTree* tree, const char* arg, int default_mode);

#endif // MKDIR_H
