#include "mv.h"
#include <stdio.h>
#include <string.h>

TreeNode *SearchPathWithType(TreeNode *start, const char *path, char type) {
  char temp[MAX_PATH_LENGTH];
  strncpy(temp, path, MAX_PATH_LENGTH);

  char *token = strtok(temp, "/");
  TreeNode *cur = start;

  while (token != NULL) {
    TreeNode *child = cur->left;
    while (child != NULL) {
      if (strcmp(child->name, token) == 0) {
        break;
      }
      child = child->right;
    }
    if (child == NULL)
      return NULL;

    cur = child;
    token = strtok(NULL, "/");
  }

  if (type == '\0' || cur->type == type)
    return cur;

  return NULL;
}

void mv(DirectoryTree *dTree, char *src, char *dest) {
  TreeNode *node = SearchPathWithType(dTree->root, src, 'f');
  if (node == NULL)
    node = SearchPathWithType(dTree->root, src, 'd');

  if (node == NULL) {
    printf("mv: cannot stat '%s': No such file or directory\n", src);
    return;
  }

  TreeNode *parent = node->parent;
  if (parent == NULL) {
    printf("mv: internal error — node '%s' has no parent\n", node->name);
    return;
  }

  TreeNode *tmp = parent->left;
  TreeNode *prev = NULL;
  while (tmp != NULL && tmp != node) {
    prev = tmp;
    tmp = tmp->right;
  }
  if (tmp == NULL)
    return;

  if (prev == NULL)
    parent->left = node->right;
  else
    prev->right = node->right;

  node->right = NULL;

  if (strchr(dest, '/')) {
    char path[MAX_PATH_LENGTH];
    char *newname = strrchr(dest, '/');

    if (*(newname + 1) == '\0') {
      strncpy(path, dest, newname - dest);
      path[newname - dest] = '\0';
      newname = NULL;
    } else {
      strncpy(path, dest, newname - dest);
      path[newname - dest] = '\0';
      newname++;
    }

    TreeNode *targetDir = SearchPathWithType(dTree->root, path, 'd');
    if (targetDir == NULL) {
      printf("mv: target directory '%s' not found\n", path);
      return;
    }

    if (newname != NULL && strlen(newname) > 0) {
      strcpy(node->name, newname);
    }

    node->parent = targetDir;
    if (targetDir->left == NULL)
      targetDir->left = node;
    else {
      TreeNode *tail = targetDir->left;
      while (tail->right != NULL)
        tail = tail->right;
      tail->right = node;
    }

    printf("mv: '%s' moved to '%s/%s'\n", src, path, node->name);
  } else {
    TreeNode *targetDir = SearchPathWithType(dTree->root, dest, 'd');
    if (targetDir != NULL) {
      node->parent = targetDir;
      if (targetDir->left == NULL)
        targetDir->left = node;
      else {
        TreeNode *tail = targetDir->left;
        while (tail->right != NULL)
          tail = tail->right;
        tail->right = node;
      }
      printf("mv: '%s' moved to '%s/'\n", src, dest);
    } else {
      strcpy(node->name, dest);
      node->parent = dTree->current;
      if (dTree->current->left == NULL)
        dTree->current->left = node;
      else {
        TreeNode *tail = dTree->current->left;
        while (tail->right != NULL)
          tail = tail->right;
        tail->right = node;
      }
      printf("mv: renamed '%s' to '%s'\n", src, dest);
    }
  }
}
