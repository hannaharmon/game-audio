#include "audio_session.h"

namespace audio {

AudioSession::AudioSession() : owns_init_(false) {
    auto& manager = AudioManager::GetInstance();
    owns_init_ = manager.Initialize();
}

AudioSession::~AudioSession() {
    Close();
}

void AudioSession::Close() {
    if (!owns_init_) {
        return;
    }

    AudioManager::GetInstance().Shutdown();
    owns_init_ = false;
}

} // namespace audio
