/*

This example reads from the default PCM device
and writes to standard output for 5 seconds of data.

*/

/* Use the newer ALSA API */
#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>

typedef struct {
  snd_pcm_t *handle;
  snd_pcm_uframes_t frames;
  snd_pcm_hw_params_t *params;
  snd_pcm_format_t format;

  unsigned int bitrate;
  char *buffer;
  int buffer_frames;

  long loops;
  int rc;
  int size;
  int dir;

} Recorder;

void Recorder_Init(Recorder *recorder) {
  int err;
  recorder->bitrate = 44100;
  recorder->format = SND_PCM_FORMAT_S16_LE;
  recorder->buffer_frames = 128;

  char *device = "default";
  // char *device = "dsnoop:CARD=U0x46d0x9a1,DEV=0";

  /* Open PCM device for recording (capture). */
  if ((err = snd_pcm_open(&(recorder->handle), device, SND_PCM_STREAM_CAPTURE, 0)) < 0) {
    fprintf(stderr, "cannot open audio device %s (%s)\n",
            "default",
            snd_strerror(err));
    exit(1);
  }

  printf("audio interface opened\n");

  if ((err = snd_pcm_hw_params_malloc(&(recorder->params))) < 0) {
    fprintf(stderr, "cannot allocate hardware parameter structure (%s)\n",
            snd_strerror(err));
    exit(1);
  }

  printf("hw_params allocated\n");

  if ((err = snd_pcm_hw_params_any(recorder->handle, recorder->params)) < 0) {
    fprintf(stderr, "cannot initialize hardware parameter structure (%s)\n",
            snd_strerror(err));
    exit(1);
  }

  printf("hw_params initialized\n");

  if ((err = snd_pcm_hw_params_set_access(recorder->handle, recorder->params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
    fprintf(stderr, "cannot set access type (%s)\n",
            snd_strerror(err));
    exit(1);
  }

  printf("hw_params access setted\n");

  if ((err = snd_pcm_hw_params_set_format(recorder->handle, recorder->params, recorder->format)) < 0) {
    fprintf(stderr, "cannot set sample format (%s)\n",
            snd_strerror(err));
    exit(1);
  }

  printf("hw_params format setted\n");

  if ((err = snd_pcm_hw_params_set_channels(recorder->handle, recorder->params, 2)) < 0) {
    fprintf(stderr, "cannot set channel count (%s)\n",
            snd_strerror(err));
    exit(1);
  }

  printf("hw_params channels setted\n");

  /* 44100 bits/second sampling rate (CD quality) */
  if ((err = snd_pcm_hw_params_set_rate_near(recorder->handle, recorder->params, &(recorder->bitrate), &(recorder->dir))) < 0) {
    fprintf(stderr, "cannot set sample rate (%s)\n",
            snd_strerror(err));
    exit(1);
  }

  printf("hw_params rate setted\n");

  /* Set period size to 32 frames. */
  recorder->frames = 32;
  snd_pcm_hw_params_set_period_size_near(recorder->handle, recorder->params, &(recorder->frames), &(recorder->dir));

  /* Write the parameters to the driver */
  if ((err = snd_pcm_hw_params(recorder->handle, recorder->params)) < 0) {
    fprintf(stderr, "cannot set parameters (%s)\n",
            snd_strerror(err));
    exit(1);
  }

  printf("hw_params setted\n");

  /* Use a buffer large enough to hold one period */
  snd_pcm_hw_params_get_period_size(recorder->params, &(recorder->frames), &(recorder->dir));
  recorder->size = recorder->frames * 4; /* 2 bytes/sample, 2 channels */
  recorder->buffer = (char *)malloc(recorder->size);

  /* We want to loop for 5 seconds */
  snd_pcm_hw_params_get_period_time(recorder->params, &(recorder->bitrate), &(recorder->dir));
  recorder->loops = 5000000 / recorder->bitrate;

}

void Recorder_Record(Recorder *recorder, FILE* file) {
  while (recorder->loops > 0) {
    recorder->loops--;
    recorder->rc = snd_pcm_readi(recorder->handle, recorder->buffer, recorder->frames);
    if (recorder->rc == -EPIPE) {
      /* EPIPE means overrun */
      fprintf(stderr, "overrun occurred\n");
      snd_pcm_prepare(recorder->handle);
    } else if (recorder->rc < 0) {
      fprintf(stderr, "error from read: %s\n", snd_strerror(recorder->rc));
    } else if (recorder->rc != (int)(recorder->frames)) {
      fprintf(stderr, "short read, read %d frames\n", recorder->rc);
    }
    // recorder->rc = write(1, recorder->buffer, recorder->size);
    // recorder->rc = fwrite(recorder->buffer, recorder->size, 1, file);
    recorder->rc = fwrite(recorder->buffer, 1, recorder->size, file);
    if (recorder->rc != recorder->size)
      fprintf(stderr, "short write: wrote %d bytes\n", recorder->rc);
  }

  snd_pcm_drain(recorder->handle);
  snd_pcm_close(recorder->handle);
}

int main() {
  FILE *file = fopen("test.bin", "wb");

  // char* test = "HALLO WELT";
  // fwrite(test, 10, 1, file);

  Recorder *recorder = malloc(sizeof(Recorder));



  Recorder_Init(recorder);

  // printf("buffer->size: %i\n", recorder->size);

  Recorder_Record(recorder, file);

  // TODO
  // - write to file from code
  // - test with player
  // - add player here
  // - test different input devices
  // - ...
  // - profit

  fclose(file);
  
  free(recorder->buffer);

  return 0;
}