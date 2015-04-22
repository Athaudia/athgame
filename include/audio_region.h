#ifndef GUARD_AUDIO_REGION_H
#define GUARD_AUDIO_REGION_H

typedef float ag_audio_sample;

struct ag_audio_region
{
	int channel_count;
	int sample_count;
	ag_audio_sample* samples;
};

struct ag_audio_region* ag_audio_region_new(int sample_count, int channel_count);
void ag_audio_region_destroy(struct ag_audio_region* audio_region);

#endif
