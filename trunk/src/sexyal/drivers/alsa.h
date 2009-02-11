void SexyALI_ALSA_Enumerate(int (*func)(uint8_t *name, char *id, void *udata), void *udata);
SexyAL_device *SexyALI_ALSA_Open(const char *id, SexyAL_format *format, SexyAL_buffering *buffering);

