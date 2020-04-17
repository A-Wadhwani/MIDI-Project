/* Aryan Wadhwani, ui.c, CS 24000, Spring 2020
 * Last updated April 16, 2020
 */

/* Included Libraries */

#include "ui.h"

#include<string.h>
#include<malloc.h>

tree_node_t *g_current_node = NULL;
song_data_t *g_current_song = NULL;
song_data_t *g_modified_song = NULL;

/* Define update_song_list here */

void update_song_list(){
  return;
}

/* Define update_drawing_area here */

void update_drawing_area(){
  return;
}

/* Define update_info here */

void update_info(){
  return;
}

/* Define update_song here */

void update_song(){
  return;
}


/* Define range_of_song here */

void range_of_song(song_data_t *midi_song, int *low_pitch,
                   int *high_pitch, int *length){
  return;
}

/* Define activate here */
/*
void activate(){

}

*/
/* Define add_song_cb here */

/* Define load_songs_cb here */

/* Define song_selected_cb here */

/* Define search_bar_cb here */

/* Define time_scale_cb here */

/* Define draw_cb here */

/* Define warp_time_cb here */

/* Define song_octave_cb here */

/* Define instrument_map_cb here */

/* Define note_map_cb here */

/* Define save_song_cb here */

/* Define remove_song_cb here */

/*
 * Function called prior to main that sets up the instrument to color mapping
 */

void build_color_palette()
{
  static GdkRGBA palette[16];	

  memset(COLOR_PALETTE, 0, sizeof(COLOR_PALETTE));
  char* color_specs[] = {
    // Piano, red
    "#ff0000",
    // Chromatic percussion, brown
    "#8b4513",
    // Organ, purple
    "#800080",
    // Guitar, green
    "#00ff00",
    // Bass, blue
    "#0000ff",
    // Strings, cyan
    "#00ffff",
    // Ensemble, teal
    "#008080",
    // Brass, orange
    "#ffa500",
    // Reed, magenta
    "#ff00ff",
    // Pipe, yellow
    "ffff00",
    // Synth lead, indigo
    "#4b0082",
    // Synth pad, dark slate grar
    "#2f4f4f",
    // Synth effects, silver
    "#c0c0c0",
    // Ehtnic, olive
    "#808000",
    // Percussive, silver
    "#c0c0c0",
    // Sound effects, gray
    "#808080",
  };

  for (int i = 0; i < 16; ++i) {
    gdk_rgba_parse(&palette[i], color_specs[i]);
    for (int j = 0; j < 8; ++j) {
      COLOR_PALETTE[i * 8 + j] = &palette[i];
    }
  }
} /* build_color_palette() */
