// mkdir.c
#include "mkdir.h"
#include "header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#define MAX_TOKENS 50
#define BUF_SIZE   1024

// ————————————————————————————————————————————————
// Create one directory on disk *and* in‐memory…
int my_mkdir(DirectoryTree* dTree, const char* dirname, int mode) {
    // 1) real filesystem
    if (mkdir(dirname, mode) != 0 && errno != EEXIST) {
        perror("mkdir");
        return -1;
    }

    // 2) now record in your tree
    TreeNode* New = malloc(sizeof(TreeNode));
    if (!New) { perror("malloc"); return -1; }

    time_t t; struct tm* now;
    time(&t); now = localtime(&t);

    New->left   = NULL;
    New->right  = NULL;
    strncpy(New->name, dirname, MAX_NAME_LENGTH);
    New->name[MAX_NAME_LENGTH-1] = '\0';
    New->type   = 'd';
    New->mode   = mode;       // store the actual bitmask
    New->size   = 4096;
    New->UID    = getuid();
    New->GID    = getgid();
    New->month  = now->tm_mon + 1;
    New->day    = now->tm_mday;
    New->hour   = now->tm_hour;
    New->minute = now->tm_min;
    New->parent = dTree->current;

    // left/right = first-child / next-sibling
    if (!dTree->current->left) {
        dTree->current->left = New;
    } else {
        TreeNode* tmp = dTree->current->left;
        while (tmp->right) tmp = tmp->right;
        tmp->right = New;
    }
    return 0;
}

// ————————————————————————————————————————————————
// helper for “mkdir -p a/b/c”
static int mkdir_parents(DirectoryTree* tree,
                         const char* path,
                         int mode)
{
    TreeNode* save = tree->current;
    char buf[BUF_SIZE];
    strncpy(buf, path, sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    char* tok = strtok(buf, "/");
    while (tok) {
        // find an existing child?
        TreeNode* child = tree->current->left;
        while (child &&
               !(child->type=='d' && strcmp(child->name, tok)==0))
            child = child->right;
        if (!child) {
            if (my_mkdir(tree, tok, mode) != 0) {
                tree->current = save;
                return -1;
            }
            // pick up the one we just made
            child = tree->current->left;
            while (child->right) child = child->right;
        }
        tree->current = child;
        tok = strtok(NULL, "/");
    }

    tree->current = save;
    return 0;
}

// ─ thread worker
static void* mkdir_thread_worker(void* arg) {
    MkdirThreadArg* a = arg;
    my_mkdir(a->tree, a->dirname, a->mode);
    free(a);
    return NULL;
}

// ————————————————————————————————————————————————
// run everything: parse -p, -m, then either serial (-p) or threaded
void run_mkdir_multithread(DirectoryTree* tree,
                           const char* arg,
                           int default_mode)
{
    if (!arg || !*arg) {
        printf("mkdir: missing operand\n");
        return;
    }

    // tokenize
    char buf[BUF_SIZE];
    strncpy(buf, arg, sizeof(buf));
    buf[sizeof(buf)-1] = '\0';

    char*  tokens[MAX_TOKENS] = {0};
    int    ntok   = 0;
    int    pflag  = 0;
    int    mode   = default_mode;
    char*  tk     = strtok(buf, " ");
    while (tk && ntok < MAX_TOKENS) {
        if (strcmp(tk, "-p") == 0) {
            pflag = 1;
        }
        else if (strcmp(tk, "-m") == 0) {
            char* mstr = strtok(NULL, " ");
            if (mstr) mode = strtol(mstr, NULL, 8);
        }
        else {
            tokens[ntok++] = tk;
        }
        tk = strtok(NULL, " ");
    }

    // if no -p, slash in name is an error
    for (int i = 0; i < ntok; i++) {
        if (!pflag && tokens[i] && strchr(tokens[i], '/')) {
            printf("mkdir: cannot create directory '%s': No such file or directory\n",
                   tokens[i]);
            tokens[i] = NULL;
        }
    }

    // serial parent‐creation
    if (pflag) {
        for (int i = 0; i < ntok; i++) {
            if (tokens[i])
                if (mkdir_parents(tree, tokens[i], mode) != 0)
                    printf("mkdir: failed to create '%s'\n", tokens[i]);
        }
        return;
    }

    // threaded leaf‐only creation
    pthread_t thr[MAX_TOKENS];
    int thcount = 0;
    for (int i = 0; i < ntok; i++) {
        if (!tokens[i]) continue;
        MkdirThreadArg* a = malloc(sizeof(*a));
        a->tree  = tree;
        a->mode  = mode;
        strncpy(a->dirname, tokens[i], MAX_NAME_LENGTH);
        a->dirname[MAX_NAME_LENGTH-1] = '\0';
        if (pthread_create(&thr[thcount], NULL, mkdir_thread_worker, a) == 0)
            thcount++;
        else {
            perror("pthread_create");
            free(a);
        }
    }
    while (thcount--) pthread_join(thr[thcount], NULL);
}
