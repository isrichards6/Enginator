#pragma once
#include "SDL_mixer.h"
#include <string>
#include <unordered_map>

#include "Helper.h"

class AudioDB {
public:
	static void Init();
	static void PlayChannel(float channel, const std::string& audio_clip_name, bool does_loop);
	static inline std::unordered_map<std::string, Mix_Chunk*> AudioCache;
	
	static int HaltChannel(float channel);
	static void SetVolume(float channel, float volume);
	static int GetChannel() {
		// cycle through channels 2-49
		return (Helper::GetFrameNumber() % 48) + 2;
	}

private:
	static inline std::unordered_map<std::string, Mix_Chunk*> loaded_audio;
	
};

