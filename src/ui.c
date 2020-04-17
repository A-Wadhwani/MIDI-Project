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

char* open_folder_dialog();
char* gtk_file_chooser_get_file_name(GtkFileChooser*);

// This structure contains all the widgets in GUI
struct ui_widgets {
  GtkBuilder *builder;
  GtkWidget *window;
  GtkWidget *fixed;
  GtkButton *load_button;
  GtkButton *add_song_button;
} g_widgets;

// This structure contains all the global parameters used
// among different GUI pointers
struct parameters{
  char *folder_directory;
} g_parameters;

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

void activate(GtkApplication *app, gpointer user_data){
  g_widgets.builder = gtk_builder_new_from_file("src/ui.glade");
  g_widgets.window = gtk_application_window_new(app);
  
  gtk_window_set_title(GTK_WINDOW(g_widgets.window), "MIDI Library");
  gtk_window_set_resizable(GTK_WINDOW(g_widgets.window), false);
  gtk_window_set_default_size(GTK_WINDOW(g_widgets.window), 950, 720);
  gtk_window_set_position(GTK_WINDOW(g_widgets.window), GTK_WIN_POS_CENTER);

  g_widgets.fixed = GTK_WIDGET(gtk_builder_get_object(g_widgets.builder, "fixed_grid"));
  gtk_container_add(GTK_CONTAINER(g_widgets.window), g_widgets.fixed);

  g_widgets.load_button = GTK_BUTTON(gtk_builder_get_object(g_widgets.builder, "load_directory"));
  g_signal_connect(g_widgets.load_button, "clicked", G_CALLBACK(load_songs_cb), NULL);
  
  g_signal_connect(g_widgets.window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_builder_connect_signals(g_widgets.builder, NULL);
  gtk_widget_show_all(g_widgets.window);
}

/* Define add_song_cb here */

void add_song_cb(GtkButton *button, gpointer user_data){
  
}

/* Define load_songs_cb here */

void load_songs_cb(GtkButton *button, gpointer user_data){
  
}

char* open_folder_dialog(){
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
  char *file_name = NULL;
  GtkWidget *dialog = gtk_file_chooser_dialog_new("Select Folder", GTK_WINDOW(g_widgets.window), 
                                       action, "_Cancel", GTK_RESPONSE_CANCEL,
                                       "_Open Folder", GTK_RESPONSE_ACCEPT, NULL);
  gint result = gtk_dialog_run (GTK_DIALOG(dialog));
  if (result == GTK_RESPONSE_ACCEPT){
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
    file_name = gtk_file_chooser_get_file_name (chooser);
  }
  gtk_widget_destroy (dialog);
  return file_name;
}

/* Define song_selected_cb here */

void song_selected_cb(GtkListBox *list_box, GtkListBoxRow *row){
  
}

/* Define search_bar_cb here */

void search_bar_cb(GtkSearchBar *search_bar, gpointer user_data){

}

/* Define time_scale_cb here */

void time_scale_cb(GtkSpinButton *time_scale, gpointer user_data){

}

/* Define draw_cb here */

gboolean draw_cb(GtkDrawingArea *draw_area, cairo_t *painter, gpointer user_data){
  return false;  
}

/* Define warp_time_cb here */

void warp_time_cb(GtkSpinButton *warp_scale, gpointer user_data){
  
}

/* Define song_octave_cb here */

void song_ocatave_cb(GtkSpinButton *octave_scale, gpointer user_data){
  
}

/* Define instrument_map_cb here */

void instrument_map_cb(GtkComboBoxText *picked_inst, gpointer user_data){
}

/* Define note_map_cb here */

void note_map_cb(GtkComboBoxText *picked_note, gpointer user_data){
  
}

/* Define save_song_cb here */

void save_song_cb(GtkButton *button, gpointer user_data){

}

/* Define remove_song_cb here */

void remove_song(GtkButton *button, gpointer user_data){

}

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
