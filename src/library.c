/* Name, library.c, CS 24000, Spring 2020
 * Last updated March 27, 2020
 */

/* Add any includes here */

#include "library.h"

#include <string.h>
#include <malloc.h>

/* Global Variables */
tree_node_t *g_song_library;

/* Define find_parent_pointer here */
tree_node_t **find_parent_pointer(tree_node_t **tree_node, const char *song_name){
  if (tree_node == NULL){
    return NULL;
  }
  int read_value = strcmp((*tree_node)->song_name, song_name);
  if (read_value == 0){
    return tree_node;
  }
  if (read_value < 0){
    return find_parent_pointer(&((*tree_node)->left_child), song_name);
  }
  return find_parent_pointer(&((*tree_node)->right_child), song_name);
}

/* Define tree_insert here */
int tree_insert(tree_node_t **tree_root, tree_node_t *tree_node){
  return 0;
}

/* Define remove_song_from_tree here */
int remove_song_from_tree(tree_node_t **tree_root, const char *song_name){
  return 0;
}

/* Define free_node here */
void free_node(tree_node_t *tree_node){
  return;
}

/* Define print_node here */
void print_node(tree_node_t *tree_node, FILE *output_file){
  return;
}

/* Define traverse_pre_order here */
void traverse_pre_order(tree_node_t *tree_node, void *data, traversal_func_t function){
  return;
}

/* Define traverse_in_order here */
void traverse_in_order(tree_node_t *tree_node, void *data, traversal_func_t function){
  return;
}

/* Define traverse_post_order here */
void traverse_post_order(tree_node_t *tree_node, void *data, traversal_func_t function){
  return;
}

/* Define free_library here */
void free_library(tree_node_t *tree_node){
  return;
}

/* Define write_song_list here */
void write_song_list(FILE *fp, tree_node_t *tree_node){
  return;
}
/* Define make_library here */
void make_library(const char *directory_name){
  return;
}
