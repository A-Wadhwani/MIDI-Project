/* Aryan Wadhwani, alterations.c, CS 24000, Spring 2020
 * Last updated April 16, 2020
 */

/* Included Libraries */

#include "alterations.h"

#include <assert.h>
#include <malloc.h>
#include <string.h>

#include "song_writer.h"

/* Function Declarations */

int get_event_octave(event_node_t *);
int find_vlq_length(uint32_t);
int find_lowest_channel(track_node_t *);
track_node_t *find_track(song_data_t *, int);
track_node_t *duplicate_track(track_node_t *);

/* Function Definitions */

/*
 * Alters the octaves of the given event
 */

int change_event_octave(event_t *song_event, int *num_octaves){
  if (event_type(song_event) != MIDI_EVENT_T){
    return 0;
  }
  midi_event_t midi_event = song_event->midi_event;
  if ((strcmp(midi_event.name, "Note Off") == 0) ||
      (strcmp(midi_event.name, "Note On") == 0) ||
      (strcmp(midi_event.name, "Polyphonic Key") == 0)){
    uint8_t note = midi_event.data[0];
    note = note + ((*num_octaves) * 12);
    if ((note > 127) || (note < 0)){
      return 0;
    }
    midi_event.data[0] = note;
    song_event->midi_event = midi_event;
    if ((*num_octaves) == 0){
      return 0;
    }
    return 1;
  }
  return 0;
} /* change_event_octave() */

/*
 * Alters the playback speed of the given
 * event.
 */

int change_event_time(event_t *song_event, float *delta_time){
  float new_delta = song_event->delta_time;
  new_delta = new_delta * (*delta_time);
  int count_new = find_vlq_length(new_delta);
  int count_old = find_vlq_length(song_event->delta_time);
  song_event->delta_time = (uint32_t) new_delta;
  return count_new - count_old;
 } /* change_event_time() */

/*
 * Changes the instrument played by the given
 * event.
 */

int change_event_instrument(event_t *song_event, remapping_t mapped_inst){
  if (event_type(song_event) != MIDI_EVENT_T){
    return 0;
  }
  midi_event_t midi_event = song_event->midi_event;
  if ((midi_event.status >= 0xC0) && (midi_event.status <= 0xCF)){
    midi_event.data[0] = mapped_inst[midi_event.data[0]];
    song_event->midi_event = midi_event;
    return 1;
  }
  return 0;
} /* change_event_instrument() */

/*
 * Changes the note of the given event.
 */

int change_event_note(event_t *song_event, remapping_t mapped_notes){
  if (event_type(song_event) != MIDI_EVENT_T){
    return 0;
  }
  midi_event_t midi_event = song_event->midi_event;
  if ((midi_event.status >= 0x80) && (midi_event.status <= 0xAF)){
    midi_event.data[0] = mapped_notes[midi_event.data[0]];
    song_event->midi_event = midi_event;
    return 1;
  }
  return 0;
} /* change_event_note() */

/*
 * Calls the passed-in function pointer
 * to every event present in the given
 * song structure
 */

int apply_to_events(song_data_t *midi_song, event_func_t event_mod, void *data){
  assert(midi_song);
  int sum = 0;
  track_node_t *track_list = midi_song->track_list;
  while (track_list != NULL){
    event_node_t *event_list = track_list->track->event_list;
    while (event_list != NULL){
      sum = sum + event_mod(event_list->event, data);
      event_list = event_list->next_event;
    }
    track_list = track_list->next_track;
  }
  return sum;
} /* apply_to_events() */

/*
 * Changes the octave range of the song.
 */

int change_octave(song_data_t *midi_song, int num_octaves){
  return apply_to_events(midi_song,
                        (event_func_t)(change_event_octave),
                        &num_octaves);
} /* change_octave() */

/*
 * Alters the speed of playback
 * of a song.
 */

int warp_time(song_data_t *midi_song, float delta_time){
  assert(midi_song);
  int sum = 0;
  track_node_t *track_list = midi_song->track_list;
  while (track_list != NULL){
    event_node_t *event_list = track_list->track->event_list;
    while (event_list != NULL){
      int size_difference = change_event_time(event_list->event, &delta_time);
      track_list->track->length = track_list->track->length + size_difference;
      sum = sum + size_difference;
      event_list = event_list->next_event;
    }
    track_list = track_list->next_track;
  }
  return sum;
} /* warp_time() */

/*
 * Changes the events such that all
 * notes are played through the given
 * instrument map
 */

int remap_instruments(song_data_t *midi_song, remapping_t mapped_inst){
  return apply_to_events(midi_song,
                        (event_func_t)(change_event_instrument),
                        mapped_inst);
} /* remap_instruments() */

/*
 * Remaps all notes to the notes given.
 */

int remap_notes(song_data_t *midi_song, remapping_t mapped_notes){
  return apply_to_events(midi_song,
                        (event_func_t)(change_event_note),
                        mapped_notes);
} /* remap_notes() */

/*
 * Applies modifications to the given song,
 * duplicating at the given track index.
 */

void add_round(song_data_t *midi_song, int track_index, int octave_diff,
               unsigned int time_delay, uint8_t instrument){
  assert(midi_song);
  assert(midi_song->format != 2);
  track_node_t *found_track = find_track(midi_song, track_index);
  assert(found_track);
  track_node_t *copy_track = duplicate_track(found_track);
  event_node_t *copy_event = copy_track->track->event_list;
  int channel_no = find_lowest_channel(midi_song->track_list);
  while (copy_event != NULL){
    int octave = get_event_octave(copy_event) + octave_diff;
    int is_note = change_event_octave(copy_event->event, &octave);
    copy_event->event->delta_time -= time_delay;
    int new_length = find_vlq_length(copy_event->event->delta_time);
    copy_track->track->length = new_length;
    remapping_t new_instrument = {0};
    for (int i = 0; i < 0xFF; i++){
      new_instrument[i] = instrument;
    }
    change_event_instrument(copy_event->event, new_instrument);
    if ((event_type(copy_event->event) == MIDI_EVENT_T)
        && (is_note == 1)){
      uint8_t save_status = copy_event->event->midi_event.status >> 4;
      copy_event->event->midi_event.status = (save_status << 4) & channel_no;
      copy_event->event->type = copy_event->event->midi_event.status;
    }
    copy_event = copy_event->next_event;
  }
  track_node_t *copy_track_head = midi_song->track_list;
  while (copy_track_head->next_track != NULL){
    copy_track_head = copy_track_head->next_track;
  }
  copy_track_head->next_track = copy_track;
  midi_song->num_tracks += 1;
  midi_song->format = 1;
} /* add_round() */

/*
 * Returns the octave value of the current event,
 * if it is a midi note event.
 */

int get_event_octave(event_node_t *song_event){
  if (song_event == NULL){
    return 0;
  }
  if (event_type(song_event->event) != MIDI_EVENT_T){
    return 0;
  }
  midi_event_t midi_event = song_event->event->midi_event;
  if ((strcmp(midi_event.name, "Note Off") == 0) ||
      (strcmp(midi_event.name, "Note On") == 0) ||
      (strcmp(midi_event.name, "Polyphonic Key") == 0)){
    int octave = (midi_event.data[0] / 12) - 1;
    return octave;
  }
  return 0;
} /* get_event_octave() */

/*
 * Returns the value of the lowest channel present in the file.
 */

int find_lowest_channel(track_node_t *midi_track){
  if (midi_track == NULL){
    return 0;
  }
  bool is_duplicate = false;
  int lowest_channel = 0;
  do {
    track_node_t *dup_track = midi_track;
    while (dup_track != NULL){
      event_node_t *dup_event = midi_track->track->event_list;
      while ((dup_event) &&
             (event_type(dup_event->event) != MIDI_EVENT_T)){
        dup_event = dup_event->next_event;
      }
      if (dup_event){
        if (event_type(dup_event->event) == MIDI_EVENT_T){
          uint8_t channel_no = 0x00001111 & dup_event->event->midi_event.status;
          if (channel_no == lowest_channel){
            lowest_channel++;
            is_duplicate = true;
          }
        }
      }
      dup_track = dup_track->next_track;
    }
  } while (is_duplicate);
  return lowest_channel;
} /* find_lowest_channel() */

/*
 * Returns a deep copy of the given track node.
 */

track_node_t *duplicate_track(track_node_t *given_track){
  track_node_t *new_track = malloc(sizeof(track_node_t));
  assert(new_track);
  new_track->next_track = NULL;
  new_track->track = malloc(sizeof(track_t));
  assert(new_track->track);
  new_track->track->length = given_track->track->length;
  new_track->track->event_list = NULL;
  event_node_t *copy_event = given_track->track->event_list;
  do {
    if (copy_event == NULL){
      return new_track;
    }
    event_node_t *new_event = malloc(sizeof(event_node_t));
    new_event->event = malloc(sizeof(event_t));
    new_event->next_event = NULL;
    new_event->event->type = copy_event->event->type;
    new_event->event->delta_time = copy_event->event->delta_time;
    uint8_t type = event_type(copy_event->event);
    if (type == MIDI_EVENT_T){
      midi_event_t midi_event = copy_event->event->midi_event;
      int data_len = copy_event->event->midi_event.data_len;
      if (data_len > 0){
        midi_event.data = malloc(sizeof(uint8_t) * data_len);
        for (int i = 0; i < data_len; i++){
          midi_event.data[i] = copy_event->event->midi_event.data[i];
        }
      }
      new_event->event->midi_event = midi_event;
    }
    if (type == META_EVENT_T){
      meta_event_t meta_event = copy_event->event->meta_event;
      if (meta_event.data_len > 0){
        meta_event.data = malloc(sizeof(uint8_t) * meta_event.data_len);
        for (int i = 0; i < meta_event.data_len; i++){
          meta_event.data[i] = copy_event->event->meta_event.data[i];
        }
      }
      new_event->event->meta_event = meta_event;
    }
    if (type == SYS_EVENT_T){
      sys_event_t sys_event = copy_event->event->sys_event;
      if (sys_event.data_len > 0){
        sys_event.data = malloc(sizeof(uint8_t) * sys_event.data_len);
        for (int i = 0; i < sys_event.data_len; i++){
          sys_event.data[i] = copy_event->event->sys_event.data[i];
        }
      }
      new_event->event->sys_event = sys_event;
    }
    if (new_track->track->event_list == NULL){
      new_track->track->event_list = new_event;
    }
    else {
      event_node_t *insert_event = new_track->track->event_list;
      while (insert_event->next_event != NULL){
        insert_event = insert_event->next_event;
      }
      insert_event->next_event = new_event;
    }
    copy_event = copy_event->next_event;
  } while (copy_event != NULL);
  return new_track;
} /* duplicate_track() */

/*
 * Returns a pointer to the track at the given
 * index
 */

track_node_t *find_track(song_data_t *midi_song, int track_index){
  track_node_t *copy_head = midi_song->track_list;
  for (int i = 0; i < track_index; i++){
    if (copy_head == NULL){
      return NULL;
    }
    copy_head = copy_head->next_track;
  }
  return copy_head;
} /* find_track() */

/*
 * Function called prior to main that sets up random mapping tables
 */

void build_mapping_tables()
{
  for (int i = 0; i <= 0xFF; i++) {
    I_BRASS_BAND[i] = 61;
  }

  for (int i = 0; i <= 0xFF; i++) {
    I_HELICOPTER[i] = 125;
  }

  for (int i = 0; i <= 0xFF; i++) {
    N_LOWER[i] = i;
  }
  //  Swap C# for C
  for (int i = 1; i <= 0xFF; i += 12) {
    N_LOWER[i] = i - 1;
  }
  //  Swap F# for G
  for (int i = 6; i <= 0xFF; i += 12) {
    N_LOWER[i] = i + 1;
  }
} /* build_mapping_tables() */

/*
 * Returns the number of bytes required
 * to store the given value in variable
 * length format.
 */

int find_vlq_length(uint32_t variable_len){
  int length = 1;
  if (variable_len >= 0x80){
    length++;
  }
  if (variable_len >= 0x4000){
    length++;
  }
  if (variable_len >= 0x200000){
    length++;
  }
  return length;
} /* find_vlq_length() */
