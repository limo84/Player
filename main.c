#include <gtk/gtk.h>

#include "Recorder/Recorder.h"

static void print_hello(GtkWidget *widget, gpointer data) {
  g_print("Hello World\n");
}

void print_stuff(GtkWidget *widget, gpointer data) {
  fwrite("HELLO", 1, 5, (FILE*)data);
  g_print("stuff\n");
}

void quitApp(GtkWidget *widget, gpointer data) {
  printf("quitting app...\n");
  gtk_main_quit();
}

int main(int argc, char *argv[]) {

  FILE* file = fopen("sound.raw", "wb");

  Recorder *recorder;
  Recorder_Init(recorder);

  GtkBuilder *builder;
  GObject *window;
  GObject *button;
  GError *error = NULL;

  gtk_init(&argc, &argv);

  /* Construct a GtkBuilder instance and load our UI description */
  builder = gtk_builder_new();
  if (gtk_builder_add_from_file(builder, "builder.xml", &error) == 0) {
    g_printerr("Error loading file: %s\n", error->message);
    g_clear_error(&error);
    return 1;
  }

  /* Connect signal handlers to the constructed widgets. */
  window = gtk_builder_get_object(builder, "window");
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  button = gtk_builder_get_object(builder, "button1");
  g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);

  button = gtk_builder_get_object(builder, "button2");
  g_signal_connect(button, "clicked", G_CALLBACK(print_stuff), file);

  button = gtk_builder_get_object(builder, "quit");
  g_signal_connect(button, "clicked", G_CALLBACK(quitApp), NULL);

  gtk_main();

  fclose(file);

  return 0;
}
