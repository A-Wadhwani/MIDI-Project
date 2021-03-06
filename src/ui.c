/* Aryan Wadhwani, ui.c, CS 24000, Spring 2020
 * Last updated April 16, 2020
 */

/* Included Libraries */

#include "ui.h"

#include<string.h>
#include<malloc.h>
#include<ftw.h>
#include<ctype.h>

#include "parser.h"
#include "alterations.h"
#include "song_writer.h"

#define XML_STRING "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\
<interface>\
  <requires lib=\"gtk+\" version=\"3.20\"/>\
  <object class=\"GtkAdjustment\" id=\"adjustment1\">\
    <property name=\"lower\">1</property>\
    <property name=\"upper\">1000</property>\
    <property name=\"step_increment\">1</property>\
    <property name=\"page_increment\">10</property>\
  </object>\
  <object class=\"GtkAdjustment\" id=\"adjustment2\">\
    <property name=\"upper\">20</property>\
    <property name=\"value\">1</property>\
    <property name=\"step_increment\">0.10000000000000001</property>\
    <property name=\"page_increment\">0.10000000000000001</property>\
  </object>\
  <object class=\"GtkAdjustment\" id=\"adjustment3\">\
    <property name=\"lower\">-6</property>\
    <property name=\"upper\">16</property>\
    <property name=\"step_increment\">1</property>\
    <property name=\"page_increment\">10</property>\
  </object>\
  <object class=\"GtkFixed\" id=\"fixed_grid\">\
    <property name=\"name\">draw_original</property>\
    <property name=\"visible\">True</property>\
    <property name=\"can_focus\">False</property>\
    <child>\
      <object class=\"GtkButton\" id=\"add_song\">\
        <property name=\"label\" translatable=\"yes\">Add Song from File</property>\
        <property name=\"name\">add_song</property>\
        <property name=\"width_request\">160</property>\
        <property name=\"height_request\">35</property>\
        <property name=\"visible\">True</property>\
        <property name=\"can_focus\">True</property>\
        <property name=\"receives_default\">True</property>\
      </object>\
      <packing>\
        <property name=\"x\">10</property>\
        <property name=\"y\">10</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkButton\" id=\"load_directory\">\
        <property name=\"label\" translatable=\"yes\">Load from Directory</property>\
        <property name=\"name\">load_directory</property>\
        <property name=\"width_request\">160</property>\
        <property name=\"height_request\">35</property>\
        <property name=\"visible\">True</property>\
        <property name=\"can_focus\">True</property>\
        <property name=\"receives_default\">True</property>\
      </object>\
      <packing>\
        <property name=\"x\">180</property>\
        <property name=\"y\">10</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkSearchEntry\" id=\"search_song\">\
        <property name=\"name\">search_song</property>\
        <property name=\"width_request\">335</property>\
        <property name=\"height_request\">20</property>\
        <property name=\"visible\">True</property>\
        <property name=\"can_focus\">True</property>\
        <property name=\"primary_icon_name\">edit-find-symbolic</property>\
        <property name=\"primary_icon_activatable\">False</property>\
        <property name=\"primary_icon_sensitive\">False</property>\
      </object>\
      <packing>\
        <property name=\"x\">10</property>\
        <property name=\"y\">675</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkLabel\" id=\"file_details\">\
        <property name=\"name\">file_details</property>\
        <property name=\"width_request\">580</property>\
        <property name=\"height_request\">70</property>\
        <property name=\"visible\">True</property>\
        <property name=\"can_focus\">False</property>\
        <property name=\"halign\">start</property>\
        <property name=\"valign\">start</property>\
       <property name=\"label\" translatable=\"yes\">Select a file from list to start...\
</property>\
        <property name=\"wrap\">True</property>\
        <property name=\"ellipsize\">end</property>\
        <property name=\"width_chars\">1</property>\
        <property name=\"max_width_chars\">15</property>\
        <property name=\"xalign\">0</property>\
        <property name=\"yalign\">0</property>\
      </object>\
      <packing>\
        <property name=\"x\">360</property>\
        <property name=\"y\">10</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkSpinButton\" id=\"time_scale\">\
        <property name=\"name\">time_scale</property>\
        <property name=\"width_request\">300</property>\
        <property name=\"height_request\">35</property>\
        <property name=\"visible\">True</property>\
        <property name=\"sensitive\">False</property>\
        <property name=\"can_focus\">True</property>\
        <property name=\"activates_default\">True</property>\
        <property name=\"adjustment\">adjustment1</property>\
        <property name=\"value\">10</property>\
      </object>\
      <packing>\
        <property name=\"x\">640</property>\
        <property name=\"y\">90</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkLabel\" id=\"time_scale_label\">\
        <property name=\"name\">time_scale_label</property>\
        <property name=\"width_request\">290</property>\
        <property name=\"height_request\">35</property>\
        <property name=\"visible\">True</property>\
        <property name=\"can_focus\">False</property>\
        <property name=\"label\" translatable=\"yes\">T scale:</property>\
      </object>\
      <packing>\
        <property name=\"x\">350</property>\
        <property name=\"y\">90</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkFrame\" id=\"frame1\">\
        <property name=\"width_request\">580</property>\
        <property name=\"height_request\">180</property>\
        <property name=\"visible\">True</property>\
        <property name=\"can_focus\">False</property>\
        <property name=\"label_xalign\">0</property>\
        <property name=\"shadow_type\">none</property>\
        <child>\
          <object class=\"GtkAlignment\" id=\"orginal_song_are\">\
            <property name=\"visible\">True</property>\
            <property name=\"can_focus\">False</property>\
            <property name=\"left_padding\">12</property>\
            <child>\
              <object class=\"GtkScrolledWindow\">\
                <property name=\"width_request\">560</property>\
                <property name=\"height_request\">160</property>\
                <property name=\"visible\">True</property>\
                <property name=\"can_focus\">True</property>\
                <property name=\"hscrollbar_policy\">always</property>\
                <property name=\"vscrollbar_policy\">never</property>\
                <property name=\"shadow_type\">in</property>\
                <child>\
                  <object class=\"GtkViewport\">\
                    <property name=\"visible\">True</property>\
                    <property name=\"can_focus\">False</property>\
                    <child>\
                      <object class=\"GtkDrawingArea\" id=\"draw_original\">\
                        <property name=\"visible\">True</property>\
                        <property name=\"can_focus\">False</property>\
                      </object>\
                    </child>\
                  </object>\
                </child>\
              </object>\
            </child>\
          </object>\
        </child>\
        <child type=\"label\">\
          <object class=\"GtkLabel\">\
            <property name=\"visible\">True</property>\
            <property name=\"can_focus\">False</property>\
            <property name=\"label\" translatable=\"yes\">Original song:</property>\
          </object>\
        </child>\
      </object>\
      <packing>\
        <property name=\"x\">360</property>\
        <property name=\"y\">135</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkSearchBar\" id=\"search_bar\">\
        <property name=\"name\">search_bar</property>\
        <property name=\"can_focus\">False</property>\
        <property name=\"double_buffered\">False</property>\
        <property name=\"search_mode_enabled\">True</property>\
        <child>\
          <placeholder/>\
        </child>\
      </object>\
      <packing>\
        <property name=\"x\">10</property>\
        <property name=\"y\">655</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkListBox\" id=\"song_list\">\
        <property name=\"name\">song_list</property>\
        <property name=\"width_request\">335</property>\
        <property name=\"height_request\">610</property>\
        <property name=\"visible\">True</property>\
        <property name=\"can_focus\">True</property>\
      </object>\
      <packing>\
        <property name=\"x\">10</property>\
        <property name=\"y\">55</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkFrame\" id=\"frame2\">\
        <property name=\"width_request\">580</property>\
        <property name=\"height_request\">180</property>\
        <property name=\"visible\">True</property>\
        <property name=\"can_focus\">False</property>\
        <property name=\"label_xalign\">0</property>\
        <property name=\"shadow_type\">none</property>\
        <child>\
          <object class=\"GtkAlignment\" id=\"orginal_song_are1\">\
            <property name=\"name\">after_effect</property>\
            <property name=\"visible\">True</property>\
            <property name=\"can_focus\">False</property>\
            <property name=\"left_padding\">12</property>\
            <child>\
              <object class=\"GtkScrolledWindow\">\
                <property name=\"width_request\">560</property>\
                <property name=\"height_request\">160</property>\
                <property name=\"visible\">True</property>\
                <property name=\"can_focus\">True</property>\
                <property name=\"hscrollbar_policy\">always</property>\
                <property name=\"vscrollbar_policy\">never</property>\
                <property name=\"shadow_type\">in</property>\
                <child>\
                  <object class=\"GtkViewport\">\
                    <property name=\"visible\">True</property>\
                    <property name=\"can_focus\">False</property>\
                    <child>\
                      <object class=\"GtkDrawingArea\" id=\"draw_after\">\
                        <property name=\"visible\">True</property>\
                        <property name=\"can_focus\">False</property>\
                      </object>\
                    </child>\
                  </object>\
                </child>\
              </object>\
            </child>\
          </object>\
        </child>\
        <child type=\"label\">\
          <object class=\"GtkLabel\">\
            <property name=\"visible\">True</property>\
            <property name=\"can_focus\">False</property>\
            <property name=\"label\" translatable=\"yes\">After effect:</property>\
          </object>\
        </child>\
      </object>\
      <packing>\
        <property name=\"x\">360</property>\
        <property name=\"y\">320</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkSpinButton\" id=\"warp_scale\">\
        <property name=\"name\">time_scale</property>\
        <property name=\"width_request\">275</property>\
        <property name=\"height_request\">35</property>\
        <property name=\"visible\">True</property>\
        <property name=\"sensitive\">False</property>\
        <property name=\"can_focus\">True</property>\
        <property name=\"activates_default\">True</property>\
        <property name=\"text\" translatable=\"yes\">10</property>\
        <property name=\"adjustment\">adjustment2</property>\
        <property name=\"digits\">1</property>\
        <property name=\"value\">1</property>\
      </object>\
      <packing>\
        <property name=\"x\">640</property>\
        <property name=\"y\">510</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkLabel\" id=\"warp_scale_label\">\
        <property name=\"name\">warp_scale_label</property>\
        <property name=\"width_request\">290</property>\
        <property name=\"height_request\">35</property>\
        <property name=\"visible\">True</property>\
        <property name=\"can_focus\">False</property>\
        <property name=\"label\" translatable=\"yes\">Warp time:</property>\
      </object>\
      <packing>\
        <property name=\"x\">350</property>\
        <property name=\"y\">510</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkSpinButton\" id=\"octave_scale\">\
        <property name=\"name\">octave_scale</property>\
        <property name=\"width_request\">275</property>\
        <property name=\"height_request\">35</property>\
        <property name=\"visible\">True</property>\
        <property name=\"sensitive\">False</property>\
        <property name=\"can_focus\">True</property>\
        <property name=\"activates_default\">True</property>\
        <property name=\"text\" translatable=\"yes\">1.0</property>\
        <property name=\"adjustment\">adjustment3</property>\
        <property name=\"value\">1</property>\
      </object>\
      <packing>\
        <property name=\"x\">640</property>\
        <property name=\"y\">550</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkLabel\" id=\"octave_scale_label\">\
        <property name=\"name\">octave_scale_label</property>\
        <property name=\"width_request\">290</property>\
        <property name=\"height_request\">35</property>\
        <property name=\"visible\">True</property>\
        <property name=\"can_focus\">False</property>\
        <property name=\"label\" translatable=\"yes\">Change octave:</property>\
      </object>\
      <packing>\
        <property name=\"x\">350</property>\
        <property name=\"y\">550</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkLabel\" id=\"remap_inst_label\">\
        <property name=\"name\">remap_label</property>\
        <property name=\"width_request\">290</property>\
        <property name=\"height_request\">35</property>\
        <property name=\"visible\">True</property>\
        <property name=\"can_focus\">False</property>\
        <property name=\"label\" translatable=\"yes\">Remap instruments:</property>\
      </object>\
      <packing>\
        <property name=\"x\">350</property>\
        <property name=\"y\">590</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkComboBoxText\" id=\"remap_inst\">\
        <property name=\"name\">remap_inst</property>\
        <property name=\"width_request\">275</property>\
        <property name=\"height_request\">30</property>\
        <property name=\"visible\">True</property>\
        <property name=\"sensitive\">False</property>\
        <property name=\"can_focus\">False</property>\
        <property name=\"has_entry\">True</property>\
        <items>\
          <item translatable=\"yes\"> </item>\
          <item translatable=\"yes\">Brass Band</item>\
          <item translatable=\"yes\">Helicopter</item>\
        </items>\
        <child internal-child=\"entry\">\
          <object class=\"GtkEntry\">\
            <property name=\"can_focus\">False</property>\
            <property name=\"text\" translatable=\"yes\"> </property>\
          </object>\
        </child>\
      </object>\
      <packing>\
        <property name=\"x\">640</property>\
        <property name=\"y\">590</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkLabel\" id=\"remap_notes_label\">\
        <property name=\"name\">remap_notes_label</property>\
        <property name=\"width_request\">290</property>\
        <property name=\"height_request\">35</property>\
        <property name=\"visible\">True</property>\
        <property name=\"can_focus\">False</property>\
        <property name=\"label\" translatable=\"yes\">Remap notes:</property>\
      </object>\
      <packing>\
        <property name=\"x\">350</property>\
        <property name=\"y\">630</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkComboBoxText\" id=\"remap_notes\">\
        <property name=\"name\">remap_notes</property>\
        <property name=\"width_request\">275</property>\
        <property name=\"height_request\">35</property>\
        <property name=\"visible\">True</property>\
        <property name=\"sensitive\">False</property>\
        <property name=\"can_focus\">False</property>\
        <property name=\"has_entry\">True</property>\
        <items>\
          <item translatable=\"yes\"> </item>\
          <item translatable=\"yes\">Lower Notes</item>\
        </items>\
        <child internal-child=\"entry\">\
          <object class=\"GtkEntry\">\
            <property name=\"can_focus\">True</property>\
            <property name=\"text\" translatable=\"yes\"> </property>\
          </object>\
        </child>\
      </object>\
      <packing>\
        <property name=\"x\">640</property>\
        <property name=\"y\">630</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkButton\" id=\"save_song\">\
        <property name=\"label\" translatable=\"yes\">Save Song</property>\
        <property name=\"name\">save_song</property>\
        <property name=\"width_request\">270</property>\
        <property name=\"height_request\">35</property>\
        <property name=\"visible\">True</property>\
        <property name=\"sensitive\">False</property>\
        <property name=\"can_focus\">True</property>\
        <property name=\"receives_default\">True</property>\
      </object>\
      <packing>\
        <property name=\"x\">360</property>\
        <property name=\"y\">675</property>\
      </packing>\
    </child>\
    <child>\
      <object class=\"GtkButton\" id=\"remove_song\">\
        <property name=\"label\" translatable=\"yes\">Remove Song</property>\
        <property name=\"name\">save_song</property>\
        <property name=\"width_request\">270</property>\
        <property name=\"height_request\">35</property>\
        <property name=\"visible\">True</property>\
        <property name=\"sensitive\">False</property>\
        <property name=\"can_focus\">True</property>\
        <property name=\"receives_default\">True</property>\
      </object>\
      <packing>\
        <property name=\"x\">645</property>\
        <property name=\"y\">675</property>\
      </packing>\
    </child>\
  </object>\
</interface>"

tree_node_t *g_current_node = NULL;
song_data_t *g_current_song = NULL;
song_data_t *g_modified_song = NULL;

int get_y_pos(int, int, int, int);
int get_delta_len(event_node_t *, int);
bool compare_strings(const char*, const char *);
char* get_file_name(const char *);
char* open_file_dialog();
char* open_folder_dialog();
void handle_painting(cairo_t *, song_data_t *, int, int, int);
void draw_line(cairo_t *, int, int, int);
void add_to_song_list(tree_node_t*, int*);
void remove_list();
void write_gui();

// This structure contains all the widgets in GUI
struct ui_widgets {
  GtkBuilder *builder;
  GtkWidget *window;
  GtkWidget *fixed;
  
  GtkListBox *song_list;
  GtkButton *load_button;
  GtkButton *add_button;
  GtkButton *save_song;
  GtkButton *remove_song;
  
  GtkComboBoxText *remap_instruments;
  GtkComboBoxText *remap_notes;
  
  GtkLabel *file_details;
  
  GtkSearchBar *search_bar;
  GtkSearchEntry *search_entry;
  
  GtkSpinButton *time_scale;
  GtkSpinButton *warp_time;
  GtkSpinButton *song_octave;

  GtkDrawingArea *original_area;
  GtkDrawingArea *after_area;
} g_widgets;

// This structure contains all the global parameters used
// among different GUI pointers
struct parameters{
  char *folder_directory;
  char *selected_song_name;
  
  int time_scale;
  double change_time;
  int change_octave;

  remapping_t *notes;
  bool use_notes;
  remapping_t *instrument;
  bool use_inst;

  int load_original;
  int load_modified;
} g_parameters;

/* Define update_song_list here */

void update_song_list(){
  int count = 0;
  remove_list();
  traverse_in_order(g_song_library, &count, (traversal_func_t) add_to_song_list);
}

void remove_list(){
  GtkListBoxRow *row = gtk_list_box_get_row_at_index(g_widgets.song_list, 0);
  while (row != NULL){
    gtk_widget_destroy(GTK_WIDGET(row));
    row = gtk_list_box_get_row_at_index(g_widgets.song_list, 0);
  }
}

void add_to_song_list(tree_node_t *node, int *count){
  GtkWidget *row = gtk_list_box_row_new();
  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  g_object_set (box, "margin-start", 10, "margin-end", 10, NULL);
  gtk_container_add (GTK_CONTAINER (row), box);
  GtkWidget *song_label = gtk_label_new(node->song_name);
  gtk_container_add_with_properties (GTK_CONTAINER (box), song_label, "expand", TRUE, NULL);
  gtk_widget_show_all(row);
  gtk_list_box_insert(g_widgets.song_list, row, *count);
  (*count)++;
  return;
}
/* Define update_drawing_area here */

void update_drawing_area(){
  if (g_modified_song){
    free_song(g_modified_song);
    g_modified_song = NULL;
  }
  if (g_current_node){
    g_modified_song = parse_file(g_current_node->song->path);
    if (g_parameters.change_time != -2){
      warp_time(g_modified_song, g_parameters.change_time);
    }
    if (g_parameters.change_octave != -2){
      change_octave(g_modified_song, g_parameters.change_octave);
    }
    if (g_parameters.use_inst == true){
      remap_instruments(g_modified_song, *g_parameters.instrument);
    }
    if (g_parameters.use_notes == true){
      remap_notes(g_modified_song, *g_parameters.notes);
    }
  }
  gtk_widget_queue_draw(GTK_WIDGET(g_widgets.original_area));
  gtk_widget_queue_draw(GTK_WIDGET(g_widgets.after_area));
  return;
}

/* Define update_info here */

void update_info(){
  if (g_current_node){
    
    char name_string[1024];
    char full_path[1024];
    char note_range[1024];
    char original_length[1024];
    char buffer[4096];
    int low_pitch = 128;
    int high_pitch = -1;
    int length = 0;
    range_of_song(g_current_song, &low_pitch, &high_pitch, &length);

    snprintf(name_string, 1024, "File name: %s", g_current_node->song_name);
    snprintf(full_path, 1024, "Full path: %s", g_current_song->path);
    snprintf(note_range, 1024, "Note range: [%d, %d]", low_pitch, high_pitch);
    snprintf(original_length, 1024, "Original length: %d", length);
    snprintf(buffer, 4096, "%s\n%s\n%s\n%s\n", name_string, full_path, note_range, original_length);
    gtk_label_set_text(g_widgets.file_details, buffer);

    g_object_set(GTK_WIDGET(g_widgets.remove_song), "sensitive", true, NULL);
    g_object_set(GTK_WIDGET(g_widgets.save_song), "sensitive", true, NULL);
    g_object_set(GTK_WIDGET(g_widgets.remap_instruments), "sensitive", true, NULL);
    g_object_set(GTK_WIDGET(g_widgets.remap_notes), "sensitive", true, NULL);
    g_object_set(GTK_ENTRY(g_widgets.time_scale), "sensitive", true, NULL);
    g_object_set(GTK_ENTRY(g_widgets.warp_time), "sensitive", true, NULL);
    g_object_set(GTK_ENTRY(g_widgets.song_octave), "sensitive", true, NULL);
    gtk_spin_button_set_value(g_widgets.time_scale, 10.00);
    gtk_spin_button_set_value(g_widgets.warp_time, 1.00);
    gtk_spin_button_set_value(g_widgets.song_octave, 0.0);
  }
  else {
    gtk_label_set_text(g_widgets.file_details, "Select a song from list to start....");
    g_object_set(GTK_WIDGET(g_widgets.remove_song), "sensitive", false, NULL);
    g_object_set(GTK_WIDGET(g_widgets.save_song), "sensitive", false, NULL);
    g_object_set(GTK_WIDGET(g_widgets.remap_instruments), "sensitive", false, NULL);
    g_object_set(GTK_WIDGET(g_widgets.remap_notes), "sensitive", false, NULL);
    g_object_set(GTK_ENTRY(g_widgets.time_scale), "sensitive", false, NULL);
    g_object_set(GTK_ENTRY(g_widgets.warp_time), "sensitive", false, NULL);
    g_object_set(GTK_ENTRY(g_widgets.song_octave), "sensitive", false, NULL);
  }
  update_drawing_area();
}

/* Define update_song here */

void update_song(){
  update_info();
  return;
}


/* Define range_of_song here */

void range_of_song(song_data_t *midi_song, int *low_pitch,
                   int *high_pitch, int *length){
  track_node_t *copy_track = midi_song->track_list;
  if (low_pitch){
    *low_pitch = 128;
  }
  if (high_pitch){
    *high_pitch = -1;
  }
  if (length){
    *length = 0;
  }
  int save_length = 0;
  while (copy_track != NULL){
    event_node_t *copy_event = copy_track->track->event_list;
    while (copy_event != NULL){
      if (event_type(copy_event->event) == MIDI_EVENT_T){
        if (copy_event->event->midi_event.status >= 0x80 &&
            copy_event->event->midi_event.status <= 0xAF){
          if (low_pitch){
            if (*low_pitch > copy_event->event->midi_event.data[0]){
              *low_pitch = copy_event->event->midi_event.data[0];
            }
          }
          if (high_pitch){
            if (*high_pitch < copy_event->event->midi_event.data[0]){
              *high_pitch = copy_event->event->midi_event.data[0];
            }
          }
        }
      }
      if (length){
        (*length) = (*length) + copy_event->event->delta_time;
      }
      copy_event = copy_event->next_event;
    }
    if (midi_song->format == 1){
      if (length){
        if (save_length < (*length)){
          save_length = *length;
        }
        *length = 0;
      }
    }
    copy_track = copy_track->next_track;
  }
  if (save_length > *length){
    *length = save_length;
  }
  return;
}


void write_gui(){
	FILE *fp = fopen("src/ui.glade", "w");
	fprintf(fp, "%s", XML_STRING);
	fclose(fp);
	fp = NULL;
}

/* Define activate here */

void activate(GtkApplication *app, gpointer user_data){
  write_gui();
  g_widgets.builder = gtk_builder_new_from_file("src/ui.glade");
  g_widgets.window = gtk_application_window_new(app);
  
  g_parameters.time_scale = 10;
  g_parameters.load_original = 0;
  g_parameters.load_modified = 1;

  gtk_window_set_title(GTK_WINDOW(g_widgets.window), "MIDI Library");
  gtk_window_set_resizable(GTK_WINDOW(g_widgets.window), false);
  gtk_window_set_default_size(GTK_WINDOW(g_widgets.window), 950, 720);
  gtk_window_set_position(GTK_WINDOW(g_widgets.window), GTK_WIN_POS_CENTER);

  g_widgets.fixed = GTK_WIDGET(gtk_builder_get_object(g_widgets.builder, "fixed_grid"));
  gtk_container_add(GTK_CONTAINER(g_widgets.window), g_widgets.fixed);

  g_widgets.file_details = GTK_LABEL(gtk_builder_get_object(g_widgets.builder, "file_details"));

  g_widgets.song_list = GTK_LIST_BOX(gtk_builder_get_object(g_widgets.builder, "song_list"));
  g_signal_connect(g_widgets.song_list, "row-activated", G_CALLBACK(song_selected_cb), NULL);

  g_widgets.search_bar = GTK_SEARCH_BAR(gtk_builder_get_object(g_widgets.builder, "search_bar"));
  g_widgets.search_entry = GTK_SEARCH_ENTRY(gtk_builder_get_object(g_widgets.builder, "search_song"));
  gtk_search_bar_connect_entry(g_widgets.search_bar, GTK_ENTRY(g_widgets.search_entry));
  g_signal_connect(g_widgets.search_entry, "search-changed", G_CALLBACK(search_bar_cb), NULL);

  g_widgets.time_scale = GTK_SPIN_BUTTON(gtk_builder_get_object(g_widgets.builder, "time_scale"));
  g_signal_connect(g_widgets.time_scale, "value-changed", G_CALLBACK(time_scale_cb), NULL);

  g_widgets.warp_time = GTK_SPIN_BUTTON(gtk_builder_get_object(g_widgets.builder, "warp_scale"));
  g_signal_connect(g_widgets.warp_time, "value-changed", G_CALLBACK(warp_time_cb), NULL);

  g_widgets.song_octave = GTK_SPIN_BUTTON(gtk_builder_get_object(g_widgets.builder, "octave_scale"));
  g_signal_connect(g_widgets.song_octave, "value-changed", G_CALLBACK(song_octave_cb), NULL);

  g_widgets.save_song = GTK_BUTTON(gtk_builder_get_object(g_widgets.builder, "save_song"));
  g_signal_connect(g_widgets.save_song, "clicked", G_CALLBACK(save_song_cb), NULL);
  
  g_widgets.remove_song = GTK_BUTTON(gtk_builder_get_object(g_widgets.builder, "remove_song"));
  g_signal_connect(g_widgets.remove_song, "clicked", G_CALLBACK(remove_song_cb), NULL);
 
  g_widgets.remap_instruments = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(g_widgets.builder, "remap_inst"));
  g_signal_connect(g_widgets.remap_instruments, "changed", G_CALLBACK(instrument_map_cb), NULL);
 
  g_widgets.remap_notes = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(g_widgets.builder, "remap_notes"));
  g_signal_connect(g_widgets.remap_notes, "changed", G_CALLBACK(note_map_cb), NULL);

  g_widgets.load_button = GTK_BUTTON(gtk_builder_get_object(g_widgets.builder, "load_directory"));
  g_signal_connect(g_widgets.load_button, "clicked", G_CALLBACK(load_songs_cb), NULL);
  
  g_widgets.add_button = GTK_BUTTON(gtk_builder_get_object(g_widgets.builder, "add_song"));
  g_signal_connect(g_widgets.add_button, "clicked", G_CALLBACK(add_song_cb), NULL);
  
  g_widgets.original_area = GTK_DRAWING_AREA(gtk_builder_get_object(g_widgets.builder, "draw_original"));
  g_signal_connect(g_widgets.original_area, "draw", G_CALLBACK(draw_cb), &g_parameters.load_original);
  
  g_widgets.after_area = GTK_DRAWING_AREA(gtk_builder_get_object(g_widgets.builder, "draw_after"));
  g_signal_connect(g_widgets.after_area, "draw", G_CALLBACK(draw_cb), &g_parameters.load_modified);
  
  g_signal_connect(g_widgets.window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  gtk_builder_connect_signals(g_widgets.builder, NULL);
  gtk_widget_show_all(g_widgets.window);

}

/* Define add_song_cb here */

void add_song_cb(GtkButton *button, gpointer user_data){
  gtk_widget_queue_draw(GTK_WIDGET(g_widgets.original_area));
  char* file_name = open_file_dialog();
  if (file_name == NULL){
    return;
  }
  tree_node_t *new_node = malloc(sizeof(tree_node_t));
  new_node->song = parse_file(file_name);
  new_node->song_name = get_file_name(new_node->song->path);
  new_node->left_child = NULL;
  new_node->right_child = NULL;
  tree_insert(&g_song_library, new_node);
  g_free(file_name);
  file_name = NULL;
  update_song_list();
}

char* open_file_dialog(){
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
  char *file_name = NULL;
  GtkWidget *dialog = gtk_file_chooser_dialog_new("Select Song to Import", GTK_WINDOW(g_widgets.window), 
                                       action, "_Cancel", GTK_RESPONSE_CANCEL,
                                       "_Open File", GTK_RESPONSE_ACCEPT, NULL);
  gint result = gtk_dialog_run (GTK_DIALOG(dialog));
  if (result == GTK_RESPONSE_ACCEPT){
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
    file_name = gtk_file_chooser_get_filename (chooser);
  }
  gtk_widget_destroy(dialog);
  return file_name;
}

/* Define load_songs_cb here */

void load_songs_cb(GtkButton *button, gpointer user_data){
  if (g_parameters.folder_directory != NULL){
    if (g_song_library){
      free_library(g_song_library);
      g_song_library = NULL;
    }
    g_current_node = NULL;
    g_current_song = NULL;
    g_modified_song = NULL;
    g_free(g_parameters.folder_directory);
    g_parameters.folder_directory = NULL;
  }
  g_parameters.folder_directory = open_folder_dialog();
  if (g_parameters.folder_directory == NULL){
    return;
  }
  make_library(g_parameters.folder_directory);
  update_song_list();
  update_info();
}

char* open_folder_dialog(){
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
  char *file_name = NULL;
  GtkWidget *dialog = gtk_file_chooser_dialog_new("Select Songs Directory",
                                       GTK_WINDOW(g_widgets.window), 
                                       action, "_Cancel", GTK_RESPONSE_CANCEL,
                                       "_Open Folder", GTK_RESPONSE_ACCEPT, NULL);
  gint result = gtk_dialog_run (GTK_DIALOG(dialog));
  if (result == GTK_RESPONSE_ACCEPT){
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
    file_name = gtk_file_chooser_get_filename (chooser);
  }
  gtk_widget_destroy(dialog);
  return file_name;
}

/* Define song_selected_cb here */

void song_selected_cb(GtkListBox *list_box, GtkListBoxRow *row){
  g_current_node = NULL;
  g_current_song = NULL;
  if (g_modified_song){
    free_song(g_modified_song);
  }
  g_modified_song = NULL;

  GList *list = gtk_container_get_children(GTK_CONTAINER(row));
  GtkWidget *box = GTK_WIDGET(list->data);
  list = gtk_container_get_children(GTK_CONTAINER(box));
  GtkWidget *label = GTK_WIDGET(list->data);
  const char *song_name = gtk_label_get_text(GTK_LABEL(label));
  g_current_node = *(find_parent_pointer(&g_song_library, song_name));
  g_current_song = g_current_node->song;
  
  g_parameters.change_time = -2;
  g_parameters.change_octave = -2;
  gtk_combo_box_set_active (GTK_COMBO_BOX(g_widgets.remap_notes), 0);
  gtk_combo_box_set_active (GTK_COMBO_BOX(g_widgets.remap_instruments), 0);
  g_parameters.use_inst = false;
  g_parameters.use_notes = false;
  update_info();
}

/* Define search_bar_cb here */

void search_bar_cb(GtkSearchBar *search_bar, gpointer user_data){
  update_song_list();
  const char *search_string = gtk_entry_get_text(GTK_ENTRY(g_widgets.search_entry));
  GtkListBoxRow *row = gtk_list_box_get_row_at_index(g_widgets.song_list, 0);
  int count = 0;
  while (row != NULL){
    GList *list = gtk_container_get_children(GTK_CONTAINER(row));
    GtkWidget *box = GTK_WIDGET(list->data);
    list = gtk_container_get_children(GTK_CONTAINER(box));
    GtkWidget *label = GTK_WIDGET(list->data);
    const char *song_name = gtk_label_get_text(GTK_LABEL(label));
    if (compare_strings(song_name, search_string) == false){
      gtk_widget_destroy(GTK_WIDGET(row));
      row = gtk_list_box_get_row_at_index(g_widgets.song_list, count);
    } else {
      row = gtk_list_box_get_row_at_index(g_widgets.song_list, ++count);
    }
  }
}

bool compare_strings(const char* string, const char *pattern){
  char string_low[strlen(string)];
  char pattern_low[strlen(pattern)];
  strcpy(string_low, string);
  strcpy(pattern_low, pattern);
  for (int i = 0; i < strlen(string_low); i++){
    string_low[i] = tolower(string_low[i]);
  }
  for (int i = 0; i < strlen(pattern_low); i++){
    pattern_low[i] = tolower(pattern_low[i]);
  }
  if (strstr(string_low, pattern_low) == NULL){
    return false;
  }
  return true;
}

/* Define time_scale_cb here */

void time_scale_cb(GtkSpinButton *time_scale, gpointer user_data){
  g_parameters.time_scale = gtk_spin_button_get_value_as_int(time_scale);
  update_drawing_area();
}

/* Define draw_cb here */

gboolean draw_cb(GtkDrawingArea *draw_area, cairo_t *painter, gpointer user_data){
  cairo_set_source_rgb(painter, 255, 255, 255);
  cairo_set_line_width(painter, 1.0);
  if (g_current_node == NULL){
    // Draws the white box

    guint height = gtk_widget_get_allocated_height(GTK_WIDGET(draw_area));
    guint width = gtk_widget_get_allocated_width(GTK_WIDGET(draw_area));
    if (width != 560){
      gtk_widget_set_size_request(GTK_WIDGET(draw_area), 560, 160);
    }
    cairo_rectangle(painter, 0.0, 0.0, width, height);
    cairo_stroke_preserve(painter);
    cairo_fill(painter);
    
    return false;
  }
  song_data_t *given_song = NULL;
  int *flag = (int*)(user_data);
  if (*flag == 0){
    given_song = g_current_song;
  }
  if (*flag == 1){
    given_song = g_modified_song;
  }
  int length = 0;
  int high_note = 0;
  int low_note = 0;
  range_of_song(given_song, &low_note, &high_note, &length);
  guint height =  gtk_widget_get_allocated_height(GTK_WIDGET(draw_area));
  guint width = length / g_parameters.time_scale;
  if (width < 560){
    width = 560;
  }
  gtk_widget_set_size_request(GTK_WIDGET(draw_area), width, height);
  if (high_note < 60){
    high_note = 60;
  }
  if (low_note > 60){
    low_note = 60;
  }
  guint note_scale = height / (high_note - low_note + 10);
  
  // Draws the white box

  cairo_rectangle(painter, 0.0, 0.0, width, height);
  cairo_stroke_preserve(painter);
  cairo_fill(painter);

  // Draws the middle C line
  
  int middle_c_pos = get_y_pos(height, note_scale, 60, low_note);
  cairo_set_line_width(painter, 1.0);
  cairo_set_source_rgb(painter, 0.0, 0.0, 0.0);
  draw_line(painter, middle_c_pos, 0.0, length);
  cairo_set_line_width(painter, 2.0);
  handle_painting(painter, given_song, height, width, note_scale);
  return false;  
}

void draw_line(cairo_t *painter, int note_pos, int begin_time, int length){
  int begin_pos = begin_time / g_parameters.time_scale;
  int end_pos = (length + begin_time) / g_parameters.time_scale;
  cairo_move_to(painter, begin_pos, note_pos);
  cairo_line_to(painter, end_pos, note_pos);
  cairo_stroke(painter);
}

void handle_painting(cairo_t *painter, song_data_t *song, int height, int width, int note_scale){
  int total_time = 0;
  int low_note = 0;
  range_of_song(song, &low_note, NULL, &total_time);
  if (low_note > 60){
    low_note = 60;
  }
  track_node_t *copy_track = song->track_list;
  GdkRGBA *given_color = NULL;
  int current_time = 0;
  while (copy_track != NULL){
    event_node_t *copy_event = copy_track->track->event_list;
    while (copy_event != NULL){
      if (event_type(copy_event->event) == MIDI_EVENT_T){
        if (copy_event->event->midi_event.status >= 0xC0 &&
            copy_event->event->midi_event.status <= 0xCF){
          given_color = COLOR_PALETTE[copy_event->event->midi_event.data[0]];
          cairo_set_source_rgba(painter, given_color->red, given_color->green, 
                                given_color->blue, given_color->alpha);
        } 
        // If a Note On event is found
        
        if (copy_event->event->midi_event.status >= 0x90 &&
            copy_event->event->midi_event.status <= 0x9F){
          if (copy_event->event->midi_event.data[1] != 0){
            int note = copy_event->event->midi_event.data[0];
            int length = get_delta_len(copy_event, note);
            int note_pos = get_y_pos(height, note_scale, note, low_note);
            draw_line(painter, note_pos, current_time, length);
          }
        }

      }
      current_time = current_time + copy_event->event->delta_time;
      copy_event = copy_event->next_event;
    }
    if (song->format == 1){
      current_time = 0;
    }
    copy_track = copy_track->next_track;
  }
  
}

int get_delta_len(event_node_t *found_event, int note){
  event_node_t *copy_event = found_event;
  int delta_len = 0;
  while (copy_event != NULL){
    delta_len = delta_len + copy_event->event->delta_time;
    if (event_type(copy_event->event) == MIDI_EVENT_T){
      if (copy_event->event->midi_event.status >= 0x80 &&
          copy_event->event->midi_event.status <= 0x8F){
        if (copy_event->event->midi_event.data[0] == note){
          return delta_len;
        }
      }
      if (copy_event->event->midi_event.status >= 0x90 &&
          copy_event->event->midi_event.status <= 0x9F){
        if (copy_event->event->midi_event.data[1] == 0){
          return delta_len;
        }
      }
    }
    copy_event = copy_event->next_event;
  }
  return delta_len;
}

int get_y_pos(int height, int note_scale, int note, int low_note){
  return height - ((note - low_note + 10) * note_scale);
}

/* Define warp_time_cb here */

void warp_time_cb(GtkSpinButton *warp_scale, gpointer user_data){
  g_parameters.change_time = gtk_spin_button_get_value(warp_scale);  
  update_drawing_area();
}

/* Define song_octave_cb here */

void song_octave_cb(GtkSpinButton *octave_scale, gpointer user_data){
  g_parameters.change_octave = gtk_spin_button_get_value_as_int(octave_scale);
  update_drawing_area();
}

/* Define instrument_map_cb here */

void instrument_map_cb(GtkComboBoxText *picked_inst, gpointer user_data){
  char *inst_name = gtk_combo_box_text_get_active_text(picked_inst);
  if (strcmp(inst_name, "Helicopter") == 0){
    g_parameters.instrument = &I_HELICOPTER;
    g_parameters.use_inst = true;
  }
  else if (strcmp(inst_name, "Brass Band") == 0){
    g_parameters.instrument = &I_BRASS_BAND;
    g_parameters.use_inst = true;
  }
  else {
    g_parameters.use_inst = false;
  }
  update_drawing_area();
}

/* Define note_map_cb here */

void note_map_cb(GtkComboBoxText *picked_note, gpointer user_data){
  char *note_name = gtk_combo_box_text_get_active_text(picked_note);
  if (strcmp(note_name, "Lower Notes") == 0){
    g_parameters.notes = &N_LOWER;
    g_parameters.use_notes = true;
  }
  else {
    g_parameters.use_notes = false;
  }
  update_drawing_area();
}

/* Define save_song_cb here */

void save_song_cb(GtkButton *button, gpointer user_data){
  write_song_data(g_modified_song, g_current_song->path);
  char file_name[strlen(g_current_song->path)];
  strcpy(file_name, g_current_song->path);
  remove_song_cb(NULL, NULL);
  tree_node_t *new_node = malloc(sizeof(tree_node_t));
  new_node->song = parse_file(file_name);
  new_node->song_name = get_file_name(new_node->song->path);
  new_node->left_child = NULL;
  new_node->right_child = NULL;
  tree_insert(&g_song_library, new_node);
  update_song_list();
  update_drawing_area();
}

/* Define remove_song_cb here */

void remove_song_cb(GtkButton *button, gpointer user_data){
  remove_song_from_tree(&g_song_library, g_current_node->song_name);
  g_current_node = NULL;
  g_current_song = NULL;
  free_song(g_modified_song);
  g_modified_song = NULL;
  gtk_combo_box_set_active (GTK_COMBO_BOX(g_widgets.remap_notes), 0);
  gtk_combo_box_set_active (GTK_COMBO_BOX(g_widgets.remap_instruments), 0);
  update_song_list();
  update_info();
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
