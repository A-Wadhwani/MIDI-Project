/* Aryan Wadhwani, alterations.c, CS 24000, Spring 2020
 * Last updated April 16, 2020
 */

/* Included Libraries */

#include "alterations.h"

#include<assert.h>
#include<malloc.h>

#include "song_writer.h"

/* Function Declarations */

int find_vlq_length(uint32_t);
track_node_t *find_track(song_data_t *, int);
track_node_t *duplicate_track(track_node_t *);
/* Function Definitions */

/*Define change_event_octave here */

int change_event_octave(event_t *song_event, int *num_octaves){
  if (event_type(song_event) != MIDI_EVENT_T){
    return 0;
  }
  midi_event_t midi_event = song_event->midi_event;
  if ((midi_event.status >= 0x80) && (midi_event.status <= 0xAF)){
    uint8_t note = midi_event.data[0];
    uint8_t note_number = note % 12;
    uint8_t octave_number = (note / 12) - 1;
    note = note_number + (octave_number + *num_octaves + 1) * 12;
    if (note > 127 || note < 0){
      return 0;
    }
    midi_event.data[0] = note;
    song_event->midi_event = midi_event;
    return 1;
  }
  return 0;
}

/*Define change_event_time here */

int change_event_time(event_t *song_event, float *delta_time){
  float new_delta = song_event->delta_time;
  new_delta = new_delta * (*delta_time);
  int count_new = find_vlq_length(new_delta);
  int count_old = find_vlq_length(song_event->delta_time);
  song_event->delta_time = (uint32_t) new_delta;
  return count_new - count_old;
}

/*Define change_event_instrument here */

int change_event_instrument(event_t *song_event, remapping_t mapped_inst){
  if (event_type(song_event) != MIDI_EVENT_T){
    return 0;
  }
  midi_event_t midi_event = song_event->midi_event;
  if (midi_event.status >= 0xC0 && midi_event.status <= 0xCF){
    midi_event.data[0] = mapped_inst[midi_event.data[0]];
    song_event->midi_event = midi_event;
    return 1;
  }
  return 0;
}

/*Define change_event_note here */

int change_event_note(event_t *song_event, remapping_t mapped_notes){
  if (event_type(song_event) != MIDI_EVENT_T){
    return 0;
  }
  midi_event_t midi_event = song_event->midi_event;
  if (midi_event.status >= 0x80 && midi_event.status <= 0xAF){
    midi_event.data[0] = mapped_notes[midi_event.data[0]];
    song_event->midi_event = midi_event;
    return 1;
  }
  return 0;
}

/*Define apply_to_events here */

int apply_to_events(song_data_t *midi_song, event_func_t event_mod, void *data){
  assert(midi_song);
  int sum = 0;
  track_node_t *track_list = midi_song->track_list;
  while(track_list != NULL){
    event_node_t *event_list = track_list->track->event_list;
    while (event_list != NULL){
      sum = sum + event_mod(event_list->event, data);
      event_list = event_list->next_event;
    }
    track_list = track_list->next_track;
  }
  return sum;
}

/*Define change_octave here */

int change_octave(song_data_t *midi_song, int num_octaves){
  return apply_to_events(midi_song, (event_func_t)(change_event_octave), &num_octaves);
}

/*Define warp_time here */

int warp_time(song_data_t *midi_song, float delta_time){
  assert(midi_song);
  int sum = 0;
  track_node_t *track_list = midi_song->track_list;
  while(track_list != NULL){
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
}

/*Define remap_instruments here */

int remap_instruments(song_data_t *midi_song, remapping_t mapped_inst){
  return apply_to_events(midi_song, (event_func_t)(change_event_instrument), mapped_inst);
}

/*Define remap_notes here */

int remap_notes(song_data_t *midi_song, remapping_t mapped_notes){
  return apply_to_events(midi_song, (event_func_t)(change_event_note), mapped_notes);
}

/*Define add_round here */

void add_round(song_data_t *midi_song, int track_index, int octave_diff,
               unsigned int time_delay, uint8_t instrument){
  assert(midi_song);
  assert(midi_song->format != 2);
  track_node_t *found_track = find_track(midi_song, track_index);
  assert(found_track);
  track_node_t *copy_track = duplicate_track(found_track);
  event_node_t *copy_event = copy_track->track->event_list;
  while(copy_event != NULL){
    //TODO: Create Channel Changer
    change_event_octave(copy_event->event, &octave_diff);
    int old_length = find_vlq_length(copy_event->event->delta_time);
    copy_event->event->delta_time -= time_delay;
    int new_length = find_vlq_length(copy_event->event->delta_time);
    copy_track->track->length += new_length - old_length;
    remapping_t new_instrument = {0};
    for(int i = 0; i < 0xFF; i++){
      new_instrument[i] = instrument;
    }
    change_event_instrument(copy_event->event, new_instrument);
    copy_event = copy_event->next_event;
  }
  track_node_t *copy_track_head = midi_song->track_list;
  while(copy_track_head->next_track != NULL){
    copy_track_head = copy_track_head->next_track;
  }
  copy_track_head->next_track = copy_track;
  midi_song->num_tracks += 1;
  midi_song->format = 1;
}

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
    switch(event_type(copy_event->event)){
      case MIDI_EVENT_T:
        new_event->event->midi_event = copy_event->event->midi_event;
        break;
      case META_EVENT_T:
        new_event->event->meta_event = copy_event->event->meta_event;
        break;
      case SYS_EVENT_T:
        new_event->event->sys_event = copy_event->event->sys_event;
        break;
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
  } while(copy_event != NULL);
  return new_track;
}

track_node_t *find_track(song_data_t *midi_song, int track_index){
  track_node_t *copy_head = midi_song->track_list;
  for(int i = 1; i <= track_index; i++){
    if (copy_head == NULL){
      return NULL;
    }
    copy_head = copy_head->next_track;
  }
  return copy_head;
}

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
    N_LOWER[i] = i-1;
  }
  //  Swap F# for G
  for (int i = 6; i <= 0xFF; i += 12) {
    N_LOWER[i] = i+1;
  }
} /* build_mapping_tables() */

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
}

int main(){
  build_mapping_tables();
  char file_name[300];
  printf("Enter file_name: ");
  scanf("%[^\n]", file_name);
  song_data_t *new_song = parse_file(file_name);
  //change_octave(new_song, 1);
  //warp_time(new_song, 0.75);
  remap_notes(new_song, N_LOWER);
  //remap_instruments(new_song, I_BRASS_BAND);
  write_song_data(new_song, "music/demo.mid");
}
