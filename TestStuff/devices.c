/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void listdev(char* devname)
{
  char** hints;
  int err;
  char** n;
  char* name;
  char* desc;
  char* ioid;

  /* Enumerate sound devices */
  err = snd_device_name_hint(-1, devname, (void***)&hints);
  if (err != 0) {
    fprintf(stderr, "*** Cannot get device names\n");
    exit(1);
  }

  n = hints;
  while (*n != NULL) {
    name = snd_device_name_get_hint(*n, "NAME");
    desc = snd_device_name_get_hint(*n, "DESC");
    ioid = snd_device_name_get_hint(*n, "IOID");

    printf("Name of device: %s\n", name);
    printf("Description of device: %s\n", desc);
    printf("I/O type of device: %s\n", ioid);
    printf("\n");

    if (name && strcmp("null", name)) free(name);
    if (desc && strcmp("null", desc)) free(desc);
    if (ioid && strcmp("null", ioid)) free(ioid);
    n++;
  }

  // Free hint buffer too
  snd_device_name_free_hint((void**)hints);
}

int main(void) {
  printf("PCM devices:\n");
  printf("\n");
  listdev("pcm");

  printf("MIDI devices:\n");
  printf("\n");
  listdev("rawmidi");

  return 0;
}