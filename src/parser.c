/* Aryan Wadhwani, parser.c, CS 24000, Spring 2020
 * Last updated March 27, 2020
 */

/* Add any includes here */
#include "parser.h"
#include "song_writer.h"
#include "event_tables.h"

#include <malloc.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

/* Function Declarations */
uint32_t endian_swap_32(uint32_t);
uint16_t endian_swap_16(uint16_t);
// uint32_t get_event_size(event_t *);
bool end_of_track(FILE *);
bool is_status(uint8_t);
void print_binary(int);

/* Global Variables */
uint8_t prev_status = 0x00;

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
  
  midi_song->track_list = NULL;
  
  for(int i = 0; i < midi_song->num_tracks; i++){
    parse_track(read_file, midi_song);
  }
  assert(feof(read_file));
  fclose(read_file);
  read_file = NULL;
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
  chunk_type = NULL;

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
    midi_song->division.frames_per_sec = read_division >> 8;
    midi_song->division.ticks_per_frame = read_division;
  }
  return;
} /* parse_header() */

/* Define parse_track here */
void parse_track(FILE *read_file, song_data_t *midi_song){
  track_t *new_track = malloc(sizeof(track_t));
  assert(new_track);

  char *chunk_type = malloc(4 * sizeof(char));
  assert(chunk_type);
  
  int check_error = fread(chunk_type, 4 * sizeof(char), 1, read_file);
  assert(check_error == 1);
  assert(strcmp(chunk_type, "MTrk") == 0);
  free(chunk_type);
  chunk_type = NULL;

  uint32_t length = 0;
  check_error = fread(&length, sizeof(length), 1, read_file);
  assert(check_error == 1);
  length = endian_swap_32(length);
  new_track->length = length;

  new_track->event_list = malloc(sizeof(event_node_t));
  assert(new_track->event_list);
  event_node_t *copy_track_head = new_track->event_list;
  
  while (!end_of_track(read_file)){
    copy_track_head->event = malloc(sizeof(event_t));
    assert(copy_track_head->event);
    copy_track_head->event = parse_event(read_file);
    if (!end_of_track(read_file)){
      copy_track_head->next_event = malloc(sizeof(event_node_t));
      assert(copy_track_head->next_event);
      copy_track_head = copy_track_head->next_event;
    }
    else {
      copy_track_head->next_event = NULL;
    }
  }

  track_node_t *copy_song_head = midi_song->track_list;
  if (copy_song_head == NULL){
    midi_song->track_list = malloc(sizeof(track_node_t));
    midi_song->track_list->track = new_track;
    return;
  }
  while (copy_song_head->next_track != NULL){
    copy_song_head = copy_song_head->next_track;
  }
  copy_song_head->next_track = malloc(sizeof(track_node_t));
  assert(copy_song_head->next_track);
  copy_song_head = copy_song_head->next_track;
  copy_song_head->track = new_track;
  return;
}

/* Define parse_event here */
event_t *parse_event(FILE *read_file){
  event_t *new_event = malloc(sizeof(event_t));
  assert(new_event);
  new_event->delta_time = parse_var_len(read_file);

  uint8_t read_type = 0x00;
  int check_error = fread(&read_type, sizeof(uint8_t), 1, read_file);
  assert(check_error == 1);
  new_event->type = read_type;
  
  switch(event_type(new_event)){
    case SYS_EVENT_T:
      new_event->sys_event = parse_sys_event(read_file, read_type);
      printf("SYS_EVENT called\n");
      break;
    case META_EVENT_T:
      new_event->meta_event = parse_meta_event(read_file);
      break;
    case MIDI_EVENT_T:
      new_event->midi_event = parse_midi_event(read_file, read_type);
      break;
  }
  return new_event;
}

/* Define parse_sys_event here */
sys_event_t parse_sys_event(FILE *read_file, uint8_t type){
  sys_event_t sys_event = (sys_event_t) {0, NULL};
  
  sys_event.data_len = parse_var_len(read_file);
  sys_event.data = malloc(sys_event.data_len * sizeof(uint8_t));
  assert(sys_event.data);

  int check_error = fread(sys_event.data,
                      sys_event.data_len * sizeof(uint8_t),
                      1, read_file);
  assert(check_error == 1);
  return sys_event;
}

/* Define parse_meta_event here */
meta_event_t parse_meta_event(FILE *read_file){
  build_event_tables();
  meta_event_t meta_event = (meta_event_t) {"\0", 0, NULL};
  uint8_t read_type = 0x00;

  int check_error = fread(&read_type, sizeof(uint8_t), 1, read_file);
  assert(check_error == 1);

  meta_event = META_TABLE[read_type];
  assert(strcmp(meta_event.name, "") != 0);
  if (meta_event.data_len != 0){
    assert(parse_var_len(read_file) == meta_event.data_len);
  } else {
    meta_event.data_len = parse_var_len(read_file);
  }

  if (meta_event.data_len == 0){
    return meta_event;
  }

  meta_event.data = malloc(meta_event.data_len * sizeof(uint8_t));
  assert(meta_event.data);
  check_error = fread(meta_event.data, 
                      meta_event.data_len * sizeof(uint8_t),
                      1, read_file);
  assert(check_error == 1);
  return meta_event;
}

/* Define parse_midi_event here */
midi_event_t parse_midi_event(FILE *read_file, uint8_t read_status){
  midi_event_t midi_event = (midi_event_t) {"\0", 0, 0, NULL};
  if (is_status(read_status)){
    midi_event.status = read_status;
  } else {
    midi_event.status = prev_status;
    fseek(read_file, -1 * sizeof(char), SEEK_CUR);
  }
  midi_event = MIDI_TABLE[midi_event.status];
  assert(strcmp(midi_event.name, "") != 0);

  if(midi_event.data_len == 0){
    return midi_event;
  }

  midi_event.data = malloc(midi_event.data_len * sizeof(uint8_t));
  assert(midi_event.data);
  int check_error = fread(midi_event.data, midi_event.data_len * sizeof(uint8_t),
                          1, read_file);
  assert(check_error == 1);
  return midi_event;
}

/* Define parse_var_len here */
uint32_t parse_var_len(FILE *read_file){
  uint32_t parsed_val = 0;
  uint8_t read;
  int count = 0;
  int check_error = 0;
  do {
    check_error = fread(&read, sizeof(uint8_t), 1, read_file);
    assert(check_error == 1);
    if (read > 127){
      read = read - 128;
      parsed_val = (parsed_val << 7) + read;
    }
    else { 
      parsed_val = (parsed_val << 7) + read;
      return parsed_val;
    }
    count++;
  } while(count != 4);
  return parsed_val;
}

/* Define event_type here */
uint8_t event_type(event_t *event){
  switch(event->type){
    case SYS_EVENT_1:
    case SYS_EVENT_2:
      return SYS_EVENT_T;
    case META_EVENT:
      return META_EVENT_T;
    default:
      return MIDI_EVENT_T;
  }
}

/* Define free_song here */
void free_song(song_data_t *midi_song){
  if (midi_song == NULL){
    return;
  }
  free(midi_song->path);
  midi_song->path = NULL;
  while(midi_song->track_list != NULL){
    track_node_t *copy_node = midi_song->track_list;
    midi_song->track_list = copy_node->next_track;
    free_track_node(copy_node);
    copy_node = NULL;
  }
  midi_song->track_list = NULL;
  free(midi_song);
}

/* Define free_track_node here */
void free_track_node(track_node_t *track_node){
  while(track_node->track->event_list != NULL){
    event_node_t *copy_node = track_node->track->event_list;
    track_node->track->event_list = copy_node->next_event;
    free_event_node(copy_node);
    copy_node = NULL;
  }
  track_node->next_track = NULL;
  free(track_node->track);
  free(track_node);
}

/* Define free_event_node here */
void free_event_node(event_node_t *event_node){
  free(event_node->event);
  event_node->event = NULL;
  event_node->next_event = NULL;
  free(event_node);
  return;
}

bool end_of_track(FILE *read_file){ 
  if(feof(read_file)){
    return true;
  }
  char *chunk_type = malloc(4 * sizeof(char));
  assert(chunk_type);
  int check_error = fread(chunk_type, 4 * sizeof(char), 1, read_file);
  if (check_error == 0){
    return true;
  }
  fseek(read_file, -4 * sizeof(char), SEEK_CUR);
  bool is_end = strcmp(chunk_type, "MTrk") == 0;
  assert(check_error == 1);
  free(chunk_type);
  chunk_type = NULL;
  return is_end;
}

/* 
uint32_t get_var_len_size(uint32_t number){
  uint32_t var_len_size = 0;
  uint8_t check_size = number;
  do {
    check_size = number;
    number = number >> 8;
    var_len_size = var_len_size + sizeof(uint8_t);
  } while (check_size > 127 || number != 0);
  return var_len_size;
}

uint32_t get_event_size(event_t *event){
  uint32_t event_size = 2 * sizeof(uint8_t) + get_var_len_size(event->delta_time);
  switch(event_type(event)){
    case SYS_EVENT_T:
      event_size = event_size + get_var_len_size(event->sys_event.data_len) +
                   event->sys_event.data_len * sizeof(uint8_t);
      break;
    case META_EVENT_T:
      event_size = event_size + get_var_len_size(event->meta_event.data_len) +
                   event->meta_event.data_len * sizeof(uint8_t);
      break;
    case MIDI_EVENT_T:
      if (is_status(event->is_status()))
      event_size = event_size +
                   event->midi_event.data_len * sizeof(uint8_t);
      break;
  }
  return event_size;
} */

/* Define end_swap_16 here */
uint16_t end_swap_16(uint8_t number[2]){
  return ((number[1] << 8) | (number[0]));
}
/* Define end_swap_32 here */
uint32_t end_swap_32(uint8_t number[4]){
  uint32_t num = number[3] << 24;
  return (num | (number[2] << 16) | (number[1] << 8) | number[0]);
}

uint16_t endian_swap_16(uint16_t num_16){
  uint8_t nums_8[2] = {num_16 >> 8, num_16};
  return end_swap_32(nums_8);
}

bool is_status(uint8_t status){
  if (status >> 7 == 1){
    prev_status = status;
    return true;
  } else {
    return false;
  }
}

uint32_t endian_swap_32(uint32_t num_32){
  uint8_t nums_8[4] = {num_32 >> 24, num_32 >> 16, num_32 >> 8, num_32};
  return end_swap_32(nums_8);
  return num_32;
}

void print_binary(int number){
  if (number > 1)
    print_binary(number / 2);
  printf("%d", number % 2);
}

void verify_song(song_data_t *midi_song){
  printf("Header details Format: %u, Num Tracks: %u, Division: %u\n", 
         midi_song->format, midi_song->num_tracks, midi_song->division.ticks_per_qtr);
  track_node_t *copy_track = midi_song->track_list;
  for(int i = 0; i < midi_song->num_tracks; i++){
    printf("Track %u: Length %u\n",i, copy_track->track->length);
    event_node_t *copy_event = copy_track->track->event_list;
    while(copy_event != NULL){
      printf("  Event type %u, Delta time %u\n",
             event_type(copy_event->event), copy_event->event->delta_time);
      copy_event = copy_event->next_event;
    }
    copy_track = copy_track->next_track;
  }
  printf("End of read\n");
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
  free_song(midi_song);
  return 0;
}
