#include "audio.h"

std::map<std::string, Audio*> Audio::sAudiosLoaded;

Audio::Audio()
{
	sample = 0;
}

Audio::~Audio()
{
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


bool Audio::Stop(HCHANNEL channel) {
	return BASS_ChannelStop(channel);
}
