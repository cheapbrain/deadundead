#include "audio.h"

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

char *file = "../audio/test.ogg";

void DisplayALError(char *str, ALenum error){
	FILE *file;
	file = fopen("alError.txt", "w");
	fprintf(file, "%s %i", str, error);
	fclose(file);
}

void test_audio() {

#define chunk 65536

stb_vorbis* stream;
stb_vorbis_info info;
short *data;
unsigned int samples;

stream = stb_vorbis_open_filename(file, NULL, NULL);
info = stb_vorbis_get_info(stream);

samples = stb_vorbis_stream_length_in_samples(stream) * info.channels;

data = (short *) malloc(chunk*sizeof(short));

ALuint source, amount, which;
ALint state, processed;
ALCdevice *device;
ALCcontext *context;
ALuint buffer[1];

device = alcOpenDevice(NULL);
if (device) {
    context = alcCreateContext(device, NULL);
    alcMakeContextCurrent(context);
}

//printf("Using '%s' device.\n", alcGetString(device, ALC_DEVICE_SPECIFIER));

alGetError();
alGenBuffers(1, buffer);

stb_vorbis_get_samples_short_interleaved(stream, info.channels, data, chunk);
alBufferData(buffer[0], AL_FORMAT_STEREO16, data, chunk*sizeof(short), info.sample_rate);

alGenSources(1, &source);

alSourceQueueBuffers(source, 1, buffer);
alSourcePlay(source);

do {
    alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

    if (processed) {
        alSourceUnqueueBuffers(source, 1, &which);

        amount = stb_vorbis_get_samples_short_interleaved(stream, info.channels, data, chunk);
        
        if (amount > 0) {
            alBufferData(which, AL_FORMAT_MONO16, data, amount*2*sizeof(short), info.sample_rate/2);
            alSourceQueueBuffers(source, 1, &which);
		}
	}

    Sleep(10);

    alGetSourcei(source, AL_SOURCE_STATE, &state);

} while (state == AL_PLAYING);

stb_vorbis_close(stream);
free(data);

alDeleteSources(1, &source);
alDeleteBuffers(2, buffer);
alcMakeContextCurrent(NULL);
alcDestroyContext(context);
alcCloseDevice(device);
}