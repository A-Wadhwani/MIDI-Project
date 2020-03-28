/* Aryan Wadhwani, parser.c, CS 24000, Spring 2020
 * Last updated March 27, 2020
 */

/* Add any includes here */
#include "../include/parser.h"
#include "../include/song_writer.h"
#include "../include/event_tables.h"

#include <malloc.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>

/* Function Declarations */
uint32_t endian_swap_32(uint32_t);
uint16_t endian_swap_16(uint16_t);
uint32_t get_size(event_t *);
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

  midi_song->track_list = malloc(sizeof(track_node_t));
  assert(midi_song->track_list);

  parse_header(read_file, midi_song);
  for(int i = 0; i < midi_song->num_tracks; i++){
    parse_track(read_file, midi_song);
  }
//  assert(feof(read_file));

  return midi_song;
} /* parse_file() */

/* Define parse_header here */
void parse_header(FILE *read_file, song_data_t *midi_song){
  char *chunk_type = malloc(4 * sizeof(char));
  assert(chunk_type);
  int check_error = fread(chunk_type, 4 * sizeof(char), 1, read_file);
  assert(strcmp(chunk_type, "MThd") == 0);
  assert(check_error == 1);
  free(chunk_type);

  uint32_t length = 0;
  check_error = fread(&length, sizeof(length), 1, read_file);
  assert(check_error == 1);
  length = endian_swap_32(length);
  assert(length == 6);

  uint16_t format = 0;
  check_error = fread(&format, sizeof(format), 1, read_file);
  assert(check_error == 1);
  format = endian_swap_16(format);
  assert((format == 0) || (format == 1) || (format == 2));
  midi_song->format = format;

  uint16_t num_tracks = 0;
  check_error = fread(&num_tracks, sizeof(num_tracks), 1, read_file);
  assert(check_error == 1);
  num_tracks = endian_swap_16(num_tracks);
  midi_song->num_tracks = num_tracks;

  uint16_t read_division = 0;
  check_error = fread(&read_division, sizeof(read_division), 1, read_file);
  assert(check_error == 1);
  read_division = endian_swap_16(read_division);
  midi_song->division.uses_tpq = (read_division >> 15) == 0;
  if (read_division >> 15 == 0){
    midi_song->division.ticks_per_qtr = read_division;
  } else {
    printf("does not use \n");
    midi_song->division.frames_per_sec = read_division >> 8;
    midi_song->division.ticks_per_frame = read_division;
  }
  return;
} /* parse_header() */

/* Define parse_track here */
void parse_track(FILE *read_file, song_data_t *midi_song){
  /*
  track_t *new_track = malloc(sizeof(track_t)); 
  char *chunk_type = malloc(4 * sizeof(char));
  assert(chunk_type);
  int check_error = fread(chunk_type, 4 * sizeof(char), 1, read_file);
  assert(check_error == 1);
  assert(strcmp(chunk_type, "MTrk") == 0);
  free(chunk_type);

  uint32_t length = 0;
  check_error = fread(&length, sizeof(length), 1, read_file);
  assert(check_error == 1);
  length = endian_swap_32(length);
  new_track->length = length;

  new_track->event_list = malloc(sizeof(event_node_t));
  assert(new_track->event_list);
  event_node_t *copy_track_head = new_track->event_list;
  
  uint32_t count_bytes = 0;
  while (count_bytes != length){
    copy_track_head->event = malloc(sizeof(event_t));
    assert(copy_track_head->event);
    copy_track_head->event = parse_event(read_file);
    length = length + get_size(copy_track_head->event);
    //TODO: Get parse_event to work, and find a way to figure out the size of the file
    if (count_bytes != length){
      copy_track_head->next_event = malloc(sizeof(event_node_t));
      assert(copy_track_head->next_event);
      copy_track_head = copy_track_head->next_event;
    }
  }

  track_node_t *copy_song_head = midi_song->track_list;
  while (copy_song_head->track != NULL){
    copy_song_head = copy_song_head->next_track;
  }
  copy_song_head->track = new_track;
  return; */
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

uint32_t get_size(event_t *event){
  uint32_t event_size = 0;
  uint32_t delta_time = event->delta_time;
  uint8_t check_size = delta_time;
  do {
    check_size = delta_time;
    delta_time = delta_time >> 8;
    event_size = event_size + sizeof(uint8_t);
  } while (check_size != 0 || delta_time != 0);
  switch(event->type){
    case SYS_EVENT_T:
      break;
    case META_EVENT_T:
      break;
    case MIDI_EVENT_T:
      break;
  }
  return 0;
}


/* Define end_swap_16 here */
uint16_t end_swap_16(uint8_t number[2]){
  return ((number[1] << 8) | (number[0]));
}
/* Define end_swap_32 here */
uint32_t end_swap_32(uint8_t number[4]){
  return ((number[3] << 24) | (number[2] << 16) | (number[1] << 8) | (number[0]));
}

uint16_t endian_swap_16(uint16_t num_16){
  uint8_t nums_8[2] = {num_16 >> 8, num_16};
  return end_swap_32(nums_8);
}


uint32_t endian_swap_32(uint32_t num_32){
  uint8_t nums_8[4] = {num_32 >> 24, num_32 >> 16, num_32 >> 8, num_32};
  return end_swap_32(nums_8);
}

void print_binary(int number){
  if (number > 1)
    print_binary(number / 2);
  printf("%d", number % 2);
}

int main(){
  char read_file_name[50];
  printf("Provide file name: \n");
  scanf("%s", read_file_name);
  printf("Reading file...\n");
  song_data_t *midi_song = parse_file(read_file_name);
  printf("Reading complete!\n");
  printf("Writing to output.mid....\n");
  write_song_data(midi_song, "output.mid");
  printf("Writing complete!\n");
  return 0;
}
