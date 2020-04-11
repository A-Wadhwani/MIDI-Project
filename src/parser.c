/* Aryan Wadhwani, parser.c, CS 24000, Spring 2020
 * Last updated March 27, 2020
 */

/* Included Libraries */

#include "parser.h"

#include <malloc.h>
#include <assert.h>
#include <string.h>

/* Defined Constants */

#define MIDI_HEADER "MThd"
#define MIDI_TRACK "MTrk"
#define OK_CHUNK_READ (1)
#define HEADER_LEN (6)

/* Function Declarations */

uint32_t endian_swap_32(uint32_t);
uint16_t endian_swap_16(uint16_t);
bool check_midi_status(uint8_t);
bool end_of_track(FILE *);

/* Global Variables */

uint8_t g_prev_midi_status = 0;

/* Function Definitions */

/*
 * Dynamically allocates memory for a song_data_t struct
 * and parses a .mid binary file and stores it's information
 * into the struct.
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
  midi_song->track_list = NULL;

  for (int i = 0; i < midi_song->num_tracks; i++){
    parse_track(read_file, midi_song);
  }

  assert(feof(read_file));
  fclose(read_file);
  read_file = NULL;
  return midi_song;
} /* parse_file() */

/*
 * Reads the midi header information into the
 * song_data_t struct
 */

void parse_header(FILE *read_file, song_data_t *midi_song){
  char *chunk_type = malloc(strlen(MIDI_HEADER) * sizeof(char));
  assert(chunk_type);

  int check_error = fread(chunk_type, strlen(MIDI_HEADER) *
                          sizeof(char), 1, read_file);

  assert(strncmp(chunk_type, MIDI_HEADER, strlen(MIDI_HEADER)) == 0);
  assert(check_error == OK_CHUNK_READ);

  free(chunk_type);
  chunk_type = NULL;

  uint32_t length = 0;
  check_error = fread(&length, sizeof(length), 1, read_file);
  assert(check_error == OK_CHUNK_READ);
  length = endian_swap_32(length);
  assert(length == HEADER_LEN);

  uint16_t format = 0;
  check_error = fread(&format, sizeof(format), 1, read_file);
  assert(check_error == OK_CHUNK_READ);
  format = endian_swap_16(format);
  assert((format == 0) || (format == 1) || (format == 2));
  midi_song->format = format;

  uint16_t num_tracks = 0;
  check_error = fread(&num_tracks, sizeof(num_tracks),
                      1, read_file);
  assert(check_error == OK_CHUNK_READ);
  num_tracks = endian_swap_16(num_tracks);
  midi_song->num_tracks = num_tracks;

  uint16_t read_division = 0;
  check_error = fread(&read_division, sizeof(read_division),
                      1, read_file);
  assert(check_error == OK_CHUNK_READ);
  read_division = endian_swap_16(read_division);

  midi_song->division.uses_tpq = ((read_division >> 15) == 0);
  if (read_division >> 15 == 0){
    midi_song->division.ticks_per_qtr = read_division;
  } else {
    midi_song->division.frames_per_sec = read_division >> 8;
    midi_song->division.ticks_per_frame = read_division;
  }
  return;
} /* parse_header() */

/*
 * Parses track and events that are a part of the
 * track from the .mid file, and stores them in
 * the song_data_t struct
 */

void parse_track(FILE *read_file, song_data_t *midi_song){
  track_t *new_track = malloc(sizeof(track_t));
  assert(new_track);

  char *chunk_type = malloc(strlen(MIDI_TRACK) * sizeof(char));
  assert(chunk_type);

  int check_error = fread(chunk_type, strlen(MIDI_TRACK) *
                          sizeof(char), 1, read_file);
  assert(check_error == OK_CHUNK_READ);
  assert(strncmp(chunk_type, MIDI_TRACK, strlen(MIDI_TRACK)) == 0);

  free(chunk_type);
  chunk_type = NULL;

  uint32_t length = 0;
  check_error = fread(&length, sizeof(length),
                          1, read_file);
  assert(check_error == OK_CHUNK_READ);
  length = endian_swap_32(length);
  new_track->length = length;

  new_track->event_list = malloc(sizeof(event_node_t));
  event_node_t *copy_track_head = new_track->event_list;

  while (!end_of_track(read_file)){
    copy_track_head->event = parse_event(read_file);

    if (!end_of_track(read_file)){
      copy_track_head->next_event = malloc(sizeof(event_node_t));
      copy_track_head = copy_track_head->next_event;
    }
    else {
      copy_track_head->next_event = NULL;
      break;
    }
  }

  track_node_t *copy_song_head = midi_song->track_list;
  if (copy_song_head == NULL){
    midi_song->track_list = malloc(sizeof(track_node_t));
    midi_song->track_list->track = new_track;
    midi_song->track_list->next_track = NULL;
    return;
  }

  while (copy_song_head->next_track != NULL){
    copy_song_head = copy_song_head->next_track;
  }

  copy_song_head->next_track = malloc(sizeof(track_node_t));
  assert(copy_song_head->next_track);

  copy_song_head = copy_song_head->next_track;
  copy_song_head->track = new_track;
  copy_song_head->next_track = NULL;
  return;
} /* parse_track() */

/*
 * Reads a MTrk event from file and returns a pointer
 * to the dynamically allocated memory of the event
 */

event_t *parse_event(FILE *read_file){
  event_t *new_event = malloc(sizeof(event_t));
  new_event->delta_time = parse_var_len(read_file);

  uint8_t read_type = 0;
  int check_error = fread(&read_type, sizeof(uint8_t),
                          1, read_file);
  assert(check_error == OK_CHUNK_READ);
  new_event->type = read_type;

  switch (event_type(new_event)){
    case SYS_EVENT_T:
      new_event->sys_event = parse_sys_event(read_file, read_type);
      break;
    case META_EVENT_T:
      new_event->meta_event = parse_meta_event(read_file);
      break;
    case MIDI_EVENT_T:
      new_event->midi_event = parse_midi_event(read_file, read_type);
      break;
  }
  return new_event;
} /* parse_event() */

/*
 * Handles parsing of a sys_event from a file.
 */

sys_event_t parse_sys_event(FILE *read_file, uint8_t type){
  sys_event_t sys_event = (sys_event_t) {0, NULL};

  sys_event.data_len = parse_var_len(read_file);
  sys_event.data = malloc(sys_event.data_len * sizeof(uint8_t));

  int check_error = fread(sys_event.data, sys_event.data_len *
                          sizeof(uint8_t), 1, read_file);
  assert(check_error == OK_CHUNK_READ);
  return sys_event;
} /* parse_sys_event() */

/*
 * Handles parsing of a meta_event from a file.
 */

meta_event_t parse_meta_event(FILE *read_file){
  build_event_tables();
  meta_event_t meta_event = (meta_event_t) {"\0", 0, NULL};
  uint8_t read_type = 0;

  int check_error = fread(&read_type, sizeof(uint8_t), 1, read_file);
  assert(check_error == OK_CHUNK_READ);

  meta_event = META_TABLE[read_type];
  assert(meta_event.name != NULL);

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
  check_error = fread(meta_event.data, meta_event.data_len *
                      sizeof(uint8_t), 1, read_file);
  assert(check_error == OK_CHUNK_READ);
  return meta_event;
} /* parse_meta_event() */

/*
 * Handles parsing of a midi event from a file
 */

midi_event_t parse_midi_event(FILE *read_file, uint8_t read_status){
  midi_event_t midi_event = (midi_event_t) {"\0", 0, 0, NULL};
  if (check_midi_status(read_status)){
    midi_event.status = read_status;
  } else {
    midi_event.status = g_prev_midi_status;
    fseek(read_file, -1 * sizeof(char), SEEK_CUR);
  }
  midi_event = MIDI_TABLE[midi_event.status];
  assert(strncmp(midi_event.name, "", 1) != 0);

  if (midi_event.data_len == 0){
    return midi_event;
  }

  midi_event.data = malloc(midi_event.data_len * sizeof(uint8_t));
  assert(midi_event.data);
  int check_error = fread(midi_event.data, midi_event.data_len *
                          sizeof(uint8_t), 1, read_file);
  assert(check_error == OK_CHUNK_READ);
  return midi_event;
} /* parse_midi_event() */

/*
 * Returns the length of the variable
 * sized uint32_t
 */

uint32_t parse_var_len(FILE *read_file){
  uint32_t parsed_val = 0;
  uint8_t read_num = 0;
  int count = 0;
  int check_error = 0;
  do {
    check_error = fread(&read_num, sizeof(uint8_t), 1, read_file);
    assert(check_error == OK_CHUNK_READ);
    if (read_num > 127){
      read_num = read_num - 128;
      parsed_val = (parsed_val << 7) + read_num;
    }
    else {
      parsed_val = (parsed_val << 7) + read_num;
      return parsed_val;
    }
    count++;
  } while (count != 4);
  return parsed_val;
} /* parse_var_len() */

/*
 * Returns the type of event
 */

uint8_t event_type(event_t *event){
  switch (event->type){
    case SYS_EVENT_1:
      return SYS_EVENT_T;
    case SYS_EVENT_2:
      return SYS_EVENT_T;
    case META_EVENT:
      return META_EVENT_T;
    default:
      return MIDI_EVENT_T;
  }
} /* event_type() */

/*
 * Frees all memory related to a song
 */

void free_song(song_data_t *midi_song){
  if (midi_song == NULL){
    return;
  }
  free(midi_song->path);
  midi_song->path = NULL;
  while (midi_song->track_list != NULL){
    track_node_t *copy_node = midi_song->track_list;
    midi_song->track_list = copy_node->next_track;
    free_track_node(copy_node);
    copy_node = NULL;
  }
  free(midi_song);
  midi_song = NULL;
} /* free_song() */

/*
 * Frees all memory related to a track
 */

void free_track_node(track_node_t *track_node){
  while (track_node->track->event_list != NULL){
    event_node_t *copy_node = track_node->track->event_list;
    track_node->track->event_list = copy_node->next_event;
    free_event_node(copy_node);
    copy_node = NULL;
  }
  free(track_node->track);
  track_node->track = NULL;
  free(track_node);
  track_node = NULL;
} /* free_track_node() */

/*
 * Frees all memory related to an event
 */

void free_event_node(event_node_t *event_node){
  switch (event_type(event_node->event)){
    case META_EVENT_T:
      free(event_node->event->meta_event.data);
      event_node->event->meta_event.data = NULL;
      break;
    case MIDI_EVENT_T:
      free(event_node->event->midi_event.data);
      event_node->event->midi_event.data = NULL;
      break;
    case SYS_EVENT_T:
      free(event_node->event->sys_event.data);
      event_node->event->sys_event.data = NULL;
  }
  free(event_node->event);
  event_node->event = NULL;
  free(event_node);
  event_node = NULL;
  return;
} /* free_event_node() */

/*
 * Returns true if the file has reached
 * the end of the track.
 */

bool end_of_track(FILE *read_file){
  if (feof(read_file)){
    return true;
  }
  char *chunk_type = malloc(sizeof(char) * 4);
  int check_error = fread(chunk_type, sizeof(char), 4, read_file);
  if (check_error != 4 * OK_CHUNK_READ){
    free(chunk_type);
    chunk_type = NULL;
    return true;
  }
  bool is_end = strncmp(chunk_type, "MTrk", strlen("MTrk")) == 0;
  free(chunk_type);
  chunk_type = NULL;
  fseek(read_file, -4 * sizeof(char), SEEK_CUR);
  return is_end;
} /* end_of_track() */

/*
 * Creates a uint16_t by endian swapping
 * a uint8_t array's elements
 */

uint16_t end_swap_16(uint8_t number[2]){
  return ((number[1] << 8) | (number[0]));
} /* end_swap_16() */

/*
 * Creates a uint32_t by endian swapping
 * a uint8_t array's elements
 */

uint32_t end_swap_32(uint8_t number[4]){
  uint32_t num = number[3] << 24;
  return (num | (number[2] << 16) | (number[1] << 8) | number[0]);
} /* end_swap_32() */

/*
 * Creates an array of uint8_ts from a
 * uint32_t to be used in end_swap_16
 */

uint16_t endian_swap_16(uint16_t num_16){
  uint8_t nums_8[2] = {num_16 >> 8, num_16};
  return end_swap_32(nums_8);
} /* endian_swap_16() */

/*
 * Creates an array of uint8_ts from a
 * uint32_t to be used in end_swap_32
 */

uint32_t endian_swap_32(uint32_t num_32){
  uint8_t nums_8[4] = {num_32 >> 24, num_32 >> 16, num_32 >> 8, num_32};
  return end_swap_32(nums_8);
} /* endian_swap_32() */

/*
 * Checks if the given midi status
 * is of the correct format
 */

bool check_midi_status(uint8_t status){
  if (status >> 7 == 1){
    g_prev_midi_status = status;
    return true;
  } else {
    return false;
  }
} /* check_midi_status() */
