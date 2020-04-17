#include<gtk/gtk.h>

struct ui_widgets {
  GtkBuilder *builder;
  GtkWidget *window;
} g_widgets;

int main(int argc, char *argv[]){
  gtk_init(&argc, &argv);
  g_widgets.builder = gtk_builder_new_from_file("ui.glade");
  g_widgets.window = GTK_WIDGET(gtk_builder_get_object(g_widgets.builder, "app_window"));
  
  // gtk_window_set_application(GTK_WINDOW(g_widgets.window), app);
  g_signal_connect(g_widgets.window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
  
  gtk_builder_connect_signals(g_widgets.builder, NULL);

  gtk_widget_show_all(g_widgets.window);
  gtk_main();
}
