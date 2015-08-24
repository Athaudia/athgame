#include "audio_region.h"
#include <stdlib.h>

struct ag_audio_region* ag_audio_region_new(int sample_count, int channel_count)
{
	struct ag_audio_region* audio_region = (struct ag_audio_region*)malloc(sizeof(struct ag_audio_region));
	audio_region->sample_count = sample_count;
	audio_region->channel_count = channel_count;
	audio_region->samples = (ag_audio_sample*)malloc(sizeof(ag_audio_sample)*sample_count*channel_count);
	return audio_region;
	
}

void ag_audio_region_destroy(struct ag_audio_region* audio_region)
{
	free(audio_region->samples);
	free(audio_region);
}
