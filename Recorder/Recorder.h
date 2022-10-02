#ifndef RECORDER_H
#define RECORDER_H

#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>

typedef struct {
  snd_pcm_t *handle;
  snd_pcm_uframes_t frames;
  snd_pcm_hw_params_t *params;
  snd_pcm_format_t format;

  FILE *file;

  unsigned int bitrate;
  char *buffer;
  int buffer_frames;

  long loops;
  int rc;
  int size;
  int dir;

} Recorder;

void Recorder_Init(Recorder *recorder);

void Recorder_Record(Recorder *recorder, FILE* file);

#endif