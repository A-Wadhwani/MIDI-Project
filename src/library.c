/* Aryan Wadhwani, library.c, CS 24000, Spring 2020
 * Last updated April 3, 2020
 */

/* Included Libraries */

#include "library.h"
#include "song_writer.h"

#include <string.h>
#include <malloc.h>
#include <dirent.h>
#include <ftw.h>
#include <assert.h>

/* Global Variables */

tree_node_t *g_song_library = NULL;

/* Function Definitions */
tree_node_t **find_parent_handler(tree_node_t **, tree_node_t **, const char *);
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
  return find_parent_handler(tree_node, tree_node, song_name);
}

tree_node_t **find_parent_handler(tree_node_t **tree_root, tree_node_t **tree_parent,
                                  const char *song_name){
  if (tree_root == NULL){
    return NULL;
  }
  if (*tree_root == NULL){
    return NULL;
  }
  int compare_strings = strncmp((*tree_root)->song_name, song_name, strlen(song_name));
  if (compare_strings == 0){
    if (strncmp((*tree_parent)->song_name, (*tree_root)->song_name, strlen(song_name)) < 0){
      return &(*tree_parent)->left_child;
    }
    return &(*tree_parent)->right_child;
  }
  if (compare_strings < 0){
    return find_parent_handler(&(*tree_root)->left_child, tree_root, song_name);
  }
  return find_parent_handler(&(*tree_root)->right_child, tree_root, song_name);
}

/* Define tree_insert here */
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
    int compare_strings = strncmp(copy_root->song_name, tree_node->song_name, strlen(tree_node->song_name));
    if (compare_strings == 0){
      return DUPLICATE_SONG;
    }
    if (compare_strings < 0){
      copy_root = copy_root->left_child;
    }
    else {
      copy_root = copy_root->right_child;
    }
  }
  if (strncmp(store_parent->song_name, tree_node->song_name, strlen(tree_node->song_name)) < 0){
    store_parent->left_child = tree_node;
    return INSERT_SUCCESS;
  }
  store_parent->right_child = tree_node;
  return INSERT_SUCCESS;
}

/* Define remove_song_from_tree here */
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
}

/* Define free_node here */
void free_node(tree_node_t *tree_node){
  if (tree_node == NULL){
    return;
  }
  if (tree_node->song_name != NULL){
    free_song(tree_node->song);
  }
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
}

/* Define write_song_list here */
void write_song_list(FILE *fp, tree_node_t *tree_node){
  traverse_in_order(tree_node, fp, (traversal_func_t) print_node);
  return;
}

char* get_file_name(const char* file_path){
  char *file_name = strchr(file_path, '/');
  while (strchr(file_path + 1, '/') != NULL){
    file_name = strchr(file_name + 1, '/');
    printf("%s", file_name);
  }
  return file_name;
}


int add_file_to_library(const char *file_path, const struct stat *sb, int type_flag){
  if (type_flag != FTW_F){
    return 0;
  }
  tree_node_t *new_node = malloc(sizeof(tree_node_t));


  /* char *file_name = malloc(strlen(file_path) * sizeof(char) + 1);
  char *save_right = malloc(strlen(file_path) * sizeof(char) + 1);
  char *dir_string = malloc(strlen(file_path) * sizeof(char) + 1);

  char save_right[strlen(file_path)];
  char dir_string[strlen(file_path)];

  int check_error = 0;
  strncpy(dir_string, file_path, strlen(file_path));
  dir_string[strlen(file_path)] = '\0';
  do {
    char file_name[strlen(file_path)];
    check_error = sscanf(dir_string, "%[^/]/%[^\n]", file_name, save_right);
    strncpy(dir_string, save_right, strlen(save_right));
    dir_string[strlen(save_right)] = '\0';
  } while (check_error == 2);

  new_node->song_name = malloc(strlen(save_right) * sizeof(char) + 1);
  strncpy(new_node->song_name, save_right, strlen(save_right));
  new_node->song_name[strlen(save_right)] = '\0';
*/
  new_node->song = parse_file(file_path);
  new_node->song_name = get_file_name(new_node->song->path); 
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
