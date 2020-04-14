/* Aryan Wadhwani, library.c, CS 24000, Spring 2020
 * Last updated April 11, 2020
 */

/* Included Libraries */

#include "library.h"

#include <string.h>
#include <malloc.h>
#include <ftw.h>
#include <assert.h>

/* Defined Constants */

#define MIDI_FILE_EXTENSION ".mid"
#define MAX_DIRECTORY_DEPTH (20)

/* Global Variables */

tree_node_t *g_song_library = NULL;

/* Function Definitions */
tree_node_t **find_parent_handler(tree_node_t **, tree_node_t **, const char *);
int add_file_to_library(const char *, const struct stat *, int);
char* get_file_name(const char *);

/*
 * Finds the parent pointer of the tree_node with the
 * given song_name.
 */

tree_node_t **find_parent_pointer(tree_node_t **tree_node,
                                  const char *song_name){
  if (tree_node == NULL){
    return NULL;
  }
  if (*tree_node == NULL){
    return NULL;
  }
  if (strcmp((*tree_node)->song_name, song_name) == 0){
    return tree_node;
  }
  return find_parent_handler(tree_node, tree_node, song_name);
} /* find_parent_pointer() */

/*
 * Called by find_parent_pointer to handle the finding
 * of the node's parent pointer.
 */

tree_node_t **find_parent_handler(tree_node_t **tree_root,
                                  tree_node_t **tree_parent,
                                  const char *song_name){
  if (tree_root == NULL){
    return NULL;
  }

  if (*tree_root == NULL){
    return NULL;
  }

  int compare_strings = strncmp((*tree_root)->song_name,
                                song_name, strlen(song_name));
  if (compare_strings == 0){
    if (strncmp((*tree_parent)->song_name,
                (*tree_root)->song_name, strlen(song_name)) > 0){
      return &(*tree_parent)->left_child;
    }
    return &(*tree_parent)->right_child;
  }
  if (compare_strings > 0){
    return find_parent_handler(&(*tree_root)->left_child, tree_root, song_name);
  }
  return find_parent_handler(&(*tree_root)->right_child, tree_root, song_name);
} /* find_parent_handler() */

/*
 * Inserts given node into the tree
 */

int tree_insert(tree_node_t **tree_root, tree_node_t *tree_node){
  if (tree_root == NULL){
    tree_root = &tree_node;
    return INSERT_SUCCESS;
  }

  if (*tree_root == NULL){
    *tree_root = tree_node;
    return INSERT_SUCCESS;
  }

  tree_node_t *copy_root = *tree_root;
  tree_node_t *store_parent = NULL;

  while (copy_root != NULL){
    store_parent = copy_root;
    int compare_strings = strncmp(copy_root->song_name,
                                  tree_node->song_name,
                                  strlen(tree_node->song_name));
    if (compare_strings == 0){
      return DUPLICATE_SONG;
    }
    if (compare_strings > 0){
      copy_root = copy_root->left_child;
    }
    else {
      copy_root = copy_root->right_child;
    }
  }
  if (strncmp(store_parent->song_name,
              tree_node->song_name,
              strlen(tree_node->song_name)) > 0){
    store_parent->left_child = tree_node;
    return INSERT_SUCCESS;
  }
  store_parent->right_child = tree_node;
  return INSERT_SUCCESS;
} /* tree_insert() */

/*
 * Removes the node with the given song_name from
 * the binary tree. Inserts it's children back into
 * the tree.
 */

int remove_song_from_tree(tree_node_t **tree_root, const char *song_name){
  if (tree_root == NULL){
    return SONG_NOT_FOUND;
  }
  if (*tree_root == NULL){
    return SONG_NOT_FOUND;
  }
  tree_node_t **found_song = find_parent_pointer(tree_root, song_name);
  if (found_song == NULL){
    return SONG_NOT_FOUND;
  }
  tree_node_t *save_left = (*found_song)->left_child;
  tree_node_t *save_right = (*found_song)->right_child;
  free_node(*found_song);
  *found_song = NULL;
  if (save_left){
    tree_insert(tree_root, save_left);
  }
  if (save_right){
    tree_insert(tree_root, save_right);
  }
  return DELETE_SUCCESS;
} /* remove_song_from_tree() */

/*
 * Frees memory allocated to a node of a
 * tree
 */

void free_node(tree_node_t *tree_node){
  if (tree_node == NULL){
    return;
  }
  if (tree_node->song_name != NULL){
    free_song(tree_node->song);
  }
  free(tree_node);
} /* free_node() */

/*
 * Prints the song_name of the passed-in
 * node.
 */

void print_node(tree_node_t *tree_node, FILE *output_file){
  fprintf(output_file, "%s\n", tree_node->song_name);
  return;
} /* print_node() */

/*
 * Traverse the tree in pre order, running function
 * run_operation on each node. Order is
 * CENTER - LEFT - RIGHT
 */

void traverse_pre_order(tree_node_t *tree_node,
                        void *data, traversal_func_t run_operation){
  if (tree_node == NULL){
    return;
  }
  run_operation(tree_node, data);
  traverse_pre_order(tree_node->left_child, data, run_operation);
  traverse_pre_order(tree_node->right_child, data, run_operation);
  return;
} /* traverse_pre_order() */

/*
 * Traverse the tree in in-order, running function
 * run_operation() on each node. Order is
 * LEFT - CENTER - RIGHT
 */

void traverse_in_order(tree_node_t *tree_node,
                       void *data, traversal_func_t run_operation){
  if (tree_node == NULL){
    return;
  }
  traverse_in_order(tree_node->left_child, data, run_operation);
  run_operation(tree_node, data);
  traverse_in_order(tree_node->right_child, data, run_operation);
  return;
} /* traverse_in_order() */

/*
 * Traverses the tree in post order, running function
 * run_operation() on each node. Order is
 * LEFT - RIGHT - CURRENT
 */

void traverse_post_order(tree_node_t *tree_node,
                         void *data, traversal_func_t run_operation){
  if (tree_node == NULL){
    return;
  }
  traverse_post_order(tree_node->left_child, data, run_operation);
  traverse_post_order(tree_node->right_child, data, run_operation);
  run_operation(tree_node, data);
  return;
} /* traverse_post_order() */

/*
 * Frees all memory associated with a tree node
 */

void free_library(tree_node_t *tree_node){
  if (tree_node == NULL){
    return;
  }
  if (tree_node->left_child != NULL){
    free_library(tree_node->left_child);
    tree_node->left_child = NULL;
  }
  if (tree_node->right_child != NULL){
    free_library(tree_node->right_child);
    tree_node->right_child = NULL;
  }
  free_node(tree_node);
  tree_node = NULL;
} /* free_library() */

/*
 * Writes all of the songs in sorted order
 */

void write_song_list(FILE *fp, tree_node_t *tree_node){
  traverse_in_order(tree_node, fp, (traversal_func_t) print_node);
} /* write_song_list() */

/*
 * Returns a pointer to the location in the
 * file_path where the file_name is found.
 */

char *get_file_name(const char* file_path){
  char *file_name = strchr(file_path, '/');
  if (file_name == NULL){
    file_name = strchr(file_path, file_path[0]);
    return file_name;
  }
  while (strchr(file_name + 1, '/') != NULL){
    file_name = strchr(file_name + 1, '/');
  }
  return file_name + 1;
} /* get_file_name() */

/*
 * Used by make_library to handle creation of a new node
 * and adding it to the global g_song_library
 */

int add_file_to_library(const char *file_path,
                        const struct stat *sb, int type_flag){
  if (type_flag != FTW_F){
    return 0;
  }
  if (strstr(file_path, MIDI_FILE_EXTENSION) == NULL){
    return 0;
  }
  tree_node_t *new_node = malloc(sizeof(tree_node_t));
  new_node->song = parse_file(file_path);
  new_node->song_name = get_file_name(new_node->song->path);
  new_node->left_child = NULL;
  new_node->right_child = NULL;
  tree_insert(&g_song_library, new_node);
  return 0;
} /* add_file_to_library() */

/*
 * Creates a song library using ftw, which
 * recursively calls add_file_to_library to
 * every file in the provided directory
 */

void make_library(const char *directory_name){
  if (ftw(directory_name, &add_file_to_library, MAX_DIRECTORY_DEPTH) != 0){
    free_library(g_song_library);
    g_song_library = NULL;
  }
  return;
} /* make_library() */
