/* Aryan Wadhwani, library.c, CS 24000, Spring 2020
 * Last updated April 3, 2020
 */

/* Included Libraries */

#include "library.h"
#include "song_writer.h"

#include <string.h>
#include <libgen.h>
#include <malloc.h>
#include <dirent.h>
#include <ftw.h>
#include <assert.h>

/* Global Variables */

tree_node_t *g_song_library = NULL;

/* Function Definitions */

int add_file_to_library(const char *, const struct stat *, int);
char* get_file_name(const char *);

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
  tree_node = NULL;
}

/* Define write_song_list here */
void write_song_list(FILE *fp, tree_node_t *tree_node){
  traverse_pre_order(tree_node, fp, (traversal_func_t) print_node);
  return;
}

int add_file_to_library(const char *file_path, const struct stat *sb, int type_flag){
  if (type_flag != FTW_F){
    return 0;
  }
  tree_node_t *new_node = malloc(sizeof(tree_node_t));
  
  /* 
  char *file_name = malloc(strlen(file_path) * sizeof(char) + 1);
  char *save_right = malloc(strlen(file_path) * sizeof(char) + 1);
  char *dir_string = malloc(strlen(file_path) * sizeof(char) + 1);
  */

  char file_name[strlen(file_path)];
  char save_right[strlen(file_path)];
  char dir_string[strlen(file_path)];

  int check_error = 0;
  strncpy(dir_string, file_path, strlen(file_path));
  dir_string[strlen(file_path)] = '\0';
  do {
    check_error = sscanf(dir_string, "%[^/]/%[^\n]", file_name, save_right);
    strncpy(dir_string, save_right, strlen(save_right));
    dir_string[strlen(save_right)] = '\0';
  } while (check_error == 2);
 
  new_node->song_name = malloc(strlen(save_right) * sizeof(char) + 1);
  strncpy(new_node->song_name, save_right, strlen(save_right));
  new_node->song_name[strlen(save_right)] = '\0';

  /* 
  
  free(file_name);
  free(save_right);
  free(dir_string);
  file_name = NULL;
  save_right = NULL;
  dir_string = NULL;
  
  */
  new_node->song = parse_file(file_path);
  new_node->left_child = NULL;
  new_node->right_child = NULL;
  tree_insert(&g_song_library, new_node); 
  return 0;
}

/* Define make_library here */
void make_library(const char *directory_name){
  assert(ftw(directory_name, &add_file_to_library, 20) == 0);
  return;
}

void write_song_to_file(tree_node_t *tree_node, char *dir){
  char result[strlen(tree_node->song_name) + strlen(dir) + 2];
  strcpy(result, dir);
  strcat(result, tree_node->song_name);
  write_song_data(tree_node->song, result);
}

/* 
int main(){
  make_library("music/");
  traverse_pre_order(g_song_library, "music/my_tests/output_", (traversal_func_t) write_song_to_file);
  return 0;
}
*/
