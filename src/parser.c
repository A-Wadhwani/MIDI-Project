/* Aryan Wadhwani, parser.c, CS 24000, Spring 2020
 * Last updated March 27, 2020
 */

/* Add any includes here */
#include "../include/parser.h"

#include <malloc_debug.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

/* Function Definitions */

/*
 * Define parse_file here
 */

song_data_t *parse_file(const char* file_name){
  assert(file_name);

  song_data_t *midi_song = malloc(sizeof(song_data_t));
  assert(midi_song);

  FILE *read_file = fopen(file_name, "rb");
  assert(read_file);

  midi_song->path = malloc(strlen(file_name) + 1);
  assert(midi_song->path);

  strcpy(midi_song->path, file_name);
  midi_song->path[strlen(file_name)] = '\0';

  parse_header(read_file, midi_song);
  parse_track(read_file, midi_song);
  assert(feof(read_file));

  return midi_song;
} /* parse_file() */

/* Define parse_header here */
void parse_header(FILE *read_file, song_data_t *midi_song){
  char *chunk_type = malloc(4 * sizeof(char));
  int check_error = fread(chunk_type, sizeof(chunk_type), 1, read_file);
  assert(strcmp(chunk_type, "MThd") == 0);
  assert(check_error == 1);

  free(chunk_type);

  uint32_t length = 0;
  check_error = fread(&length, sizeof(length), 1, read_file);
  assert(length == 6);
  assert(check_error == 1);

  uint16_t format = 0;
  check_error = fread(&format, sizeof(format), 1, read_file);
  assert((format == 0) || (format == 1) || (format == 2));
  assert(check_error == 1);
  midi_song->format = format;

  uint16_t num_tracks = 0;
  check_error = fread(&num_tracks, sizeof(num_tracks), 1, read_file);
  assert(check_error == 1);
  midi_song->num_tracks = num_tracks;
  
  uint16_t read_division = 0;
  check_error = fread(&read_division, sizeof(read_division), 1, read_file);
  assert(check_error == 1);
  midi_song->division.uses_tpq = (read_division >> 15) == 0;
  midi_song->division.ticks_per_qtr = read_division;
  return;
} /* parse_header() */

/* Define parse_track here */
void parse_track(FILE *read_file, song_data_t *midi_song){
  return;
}

/* Define parse_event here */
event_t *parse_event(FILE *read_file){
  return NULL;
}

/* Define parse_sys_event here */
sys_event_t parse_sys_event(FILE *read_file, uint8_t size){
  sys_event_t a;
  a.data_len = 0;
  return a;
}

/* Define parse_meta_event here */
meta_event_t parse_meta_event(FILE *read_file){
  meta_event_t a;
  a.data_len = 0;
  return a;
}

/* Define parse_midi_event here */
midi_event_t parse_midi_event(FILE *read_file, uint8_t size){
  midi_event_t a;
  a.status = 0;
  return a;
}

/* Define parse_var_len here */
uint32_t parse_var_len(FILE *read_file){
  return 0;
}

/* Define event_type here */
uint8_t event_type(event_t *event){
  return 0;
}

/* Define free_song here */
void free_song(song_data_t *midi_song){
  return;
}

/* Define free_track_node here */
void free_track_node(track_node_t *song_track){
  return;
}

/* Define free_event_node here */
void free_event_node(event_node_t *song_event){
  return;
}

/* Define end_swap_16 here */
uint16_t end_swap_16(uint8_t num[2]){
  return 0;
}

/* Define end_swap_32 here */
uint32_t end_swap_32(uint8_t num[4]){
  return 0;
}

