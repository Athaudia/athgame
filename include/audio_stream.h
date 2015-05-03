#ifndef GUARD_AG_AUDIO_STREAM_H
#define GUARD_AG_AUDIO_STREAM_H

#include "audio_region.h"
//audio_region -> audio_frame -> audio_sample

struct ag_audio_stream
{
	int buffer_count;
	int buffer_size;
	struct ag_audio_region* buffers;
};

struct ag_audio_stream* ag_audio_stream_new();

#endif
