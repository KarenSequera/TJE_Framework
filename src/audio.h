#pragma once

#include "extra/bass.h"
#include <map>
#include <string>

class Audio {
public:

	// Handler for BASS_SampleLoad
	HSAMPLE sample;

	Audio();	// Sample = 0 here
	~Audio(); 	// Free sample (BASS_SampleFree)

	// Manager stuff
	// The map
	static std::map<std::string, Audio*>
		sAudiosLoaded;

	// Initialize BASS
	// This function needs to be called before using any other. 
	static bool Init();

	// Get from manager map
	static Audio* Get(const char* filename);
	// Play Manager API
	static HCHANNEL Play(const char* filename, float volume, bool loop);
	//static HCHANNEL Play3D(const char* filename, Vector3 position);
	// Stop sounds
	static bool Stop(HCHANNEL channel);
};



