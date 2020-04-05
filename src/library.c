/* Aryan Wadhwani, library.c, CS 24000, Spring 2020
 * Last updated April 3, 2020
 */

/* Included Libraries */

#include "library.h"

#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include <ftw.h>

/* Global Variables */

tree_node_t *g_song_library = NULL;

/* Define find_parent_pointer here */
tree_node_t **find_parent_pointer(tree_node_t **tree_node, const char *song_name){
  if (tree_node == NULL){
    return NULL;
  }
  if (*tree_node == NULL){
    return NULL;
  }
  int compare_strings = strcmp((*tree_node)->song_name, song_name);
  if (compare_strings == 0){
    return tree_node;
  }
  if (compare_strings < 0){
    return find_parent_pointer(&((*tree_node)->left_child), song_name);
  }
  return find_parent_pointer(&((*tree_node)->right_child), song_name);
}

/* Define tree_insert here */
int tree_insert(tree_node_t **tree_root, tree_node_t *tree_node){
  if (tree_root == NULL){
    tree_root = malloc(sizeof(tree_node_t*));
  }
  if (*tree_root == NULL){
    *tree_root = tree_node;
    return INSERT_SUCCESS;
  }
  int compare_strings = strcmp((*tree_root)->song_name, tree_node->song_name);
  if (compare_strings == 0){
    return DUPLICATE_SONG;
  }
  if (compare_strings < 0){
    return tree_insert(&((*tree_root)->left_child), tree_node);
  }
  return tree_insert(&((*tree_root)->right_child), tree_node);
}

/* Define remove_song_from_tree here */
int remove_song_from_tree(tree_node_t **tree_root, const char *song_name){
  if (tree_root == NULL){
    return SONG_NOT_FOUND;
  }
  if (*tree_root == NULL){
    return SONG_NOT_FOUND;
  }
  tree_node_t **copy_root = tree_root;
  while(copy_root != NULL){
    int compare_strings = strcmp((*copy_root)->song_name, song_name);
    if (compare_strings == 0){
      tree_node_t *save_left = (*copy_root)->left_child;
      tree_node_t *save_right = (*copy_root)->right_child;
      free_node(*copy_root);
      *copy_root = NULL;
      tree_insert(tree_root, save_left);
      tree_insert(tree_root, save_right);
      return DELETE_SUCCESS;
    }
    if (compare_strings < 0){
      copy_root = &((*copy_root)->left_child);
    }
    else {
      copy_root = &((*copy_root)->right_child);
    }
  }
  return SONG_NOT_FOUND;
}

/* Define free_node here */
void free_node(tree_node_t *tree_node){
  if (tree_node == NULL){
    return;
  }
  free(tree_node->song_name);
  free_song(tree_node->song);
  free(tree_node);
}

/* Define print_node here */
void print_node(tree_node_t *tree_node, FILE *output_file){
  fprintf(output_file, "%s\n", tree_node->song_name);
  return;
}

/* Define traverse_pre_order here */
void traverse_pre_order(tree_node_t *tree_node, void *data, traversal_func_t run_operation){
  if (tree_node == NULL){
    return;
  }
  run_operation(tree_node, data);
  traverse_pre_order(tree_node->left_child, data, run_operation);
  traverse_pre_order(tree_node->right_child, data, run_operation);
  return;
}

/* Define traverse_in_order here */
void traverse_in_order(tree_node_t *tree_node, void *data, traversal_func_t run_operation){
  if (tree_node == NULL){
    return;
  }
  traverse_in_order(tree_node->left_child, data, run_operation);
  run_operation(tree_node, data);
  traverse_in_order(tree_node->right_child, data, run_operation);
  return;
}

/* Define traverse_post_order here */
void traverse_post_order(tree_node_t *tree_node, void *data, traversal_func_t run_operation){
  if (tree_node == NULL){
    return;
  }
  traverse_post_order(tree_node->left_child, data, run_operation);
  traverse_post_order(tree_node->right_child, data, run_operation);
  run_operation(tree_node, data);
  return;
}

/* Define free_library here */
void free_library(tree_node_t *tree_node){
  if (tree_node == NULL){
    return;
  }
  free_library(tree_node->left_child);
  free_library(tree_node->right_child);
  free_node(tree_node);
}

/* Define write_song_list here */
void write_song_list(FILE *fp, tree_node_t *tree_node){
  traverse_pre_order(tree_node, fp, (traversal_func_t) print_node);
  return;
}

/* Define make_library here */
void make_library(const char *directory_name){
  DIR *directory = opendir(directory_name);
  struct dirent *dir = NULL;
  while ((dir = readdir(directory)) != NULL){
  }
  return;
}

