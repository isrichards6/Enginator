#include "AudioDB.h"
#include "AudioHelper.h"
#include "Config.h"
#include "PrintUtils.h"

void AudioDB::Init() {
	AudioHelper::Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
	AudioHelper::Mix_AllocateChannels(50);
}

void AudioDB::PlayChannel(float channel, const std::string& audio_clip_name, bool does_loop) {
	std::string base_audio_path = (Config::AUDIO_DIR / (audio_clip_name)).string();
	std::string full_audio_path = "";

	// extension check
	if (std::filesystem::exists(base_audio_path + ".wav")) {
		full_audio_path = base_audio_path + ".wav";
	} 
	else if (std::filesystem::exists(base_audio_path + ".ogg")) {
		full_audio_path = base_audio_path + ".ogg";
	}

	if (full_audio_path.empty()) {
		print("error: failed to play audio clip ", audio_clip_name);
		exit(0);
	}

	auto [it, inserted] = AudioCache.insert({ audio_clip_name, nullptr });
	// cache miss
	if (inserted) {
		it->second = AudioHelper::Mix_LoadWAV(full_audio_path.c_str());
	}

	// -1 = infinite loop, 0 = no loop
	int loop_count = does_loop ? -1 : 0;

	// play audio
	AudioHelper::Mix_PlayChannel(static_cast<int>(channel), it->second, loop_count);
}

int AudioDB::HaltChannel(float channel) {
	return AudioHelper::Mix_HaltChannel(channel);
}

void AudioDB::SetVolume(float channel, float volume) {
	AudioHelper::Mix_Volume(static_cast<int>(channel), glm::min(static_cast<int>(volume), MIX_MAX_VOLUME));
}