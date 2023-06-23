#include "audio.h"

std::map<std::string, Audio*> Audio::sAudiosLoaded;
sDelayedAudio Audio::d_sounds[MAX_DELAYED_SOUNDS];
int Audio::num_delayed;

Audio::Audio()
{
	sample = 0;
}

Audio::~Audio()
{
	num_delayed = 0;

	sDelayedAudio delayed_audio;
	delayed_audio.delay = -9999.f;
	delayed_audio.volume = 1.f;
	delayed_audio.filename = "";

	for (int i = 0; i < MAX_DELAYED_SOUNDS; i++) {
		d_sounds[i] = delayed_audio;
	}

	BASS_SampleFree(sample);
}

bool Audio::Init()
{	//Init the bass library
	return BASS_Init(-1, 44100, 0, 0, NULL);
}

Audio* Audio::Get(const char* filename)
{
	// Check if the audio is already loaded in the map
	auto iterator = sAudiosLoaded.find(filename);

	
	if (iterator != sAudiosLoaded.end())
	{
		// Audio found, return the corresponding Audio* pointer (second element of the iterator)
		return iterator->second;
	}

	// If the audio is not found, we load it
	Audio* newAudio = new Audio();
	newAudio->sample = BASS_SampleLoad(false, filename, 0, 0, 3, 0);

	// Add the new audio to the map
	sAudiosLoaded[filename] = newAudio;

	return newAudio;
	
}

HCHANNEL Audio::Play(const char* filename, float volume, bool loop)
{
	Audio* audio = Get(filename);
	
	// Get the channel handler for our sample. 
	HCHANNEL hSampleChannel = BASS_SampleGetChannel(audio->sample, false);

	// Set the volume for the channel
	BASS_ChannelSetAttribute(hSampleChannel, BASS_ATTRIB_VOL, volume);

	DWORD flags = loop ? BASS_SAMPLE_LOOP : 0;
	BASS_ChannelFlags(hSampleChannel, flags, BASS_SAMPLE_LOOP);


	//Play the sample 
	BASS_ChannelPlay(hSampleChannel, true);

	return hSampleChannel;
}


void Audio::PlayDelayed(const char* filename, float volume, float delay) {
	if (num_delayed == MAX_DELAYED_SOUNDS)
		return;

	sDelayedAudio delayed_audio;
	delayed_audio.delay = delay;
	delayed_audio.filename = filename;
	delayed_audio.volume = volume;


	for (int i = 0; i < MAX_DELAYED_SOUNDS; i++) {
		if (d_sounds[i].delay <= -1.f)
			d_sounds[i] = delayed_audio;
	}
}

bool Audio::Stop(HCHANNEL channel) {
	return BASS_ChannelStop(channel);
}
