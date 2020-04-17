#include<gtk/gtk.h>

int main(int argc, char *argv[]){
  gtk_init(&argc, &argv);
  GtkBuilder *builder = gtk_builder_new_from_file("ui.glade");
  GtkWidget *window = GTK_WIDGET (gtk_builder_get_object(builder, "app_window"));
  gtk_widget_show(window);
  gtk_main();
  return 0;
}
