/* Aryan Wadhwani, alterations.c, CS 24000, Spring 2020
 * Last updated April 16, 2020
 */

/* Included Libraries */

#include "alterations.h"

#include<assert.h>
#include<malloc.h>

/* Function Declarations */

/* Function Definitions */

/*Define change_event_octave here */

int change_event_octave(event_t *song_event, int *num_octaves){
  if (event_type(song_event) != MIDI_EVENT_T){
    return 0;
  }
  midi_event_t midi_event = song_event->midi_event;
  if ((midi_event.status >= 0x80) && (midi_event.status <= 0xAF)){
    printf("%u\n", midi_event.data[0]);
    return 1;
  }
  return 0;
}

/*Define change_event_time here */

int change_event_time(event_t *song_event, float *delta_time){
  return 0;
}

/*Define change_event_instrument here */

int change_event_instrument(event_t *song_event, remapping_t mapped_inst){
  return 0;
}

/*Define change_event_note here */

int change_event_node(event_t *song_event, remapping_t mapped_notes){
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
  return 0;
}

/*Define warp_time here */

int warp_time(song_data_t *midi_song, float delta_time){
  return 0;
}

/*Define remap_instruments here */

int remap_instruments(song_data_t *midi_song, remapping_t mapped_inst){
  return 0;
}

/*Define remap_notes here */

int remap_notes(song_data_t *midi_song, remapping_t mapped_notes){
  return 0;
}

/*Define add_round here */

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
